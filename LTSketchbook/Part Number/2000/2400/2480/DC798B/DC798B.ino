/*!
Linear Technology DC798B Demonstration Board.
LTC2480: 16-Bit Delta-Sigma ADC with Easy Drive Input Current Cancellation.

@verbatim

The LTC2480 is a 16-Bit Delta-Sigma ADC with Easy Drive Input Current Cancellation.
It includes on-chip programmable gain and an oscillator. The LTC2480 can be
configured to provide a programmable gain from 1 to 256 in 8 steps, measure an
external signal or internal temperature sensor and reject line frequencies. 50Hz,
60Hz or simultaneous 50Hz/60Hz line frequency rejection can be selected as well
as a 2x speed-up mode.

The voltage on the input pins can have any value between GND – 0.3V and VCC + 0.3V.
Within these limits the converter bipolar input range (VIN = IN+ – IN–) extends
from –0.5 • VREF/GAIN to 0.5 • VREF/GAIN. Outside this input range the converter
produces unique overrange and underrange output codes.

@endverbatim

http://www.linear.com/product/LTC2480

http://www.linear.com/product/LTC2480#demoboards


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

/*! @file
    @ingroup LTC2480
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC24XX_general.h"
#include "LTC2480.h"
#include <SPI.h>

// Function Declaration
void print_title();                             // Print the title block
void print_prompt();                            // Prompt the user for an input command

void menu_1_read_input();
void menu_2_select_gain();
void menu_3_select_rejection();
void menu_4_reference_voltage();

static uint8_t demo_board_connected;            //!< Set to 1 if the board is connected

// Global variables
float LTC2480_vref = 5;
float LTC2480_range = LTC2480_vref/2;
uint8_t LTC2480_gain_selection = LTC2480_GAIN_1;
uint8_t LTC2480_gain = 1;
uint8_t LTC2480_rejection_mode = 0;       // Simultaneous 50Hz/60Hz Rejection

//! Initialize Linduino
void setup()
{
  char demo_name[]="DC798";     // Demo Board Name stored in QuikEval EEPROM

  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz
  quikeval_SPI_init();          // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();       // Connect SPI to main data port

  Serial.begin(115200);         // Initialize the serial port to the PC
  print_title();
  demo_board_connected = discover_demo_board(demo_name);
  if (demo_board_connected)
  {
    print_prompt();
  }
  LTC2480_set_modes(LTC2480_CS, LTC2480_gain_selection, LTC2480_rejection_mode);
}

//! Repeats Linduino loop
void loop()
{
  uint16_t user_command;
  {
    if (Serial.available())
    {
      user_command = read_int();        // Read the user command
      if (user_command != 'm')
        Serial.println(user_command);   // Prints the user command to com port
      switch (user_command)
      {
        case 1:
          menu_1_read_input();
          break;
        case 2:
          menu_2_select_gain();
          break;
        case 3:
          menu_3_select_rejection();
          break;
        case 4:
          menu_4_reference_voltage();
          break;
        default:
          Serial.println(F("Invalid Option"));
          break;
      }
      Serial.println();
      print_prompt();
    }
  }
}

// Function Definitions
//! Read channel
//! @return void
void menu_1_read_input()
{
  uint8_t user_command;
  uint32_t adc_code;                           // The LTC2480 code
  int32_t display_code = 0;
  float adc_voltage;                               // The LTC2480 voltage

  // Read and display
  LTC2480_read(LTC2480_CS, &adc_code);      // discard the first reading
  delay(200);
  LTC2480_read(LTC2480_CS, &adc_code);

  display_code = adc_code >> 4;
  display_code = display_code & 0xFFFF;

  Serial.print(F("  Received Code: 0x"));
  Serial.println(display_code, HEX);

  // Convert the received code to voltage
  adc_voltage = LTC2480_code_to_voltage(display_code, LTC2480_range, LTC2480_gain);

  if (adc_code & 0x100000)    // Checking sign bit
  {
    adc_voltage = LTC2480_range - adc_voltage;
    adc_voltage = adc_voltage * (-1);
  }

  Serial.print(F("  Equivalent voltage: "));
  Serial.print(adc_voltage, 4);
  Serial.println(F(" V"));
}

//! Select number of bits
//! @return void
void menu_2_select_gain()
{
  uint16_t user_command;

  Serial.println();
  Serial.print(F("  Select a gain (1, 4, 8, 16, 32, 64, 128, 256): "));

  user_command = read_int();    // Read user input
  Serial.println(user_command);   // Prints the user command to com port
  switch (user_command)
  {
    case 1:
      LTC2480_gain_selection = LTC2480_GAIN_1;
      LTC2480_gain = 1;
      Serial.println(F("  Gain = 1"));
      break;
    case 4:
      LTC2480_gain_selection = LTC2480_GAIN_4;
      LTC2480_gain = 4;
      Serial.println(F("  Gain = 4"));
      break;
    case 8:
      LTC2480_gain_selection = LTC2480_GAIN_8;
      LTC2480_gain = 8;
      Serial.println(F("  Gain = 8"));
      break;
    case 16:
      LTC2480_gain_selection = LTC2480_GAIN_16;
      LTC2480_gain = 16;
      Serial.println(F("  Gain = 16"));
      break;
    case 32:
      LTC2480_gain_selection = LTC2480_GAIN_32;
      LTC2480_gain = 32;
      Serial.println(F("  Gain = 32"));
      break;
    case 64:
      LTC2480_gain_selection = LTC2480_GAIN_64;
      LTC2480_gain = 64;
      Serial.println(F("  Gain = 64"));
      break;
    case 128:
      LTC2480_gain_selection = LTC2480_GAIN_128;
      LTC2480_gain = 128;
      Serial.println(F("  Gain = 128"));
      break;
    case 256:
      LTC2480_gain_selection = LTC2480_GAIN_256;
      LTC2480_gain = 256;
      Serial.println(F("  Gain = 256"));
      break;
    default:
      {
        Serial.println(F("  Invalid Option"));
        return;
      }
      break;
  }
  LTC2480_set_modes(LTC2480_CS, LTC2480_gain_selection, LTC2480_rejection_mode);
}

//! Select frequency rejection
//! @return void
void menu_3_select_rejection()
{
  uint8_t user_command;

  Serial.println(F("  0: Simultaneous 50Hz/60Hz rejection"));
  Serial.println(F("  1: 50Hz Rejection"));
  Serial.println(F("  2: 60Hz Rejection"));
  Serial.print(F("  Enter the required rejection mode: "));

  user_command = read_int();    // Read user input
  Serial.println(user_command);   // Prints the user command to com port

  switch (user_command)
  {
    case 0:
      Serial.println(F("Selected simultaneous 50Hz/60Hz rejection"));
      LTC2480_rejection_mode = 0x00;
      break;
    case 1:
      Serial.println(F("Selected 50Hz Rejection"));
      LTC2480_rejection_mode = 0x02;
      break;
    case 2:
      Serial.println(F("Selected 60Hz rejection"));
      LTC2480_rejection_mode = 0x04;
      break;
    default:
      {
        Serial.println(F("  Invalid Option"));
        return;
      }
      break;
  }
  LTC2480_set_modes(LTC2480_CS, LTC2480_gain_selection, LTC2480_rejection_mode);
}

void menu_4_reference_voltage()
{
  Serial.print(F("\n  Enter the reference voltage: "));
  LTC2480_vref = read_float();
  Serial.print(LTC2480_vref);
  Serial.println(F(" V"));

  LTC2480_range = LTC2480_vref/2;
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC798B Demonstration Program                                  *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send data and receive data   *\n"));
  Serial.print(F("* from the 16-bit ADC.                                          *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("\n1-Read Input\n"));
  Serial.print(F("2-Select Gain\n"));
  Serial.print(F("3-Select Rejection\n"));
  Serial.print(F("4-Change reference voltage\n"));
  Serial.print(F("\nEnter a Command: "));
}