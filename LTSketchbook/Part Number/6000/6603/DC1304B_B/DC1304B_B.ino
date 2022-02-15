

/*!
  Linear Technology DC1304B-B Demonstration Board.
  DC1304: Dual Matched, High Frequency Lowpass Filters

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

  http://www.analog.com/en/products/amplifiers/adc-drivers/fully-differential-amplifiers/ltc6603.html

  http://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/dc1304b-b.html



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
    @ingroup DC1304
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LT_I2C.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC6603.h"
#include "LTC6903.h"
#include "DC1304.h"
#include <SPI.h>
#include <Wire.h>

// Function Declaration
void print_title();                         // Print the title block
void print_prompt();                        // Prompt the user for an input command
void menu_1_set_filter();					// Set filter parameters based on user requirements
void menu_2_set_gpo();                      // Sets the GPO
void menu_3_shutdown();                     // Shuts down the LTC6603
void menu_4_poweron();                      // Power up the LTC6603

// Global variables
uint8_t filter_gain_settings = LTC6603_GAIN_0dB;
uint8_t filter_lp_settings = LTC6603_LPF0 ;
uint8_t filter_noFunc_settings = LTC6603_NOFUNC0 ;
uint8_t gpio_settings = LTC6603_GPO_LOW ;
uint8_t shdn_settings = LTC6603_SHDN;
float freq_settings = LTC6603_FREQ_MIN;
static uint8_t LTC6903_output_config = LTC6903_CLK_ON_CLK_INV_ON;  //!< Keeps track of output configuration.

#define device_LTC6603
//#define device_LTC6602

LTC6603 device(LTC6603_CS);
LTC6903 clock_device(LTC6903_CS);

#define num_freq_ranges 3

struct frequency_range
{
  float range_lower;
  float range_upper;
} range[num_freq_ranges];




//! Initialize Linduino
void setup()
{
  quikeval_SPI_init();  // Configure the spi port for 4MHz SCK
  Serial.begin(115200); // Initialize the serial port to the PC
  print_title();        // Displays the title
  print_prompt();       // Display user options

#ifdef device_LTC6603
  {
    device.set_multiplier(512, 128, 32);
    range[0] = {20, 130};
    range[1] = {130, 300};
    range[2] = {300, 2125};
  }
#else ifdef device_LTC6602
  {
    device.set_multiplier(600, 300, 100);
    range[0] = {20, 50};
    range[1] = {50, 100};
    range[2] = {100, 680};
  }
#endif
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
        menu_1_set_filter();
        break;
      case 2:
        menu_3_shutdown();
        break;
      case 3:
        menu_4_poweron();
        break;
      case 4:
        menu_5_set_gain();
        break;
      default:
        Serial.println("Incorrect Option");
        break;
    }
    print_prompt();
  }
}

// Function Definitions

int8_t findFreqRange(float freq)
{
  for (int i = 0; i < num_freq_ranges; ++i)
  {
    if (freq >= range[i].range_lower && freq <= range[i].range_upper)
    {
      return i;
    }
  }
  return (-1);
}


void menu_1_set_filter()
{
  float LP_cutoff_f, HP_cutoff_f; // Cut off frequency
  int8_t Freq_range = -1;
  float required_fclk;
  float sampling_f;

  Serial.print(F("\nEnter the required low pass cut off frequency in KHz: "));
  LP_cutoff_f = read_float();
  Serial.print(LP_cutoff_f);

  Freq_range = findFreqRange(LP_cutoff_f);
  if (Freq_range < 0)
  {
    Serial.print(F("\nFrequency not in range"));
    return;
  }

  // set 6903 to Fclk
  required_fclk = LP_cutoff_f * device.get_multiplier(Freq_range);
  Serial.print("\nFclk in KHz: " + String(required_fclk));
  clock_device.set_frequency(required_fclk);

  // set 6603 LPF0, LPF1
  device.set_control_byte(LTC6603_LPF_MASK, device.get_LPF_bits(Freq_range));
  device.print_control_byte();
  if (device.check_device_ready())
  {
    Serial.print(F("\nDevice ready"));
  }
  else
  {
    Serial.print(F("\nDevice not ready"));
  }

#ifdef device_LTC6602
  {
    Serial.print("\n\n1. " + String(required_fclk / 6000) + " KHz");
    Serial.print("\n2. " + String(required_fclk / 3000) + " KHz");
    Serial.print("\n3. " + String(required_fclk / 1000) + " KHz");
    Serial.print("\n4. No HPF");
    Serial.print(F("\nSelect the required high pass cut off frequency: "));

    uint8_t command = read_int();
    // set 6602 HPF0, HPF1
    device.set_control_byte(LTC6602_HPF_MASK, device.get_HPF_bits(command - 1));
    device.print_control_byte();
    if (device.check_device_ready())
    {
      Serial.print(F("\nDevice ready"));
    }
    else
    {
      Serial.print(F("\nDevice not ready"));
    }
    switch (Freq_range)
    {
      case 0:
      case 1: sampling_f = required_fclk / 3; break;
      case 2: sampling_f = required_fclk; break;
    }
    Serial.print(F("\nSampling frequency = "));
    Serial.print(sampling_f);
    Serial.print(F("kHz\n"));
  }
#else ifdef device_LTC6603
  {
    switch (Freq_range)
    {
      case 0: sampling_f = required_fclk / 4; break;
      case 1: sampling_f = required_fclk / 2; break;
      case 2: sampling_f = required_fclk; break;
    }
    Serial.print(F("\nSampling frequency = "));
    Serial.print(sampling_f);
    Serial.print(F("kHz\n"));
  }
#endif
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
        device.set_control_byte(LTC6603_GPO_MASK, LTC6603_GPO_LOW);
        break;
      case 1:
        device.set_control_byte(LTC6603_GPO_MASK, LTC6603_GPO_HIGH);
        break;
      default:
        Serial.println (F("incorrect option"));
        break;
    }
    device.print_control_byte();
  }
}


void menu_3_shutdown()
{
  Serial.print(F("* The LTC6603 is powered down *\n"));
  device.set_control_byte(LTC6603_ONOFF_MASK, LTC6603_SHDN);
  device.print_control_byte();
}


void menu_4_poweron()
{
  Serial.print(F("* The LTC6603 is turning on *\n"));
  device.set_control_byte(LTC6603_ONOFF_MASK, LTC6603_PRUP);
  device.print_control_byte();
}

void menu_5_set_gain()
{
  int16_t user_command; // The user input command

  while (1)
  {
    Serial.print(F("*************************\n\n"));
    Serial.println (F("Gain: 0-0dB, 1-6dB, 2-12dB, 3-24dB"));
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
        device.set_control_byte(LTC6603_GAIN_MASK, LTC6603_GAIN_0dB);
        break;
      case 1:
        device.set_control_byte(LTC6603_GAIN_MASK, LTC6603_GAIN_6dB);
        break;
      case 2:
        device.set_control_byte(LTC6603_GAIN_MASK, LTC6603_GAIN_12dB);
        break;
      case 3:
        device.set_control_byte(LTC6603_GAIN_MASK, LTC6603_GAIN_24dB);
        break;
      default:
        Serial.println (F("incorrect option"));
    }
    device.print_control_byte();
  }
}


//! Prints the title block when program first starts.
void print_title()
{
  (F("\n*****************************************************************\n"));
  Serial.print(F("* DC1304B-B Demonstration Program                               *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to setup LTC6603.               *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu. *updated new prompt since menu 2 was useless
//If want to use this function again, add it in the function case statement above
void print_prompt()
{
  Serial.print(F("\n1-LTC660x: Setup filter\n"));
  Serial.print(F("2-LTC660x: Power Down\n"));
  Serial.print(F("3-LTC660x: Power Up\n"));
  Serial.print(F("4-LTC660x: Set Gain\n"));
  Serial.println();
  Serial.print(F("Enter a command:"));
}

/*
// old prompt
void print_prompt()
{
  Serial.print(F("\n1-LTC660x: Setup filter\n"));
  Serial.print(F("2-LTC660x: Set General Purpose Output\n"));
  Serial.print(F("3-LTC660x: Power Down\n"));
  Serial.print(F("4-LTC660x: Power Up\n"));
  Serial.print(F("5-LTC660x: Set Gain\n"));
  Serial.println();
  Serial.print(F("Enter a command:"));
}*/
