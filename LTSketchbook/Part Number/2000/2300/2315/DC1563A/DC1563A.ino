/*!
Linear Technology DC1563A Demonstration Board.
LTC2312-12: 12-Bit, 500Ksps ADC.
LTC2312-14: 14-Bit, 500Ksps ADC.
LTC2313-12: 12-Bit, 2.5Msps ADC
LTC2313-14: 14-Bit, 2.5Msps ADC
LTC2314-14: 14-Bit, 4.5Msps ADC
LTC2315-12: 12-Bit, 5Msps ADC

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. Equipment
   required is a voltage source (preferably low-noise) and a precision voltmeter.
   Ensure all jumpers on the demo board are installed in their default positions
   from the factory. Refer to Demo Manual DC1563A.

  How to test:
   The voltage source should be connected to input AIN. Ensure the input is within its
   specified absolute input voltage range. (It is easiest
   to tie the voltage source negative terminal to COM.) Ensure the voltage
   source is set within the range of 0V to +2.048V for low range, or within 0V to +4.096V
   for high range.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2312-12
http://www.linear.com/product/LTC2312-14
http://www.linear.com/product/LTC2313-12
http://www.linear.com/product/LTC2313-14
http://www.linear.com/product/LTC2314-14
http://www.linear.com/product/LTC2315-12

http://www.linear.com/product/LTC2312-12#demoboards
http://www.linear.com/product/LTC2312-14#demoboards
http://www.linear.com/product/LTC2313-12#demoboards
http://www.linear.com/product/LTC2313-14#demoboards
http://www.linear.com/product/LTC2314-14#demoboards
http://www.linear.com/product/LTC2315-12#demoboards


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
    @ingroup LTC2315
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC2315.h"
#include <SPI.h>
#include <Wire.h>

// Function Declaration
void print_title();                                         // Print the title block
void print_prompt();                                        // Prompt the user for an input command
void print_user_command(uint8_t menu);                      // Display selected differential channels

void menu_1_read_input();
void menu_2_select_bits();
void menu_3_select_vref();
void menu_4_select_part();

// Global variables
static uint8_t LTC2315_bits = 12;                   //!< Default set for 12 bits
static uint8_t LTC2315_shift = 1;
float LTC2315_vref = 4.096;
static uint8_t LTC2315_JP3 = 1;                     //!< Default set for 4.096V (Vdd = 5V)


//! Initialize Linduino
void setup()
{
  uint32_t adc_code;
  quikeval_I2C_init();           // Configure the EEPROM I2C port for 100kHz
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
          menu_2_select_bits();
          break;
        case 3:
          menu_3_select_vref();
          break;
        case 4:
          menu_4_select_part();
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
  uint16_t adc_code;                           // The LTC2315 code
  uint16_t display_code;
  float adc_voltage;                               // The LTC2315 voltage

  // Read and display a selected channel
  LTC2315_read(LTC2315_CS, &adc_code);  //discard the first reading
  delay(100);
  LTC2315_read(LTC2315_CS, &adc_code);

  display_code = adc_code >> (16 - LTC2315_bits - LTC2315_shift);  //the data is left justified to bit_14 of a 16 bit word for the LTC2314/2315, and left justified to bit_15 for the LTC2312/2313
  if (LTC2315_bits == 12)
    display_code = display_code & 0xFFF;
  else
    display_code = display_code & 0x3FFF;

  Serial.print(F("  Received Code: b"));
  Serial.println(display_code, BIN);

  // Convert the received code to voltage
  adc_voltage = LTC2315_code_to_voltage(adc_code, LTC2315_shift, LTC2315_vref);

  Serial.print(F("  Equivalent voltage: "));
  Serial.print(adc_voltage, 4);
  Serial.println(F("V"));
}


//! Select number of bits
//! @return void
void menu_2_select_bits()
{
  uint8_t user_command;

  Serial.println(F("  12 = 231X-12"));
  Serial.println(F("  14 = 231X-14"));
  Serial.print(F("  Enter a Command, based upon the resolution of the part under test: "));

  user_command = read_int();    // Read user input
  Serial.println(user_command);   // Prints the user command to com port
  switch (user_command)
  {
    case 12:
      LTC2315_bits = 12;
      Serial.println(F("  12 bits selected"));
      break;
    case 14:
      LTC2315_bits = 14;
      Serial.println(F("  14 bits selected"));
      break;
    default:
      {
        Serial.println("  Invalid Option");
        return;
      }
      break;
  }
}


//! Select vref
//! @return void
void menu_3_select_vref()
{
  uint8_t user_command;

  Serial.println(F("  0 = 2.048V Vref (Vdd = 3.3V)"));
  Serial.println(F("  1 = 4.096V Vref (Vdd = 5V)"));
  Serial.print(F("  Enter a Command, based upon the position of JP3: "));

  user_command = read_int();    // Read user input
  Serial.println(user_command);   // Prints the user command to com port
  switch (user_command)
  {
    case 0:
      LTC2315_JP3 = 0;
      LTC2315_vref = 2.048;
      Serial.println(F("  2.048V Vref selected"));
      break;
    case 1:
      LTC2315_JP3 = 1;
      LTC2315_vref = 4.096;
      Serial.println(F("  4.096V Vref selected"));
      break;
    default:
      {
        Serial.println("  Invalid Option");
        return;
      }
      break;
  }
}


//! Select part
//! @return void
void menu_4_select_part()
{
  uint8_t user_command;

  Serial.println(F("  0 = LTC2314/2315"));
  Serial.println(F("  1 = LTC2312/2313"));
  Serial.print(F("  Enter a Command: "));

  user_command = read_int();    // Read user input
  Serial.println(user_command);   // Prints the user command to com port
  switch (user_command)
  {
    case 0:
      LTC2315_shift = 1;
      Serial.println(F("  LTC2314/2315 selected"));
      break;
    case 1:
      LTC2315_shift = 0;
      Serial.println(F("  LTC2312/2313 selected"));
      break;
    default:
      Serial.println("  Invalid Option");
      return;
      break;
  }
}


//! Prints the title block when program first starts.
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC1563A Demonstration Program                                 *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to receive data                 *"));
  Serial.println(F("* from the following ADCs:                                      *"));
  Serial.println(F("*   LTC2312-12                                                  *"));
  Serial.println(F("*   LTC2312-14                                                  *"));
  Serial.println(F("*   LTC2313-12                                                  *"));
  Serial.println(F("*   LTC2313-14                                                  *"));
  Serial.println(F("*   LTC2314-14                                                  *"));
  Serial.println(F("*   LTC2315-12                                                  *"));
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
  Serial.println(F("2-Select Number of bits (Default is 12 bits)"));
  Serial.println(F("3-Select Vref (Default is 4.096V)"));
  Serial.println(F("4-Select Part (Default is LTC2314/2315)\n"));
  Serial.print(F("Enter a command:  "));
}

