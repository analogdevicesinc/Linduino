/*!
DC1697A
LTC2945: Wide Range Power Monitor

@verbatim

Setting the Alerts:
    1. Select the Alert option from the main menu.
    2. Select the desired alerts to be changed. Then enter the minimum and maximum
       values.
    3. Select the Enable and Disable Alert Option and Enable the desired alerts.
    4. Lastly, go back to the main menu and start reading values in Continuous Mode
       or Snapshot Mode.
       Note: Alerts only respond when conversion is done. Therefore, in continuous
       mode the alerts will constantly be updated.

Reading and Clearing an Alert:
    1. Select the Alert option from the main menu.
    2. Select the Read and Clear Alerts option. This reads all faults that occured
       and clears the alerts.

NOTES
 Setup:
 Set the terminal baud rate to 115200 and select the newline terminator.
 Requires a power supply.
 Refer to demo manual DC1697A.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim


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
    @ingroup LTC2945
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC2945.h"
#include <Wire.h>
#include <SPI.h>

// Function Declaration
void print_title();                 // Print the title block
void print_prompt();                // Print the Prompt
void store_alert_settings();        // Store the alert settings to the EEPROM
int8_t restore_alert_settings();    // Read the alert settings from EEPROM

int8_t menu_1_continuous_mode(int8_t VDD_or_SENSE_monitoring, int8_t ADIN_or_SENSE, float scale);
int8_t menu_2_snapshot_mode(int8_t VDD_or_SENSE_monitoring, int8_t ADIN_or_SENSE);
int8_t menu_2_snapshot_mode_menu_1_current(int8_t VDD_or_SENSE_monitoring, int8_t ADIN_or_SENSE);
int8_t menu_2_snapshot_mode_menu_2_vin();
int8_t menu_2_snapshot_mode_menu_3_ADIN();
int8_t menu_3_alert(int8_t ADIN_or_SENSE);
int8_t menu_3_alert_menu_1_set_power_alerts(int8_t ADIN_or_SENSE);
int8_t menu_3_alert_menu_2_set_current_alerts();
int8_t menu_3_alert_menu_3_set_vin_alerts();
int8_t menu_3_alert_menu_4_set_adin_alerts();
int8_t menu_3_alert_menu_5_enable_disable_alerts();
int8_t menu_3_alert_menu_6_read_clear_alerts();
int8_t menu_4_shutdown();
void menu_5_settings(int8_t *VDD_or_SENSE_monitoring, int8_t *ADIN_or_SENSE, float *scale);
int8_t menu_6_clear_min_max();

#define CONTINUOUS_MODE_DISPLAY_DELAY 1000                  //!< The delay between readings

// LSB Weights
const float LTC2945_ADIN_lsb = 5.001221E-04;                //!< Typical ADIN lsb weight in volts
const float LTC2945_DELTA_SENSE_lsb = 2.5006105E-05;        //!< Typical Delta lsb weight in volts
const float LTC2945_VIN_lsb = 2.5006105E-02;                //!< Typical VIN lsb weight in volts
const float LTC2945_Power_lsb = 6.25305E-07;                //!< Typical POWER lsb weight in V^2
const float LTC2945_ADIN_DELTA_SENSE_lsb = 1.25061E-08;     //!< Typical sense lsb weight in V^2  *ADIN_lsb * DELTA_SENSE_lsb

const float resistor = .02;         //!< resistor value on demo board

// Error string
const char ack_error[] = "Error: No Acknowledge. Check I2C Address."; //!< Error message

// Global variables
static int8_t demo_board_connected;        //!< Set to 1 if the board is connected
static uint8_t alert_code = 0;             //!< Value stored or read from ALERT register.  Shared between loop() and restore_alert_settings()

//! Initialize Linduino
void setup()
{
  char demo_name[] = "DC1697";      // Demo Board Name stored in QuikEval EEPROM

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
    restore_alert_settings();
    print_prompt();
  }
}

//! Repeats Linduino loop
void loop()
{
  int8_t ack = 0;                               // I2C acknowledge indicator
  static uint8_t user_command;                  // The user input command

  static int8_t VDD_or_SENSE_monitoring = 1;    // Choose whether VIN is sensed at VDD pin or SENSE pin (A2 bit of CONTROL Register A)
  static int8_t ADIN_or_SENSE = 1;              // Choose whether power multiplier uses ADIN pin or SENSE pin as voltage input (A0 bit in CONTROL Register A)

  static float scale;                           // Stores division ration for resistive divider on ADIN pin.  Configured inside "Settings" menu.

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
          ack |= menu_1_continuous_mode(VDD_or_SENSE_monitoring, ADIN_or_SENSE, scale);  // Continuous Mode
          break;

        case 2:
          ack |= menu_2_snapshot_mode(VDD_or_SENSE_monitoring, ADIN_or_SENSE);
          break;

        case 3:
          ack |= menu_3_alert(ADIN_or_SENSE);  // Alert
          break;

        case 4:
          ack |= menu_4_shutdown();  // Shutdown Down Mode
          break;

        case 5:
          menu_5_settings(&VDD_or_SENSE_monitoring, &ADIN_or_SENSE, &scale);  // Settings
          break;

        case 6:
          ack |= menu_6_clear_min_max();  // Clear Min/Max
          break;

        default:
          Serial.println("Incorrect Option");
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
  Serial.print(F("* DC1697 Demonstration Program                                  *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program communicates with the LTC2945 12-Bit Wide Range  *\n"));
  Serial.print(F("* I2C Power Monitor found on the DC1697 demo board.             *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Print the Prompt
void print_prompt()
{
  Serial.print(F("\n1-Continuous Mode\n"));
  Serial.print(F("2-Snapshot Mode\n"));
  Serial.print(F("3-Alert\n"));
  Serial.print(F("4-Shutdown Mode\n"));
  Serial.print(F("5-Settings\n"));
  Serial.print(F("6-Clear Min/Max\n\n"));
  Serial.print(F("Enter a command: "));
}

//! Continuous Mode.
int8_t menu_1_continuous_mode(int8_t VDD_or_SENSE_monitoring,   //!< Choose whether VIN is sensed at VDD pin or SENSE pin (A2 bit of CONTROL Register A)
                              int8_t ADIN_or_SENSE,              //!< Choose whether power multiplier uses ADIN pin or SENSE pin as voltage input (A0 bit in CONTROL Register A)
                              float scale)                       //!< Stores division ratio for resistive divider on ADIN pin.  Configured inside "Settings" menu.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t LTC2945_mode;
  int8_t ack = 0;
  LTC2945_mode = LTC2945_CONTINUOUS_MODE;
  if (VDD_or_SENSE_monitoring == 1)
    LTC2945_mode = LTC2945_mode | LTC2945_SENSE_MONITOR;                          //! Enables SENSE+ voltage monitor
  if (ADIN_or_SENSE == 1)
    LTC2945_mode = LTC2945_mode | LTC2945_SENSE_MULTIPLIER;                       //! Selects VIN as a multiplier for power
  Serial.println();
  ack |= LTC2945_write(LTC2945_I2C_ADDRESS, LTC2945_CONTROL_REG, LTC2945_mode);   //! Sets the LTC2945 to continuous mode
  do
  {
    if (ADIN_or_SENSE == 1)
    {
      Serial.print(F("*************************\n\n"));

      int32_t power_code, max_power_code, min_power_code;
      ack |= LTC2945_read_24_bits(LTC2945_I2C_ADDRESS, LTC2945_POWER_MSB2_REG, &power_code);
      ack |= LTC2945_read_24_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_POWER_MSB2_REG, &max_power_code);
      ack |= LTC2945_read_24_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_POWER_MSB2_REG, &min_power_code);

      float power, max_power, min_power;  // Store power results
      power = LTC2945_code_to_power(power_code, resistor, LTC2945_Power_lsb);
      max_power = LTC2945_code_to_power(max_power_code, resistor, LTC2945_Power_lsb);
      min_power = LTC2945_code_to_power(min_power_code, resistor, LTC2945_Power_lsb);

      Serial.print(F("****Power: "));
      Serial.print(power, 4);
      Serial.print(F(" W\n"));

      Serial.print(F("Max Power: "));
      Serial.print(max_power, 4);
      Serial.print(F(" W\n"));

      Serial.print(F("Min Power: "));
      Serial.print(min_power, 4);
      Serial.print(F(" W\n"));
    }
    else
    {
      Serial.print(F("*************************"));
      Serial.println();
      Serial.println();

      int32_t power_code, max_power_code, min_power_code;
      ack |= LTC2945_read_24_bits(LTC2945_I2C_ADDRESS, LTC2945_POWER_MSB2_REG, &power_code);
      ack |= LTC2945_read_24_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_POWER_MSB2_REG, &max_power_code);
      ack |= LTC2945_read_24_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_POWER_MSB2_REG, &min_power_code);

      float power, max_power, min_power;
      power = LTC2945_code_to_ADIN_power(power_code, resistor, LTC2945_ADIN_DELTA_SENSE_lsb) * scale;
      max_power = LTC2945_code_to_ADIN_power(max_power_code, resistor, LTC2945_ADIN_DELTA_SENSE_lsb) * scale;
      min_power = LTC2945_code_to_ADIN_power(min_power_code, resistor, LTC2945_ADIN_DELTA_SENSE_lsb) * scale;

      Serial.print(F("\n***ADIN Power: "));
      Serial.print(power, 4);
      Serial.print(F(" W\n"));

      Serial.print(F("Max ADIN Power: "));
      Serial.print(max_power, 4);
      Serial.print(F(" W\n"));

      Serial.print(F("Min ADIN Power: "));
      Serial.print(min_power, 4);
      Serial.print(F(" W\n"));
    }

    uint16_t current_code, max_current_code, min_current_code;
    ack |= LTC2945_read_12_bits(LTC2945_I2C_ADDRESS, LTC2945_DELTA_SENSE_MSB_REG, &current_code);
    ack |= LTC2945_read_12_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_DELTA_SENSE_MSB_REG, &max_current_code);
    ack |= LTC2945_read_12_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_DELTA_SENSE_MSB_REG, &min_current_code);

    float current, max_current, min_current;
    current = LTC2945_code_to_current(current_code, resistor, LTC2945_DELTA_SENSE_lsb);
    max_current = LTC2945_code_to_current(max_current_code, resistor, LTC2945_DELTA_SENSE_lsb);
    min_current = LTC2945_code_to_current(min_current_code, resistor, LTC2945_DELTA_SENSE_lsb);

    Serial.print(F("\n****Current: "));
    Serial.print(current, 4);
    Serial.print(F(" A\n"));

    Serial.print(F("Max Current: "));
    Serial.print(max_current, 4);
    Serial.print(F(" A\n"));

    Serial.print(F("Min Current: "));
    Serial.print(min_current, 4);
    Serial.print(F(" A\n"));

    uint16_t VIN_code, max_VIN_code, min_VIN_code;
    ack |= LTC2945_read_12_bits(LTC2945_I2C_ADDRESS, LTC2945_VIN_MSB_REG, &VIN_code);
    ack |= LTC2945_read_12_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_VIN_MSB_REG, &max_VIN_code);
    ack |= LTC2945_read_12_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_VIN_MSB_REG, &min_VIN_code);

    float VIN, max_VIN, min_VIN;
    VIN = LTC2945_VIN_code_to_voltage(VIN_code , LTC2945_VIN_lsb);
    max_VIN = LTC2945_VIN_code_to_voltage(max_VIN_code, LTC2945_VIN_lsb);
    min_VIN = LTC2945_VIN_code_to_voltage(min_VIN_code, LTC2945_VIN_lsb);

    Serial.print(F("\n****VIN: "));
    Serial.print(VIN, 4);
    Serial.print(F(" V\n"));

    Serial.print(F("Max VIN: "));
    Serial.print(max_VIN, 4);
    Serial.print(F(" V\n"));

    Serial.print(F("Min VIN: "));
    Serial.print(min_VIN, 4);
    Serial.print(F(" V\n"));

    uint16_t ADIN_code, max_ADIN_code, min_ADIN_code;
    ack |= LTC2945_read_12_bits(LTC2945_I2C_ADDRESS, LTC2945_ADIN_MSB_REG, &ADIN_code);
    ack |= LTC2945_read_12_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_ADIN_MSB_REG, &max_ADIN_code);
    ack |= LTC2945_read_12_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_ADIN_MSB_REG, &min_ADIN_code);

    float ADIN, max_ADIN, min_ADIN;
    ADIN = LTC2945_ADIN_code_to_voltage(ADIN_code, LTC2945_ADIN_lsb);
    max_ADIN = LTC2945_ADIN_code_to_voltage(max_ADIN_code, LTC2945_ADIN_lsb);
    min_ADIN = LTC2945_ADIN_code_to_voltage(min_ADIN_code, LTC2945_ADIN_lsb);

    Serial.print(F("\n****ADIN: "));
    Serial.print(ADIN, 4);
    Serial.print(F(" V\n"));

    Serial.print(F("Max ADIN: "));
    Serial.print(max_ADIN, 4);
    Serial.print(F(" V\n"));

    Serial.print(F("Min ADIN: "));
    Serial.print(min_ADIN, 4);
    Serial.print(F(" V\n"));
    Serial.print(F("m-Main Menu\n\n"));
    Serial.flush();
    delay(CONTINUOUS_MODE_DISPLAY_DELAY);
  }
  while (Serial.available() == false);
  read_int();  // clears the Serial.available
  return(ack);
}

// DJE: Should user_command be passed as a pointer to allow 'm' option to break properly?
//! Snapshot Mode Menu
int8_t menu_2_snapshot_mode(int8_t VDD_or_SENSE_monitoring, //!< Choose whether VIN is sensed at VDD pin or SENSE pin (A2 bit of CONTROL Register A)
                            int8_t ADIN_or_SENSE)           //!< Choose whether power multiplier uses ADIN pin or SENSE pin as voltage input (A0 bit in CONTROL Register A)
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  //! Print submenu
  Serial.print(F("*************************\n\n"));
  Serial.print(F("Snapshot Mode\n"));
  Serial.print(F("1-Current\n"));
  Serial.print(F("2-VIN\n"));

  Serial.print(F("3-ADIN\n"));
  Serial.print(F("m-Main Menu\n"));
  uint8_t user_command;
  do
  {
    Serial.print(F("\n\nEnter a Command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
    {
      Serial.println(user_command);
    }
    if (user_command != 'm')
    {
      Serial.print(F("*************************\n\n"));
      Serial.print(F("Snapshot Mode\n"));
      Serial.print(F("1-Current\n"));
      Serial.print(F("2-VIN\n"));
      Serial.print(F("3-ADIN\n"));
      Serial.print(F("m-Main Menu\n"));
    }
    Serial.println();
    switch (user_command)
    {
      case 1:  // Current - Snapshot Mode
        ack |= menu_2_snapshot_mode_menu_1_current(VDD_or_SENSE_monitoring, ADIN_or_SENSE);
        break;

      case 2:  // VIN - Snapshot Mode
        ack |= menu_2_snapshot_mode_menu_2_vin();
        break;

      case 3:  // ADIN - Snapshot Mode
        ack |= menu_2_snapshot_mode_menu_3_ADIN();
        break;

      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }
  }
  while (!((user_command == 'm') || (ack)));
  return(ack);
}

//! Current - Snapshot mode
int8_t menu_2_snapshot_mode_menu_1_current(int8_t VDD_or_SENSE_monitoring,  //!< Choose whether VIN is sensed at VDD pin or SENSE pin (A2 bit of CONTROL Register A)
    int8_t ADIN_or_SENSE)            //!< Choose whether power multiplier uses ADIN pin or SENSE pin as voltage input (A0 bit in CONTROL Register A)
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  int8_t LTC2945_mode;
  LTC2945_mode = LTC2945_DELTA_SENSE_SNAPSHOT;
  if (ADIN_or_SENSE == 1) LTC2945_mode = LTC2945_mode | LTC2945_SENSE_MONITOR;
  if (VDD_or_SENSE_monitoring == 1) LTC2945_mode = LTC2945_mode | LTC2945_SENSE_MULTIPLIER;
  ack |= LTC2945_write(LTC2945_I2C_ADDRESS, LTC2945_CONTROL_REG, LTC2945_mode);

  uint8_t busy;
  do
  {
    ack |= LTC2945_read(LTC2945_I2C_ADDRESS, LTC2945_CONTROL_REG, &busy);
  }
  while (0x8 & busy);

  uint16_t current_code;
  ack |= LTC2945_read_12_bits(LTC2945_I2C_ADDRESS, LTC2945_DELTA_SENSE_MSB_REG, &current_code);

  float current;
  current = LTC2945_code_to_current(current_code, resistor, LTC2945_DELTA_SENSE_lsb);
  Serial.print(F("Current: "));
  Serial.print(current, 4);
  Serial.print(F(" A"));
  return(ack);
}

//! VIN - Snapshot Mode
int8_t menu_2_snapshot_mode_menu_2_vin()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  ack |= LTC2945_write(LTC2945_I2C_ADDRESS, LTC2945_CONTROL_REG, LTC2945_VIN_SNAPSHOT);

  uint8_t busy;
  do
  {
    ack |= LTC2945_read(LTC2945_I2C_ADDRESS, LTC2945_CONTROL_REG, &busy);
  }
  while (0x8 & busy);

  uint16_t VIN_code;
  ack |= LTC2945_read_12_bits(LTC2945_I2C_ADDRESS, LTC2945_VIN_MSB_REG, &VIN_code);

  float VIN;
  VIN = LTC2945_VIN_code_to_voltage(VIN_code, LTC2945_VIN_lsb);
  Serial.print(F("VIN: "));
  Serial.print(VIN, 4);
  Serial.print(F(" V"));
  return(ack);
}

//! ADIN - Snapshot Mode
int8_t menu_2_snapshot_mode_menu_3_ADIN()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;

  ack |= LTC2945_write(LTC2945_I2C_ADDRESS, LTC2945_CONTROL_REG, LTC2945_ADIN_SNAPSHOT);

  uint8_t busy;
  do
  {
    ack |= LTC2945_read(LTC2945_I2C_ADDRESS, LTC2945_CONTROL_REG, &busy);
  }
  while (0x8 & busy);

  uint16_t ADIN_code;
  ack |= LTC2945_read_12_bits(LTC2945_I2C_ADDRESS, LTC2945_ADIN_MSB_REG, &ADIN_code);

  float ADIN;
  ADIN = LTC2945_ADIN_code_to_voltage(ADIN_code, LTC2945_ADIN_lsb);
  Serial.print(F("ADIN: "));
  Serial.print(ADIN, 4);
  Serial.print(F(" V"));
  return(ack);
}

//! Alert Menu
int8_t menu_3_alert(int8_t ADIN_or_SENSE)   //!< Choose whether power multiplier uses ADIN pin or SENSE pin as voltage input (A0 bit in CONTROL Register A)
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  int8_t user_command;
  do
  {
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Set Power Alerts\n"));
    Serial.print(F("2-Set Current Alerts\n"));
    Serial.print(F("3-Set VIN Alerts\n"));
    Serial.print(F("4-Set ADIN Alerts\n"));
    Serial.print(F("5-Enable and Disable Alerts\n"));
    Serial.print(F("6-Read and Clear Alerts\n"));
    Serial.print(F("7-Store Alert Settings to EEPROM\n"));
    Serial.print(F("m-Main Menu\n\n"));
    Serial.print(F("Enter a command: "));

    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    switch (user_command)
    {
      case 1:
        ack |= menu_3_alert_menu_1_set_power_alerts(ADIN_or_SENSE); // Set Power Alerts
        break;

      case 2:
        ack |= menu_3_alert_menu_2_set_current_alerts();            // Set Current Alerts
        break;

      case 3:
        ack |= menu_3_alert_menu_3_set_vin_alerts();                // Set VIN Alerts
        break;

      case 4:
        ack |= menu_3_alert_menu_4_set_adin_alerts();               // Set ADIN Alerts
        break;

      case 5:
        ack |= menu_3_alert_menu_5_enable_disable_alerts();         // Enable/Disable Alerts
        break;

      case 6:
        ack |= menu_3_alert_menu_6_read_clear_alerts();             // Read Alerts
        break;

      case 7:
        // Read current min/max alarm and store to EEPROM
        store_alert_settings();
        break;
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }
  }
  while (!((user_command == 'm') || (ack)));
  return(ack);
}

//! Set Power Alerts
int8_t menu_3_alert_menu_1_set_power_alerts(int8_t ADIN_or_SENSE)   //!< Choose whether power multiplier uses ADIN pin or SENSE pin as voltage input (A0 bit in CONTROL Register A)
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  Serial.print(F("Enter Max Power Threshold:"));

  float max_power_threshold;
  max_power_threshold = read_float();
  Serial.println(max_power_threshold, 4);

  int32_t max_power_threshold_code;
  if (ADIN_or_SENSE == 1)
    max_power_threshold_code = (max_power_threshold / LTC2945_Power_lsb) * resistor;
  else
    max_power_threshold_code = (max_power_threshold / LTC2945_ADIN_DELTA_SENSE_lsb) * resistor;

  ack |= LTC2945_write_24_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_POWER_THRESHOLD_MSB2_REG, max_power_threshold_code);

  Serial.print(F("Enter Min Power Threshold:"));

  float min_power_threshold;
  min_power_threshold = read_float();
  Serial.println(min_power_threshold, 4);

  int32_t min_power_threshold_code;
  if (ADIN_or_SENSE == 1)
    min_power_threshold_code = (min_power_threshold / LTC2945_Power_lsb) * resistor;
  else
    min_power_threshold_code = min_power_threshold / LTC2945_ADIN_DELTA_SENSE_lsb * resistor;
  ack |= LTC2945_write_24_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_POWER_THRESHOLD_MSB2_REG, min_power_threshold_code);
  return(ack);
}

//! Set Current Alerts
int8_t menu_3_alert_menu_2_set_current_alerts()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  Serial.print(F("Enter Max Current Threshold:"));

  float max_current_threshold;
  max_current_threshold = read_float();
  Serial.println(max_current_threshold, 4);

  int32_t max_current_threshold_code;
  max_current_threshold_code = (max_current_threshold / LTC2945_DELTA_SENSE_lsb) * resistor;

  ack |= LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_DELTA_SENSE_THRESHOLD_MSB_REG, (max_current_threshold_code << 4));

  Serial.print(F("Enter Min Current Threshold:"));

  float min_current_threshold;
  min_current_threshold = read_float();
  Serial.println(min_current_threshold, 4);

  int32_t min_current_threshold_code;
  min_current_threshold_code = (min_current_threshold / LTC2945_DELTA_SENSE_lsb) * resistor;
  ack |= LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_DELTA_SENSE_THRESHOLD_MSB_REG, (min_current_threshold_code << 4));
  return(ack);
}

//! Set VIN Alerts
int8_t menu_3_alert_menu_3_set_vin_alerts()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  Serial.print(F("Enter Max VIN Threshold:"));

  float max_VIN_threshold;
  max_VIN_threshold = read_float();
  Serial.println(max_VIN_threshold, 4);

  int32_t max_VIN_threshold_code;
  max_VIN_threshold_code = max_VIN_threshold / LTC2945_VIN_lsb;
  ack |= LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_VIN_THRESHOLD_MSB_REG, (max_VIN_threshold_code << 4));

  Serial.print(F("Enter Min VIN Threshold:"));

  float min_VIN_threshold;
  min_VIN_threshold = read_float();
  Serial.println(min_VIN_threshold, 4);

  int32_t min_VIN_threshold_code;
  min_VIN_threshold_code = min_VIN_threshold / LTC2945_VIN_lsb;
  ack |= LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_VIN_THRESHOLD_MSB_REG, (min_VIN_threshold_code << 4));
  return(ack);
}

//! Set ADIN Alerts
int8_t menu_3_alert_menu_4_set_adin_alerts()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  Serial.print(F("Enter Max ADIN Threshold:"));

  float max_ADIN_threshold;
  max_ADIN_threshold = read_float();
  Serial.println(max_ADIN_threshold, 4);

  int32_t max_ADIN_threshold_code;
  max_ADIN_threshold_code = max_ADIN_threshold / LTC2945_ADIN_lsb;
  ack |= LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_ADIN_THRESHOLD_MSB_REG, (max_ADIN_threshold_code << 4));

  Serial.print(F("Enter Min ADIN Threshold:"));

  float min_ADIN_threshold;
  min_ADIN_threshold = read_float();
  Serial.println(min_ADIN_threshold, 4);

  int32_t min_ADIN_threshold_code;
  min_ADIN_threshold_code = min_ADIN_threshold / LTC2945_ADIN_lsb;
  ack |= LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_ADIN_THRESHOLD_MSB_REG, (min_ADIN_threshold_code << 4));
  return(ack);
}

//! Enable/Disable Alerts
int8_t menu_3_alert_menu_5_enable_disable_alerts()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  uint8_t user_command;
  do
  {
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Enable/Disable Power Alerts\n"));
    Serial.print(F("2-Enable/Disable Current Alerts\n"));
    Serial.print(F("3-Enable/Disable VIN Alerts\n"));
    Serial.print(F("4-Enable/Disable ADIN Alerts\n"));
    Serial.print(F("m-Alert Menu\n\n"));
    Serial.print(F("Enter a command:"));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println('m');
    else
      Serial.println(user_command);
    if (!(user_command == 0))
    {
      Serial.print(F("1-Enable, 2-Disable\n"));
    }
    switch (user_command)
    {
      case 1:
        // ENABLE/Disable Power Alerts
        Serial.print(F(" Max Power:"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert_code = alert_code | LTC2945_ENABLE_MAX_POWER_ALERT;
        else
          alert_code = alert_code & LTC2945_DISABLE_MAX_POWER_ALERT;

        Serial.print(F(" Min Power:"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert_code = alert_code | LTC2945_ENABLE_MIN_POWER_ALERT;
        else
          alert_code = alert_code & LTC2945_DISABLE_MIN_POWER_ALERT;
        break;
      case 2:
        // ENABLE/Disable Current Alerts
        Serial.print(F(" Max Current:"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert_code = alert_code | LTC2945_ENABLE_MAX_DELTA_SENSE_ALERT;
        else
          alert_code = alert_code & LTC2945_DISABLE_MAX_DELTA_SENSE_ALERT;

        Serial.print(F(" Min Current:"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert_code = alert_code | LTC2945_ENABLE_MIN_DELTA_SENSE_ALERT;
        else
          alert_code = alert_code & LTC2945_DISABLE_MIN_DELTA_SENSE_ALERT;
        break;
      case 3:
        // ENABLE/Disable VIN Alerts
        Serial.print(F(" Max VIN:"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert_code = alert_code | LTC2945_ENABLE_MAX_VIN_ALERT;
        else
          alert_code = alert_code & LTC2945_DISABLE_MAX_VIN_ALERT;

        Serial.print(F(" Min VIN:"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert_code = alert_code | LTC2945_ENABLE_MIN_VIN_ALERT;
        else
          alert_code = alert_code & LTC2945_DISABLE_MIN_VIN_ALERT;
        break;
      case 4:
        // ENABLE/Disable ADIN Alerts
        Serial.print(F(" Max ADIN:"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert_code = alert_code | LTC2945_ENABLE_MAX_ADIN_ALERT;
        else
          alert_code = alert_code & LTC2945_DISABLE_MAX_ADIN_ALERT;

        Serial.print(F(" Min ADIN:"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert_code = alert_code | LTC2945_ENABLE_MIN_ADIN_ALERT;
        else
          alert_code = alert_code & LTC2945_DISABLE_MIN_ADIN_ALERT;
        break;
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }
  }
  while (user_command != 'm');
  ack |= LTC2945_write(LTC2945_I2C_ADDRESS, LTC2945_ALERT_REG, alert_code);
  return(ack);
}

//! Read Alerts and Clear Alerts
int8_t menu_3_alert_menu_6_read_clear_alerts()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  ack |= LTC2945_read(LTC2945_I2C_ADDRESS, LTC2945_FAULT_CoR_REG, &alert_code);
  if (alert_code & 0x80) Serial.print(F("Power Overvalue Fault Occurred\n\n"));
  if (alert_code & 0x40) Serial.print(F("Power Undervalue Fault Occurred\n"));
  if (alert_code & 0x20) Serial.print(F("Current Overvalue Fault Occurred\n"));
  if (alert_code & 0x10) Serial.print(F("Current Undervalue Fault Occurred\n"));
  if (alert_code & 0x08) Serial.print(F("VIN Overvalue Fault Occurred\n"));
  if (alert_code & 0x04) Serial.print(F("VIN Undervalue Fault Occurred\n"));
  if (alert_code & 0x02) Serial.print(F("ADIN Overvalue Fault Occurred\n"));
  if (alert_code & 0x01) Serial.print(F("ADIN Undervalue Fault Occurred\n"));
  if (!alert_code) Serial.print(F("No Alarms Were Present\n"));
  return(ack);
}

//! Shutdown Down Mode
int8_t menu_4_shutdown()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;

  ack |= LTC2945_write(LTC2945_I2C_ADDRESS, LTC2945_CONTROL_REG, LTC2945_SHUTDOWN_MODE);
  Serial.print(F("LTC2945 Has Been Shutdown\n"));
  return(ack);
}

//! change Settings
void menu_5_settings(int8_t *VDD_or_SENSE_monitoring,   //!< Choose whether VIN is sensed at VDD pin or SENSE pin (A2 bit of CONTROL Register A)
                     int8_t *ADIN_or_SENSE,             //!< Choose whether power multiplier uses ADIN pin or SENSE pin as voltage input (A0 bit in CONTROL Register A)
                     float *scale)                      //!< Stores division ratio for resistive divider on ADIN pin.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  Serial.print(F("VIN Monitor"));
  Serial.print(F("\n0-VDD or 1-Sense+ Monitoring (Default=1)\n\n"));
  *VDD_or_SENSE_monitoring = read_int();
  if ((*VDD_or_SENSE_monitoring < 0) || (*VDD_or_SENSE_monitoring > 1))
    *VDD_or_SENSE_monitoring = 1;
  Serial.println(*VDD_or_SENSE_monitoring);
  Serial.print(F("Multiplier Select\n"));
  Serial.print(F("0-ADIN or 1-SENS+/VDD (Default=1)"));
  *ADIN_or_SENSE = read_int();
  if ((*ADIN_or_SENSE < 0) || (*ADIN_or_SENSE > 1))
    *ADIN_or_SENSE = 1;
  if (*ADIN_or_SENSE == 0)
  {
    Serial.print(F("\nresistor divider needs to be set such that New FS is 2.048v to ADIN\n"));
    Serial.print(F("Enter in new full scale input voltage:"));
    float user_float;
    user_float = read_float();
    *scale = user_float / 2.048;
    Serial.println(user_float);
    Serial.print(F("Scale:"));
    Serial.println(*scale);
  }
}

//! Clear Min/Max
int8_t menu_6_clear_min_max()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;

  ack |= LTC2945_write_24_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_POWER_MSB2_REG, LTC2945_MAX_POWER_MSB2_RESET);
  ack |= LTC2945_write_24_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_POWER_MSB2_REG, LTC2945_MIN_POWER_MSB2_RESET);
  ack |= LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_DELTA_SENSE_MSB_REG, LTC2945_MAX_DELTA_SENSE_MSB_RESET);
  ack |= LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_DELTA_SENSE_MSB_REG, LTC2945_MIN_DELTA_SENSE_MSB_RESET);
  ack |= LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_VIN_MSB_REG, LTC2945_MAX_VIN_MSB_RESET);
  ack |= LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_VIN_MSB_REG, LTC2945_MIN_VIN_MSB_RESET);
  ack |= LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_ADIN_MSB_REG, LTC2945_MAX_ADIN_MSB_RESET);
  ack |= LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_ADIN_MSB_REG, LTC2945_MIN_ADIN_MSB_RESET);
  Serial.println();
  return(ack);
}

//! Used to manipulate EEPROM data.
union eeprom_data_union
{
  struct data_struct_type               //! EEPROM data structure
  {
    int16_t cal_key;                    //!< The key that keeps track of the calibration
    uint8_t alert;                      //!< Used to store the alert settings
    int32_t max_power_threshold;        //!< Used to store max power alert settings
    int32_t min_power_threshold;        //!< Used to store min power alert settings
    uint16_t max_delta_sense_thresh;    //!< Used to store max delta sense alert settings
    uint16_t min_delta_sense_thresh;    //!< Used to store min delta sense alert settings
    uint16_t max_Vin_thresh;            //!< Used to store max Vin alert settings
    uint16_t min_Vin_thresh;            //!< Used to store min Vin alert settings
    uint16_t max_adin_thresh;           //!< Used to store adin max alert settings
    uint16_t min_adin_thresh;           //!< Used to store adin min alert settings
  } data_struct;                        //!< Name of structure

  char byte_array[sizeof(data_struct_type)]; //!< Array used to store the structure
};

//! Read the alert settings from EEPROM
int8_t restore_alert_settings()
//! @return Return 1 if successful, 0 if not
{
  int16_t cal_key;

  // Read the cal key from the EEPROM
  eeprom_read_int16(EEPROM_I2C_ADDRESS, &cal_key, EEPROM_CAL_STATUS_ADDRESS);

  if (cal_key == EEPROM_CAL_KEY)
  {
    // Calibration has been stored, read thresholds

    eeprom_data_union eeprom;

    eeprom_read_byte_array(EEPROM_I2C_ADDRESS, eeprom.byte_array, EEPROM_CAL_STATUS_ADDRESS, sizeof(eeprom_data_union));
    alert_code = eeprom.data_struct.alert;  // Global variable used to communicate with loop()

    LTC2945_write(LTC2945_I2C_ADDRESS, LTC2945_ALERT_REG, eeprom.data_struct.alert);
    LTC2945_write_24_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_POWER_THRESHOLD_MSB2_REG, eeprom.data_struct.max_power_threshold);
    LTC2945_write_24_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_POWER_THRESHOLD_MSB2_REG, eeprom.data_struct.min_power_threshold);
    LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_DELTA_SENSE_THRESHOLD_MSB_REG, eeprom.data_struct.max_delta_sense_thresh);
    LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_DELTA_SENSE_THRESHOLD_MSB_REG, eeprom.data_struct.min_delta_sense_thresh);
    LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_VIN_THRESHOLD_MSB_REG, eeprom.data_struct.max_Vin_thresh);
    LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_VIN_THRESHOLD_MSB_REG, eeprom.data_struct.min_Vin_thresh);
    LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_ADIN_THRESHOLD_MSB_REG, eeprom.data_struct.max_adin_thresh);
    LTC2945_write_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_ADIN_THRESHOLD_MSB_REG, eeprom.data_struct.min_adin_thresh);

    Serial.println("Alert Settings Restored");
    return(1);
  }
  else
  {
    Serial.println("Alert Settings not found");
    return(0);
  }
}

//! Store the alert settings to the EEPROM
void store_alert_settings()
{
  eeprom_data_union eeprom;

  eeprom.data_struct.cal_key = EEPROM_CAL_KEY;

  LTC2945_read(LTC2945_I2C_ADDRESS, LTC2945_ALERT_REG, &eeprom.data_struct.alert);
  LTC2945_read_24_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_POWER_THRESHOLD_MSB2_REG, &eeprom.data_struct.max_power_threshold);
  LTC2945_read_24_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_POWER_THRESHOLD_MSB2_REG, &eeprom.data_struct.min_power_threshold);
  LTC2945_read_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_DELTA_SENSE_THRESHOLD_MSB_REG, &eeprom.data_struct.max_delta_sense_thresh);
  LTC2945_read_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_DELTA_SENSE_THRESHOLD_MSB_REG, &eeprom.data_struct.min_delta_sense_thresh);
  LTC2945_read_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_VIN_THRESHOLD_MSB_REG, &eeprom.data_struct.max_Vin_thresh);
  LTC2945_read_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_VIN_THRESHOLD_MSB_REG, &eeprom.data_struct.min_Vin_thresh);
  LTC2945_read_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MAX_ADIN_THRESHOLD_MSB_REG, &eeprom.data_struct.max_adin_thresh);
  LTC2945_read_16_bits(LTC2945_I2C_ADDRESS, LTC2945_MIN_ADIN_THRESHOLD_MSB_REG, &eeprom.data_struct.min_adin_thresh);

  eeprom_write_byte_array(EEPROM_I2C_ADDRESS, eeprom.byte_array, EEPROM_CAL_STATUS_ADDRESS, sizeof(eeprom_data_union));

  Serial.println("Alert Settings Stored to EEPROM");
}

