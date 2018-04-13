/*!
Copyright 2018(c) Analog Devices, Inc.

LTC5566: Dual Programmable Gain Downconverting Mixer

@verbatim

The LTC5566 dual programmable gain downconverting mixer
is ideal for diversity and MIMO receivers that require precise
gain setting. Each channel incorporates an active mixer and a
digital IF VGA with 15.5dB gain control range. The IF gain of
each channel is programmed in 0.5dB steps through the SPI.

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

//! @ingroup RF_Timing
//! @{
//! @defgroup LTC5566 LTC5566: Dual Programmable Gain Downconverting Mixer
//! @}

/*! @file
    @ingroup LTC5566
    Library for LTC5566: Dual Programmable Gain Downconverting Mixer
*/

#include "LTC5566.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "Linduino.h"
#include <SPI.h>

// Global Variables

// 8 control bits for mixer 1 (1/2 of the register)
uint8_t mixer_1_value;

// 8 control bits for mixer 2 (1/2 of the register)
uint8_t mixer_2_value;

// 16 bit register for writing to the part
uint16_t whole_register;

// 16 bit output from the part
uint16_t full_output;

// 8 bits read from the LTC5566's internal register
uint8_t mixer_1_output;

// 8 bits read from the LTC5566's internal register
uint8_t mixer_2_output;

// Writes to the LTC5566 twice and reads back the last
// two bytes to make sure the LTC5566 was loaded properly
void LTC5566_write(uint8_t cs,  uint16_t tx, uint16_t *rx) {
  // Transfer 4 bytes
  spi_transfer_word(cs, tx, rx);
  spi_transfer_word(cs, tx, rx);
}

// Take inputs and apply same settings to both mixer channels
uint8_t LTC5566_dupl_settings() {
  // Bit for controlling the LTC5566's power mode
  uint8_t power_bit;

  // Prompt for getting power mode
  char *power_prompt = "Enter a command: ";

  // Bit for controlling the LTC5566's RF input tune mode
  uint8_t tune_bits;

  // Prompt for getting tune mode
  char *tune_prompt = "\nDesired RF Input Tune setting: ";

  // Bit for controlling the LTC5566's IF attenuation
  uint8_t att_bits;

  // Prompt for getting att level
  char *att_prompt = "\nDesired IF Attenuation (dB): ";

  // Get power mode bit
  power_bit = LTC5566_get_power_mode(power_prompt);

  // Get RF input tune mode
  tune_bits = LTC5566_get_tune_mode(tune_prompt);

  // Get IF attenuation level
  att_bits = LTC5566_get_att(att_prompt);

  // Build mixer command byte
  mixer_1_value = ((power_bit | tune_bits) | att_bits);

  // Load the register command with the first command byte
  whole_register = mixer_1_value;

  // Shift the first command byte up to the most significant byte
  whole_register  = whole_register << 8;

  // Load the least significant byte with the mixer command byte
  whole_register = whole_register | mixer_1_value;

  // Send the data to the LTC5566 and read back the previously loaded value
  LTC5566_write(LTC5566_CS, whole_register, &full_output);

  // Return the register's value to the main program
  return full_output;
}

// Take inputs and apply different settings to each Mixer Channel
uint16_t LTC5566_diff_settings() {
  // Bit for controlling the LTC5566's power mode (Channel 1)
  uint8_t power_1_bit;

  // Prompt for getting power mode (Channel 1)
  char *power_1_prompt = "Enter a command for Channel 1: ";

  // Bit for controlling the LTC5566's power mode (Channel 2)
  uint8_t power_2_bit;

  // Prompt for getting power mode (Channel 2)
  char *power_2_prompt = "Enter a command for Channel 2: ";

  // Bit for controlling the LTC5566's RF input tune mode (Channel 1)
  uint8_t tune_1_bits;

  // Prompt for getting tune mode (Channel 1)
  char *tune_1_prompt = "\nDesired Channel 1 RF Input Tune setting: ";

  // Bit for controlling the LTC5566's RF input tune mode (Channel 2)
  uint8_t tune_2_bits;

  // Prompt for getting tune mode (Channel 2)
  char *tune_2_prompt = "\nDesired Channel 2 RF Input Tune setting: ";

  // Bit for controlling the LTC5566's IF attenuation (Channel 1)
  uint8_t att_1_bits;

  // Prompt for getting att level (Channel 1)
  char *att_1_prompt = "\nDesired Channel 1 IF Attenuation (dB): ";

  // Bit for controlling the LTC5566's IF attenuation (Channel 2)
  uint8_t att_2_bits;

  // Prompt for getting att level (Channel 2)
  char *att_2_prompt = "\nDesired Channel 2 IF Attenuation (dB): ";

  // Get channel 1 power mode bit
  power_1_bit = LTC5566_get_power_mode(power_1_prompt);

  // Get channel 2 power mode bit
  power_2_bit = LTC5566_get_power_mode(power_2_prompt);

  // Get RF1 input tune mode
  tune_1_bits = LTC5566_get_tune_mode(tune_1_prompt);

  // Get RF2 input tune mode
  tune_2_bits = LTC5566_get_tune_mode(tune_2_prompt);

  // Get IF1 attenuation level
  att_1_bits = LTC5566_get_att(att_1_prompt);

  // Get IF2 attenuation level
  att_2_bits = LTC5566_get_att(att_2_prompt);

  // Build mixer 1 command byte
  mixer_1_value = ((power_1_bit | tune_1_bits) | att_1_bits);

  // Build mixer 2 command byte
  mixer_2_value = ((power_2_bit | tune_2_bits) | att_2_bits);

  // Load the register command with channel 2's command byte
  whole_register = mixer_2_value;

  // Shift channel 2's command byte up to the most significant byte
  whole_register  = whole_register << 8;

  // Load the least significant byte with channel 1's command byte
  whole_register = whole_register | mixer_1_value;

  // Send the data to the LTC5566 and read back the previously loaded value
  LTC5566_write(LTC5566_CS, whole_register, &full_output);

  // Return the register's value to the main program
  return full_output;
}

// Get power mode value from user
uint8_t LTC5566_get_power_mode(char *prompt) {
  // Variable to break out of while loop
  int keep_looping = 0;

  // The user command for power mode
  int8_t power_command;

  // Bits for controlling the LTC5566's power mode
  uint8_t power_bit = LTC5566_FULL_POWER;
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
        power_bit = LTC5566_FULL_POWER;
        break;
      case 2:
        // Set the power bit to reduced power
        power_bit = LTC5566_REDUCED_POWER;
        break;
      default:
        // User input was wrong, ask again
        Serial.println(F("\n\nIncorrect Option\n"));
        keep_looping = 1;
    }
    
    if (keep_looping == 0) {
      // Return power bit value to the main program
      return power_bit;
    }
  }
}

// Get RF input tune mode value from user
uint8_t LTC5566_get_tune_mode(char *prompt) {
  // Variable to break out of while loop
  int keep_looping = 0;

  // The user command for RF input tune mode
  int8_t tune_command;

  // Bits for controlling the LTC5566's RF input tune mode
  uint8_t tune_bits = LTC5566_RF_TUNE_11;
  while (1) {
    keep_looping = 0;
    Serial.println(F("\n1. RF Tune 00 (3.1GHz - 5.1GHz)"));
    Serial.println(F("2. RF Tune 01 (1.8GHz - 4.4GHz)"));
    Serial.println(F("3. RF Tune 10 (1.3GHz - 3.9GHz)"));
    Serial.println(F("4. RF Tune 11 (Less than 1.3GHz)"));
    Serial.print(prompt);

    // Take input from the user
    tune_command = read_int();

    // Prints the user command to com port
    Serial.println(tune_command);
    switch (tune_command) {
      // Act on user input
      case 1:
        // Set the RF input tune bits to 00
        tune_bits = LTC5566_RF_TUNE_00;
        break;
      case 2:
        // Set the RF input tune bits to 01
        tune_bits = LTC5566_RF_TUNE_01;
        break;
      case 3:
        // Set the RF input tune bits to 10
        tune_bits = LTC5566_RF_TUNE_10;
        break;
      case 4:
        // Set the RF input tune bits to 11
        tune_bits = LTC5566_RF_TUNE_11;
        break;
      default:
        // User input was wrong, ask again
        Serial.println(F("\n\nIncorrect Option\n"));
        keep_looping = 1;
    }
    
    if (keep_looping == 0) {
      // Return tune bits value to the main program
      return tune_bits;
    }
  }
}

// Get attenuation value from user
uint8_t LTC5566_get_att(char *prompt) {
  // Variable to break out of while loop
  int keep_looping = 0;

  // The user command for IF attenuation level
  float att_command;

  // Bit for controlling the LTC5566's IF attenuation
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

// Decode the register value read from the LTC5566
void LTC5566_decode_output(uint8_t output_register) {
  // Power bit read back from the LTC5566
  uint8_t power_bit_read;

  // Power setting read back from the LTC5566
  String power_setting_read;

  // Tune bits read back from the LTC5566
  uint8_t tune_bits_read;

  // RF Tune setting read back from the LTC5566
  String tune_setting_read;

  // Att bits read back from the LTC5566
  uint8_t att_bits_read;

  // Attenuation setting read back from the LTC5566
  float att_setting_read;

  // Read bit 7 to get power bit back
  power_bit_read = (output_register & 0x90) >> 7;
  switch (power_bit_read) {
    // The LTC5566 was in Full Power mode
    case 0:
      power_setting_read = "Full Power";
      break;
    // The LTC5566 was in Low Power mode
    case 1:
      power_setting_read = "Low Power";
      break;
  }

  // Print the Power setting to the serial monitor
  Serial.println("Power Setting: " + power_setting_read);

  // Read bits 5 and 6 to get RF tune bits back
  tune_bits_read = (output_register & 0x60) >> 5;
  switch (tune_bits_read) {
    case 0:
      tune_setting_read = "RF Tune 00 (3.1GHz - 5.1GHz)";
      break;
    case 1:
      tune_setting_read = "RF Tune 01 (1.8GHz - 4.4GHz)";
      break;
    case 2:
      tune_setting_read = "RF Tune 10 (1.3GHz - 3.9GHz)";
      break;
    case 3:
      tune_setting_read = "RF Tune 11 (Less than 1.3GHz)";
      break;
  }

  // Print the RF Tune setting to the serial monitor
  Serial.println("RF Input Setting: " + tune_setting_read);

  // Read bits 0-4 to get attenuation bits back
  att_bits_read = output_register & 0x1F;

  // Convert the 5 Attenuation bits (0-31) to an attenuation setting (0-15.5)
  att_setting_read = (att_bits_read / 2.);

  // Print the Attenuation setting to the serial monitor
  Serial.println("Attenuation Setting: " + String(att_setting_read, 1) + " dB");
}
