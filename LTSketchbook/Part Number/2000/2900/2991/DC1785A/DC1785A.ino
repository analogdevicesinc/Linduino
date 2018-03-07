/*!
Linear Technology DC1785 Demonstration Board.
LTC2991: 14-bit ADC Octal I2C Voltage, Current, and Temperature monitor.

@verbatim

 Setup:
    Set the terminal baud rate to 115200 and select the newline terminator.
    A precision voltage source (preferably low-noise) may be used to apply a voltage
    to input terminals V1-V8. A precision voltmeter may be used to verify applied
    voltages. An oscilloscope may be used to view the PWM output. Ensure JP5, JP6
    and JP7 are in the LOW position. Refer to Demo Manual DC1785A

  Explanation of Commands:

    1 - Single-Ended Voltage - Selects the Single-Ended Voltage Menu.
             1-8: Displays the measured single-ended voltage at one of the V1-V8
                  inputs. When measuring V1 and V8, ensure jumpers are set to VOLT
                  position.
               9: Vcc - Displays the measured Vcc voltage.
              10: ALL - Displays the measured voltages at all of the V1-V8 inputs
                  and Vcc.

    2 - Differential Voltage - Selects the Differential Voltage Menu.
        Maximum full scale differential voltage is 0.300V.

             1-4: Displays the measured differential voltage across one of the V1-V8
                  input pairs. The input common-mode range is 0V to Vcc.  It is
                  easiest to ground the lower input. When measuring V1 and V8,
                  ensure jumpers are set to VOLT position.
               5: ALL - Displays the measured differential voltages at all terminals.

    3 - Temperature - Selects the Temperature Menu
        To measure temperature using onboard transistors, set JP1, JP2, JP3 and JP4
        to TEMP position.
               1: V1-V2 - Measure temperature of Q1 (mounted to demo board) when JP1
                  and JP2 are in TEMP position.
               2: V3-V4 - Measure temperature of external transistor connected to V3
                  and V4 terminals.
               3: V5-V6 - Measure temperature of external transistor connected to V5
                  and V6 terminals.
               4: V7-V8 - Measure temperature of Q2 (mounted to demo board) when JP3
                  and JP4 are in TEMP position.
               5: Internal - Measure temperature using the internal temperature
                  sensor.
               6: All - Displays temperatures at all connections as well as the
                  internal temperature sensor.

    4 - Settings - Selects the Settings Menu
        Enable/disable the on-chip digital filters.  Also toggle temperature units
        between degrees Celsius or degrees Kelvin.
             1-5: Entering these one can enable/disable various channel filters.
               6: Toggle temperature units between degrees Celsius and degrees
                  Kelvin.

    5 - PWM - Selects the PWM Menu
        Generates Proportional PWM output using remote diode connected to pins 7 and
        8. When JP3 and JP4 are in TEMP positions, Q2 is used as the temperature
        sensing diode (mounted on demo board).
               1: Set Threshold Temperature - Enter temperature corresponding to 0%
                  (100% in Inverted Mode) duty cycle.
               2: Inverted/Non-Inverted PWM - Toggles between Inverted/Non-Inverted
                  modes
               3: Enable/Disable - Enables or Disables PWM (Toggle)

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2991

http://www.linear.com/product/LTC2991#demoboards


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
    @ingroup LTC2991
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC2991.h"
#include <Wire.h>
#include <SPI.h>

// Function Declaration
void print_title();                     // Print the title block
void print_prompt();                    // Prompt the user for an input command

int8_t menu_1_single_ended_voltage();   //Sub-menu functions
int8_t menu_2_read_differential_voltage();
int8_t menu_3_read_temperature();
int8_t menu_4_settings();
int8_t menu_5_pwm_options();

// Global variables
static uint8_t demo_board_connected;   //!< Set to 1 if the board is connected

const uint16_t LTC2991_TIMEOUT=1000;  //!< Configures the maximum timeout allowed for an LTC2991 read.

//! Initialize Linduino
void setup()
{
  char demo_name[] = "DC1785";  // Demo Board Name stored in QuikEval EEPROM
  int8_t ack=0;
  quikeval_I2C_init();          //! Initializes Linduino I2C port.
  quikeval_I2C_connect();       //! Connects I2C port to the QuikEval connector
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();
  demo_board_connected = discover_demo_board(demo_name);  //! Checks if correct demo board is connected.
  if (demo_board_connected)
  {
    print_prompt();
    ack |= LTC2991_register_write(LTC2991_I2C_ADDRESS, LTC2991_CHANNEL_ENABLE_REG, LTC2991_ENABLE_ALL_CHANNELS);   //! Enables all channels
    ack |= LTC2991_register_write(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, 0x00);                           //! Sets registers to default starting values.
    ack |= LTC2991_register_write(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, 0x00);
    ack |= LTC2991_register_write(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_PWM_Tinternal_REG, LTC2991_REPEAT_MODE);    //! Configures LTC2991 for Repeated Acquisition mode
  }
}

//! Repeats Linduino loop
void loop()
{
  int8_t ack=0;

  uint8_t user_command;
  if (demo_board_connected)           //! Does nothing if the demo board is not connected
  {
    if (Serial.available())           // Checks for user input
    {
      user_command = read_int();      //! Reads the user command
      if (user_command != 'm')
        Serial.println(user_command);
      ack = 0;
      switch (user_command)           //! Prints the appropriate submenu
      {
        case 1:
          ack |= menu_1_single_ended_voltage();       // Print single-ended voltage menu
          break;
        case 2:
          ack |= menu_2_read_differential_voltage();  // Differential voltage menu
          break;
        case 3:
          ack |= menu_3_read_temperature();           // Temperature menu
          break;
        case 4:
          ack |= menu_4_settings();                   // Settings menu
          break;
        case 5:
          ack |= menu_5_pwm_options();                // PWM options menu
          break;
        default:
          Serial.println("Incorrect Option");
          break;
      }
      if (ack != 0)
      {
        Serial.print(F("Error: No Acknowledge. Check I2C Address.\n"));
      }
      print_prompt();
    }
  }
}

// Function Definitions

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC1785A Demonstration Program                                 *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send and receive data from   *\n"));
  Serial.print(F("* the LTC2991 14-Bit Octal I2C Voltage, Current, and            *\n"));
  Serial.print(F("* Temperature Monitor.                                          *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("\n  1-Single-Ended Voltage\n"));
  Serial.print(F("  2-Differential Voltage\n"));
  Serial.print(F("  3-Temperature\n"));
  Serial.print(F("  4-Settings\n"));
  Serial.print(F("  5-PWM\n\n"));
  Serial.print(F("Enter a command:"));
}

//! Read single-ended voltages
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t menu_1_single_ended_voltage()
{
  int8_t ack=0;
  uint8_t user_command;
  do
  {
    //! Displays the single-ended voltage menu
    Serial.print(F("\nSingle-Ended Voltage\n\n"));
    Serial.print(F("  1-V1\n"));
    Serial.print(F("  2-V2\n"));
    Serial.print(F("  3-V3\n"));
    Serial.print(F("  4-V4\n"));
    Serial.print(F("  5-V5\n"));
    Serial.print(F("  6-V6\n"));
    Serial.print(F("  7-V7\n"));
    Serial.print(F("  8-V8\n"));
    Serial.print(F("  9-Vcc\n"));
    Serial.print(F("  10-ALL\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\n\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
    {
      Serial.print(F("m\n"));
    }
    else
      Serial.println(user_command);                         // Print user command

    int16_t code;
    int8_t data_valid;
    float voltage;

    //! Enables single-ended mode.
    //! Flushes one reading following mode change.
    //! Reads single-ended voltage from ADC and prints it.
    switch (user_command)
    {
      case 1:
        // Enable Single-Ended Mode by clearing LTC2991_V1_V2_DIFFERENTIAL_ENABLE bit in LTC2991_CONTROL_V1234_REG
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, 0x00, LTC2991_V1_V2_DIFFERENTIAL_ENABLE | LTC2991_V1_V2_TEMP_ENABLE);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V1_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("\n  V1: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 2:
        // Enable Single-Ended Mode by clearing LTC2991_V1_V2_DIFFERENTIAL_ENABLE bit in LTC2991_CONTROL_V1234_REG
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, 0x00, LTC2991_V1_V2_DIFFERENTIAL_ENABLE | LTC2991_V1_V2_TEMP_ENABLE);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V2_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("\n  V2: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 3:
        // Enable Single-Ended Mode by clearing LTC2991_V3_V4_DIFFERENTIAL_ENABLE bit in LTC2991_CONTROL_V1234_REG
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, 0x00, LTC2991_V3_V4_DIFFERENTIAL_ENABLE | LTC2991_V1_V2_TEMP_ENABLE);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V3_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("\n  V3: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 4:
        // Enable Single-Ended Mode by clearing LTC2991_V3_V4_DIFFERENTIAL_ENABLE bit in LTC2991_CONTROL_V1234_REG
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, 0x00, LTC2991_V3_V4_DIFFERENTIAL_ENABLE | LTC2991_V1_V2_TEMP_ENABLE);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V4_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("\n  V4: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 5:
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, 0x00, LTC2991_V5_V6_DIFFERENTIAL_ENABLE | LTC2991_V1_V2_TEMP_ENABLE);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V5_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("\n  V5: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 6:
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, 0x00, LTC2991_V5_V6_DIFFERENTIAL_ENABLE | LTC2991_V1_V2_TEMP_ENABLE);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V6_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("\n  V6: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 7:
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, 0x00, LTC2991_V7_V8_DIFFERENTIAL_ENABLE | LTC2991_V1_V2_TEMP_ENABLE);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V8_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("\n  V7: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 8:
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, 0x00, LTC2991_V7_V8_DIFFERENTIAL_ENABLE | LTC2991_V1_V2_TEMP_ENABLE);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V8_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("\n  V8: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 9:
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_Vcc_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_vcc_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("\n  Vcc: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 10:
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, 0x00, (LTC2991_V1_V2_DIFFERENTIAL_ENABLE | LTC2991_V3_V4_DIFFERENTIAL_ENABLE | LTC2991_V1_V2_TEMP_ENABLE | LTC2991_V3_V4_TEMP_ENABLE));
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, 0x00, (LTC2991_V5_V6_DIFFERENTIAL_ENABLE | LTC2991_V7_V8_DIFFERENTIAL_ENABLE | LTC2991_V5_V6_TEMP_ENABLE | LTC2991_V7_V8_TEMP_ENABLE));
        // Reads and displays all single-ended voltages
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V1_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("\n  V1: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        if (ack)
          break;
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V2_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("  V2: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        if (ack)
          break;
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V3_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("  V3: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        if (ack)
          break;
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V4_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("  V4: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        if (ack)
          break;
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V5_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("  V5: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        if (ack)
          break;
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V6_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("  V6: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        if (ack)
          break;
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V7_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("  V7: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        if (ack)
          break;
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V8_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_single_ended_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("  V8: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        if (ack)
          break;
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_Vcc_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_vcc_voltage(code, LTC2991_SINGLE_ENDED_lsb);
        Serial.print(F("  Vcc: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      default:
        if (user_command != 'm')
          Serial.println(" Incorrect Option");
        break;
    }
  }
  while ((user_command != 'm') && (ack != 1));
  return(ack);
}

//! Read differential voltages.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t menu_2_read_differential_voltage()
{
  int8_t ack=0;
  uint8_t user_command;
  do
  {
    //! Display differential voltage menu.
    Serial.print(F("\nDifferential Voltage\n\n"));
    Serial.print(F("  1-V1-V2\n"));
    Serial.print(F("  2-V3-V4\n"));
    Serial.print(F("  3-V5-V6\n"));
    Serial.print(F("  4-V7-V8\n"));
    Serial.print(F("  5-ALL\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\n\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
    {
      Serial.print(F("m\n"));
    }
    else
      Serial.println(user_command);                       // Print user command

    int8_t data_valid;
    int16_t code;
    float voltage;

    //! Enables differential mode.
    //! Flushes one reading following mode change.
    //! Reads differential voltage from ADC and prints it.
    switch (user_command)
    {
      case 1:
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, LTC2991_V1_V2_DIFFERENTIAL_ENABLE, LTC2991_V1_V2_TEMP_ENABLE);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V2_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_differential_voltage(code, LTC2991_DIFFERENTIAL_lsb);
        Serial.print(F("\n  V1-V2: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 2:
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, LTC2991_V3_V4_DIFFERENTIAL_ENABLE, LTC2991_V3_V4_TEMP_ENABLE);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V4_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_differential_voltage(code, LTC2991_DIFFERENTIAL_lsb);
        Serial.print(F("\n  V3-V4: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 3:
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, LTC2991_V5_V6_DIFFERENTIAL_ENABLE, LTC2991_V5_V6_TEMP_ENABLE);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V6_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_differential_voltage(code, LTC2991_DIFFERENTIAL_lsb);
        Serial.print(F("\n  V5-V6: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 4:
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, LTC2991_V7_V8_DIFFERENTIAL_ENABLE, LTC2991_V7_V8_TEMP_ENABLE);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V8_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_differential_voltage(code, LTC2991_DIFFERENTIAL_lsb);
        Serial.print(F("\n  V7-V8: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 5:
        // reads all differential voltages
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, LTC2991_V1_V2_DIFFERENTIAL_ENABLE | LTC2991_V3_V4_DIFFERENTIAL_ENABLE, LTC2991_V1_V2_TEMP_ENABLE | LTC2991_V3_V4_TEMP_ENABLE);
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, LTC2991_V5_V6_DIFFERENTIAL_ENABLE | LTC2991_V7_V8_DIFFERENTIAL_ENABLE, LTC2991_V5_V6_TEMP_ENABLE | LTC2991_V7_V8_TEMP_ENABLE);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V2_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_differential_voltage(code, LTC2991_DIFFERENTIAL_lsb);
        Serial.print(F("\n  V1-V2: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        if (ack)
          break;
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V4_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_differential_voltage(code, LTC2991_DIFFERENTIAL_lsb);
        Serial.print(F("  V3-V4: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        if (ack)
          break;
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V6_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_differential_voltage(code, LTC2991_DIFFERENTIAL_lsb);
        Serial.print(F("  V5-V6: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        if (ack)
          break;
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V8_MSB_REG, &code, &data_valid, LTC2991_TIMEOUT);
        voltage = LTC2991_code_to_differential_voltage(code, LTC2991_DIFFERENTIAL_lsb);
        Serial.print(F("  V7-V8: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      default:
        if (user_command != 'm')
        {
          Serial.print(F("Incorrect Option\n"));
        }
        break;
    }
  }
  while ((user_command != 'm') && (ack != 1));
  return(ack);
}

//! Read temperatures
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t menu_3_read_temperature()
{
  int8_t ack=0;
  boolean isKelvin = false;     //!Keeps track of the unit of measurement
  uint8_t user_command;
  do
  {
    //! Displays temperature menu
    Serial.print(F("\nTemperature\n\n"));
    Serial.print(F("  1-V1-V2\n"));
    Serial.print(F("  2-V3-V4\n"));
    Serial.print(F("  3-V5-V6\n"));
    Serial.print(F("  4-V7-V8\n"));
    Serial.print(F("  5-Internal\n"));
    Serial.print(F("  6-ALL\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\n\nEnter a command: "));
    user_command = read_int();                                  //! Reads the user command
    if (user_command == 'm')                                    // Print m if it is entered
    {
      Serial.print(F("m\n"));
    }
    else
      Serial.println(user_command);                             // Print user command

    // Read Temperature
    int8_t data_valid;
    int16_t adc_code;
    uint8_t reg_data;
    float temperature;

    //! Enables temperature mode.
    //! Flushes one reading following mode change.
    //! Reads temperature from ADC and prints it.
    switch (user_command)
    {
      case 1:

        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, LTC2991_V1_V2_TEMP_ENABLE, 0x00);
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V1_MSB_REG, &adc_code, &data_valid, LTC2991_TIMEOUT);
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, &reg_data);
        if (reg_data & LTC2991_V1_V2_KELVIN_ENABLE) isKelvin= true;
        else isKelvin=false;
        temperature = LTC2991_temperature(adc_code, LTC2991_TEMPERATURE_lsb, isKelvin);
        Serial.print(F("\n  V1-V2: "));
        Serial.print(temperature, 2);
        if (isKelvin) Serial.print(F(" K\n"));
        else Serial.print(F(" C\n"));
        break;
      case 2:

        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, LTC2991_V3_V4_TEMP_ENABLE, 0x00);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V3_MSB_REG, &adc_code, &data_valid, LTC2991_TIMEOUT);
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, &reg_data);
        if (reg_data & LTC2991_V3_V4_KELVIN_ENABLE) isKelvin=true;
        else isKelvin = false;
        temperature = LTC2991_temperature(adc_code, LTC2991_TEMPERATURE_lsb, isKelvin);
        Serial.print(F("\n  V3-V4: "));
        Serial.print(temperature, 2);

        if (isKelvin) Serial.print(F(" K\n"));
        else Serial.print(F(" C\n"));
        break;

      case 3:


        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, LTC2991_V5_V6_TEMP_ENABLE, 0x00);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V5_MSB_REG, &adc_code, &data_valid, LTC2991_TIMEOUT);
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, &reg_data);
        if (reg_data & LTC2991_V5_V6_KELVIN_ENABLE) isKelvin=true;
        else isKelvin=false;
        temperature = LTC2991_temperature(adc_code, LTC2991_TEMPERATURE_lsb, isKelvin);
        Serial.print(F("\n  V5-V6: "));
        Serial.print(temperature, 2);

        if (isKelvin) Serial.print(F(" K\n"));
        else Serial.print(F(" C\n"));
        break;
      case 4:

        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, LTC2991_V7_V8_TEMP_ENABLE, 0x00);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V7_MSB_REG, &adc_code, &data_valid, LTC2991_TIMEOUT);
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, &reg_data);
        if (reg_data & LTC2991_V7_V8_KELVIN_ENABLE) isKelvin = true;
        else isKelvin = false;
        temperature = LTC2991_temperature(adc_code, LTC2991_TEMPERATURE_lsb, isKelvin);
        Serial.print(F("\n  V7-V8: "));
        Serial.print(temperature, 2);

        if (isKelvin) Serial.print(F(" K\n"));
        else Serial.print(F(" C\n"));
        break;
      case 5:



        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_T_Internal_MSB_REG, &adc_code, &data_valid, LTC2991_TIMEOUT);
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_PWM_Tinternal_REG, &reg_data);
        if (reg_data & LTC2991_INT_KELVIN_ENABLE) isKelvin=true;
        else isKelvin=false;
        temperature = LTC2991_temperature(adc_code, LTC2991_TEMPERATURE_lsb,isKelvin);
        Serial.print(F("\n  Internal: "));
        Serial.print(temperature, 2);

        if (isKelvin) Serial.print(F(" K\n"));
        else Serial.print(F(" C\n"));
        break;
      case 6:
        // All Temperatures



        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, LTC2991_V1_V2_TEMP_ENABLE | LTC2991_V3_V4_TEMP_ENABLE, 0x00);
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, LTC2991_V5_V6_TEMP_ENABLE | LTC2991_V7_V8_TEMP_ENABLE, 0x00);
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V1_MSB_REG, &adc_code, &data_valid, LTC2991_TIMEOUT);
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, &reg_data);
        if (reg_data & LTC2991_V1_V2_KELVIN_ENABLE) isKelvin = true;
        else isKelvin=false;

        temperature = LTC2991_temperature(adc_code, LTC2991_TEMPERATURE_lsb,isKelvin);
        Serial.print(F("\n  V1-V2: "));
        Serial.print(temperature, 2);

        if (isKelvin) Serial.print(F(" K\n"));
        else Serial.print(F(" C\n"));
        if (ack)
          break;


        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V3_MSB_REG, &adc_code, &data_valid, LTC2991_TIMEOUT);
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, &reg_data);
        if (reg_data & LTC2991_V3_V4_KELVIN_ENABLE) isKelvin = true;
        else isKelvin=false;
        temperature = LTC2991_temperature(adc_code, LTC2991_TEMPERATURE_lsb, isKelvin);
        Serial.print(F("  V3-V4: "));
        Serial.print(temperature, 2);
        if (isKelvin) Serial.print(F(" K\n"));
        else Serial.print(F(" C\n"));
        if (ack)
          break;


        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V5_MSB_REG, &adc_code, &data_valid, LTC2991_TIMEOUT);
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, &reg_data);
        if (reg_data & LTC2991_V5_V6_KELVIN_ENABLE) isKelvin = true;
        else isKelvin=false;
        temperature = LTC2991_temperature(adc_code, LTC2991_TEMPERATURE_lsb,isKelvin);
        Serial.print(F("  V5-V6: "));
        Serial.print(temperature, 2);
        if (isKelvin) Serial.print(F(" K\n"));
        else Serial.print(F(" C\n"));
        if (ack)
          break;


        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_V7_MSB_REG, &adc_code, &data_valid, LTC2991_TIMEOUT);
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, &reg_data);
        if (reg_data & LTC2991_V7_V8_KELVIN_ENABLE) isKelvin = true;
        else isKelvin=false;
        temperature = LTC2991_temperature(adc_code, LTC2991_TEMPERATURE_lsb, isKelvin);
        Serial.print(F("  V7-V8: "));
        Serial.print(temperature, 2);
        if (isKelvin) Serial.print(F(" K\n"));
        else Serial.print(F(" C\n"));
        if (ack)
          break;


        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2991_adc_read_new_data(LTC2991_I2C_ADDRESS, LTC2991_T_Internal_MSB_REG, &adc_code, &data_valid, LTC2991_TIMEOUT);
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_PWM_Tinternal_REG, &reg_data);
        if (reg_data & LTC2991_INT_KELVIN_ENABLE) isKelvin = true;
        else isKelvin = false;
        temperature = LTC2991_temperature(adc_code, LTC2991_TEMPERATURE_lsb, isKelvin);
        Serial.print(F("  Internal: "));
        Serial.print(temperature, 2);
        if (isKelvin) Serial.print(F(" K\n"));
        else Serial.print(F(" C\n"));
        break;
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }
  }
  while ((user_command != 'm') && (ack == 0));
  return(ack);
}

//! Configure settings
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t menu_4_settings()
{
  uint8_t user_command;
  int8_t ack=0;
  uint8_t reg_data;

  do
  {
    Serial.print(F("\nSettings\n"));                           //! Displays Setting menu
    Serial.print(F("************************\n"));
    Serial.print(F("*              Filter  *\n"));
    Serial.print(F("************************\n"));
    Serial.print(F("* 1-V1/V2:    "));

    //! Displays present status of all settings by reading from LTC2991 registers.
    ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, &reg_data);
    if (reg_data & LTC2991_V1_V2_FILTER_ENABLE) Serial.print(F("Enabled  *\n"));
    else Serial.print(F("Disabled *\n"));

    Serial.print(F("* 2-V3/V4:    "));

    if (reg_data & LTC2991_V3_V4_FILTER_ENABLE) Serial.print(F("Enabled  *\n"));
    else Serial.print(F("Disabled *\n"));

    ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, &reg_data);
    Serial.print(F("* 3-V5/V6:    "));

    if (reg_data & LTC2991_V5_V6_FILTER_ENABLE) Serial.print(F("Enabled  *\n"));
    else Serial.print(F("Disabled *\n"));

    Serial.print(F("* 4-V7/V8:    "));

    if (reg_data & LTC2991_V7_V8_FILTER_ENABLE) Serial.print(F("Enabled  *\n"));
    else Serial.print(F("Disabled *\n"));

    Serial.print(F("* 5-Internal: "));

    ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_PWM_Tinternal_REG, &reg_data);
    if (reg_data & LTC2991_INT_FILTER_ENABLE) Serial.print(F("Enabled  *\n"));
    else Serial.print(F("Disabled *\n"));

    Serial.print(F("************************\n"));
    Serial.print(F("* 6-Deg C/K:"));

    //Only looks at the internal temperature sensor's setting for Celsius/Kelvin, but all channels should be the same in this program.
    ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_PWM_Tinternal_REG, &reg_data);
    if (reg_data & LTC2991_INT_KELVIN_ENABLE) Serial.print(F("    Kelvin *\n"));
    else Serial.print(F("    Celsius*\n"));

    Serial.print(F("* m-Main Menu          *\n"));
    Serial.print(F("************************\n\n"));
    Serial.print(F("Enter a command: "));

    user_command = read_int();                              //! Reads the user command

    if (user_command == 'm') Serial.print(F("m\n"));        // Print m if it is entered
    else Serial.println(user_command);                      // Print user command

    //! Toggles the setting selected by user.
    switch (user_command)
    {
      case 1:
        // Toggle Filter bit
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, &reg_data);
        ack |= LTC2991_register_write(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, reg_data ^ LTC2991_V1_V2_FILTER_ENABLE);
        break;
      case 2:
        // Toggle Filter bit
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, &reg_data);
        ack |= LTC2991_register_write(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, reg_data ^ LTC2991_V3_V4_FILTER_ENABLE);
        break;
      case 3:
        // Toggle Filter bit
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, &reg_data);
        ack |= LTC2991_register_write(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, reg_data ^ LTC2991_V5_V6_FILTER_ENABLE);
        break;
      case 4:
        // Toggle Filter bit
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, &reg_data);
        ack |= LTC2991_register_write(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, reg_data ^ LTC2991_V7_V8_FILTER_ENABLE);
        break;
      case 5:
        // Toggle Filter bit
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_PWM_Tinternal_REG, &reg_data);
        ack |= LTC2991_register_write(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_PWM_Tinternal_REG, reg_data ^ LTC2991_INT_FILTER_ENABLE);
        break;
      case 6:
        // Toggle Kelvin/Celsius bits
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, &reg_data);
        ack |= LTC2991_register_write(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V1234_REG, reg_data ^ (LTC2991_V1_V2_KELVIN_ENABLE | LTC2991_V3_V4_KELVIN_ENABLE));
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, &reg_data);
        ack |= LTC2991_register_write(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, reg_data ^ (LTC2991_V5_V6_KELVIN_ENABLE | LTC2991_V7_V8_KELVIN_ENABLE));
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_PWM_Tinternal_REG, &reg_data);
        ack |= LTC2991_register_write(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_PWM_Tinternal_REG, reg_data ^ LTC2991_INT_KELVIN_ENABLE);
        break;
      default:
        if (user_command != 'm')
          Serial.print(F("Incorrect Option\n"));
        break;
    }
  }
  while ((user_command != 'm') && (ack != 1));
  return(ack);
}

//! Configure PWM options
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t menu_5_pwm_options()
{
  int8_t ack=0;

  uint8_t user_command;
  int16_t pwm_temp = 0;
  uint8_t reg_data, reg_data_msb;

  do
  {
    Serial.print(F("\n\nPWM Settings - Select to Toggle\n\n"));                           //! Displays PWM Settings menu
    Serial.print(F("  1-Temp Threshold: "));
    // Check if V7-V8 are configured for temperature mode

    //! Print present status of all PWM settings based on LTC2991 register contents.
    ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, &reg_data);
    if (reg_data & LTC2991_V7_V8_TEMP_ENABLE)
    {
      // Print the configured PWM temperature threshold
      ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_PWM_THRESHOLD_MSB_REG, &reg_data_msb);
      ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_PWM_Tinternal_REG, &reg_data);
      //  Combine temperature threshold's upper 8 MSB's from LTC2991_PWM_THRESHOLD_MSB_REG with the 1 LSB in the upper bit of LTC2991_CONTROL_PWM_Tinternal_REG
      pwm_temp = (int16_t)((reg_data_msb << 1) | (reg_data & LTC2991_PWM_0) >> 7);
      Serial.print(pwm_temp);
      //Print degrees C or K configured for V7/V8
      ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, &reg_data);
      if (reg_data & LTC2991_V7_V8_KELVIN_ENABLE) Serial.print(F(" K\n"));
      else Serial.print(F(" C\n"));
    }
    else
    {
      Serial.print(F("Not Configured"));
    }
    ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_PWM_Tinternal_REG, &reg_data);
    if (reg_data & LTC2991_PWM_INVERT) Serial.print(F("  2-Inverted\n"));
    else Serial.print(F("  2-Noninverted\n"));

    if (reg_data & LTC2991_PWM_ENABLE) Serial.print(F("  3-Enabled\n"));
    else Serial.print(F("  3-Disabled\n"));

    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\n\nEnter a command: "));
    user_command = read_int();                          //! Reads the user command
    if (user_command == 'm')                            // Print m if it is entered
    {
      Serial.print(F("m\n"));
    }
    else
    {
      Serial.println(user_command);                     // Print user command
    }

    //! Modify PWM threshold or toggle a PWM setting based on user input.
    switch (user_command)
    {
      case 1:
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, &reg_data);
        if (reg_data & LTC2991_V7_V8_KELVIN_ENABLE) Serial.print(F("\nEnter Temperature Threshold in Kelvin: "));
        else Serial.print(F("\nEnter Temperature Threshold in Celsius: "));
        pwm_temp = (int16_t)read_int();  // read the user command
        Serial.println(pwm_temp);
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, LTC2991_V7_V8_TEMP_ENABLE, 0x00);  // Enables V7-V8
        ack |= LTC2991_register_write(LTC2991_I2C_ADDRESS, LTC2991_PWM_THRESHOLD_MSB_REG, pwm_temp >> 1);
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_PWM_Tinternal_REG, (pwm_temp & 0x01) << 7, 0x00);
        break;
      case 2:
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_PWM_Tinternal_REG, &reg_data);
        ack |= LTC2991_register_write(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_PWM_Tinternal_REG, reg_data ^ LTC2991_PWM_INVERT);
        break;
      case 3:
        // Enable temperature mode for V7-V8 in case it was in voltage mode
        ack |= LTC2991_register_set_clear_bits(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_V5678_REG, LTC2991_V7_V8_TEMP_ENABLE, 0x00);
        // Toggle PWM Enable
        ack |= LTC2991_register_read(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_PWM_Tinternal_REG, &reg_data);
        ack |= LTC2991_register_write(LTC2991_I2C_ADDRESS, LTC2991_CONTROL_PWM_Tinternal_REG, reg_data ^ LTC2991_PWM_ENABLE);
        break;
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }
  }
  while ((user_command != 'm') && (ack != 1));
  return(ack);
}