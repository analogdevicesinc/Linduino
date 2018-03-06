/*!
Linear Technology DC1410A-A Demonstration Board.
LTC2498: 24-Bit, 16-Channel Delta Sigma ADC with SPI interface

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. Equipment
   required is a precision voltage source and a precision voltmeter. Additionally,
   an external power supply is required to provide a negative voltage for Amp V-.
   Set it to anywhere from -1V to -5V. Set Amp V+ to Vcc. Ensure the COM and REF-
   pins are connected to ground. The REF+ pin should be connected to +5V.

  How Measure a Load Cell:
   The LTC2498 allowas a wide common mode range of 0V to Vcc. The LT1678 OPAMP common
   mode range is V- + 1.5 to V+ - 0.8V. Ensure the load cell sensor meets the requirments
   of the ADC and amplifier. After, set the desired gain and read in differential mode.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2498

http://www.linear.com/product/LTC2498#demoboards


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
    @ingroup LTC2498
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include <SPI.h>
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC24XX_general.h"
#include "LTC2498.h"
#include "LTC24XX_general.h"

// Function Declaration
void print_title();                             // Print the title block
void print_prompt();                            // Prompt the user for an input command
void print_user_command(uint8_t menu);          // Display selected differential channels

uint8_t menu_1_read_differential();             // Read the ADC in differential mode
void set_gain(uint8_t a_pin, uint8_t b_pin, uint8_t gain);  // Drive the select pins to get desired gain
void init_gain_pins(uint8_t a_pin, uint8_t b_pin);          // Initialize the MUX select pins
void menu_2_set_1X2X();                         // Enables and disables 2X mode
void menu_3_set_channel_gain();                 // Sets gains for indvidual channels
void print_gain(uint8_t gain);                  // Displays the current channel gain setting

enum Av {unity, g8, g16, g32};                  // Gain options

// Global variables
static uint8_t demo_board_connected;                    //!< Set to 1 if the board is connected
static uint8_t two_x_mode = LTC2498_SPEED_1X;           //!< The LTC2498 2X Mode settings
static uint8_t rejection_mode = LTC2498_R50_R60;        //!< The LTC2498 rejection mode settings
static float LTC2498_vref = 5.0;                        //!< The LTC2498 ideal reference voltage
static uint16_t eoc_timeout = 250;                      //!< Timeout in ms
uint8_t filt_flag = 1;                                  //!< Keeps track when the filter needs to be rest
uint8_t channel_gain[6] = {g32, g32, g32, g32, g32, g32}; //!< default is gain 32

// Constants
//! Lookup table for reads
const uint8_t read_command_seq[] = {  LTC2498_P12_N13,
                                      LTC2498_P14_N15, LTC2498_P14_N15, LTC2498_P14_N15, LTC2498_P14_N15,
                                      LTC2498_P0_N1, LTC2498_P2_N3, LTC2498_P4_N5, LTC2498_P6_N7,
                                      LTC2498_P8_N9, LTC2498_P10_N11, LTC2498_P10_N11
                                   }; //!< Channel read sequence for the demo board
//! lookup table for gain setting reads
uint8_t read_gain_seq[] = {unity, unity, unity , g8, g16, g32, channel_gain[0], channel_gain[1], channel_gain[2], channel_gain[3],
                           channel_gain[4], channel_gain[5]
                          }; //!< The gain sequence for the auto calibration

//! Gain MUX pins
const uint8_t A_PIN = 4;
const uint8_t B_PIN = 7;

//! Exponential filter smothing constant
const float smoothing_const = .7;

//! Initialize Linduino
void setup()
{
  init_gain_pins(A_PIN, B_PIN);

  char demo_name[] = "DC1410";  // Demo Board Name stored in QuikEval EEPROM
  quikeval_SPI_init();          // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();       // Connect SPI to main data port
  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz
  Serial.begin(115200);         // Initialize the serial port to the PC
  print_title();

  demo_board_connected = discover_demo_board(demo_name);
  if (demo_board_connected)
  {
    print_prompt();
  }
  else
  {
    Serial.println(F("EEPROM not detected, will attempt to proceed"));
    print_prompt();
  }
  quikeval_SPI_connect();       // Initialize for SPI
}

//! Repeats Linduino loop
void loop()
{
  int16_t user_command;               // The user input command
  uint8_t ack_EOC = 0;                // Keeps track of the EOC timeout
  if (Serial.available())             // Check for user input
  {
    user_command = read_int();        // Read the user command
    if (user_command != 'm')
      Serial.println(user_command);   // Prints the user command to com port
    Serial.flush();
    switch (user_command)
    {
      case 1:
        ack_EOC |= menu_1_read_differential();
        break;
      case 2:
        menu_2_set_1X2X();
        break;
      case 3:
        menu_3_set_channel_gain();
        break;
      default:
        Serial.println(F("Incorrect Option"));
    }
    if (ack_EOC)
      Serial.println(F("\n******SPI ERROR******\n"));
    Serial.print(F("\n*************************\n"));
    print_prompt();
  }
}

// Function Definitions

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC1410A-A Demonstration Program                               *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to measure strain gauge or      *\n"));
  Serial.print(F("* other form of a Wheatstone bridge sensors with the LTC2498    *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("\n1-Read Differential\n"));
  Serial.print(F("2-2X Mode Settings\n"));
  Serial.print(F("3-Set Channel Gain\n"));
  Serial.print(F("Enter a Command: "));
}

//! Read channels in differential mode
//! @return Returns 0=successful, 1=unsuccessful (exceeded timeout)
uint8_t menu_1_read_differential()
{
  int8_t y;                 // Offset into differential channel array to select polarity
  uint8_t adc_command_high; // The LTC2498 command high byte
  uint8_t adc_command_low;  // The LTC2498 command low byte
  int16_t user_command;     // The user input command
  int32_t adc_code[12];     // The LTC2498 code
  float adc_voltage;        // The LTC2498 voltage
  static float offset = 0.0;
  static float gain_unity = 0.0;
  static float gain_8 = 0.0;
  static float gain_16 = 0.0;
  static float gain_32 = 0.0;
  static float gain_value[4];

  adc_command_low = rejection_mode | two_x_mode;
  do
  {
    for (int8_t i = 0; i <= 11; i++)
    {
      set_gain(A_PIN, B_PIN, read_gain_seq[i]); // Set the MUX to the desired gain
      adc_command_high = read_command_seq[i];   // Set channel
      // Ensure a conversions are complete
      if (LTC2498_EOC_timeout(LTC2498_CS, eoc_timeout))
        return 1;
      // Read the LTC2498
      LTC2498_read(LTC2498_CS, adc_command_high, adc_command_low, adc_code + i);
    }
    if (filt_flag)
    {
      // Reset the filter
      filt_flag = 0;
      offset = adc_code[1] - 536870912;
      gain_unity = ((float)adc_code[2]) - 536870912;
      gain_8 = ((float)adc_code[3] - 536870912) / gain_unity;
      gain_16 = ((float)adc_code[4] - 536870912) / gain_unity;
      gain_32 = ((float)adc_code[5] - 536870912) / gain_unity;
    }
    else
    {
      // IIR exponential filter
      offset = (1.0 - smoothing_const) * ((float)adc_code[1] - 536870912) + smoothing_const * offset;

      if (two_x_mode)
      {
        gain_unity = ((float)adc_code[2]) - offset - 536870912;
        gain_value[1] = ((float)adc_code[3]  - offset - 536870912) / gain_unity;
        gain_16 = ((float)adc_code[4] - offset - 536870912) / gain_unity;
        gain_32 = ((float)adc_code[5] - offset - 536870912) / gain_unity;
      }
      else
      {
        gain_unity = ((float)adc_code[2]) - 536870912;
        gain_8 = (1.0 - smoothing_const) * (((float)adc_code[3] - 536870912) / gain_unity) + smoothing_const * gain_8;
        gain_16 = (1.0 - smoothing_const) * (((float)adc_code[4] - 536870912) / gain_unity) + smoothing_const * gain_16;
        gain_32 = (1.0 - smoothing_const) * (((float)adc_code[5] - 536870912) / gain_unity) + smoothing_const * gain_32;
      }
    }

    gain_value[0] = 1; // Assume ideal unity gain
    gain_value[1] = gain_8;
    gain_value[2] = gain_16;
    gain_value[3] = gain_32;

    Serial.println(F("\n**********"));
    Serial.print(F("  Offset: "));
    Serial.print(offset, 6);
    Serial.print(F("  "));
    Serial.print(LTC2498_code_to_voltage((int32_t)offset + 536870912, LTC2498_vref) * 1000, 4);
    Serial.println(F(" mV"));
    Serial.print(F("  G8: "));
    Serial.println(gain_8, 6);
    Serial.print(F("  G16: "));
    Serial.println(gain_16, 6);
    Serial.print(F("  G32: "));
    Serial.println(gain_32, 6);
    Serial.println();

    Serial.println(F("CH       ADC Code    Gain      Comp Voltage (mV)"));
    Serial.print(F("P0-N1:   0x"));
    Serial.print(adc_code[6], HEX);
    Serial.print("  ");
    Serial.print(gain_value[channel_gain[0]], 6);
    Serial.print(F("  "));
    Serial.println(1000 * LTC24XX_diff_code_to_calibrated_voltage(adc_code[6], LTC2498_vref / 536870911, offset) / gain_value[channel_gain[0]], 6);

    Serial.print(F("P2-N3:   0x"));
    Serial.print(adc_code[7], HEX);
    Serial.print(F("  "));
    Serial.print(gain_value[channel_gain[1]], 6);
    Serial.print("  ");
    Serial.println(1000 * LTC24XX_diff_code_to_calibrated_voltage(adc_code[7], LTC2498_vref / 536870911, offset) / gain_value[channel_gain[1]], 6);

    Serial.print(F("P4-N5:   0x"));
    Serial.print(adc_code[8], HEX);
    Serial.print(F("  "));
    Serial.print(gain_value[channel_gain[2]], 6);
    Serial.print("  ");
    Serial.println(1000 * LTC24XX_diff_code_to_calibrated_voltage(adc_code[8], LTC2498_vref / 536870911, offset) / gain_value[channel_gain[2]], 6);

    Serial.print(F("P6-N7:   0x"));
    Serial.print(adc_code[9], HEX);
    Serial.print(F("  "));
    Serial.print(gain_value[channel_gain[3]], 6);
    Serial.print("  ");
    Serial.println(1000 * LTC24XX_diff_code_to_calibrated_voltage(adc_code[9], LTC2498_vref / 536870911, offset) / gain_value[channel_gain[3]], 6);

    Serial.print(F("P8-N9:   0x"));
    Serial.print(adc_code[10], HEX);
    Serial.print(F("  "));
    Serial.print(gain_value[channel_gain[4]], 6);
    Serial.print("  ");
    Serial.println(1000 * LTC24XX_diff_code_to_calibrated_voltage(adc_code[10], LTC2498_vref / 536870911, offset) / gain_value[channel_gain[4]], 6);

    Serial.print(F("P10-N11: 0x"));
    Serial.print(adc_code[11], HEX);
    Serial.print(F("  "));
    Serial.print(gain_value[channel_gain[5]], 6);
    Serial.print("  ");
    Serial.println(1000 * LTC24XX_diff_code_to_calibrated_voltage(adc_code[11], LTC2498_vref / 536870911, offset) / gain_value[channel_gain[5]], 6);

    Serial.println();
    Serial.println(F("Enter any character to exit"));

  }
  while (Serial.available() == 0);
  while (Serial.available())
    Serial.read();
  Serial.flush();
  return 0;
}

//! Set the Mux select pins to the desired gain
void set_gain(uint8_t a_pin, uint8_t b_pin, uint8_t gain)
{
  digitalWrite(a_pin, gain & 0b01);
  digitalWrite(b_pin, gain & 0b10);
}

//! initialize the MUX select pins
void init_gain_pins(uint8_t a_pin, uint8_t b_pin)
{
  pinMode(a_pin, OUTPUT);
  pinMode(b_pin, OUTPUT);
  digitalWrite(a_pin, LOW);
  digitalWrite(b_pin, LOW);
}

//! Allows the user to sets the desired gain
void menu_3_set_channel_gain()
{
  int16_t user_command; // The user input cammand
  while (1)
  {
    // Display gain settings
    Serial.print(F("\nCurrent Gain Settings:\n"));
    Serial.print(F("  CH 0P-1N   gain: "));
    print_gain(channel_gain[0]);
    Serial.print(F("  CH 2P-3N   gain: "));
    print_gain(channel_gain[1]);
    Serial.print(F("  CH 4P-5N   gain: "));
    print_gain(channel_gain[2]);
    Serial.print(F("  CH 6P-7N   gain: "));
    print_gain(channel_gain[3]);
    Serial.print(F("  CH 8P-9N   gain: "));
    print_gain(channel_gain[4]);
    Serial.print(F("  CH 10P-11N gain: "));
    print_gain(channel_gain[5]);

    Serial.print(F("\n\n"));
    Serial.print(F("Channel Select\n"));
    Serial.print(F("  0-0P-1N     3-6P-7N\n"));
    Serial.print(F("  1-2P-3N     4-8P-9N\n"));
    Serial.print(F("  2-4P-5N     5-10P-11N\n"));
    Serial.print(F("  6-ALL\n"));
    Serial.print(F("m-Main Menu\n"));
    Serial.print(F("Enter a Command: "));

    user_command = read_int();          // Read the single command
    uint8_t channel = user_command;
    if (user_command == 'm')
    {
      Serial.println(F("m"));
      break;
    }
    Serial.println(user_command);
    if (user_command > 6)
      Serial.println(F("Incorrect Option"));
    else
    {
      if (user_command == 6)
      {
        Serial.print(F("\nGain Options\n"));
        Serial.print(F("  0-Unity Gain\n"));
        Serial.print(F("  1-Gain of 8\n"));
        Serial.print(F("  2-Gain of 16\n"));
        Serial.print(F("  3-Gain of 32\n"));
        Serial.print(F("Enter a Command: "));
        user_command = read_int();        // Read the single command
        Serial.println(user_command);
        if (user_command < 0 || user_command > 3)
          Serial.print(F("Incorrect Gain Option"));
        else
        {
          for (uint8_t i = 0; i <= 5; i++)
          {
            channel_gain[i] = user_command;
          }
        }
      }
      else
      {
        Serial.print(F("\nGain Options\n"));
        Serial.print(F("  0-Unity Gain\n"));
        Serial.print(F("  1-Gain of 8\n"));
        Serial.print(F("  2-Gain of 16\n"));
        Serial.print(F("  3-Gain of 32\n"));
        Serial.print(F("Enter a Command: "));
        user_command = read_int();        // Read the single command
        Serial.println(user_command);
        if (user_command < 0 || user_command > 3)
          Serial.print(F("Incorrect Gain Option"));
        else
          channel_gain[channel] = user_command;
      }
    }
  }
  for (int8_t i = 0; i <= 5; i++)
    read_gain_seq[i + 6] = channel_gain[i];
  filt_flag = 1;
}

//! Utility function to display the gain
void print_gain(uint8_t gain)
{
  switch (gain)
  {
    case unity:
      Serial.print(F("Unity\n"));
      break;
    case g8:
      Serial.print(F("8\n"));
      break;
    case g16:
      Serial.print(F("16\n"));
      break;
    case g32:
      Serial.print(F("32\n"));
      break;
  }
}

//! Set 1X or 2X mode
void menu_2_set_1X2X()
{
  int16_t user_command; // The user input command

  // 2X Mode
  Serial.print(F("2X Mode Settings\n\n"));
  Serial.print(F("0-Disable\n"));
  Serial.print(F("1-Enable\n"));
  Serial.print(F("Enter a Command: "));
  user_command = read_int();
  Serial.println(user_command);

  if (user_command == 0)
  {
    two_x_mode = LTC2498_SPEED_1X;
    Serial.print(F("2X Mode Disabled\n"));
  }
  else
  {
    two_x_mode = LTC2498_SPEED_2X;
    Serial.print(F("2X Mode Enabled\n"));
  }
  filt_flag = 1;
}
