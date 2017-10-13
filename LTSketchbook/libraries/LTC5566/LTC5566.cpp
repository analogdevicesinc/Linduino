/*!
LTC5566: Dual Programmable Gain Downconverting Mixer

@verbatim

The LTC5566 dual programmable gain downconverting mixer
is ideal for diversity and MIMO receivers that require precise
gain setting. Each channel incorporates an active mixer and a
digital IF VGA with 15.5dB gain control range. The IF gain of
each channel is programmed in 0.5dB steps through the SPI.

@endverbatim

http://www.linear.com/product/LTC5566

http://www.linear.com/product/LTC5566#demoboards

REVISION HISTORY
$Revision: 6237 $
$Date: 2016-12-20 15:09:16 -0800 (Tue, 20 Dec 2016) $

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
uint8_t mixer_1_value;                                                  // 8 control bits for mixer 1 (1/2 of the register)
uint8_t mixer_2_value;                                                  // 8 control bits for mixer 2 (1/2 of the register)
uint16_t whole_register;                                                // 16 bit register for writing to the part
uint16_t output;                                                        // 16 bit output from the part
uint8_t mixer_1_output;                                                 // 8 bits read from the LTC5566's internal register
uint8_t mixer_2_output;                                                 // 8 bits read from the LTC5566's internal register

// Writes to the LTC5566 twice and reads back the last two bytes to make sure the LTC5566 was loaded properly
void LTC5566_write(uint8_t cs,  uint16_t tx, uint16_t *rx)
{
  spi_transfer_word(cs, tx, rx);                                        // Transfer 2 bytes
  spi_transfer_word(cs, tx, rx);                                        // Transfer 2 bytes
}

// Take inputs and apply same settings to both mixer channels
uint8_t LTC5566_dupl_settings()
{
  uint8_t power_bit;                                                    // Bit for controlling the LTC5566's power mode
  char *power_prompt = "Enter a command: ";                             // Prompt for getting power mode
  uint8_t tune_bits;                                                    // Bit for controlling the LTC5566's RF input tune mode
  char *tune_prompt = "\nDesired RF Input Tune setting: ";              // Prompt for getting tune mode
  uint8_t att_bits;                                                     // Bit for controlling the LTC5566's IF attenuation
  char *att_prompt = "\nDesired IF Attenuation (dB): ";                 // Prompt for getting att level
  uint8_t output_register;

  power_bit = LTC5566_get_power_mode(power_prompt);                     // Get power mode bit
  tune_bits = LTC5566_get_tune_mode(tune_prompt);                       // Get RF input tune mode
  att_bits = LTC5566_get_att(att_prompt);                               // Get IF attenuation level

  mixer_1_value = ((power_bit | tune_bits) | att_bits);                 // Build mixer command byte
  whole_register = mixer_1_value;                                       // Load the register command with the first command byte
  whole_register  = whole_register << 8;                                // Shift the first command byte up to the most significant byte
  whole_register = whole_register | mixer_1_value;                      // Load the least significant byte with the mixer command byte

  LTC5566_write(LTC5566_CS, whole_register, &output);                   // Send the data to the LTC5566 and read back the previously loaded value

  output_register = output & 0xFF;                                      // Truncate to 8 bits since the registers received the same settings
  return output_register;                                               // Return the register's value to the main program
}

// Take inputs and apply different settings to each Mixer Channel
uint16_t LTC5566_diff_settings()
{
  uint8_t power_1_bit;                                                  // Bit for controlling the LTC5566's power mode (Channel 1)
  char *power_1_prompt = "Enter a command for Channel 1: ";             // Prompt for getting power mode (Channel 1)
  uint8_t power_2_bit;                                                  // Bit for controlling the LTC5566's power mode (Channel 2)
  char *power_2_prompt = "Enter a command for Channel 2: ";             // Prompt for getting power mode (Channel 2)
  uint8_t tune_1_bits;                                                  // Bit for controlling the LTC5566's RF input tune mode (Channel 1)
  char *tune_1_prompt = "\nDesired Channel 1 RF Input Tune setting: ";  // Prompt for getting tune mode (Channel 1)
  uint8_t tune_2_bits;                                                  // Bit for controlling the LTC5566's RF input tune mode (Channel 2)
  char *tune_2_prompt = "\nDesired Channel 2 RF Input Tune setting: ";  // Prompt for getting tune mode (Channel 2)
  uint8_t att_1_bits;                                                   // Bit for controlling the LTC5566's IF attenuation (Channel 1)
  char *att_1_prompt = "\nDesired Channel 1 IF Attenuation (dB): ";     // Prompt for getting att level (Channel 1)
  uint8_t att_2_bits;                                                   // Bit for controlling the LTC5566's IF attenuation (Channel 2)
  char *att_2_prompt = "\nDesired Channel 2 IF Attenuation (dB): ";     // Prompt for getting att level (Channel 2)

  power_1_bit = LTC5566_get_power_mode(power_1_prompt);                 // Get channel 1 power mode bit
  power_2_bit = LTC5566_get_power_mode(power_2_prompt);                 // Get channel 2 power mode bit
  tune_1_bits = LTC5566_get_tune_mode(tune_1_prompt);                   // Get RF1 input tune mode
  tune_2_bits = LTC5566_get_tune_mode(tune_2_prompt);                   // Get RF2 input tune mode
  att_1_bits = LTC5566_get_att(att_1_prompt);                           // Get IF1 attenuation level
  att_2_bits = LTC5566_get_att(att_2_prompt);                           // Get IF2 attenuation level

  mixer_1_value = ((power_1_bit | tune_1_bits) | att_1_bits);           // Build mixer 1 command byte
  mixer_2_value = ((power_2_bit | tune_2_bits) | att_2_bits);           // Build mixer 2 command byte
  whole_register = mixer_2_value;                                       // Load the register command with channel 2's command byte
  whole_register  = whole_register << 8;                                // Shift channel 2's command byte up to the most significant byte
  whole_register = whole_register | mixer_1_value;                      // Load the least significant byte with channel 1's command byte

  LTC5566_write(LTC5566_CS, whole_register, &output);                   // Send the data to the LTC5566 and read back the previously loaded value
  return output;                                                        // Return the register's value to the main program
}

// Get power mode value from user
uint8_t LTC5566_get_power_mode(char *prompt)
{
  int keep_looping = 0;                                                 // Variable to break out of while loop
  int8_t power_command;                                                 // The user command for power mode
  uint8_t power_bit = LTC5566_FULL_POWER;                               // Bits for controlling the LTC5566's power mode
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
        power_bit = LTC5566_FULL_POWER;                                 // Set the power bit to full power
        break;
      case 2:
        power_bit = LTC5566_REDUCED_POWER;                              // Set the power bit to reduced power
        break;
      default:
        Serial.println(F("\n\nIncorrect Option\n"));                    // User input was wrong, ask again
        keep_looping = 1;
    }
    if (keep_looping == 0)
    {
      return power_bit;                                                 // Return power bit value to the main program
    }
  }
}

// Get RF input tune mode value from user
uint8_t LTC5566_get_tune_mode(char *prompt)
{
  int keep_looping = 0;                                                 // Variable to break out of while loop
  int8_t tune_command;                                                  // The user command for RF input tune mode
  uint8_t tune_bits = LTC5566_RF_TUNE_11;                               // Bits for controlling the LTC5566's RF input tune mode
  while (1)
  {
    keep_looping = 0;
    Serial.println(F("\n1. RF Tune 00 (3.1GHz - 5.1GHz)"));
    Serial.println(F("2. RF Tune 01 (1.8GHz - 4.4GHz)"));
    Serial.println(F("3. RF Tune 10 (1.3GHz - 3.9GHz)"));
    Serial.println(F("4. RF Tune 11 (Less than 1.3GHz)"));
    Serial.print(prompt);
    tune_command = read_int();                                          // Take input from the user
    Serial.println(tune_command);                                       // Prints the user command to com port
    switch (tune_command)
      // Act on user input
    {
      case 1:
        tune_bits = LTC5566_RF_TUNE_00;                                 // Set the RF input tune bits to 00
        break;
      case 2:
        tune_bits = LTC5566_RF_TUNE_01;                                 // Set the RF input tune bits to 01
        break;
      case 3:
        tune_bits = LTC5566_RF_TUNE_10;                                 // Set the RF input tune bits to 10
        break;
      case 4:
        tune_bits = LTC5566_RF_TUNE_11;                                 // Set the RF input tune bits to 11
        break;
      default:
        Serial.println(F("\n\nIncorrect Option\n"));                    // User input was wrong, ask again
        keep_looping = 1;
    }
    if (keep_looping == 0)
    {
      return tune_bits;                                                 // Return tune bits value to the main program
    }
  }
}

// Get attenuation value from user
uint8_t LTC5566_get_att(char *prompt)
{
  int keep_looping = 0;                                                 // Variable to break out of while loop
  float att_command;                                                    // The user command for IF attenuation level
  uint8_t att_bits;                                                     // Bit for controlling the LTC5566's IF attenuation
  while (1)
  {
    keep_looping = 0;
    Serial.print(prompt);
    att_command = read_float();
    Serial.println(att_command, 1);                                     // Prints the user command to com port
    att_command = round(att_command * 2);                               // Converts the attenuation input to an integer from 1-32 so a switch-case statement can be used
    switch ((uint8_t)att_command)
    {
      case 0:
        att_bits = LTC5566_ATT_0_0dB;                                   // Set the IF attenuation to 0.0dB
        break;
      case 1:
        att_bits = LTC5566_ATT_0_5dB;                                   // Set the IF attenuation to 0.5dB
        break;
      case 2:
        att_bits = LTC5566_ATT_1_0dB;                                   // Set the IF attenuation to 1.0dB
        break;
      case 3:
        att_bits = LTC5566_ATT_1_5dB;                                   // Set the IF attenuation to 1.5dB
        break;
      case 4:
        att_bits = LTC5566_ATT_2_0dB;                                   // Set the IF attenuation to 2.0dB
        break;
      case 5:
        att_bits = LTC5566_ATT_2_5dB;                                   // Set the IF attenuation to 2.5dB
        break;
      case 6:
        att_bits = LTC5566_ATT_3_0dB;                                   // Set the IF attenuation to 3.0dB
        break;
      case 7:
        att_bits = LTC5566_ATT_3_5dB;                                   // Set the IF attenuation to 3.5dB
        break;
      case 8:
        att_bits = LTC5566_ATT_4_0dB;                                   // Set the IF attenuation to 4.0dB
        break;
      case 9:
        att_bits = LTC5566_ATT_4_5dB;                                   // Set the IF attenuation to 4.5dB
        break;
      case 10:
        att_bits = LTC5566_ATT_5_0dB;                                   // Set the IF attenuation to 5.0dB
        break;
      case 11:
        att_bits = LTC5566_ATT_5_5dB;                                   // Set the IF attenuation to 5.5dB
        break;
      case 12:
        att_bits = LTC5566_ATT_6_0dB;                                   // Set the IF attenuation to 6.0dB
        break;
      case 13:
        att_bits = LTC5566_ATT_6_5dB;                                   // Set the IF attenuation to 6.5dB
        break;
      case 14:
        att_bits = LTC5566_ATT_7_0dB;                                   // Set the IF attenuation to 7.0dB
        break;
      case 15:
        att_bits = LTC5566_ATT_7_5dB;                                   // Set the IF attenuation to 7.5dB
        break;
      case 16:
        att_bits = LTC5566_ATT_8_0dB;                                   // Set the IF attenuation to 8.0dB
        break;
      case 17:
        att_bits = LTC5566_ATT_8_5dB;                                   // Set the IF attenuation to 8.5dB
        break;
      case 18:
        att_bits = LTC5566_ATT_9_0dB;                                   // Set the IF attenuation to 9.0dB
        break;
      case 19:
        att_bits = LTC5566_ATT_9_5dB;                                   // Set the IF attenuation to 9.5dB
        break;
      case 20:
        att_bits = LTC5566_ATT_10_0dB;                                  // Set the IF attenuation to 10.0dB
        break;
      case 21:
        att_bits = LTC5566_ATT_10_5dB;                                  // Set the IF attenuation to 10.5dB
        break;
      case 22:
        att_bits = LTC5566_ATT_11_0dB;                                  // Set the IF attenuation to 11.0dB
        break;
      case 23:
        att_bits = LTC5566_ATT_11_5dB;                                  // Set the IF attenuation to 11.5dB
        break;
      case 24:
        att_bits = LTC5566_ATT_12_0dB;                                  // Set the IF attenuation to 12.0dB
        break;
      case 25:
        att_bits = LTC5566_ATT_12_5dB;                                  // Set the IF attenuation to 12.5dB
        break;
      case 26:
        att_bits = LTC5566_ATT_13_0dB;                                  // Set the IF attenuation to 13.0dB
        break;
      case 27:
        att_bits = LTC5566_ATT_13_5dB;                                  // Set the IF attenuation to 13.5dB
        break;
      case 28:
        att_bits = LTC5566_ATT_14_0dB;                                  // Set the IF attenuation to 14.0dB
        break;
      case 29:
        att_bits = LTC5566_ATT_14_5dB;                                  // Set the IF attenuation to 14.5dB
        break;
      case 30:
        att_bits = LTC5566_ATT_15_0dB;                                  // Set the IF attenuation to 15.0dB
        break;
      case 31:
        att_bits = LTC5566_ATT_15_5dB;                                  // Set the IF attenuation to 15.5dB
        break;
      default:                                                          // User input was wrong, ask again
        Serial.println(F("\n\nIncorrect Option - Choose a number between 0 and 15.5dB in 0.5dB increments\n"));
        keep_looping = 1;
    }
    if (keep_looping == 0)
    {
      return att_bits;                                                  // Return att bits value to the main program
    }
  }
}

// Decode the register value read from the LTC5566
void LTC5566_decode_output(uint8_t output_register)
{
  uint8_t power_bit_read;                                               // Power bit read back from the LTC5566
  String power_setting_read;                                            // Power setting read back from the LTC5566
  uint8_t tune_bits_read;                                               // Tune bits read back from the LTC5566
  String tune_setting_read;                                             // RF Tune setting read back from the LTC5566
  uint8_t att_bits_read;                                                // Att bits read back from the LTC5566
  String att_setting_read;                                              // Attenuation setting read back from the LTC5566

  power_bit_read = (output_register & 0x90) >> 7;                       // Read bit 7 to get power bit back
  switch (power_bit_read)
  {
    case 0:
      power_setting_read = "Full Power";                                // The LTC5566 was in Full Power mode
      break;
    case 1:
      power_setting_read = "Low Power";                                 // The LTC5566 was in Low Power mode
      break;
  }
  Serial.println("Power Setting: " + power_setting_read);               // Print the Power setting to the serial monitor

  tune_bits_read = (output_register & 0x60) >> 5;                       // Read bits 5 and 6 to get RF tune bits back
  switch (tune_bits_read)
  {
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
  Serial.println("RF Input Setting: " + tune_setting_read);             // Print the RF Tune setting to the serial monitor

  att_bits_read = output_register & 0x1F;                               // Read bits 0-4 to get attenuation bits back
  switch (att_bits_read)
  {
    case 0:
      att_setting_read = "0dB";
      break;
    case 1:
      att_setting_read = "0.5dB";
      break;
    case 2:
      att_setting_read = "1dB";
      break;
    case 3:
      att_setting_read = "1.5dB";
      break;
    case 4:
      att_setting_read = "2dB";
      break;
    case 5:
      att_setting_read = "2.5dB";
      break;
    case 6:
      att_setting_read = "3dB";
      break;
    case 7:
      att_setting_read = "3.5dB";
      break;
    case 8:
      att_setting_read = "4dB";
      break;
    case 9:
      att_setting_read = "4.5dB";
      break;
    case 10:
      att_setting_read = "5dB";
      break;
    case 11:
      att_setting_read = "5.5dB";
      break;
    case 12:
      att_setting_read = "6dB";
      break;
    case 13:
      att_setting_read = "6.5dB";
      break;
    case 14:
      att_setting_read = "7dB";
      break;
    case 15:
      att_setting_read = "7.5dB";
      break;
    case 16:
      att_setting_read = "8dB";
      break;
    case 17:
      att_setting_read = "8.5dB";
      break;
    case 18:
      att_setting_read = "9dB";
      break;
    case 19:
      att_setting_read = "9.5dB";
      break;
    case 20:
      att_setting_read = "10dB";
      break;
    case 21:
      att_setting_read = "10.5dB";
      break;
    case 22:
      att_setting_read = "11dB";
      break;
    case 23:
      att_setting_read = "11.5dB";
      break;
    case 24:
      att_setting_read = "12dB";
      break;
    case 25:
      att_setting_read = "12.5dB";
      break;
    case 26:
      att_setting_read = "13dB";
      break;
    case 27:
      att_setting_read = "13.5dB";
      break;
    case 28:
      att_setting_read = "14dB";
      break;
    case 29:
      att_setting_read = "14.5dB";
      break;
    case 30:
      att_setting_read = "15dB";
      break;
    case 31:
      att_setting_read = "15.5dB";
      break;
  }
  Serial.println("Attenuation Setting: " + att_setting_read);           // Print the Attenuation setting to the serial monitor
}
