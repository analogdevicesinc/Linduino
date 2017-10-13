/*!
Linear Technology DC1304A-A Demonstration Board.
LTC6602: Dual Matched, High Frequency Bandpass/Lowpass Filters

@verbatim

NOTES
  Setup:
    Set the terminal baud rate to 115200 and select the newline terminator.
    Equipment required is a precision voltage source and a precision
    voltmeter. No external power supply is required. Ensure all jumpers on
    the demo board are installed in their default positions from the
    factory.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC6602

http://www.linear.com/product/LTC6602#demoboards

REVISION HISTORY
$Revision: 4047 $
$Date: 2015-09-22 17:01:37 -0700 (Tue, 22 Sep 2015) $

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
    @ingroup LTC6602
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LT_I2C.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC6602.h"
#include <SPI.h>
#include <Wire.h>

// Function Declaration
void print_title();                         // Print the title block
void print_prompt();                        // Prompt the user for an input command
void print_user_command(uint8_t menu);      // Display selected differential channels
void menu_1_filter_settings();              // Sets the gain and cutoff frequencies
void menu_2_set_gpo();                      // Sets the GPO
void menu_3_shutdown();                     // Shuts down the LTC6602
void menu_4_poweron();                      // Power up the LTC6602

// Global variables
uint8_t spi_out;
uint8_t control_byte;
uint8_t filter_gain_settings = LTC6602_GAIN_0dB;
uint8_t filter_lp_settings = LTC6602_LPF0 ;
uint8_t filter_hp_settings = LTC6602_HPF0 ;
uint8_t gpio_settings = LTC6602_GPO_LOW ;
uint8_t shdn_settings = LTC6602_SHDN;

//! Initialize Linduino
void setup()
{
  quikeval_SPI_init();  // Configure the spi port for 4MHz SCK
  Serial.begin(115200); // Initialize the serial port to the PC
  print_title();        // Displays the title
  print_prompt();       // Display user options
  control_byte = 0x00;  // A global variable that contains the whole control byte.

}

//! Repeats Linduino loop
void loop()
{
  int16_t user_command;         // The user input command
  if (Serial.available())       // Check for user input
  {
    user_command = read_int();        // Read the user command
    if (user_command != 'm')
      Serial.println(user_command);   // Prints the user command to com port

    switch (user_command)
    {
      case 1:
        menu_1_filter_settings();
        break;
      case 2:
        menu_2_set_gpo();
        break;
      case 3:
        menu_3_shutdown();
        break;
      case 4:
        menu_4_poweron();
        break;
      default:
        Serial.println("Incorrect Option");
        break;
    }
    print_prompt();
  }
}

// Function Definitions

//! Set filter settings
//! @return void
void  menu_1_filter_settings()
{
  int16_t user_command; // The user input command
  while (1)
  {
    Serial.print(F("*************************\n\n"));
    Serial.println (F("0-Set Gain"));
    Serial.println (F("1-Set Lowpass"));
    Serial.println (F("2-Set Highpass"));
    Serial.println (F("3-Upload Filter Settings"));
    Serial.print   (F("m-Main Menu\n"));
    Serial.print   (F("\nEnter a Command: "));
    user_command = read_int();                              // Read the single command
    if (user_command == 'm')
      return;
    else
      Serial.println(user_command);
    Serial.println();

    switch (user_command)
    {
      case 0:
        Serial.println (F("Gain: 0-0dB, 1-12dB, 2-24dB, 3-30dB"));
        user_command = read_int();
        switch (user_command)
        {
          case 0:
            control_byte = (control_byte & LTC6602_GAIN_MASK) | LTC6602_GAIN_0dB;
            break;
          case 1:
            control_byte = (control_byte & LTC6602_GAIN_MASK) | LTC6602_GAIN_12dB;
            break;
          case 2:
            control_byte = (control_byte & LTC6602_GAIN_MASK) | LTC6602_GAIN_24dB;
            break;
          case 3:
            control_byte = (control_byte & LTC6602_GAIN_MASK) | LTC6602_GAIN_30dB;
            break;
          default:
            Serial.println (F("incorrect option"));
        }
        break;
      case 1:
        Serial.println (F("Lowpass Divider: 0-100, 1-100, 2-300, 3-600"));
        user_command = read_int();
        switch (user_command)
        {
          case 0:
            control_byte = (control_byte & LTC6602_LPF_MASK) | LTC6602_LPF3;
            break;
          case 1:
            control_byte = (control_byte & LTC6602_LPF_MASK) | LTC6602_LPF2;
            break;
          case 2:
            control_byte = (control_byte & LTC6602_LPF_MASK) | LTC6602_LPF1;
            break;
          case 3:
            control_byte = (control_byte & LTC6602_LPF_MASK) | LTC6602_LPF0;
            break;
          default:
            Serial.println (F("incorrect option"));
        }
        break;
      case 2:
        Serial.println (F("Highpass Divider: 0-1000, 1-2000, 2-6000, 3-Bypass HPF"));
        user_command = read_int();
        switch (user_command)
        {
          case 0:
            control_byte = (control_byte & LTC6602_HPF_MASK) | LTC6602_HPF2;
            break;
          case 1:
            control_byte = (control_byte & LTC6602_HPF_MASK) | LTC6602_HPF1;
            break;
          case 2:
            control_byte = (control_byte & LTC6602_HPF_MASK) | LTC6602_HPF0;
            break;
          case 3:
            control_byte = (control_byte & LTC6602_HPF_MASK) | LTC6602_HPF3;
            break;
          default:
            Serial.println (F("incorrect option"));
        }
        break;
      case 3:
        spi_out = control_byte;
        LTC6602_write(LTC6602_CS, &spi_out, (uint8_t)1);
        break;
      default:
        Serial.println("Incorrect Option");
        break;
    }
    Serial.print ("\Control byte 0x" + String(control_byte, HEX) + "\n");
  }
}

void menu_2_set_gpo()
{
  int16_t user_command; // The user input command

  while (1)
  {
    Serial.print(F("*************************\n\n"));
    Serial.println(F("0-Set GPIO Low"));
    Serial.println (F("1-Set GPIO High"));
    Serial.print(F("m-Main Menu\n"));
    Serial.print(F("\nEnter a Command: "));
    user_command = read_int();                              // Read the single command
    if (user_command == 'm')
      return;
    else
      Serial.println(user_command);
    Serial.println();
    switch (user_command)
    {
      case 0:
        control_byte = (control_byte & LTC6602_GPO_MASK) | LTC6602_GPO_LOW;
        spi_out = control_byte;
        LTC6602_write(LTC6602_CS, &spi_out, (uint8_t)1);
        break;
      case 1:
        control_byte = (control_byte & LTC6602_GPO_MASK) | LTC6602_GPO_HIGH;
        spi_out = control_byte;
        LTC6602_write(LTC6602_CS, &spi_out, (uint8_t)1);
        break;
      default:
        Serial.println (F("incorrect option"));
        break;
    }
    Serial.print ("\Control byte 0x" + String(control_byte, HEX) + "\n");
  }
}

void menu_3_shutdown()
{
  Serial.print(F("* The LTC6602 is powered down *\n"));
  control_byte = (control_byte & LTC6602_ONOFF_MASK) | LTC6602_SHDN;
  spi_out = control_byte;
  LTC6602_write(LTC6602_CS, &spi_out, (uint8_t)1);
  Serial.print ("\Control byte 0x" + String(control_byte, HEX) + "\n");

}

void menu_4_poweron()
{
  Serial.print(F("* The LTC6602 is turning on *\n"));
  control_byte = (control_byte & LTC6602_ONOFF_MASK) | LTC6602_PRUP;
  spi_out = control_byte;
  LTC6602_write(LTC6602_CS, &spi_out, (uint8_t)1);
  Serial.print ("\Control byte 0x" + String(control_byte, HEX) + "\n");

}

//! Prints the title block when program first starts.
void print_title()
{
  (F("\n*****************************************************************\n"));
  Serial.print(F("* DC1304A-A Demonstration Program                               *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send data to the LTC6602.    *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("\n1-Filter Setting\n"));
  Serial.print(F("2-Set General Purpose Output\n"));
  Serial.print(F("3-Power Down LTC6602\n"));
  Serial.print(F("4-Power Up LTC6602\n"));
  Serial.println();
  Serial.print(F("Enter a command:"));
}