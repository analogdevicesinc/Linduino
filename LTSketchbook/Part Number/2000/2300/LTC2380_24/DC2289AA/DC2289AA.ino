/*!
LTC2380-24: Low Noise, High Speed, 24-Bit SAR ADC With Digital Filter

@verbatim


@endverbatim
http://www.linear.com/product/LTC2380-24


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
    @ingroup LTC2380-24
*/

#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include <SPI.h>
#include "LTC2380_24.h"

#ifndef LTC2380_CNV
#define LTC2380_CNV QUIKEVAL_CS
#endif

// Function Declaration
void print_title();                                         // Print the title block
void print_prompt();
void menu_1_read_input();
void menu_2_select_gain_compression();
void configureCNV();

// Global variables
static uint8_t LTC2380_dgc = 0;         //!< Default set for no gain compression
float LTC2380_vref  = 10;
float voltage;

//! Initialize Linduino
void setup()
{
  quikeval_SPI_init();           // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();        // Connect SPI to main data port
  Serial.begin(115200);          // Initialize the serial port to the PC

  configureCNV();

  print_title();
  print_prompt();
}

//! Repeats Linduino loop
void loop()
{
  uint16_t user_command;
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

//! Read Input voltage
//! @return void
void menu_1_read_input()
{
  int32_t adc_code;                           // The LTC2380 code
  float adc_voltage;
  int16_t cycles;

  LTC2380_read(&adc_code, &cycles);
  adc_voltage = LTC2380_code_to_voltage(adc_code, LTC2380_dgc, LTC2380_vref);    // Convert the received code to voltage

  Serial.print("\n24-bit decimal data: 0x ");
  Serial.println(adc_code & 0xFFFFFF, HEX);
  Serial.print("Voltage calculated: ");
  Serial.print(adc_voltage);
  Serial.println(" V");
  Serial.print("No:of averaging cycles: ");
  Serial.println(cycles + 1);
  Serial.println();
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
      LTC2380_dgc = 0;
      break;
    case 1:
      Serial.println(F("  Gain compression"));
      LTC2380_dgc = 1;
      break;
    default:
      {
        Serial.println("  Invalid Option");
        return;
      }
      break;
  }
}

//! Prints main menu.
void print_prompt()
{
  Serial.println(F("*************************"));
  Serial.println(F("1-Read ADC Input"));
  Serial.println(F("2-Select No Gain Compression / Gain Compression (default is no compression)"));
  Serial.print(F("Enter a command:"));
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC2289A Demonstration Program                                 *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to read data                    *"));
  Serial.println(F("* from LTC2380-24 bit part.                                     *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
}

//! Function to configure CNV pin as an output
void configureCNV()
{
  DDRB = DDRB | B00000100;       // Setting PB2 (CS) as output, connected to CNV pin.
}
