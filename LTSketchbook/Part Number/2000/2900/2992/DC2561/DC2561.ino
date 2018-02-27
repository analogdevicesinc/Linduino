/*!
LTC2992: Dual Wide Range Power Monitor

@verbatim

Setting the Thresholds:
    1. Select the Threshold Configuration from the main menu.
    2. Select the desired Threshold to be changed. Then enter the minimum and maximum
       values.
    3. If any reading exceeds the set threshold, a fault will be generated that can be viewed in
     the Read/Clear Faults Menu


Reading and Clearing a Fault:
    1. Select the Read/Clear Fault option from the main menu.
    2. To read all the faults, select Read Faults option. This will display all the faults that have occured.
  3. To clear all faults, go back to the Read/Clear Faults menu and select Clear Faults option.

NOTE: Due to memory limitations of the Atmega328 processor this sketch shows limited functionality of the LTC2992. Please
      check the datasheet for a picture of the full functionality of the LTC2992.

NOTES
 Setup:
 Set the terminal baud rate to 115200 and select the newline terminator.
 Requires a power supply.
 Refer to demo manual DC2561A.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leGPIOg 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim
http://www.linear.com/product/LTC2992

http://www.linear.com/product/LTC2992#demoboards

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
    @ingroup LTC2992
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC2992.h"
#include <Wire.h>
#include <SPI.h>

// Function Declaration
void print_title();                 // Print the title block
void print_prompt();                // Print the Prompt

int8_t menu_1_continuous_mode(int8_t CTRLA_mode,  int8_t bit_resolution, float scale);
int8_t menu_2_threshold_config(float scale, int8_t bit_resolution);
int8_t menu_3_GPIO_config();
int8_t menu_4_settings(int8_t *CTRLA_mode, int8_t *bit_resolution);
int8_t menu_5_read_clear_faults();

int8_t menu_2_threshold_menu_1_set_power(int8_t bit_resolution);
int8_t menu_2_threshold_menu_2_set_current(int8_t bit_resolution);
int8_t menu_2_threshold_menu_3_set_SENSE(int8_t bit_resolution);
int8_t menu_2_alert_menu_4_set_adin_alerts(float scale, int8_t bit_resolution);
int8_t menu_2_threshold_menu_5_reset_thresholds();

/* void store_alert_settings();        // Store the alert settings to the EEPROM
int8_t restore_alert_settings();    // Read the alert settings from EEPROM */

#define CONTINUOUS_MODE_DISPLAY_DELAY 1000                  //!< The delay between readings in continious mode

// LSB Weights
const float LTC2992_GPIO_8bit_lsb = 8.000E-3;                //!< Typical GPIO lsb weight for 8-bit mode in volts
const float LTC2992_GPIO_12bit_lsb = 0.500E-3;                //!< Typical GPIO lsb weight for 12-bit mode in volts
const float LTC2992_DELTA_SENSE_8bit_lsb = 200.00E-6;        //!< Typical Delta lsb weight for 8-bit mode in volts
const float LTC2992_DELTA_SENSE_12bit_lsb = 12.50E-6;        //!< Typical Delta lsb weight for 12-bit mode in volts
const float LTC2992_SENSE_8bit_lsb = 400.00E-3;                //!< Typical SENSE lsb weight for 8-bit mode in volts
const float LTC2992_SENSE_12bit_lsb = 25.00E-3;                //!< Typical SENSE lsb weight for 12-bit mode in volts
const float LTC2992_Power_8bit_lsb = LTC2992_DELTA_SENSE_8bit_lsb*LTC2992_SENSE_8bit_lsb;     //!< Typical POWER lsb weight for 8-bit mode in V^2
const float LTC2992_Power_12bit_lsb = LTC2992_DELTA_SENSE_12bit_lsb*LTC2992_SENSE_12bit_lsb;     //!< Typical POWER lsb weight for 12-bit mode in V^2

const float resistor = .01;         //!< resistor value on demo board

// Error string
const char ack_error[] = "Error: No Acknowledge. Check I2C Address."; //!< Error message

// Global variables
static int8_t demo_board_connected;        //!< Set to 1 if the board is connected


//! Initialize Linduino
void setup()
{
  char demo_name[] = "DC2561";      // Demo Board Name stored in QuikEval EEPROM

  quikeval_I2C_init();              //! Configure the EEPROM I2C port for 100kHz
  quikeval_I2C_connect();           //! Connects to main I2C port
  Serial.begin(115200);             //! Initialize the serial port to the PC
  print_title();
  demo_board_connected = discover_demo_board(demo_name);
  if (!demo_board_connected)
  {
    Serial.println(F("Demo board not detected, will attempt to proceed"));
    demo_board_connected = true;
  }
  if (demo_board_connected)
  {
    // restore_alert_settings();
    print_prompt();
  }
}

//! Repeats Linduino loop
void loop()
{
  int8_t ack = 0;                               //! I2C acknowledge indicator
  static uint8_t user_command;                  //! The user input command

  static int8_t CTRLA_mode = 0x00;              //! CTRLA Register Setting Default.
  static int8_t bit_resolution = 1;             //! Variable to select ADC Resolution. 1 = 12-bit, 0 = 8-bit. Settable in Settings.

  static float scale = (150/3);                 //! Stores division ratio for resistive divider on GPIO pin.  Configured inside "Settings" menu.

  if (demo_board_connected)                     //! Do nothing if the demo board is not connected
  {
    if (Serial.available())                     //! Do nothing if serial is not available
    {
      user_command = read_int();                //! Read user input command
      if (user_command != 'm')
        Serial.println(user_command);
      Serial.println();
      ack = 0;
      switch (user_command)                     //! Prints the appropriate submenu
      {
        case 1:
          ack |= menu_1_continuous_mode(CTRLA_mode, bit_resolution, scale);  //! Continuous Mode
          break;

        case 2:
          ack |= menu_2_threshold_config(scale, bit_resolution);               //! Configure Threshold Values
          break;

        case 3:
          ack |= menu_3_GPIO_config();                                         //! Toggle GPIO States
          break;

        case 4:
          ack |= menu_4_settings(&CTRLA_mode, &bit_resolution);                 //! Configure ADC Resolution and Shutdown mode in Settings.
          break;

        case 5:
          ack |= menu_5_read_clear_faults();                                    //! Read/Clear Faults
          break;

        default:
          Serial.println(F("Incorrect Option"));
          break;
      }
      if (ack != 0)
        Serial.println(ack_error);
      Serial.print(F("*************************"));
      print_prompt();
    }
  }
}

// Function Definitions

//! Print the title block
void print_title()
{
  Serial.println(F("\n*****************************************************************"));
  Serial.print(F("* DC2561 Demonstration Program                                  *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program communicates with the LTC2992 12-Bit Dual Wide   *\n"));
  Serial.print(F("* Range I2C Power Monitor found on the DC2561 demo board.       *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Print the Prompt
void print_prompt()
{
  Serial.print(F("\n1-Continuous Mode\n"));
  Serial.print(F("2-Threshold Configuration\n"));
  Serial.print(F("3-GPIO Control\n"));
  Serial.print(F("4-Settings\n"));
  Serial.print(F("5-Read/Clear Faults\n\n"));
  Serial.print(F("Enter a command: "));
}

//! Continuous Mode.
int8_t menu_1_continuous_mode(int8_t CTRLA_mode, //!< Set Continious Mode
                              int8_t bit_resolution, //!< Set ADC Resolution
                              float scale) //!< Stores division ratio for resistive divider on GPIO pin.  Configured inside "Settings" menu.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t LTC2992_mode;
  int8_t ack = 0;
  CTRLA_mode = ((CTRLA_mode & LTC2992_CTRLA_MEASUREMENT_MODE_MASK) | (LTC2992_MODE_CONTINUOUS & ~LTC2992_CTRLA_MEASUREMENT_MODE_MASK));
  Serial.println();
  ack |= LTC2992_write(LTC2992_I2C_ADDRESS, LTC2992_CTRLA_REG, LTC2992_mode); //! Sets the LTC2992 to continuous mode
  do
  {

    Serial.print(F("**********************************************************************************************\n\n"));
    Serial.print(F("	Channel 1					   Channel 2\n\n"));
    if (bit_resolution == 1)
    {
      uint32_t power1_code, power2_code, max_power1_code, max_power2_code, min_power1_code, min_power2_code;
      ack |= LTC2992_read_24_bits(LTC2992_I2C_ADDRESS, LTC2992_POWER1_MSB2_REG, & power1_code);
      ack |= LTC2992_read_24_bits(LTC2992_I2C_ADDRESS, LTC2992_POWER2_MSB2_REG, & power2_code);
      ack |= LTC2992_read_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_POWER1_MSB2_REG, & max_power1_code);
      ack |= LTC2992_read_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_POWER2_MSB2_REG, & power2_code);
      ack |= LTC2992_read_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_POWER1_MSB2_REG, & min_power1_code);
      ack |= LTC2992_read_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_POWER2_MSB2_REG, & min_power2_code);

      float power1, power2, max_power1, max_power2, min_power1, min_power2; // Store power results
      power1 = LTC2992_code_to_power(power1_code, resistor, LTC2992_Power_12bit_lsb);
      power2 = LTC2992_code_to_power(power2_code, resistor, LTC2992_Power_12bit_lsb);
      max_power1 = LTC2992_code_to_power(max_power1_code, resistor, LTC2992_Power_12bit_lsb);
      max_power2 = LTC2992_code_to_power(max_power2_code, resistor, LTC2992_Power_12bit_lsb);
      min_power1 = LTC2992_code_to_power(min_power1_code, resistor, LTC2992_Power_12bit_lsb);
      min_power2 = LTC2992_code_to_power(min_power2_code, resistor, LTC2992_Power_12bit_lsb);

      Serial.print(F("      Power 1: "));
      Serial.print(power1, 4);
      Serial.print(F(" W"));

      Serial.print(F("					Power 2: "));
      Serial.print(power2, 4);
      Serial.print(F(" W\n"));

      Serial.print(F("  Max Power 1: "));
      Serial.print(max_power1, 4);
      Serial.print(F(" W"));

      Serial.print(F("				    Max Power 2: "));
      Serial.print(max_power2, 4);
      Serial.print(F(" W\n"));

      Serial.print(F("  Min Power 1: "));
      Serial.print(min_power1, 4);
      Serial.print(F(" W"));

      Serial.print(F("				    Min Power 2: "));
      Serial.print(min_power2, 4);
      Serial.print(F(" W\n"));

      uint16_t current1_code, current2_code, max_current1_code, max_current2_code, min_current1_code, min_current2_code;
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_DELTA_SENSE1_MSB_REG, & current1_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_DELTA1_SENSE_MSB_REG, & max_current1_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_DELTA1_SENSE_MSB_REG, & min_current1_code);

      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_DELTA_SENSE2_MSB_REG, & current2_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_DELTA2_SENSE_MSB_REG, & max_current2_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_DELTA2_SENSE_MSB_REG, & min_current2_code);

      float current1, current2, max_current1, max_current2, min_current1, min_current2;
      current1 = LTC2992_code_to_current(current1_code, resistor, LTC2992_DELTA_SENSE_12bit_lsb);
      max_current1 = LTC2992_code_to_current(max_current1_code, resistor, LTC2992_DELTA_SENSE_12bit_lsb);
      min_current1 = LTC2992_code_to_current(min_current1_code, resistor, LTC2992_DELTA_SENSE_12bit_lsb);

      current2 = LTC2992_code_to_current(current2_code, resistor, LTC2992_DELTA_SENSE_12bit_lsb);
      max_current2 = LTC2992_code_to_current(max_current2_code, resistor, LTC2992_DELTA_SENSE_12bit_lsb);
      min_current2 = LTC2992_code_to_current(min_current2_code, resistor, LTC2992_DELTA_SENSE_12bit_lsb);

      Serial.print(F("\n    Current 1: "));
      Serial.print(current1, 4);
      Serial.print(F(" A"));

      Serial.print(F("                               Current 2: "));
      Serial.print(current2, 4);
      Serial.print(F(" A\n"));

      Serial.print(F("Max Current 1: "));
      Serial.print(max_current1, 4);
      Serial.print(F(" A"));

      Serial.print(F("				  Max Current 2: "));
      Serial.print(max_current2, 4);
      Serial.print(F(" A\n"));

      Serial.print(F("Min Current 1: "));
      Serial.print(min_current1, 4);
      Serial.print(F(" A"));

      Serial.print(F("			          Min Current 2: "));
      Serial.print(min_current2, 4);
      Serial.print(F(" A\n"));

      uint16_t SENSE1_code, max_SENSE1_code, min_SENSE1_code;
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_SENSE1_MSB_REG, & SENSE1_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_SENSE1_MSB_REG, & max_SENSE1_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_SENSE1_MSB_REG, & min_SENSE1_code);

      float SENSE1, max_SENSE1, min_SENSE1;
      SENSE1 = LTC2992_SENSE_code_to_voltage(SENSE1_code, LTC2992_SENSE_12bit_lsb);
      max_SENSE1 = LTC2992_SENSE_code_to_voltage(max_SENSE1_code, LTC2992_SENSE_12bit_lsb);
      min_SENSE1 = LTC2992_SENSE_code_to_voltage(min_SENSE1_code, LTC2992_SENSE_12bit_lsb);

      uint16_t SENSE2_code, max_SENSE2_code, min_SENSE2_code;
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_SENSE2_MSB_REG, & SENSE2_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_SENSE2_MSB_REG, & max_SENSE2_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_SENSE2_MSB_REG, & min_SENSE2_code);

      float SENSE2, max_SENSE2, min_SENSE2;
      SENSE2 = LTC2992_SENSE_code_to_voltage(SENSE2_code, LTC2992_SENSE_12bit_lsb);
      max_SENSE2 = LTC2992_SENSE_code_to_voltage(max_SENSE2_code, LTC2992_SENSE_12bit_lsb);
      min_SENSE2 = LTC2992_SENSE_code_to_voltage(min_SENSE2_code, LTC2992_SENSE_12bit_lsb);

      Serial.print(F("\n      SENSE 1: "));
      Serial.print(SENSE1, 4);
      Serial.print(F(" V"));

      Serial.print(F("			                SENSE 2: "));
      Serial.print(SENSE2, 4);
      Serial.print(F(" V\n"));

      Serial.print(F("  Max SENSE 1: "));
      Serial.print(max_SENSE1, 4);
      Serial.print(F(" V"));

      Serial.print(F("		                    Max SENSE 2: "));
      Serial.print(max_SENSE2, 4);
      Serial.print(F(" V\n"));

      Serial.print(F("  Min SENSE 1: "));
      Serial.print(min_SENSE1, 4);
      Serial.print(F(" V"));

      Serial.print(F("			            Min SENSE 2: "));
      Serial.print(min_SENSE2, 4);
      Serial.print(F(" V\n"));

      uint32_t power_sum_code;
      uint16_t current_sum_code;
      ack |= LTC2992_read_24_bits(LTC2992_I2C_ADDRESS, LTC2992_PSUM_MSB1_REG, & power_sum_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_ISUM_MSB_REG, & current_sum_code);

      float power_sum, current_sum;
      power_sum = LTC2992_code_to_power_sum(power_sum_code, resistor, LTC2992_Power_12bit_lsb);
      current_sum = LTC2992_code_to_current_sum(current_sum_code, resistor, LTC2992_DELTA_SENSE_12bit_lsb);

      Serial.print(F("\n   Power Sum: "));
      Serial.print(power_sum, 4);
      Serial.print(F(" W\n"));

      Serial.print(F(" Current Sum: "));
      Serial.print(current_sum, 4);
      Serial.print(F(" A\n"));

      uint16_t GPIO1_code, max_GPIO1_code, min_GPIO1_code, GPIO2_code, max_GPIO2_code, min_GPIO2_code,
               GPIO3_code, max_GPIO3_code, min_GPIO3_code, GPIO4_code, max_GPIO4_code, min_GPIO4_code;
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_GPIO1_MSB_REG, & GPIO1_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO1_MSB_REG, & max_GPIO1_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO1_MSB_REG, & min_GPIO1_code);

      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_GPIO2_MSB_REG, & GPIO2_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO2_MSB_REG, & max_GPIO2_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO2_MSB_REG, & min_GPIO2_code);

      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_GPIO3_MSB_REG, & GPIO3_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO3_MSB_REG, & max_GPIO3_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO3_MSB_REG, & min_GPIO3_code);

      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_GPIO4_MSB_REG, & GPIO4_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO4_MSB_REG, & max_GPIO4_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO4_MSB_REG, & min_GPIO4_code);

      float GPIO1, max_GPIO1, min_GPIO1, GPIO2, max_GPIO2, min_GPIO2, GPIO3, max_GPIO3, min_GPIO3, GPIO4, max_GPIO4, min_GPIO4;
      GPIO1 = LTC2992_GPIO_code_to_voltage(GPIO1_code, LTC2992_GPIO_12bit_lsb);
      max_GPIO1 = LTC2992_GPIO_code_to_voltage(max_GPIO1_code, LTC2992_GPIO_12bit_lsb);
      min_GPIO1 = LTC2992_GPIO_code_to_voltage(min_GPIO1_code, LTC2992_GPIO_12bit_lsb);

      GPIO2 = LTC2992_GPIO_code_to_voltage(GPIO2_code, LTC2992_GPIO_12bit_lsb);
      max_GPIO2 = LTC2992_GPIO_code_to_voltage(max_GPIO2_code, LTC2992_GPIO_12bit_lsb);
      min_GPIO2 = LTC2992_GPIO_code_to_voltage(min_GPIO2_code, LTC2992_GPIO_12bit_lsb);

      GPIO3 = LTC2992_GPIO_code_to_voltage(GPIO3_code, LTC2992_GPIO_12bit_lsb);
      max_GPIO3 = LTC2992_GPIO_code_to_voltage(max_GPIO3_code, LTC2992_GPIO_12bit_lsb);
      min_GPIO3 = LTC2992_GPIO_code_to_voltage(min_GPIO3_code, LTC2992_GPIO_12bit_lsb);

      GPIO4 = LTC2992_GPIO_code_to_voltage(GPIO4_code, LTC2992_GPIO_12bit_lsb);
      max_GPIO4 = LTC2992_GPIO_code_to_voltage(max_GPIO4_code, LTC2992_GPIO_12bit_lsb);
      min_GPIO4 = LTC2992_GPIO_code_to_voltage(min_GPIO4_code, LTC2992_GPIO_12bit_lsb);

      Serial.print(F("\n    GPIO1: "));
      Serial.print(GPIO1*scale, 4);
      Serial.print(F(" V"));

      Serial.print(F("        GPIO2: "));
      Serial.print(GPIO2*scale, 4);
      Serial.print(F(" V"));

      Serial.print(F("        GPIO3: "));
      Serial.print(GPIO3, 4);
      Serial.print(F(" V"));

      Serial.print(F("        GPIO4: "));
      Serial.print(GPIO4, 4);
      Serial.print(F(" V\n"));

      Serial.print(F("Max GPIO1: "));
      Serial.print(max_GPIO1, 4);
      Serial.print(F(" V"));

      Serial.print(F("    Max GPIO2: "));
      Serial.print(max_GPIO2, 4);
      Serial.print(F(" V"));

      Serial.print(F("    Max GPIO3: "));
      Serial.print(max_GPIO3, 4);
      Serial.print(F(" V"));

      Serial.print(F("    Max GPIO4: "));
      Serial.print(max_GPIO4, 4);
      Serial.print(F(" V\n"));

      Serial.print(F("Min GPIO1: "));
      Serial.print(min_GPIO1, 4);
      Serial.print(F(" V"));

      Serial.print(F("    Min GPIO2: "));
      Serial.print(min_GPIO2, 4);
      Serial.print(F(" V"));

      Serial.print(F("    Min GPIO3: "));
      Serial.print(min_GPIO3, 4);
      Serial.print(F(" V"));

      Serial.print(F("    Min GPIO4: "));
      Serial.print(min_GPIO4, 4);
      Serial.print(F(" V\n"));

      Serial.println();

      Serial.print(F("In 12 - bit mode\n"));


    }
    else
    {



      uint32_t power1_code, power2_code, max_power1_code, max_power2_code, min_power1_code, min_power2_code;
      ack |= LTC2992_read_24_bits(LTC2992_I2C_ADDRESS, LTC2992_POWER1_MSB2_REG, & power1_code);
      ack |= LTC2992_read_24_bits(LTC2992_I2C_ADDRESS, LTC2992_POWER2_MSB2_REG, & power2_code);
      ack |= LTC2992_read_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_POWER1_MSB2_REG, & max_power1_code);
      ack |= LTC2992_read_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_POWER2_MSB2_REG, & power2_code);
      ack |= LTC2992_read_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_POWER1_MSB2_REG, & min_power1_code);
      ack |= LTC2992_read_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_POWER2_MSB2_REG, & min_power2_code);

      float power1, power2, max_power1, max_power2, min_power1, min_power2; // Store power results
      power1 = LTC2992_code_to_power(power1_code>>8, resistor, LTC2992_Power_8bit_lsb);
      power2 = LTC2992_code_to_power(power2_code>>8, resistor, LTC2992_Power_8bit_lsb);
      max_power1 = LTC2992_code_to_power(max_power1_code>>8, resistor, LTC2992_Power_8bit_lsb);
      max_power2 = LTC2992_code_to_power(max_power2_code>>8, resistor, LTC2992_Power_8bit_lsb);
      min_power1 = LTC2992_code_to_power(min_power1_code>>8, resistor, LTC2992_Power_8bit_lsb);
      min_power2 = LTC2992_code_to_power(min_power2_code>>8, resistor, LTC2992_Power_8bit_lsb);

      Serial.print(F("      Power 1: "));
      Serial.print(power1, 4);
      Serial.print(F(" W"));

      Serial.print(F("					Power 2: "));
      Serial.print(power2, 4);
      Serial.print(F(" W\n"));

      Serial.print(F("  Max Power 1: "));
      Serial.print(max_power1, 4);
      Serial.print(F(" W"));

      Serial.print(F("				    Max Power 2: "));
      Serial.print(max_power2, 4);
      Serial.print(F(" W\n"));

      Serial.print(F("  Min Power 1: "));
      Serial.print(min_power1, 4);
      Serial.print(F(" W"));

      Serial.print(F("				    Min Power 2: "));
      Serial.print(min_power2, 4);
      Serial.print(F(" W\n"));

      uint16_t current1_code, current2_code, max_current1_code, max_current2_code, min_current1_code, min_current2_code;
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_DELTA_SENSE1_MSB_REG, & current1_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_DELTA1_SENSE_MSB_REG, & max_current1_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_DELTA1_SENSE_MSB_REG, & min_current1_code);

      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_DELTA_SENSE2_MSB_REG, & current2_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_DELTA2_SENSE_MSB_REG, & max_current2_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_DELTA2_SENSE_MSB_REG, & min_current2_code);

      float current1, current2, max_current1, max_current2, min_current1, min_current2;
      current1 = LTC2992_code_to_current(current1_code>>4, resistor, LTC2992_DELTA_SENSE_8bit_lsb);
      max_current1 = LTC2992_code_to_current(max_current1_code>>4, resistor, LTC2992_DELTA_SENSE_8bit_lsb);
      min_current1 = LTC2992_code_to_current(min_current1_code>>4, resistor, LTC2992_DELTA_SENSE_8bit_lsb);

      current2 = LTC2992_code_to_current(current2_code>>4, resistor, LTC2992_DELTA_SENSE_8bit_lsb);
      max_current2 = LTC2992_code_to_current(max_current2_code>>4, resistor, LTC2992_DELTA_SENSE_8bit_lsb);
      min_current2 = LTC2992_code_to_current(min_current2_code>>4, resistor, LTC2992_DELTA_SENSE_8bit_lsb);

      Serial.print(F("\n    Current 1: "));
      Serial.print(current1, 4);
      Serial.print(F(" A"));

      Serial.print(F("                               Current 2: "));
      Serial.print(current2, 4);
      Serial.print(F(" A\n"));

      Serial.print(F("Max Current 1: "));
      Serial.print(max_current1, 4);
      Serial.print(F(" A"));

      Serial.print(F("				  Max Current 2: "));
      Serial.print(max_current2, 4);
      Serial.print(F(" A\n"));

      Serial.print(F("Min Current 1: "));
      Serial.print(min_current1, 4);
      Serial.print(F(" A"));

      Serial.print(F("			          Min Current 2: "));
      Serial.print(min_current2, 4);
      Serial.print(F(" A\n"));

      uint16_t SENSE1_code, max_SENSE1_code, min_SENSE1_code;
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_SENSE1_MSB_REG, & SENSE1_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_SENSE1_MSB_REG, & max_SENSE1_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_SENSE1_MSB_REG, & min_SENSE1_code);

      float SENSE1, max_SENSE1, min_SENSE1;
      SENSE1 = LTC2992_SENSE_code_to_voltage(SENSE1_code>>4, LTC2992_SENSE_8bit_lsb);
      max_SENSE1 = LTC2992_SENSE_code_to_voltage(max_SENSE1_code>>4, LTC2992_SENSE_8bit_lsb);
      min_SENSE1 = LTC2992_SENSE_code_to_voltage(min_SENSE1_code>>4, LTC2992_SENSE_8bit_lsb);

      uint16_t SENSE2_code, max_SENSE2_code, min_SENSE2_code;
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_SENSE2_MSB_REG, & SENSE2_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_SENSE2_MSB_REG, & max_SENSE2_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_SENSE2_MSB_REG, & min_SENSE2_code);

      float SENSE2, max_SENSE2, min_SENSE2;
      SENSE2 = LTC2992_SENSE_code_to_voltage(SENSE2_code>>4, LTC2992_SENSE_8bit_lsb);
      max_SENSE2 = LTC2992_SENSE_code_to_voltage(max_SENSE2_code>>4, LTC2992_SENSE_8bit_lsb);
      min_SENSE2 = LTC2992_SENSE_code_to_voltage(min_SENSE2_code>>4, LTC2992_SENSE_8bit_lsb);

      Serial.print(F("\n      SENSE 1: "));
      Serial.print(SENSE1, 4);
      Serial.print(F(" V"));

      Serial.print(F("			                SENSE 2: "));
      Serial.print(SENSE2, 4);
      Serial.print(F(" V\n"));

      Serial.print(F("  Max SENSE 1: "));
      Serial.print(max_SENSE1, 4);
      Serial.print(F(" V"));

      Serial.print(F("		                    Max SENSE 2: "));
      Serial.print(max_SENSE2, 4);
      Serial.print(F(" V\n"));

      Serial.print(F("  Min SENSE 1: "));
      Serial.print(min_SENSE1, 4);
      Serial.print(F(" V"));

      Serial.print(F("			            Min SENSE 2: "));
      Serial.print(min_SENSE2, 4);
      Serial.print(F(" V\n"));

      uint32_t power_sum_code;
      uint16_t current_sum_code;
      ack |= LTC2992_read_24_bits(LTC2992_I2C_ADDRESS, LTC2992_PSUM_MSB1_REG, & power_sum_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_ISUM_MSB_REG, & current_sum_code);

      float power_sum, current_sum;
      power_sum = LTC2992_code_to_power_sum(power_sum_code>>8, resistor, LTC2992_Power_8bit_lsb);
      current_sum = LTC2992_code_to_current_sum(current_sum_code>>4, resistor, LTC2992_DELTA_SENSE_8bit_lsb);

      Serial.print(F("\n   Power Sum: "));
      Serial.print(power_sum, 4);
      Serial.print(F(" W\n"));

      Serial.print(F(" Current Sum: "));
      Serial.print(current_sum, 4);
      Serial.print(F(" A\n"));

      uint16_t GPIO1_code, max_GPIO1_code, min_GPIO1_code, GPIO2_code, max_GPIO2_code, min_GPIO2_code,
               GPIO3_code, max_GPIO3_code, min_GPIO3_code, GPIO4_code, max_GPIO4_code, min_GPIO4_code;
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_GPIO1_MSB_REG, & GPIO1_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO1_MSB_REG, & max_GPIO1_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO1_MSB_REG, & min_GPIO1_code);

      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_GPIO2_MSB_REG, & GPIO2_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO2_MSB_REG, & max_GPIO2_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO2_MSB_REG, & min_GPIO2_code);

      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_GPIO3_MSB_REG, & GPIO3_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO3_MSB_REG, & max_GPIO3_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO3_MSB_REG, & min_GPIO3_code);

      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_GPIO4_MSB_REG, & GPIO4_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO4_MSB_REG, & max_GPIO4_code);
      ack |= LTC2992_read_12_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO4_MSB_REG, & min_GPIO4_code);

      float GPIO1, max_GPIO1, min_GPIO1, GPIO2, max_GPIO2, min_GPIO2, GPIO3, max_GPIO3, min_GPIO3, GPIO4, max_GPIO4, min_GPIO4;
      GPIO1 = LTC2992_GPIO_code_to_voltage(GPIO1_code>>4, LTC2992_GPIO_8bit_lsb);
      max_GPIO1 = LTC2992_GPIO_code_to_voltage(max_GPIO1_code>>4, LTC2992_GPIO_8bit_lsb);
      min_GPIO1 = LTC2992_GPIO_code_to_voltage(min_GPIO1_code>>4, LTC2992_GPIO_8bit_lsb);

      GPIO2 = LTC2992_GPIO_code_to_voltage(GPIO2_code>>4, LTC2992_GPIO_8bit_lsb);
      max_GPIO2 = LTC2992_GPIO_code_to_voltage(max_GPIO2_code>>4, LTC2992_GPIO_8bit_lsb);
      min_GPIO2 = LTC2992_GPIO_code_to_voltage(min_GPIO2_code>>4, LTC2992_GPIO_8bit_lsb);

      GPIO3 = LTC2992_GPIO_code_to_voltage(GPIO3_code>>4, LTC2992_GPIO_8bit_lsb);
      max_GPIO3 = LTC2992_GPIO_code_to_voltage(max_GPIO3_code>>4, LTC2992_GPIO_8bit_lsb);
      min_GPIO3 = LTC2992_GPIO_code_to_voltage(min_GPIO3_code>>4, LTC2992_GPIO_8bit_lsb);

      GPIO4 = LTC2992_GPIO_code_to_voltage(GPIO4_code>>4, LTC2992_GPIO_8bit_lsb);
      max_GPIO4 = LTC2992_GPIO_code_to_voltage(max_GPIO4_code>>4, LTC2992_GPIO_8bit_lsb);
      min_GPIO4 = LTC2992_GPIO_code_to_voltage(min_GPIO4_code>>4, LTC2992_GPIO_8bit_lsb);

      Serial.print(F("\n    GPIO1: "));
      Serial.print(GPIO1*scale, 4);
      Serial.print(F(" V"));

      Serial.print(F("        GPIO2: "));
      Serial.print(GPIO2*scale, 4);
      Serial.print(F(" V"));

      Serial.print(F("        GPIO3: "));
      Serial.print(GPIO3, 4);
      Serial.print(F(" V"));

      Serial.print(F("        GPIO4: "));
      Serial.print(GPIO4, 4);
      Serial.print(F(" V\n"));

      Serial.print(F("Max GPIO1: "));
      Serial.print(max_GPIO1, 4);
      Serial.print(F(" V"));

      Serial.print(F("    Max GPIO2: "));
      Serial.print(max_GPIO2, 4);
      Serial.print(F(" V"));

      Serial.print(F("    Max GPIO3: "));
      Serial.print(max_GPIO3, 4);
      Serial.print(F(" V"));

      Serial.print(F("    Max GPIO4: "));
      Serial.print(max_GPIO4, 4);
      Serial.print(F(" V\n"));

      Serial.print(F("Min GPIO1: "));
      Serial.print(min_GPIO1, 4);
      Serial.print(F(" V"));

      Serial.print(F("    Min GPIO2: "));
      Serial.print(min_GPIO2, 4);
      Serial.print(F(" V"));

      Serial.print(F("    Min GPIO3: "));
      Serial.print(min_GPIO3, 4);
      Serial.print(F(" V"));

      Serial.print(F("    Min GPIO4: "));
      Serial.print(min_GPIO4, 4);
      Serial.print(F(" V\n"));

      Serial.println();


      Serial.print(F("In 8 - bit mode\n"));

    }

    Serial.print(F("\n\n**********************************************************************************************\n\n"));

    Serial.print(F("m-Main Menu\n\n"));
    Serial.flush();
    delay(CONTINUOUS_MODE_DISPLAY_DELAY);
  }
  while (Serial.available() == false);
  read_int(); // clears the Serial.available
  return (ack);
}

//! Configure Threshold Values
int8_t menu_2_threshold_config(float scale, //!< Stores division ratio for resistive divider on GPIO pin.  Configured inside "Settings" menu.
                               int8_t bit_resolution) //!< Sets ADC Resolution
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  int8_t user_command;
  do
  {
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Set Power Threshold\n"));
    Serial.print(F("2-Set Current Threshold\n"));
    Serial.print(F("3-Set SENSE Threshold\n"));
    Serial.print(F("4-Set GPIO Threshold\n"));
    Serial.print(F("5-Reset Thresholds To Default\n"));
    Serial.print(F("m-Main Menu\n\n"));
    Serial.print(F("Enter a command: "));

    user_command = read_int();
    if (user_command == 'm')
      Serial.println(F("m"));
    else
      Serial.println(user_command);
    Serial.println();
    switch (user_command)
    {
      case 1:
        ack |= menu_2_threshold_menu_1_set_power(bit_resolution); //! Set Power Min/Max Thresholds
        break;

      case 2:
        ack |= menu_2_threshold_menu_2_set_current(bit_resolution); //! Set Current Min/Max Thresholds
        break;

      case 3:
        ack |= menu_2_threshold_menu_3_set_SENSE(bit_resolution); //! Set Voltage Min/Max Thresholds
        break;

      case 4:
        ack |= menu_2_alert_menu_4_set_adin_alerts(scale, bit_resolution); //! Set GPIO Min/Max Thresholds
        break;
      case 5:
        ack |= menu_2_threshold_menu_5_reset_thresholds(); //! Set All Min/Max Thresholds to Default Value
        break;

      default:
        if (user_command != 'm')
          Serial.println(F("Incorrect Option"));
        break;
    }
  }
  while (!((user_command == 'm') || (ack)));
  return (ack);
}
//! Set Power Min/Max Values
int8_t menu_2_threshold_menu_1_set_power(int8_t bit_resolution) //!< Sets ADC Resolution
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{

  int8_t ack = 0;
  Serial.print(F("Enter Max Channel 1 Power Threshold:"));

  float max_power_threshold;
  max_power_threshold = read_float();
  Serial.println(max_power_threshold, 4);

  uint32_t max_power_threshold_code;
  if (bit_resolution == 1)
  {
    max_power_threshold_code = (max_power_threshold / LTC2992_Power_12bit_lsb) * resistor;
    ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_POWER1_THRESHOLD_MSB2_REG, max_power_threshold_code);
  }
  else
  {
    max_power_threshold_code = (max_power_threshold / LTC2992_Power_8bit_lsb) * resistor;
    ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_POWER1_THRESHOLD_MSB2_REG, max_power_threshold_code<<8);
  }
  Serial.print(F("Enter Min Channel 1 Power Threshold:"));

  float min_power_threshold;
  min_power_threshold = read_float();
  Serial.println(min_power_threshold, 4);
  Serial.println();

  uint32_t min_power_threshold_code;
  if (bit_resolution == 1)
  {
    min_power_threshold_code = (min_power_threshold / LTC2992_Power_12bit_lsb) * resistor;
    ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_POWER1_THRESHOLD_MSB2_REG, min_power_threshold_code);

  }
  else
  {
    min_power_threshold_code = (min_power_threshold / LTC2992_Power_8bit_lsb) * resistor;
    ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_POWER1_THRESHOLD_MSB2_REG, min_power_threshold_code<<8);
  }
  Serial.print(F("Enter Max Channel 2 Power Threshold:"));
  max_power_threshold = read_float();
  Serial.println(max_power_threshold, 4);

  if (bit_resolution == 1)
  {
    max_power_threshold_code = (max_power_threshold / LTC2992_Power_12bit_lsb) * resistor;
    ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_POWER2_THRESHOLD_MSB2_REG, max_power_threshold_code);

  }
  else
  {
    max_power_threshold_code = (max_power_threshold / LTC2992_Power_8bit_lsb) * resistor;
    ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_POWER2_THRESHOLD_MSB2_REG, max_power_threshold_code<<8);
  }
  Serial.print(F("Enter Min Channel 2 Power Threshold:"));

  min_power_threshold = read_float();
  Serial.println(min_power_threshold, 4);
  Serial.println();

  if (bit_resolution == 1)
  {
    min_power_threshold_code = (min_power_threshold / LTC2992_Power_12bit_lsb) * resistor;
    ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_POWER2_THRESHOLD_MSB2_REG, min_power_threshold_code);

  }
  else
  {
    min_power_threshold_code = (min_power_threshold / LTC2992_Power_8bit_lsb) * resistor;
    ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_POWER2_THRESHOLD_MSB2_REG, min_power_threshold_code<<8);
  }
  Serial.print(F("Enter Max Power SUM Threshold:"));
  max_power_threshold = read_float();
  Serial.println(max_power_threshold, 4);

  if (bit_resolution == 1)
  {
    max_power_threshold_code = (max_power_threshold / (2 * LTC2992_Power_12bit_lsb)) * resistor;
    ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_PSUM_THRESHOLD_MSB1_REG, max_power_threshold_code);

  }
  else
  {
    max_power_threshold_code = (max_power_threshold / (2 * LTC2992_Power_8bit_lsb)) * resistor;
    ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_PSUM_THRESHOLD_MSB1_REG, max_power_threshold_code<<8);
  }

  Serial.print(F("Enter Min Power Sum Threshold:"));

  min_power_threshold = read_float();
  Serial.println(min_power_threshold, 4);
  Serial.println();
  if (bit_resolution == 1)
  {
    min_power_threshold_code = (min_power_threshold / (2 * LTC2992_Power_12bit_lsb)) * resistor;
    ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_PSUM_THRESHOLD_MSB1_REG, min_power_threshold_code);
  }
  else
  {
    min_power_threshold_code = (min_power_threshold / (2 * LTC2992_Power_8bit_lsb)) * resistor;
    ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_PSUM_THRESHOLD_MSB1_REG, min_power_threshold_code<<8);
  }

  return (ack);
}

//! Set Min/Max Current Threshold
int8_t menu_2_threshold_menu_2_set_current(int8_t bit_resolution) //!< Sets ADC Resolution
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.

{
  int8_t ack = 0;
  Serial.print(F("Enter Max Channel 1 Current Threshold:"));

  float max_current_threshold;
  max_current_threshold = read_float();
  Serial.println(max_current_threshold, 4);

  uint16_t max_current_threshold_code;
  if (bit_resolution == 1)
  {
    max_current_threshold_code = (max_current_threshold / LTC2992_DELTA_SENSE_12bit_lsb) * resistor;
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_DELTA1_SENSE_THRESHOLD_MSB_REG, (max_current_threshold_code << 4));

  }
  else
  {
    max_current_threshold_code = (max_current_threshold / LTC2992_DELTA_SENSE_8bit_lsb) * resistor;
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_DELTA1_SENSE_THRESHOLD_MSB_REG, (max_current_threshold_code << 8));
  }

  Serial.print(F("Enter Min Channel 1 Current Threshold:"));

  float min_current_threshold;
  min_current_threshold = read_float();
  Serial.println(min_current_threshold, 4);
  Serial.println();

  uint16_t min_current_threshold_code;
  if (bit_resolution == 1)
  {
    min_current_threshold_code = (min_current_threshold / LTC2992_DELTA_SENSE_12bit_lsb) * resistor;
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_DELTA1_SENSE_THRESHOLD_MSB_REG, (min_current_threshold_code << 4));
  }
  else
  {
    min_current_threshold_code = (min_current_threshold / LTC2992_DELTA_SENSE_8bit_lsb) * resistor;
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_DELTA1_SENSE_THRESHOLD_MSB_REG, (min_current_threshold_code << 8));

  }

  Serial.print(F("Enter Max Channel 2 Current Threshold:"));
  max_current_threshold = read_float();
  Serial.println(max_current_threshold, 4);

  if (bit_resolution == 1)
  {
    max_current_threshold_code = (max_current_threshold / LTC2992_DELTA_SENSE_12bit_lsb) * resistor;
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_DELTA2_SENSE_THRESHOLD_MSB_REG, (max_current_threshold_code << 4));
  }
  else
  {
    max_current_threshold_code = (max_current_threshold / LTC2992_DELTA_SENSE_8bit_lsb) * resistor;
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_DELTA2_SENSE_THRESHOLD_MSB_REG, (max_current_threshold_code << 8));
  }
  Serial.print(F("Enter Min Channel 2 Current Threshold:"));

  min_current_threshold = read_float();
  Serial.println(min_current_threshold, 4);
  Serial.println();

  if (bit_resolution == 1)
  {
    min_current_threshold_code = (min_current_threshold / LTC2992_DELTA_SENSE_12bit_lsb) * resistor;
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_DELTA2_SENSE_THRESHOLD_MSB_REG, (min_current_threshold_code << 4));

  }
  else
  {
    min_current_threshold_code = (min_current_threshold / LTC2992_DELTA_SENSE_8bit_lsb) * resistor;
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_DELTA2_SENSE_THRESHOLD_MSB_REG, (min_current_threshold_code << 8));
  }
  Serial.print(F("Enter Max Current Sum Threshold:"));
  max_current_threshold = read_float();
  Serial.println(max_current_threshold, 4);

  if (bit_resolution == 1)
  {
    max_current_threshold_code = (max_current_threshold / (2 * LTC2992_DELTA_SENSE_12bit_lsb)) * resistor;
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_ISUM_THRESHOLD_MSB_REG, (max_current_threshold_code << 4));
  }
  else
  {
    max_current_threshold_code = (max_current_threshold / (2 * LTC2992_DELTA_SENSE_8bit_lsb)) * resistor;
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_ISUM_THRESHOLD_MSB_REG, (max_current_threshold_code << 8));
  }
  Serial.print(F("Enter Min Current Sum Threshold:"));

  min_current_threshold = read_float();
  Serial.println(min_current_threshold, 4);
  Serial.println();
  if (bit_resolution == 1)
  {
    min_current_threshold_code = (min_current_threshold / (2 * LTC2992_DELTA_SENSE_12bit_lsb)) * resistor;
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_ISUM_THRESHOLD_MSB_REG, (min_current_threshold_code << 4));

  }
  else
  {
    min_current_threshold_code = (min_current_threshold / (2 * LTC2992_DELTA_SENSE_8bit_lsb)) * resistor;
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_ISUM_THRESHOLD_MSB_REG, (min_current_threshold_code << 8));
  }

  return (ack);
}

//! Set Min/Max Voltage Thresholds
int8_t menu_2_threshold_menu_3_set_SENSE(int8_t bit_resolution)
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  Serial.print(F("Enter Max Channel 1 SENSE Threshold:"));

  float max_sense_threshold;
  max_sense_threshold = read_float();
  Serial.println(max_sense_threshold, 4);

  uint16_t max_sense_threshold_code;
  if (bit_resolution == 1)
  {
    max_sense_threshold_code = (max_sense_threshold / LTC2992_SENSE_12bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_SENSE1_THRESHOLD_MSB_REG, (max_sense_threshold_code << 4));
  }
  else
  {
    max_sense_threshold_code = (max_sense_threshold / LTC2992_SENSE_8bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_SENSE1_THRESHOLD_MSB_REG, (max_sense_threshold_code << 8));
  }
  Serial.print(F("Enter Min Channel 1 SENSE Threshold:"));

  float min_sense_threshold;
  min_sense_threshold = read_float();
  Serial.println(min_sense_threshold, 4);
  Serial.println();

  uint16_t min_sense_threshold_code;
  if (bit_resolution == 1)
  {
    min_sense_threshold_code = (min_sense_threshold / LTC2992_SENSE_12bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_SENSE1_THRESHOLD_MSB_REG, (min_sense_threshold_code << 4));

  }
  else
  {
    min_sense_threshold_code = (min_sense_threshold / LTC2992_SENSE_8bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_SENSE1_THRESHOLD_MSB_REG, (min_sense_threshold_code << 8));
  }
  Serial.print(F("Enter Max Channel 2 SENSE Threshold:"));
  max_sense_threshold = read_float();
  Serial.println(max_sense_threshold, 4);

  if (bit_resolution == 1)
  {
    max_sense_threshold_code = (max_sense_threshold / LTC2992_SENSE_12bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_SENSE2_THRESHOLD_MSB_REG, (max_sense_threshold_code << 4));
  }
  else
  {
    max_sense_threshold_code = (max_sense_threshold / LTC2992_SENSE_8bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_SENSE2_THRESHOLD_MSB_REG, (max_sense_threshold_code << 8));
  }

  Serial.print(F("Enter Min Channel 2 SENSE Threshold:"));

  min_sense_threshold = read_float();
  Serial.println(min_sense_threshold, 4);
  Serial.println();

  if (bit_resolution == 1)
  {
    min_sense_threshold_code = (min_sense_threshold / LTC2992_SENSE_12bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_SENSE2_THRESHOLD_MSB_REG, (min_sense_threshold_code << 4));
  }
  else
  {
    min_sense_threshold_code = (min_sense_threshold / LTC2992_SENSE_8bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_SENSE2_THRESHOLD_MSB_REG, (min_sense_threshold_code << 8));
  }


  return (ack);
}
//Set Min/Max GPIO Thresholds
int8_t menu_2_alert_menu_4_set_adin_alerts(float scale, //!< Stores division ratio for resistive divider on GPIO pin.  Configured inside "Settings" menu.
    int8_t bit_resolution) //!< Sets ADC Resolution
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  Serial.print(F("Enter Max GPIO1 Threshold (Range: 0V - 100V):"));

  float max_gpio_threshold;
  max_gpio_threshold = read_float();
  Serial.println(max_gpio_threshold, 4);

  uint16_t max_gpio_threshold_code;
  if (bit_resolution == 1)
  {
    max_gpio_threshold_code = (1 / scale) * (max_gpio_threshold / LTC2992_GPIO_12bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO1_THRESHOLD_MSB_REG, (max_gpio_threshold_code << 4));
  }
  else
  {
    max_gpio_threshold_code = (1 / scale) * (max_gpio_threshold / LTC2992_GPIO_8bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO1_THRESHOLD_MSB_REG, (max_gpio_threshold_code << 8));
  }
  Serial.print(F("Enter Min GPIO1 Threshold (Range: 0V - 100V):"));

  float min_gpio_threshold;
  min_gpio_threshold = read_float();
  Serial.println(min_gpio_threshold, 4);
  Serial.println();

  uint16_t min_gpio_threshold_code;
  if (bit_resolution == 1)
  {
    min_gpio_threshold_code = (1 / scale) * (min_gpio_threshold / LTC2992_GPIO_12bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO1_THRESHOLD_MSB_REG, (min_gpio_threshold_code << 4));
  }
  else
  {
    min_gpio_threshold_code = (1 / scale) * (min_gpio_threshold / LTC2992_GPIO_8bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO1_THRESHOLD_MSB_REG, (min_gpio_threshold_code << 8));
  }

  Serial.print(F("Enter Max GPIO2 Threshold (Range: 0V - 100V):"));
  max_gpio_threshold = read_float();
  Serial.println(max_gpio_threshold, 4);

  if (bit_resolution == 1)
  {
    max_gpio_threshold_code = (1 / scale) * (max_gpio_threshold / LTC2992_GPIO_12bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO2_THRESHOLD_MSB_REG, (max_gpio_threshold_code << 4));
  }
  else
  {
    max_gpio_threshold_code = (1 / scale) * (max_gpio_threshold / LTC2992_GPIO_8bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO2_THRESHOLD_MSB_REG, (max_gpio_threshold_code << 8));
  }

  Serial.print(F("Enter Min GPIO2 Threshold (Range: 0V - 100V):"));

  min_gpio_threshold = read_float();
  Serial.println(min_gpio_threshold, 4);
  Serial.println();

  if (bit_resolution == 1)
  {
    min_gpio_threshold_code = (1 / scale) * (min_gpio_threshold / LTC2992_GPIO_12bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO2_THRESHOLD_MSB_REG, (min_gpio_threshold_code << 4));
  }
  else
  {
    min_gpio_threshold_code = (1 / scale) * (min_gpio_threshold / LTC2992_GPIO_8bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO2_THRESHOLD_MSB_REG, (min_gpio_threshold_code << 8));
  }

  Serial.print(F("Enter Max GPIO3 Threshold (Range: 0V - 2.048V):"));
  max_gpio_threshold = read_float();
  Serial.println(max_gpio_threshold, 4);

  if (bit_resolution == 1)
  {
    max_gpio_threshold_code = (max_gpio_threshold / LTC2992_GPIO_12bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO3_THRESHOLD_MSB_REG, (max_gpio_threshold_code << 4));
  }
  else
  {
    max_gpio_threshold_code = (max_gpio_threshold / LTC2992_GPIO_8bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO3_THRESHOLD_MSB_REG, (max_gpio_threshold_code << 8));
  }

  Serial.print(F("Enter Min GPIO3 Threshold (Range: 0V - 2.048V):"));

  min_gpio_threshold = read_float();
  Serial.println(min_gpio_threshold, 4);
  Serial.println();

  if (bit_resolution == 1)
  {
    min_gpio_threshold_code = (min_gpio_threshold / LTC2992_GPIO_12bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO3_THRESHOLD_MSB_REG, (min_gpio_threshold_code << 4));
  }
  else
  {
    min_gpio_threshold_code = (min_gpio_threshold / LTC2992_GPIO_8bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO3_THRESHOLD_MSB_REG, (min_gpio_threshold_code << 8));
  }

  Serial.print(F("Enter Max GPIO4 Threshold (Range: 0V - 2.048V):"));
  max_gpio_threshold = read_float();
  Serial.println(max_gpio_threshold, 4);

  if (bit_resolution == 1)
  {
    max_gpio_threshold_code = (max_gpio_threshold / LTC2992_GPIO_12bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO4_THRESHOLD_MSB_REG, (max_gpio_threshold_code << 4));
  }
  else
  {
    max_gpio_threshold_code = (max_gpio_threshold / LTC2992_GPIO_8bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO4_THRESHOLD_MSB_REG, (max_gpio_threshold_code << 8));
  }

  Serial.print(F("Enter Min GPIO4 Threshold (Range: 0V - 2.048V):"));

  min_gpio_threshold = read_float();
  Serial.println(min_gpio_threshold, 4);
  Serial.println();

  if (bit_resolution == 1)
  {
    min_gpio_threshold_code = (min_gpio_threshold / LTC2992_GPIO_12bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO4_THRESHOLD_MSB_REG, (min_gpio_threshold_code << 4));
  }
  else
  {
    min_gpio_threshold_code = (min_gpio_threshold / LTC2992_GPIO_8bit_lsb);
    ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO4_THRESHOLD_MSB_REG, (min_gpio_threshold_code << 8));
  }


  return (ack);
}
//! Set All Thresholds to Default Values
int8_t menu_2_threshold_menu_5_reset_thresholds()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{

  int8_t ack = 0;

  uint32_t max_32bit_threshold = 0xFFFFFF;
  uint16_t max_16bit_threshold = 0xFFF0;
  uint32_t min_32bit_threshold = 0x000000;
  uint16_t min_16bit_threshold = 0x00000;

  uint32_t max_power_threshold_code = 0;
  uint32_t min_power_threshold_code = 0;

  //Reset Power Thresholds

  ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_POWER1_THRESHOLD_MSB2_REG, max_32bit_threshold);
  ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_POWER1_THRESHOLD_MSB2_REG, min_32bit_threshold);

  ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_POWER2_THRESHOLD_MSB2_REG, max_32bit_threshold);
  ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_POWER2_THRESHOLD_MSB2_REG, min_32bit_threshold);

  ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_PSUM_THRESHOLD_MSB1_REG, max_32bit_threshold);
  ack |= LTC2992_write_24_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_PSUM_THRESHOLD_MSB1_REG, min_32bit_threshold);

  //Reset Current Thresholds

  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_DELTA1_SENSE_THRESHOLD_MSB_REG, max_16bit_threshold);
  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_DELTA1_SENSE_THRESHOLD_MSB_REG, min_16bit_threshold);

  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_DELTA2_SENSE_THRESHOLD_MSB_REG, max_16bit_threshold);
  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_DELTA2_SENSE_THRESHOLD_MSB_REG, min_16bit_threshold);

  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_ISUM_THRESHOLD_MSB_REG, max_16bit_threshold);
  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_ISUM_THRESHOLD_MSB_REG, min_16bit_threshold);

  //Reset Voltage Thresholds
  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_SENSE1_THRESHOLD_MSB_REG, max_16bit_threshold);
  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_SENSE1_THRESHOLD_MSB_REG, min_16bit_threshold);

  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_SENSE2_THRESHOLD_MSB_REG, max_16bit_threshold);
  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_SENSE2_THRESHOLD_MSB_REG, min_16bit_threshold);

  //Reset GPIO Thresholds
  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO1_THRESHOLD_MSB_REG, max_16bit_threshold);
  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO2_THRESHOLD_MSB_REG, max_16bit_threshold);
  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO3_THRESHOLD_MSB_REG, max_16bit_threshold);
  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MAX_GPIO4_THRESHOLD_MSB_REG, max_16bit_threshold);

  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO1_THRESHOLD_MSB_REG, min_16bit_threshold);
  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO2_THRESHOLD_MSB_REG, min_16bit_threshold);
  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO3_THRESHOLD_MSB_REG, min_16bit_threshold);
  ack |= LTC2992_write_16_bits(LTC2992_I2C_ADDRESS, LTC2992_MIN_GPIO4_THRESHOLD_MSB_REG, min_16bit_threshold);

  if (ack == 0)
  {
    Serial.println(F("\nThresholds Reset to Defaults\n"));
  }

  return (ack);
}

//Toggle and Configure GPIO Pins
int8_t menu_3_GPIO_config()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  int8_t user_command;
  uint8_t gpio_io_code;
  uint8_t gpio4_control_code;

  do
  {
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Configure GPIO1\n"));
    Serial.print(F("2-Configure GPIO2\n"));
    Serial.print(F("3-Configure GPIO3\n"));
    Serial.print(F("4-Configure GPIO4\n"));
    Serial.print(F("m-Main Menu\n\n"));
    Serial.print(F("Enter a command: "));

    ack |= LTC2992_read(LTC2992_I2C_ADDRESS, LTC2992_GPIO_IO_CONT_REG, & gpio_io_code);
    ack |= LTC2992_read(LTC2992_I2C_ADDRESS, LTC2992_GPIO4_CFG_REG, & gpio4_control_code);

    user_command = read_int();
    if (user_command == 'm')
      Serial.println(F("m"));
    else
      Serial.println(user_command);
    Serial.println();
    switch (user_command)
    {
      case 1:
        Serial.print(F("1-GPIO1 Pulls Low, 2-GPIO1 High-Z\n"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          gpio_io_code = gpio_io_code | LTC2992_GPIO1_OUT_LOW;
        else
          gpio_io_code = gpio_io_code & LTC2992_GPIO1_OUT_HIGH_Z;

        break;

      case 2:
        Serial.print(F("1-GPIO2 Pulls Low, 2-GPIO2 High-Z\n"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          gpio_io_code = gpio_io_code | LTC2992_GPIO2_OUT_LOW;
        else
          gpio_io_code = gpio_io_code & LTC2992_GPIO2_OUT_HIGH_Z;
        break;

      case 3:
        Serial.print(F("*************************\n\n"));
        Serial.print(F("1-Configure GPIO3 as Data Ready (Latching)\n"));
        Serial.print(F("2-Configure GPIO3 as Data Ready (128us Pulse)\n"));
        Serial.print(F("3-Configure GPIO3 as Data Ready (16us Pulse)\n"));
        Serial.print(F("4-Configure GPIO3 as I/O Pin\n"));
        Serial.print(F("m-Main Menu\n\n"));
        Serial.print(F("Enter a command: "));
        user_command = read_int();
        Serial.println(user_command);
        switch (user_command)
        {
          case 1:
            gpio_io_code = gpio_io_code | LTC2992_GPIO3_CONFIG_LOW_DATARDY;
            break;
          case 2:
            gpio_io_code = gpio_io_code & LTC2992_GPIOCFG_GPIO3_MASK;
            gpio_io_code = gpio_io_code | LTC2992_GPIO3_CONFIG_128_LOW;
            break;
          case 3:
            gpio_io_code = gpio_io_code & LTC2992_GPIOCFG_GPIO3_MASK;
            gpio_io_code = gpio_io_code | LTC2992_GPIO3_CONFIG_16_LOW;
            break;
          case 4:
            gpio_io_code = gpio_io_code & LTC2992_GPIOCFG_GPIO3_MASK;
            gpio_io_code = gpio_io_code | LTC2992_GPIO3_CONFIG_IO;
            Serial.print(F("1-GPIO3 Pulls Low, 2-GPIO3 High-Z\n"));
            user_command = read_int();
            if ((user_command > 2) || (user_command < 0))
              user_command = 2;
            Serial.println(user_command);
            if (user_command == 1)
              gpio_io_code = gpio_io_code | LTC2992_GPIO3_OUT_LOW;
            else
              gpio_io_code = gpio_io_code & LTC2992_GPIO3_OUT_HIGH_Z;
            break;
          default:
            break;
        }
        break;
      case 4:
        Serial.print(F("1-GPIO4 Pulls Low, 2-GPIO1 High-Z\n"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          gpio4_control_code = gpio4_control_code | LTC2992_GPIO4_OUT_LOW;
        else
          gpio4_control_code = gpio4_control_code & LTC2992_GPIO4_OUT_HI_Z;
        break;
      default:
        if (user_command != 'm')
          Serial.println(F("Incorrect Option"));
        break;
    }
    ack |= LTC2992_write(LTC2992_I2C_ADDRESS, LTC2992_GPIO_IO_CONT_REG, gpio_io_code);
    ack |= LTC2992_write(LTC2992_I2C_ADDRESS, LTC2992_GPIO4_CFG_REG, gpio4_control_code);
  }
  while (!((user_command == 'm') || (ack)));

  return (ack);
}
//! Read/Clear Faults
int8_t menu_5_read_clear_faults()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{

  int8_t ack = 0;
  int8_t user_command;
  uint8_t fault_code;
  uint8_t no_fault_code = 0x00;

  do
  {
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Real All Faults\n"));
    Serial.print(F("2-Clear All Faults\n"));
    Serial.print(F("m-Main Menu\n\n"));
    Serial.print(F("Enter a command: "));

    user_command = read_int();
    if (user_command == 'm')
      Serial.println(F("m"));
    else
      Serial.println(user_command);
    Serial.println();
    switch (user_command)
    {
      case 1:
        ack |= LTC2992_read(LTC2992_I2C_ADDRESS, LTC2992_FAULT1_REG, & fault_code);

        if ((fault_code & (1 << 7)) != 0)
          Serial.println(F("Max Channel 1 Power Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 6)) != 0)
          Serial.println(F("Min Channel 1 Power Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 5)) != 0)
          Serial.println(F("Max Channel 1 Current Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 4)) != 0)
          Serial.println(F("Min Channel 1 Current Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 3)) != 0)
          Serial.println(F("Max Channel 1 Voltage Fault Occured!"));
        else
          no_fault_code++;;
        if ((fault_code & (1 << 2)) != 0)
          Serial.println(F("Min Channel 1 Voltage Fault Occured!"));
        else
          no_fault_code++;;
        if ((fault_code & (1 << 1)) != 0)
          Serial.println(F("Max GPIO1 Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 0)) != 0)
          Serial.println(F("Min GPIO1 Fault Occured!"));
        else
          no_fault_code++;

        ack |= LTC2992_read(LTC2992_I2C_ADDRESS, LTC2992_FAULT2_REG, & fault_code);

        if ((fault_code & (1 << 7)) != 0)
          Serial.println(F("Max Channel 2 Power Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 6)) != 0)
          Serial.println(F("Min Channel 2 Power Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 5)) != 0)
          Serial.println(F("Max Channel 2 Current Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 4)) != 0)
          Serial.println(F("Min Channel 2 Current Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 3)) != 0)
          Serial.println(F("Max Channel 2 Voltage Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 2)) != 0)
          Serial.println(F("Min Channel 2 Voltage Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 1)) != 0)
          Serial.println(F("Max GPIO2 Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 0)) != 0)
          Serial.println(F("Min GPIO2 Fault Occured!"));
        else
          no_fault_code++;

        ack |= LTC2992_read(LTC2992_I2C_ADDRESS, LTC2992_FAULT3_REG, & fault_code);

        if ((fault_code & (1 << 7)) != 0)
          Serial.println(F("Max GPIO3 Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 6)) != 0)
          Serial.println(F("Min GPIO3 Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 5)) != 0)
          Serial.println(F("Max GPIO4 Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 4)) != 0)
          Serial.println(F("Min GPIO4 Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 3)) != 0)
          Serial.println(F("Max Current SUM Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 2)) != 0)
          Serial.println(F("Min Current SUM Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 1)) != 0)
          Serial.println(F("Max Power SUM Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 0)) != 0)
          Serial.println(F("Min Power SUM Fault Occured!"));
        else
          no_fault_code++;

        ack |= LTC2992_read(LTC2992_I2C_ADDRESS, LTC2992_FAULT4_REG, & fault_code);

        if ((fault_code & (1 << 4)) != 0)
          Serial.println(F("Stuck Bus Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 3)) != 0)
          Serial.println(F("GPIO1 Input Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 2)) != 0)
          Serial.println(F("GPIO2 Input Fault Occured!"));
        else
          no_fault_code++;
        if ((fault_code & (1 << 1)) != 0)
          Serial.println(F("GPIO3 Input Fault Occured!"));
        else
          no_fault_code++;

        if (no_fault_code == 28)
        {
          Serial.println(F("No Faults Occured\n"));
          Serial.println();
        }

        break;

      case 2:
        fault_code = 0;
        ack |= LTC2992_write(LTC2992_I2C_ADDRESS, LTC2992_FAULT1_REG, fault_code);
        ack |= LTC2992_write(LTC2992_I2C_ADDRESS, LTC2992_FAULT2_REG, fault_code);
        ack |= LTC2992_write(LTC2992_I2C_ADDRESS, LTC2992_FAULT3_REG, fault_code);
        ack |= LTC2992_write(LTC2992_I2C_ADDRESS, LTC2992_FAULT4_REG, fault_code);

        if (ack == 0)
        {
          Serial.println(F("\nAll Faults Cleared\n"));
        }
        break;

      default:
        if (user_command != 'm')
          Serial.println(F("Incorrect Option"));
        break;
    }
  }
  while (!((user_command == 'm') || (ack)));

  return (ack);

}
//! Settings to configure shutdown mode and ADC Resolution
int8_t menu_4_settings(int8_t *CTRLA_mode,  //! CTRLA Register. Used to set device in Shutdown
                       int8_t *bit_resolution)  //! Sets ADC Bit Resolution.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  int8_t user_command;

  do
  {
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Configure ADC Resolution\n"));
    Serial.print(F("2-Enable/Disable Shutdown Mode\n"));
    Serial.print(F("m-Main Menu\n\n"));
    Serial.print(F("Enter a command: "));

    user_command = read_int();
    if (user_command == 'm')
      Serial.println(F("m"));
    else
      Serial.println(user_command);
    Serial.println();
    switch (user_command)
    {
      case 1:
        Serial.print(F("1-12-bit resolution, 2-8-bit resolution\n"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
        {
          * bit_resolution = 1;
          ack |= LTC2992_write(LTC2992_I2C_ADDRESS, LTC2992_NADC_REG, 0x00);
        }
        else
        {
          *bit_resolution = 0;
          ack |= LTC2992_write(LTC2992_I2C_ADDRESS, LTC2992_NADC_REG, LTC2992_ADC_RESOLUTION);
        }
        uint8_t nadc_code;
        LTC2992_read(LTC2992_I2C_ADDRESS,LTC2992_NADC_REG, &nadc_code);
        Serial.println(nadc_code);
        break;

      case 2:
        Serial.print(F("1-Enable Shutdown, 2-Disable Shutdown\n"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
        {
          * CTRLA_mode = * CTRLA_mode | LTC2992_MODE_SHUTDOWN;
          Serial.print(F("Shutdown Mode is enabled. Selecting Continious Mode in main menu will disable Shutdown mode\n"));
        }
        else
        {
          * CTRLA_mode = ( * CTRLA_mode & LTC2992_CTRLA_MEASUREMENT_MODE_MASK) | LTC2992_MODE_CONTINUOUS;
          Serial.print(F("Shutdown Mode is disabled. \n"));
        }
        ack |= LTC2992_write(LTC2992_I2C_ADDRESS, LTC2992_CTRLA_REG, *CTRLA_mode);
        break;
      default:
        if (user_command != 'm')
          Serial.println(F("Incorrect Option"));
        break;
    }
  }
  while (!((user_command == 'm') || (ack)));
  return (ack);

}

