/*!
Linear Technology DC1190A Demonstration Board.
LTC2360-12: 12-Bit, 100Ksps ADC.
LTC2361-12: 12-Bit, 250Ksps ADC.
LTC2362-12: 12-Bit, 500Ksps ADC
LTC2365-12: 12-Bit, 1Msps ADC
LTC2366-12: 12-Bit, 3Msps ADC

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. Equipment
   required is a voltage source (preferably low-noise) and a precision voltmeter.
   Ensure all jumpers on the demo board are installed in their default positions
   from the factory. Refer to Demo Manual DC1190A.

  How to test:
   The voltage source should be connected to input AIN. Ensure the input is within its
   specified absolute input voltage range. (It is easiest
   to tie the voltage source negative terminal to COM.) Ensure the voltage
   source is set within the range of 0V to +2.5V for low range, or within 0V to +3V
   for high range.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2360-12
http://www.linear.com/product/LTC2361-12
http://www.linear.com/product/LTC2362-12
http://www.linear.com/product/LTC2365-12
http://www.linear.com/product/LTC2366-12

http://www.linear.com/product/LTC2360-12#demoboards
http://www.linear.com/product/LTC2361-12#demoboards
http://www.linear.com/product/LTC2362-12#demoboards
http://www.linear.com/product/LTC2365-12#demoboards
http://www.linear.com/product/LTC2366-12#demoboards

REVISION HISTORY
$Revision: 3659 $
$Date: 2015-07-01 10:19:20 -0700 (Wed, 01 Jul 2015) $2366

Copyright (c) 2013, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/

/*! @file
    @ingroup LTC2366
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC2366.h"
#include <SPI.h>
#include <Wire.h>

// Function Declaration
void print_title();                                         // Print the title block
void print_prompt();                                        // Prompt the user for an input command
void print_user_command(uint8_t menu);                      // Display selected differential channels

void menu_1_read_input();
void menu_2_select_vref();

// Global variables
static uint8_t LTC2366_bits = 12;                   //!< Default set for 12 bits
float LTC2366_vref = 2.5;            //!< Default set for 2.5V with JP1 in the 2.5V position


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
  quikeval_SPI_connect();       //Initialize for SPI
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
  uint16_t adc_code =0;                           // The LTC2366 code
  uint16_t display_code;
  float adc_voltage;                               // The LTC2366 voltage

  // Read and display a selected channel
  LTC2366_read(LTC2366_CS, &adc_code);  //discard the first reading
  delay(100);
  LTC2366_read(LTC2366_CS, &adc_code);

  display_code = adc_code >> (16 - LTC2366_bits - 2);  //the data is left justified to bit_13 of a 16 bit word
  display_code = display_code & 0xFFF;

  Serial.print(F("  Received Code: b"));
  Serial.println(display_code, BIN);

  // Convert the received code to voltage
  adc_voltage = LTC2366_code_to_voltage(adc_code, LTC2366_vref);

  Serial.print(F("  Equivalent voltage: "));
  Serial.print(adc_voltage, 4);
  Serial.println(F("V"));
  Serial.println();
}



//! Prints the title block when program first starts.
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC1190A Demonstration Program                                 *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to receive data                 *"));
  Serial.println(F("* from the following ADCs:                                      *"));
  Serial.println(F("*   LTC2360-12                                                  *"));
  Serial.println(F("*   LTC2361-12                                                  *"));
  Serial.println(F("*   LTC2362-12                                                  *"));
  Serial.println(F("*   LTC2365-12                                                  *"));
  Serial.println(F("*   LTC2366-12                                                  *"));
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
  Serial.print(F("Enter a command:  "));
}

