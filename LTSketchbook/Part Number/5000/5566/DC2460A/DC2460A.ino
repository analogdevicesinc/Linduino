/*!
Copyright 2018(c) Analog Devices, Inc.

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
    @ingroup LTC5566
*/

#include "LTC5566.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "Linduino.h"
#include <SPI.h>

// Function Declarations

// Print the title block
void LTC5566_print_title();

// Print the prompt block
void LTC5566_print_prompt();

//! Initialize Linduino
void setup() {
  // Configure the spi port for 4MHz SCK
  quikeval_SPI_init();

  // Initialize the serial port to the PC
  Serial.begin(115200);

  // Displays the title
  LTC5566_print_title();

  // Displays the initial prompt
  LTC5566_print_prompt();
}

//! Repeats Linduino loop
void loop() {
  // The user input command
  uint8_t user_command;

  // The register value read back from the LTC5566
  uint16_t output_register;

  // Dummy variable for decoding Mixer 1's register value
  uint8_t byte_1;

  // Dummy variable for decoding Mixer 2's register value
  uint8_t byte_2;

  // Check for user input
  if (Serial.available()) {
    // Read the user command
    user_command = read_int();

    // Print the command to the screen
    Serial.println(user_command);
    switch (user_command) {
      case 1:
        // Run the duplicate settings function
        output_register = LTC5566_dupl_settings();
        Serial.println(F("\n\n\nLTC5566 settings:\n"));

        // Decode the resulting value
        LTC5566_decode_output(output_register);
        break;
      case 2:
        // Run the different settings function
        output_register = LTC5566_diff_settings();

        // Pick off least significant 8 bits for decoding
        byte_1 = output_register & 0xFF;

        // Pick off most significant 8 bits for decoding
        byte_2 = output_register >> 8;
        Serial.println(F("\n\n\nMixer 1 settings:\n"));

        // Decode the resulting value
        LTC5566_decode_output(byte_1);
        Serial.println(F("\n\n\nMixer 2 settings:\n"));

        // Decode the resulting value
        LTC5566_decode_output(byte_2);
        break;
      default:
        Serial.println(F("\n\nIncorrect Option\n"));
    }
    Serial.println(F("\n*****************************************************************\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n"));
    LTC5566_print_prompt();
  }
}

//! Print the title block
void LTC5566_print_title() {
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC2460A Demonstration Program                                 *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data to the LTC5566.    *"));
  Serial.println(F("*****************************************************************"));
}

//! Prints the main menu
void LTC5566_print_prompt() {
  Serial.println(F("\n1. Same settings for Channels A and B"));
  Serial.println(F("2. Different settings for Channels A and B\n"));
  Serial.print(F("Enter a command: "));
}
