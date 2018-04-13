/*!

Copyright 2018(c) Analog Devices, Inc.

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

/*! @ingroup RF_Timing
    @{
    @defgroup LTC5556 LTC5556: Dual Programmable Downconverting Mixer with IF DVGAs
    @}

        @file
    @ingroup LTC5556
    Library for LTC5556: Dual Programmable Downconverting Mixer with IF DVGAs
*/

#include "LTC5556.h"
#include <SPI.h>
#include "LT_SPI.h"
#include "UserInterface.h"
#include "Linduino.h"

/// Global Variables
// 8 control bits for mixer 1 (1/2 of the register)
uint8_t mixer_1_value;

// 8 control bits for mixer 2 (1/2 of the register)
uint8_t mixer_2_value;

// 16 bit register for writing to the part
uint16_t register_value;

// 16 bit output from the part
uint16_t full_output;

// 8 bit output from mixer 1
uint8_t mixer_1_output;

// 8 bit output from mixer 2
uint8_t mixer_2_output;

// Writes to the LTC5556 twice and reads back the last
// byte to make sure the LTC5556 was loaded properly
void LTC5556_write(uint8_t cs,  uint16_t tx, uint16_t *rx) {
  // Transfer 4 bytes
  spi_transfer_word(cs, tx, rx);
  spi_transfer_word(cs, tx, rx);
}

// Take inputs and apply same settings to both mixer channels
uint8_t LTC5556_dupl_settings() {
  // Bit for controlling the LTC5556's power mode
  uint8_t power_bit;

  // Prompt for getting power mode
  char *power_prompt = "Enter a command: ";

  // Bit for controlling the LTC5556's IF attenuation
  uint8_t att_bits;

  // Prompt for getting att level
  char *att_prompt = "\nDesired IF Attenuation (dB): ";

  // 16 bit input for the LTC5556
  uint16_t whole_register;

  // Output variable to read back the register into
  uint16_t output_value;

  // Get power mode bit
  power_bit = LTC5556_get_power_mode(power_prompt);

  // Get IF attenuation level
  att_bits = LTC5556_get_att(att_prompt);

  // Build mixer command byte
  mixer_1_value = (power_bit | att_bits);

  // Load the full register with two sets of identical data
  whole_register = (mixer_1_value << 8) | mixer_1_value;

  // Send the data to the LTC5556 and read back the previously loaded value
  LTC5556_write(LTC5556_CS, whole_register, &full_output);

  // Return the register's value to the main program
  return full_output;
}

// Take inputs and apply different settings to each mixer channel
uint16_t LTC5556_diff_settings() {
  // Bit for controlling the LTC5556's power mode (Channel 1)
  uint8_t power_1_bit;

  // Prompt for getting power mode (Channel 1)
  char *power_1_prompt = "Enter a command for Channel 1: ";

  // Bit for controlling the LTC5556's power mode (Channel 2)
  uint8_t power_2_bit;

  // Prompt for getting power mode (Channel 2)
  char *power_2_prompt = "Enter a command for Channel 2: ";

  // Bit for controlling the LTC5556's IF attenuation (Channel 1)
  uint8_t att_1_bits;

  // Prompt for getting att level (Channel 1)
  char *att_1_prompt = "\nDesired Channel 1 IF Attenuation (dB): ";

  // Bit for controlling the LTC5556's IF attenuation (Channel 2)
  uint8_t att_2_bits;

  // Prompt for getting att level (Channel 2)
  char *att_2_prompt = "\nDesired Channel 2 IF Attenuation (dB): ";

  // 16 bit input for the LTC5556
  uint16_t whole_register;

  // 16 bit output from the LTC5556 to be returned
  uint8_t output_value;

  // Get channel 1 power mode bit
  power_1_bit = LTC5556_get_power_mode(power_1_prompt);

  // Get channel 2 power mode bit
  power_2_bit = LTC5556_get_power_mode(power_2_prompt);

  // Get IF1 attenuation level
  att_1_bits = LTC5556_get_att(att_1_prompt);

  // Get IF2 attenuation level
  att_2_bits = LTC5556_get_att(att_2_prompt);

  // Build mixer 1 command byte
  mixer_1_value = (power_1_bit | att_1_bits);

  // Build mixer 2 command byte
  mixer_2_value = (power_2_bit | att_2_bits);

  // Load the full register with the settings for each mixer
  whole_register = (mixer_2_value << 8) | mixer_1_value;

  // Send the data to the LTC5556 and read back the previously loaded value
  LTC5556_write(LTC5556_CS, whole_register, &full_output);

  // Return the least significant byte read back to the main program
  return full_output;
}

// Get power mode value from user
uint8_t LTC5556_get_power_mode(char *prompt) {
  // Variable to break out of while loop
  int keep_looping = 0;

  // The user command for power mode
  int8_t power_command;

  // Bits for controlling the LTC5556's power mode
  uint8_t power_bit = LTC5556_FULL_POWER;
  while (1) {
    keep_looping = 0;
    Serial.println(F("\n\n1. Full Power Mode"));
    Serial.println(F("2. Low Power Mode\n"));
    Serial.print(prompt);

    // Take input from the user
    power_command = read_int();

    // Prints the user command to com port
    Serial.println(power_command);
    switch (power_command) {
        // Act on user input
      case 1:
        // Set the power bit to full power
        power_bit = LTC5556_FULL_POWER;
        break;
      case 2:
        // Set the power bit to reduced power
        power_bit = LTC5556_REDUCED_POWER;
        break;
      default:
        // User input was wrong, ask again
        Serial.println(F("\n\nIncorrect Option\n"));
        keep_looping = 1;
    }
    if (keep_looping == 0) {
      // Return power bit value to the main program
      // (shifted to correct position in the register)
      return power_bit;
    }
  }
}

// Get attenuation value from user
uint8_t LTC5556_get_att(char *prompt) {
  // Variable to break out of while loop
  int keep_looping = 0;

  // The user command for IF attenuation level
  float att_command;

  // Bit for controlling the LTC5556's IF attenuation
  uint8_t att_bits;
  while (1) {
    keep_looping = 0;
    Serial.print(prompt);
    att_command = read_float();

    // Prints the user command to com port
    Serial.println(att_command, 1);

    // User input wasn't in 0.5dB increments, ask again
    if (fmod(att_command, 0.5) != 0) {
      Serial.println(F("\n\nIncorrect Option - Choose a number "
        "between 0 and 15.5dB in 0.5dB increments\n"));
      keep_looping = 1;
    }
    // User input was negative, ask again
    else if (att_command < 0) {
      Serial.println(F("\n\nIncorrect Option - Choose a number "
        "between 0 and 15.5dB in 0.5dB increments\n"));
      keep_looping = 1;
    }
    // User input was too high, ask again
    else if (att_command > 31) {
      Serial.println(F("\n\nIncorrect Option - Choose a number "
        "between 0 and 15.5dB in 0.5dB increments\n"));
      keep_looping = 1;
    }
    else {
      // User input was within the accepted parameters

      // Double the input to convert it to a number between 0-31
      att_command = att_command * 2.;

      // Set the bits to an integer of the commanded value
      att_bits = static_cast<int>(att_command);
    }

    if (keep_looping == 0) {
      // Return att bits value to the main program
      return att_bits;
    }
  }
}

// Decode the register value read from the LTC5556
void LTC5556_decode_output(uint8_t output_register) {
  // Power setting bit read back from the LTC5556
  uint8_t power_bit_read;

  // Power setting value read back from the LTC5556
  String power_setting_read;

  // Att bits read back from the LTC5556
  uint8_t att_bits_read;

  // Attenuation setting value read back from the LTC5556
  float att_setting_read;

  // Read bit 7 to get the isel bit back
  power_bit_read = (output_register & 0x80) >> 7;
  switch (power_bit_read) {
    case 0:
      power_setting_read = "Full Power";
      break;
    case 1:
      power_setting_read = "Low Power";
      break;
  }

  // Print the power setting to the serial monitor
  Serial.println("Power Setting: " + power_setting_read);

  // Read bits 0-4 to get attenuation bits back
  att_bits_read = output_register & 0x1F;

  // Convert the 5 Attenuation bits (0-31) to an attenuation setting (0-15.5)
  att_setting_read = (att_bits_read / 2.);

  // Print the Attenuation setting to the serial monitor
  Serial.println("Attenuation Setting: " + String(att_setting_read, 1) + " dB");
}

