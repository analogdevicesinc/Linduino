/*!
Linear Technology DC2460A Demonstration Board
LTC5566: Dual Programmable Gain Downconverting Mixer

@verbatim

The LTC5566 dual programmable gain downconverting mixer
is ideal for diversity and MIMO receivers that require precise
gain setting. Each channel incorporates an active mixer and a
digital IF VGA with 15.5dB gain control range. The IF gain of
each channel is programmed in 0.5dB steps through the SPI.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0


@endverbatim

http://www.linear.com/product/LTC5566

http://www.linear.com/product/LTC5566#demoboards

REVISION HISTORY
$Revision: 5670 $
$Date: 2016-09-02 10:55:41 -0700 (Fri, 02 Sep 2016) $

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
    @ingroup LTC5566
*/

#include "LTC5566.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "Linduino.h"
#include <SPI.h>

// Function Declarations
void LTC5566_print_title();                 // Print the title block
void LTC5566_print_prompt();                // Print the prompt block

//! Initialize Linduino
void setup()
{
  quikeval_SPI_init();              // Configure the spi port for 4MHz SCK
  Serial.begin(115200);             // Initialize the serial port to the PC
  LTC5566_print_title();            // Displays the title
  LTC5566_print_prompt();           // Displays the initial prompt
}

//! Repeats Linduino loop
void loop()
{
  uint8_t user_command;             // The user input command
  uint16_t output_register;         // The register value read back from the LTC5566
  uint8_t byte_1;                   // Dummy variable for decoding Mixer 1's register value
  uint8_t byte_2;                   // Dummy variable for decoding Mixer 2's register value
  if (Serial.available())           // Check for user input
  {
    user_command = read_int();      // Read the user command
    Serial.println(user_command);   // Print the command to the screen
    switch (user_command)
    {
      case 1:
        output_register = LTC5566_dupl_settings();  // Run the duplicate settings function
        Serial.println(F("\n\n\nLTC5566 settings:\n"));
        LTC5566_decode_output(output_register);     // Decode the resulting value
        break;
      case 2:
        output_register = LTC5566_diff_settings();  // Run the different settings function
        byte_1 = output_register & 0xFF;            // Pick off least significant 8 bits for decoding
        byte_2 = output_register >> 8;              // Pick off most significant 8 bits for decoding
        Serial.println(F("\n\n\nMixer 1 settings:\n"));
        LTC5566_decode_output(byte_1);              // Decode the resulting value
        Serial.println(F("\n\n\nMixer 2 settings:\n"));
        LTC5566_decode_output(byte_2);              // Decode the resulting value
        break;
      default:
        Serial.println(F("\n\nIncorrect Option\n"));
    }
    Serial.println(F("\n*****************************************************************\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"));
    LTC5566_print_prompt();
  }
}

//! Print the title block
void LTC5566_print_title()
{
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC2460A Demonstration Program                                 *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data to the LTC5566.    *"));
  Serial.println(F("*****************************************************************"));
}

//! Prints the main menu
void LTC5566_print_prompt()
{
  Serial.println(F("\n1. Same settings for Channels A and B"));
  Serial.println(F("2. Different settings for Channels A and B\n"));
  Serial.print(F("Enter a command: "));
}
