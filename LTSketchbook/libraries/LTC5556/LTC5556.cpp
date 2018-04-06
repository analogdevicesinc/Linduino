/*!
LTC5556: Dual Programmable Downconverting Mixer with IF DVGA

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

//! @ingroup WIP
//! @{  
//! @defgroup LTC5556 LTC5556: Dual Programmable Downconverting Mixer with IF DVGAs
//! @}

/*! @file
    @ingroup LTC5556
    Library for LTC5556: Dual Programmable Downconverting Mixer with IF DVGAs
*/

#include "LTC5556.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "Linduino.h"
#include <SPI.h>

// Global Variables
uint8_t mixer_1_value;                                                  // 8 control bits for mixer 1 (1/2 of the register)
uint8_t mixer_2_value;                                                  // 8 control bits for mixer 2 (1/2 of the register)
uint16_t register_value;                                                // 16 bit register for writing to the part
uint16_t full_output;                                                   // 16 bit output from the part
uint8_t mixer_1_output;                                                 // 8 bit output from mixer 1
uint8_t mixer_2_output;                                                 // 8 bit output from mixer 2

void LTC5556_write(uint8_t cs,  uint16_t tx, uint16_t *rx)
// Writes to the LTC5556 twice and reads back the last byte to make sure the LTC5556 was loaded properly
{
  spi_transfer_word(cs, tx, rx);                                        // Transfer 2 bytes
  spi_transfer_word(cs, tx, rx);                                        // Transfer 2 bytes
}

// Take inputs and apply same settings to both mixer channels
uint8_t LTC5556_dupl_settings()
{
  uint8_t power_bit;                                                    // Bit for controlling the LTC5556's power mode
  char *power_prompt = "Enter a command: ";                             // Prompt for getting power mode
  uint8_t att_bits;                                                     // Bit for controlling the LTC5556's IF attenuation
  char *att_prompt = "\nDesired IF Attenuation (dB): ";                 // Prompt for getting att level
  uint16_t whole_register;                                              // 16 bit input for the LTC5556
  uint16_t output_value;                                                 // Output variable to read back the register into

  power_bit = LTC5556_get_power_mode(power_prompt);                     // Get power mode bit
  att_bits = LTC5556_get_att(att_prompt);                               // Get IF attenuation level

  mixer_1_value = (power_bit | att_bits);                               // Build mixer command byte
  whole_register = (mixer_1_value << 8) | mixer_1_value;                // Load the full register with two sets of identical data

  LTC5556_write(LTC5556_CS, whole_register, &full_output);              // Send the data to the LTC5556 and read back the previously loaded value
  
  return full_output;                                                  // Return the register's value to the main program
}

// Take inputs and apply different settings to each mixer channel
uint16_t LTC5556_diff_settings()
{
  uint8_t power_1_bit;                                                  // Bit for controlling the LTC5556's power mode (Channel 1)
  char *power_1_prompt = "Enter a command for Channel 1: ";             // Prompt for getting power mode (Channel 1)
  uint8_t power_2_bit;                                                  // Bit for controlling the LTC5556's power mode (Channel 2)
  char *power_2_prompt = "Enter a command for Channel 2: ";             // Prompt for getting power mode (Channel 2)
  uint8_t att_1_bits;                                                   // Bit for controlling the LTC5556's IF attenuation (Channel 1)
  char *att_1_prompt = "\nDesired Channel 1 IF Attenuation (dB): ";     // Prompt for getting att level (Channel 1)
  uint8_t att_2_bits;                                                   // Bit for controlling the LTC5556's IF attenuation (Channel 2)
  char *att_2_prompt = "\nDesired Channel 2 IF Attenuation (dB): ";     // Prompt for getting att level (Channel 2)
  uint16_t whole_register;                                              // 16 bit input for the LTC5556
  uint8_t output_value;                                                 // 16 bit output from the LTC5556 to be returned
  
  power_1_bit = LTC5556_get_power_mode(power_1_prompt);                 // Get channel 1 power mode bit
  power_2_bit = LTC5556_get_power_mode(power_2_prompt);                 // Get channel 2 power mode bit
  att_1_bits = LTC5556_get_att(att_1_prompt);                           // Get IF1 attenuation level
  att_2_bits = LTC5556_get_att(att_2_prompt);                           // Get IF2 attenuation level

  mixer_1_value = (power_1_bit | att_1_bits);                           // Build mixer 1 command byte
  mixer_2_value = (power_2_bit | att_2_bits);                           // Build mixer 2 command byte
  whole_register = (mixer_2_value << 8) | mixer_1_value;                // Load the full register with the settings for each mixer

  LTC5556_write(LTC5556_CS, whole_register, &full_output);              // Send the data to the LTC5556 and read back the previously loaded value
  
  return full_output;                                                  // Return the least significant byte read back to the main program
}

// Get power mode value from user
uint8_t LTC5556_get_power_mode(char *prompt)
{
  int keep_looping = 0;                                                 // Variable to break out of while loop
  int8_t power_command;                                                 // The user command for power mode
  uint8_t power_bit = LTC5556_FULL_POWER;                               // Bits for controlling the LTC5556's power mode
  while (1)
  {
    keep_looping = 0;
    Serial.println(F("\n\n1. Full Power Mode"));
    Serial.println(F("2. Low Power Mode\n"));
    Serial.print(prompt);
    power_command = read_int();                                         // Take input from the user
    Serial.println(power_command);                                      // Prints the user command to com port
    switch (power_command)
      // Act on user input
    {
      case 1:
        power_bit = LTC5556_FULL_POWER;                                 // Set the power bit to full power
        break;
      case 2:
        power_bit = LTC5556_REDUCED_POWER;                              // Set the power bit to reduced power
        break;
      default:
        Serial.println(F("\n\nIncorrect Option\n"));                    // User input was wrong, ask again
        keep_looping = 1;
    }
    if (keep_looping == 0)
    {
      return power_bit;                                                 // Return power bit value to the main program (shifted to correct position in the register)
    }
  }
}

uint8_t LTC5556_get_att(char *prompt)
// Get attenuation value from user
{
  int keep_looping = 0;                                                 // Variable to break out of while loop
  float att_command;                                                    // The user command for IF attenuation level
  uint8_t att_bits;                                                     // Bit for controlling the LTC5556's IF attenuation
  while (1)
  {
    keep_looping = 0;
    Serial.print(prompt);
    att_command = read_float();
    Serial.println(att_command, 1);                                     // Prints the user command to com port

    if (fmod(att_command, 0.5) != 0)                                    // User input wasn't in 0.5dB increments, ask again
    {
        Serial.println(F("\n\nIncorrect Option - Choose a number between 0 and 15.5dB in 0.5dB increments\n"));
        keep_looping = 1;
    }    
    else if (att_command < 0)                                           // User input was negative, ask again
    {
        Serial.println(F("\n\nIncorrect Option - Choose a number between 0 and 15.5dB in 0.5dB increments\n"));
        keep_looping = 1;
    }
    else if (att_command > 31)                                          // User input was too high, ask again
    {
        Serial.println(F("\n\nIncorrect Option - Choose a number between 0 and 15.5dB in 0.5dB increments\n"));
        keep_looping = 1;
    }
    else                                                                // User input was within the accepted parameters
    {
      att_command = att_command * 2.;                                   // Double the input to convert it to a number between 0-31
      att_bits = (int)att_command;                                      // Set the bits to an integer of the commanded value
    }
    
    if (keep_looping == 0)
    {
      return att_bits;                                                  // Return att bits value to the main program
    }
  }
}

void LTC5556_decode_output(uint8_t output_register)
// Decode the register value read from the LTC5556
{
  uint8_t power_bit_read;                                               // Power setting bit read back from the LTC5556
  String power_setting_read;                                            // Power setting value read back from the LTC5556
  uint8_t att_bits_read;                                                // Att bits read back from the LTC5556
  float att_setting_read;                                               // Attenuation setting value read back from the LTC5556

  power_bit_read = (output_register & 0x80) >> 7;                       // Read bit 7 to get the isel bit back
  switch (power_bit_read)
  {
    case 0:
      power_setting_read = "Full Power";
      break;               
    case 1:                
      power_setting_read = "Low Power";
      break;
  }
  Serial.println("Power Setting: " + power_setting_read);               // Print the power setting to the serial monitor
  
  att_bits_read = output_register & 0x1F;                               // Read bits 0-4 to get attenuation bits back
  att_setting_read = (att_bits_read / 2.);                              // Convert the 5 Attenuation bits (0-31) to an attenuation setting (0-15.5)
  
  Serial.println("Attenuation Setting: " + String(att_setting_read, 1) + " dB");           // Print the Attenuation setting to the serial monitor
}

