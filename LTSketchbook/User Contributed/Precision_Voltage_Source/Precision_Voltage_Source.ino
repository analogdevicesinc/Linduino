/*
Linduino Precision Voltage Source

This project implaments the LTC2756 18-bit dac to create a high accuracy voltage source.

Output range is -5v to + 5v.

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
#include "UserInterface.h"
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "QuikEval_EEPROM.h"
#include "LTC2756.h"
#include <Wire.h>
#include <SPI.h>
#include <LiquidCrystal.h>

// LCD constants
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

int8_t demo_board_connected;                    // Demo Board Name stored in QuikEval EEPROM

// Global Variables
float LTC2756_lsb = 3.8146972656e-5;            // Typical lsb for -5V to +5v bipolar mode only
int32_t LTC2756_offset = 0x20000;               // Typical offset for -5V to +5v bipolar mode only
int16_t pos;                                    // Digit position variable
int8_t volts[5] = {0, 0, 0, 0, 0};              // Array that holds the desired volts
int8_t hex[5] = {0x00, 0x00, 0x00, 0x00,0x02};  // Array that holds the desired DAC code
uint8_t HEX_DEC;                                // Holds the the mode (HEX code or Voltage)
bool sign = 0;                                  // Holds the current sign for decimal voltage

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);

void setup()
{
  char demo_name[] = "DC1792";    // Demo Board Name stored in QuikEval EEPROM
  quikeval_SPI_init();            // Configure the spi port for 4volts SCK
  quikeval_I2C_init();            // Configure the EEPROM I2C port for 100kHz
  quikeval_SPI_connect();         // Connect SPI to main data port

  demo_board_connected = discover_demo_board(demo_name);  //! Checks if correct demo board is connected.

  lcd.begin(16, 2);               // Initialize LCD

  // Display Message
  lcd.print("Voltage Source");
  delay(2000);

  if (!demo_board_connected)      // Checks for the correct demo board
  {
    lcd.clear();
    lcd.print("No Demo Board");
    lcd.setCursor(0, 1);
    lcd.print("  Detected");
    while (1);                  // Does nothing if the demo board is not connected
  }

  init_dac();
  if ( (read_switches() == SELECT))   // If SELECT button is held on startup, Menu is in HEX mode

  {
    HEX_DEC = 1;
    pos = 4;
    lcd.setCursor(0, 1);
    lcd.print("HEX Mode");
    delay(2000);
  }
  else
  {
    HEX_DEC = 0;
    pos = 4;

  }
  lcd.clear();
}

void loop()
{

  if (HEX_DEC)
    display_hex();  // Hex MODE
  else
    display_decimal();  // Decimal mode
}

// Reads the Switches
// Returns the selected button
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

// Initializes the DAC to 0V in bipolar mode
void init_dac()
{
  uint8_t tx[4],rx[4];

  // Set DAC to -5v to +5v
  tx[3] = LTC2756_WRITE_SPAN;
  tx[2] = 0x00;
  tx[1] = LTC2756_BIPOLAR_N5_P5;
  tx[0] = 0x00;

  LTC2756_transfer_4bytes(LTC2756_CS, tx, rx);

  // Set DAC Voltage to 0V
  tx[3] = LTC2756_WRITE_CODE;
  tx[2] = 0x80;
  tx[1] = 0x00;
  tx[0] = 0x00;

  LTC2756_transfer_4bytes(LTC2756_CS, tx, rx);

  // Update DAC
  tx[3] = LTC2756_UPDATE;
  tx[2] = 0x00;
  tx[1] = 0x00;
  tx[0] = 0x00;

  LTC2756_transfer_4bytes(LTC2756_CS, tx, rx);
}

// Converts decimal to voltage
void set_voltage()
{
  float voltage=0;

  //Convert the Array to a float
  voltage += volts[0]*.0001;
  voltage += volts[1]*.001;
  voltage += volts[2]*.01;
  voltage += volts[3]*.1;
  voltage += volts[4];
  if (sign)   // adjusts for the sign
    voltage *= -1;

  // Convert voltage to code
  uint32_t code;
  code = LTC2756_voltage_to_code(voltage, LTC2756_lsb, LTC2756_offset);

  uint8_t tx[4],rx[4];
  union
  {
    uint8_t byte[4];
    uint32_t code;
  } data;

  // Load tx array with write command and code
  data.code = code<<6;
  tx[3] = LTC2756_WRITE_CODE;
  tx[2] = data.byte[2];
  tx[1] = data.byte[1];
  tx[0] = data.byte[0];

  LTC2756_transfer_4bytes(LTC2756_CS, tx, rx);

  // Load tx arra with update DAC command
  tx[3] = LTC2756_UPDATE;
  tx[2] = 0x00;
  tx[1] = 0x00;
  tx[0] = 0x00;

  LTC2756_transfer_4bytes(LTC2756_CS, tx, rx);
}
void display_decimal()
{
  int8_t in;

  in = read_switches();

  // Display Name
  lcd.setCursor(0, 0);
  lcd.print("Voltage Source");

  // Display voltage
  lcd.setCursor(0, 1);
  lcd.print("V:");
  if (sign)
    lcd.print("-");
  else
    lcd.print("+");
  lcd.print(volts[4]);
  lcd.print(".");
  lcd.print(volts[3]);
  lcd.print(volts[2]);
  lcd.print(volts[1]);
  lcd.print(volts[0]);

  // Controls position
  if (in == RIGHT)
    pos -= 1;
  if (in == LEFT)
    pos += 1;
  if (pos > 5)
    pos = 0;
  if (pos < 0)
    pos = 5;

  // Sets cursor to correct location
  lcd.cursor();
  if (pos == 0)
    lcd.setCursor(8, 1);
  if (pos == 1)
    lcd.setCursor(7, 1);
  if (pos == 2)
    lcd.setCursor(6, 1);
  if (pos == 3)
    lcd.setCursor(5, 1);
  if (pos == 4)
    lcd.setCursor(3, 1);
  if (pos == 5)
    lcd.setCursor(2, 1);

  // V_Out Array Control
  if (in == UP)
  {
    if (pos == 5)
      sign -= 1;
    else
      volts[pos] += 1;
    lcd.noCursor();
    lcd.clear();
  }
  if (in == DOWN)
  {
    if (pos == 5)
      sign -= 1;
    else
      volts[pos] -= 1;
    lcd.noCursor();
    lcd.clear();
  }
  if (pos == 4)
  {
    if (volts[pos] < 0)
      volts[pos] = 5;
    if (volts[pos] > 5)
      volts[pos] = 0;
  }
  else
  {
    if (volts[pos] < 0)
      volts[pos] = 9;
    if (volts[pos] > 9)
      volts[pos] = 0;
  }

  //Sets the DAC when select is pressed
  if (in == SELECT)
  {
    set_voltage();
    lcd.clear();
  }
  delay(150);

}
void display_hex()
{
  int8_t in;

  in = read_switches();

  // Display Name
  lcd.setCursor(0, 0);
  lcd.print("Voltage Source");

  // Display HEX code
  lcd.setCursor(0, 1);
  lcd.print("DAC code:0x");
  lcd.print(hex[4], HEX);
  lcd.print(hex[3], HEX);
  lcd.print(hex[2], HEX);
  lcd.print(hex[1], HEX);
  lcd.print(hex[0], HEX);

  // Controls position
  if (in == RIGHT)
    pos -= 1;
  if (in == LEFT)
    pos += 1;
  if (pos > 4)
    pos = 0;
  if (pos < 0)
    pos = 4;

  // Sets cursor to correct location
  lcd.cursor();
  if (pos == 0)
    lcd.setCursor(15, 1);
  if (pos == 1)
    lcd.setCursor(14, 1);
  if (pos == 2)
    lcd.setCursor(13, 1);
  if (pos == 3)
    lcd.setCursor(12, 1);
  if (pos == 4)
    lcd.setCursor(11, 1);


  // Array Control
  if (in == UP)
  {
    hex[pos] += 1;
    lcd.noCursor();
    lcd.clear();
  }
  if (in == DOWN)
  {
    hex[pos] -= 1;
    lcd.noCursor();
    lcd.clear();
  }

  if (pos == 4)
  {
    if (hex[pos] < 0x00)
      hex[pos] = 0x03;
    if (hex[pos] > 0x03)
      hex[pos] = 0x00;
  }
  else
  {
    if (hex[pos] < 0x00)
      hex[pos] = 0x0F;
    if (hex[pos] > 0x0F)
      hex[pos] = 0x00;
  }

  // Sets the DAC when select is pressed
  if (in == SELECT)
  {
    set_voltage_hex();
    lcd.clear();
  }
  delay(150);
}

// Sets the DAC with the HEX Code
void set_voltage_hex()
{

  uint32_t code;

  uint8_t tx[4],rx[4];
  union
  {
    uint8_t byte[4];
    uint32_t code;
  } data;

  // Combines the array
  code += (uint32_t)hex[0];
  code += ((uint32_t)hex[1])<<4;
  code += ((uint32_t)hex[2])<<8;
  code += ((uint32_t)hex[3])<<12;
  code += ((uint32_t)hex[4])<<16;

  // Load the tx array with the write command and HEX code
  data.code = code<<6;
  tx[3] = LTC2756_WRITE_CODE;
  tx[2] = data.byte[2];
  tx[1] = data.byte[1];
  tx[0] = data.byte[0];

  LTC2756_transfer_4bytes(LTC2756_CS, tx, rx);

  // Loads the tx array with the update DAC command
  tx[3] = LTC2756_UPDATE;
  tx[2] = 0x00;
  tx[1] = 0x00;
  tx[0] = 0x00;

  LTC2756_transfer_4bytes(LTC2756_CS, tx, rx);
}