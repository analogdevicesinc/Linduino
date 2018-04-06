/*!
Linear Technology DC2693A Demonstration Board
LTC5556: Dual Programmable Downconverting Mixer with IF DVGAs

@verbatim

The LTC5556 dual programmable gain downconverting mixer
is ideal for diversity and MIMO receivers that require
precise gain setting. Each channel incorporates an active
mixer and a digital IF VGA with 15.5dB gain control range.
The IF gain of each channel is programmed in 0.5dB steps
through the SPI. A reduced power mode is also available
for each channel.

@endverbatim

http://www.analog.com/en/products/rf-microwave/mixers/single-double-triple-balanced-mixers/ltc5556.html

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
    @ingroup LTC5556
*/
 
#include "LTC5556.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "Linduino.h"
#include <SPI.h>

// Function Declarations
void LTC5556_print_title();         // Print the title block
void LTC5556_print_prompt();        // Print the initial prompt

//! Initialize Linduino
//! @return void
void setup()
{
  quikeval_SPI_init();              // Configure the SPI port for 4MHz SCK
  Serial.begin(115200);             // Initialize the serial port to the PC
  LTC5556_print_title();            // Displays the title
  LTC5556_print_prompt();           // Displays the initial prompt
}

//! Repeats Linduino loop
//! @return void
void loop()
{
  uint8_t user_command;                                         // The user input command
  uint16_t output_register;                                     // The register value read back from the LTC5556
  uint8_t byte_1;                                               // Variable for decoding mixer 1's register value
  uint8_t byte_2;                                               // Variable for decoding mixer 2's register value

  if (Serial.available())
  {
    user_command = read_int();                                  // Read the user command
    Serial.println(user_command);                               // Print the command to the screen
    switch (user_command)
    {
      case 1:
        output_register = LTC5556_dupl_settings();              // Run the duplicate settings function
        Serial.println(F("\n\n\nLTC5556 settings:\n"));
        LTC5556_decode_output(output_register);                 // Decode the resulting value
        break;
      case 2:
        output_register = LTC5556_diff_settings();              // Run the different settings function
        byte_1 = output_register & 0xFF;                        // Pick off the least significant 8 bits for decoding
        byte_2 = output_register >> 8;                          // Pick off the most significant 8 bits for decoding
        
        Serial.println(F("\n\n\nLTC5556 Mixer 1 settings:\n"));
        LTC5556_decode_output(byte_1);                          // Decode the resulting value
        Serial.println(F("\n\n\nLTC5556 Mixer 2 settings:\n"));
        LTC5556_decode_output(byte_2);                          // Decode the resulting value
        break;
      default:
        Serial.println(F("\n\nIncorrect Option\n"));
    }
    Serial.println(F("\n*****************************************************************\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"));
    LTC5556_print_prompt();
  }
}

//! Prints the title block
//! @return void
void LTC5556_print_title()
{
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC2693A Demonstration Program                                 *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data to the LTC5556.    *"));
  Serial.println(F("*****************************************************************"));
}

//! Prints the initial prompt
//! @return void
void LTC5556_print_prompt()
{
  Serial.println(F("\n1. Same settings for Channels 1 and 2"));
  Serial.println(F("2. Different settings for Channels 1 and 2\n"));
  Serial.print(F("Enter a command: "));
}

