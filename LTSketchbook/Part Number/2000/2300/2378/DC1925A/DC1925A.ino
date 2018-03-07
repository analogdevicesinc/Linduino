/*!
Linear Technology DC1925A Demonstration Board with SPI interface.
LTC2376-20: 20-Bit, 250Ksps ADC
LTC2377-20: 20-Bit, 500Ksps ADC
LTC2378-20: 20-Bit, 1Msps ADC
Max SCK rate is 100MHz.

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. Equipment
   required is a voltage source (preferably low-noise) and a precision voltmeter.
   Ensure all jumpers on the demo board are installed in their default positions
   from the factory. Refer to Demo Manual DC1925A.

  How to test:
   The voltage source should be connected between inputs AIN+ and AIN-. Ensure both
   inputs are within their specified absolute input voltage range. (It is easiest
   to tie the voltage source negative terminal to COM.) Ensure the voltage
   source is set within the range of 0V to +5V (differential voltage range).
   (Swapping input voltages results in a reversed polarity reading.)

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim
http://www.linear.com/product/LTC2376-20
http://www.linear.com/product/LTC2377-20
http://www.linear.com/product/LTC2378-20

http://www.linear.com/product/LTC2376-20#demoboards
http://www.linear.com/product/LTC2377-20#demoboards
http://www.linear.com/product/LTC2378-20#demoboards


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
    @ingroup LTC2378
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC2378.h"
#include <SPI.h>
#include <Wire.h>

// Function Declaration
void print_title();                                         // Print the title block
void print_prompt();                                        // Prompt the user for an input command
void print_user_command(uint8_t menu);                      // Display selected differential channels

void menu_1_read_input();
void menu_2_select_gain_compression();

// Global variables
static uint8_t LTC2378_dgc = 0;         //!< Default set for no gain compression
static uint8_t LTC2378_bits = 20;                   //!< Default set for 20 bits
float LTC2378_vref = 5;


//! Initialize Linduino
void setup()
{
//  quikeval_I2C_init();           // Configure the EEPROM I2C port for 100kHz
  quikeval_SPI_init();           // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();        // Connect SPI to main data port
  Serial.begin(115200);          // Initialize the serial port to the PC
  print_title();
  print_prompt();
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
          menu_2_select_gain_compression();
          break;
        default:
          Serial.println("  Invalid Option");
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
  int32_t adc_code;                           // The LTC2378 code
  int32_t display_code;
  float adc_voltage;

  // Read and display a selected channel
  LTC2378_read(LTC2378_CS, &adc_code);  //discard first reading
  delay(100);
  LTC2378_read(LTC2378_CS, &adc_code);

  display_code = adc_code >> (32 - LTC2378_bits);  //shift to the right
  display_code = display_code & 0xFFFFF;  //mask off 20 bits

  Serial.print(F("  Received Code: b"));
  Serial.println(display_code, BIN);

  adc_voltage = LTC2378_code_to_voltage(adc_code, LTC2378_dgc, LTC2378_vref);    // Convert the received code to voltage

  Serial.print(F("  Equivalent voltage: "));
  Serial.print(adc_voltage, 4);
  Serial.println(F("V"));
}


//! Select gain compression
//! @return void
void menu_2_select_gain_compression()
{
  uint8_t user_command;

  Serial.println(F("  0 = No Gain Compression"));
  Serial.println(F("  1 = Gain Compression"));
  Serial.print(F("  Enter a Command, based upon the position of jumper JP6: "));

  user_command = read_int();    // Read user input
  Serial.println(user_command);   // Prints the user command to com port
  switch (user_command)
  {
    case 0:
      Serial.println(F("  No Gain compression"));
      LTC2378_dgc = 0;
      break;
    case 1:
      Serial.println(F("  Gain compression"));
      LTC2378_dgc = 1;
      break;
    default:
      {
        Serial.println("  Invalid Option");
        return;
      }
      break;
  }
}


//! Prints the title block when program first starts.
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC1925A Demonstration Program                                 *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to receive data                 *"));
  Serial.println(F("* from the following ADCs:                                      *"));
  Serial.println(F("*   LTC2376-20                                                  *"));
  Serial.println(F("*   LTC2377-20                                                  *"));
  Serial.println(F("*   LTC2378-20                                                  *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
}


//! Prints main menu.
void print_prompt()
{
  Serial.println(F("*************************"));
  Serial.println(F("1-Read ADC Input"));
  Serial.println(F("2-Select No Gain Compression / Gain Compression (default is no compression)"));
  Serial.print(F("Enter a command:"));
}

