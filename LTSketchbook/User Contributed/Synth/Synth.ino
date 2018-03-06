/*
LTC Synthesizer Demo

Copyright 2018(c) Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
 - Neither the name of Analog Devices, Inc. nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.
 - The use of this software may or may not infringe the patent rights
   of one or more patent holders.  This license does not release you
   from the requirement that you obtain separate licenses from these
   patent holders to use this software.
 - Use of the software either in source or binary form, must be run
   on or directly connected to an Analog Devices Inc. component.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC6946.h"
#include "math.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>

const int16_t RS = 8;
const int16_t E = 9;
const int16_t D4 = 4;
const int16_t D5 = 5;
const int16_t D6 = 6;
const int16_t D7 = 7;
const int16_t BUTTON_PIN = 0;
const int8_t UP = 1;
const int8_t DOWN = 2;
const int8_t LEFT = 3;
const int8_t RIGHT = 4;
const int8_t SELECT = 5;

uint8_t REG[12];                //!< Register values to be written to read from
int8_t demo_board_connected;    //!< Demo Board Name stored in QuikEval EEPROM
char demo_name[] = "DC1705";    //!< Demo Board Name stored in QuikEval EEPROM

int16_t pos = 5;
int8_t MHz[6] = {0, 0, 0, 0, 0, 1};

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

void setup()
{
  quikeval_SPI_init();      //! Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();   //! Connect SPI to main data port
  quikeval_I2C_init();      //! Configure the EEPROM I2C port for 100kHz

  demo_board_connected = discover_demo_board(demo_name);  //! Checks if correct demo board is connected.

  lcd.begin(16, 2);

  lcd.print("LTC Synthesizer");
  lcd.setCursor(0, 1);
  lcd.print("V 1.0");
  delay(1000);
  lcd.clear();

  if (!demo_board_connected)
  {
    lcd.print("No Demo Board");
    lcd.setCursor(0, 1);
    lcd.print("  Detected");
    while (1);                  //! Does nothing if the demo board is not connected
  }

  lcd.print("Found The");
  lcd.setCursor(0, 1);
  lcd.print(demo_board.product_name);
  delay(1000);
  lcd.clear();

  restore_settings();
  lcd.clear();
}

void loop()
{
  int8_t in;

  in = read_switches();

  //Display Frequency
  lcd.setCursor(0, 0);
  lcd.print("Freq:");
  lcd.print(MHz[5]);
  lcd.print(MHz[4]);
  lcd.print(MHz[3]);
  lcd.print(MHz[2]);
  lcd.print(".");
  lcd.print(MHz[1]);
  lcd.print(MHz[0]);
  lcd.print("MHz");

  //Controls position
  if (in == RIGHT)
    pos -= 1;
  if (in == LEFT)
    pos += 1;
  if (pos > 5)
    pos = 0;
  if (pos < 0)
    pos = 5;

  //Sets cursor to correct location
  lcd.cursor();
  if (pos == 0)
    lcd.setCursor(11, 0);
  if (pos == 1)
    lcd.setCursor(10, 0);
  if (pos == 2)
    lcd.setCursor(8, 0);
  if (pos == 3)
    lcd.setCursor(7, 0);
  if (pos == 4)
    lcd.setCursor(6, 0);
  if (pos == 5)
    lcd.setCursor(5, 0);

  // Frequency Out Array Control
  if (in == UP)
  {
    MHz[pos] += 1;
    lcd.noCursor();
    lcd.clear();
  }
  if (in == DOWN)
  {
    MHz[pos] -= 1;
    lcd.noCursor();
    lcd.clear();
  }
  if (MHz[pos] < 0)
    MHz[pos] = 9;
  if (MHz[pos] > 9)
    MHz[pos] = 0;

  //Designes the loop when select is pressed
  if (in == SELECT)
  {
    loop_design();
    lcd.clear();
  }
  delay(250);
}

//Reads the Switches
//Returns the selected button
int8_t read_switches()
{
  int16_t input = 0;

  input = analogRead(BUTTON_PIN);
  if ((input <= 170) && (input >= 120))
    return UP;
  if ((input <= 350) && (input >= 300))
    return DOWN;
  if ((input <= 550) && (input >= 450))
    return LEFT;
  if ((input <= 800) && (input >= 700))
    return SELECT;
  if (input <= 100)
    return RIGHT;
  return 0;
}

void loop_design()
{
  uint8_t O_DIV;
  uint8_t filt;
  uint8_t LKWIN;
  uint8_t B_DIV;
  uint8_t BST;
  float R_DIV;
  float f_PFD;
  float N_DIV;
  float f_VCO;
  uint8_t O_DIV_possible = 0;
  uint8_t ref_out = 0;            //!< Used to keep track of reference out status

  //Change these Variables to design the loop
  //to different configurations
  uint8_t rfo = 3;
  uint8_t i_cp = 11;
  uint8_t lkcnt = 0;
  float f_ref = 100;  //MHz
  float f_STEP = .5;  //MHz
  float f_rf= 0;      //Desired frequency
  float dBm = 21;

  //Convert the Array to a float
  f_rf += MHz[0]*.01;
  f_rf += MHz[1]*.1;
  f_rf += MHz[2];
  f_rf += MHz[3]*10;
  f_rf += MHz[4]*100;
  f_rf += MHz[5]*1000;


  //Selcts the O divder for different parts
  if (strcmp(demo_board.product_name, "LTC6946-1") == 0)
    O_DIV_possible = O_divide_1(f_rf, &O_DIV);
  else if (strcmp(demo_board.product_name, "LTC6946-2") == 0)
    O_DIV_possible = O_divide_2(f_rf, &O_DIV);
  else if (strcmp(demo_board.product_name, "LTC6946-3") == 0)
    O_DIV_possible = O_divide_3(f_rf, &O_DIV);

  //If a solution is not found, then it does not design the loop
  if (!O_DIV_possible)
  {
    lcd.setCursor(0, 1);
    lcd.print("No Loop Solution");
    delay(1000);
    return;
  }

  //Calculates the R Divider
  R_DIV = R_divide(f_ref, &f_STEP, O_DIV);

  //Calculates the PFD
  f_PFD = f_pfd(f_ref, R_DIV);

  //Calculates the N divider
  N_DIV = N_divide(f_rf, O_DIV, f_PFD);

  //Calculates the VCO
  f_VCO = f_vco(f_ref, N_DIV, R_DIV);

  //Calculates the B divider
  B_DIV = B_div(f_PFD);

  //Selects the filter
  filt = filter(f_ref);

  //Calculates the LKWIN
  LKWIN = lkwin(f_PFD);

  //Calculates the BST
  BST = bst(dBm);

  //Sets the RFO
  if (rfo == 0)
    rfo = LTC6946_RFO_0;
  else if (rfo == 1)
    rfo = LTC6946_RFO_1;
  else if (rfo == 2)
    rfo = LTC6946_RFO_2;
  else
    rfo = LTC6946_RFO_3;

  //Sets the I_CP
  switch (i_cp)
  {
    case 0:
      i_cp = LTC6946_CP_0;
      break;
    case 1:
      i_cp = LTC6946_CP_1;
      break;
    case 2:
      i_cp = LTC6946_CP_2;
      break;
    case 3:
      i_cp = LTC6946_CP_3;
      break;
    case 4:
      i_cp = LTC6946_CP_4;
      break;
    case 5:
      i_cp = LTC6946_CP_5;
      break;
    case 6:
      i_cp = LTC6946_CP_6;
      break;
    case 7:
      i_cp = LTC6946_CP_7;
      break;
    case 8:
      i_cp = LTC6946_CP_8;
      break;
    case 9:
      i_cp = LTC6946_CP_9;
      break;
    case 10:
      i_cp = LTC6946_CP_10;
      break;
    default:
      i_cp = LTC6946_CP_11;
      break;
  }

  //Sets the lock count
  if (lkcnt == 3)
    lkcnt = LTC6946_LKCNT_3;
  else if (lkcnt == 2)
    lkcnt = LTC6946_LKCNT_2;
  else if (lkcnt == 1)
    lkcnt = LTC6946_LKCNT_1;
  else
    lkcnt = LTC6946_LKCNT_0;

  //Places loop design into correct registers
  union
  {
    uint8_t R[2];
    uint16_t code;
  };
  code = R_DIV;
  if (ref_out == 0)
    REG[2] = LTC6946_PDREFO | LTC6946_MTCAL;
  else
    REG[2] = LTC6946_MTCAL;
  REG[3] = B_DIV |R[1];
  REG[4] = R[0];

  union
  {
    uint8_t N[2];
    uint16_t Code;
  };
  Code = N_DIV;
  REG[5] = N[1];
  REG[6] = N[0];
  REG[7] = LTC6946_ALCCAL | LTC6946_ALCULOK | LTC6946_LKEN | LTC6946_CAL; // Required settings to allow proper loop locking
  REG[8] = BST | filt | O_DIV | rfo;
  REG[9] = i_cp | LKWIN | lkcnt;
  REG[10] = LTC6946_CPCHI | LTC6946_CPCLO; // Sets charge pump

  //Set registers
  write_all();

  lcd.clear();
  lcd.print("Loop Set");
  delay(1000);

  store_settings();
}
void store_settings()
// Store the PLL Settings to the EEPROM
{
  eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, EEPROM_CAL_STATUS_ADDRESS);         // Cal key
  for (uint8_t i = 2; i <= 10 ; i++)
    eeprom_write_byte(EEPROM_I2C_ADDRESS,(char) REG[i], EEPROM_CAL_STATUS_ADDRESS + i);
  lcd.clear();
  lcd.print("Settings Stored");
  delay(1000);
}
//! Read stored PLL settings from nonvolatile EEPROM on demo board
void restore_settings()
// Read the PLL settings from EEPROM
{
  int16_t cal_key;
  // read the cal key from the EEPROM
  eeprom_read_int16(EEPROM_I2C_ADDRESS, &cal_key, EEPROM_CAL_STATUS_ADDRESS);
  if (cal_key == EEPROM_CAL_KEY)
  {
    // PLL Settings has been stored, read PLL Settings
    for (uint8_t i = 2; i <= 10 ; i++)
      eeprom_read_byte(EEPROM_I2C_ADDRESS,(char *) &REG[i], EEPROM_CAL_STATUS_ADDRESS + i);

    //Update registers
    write_all();

    lcd.clear();
    lcd.print("Settings");
    lcd.setCursor(0,1);
    lcd.print("Restored");
    delay(1000);
  }
  else
  {
    lcd.clear();
    lcd.print("No Settings");
    lcd.setCursor(0,1);
    lcd.print("  Found");
    delay(1000);
  }
}

//! Write all registers for the LTC6946 and displays them.
void write_all()
{
  // Write All Registers
  LTC6946_write(LTC6946_CS, LTC6946_REG_H01, REG[1]);
  LTC6946_write(LTC6946_CS, LTC6946_REG_H02, REG[2]);
  LTC6946_write(LTC6946_CS, LTC6946_REG_H03, REG[3]);
  LTC6946_write(LTC6946_CS, LTC6946_REG_H04, REG[4]);
  LTC6946_write(LTC6946_CS, LTC6946_REG_H05, REG[5]);
  LTC6946_write(LTC6946_CS, LTC6946_REG_H06, REG[6]);
  LTC6946_write(LTC6946_CS, LTC6946_REG_H07, REG[7]);
  LTC6946_write(LTC6946_CS, LTC6946_REG_H08, REG[8]);
  LTC6946_write(LTC6946_CS, LTC6946_REG_H09, REG[9]);
  LTC6946_write(LTC6946_CS, LTC6946_REG_H0A, REG[10]);
}
