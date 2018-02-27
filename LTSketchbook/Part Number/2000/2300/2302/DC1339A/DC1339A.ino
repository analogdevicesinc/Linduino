/*!
Linear Technology DC1186A Demonstration Board.
LTC2302: 12-Bit, 1-Channel 500ksps SAR ADC with SPI Interface.

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. No
   external power supply is required and it has an on-board reference voltage
   of 4.096V. The analog input is given at IN+ and IN-.

   There are options to chose between unipolar and bipolar modes.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2302

http://www.linear.com/product/LTC2302#demoboards


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
    @ingroup LTC2302
*/

// Headerfiles
#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include <SPI.h>
#include "LTC2302.h"

// Global variables
static uint16_t uni_bipolar = LTC2302_UNIPOLAR;    //!< Default set for unipolar mode
static float LTC2302_vref = 4.096;

//! Initialize Linduino
void setup()
{
  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz
  quikeval_SPI_init();          // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();       // Connect SPI to main data port

  Serial.begin(115200);         // Initialize the serial port to the PC
  print_title();
  print_prompt();
}

//! Repeats Linduino loop
void loop()
{
  uint16_t user_command;
  if (Serial.available())               // Check for user input
  {
    user_command = read_int();        // Read the user command
    if (user_command != 'm')
      Serial.println(user_command);   // Prints the user command to com port
    Serial.flush();
    switch (user_command)
    {
      case 1:
        menu_1_read_input();
        break;
      case 2:
        menu_2_select_uni_bipolar();
        break;
      default:
        Serial.println(F("Invalid Option"));
        break;
    }
    Serial.println();
    Serial.println(F("*************************\n"));
    print_prompt();
  }
}

//! Read ADC code and display the measured voltage
void menu_1_read_input()
{
  uint16_t adc_command;
  uint16_t adc_code;
  float voltage;

  adc_command = uni_bipolar;
  LTC2302_read(QUIKEVAL_CS, adc_command, &adc_code);
  adc_code = (adc_code >> 4) & 0x0FFF;
  voltage = LTC2302_code_to_voltage(adc_code, LTC2302_vref, uni_bipolar);
  Serial.print("\nMeasured Voltage = ");
  Serial.println(voltage);
}

//! Select unipolar (0-VREF) or bipolar (+/- 0.5 x VREF) mode
void menu_2_select_uni_bipolar()
{
  uint8_t user_command;

  Serial.println(F("\n  0 = Bipolar"));
  Serial.println(F("  1 = Unipolar"));
  Serial.print(F("  Enter an option: "));

  user_command = read_int();    // Read user input for uni_bipolar
  Serial.println(user_command);
  switch (user_command)
  {
    case 0:
      Serial.println(F("\n  Bipolar mode selected"));
      uni_bipolar = LTC2302_BIPOLAR;
      break;
    case 1:
      Serial.println(F("\n  Unipolar mode selected"));
      uni_bipolar = LTC2302_UNIPOLAR;
      break;
    default:
      Serial.println("\n  Invalid Option");
      return;
      break;
  }
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC1339A Demonstration Program                                 *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data and receive data   *"));
  Serial.println(F("* from the LTC2302 12-bit ADC.                                  *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
}


//! Prints main menu.
void print_prompt()
{
  Serial.println(F("  OPTIONS\n"));
  Serial.println(F("1-Read ADC Input "));
  Serial.println(F("2-Select Unipolar / Bipolar measurement (default is Unipolar)"));
  Serial.println();
  Serial.print(F("Enter a command:  "));
}

