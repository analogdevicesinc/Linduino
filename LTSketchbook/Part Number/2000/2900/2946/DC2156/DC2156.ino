/*!
DC2156A
LTC2946: 12-Bit Wide Range Power, Charge and Energy Monitor

@verbatim

Setting the Alert Thresholds:
    1. Select the Alert option from the main menu.
    2. Select the desired Thresholds to be changed. Then enter the minimum and maximum
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
 Refer to demo manual DC2156A.

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
    @ingroup LTC2946
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC2946.h"
#include <Wire.h>
#include <SPI.h>

// Function Declaration
void print_title();                 // Print the title block
void print_prompt();                // Print the Prompt
void store_alert_settings();        // Store the alert settings to the EEPROM
int8_t restore_alert_settings();    // Read the alert settings from EEPROM

int8_t menu_1_continuous_mode(uint8_t CTRLA, uint8_t VOLTAGE_SEL, float scale);
int8_t menu_2_snapshot_mode(uint8_t VOLTAGE_SEL, float scale);
int8_t menu_2_snapshot_mode_menu_1_SENSE();
int8_t menu_2_snapshot_mode_menu_2_ADIN(float scale);
int8_t menu_2_snapshot_mode_menu_3_VDD();
int8_t menu_2_snapshot_mode_menu_4_Current();
int8_t menu_3_alert(uint8_t VOLTAGE_SEL, float scale);
int8_t menu_3_alert_menu_1_set_power_alerts(uint8_t VOLTAGE_SEL, float scale);
int8_t menu_3_alert_menu_2_set_current_alerts();
int8_t menu_3_alert_menu_3_set_vin_alerts();
int8_t menu_3_alert_menu_4_set_adin_alerts(float scale);
int8_t menu_3_alert_menu_5_enable_disable_alerts();
int8_t menu_3_alert_menu_5_enable_disable_alerts_menu_1_ALERT1();
int8_t menu_3_alert_menu_5_enable_disable_alerts_menu_2_ALERT2();
int8_t menu_3_alert_menu_6_read_clear_faults();
int8_t menu_4_shutdown(uint8_t *CTRLB, uint8_t *shutdown);
int8_t menu_5_settings(uint8_t *CTRLA, uint8_t *CTRLB, uint8_t *VOLTAGE_SEL, uint8_t *GPIO_CFG, uint8_t *GPIO3_CTRL, float *scale);
int8_t menu_5_settings_menu_1_configure_CTRLA_reg(uint8_t *CTRLA, uint8_t *VOLTAGE_SEL);
void menu_5_settings_menu_1_configure_CTRLA_reg_menu_1_ADIN_Configuration(uint8_t *CTRLA);
void menu_5_settings_menu_1_configure_CTRLA_reg_menu_2_Offset_Configuration(uint8_t *CTRLA);
void menu_5_settings_menu_1_configure_CTRLA_reg_menu_3_Voltage_Configuration(uint8_t *CTRLA, uint8_t *VOLTAGE_SEL);
void menu_5_settings_menu_1_configure_CTRLA_reg_menu_4_Channel_Configuration(uint8_t *CTRLA);
int8_t menu_5_settings_menu_2_configure_CTRLB_reg(uint8_t *CTRLB);
int8_t menu_5_settings_menu_3_configure_GPIO(uint8_t *GPIO_CFG, uint8_t *GPIO3_CTRL);
void menu_5_settings_menu_4_scaling_for_ADIN(uint8_t *scale);
int8_t menu_6_clear_min_max();

#define CONTINUOUS_MODE_DISPLAY_DELAY 2000                  //!< The delay between readings

const float resistor = .02;         //!< resistor value on demo board
const float CLK_FREQ = 4E6;         //!< CLK Frequency


// LSB Weights
const float LTC2946_ADIN_lsb = 5.001221E-04;                      //!< Typical ADIN lsb weight in volts
const float LTC2946_DELTA_SENSE_lsb = 2.5006105E-05;              //!< Typical Delta lsb weight in volts
const float LTC2946_VIN_lsb = 2.5006105E-02;                      //!< Typical VIN lsb weight in volts
const float LTC2946_Power_lsb = 6.25305E-07;                      //!< Typical POWER lsb weight in V^2 VIN_lsb * DELTA_SENSE_lsb
const float LTC2946_ADIN_DELTA_SENSE_lsb = 1.25061E-08;           //!< Typical sense lsb weight in V^2  *ADIN_lsb * DELTA_SENSE_lsb
const float LTC2946_INTERNAL_TIME_lsb = 4101.00/250000.00;        //!< Internal TimeBase lsb. Use LTC2946_TIME_lsb if an external CLK is used. See Settings menu for how to calculate Time LSB.

static float LTC2946_TIME_lsb = 16.39543E-3;                       //!< Static variable which is based off of the default clk frequency of 250KHz.

// Error string
const char ack_error[] = "Error: No Acknowledge. Check I2C Address."; //!< Error message

// Global variables
static int8_t demo_board_connected;        //!< Set to 1 if the board is connected
static uint8_t alert1_code = 0;             //!< Value stored or read from ALERT1 register.  Shared between loop() and restore_alert_settings()
static uint8_t alert2_code = 0;            //!< Value stored or read from ALERT2 register.  Shared between loop() and restore_alert_settings()
static bool internalCLK = true;

//! Initialize Linduino
void setup()
{
  char demo_name[] = "DC2156";      // Demo Board Name stored in QuikEval EEPROM

  quikeval_I2C_init();              //! Configure the EEPROM I2C port for 100kHz
  quikeval_I2C_connect();           //! Connects to main I2C port
  Serial.begin(115200);             //! Initialize the serial port to the PC
  print_title();                    //! Print Title
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

  static uint8_t CTRLA = LTC2946_CHANNEL_CONFIG_V_C_3|LTC2946_SENSE_PLUS|LTC2946_OFFSET_CAL_EVERY|LTC2946_ADIN_GND;  //! Set Control A register to default value.
  static uint8_t CTRLB = LTC2946_DISABLE_ALERT_CLEAR&LTC2946_DISABLE_SHUTDOWN&LTC2946_DISABLE_CLEARED_ON_READ&LTC2946_DISABLE_STUCK_BUS_RECOVER&LTC2946_ENABLE_ACC&LTC2946_DISABLE_AUTO_RESET; //! Set Control B Register to default value
  static uint8_t GPIO_CFG = LTC2946_GPIO1_OUT_LOW |LTC2946_GPIO2_IN_ACC|LTC2946_GPIO3_OUT_ALERT;  //! Set GPIO_CFG Register to Default value
  static uint8_t GPIO3_CTRL = LTC2946_GPIO3_OUT_HIGH_Z;                                           //! Set GPIO3_CTRL to Default Value
  static uint8_t VOLTAGE_SEL = LTC2946_SENSE_PLUS;                                                //! Set Voltage selection to default value.
  static uint8_t shutdown = 0;                                                                    //! Set Shutdown = 1 to put part in shutdown. This is done through menu item 4.


  static float scale = 102.4/2/046;                           //! Stores division ration for resistive divider on ADIN pin.  Configured inside "Settings" menu.

  if (demo_board_connected)                                   //! Do nothing if the demo board is not connected
  {
    if (Serial.available())                                   //! Do nothing if serial is not available
    {
      user_command = read_int();                              //! Read user input command
      if (user_command != 'm')
        Serial.println(user_command);
      Serial.println();
      ack = 0;
      switch (user_command)                                         //! Prints the appropriate submenu
      {
        case 1:
          ack |= menu_1_continuous_mode(CTRLA, VOLTAGE_SEL, scale);  //! Continuous Mode Measurement
          break ;

        case 2:
          ack |= menu_2_snapshot_mode(VOLTAGE_SEL, scale);         //! SnapShot Mode Measurement
          break;

        case 3:
          ack |= menu_3_alert(VOLTAGE_SEL, scale);                 //! Alert and Threshold Menu
          break;

        case 4:
          ack |= menu_4_shutdown(&CTRLB, &shutdown);              //! Toggle Shutdown Down Mode
          break;

        case 5:
          menu_5_settings(&CTRLA, &CTRLB, &VOLTAGE_SEL, &GPIO_CFG, &GPIO3_CTRL, &scale);  // Settings
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
  Serial.print(F("* DC2156 Demonstration Program                                  *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program communicates with the LTC2946 12-Bit Wide Range  *\n"));
  Serial.print(F("* I2C Energy and Power Monitor found on the DC2156 demo board.  *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Print the Prompt
void print_prompt()
{
  Serial.print(F("\n1-Continuous Mode\n"));
  Serial.print(F("2-Snapshot Mode\n"));
  Serial.print(F("3-Alert and Threshold Menu\n"));
  Serial.print(F("4-Toggle Shutdown Mode\n"));
  Serial.print(F("5-Settings\n"));
  Serial.print(F("6-Clear Min/Max\n\n"));
  Serial.print(F("Enter a command: "));
}

//! Continuous Mode.
int8_t menu_1_continuous_mode(uint8_t CTRLA,                    //!< CTRLA Register sets the mode in which Continious measurements are made. Configured in "Settings" menu.
                              uint8_t VOLTAGE_SEL,              //!< VOLTAGE_SEL variable represents the voltage channel selected. Scaling is done if ADIN channel is selected and resistive dividers are present.
                              float scale)                       //!< Stores division ratio for resistive divider on ADIN pin.  Configured inside "Settings" menu.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  uint8_t LTC2946_mode;

  int8_t ack = 0;

  LTC2946_mode = CTRLA;                                                         //! Set the configuration of the CTRLA Register.
  Serial.println();
  ack |= LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_CTRLA_REG, LTC2946_mode);   //! Sets the LTC2946 to continuous mode


  //! Measurement Loop. Keeps Measuring until 'm' is pressed.
  //! Measurement is done by first reading the adc code and then converting it to the respective value.
  do
  {
    if (VOLTAGE_SEL != LTC2946_ADIN)
    {

      Serial.print(F("*************************\n\n"));

      uint32_t power_code, max_power_code, min_power_code;
      ack |= LTC2946_read_24_bits(LTC2946_I2C_ADDRESS, LTC2946_POWER_MSB2_REG, &power_code);
      ack |= LTC2946_read_24_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_POWER_MSB2_REG, &max_power_code);
      ack |= LTC2946_read_24_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_POWER_MSB2_REG, &min_power_code);


      float power, max_power, min_power;  // Store power results
      power = LTC2946_code_to_power(power_code, resistor, LTC2946_Power_lsb);
      max_power = LTC2946_code_to_power(max_power_code, resistor, LTC2946_Power_lsb);
      min_power = LTC2946_code_to_power(min_power_code, resistor, LTC2946_Power_lsb);

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

      uint32_t power_code, max_power_code, min_power_code;
      ack |= LTC2946_read_24_bits(LTC2946_I2C_ADDRESS, LTC2946_POWER_MSB2_REG, &power_code);
      ack |= LTC2946_read_24_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_POWER_MSB2_REG, &max_power_code);
      ack |= LTC2946_read_24_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_POWER_MSB2_REG, &min_power_code);

      float power, max_power, min_power;
      power = LTC2946_code_to_power(power_code, resistor, LTC2946_ADIN_DELTA_SENSE_lsb) * scale;
      max_power = LTC2946_code_to_power(max_power_code, resistor, LTC2946_ADIN_DELTA_SENSE_lsb) * scale;
      min_power = LTC2946_code_to_power(min_power_code, resistor, LTC2946_ADIN_DELTA_SENSE_lsb) * scale;

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
    ack |= LTC2946_read_12_bits(LTC2946_I2C_ADDRESS, LTC2946_DELTA_SENSE_MSB_REG, &current_code);
    ack |= LTC2946_read_12_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_DELTA_SENSE_MSB_REG, &max_current_code);
    ack |= LTC2946_read_12_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_DELTA_SENSE_MSB_REG, &min_current_code);

    float current, max_current, min_current;
    current = LTC2946_code_to_current(current_code, resistor, LTC2946_DELTA_SENSE_lsb);
    max_current = LTC2946_code_to_current(max_current_code, resistor, LTC2946_DELTA_SENSE_lsb);
    min_current = LTC2946_code_to_current(min_current_code, resistor, LTC2946_DELTA_SENSE_lsb);

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
    ack |= LTC2946_read_12_bits(LTC2946_I2C_ADDRESS, LTC2946_VIN_MSB_REG, &VIN_code);
    ack |= LTC2946_read_12_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_VIN_MSB_REG, &max_VIN_code);
    ack |= LTC2946_read_12_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_VIN_MSB_REG, &min_VIN_code);

    float VIN, max_VIN, min_VIN;
    VIN = LTC2946_VIN_code_to_voltage(VIN_code , LTC2946_VIN_lsb);
    max_VIN = LTC2946_VIN_code_to_voltage(max_VIN_code, LTC2946_VIN_lsb);
    min_VIN = LTC2946_VIN_code_to_voltage(min_VIN_code, LTC2946_VIN_lsb);

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
    ack |= LTC2946_read_12_bits(LTC2946_I2C_ADDRESS, LTC2946_ADIN_MSB_REG, &ADIN_code);
    ack |= LTC2946_read_12_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_ADIN_MSB_REG, &max_ADIN_code);
    ack |= LTC2946_read_12_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_ADIN_MSB_REG, &min_ADIN_code);

    float ADIN, max_ADIN, min_ADIN;
    ADIN = LTC2946_ADIN_code_to_voltage(ADIN_code, LTC2946_ADIN_lsb)*scale;
    max_ADIN = LTC2946_ADIN_code_to_voltage(max_ADIN_code, LTC2946_ADIN_lsb)*scale;
    min_ADIN = LTC2946_ADIN_code_to_voltage(min_ADIN_code, LTC2946_ADIN_lsb)*scale;

    Serial.print(F("\n****ADIN: "));
    Serial.print(ADIN, 4);
    Serial.print(F(" V\n"));

    Serial.print(F("Max ADIN: "));
    Serial.print(max_ADIN, 4);
    Serial.print(F(" V\n"));

    Serial.print(F("Min ADIN: "));
    Serial.print(min_ADIN, 4);
    Serial.print(F(" V\n"));



    uint32_t energy_code;
    ack |= LTC2946_read_32_bits(LTC2946_I2C_ADDRESS, LTC2946_ENERGY_MSB3_REG, &energy_code);

    uint32_t charge_code;
    ack |= LTC2946_read_32_bits(LTC2946_I2C_ADDRESS, LTC2946_CHARGE_MSB3_REG, &charge_code);


    uint32_t time_code;
    ack |= LTC2946_read_32_bits(LTC2946_I2C_ADDRESS, LTC2946_TIME_COUNTER_MSB3_REG, &time_code);


    float energy,charge,time;

    if (internalCLK)
    {
      energy = LTC2946_code_to_energy(energy_code,resistor,LTC2946_Power_lsb, LTC2946_INTERNAL_TIME_lsb);
      charge = LTC2946_code_to_coulombs(charge_code,resistor,LTC2946_DELTA_SENSE_lsb, LTC2946_INTERNAL_TIME_lsb);
      time = LTC2946_code_to_time(time_code, LTC2946_INTERNAL_TIME_lsb);
    }
    else
    {
      energy = LTC2946_code_to_energy(energy_code,resistor,LTC2946_Power_lsb, LTC2946_TIME_lsb);
      charge = LTC2946_code_to_coulombs(charge_code,resistor,LTC2946_DELTA_SENSE_lsb, LTC2946_TIME_lsb);
      time = LTC2946_code_to_time(time_code, LTC2946_TIME_lsb);
    }

    Serial.print(F("**********Accumulators**********\n"));

    Serial.print(F("\n****Energy: "));
    Serial.print(energy, 4);
    Serial.print(F(" J\n"));

    Serial.print(F("****Charge: "));
    Serial.print(charge, 4);
    Serial.print(F(" C\n"));

    Serial.print(F("****Time: "));
    Serial.print(time, 4);
    Serial.print(F(" s\n\n"));

    Serial.print(F("********************************\n"));


    Serial.print(F("m-Main Menu\n\n"));
    Serial.flush();
    delay(CONTINUOUS_MODE_DISPLAY_DELAY);
  }
  while (Serial.available() == false);
  read_int();  // clears the Serial.available
  return(ack);
}

//! Snapshot Mode Menu
int8_t menu_2_snapshot_mode(uint8_t VOLTAGE_SEL,    //!< VOLTAGE_SEL variable represents the voltage channel selected. Scaling is done if ADIN channel is selected and resistive dividers are present.
                            float scale)           //!< Stores division ratio for resistive divider on ADIN pin.  Configured inside "Settings" menu.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  //! Print submenu
  Serial.print(F("*************************\n\n"));
  Serial.print(F("Snapshot Mode\n"));
  Serial.print(F("1-SENSE+\n"));
  Serial.print(F("2-ADIN\n"));
  Serial.print(F("3-VDD\n"));
  Serial.print(F("4-Current\n"));
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
      Serial.print(F("1-SENSE+\n"));
      Serial.print(F("2-ADIN\n"));
      Serial.print(F("3-VDD\n"));
      Serial.print(F("4-Current\n"));
      Serial.print(F("m-Main Menu\n"));
    }
    Serial.println();
    switch (user_command)
    {
      case 1:  // SENSE+ - Snapshot Mode
        ack |= menu_2_snapshot_mode_menu_1_SENSE();
        break;

      case 2:  // VIN - Snapshot Mode
        ack |= menu_2_snapshot_mode_menu_2_ADIN(scale);
        break;

      case 3:  // ADIN - Snapshot Mode
        ack |= menu_2_snapshot_mode_menu_3_VDD();
        break;
      case 4: //Current - Snapshot Mode
        ack |= menu_2_snapshot_mode_menu_4_Current();
        break;
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }

    if (user_command != 'm')
    {
      Serial.print(F("*************************\n\n"));
      Serial.print(F("Snapshot Mode\n"));
      Serial.print(F("1-SENSE+\n"));
      Serial.print(F("2-ADIN\n"));
      Serial.print(F("3-VDD\n"));
      Serial.print(F("4-Current\n"));
      Serial.print(F("m-Main Menu\n"));
    }

  }
  while (!((user_command == 'm') || (ack)));
  return(ack);
}

//! SENSE+ - Snapshot mode
int8_t menu_2_snapshot_mode_menu_1_SENSE()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  int8_t LTC2946_mode;
  LTC2946_mode = LTC2946_CHANNEL_CONFIG_SNAPSHOT | LTC2946_SENSE_PLUS;
  ack |= LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_CTRLA_REG, LTC2946_mode);

  uint8_t busy;
  do
  {
    ack |= LTC2946_read(LTC2946_I2C_ADDRESS, LTC2946_STATUS2_REG, &busy);
  }
  while (0x8 & busy);

  uint16_t voltage_code;
  ack |= LTC2946_read_12_bits(LTC2946_I2C_ADDRESS, LTC2946_VIN_MSB_REG, &voltage_code);

  float voltage;
  voltage = LTC2946_VIN_code_to_voltage(voltage_code, LTC2946_VIN_lsb);
  Serial.print(F("SENSE+: "));
  Serial.print(voltage, 4);
  Serial.print(F(" V\n\n"));
  return(ack);
}

//! ADIN - Snapshot Mode
int8_t menu_2_snapshot_mode_menu_2_ADIN(float scale)
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  int8_t LTC2946_mode;
  LTC2946_mode = LTC2946_CHANNEL_CONFIG_SNAPSHOT | LTC2946_ADIN;
  ack |= LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_CTRLA_REG, LTC2946_mode);

  uint8_t busy;
  do
  {
    ack |= LTC2946_read(LTC2946_I2C_ADDRESS, LTC2946_STATUS2_REG, &busy);
  }
  while (0x8 & busy);

  uint16_t ADIN_code;
  ack |= LTC2946_read_12_bits(LTC2946_I2C_ADDRESS, LTC2946_ADIN_MSB_REG, &ADIN_code);

  float ADIN;
  ADIN = LTC2946_ADIN_code_to_voltage(ADIN_code, LTC2946_ADIN_lsb)*scale;
  Serial.print(F("ADIN: "));
  Serial.print(ADIN, 4);
  Serial.print(F(" V\n\n"));
  return(ack);
}

//! VDD - Snapshot Mode
int8_t menu_2_snapshot_mode_menu_3_VDD()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  int8_t LTC2946_mode;
  LTC2946_mode = LTC2946_CHANNEL_CONFIG_SNAPSHOT | LTC2946_VDD;
  ack |= LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_CTRLA_REG, LTC2946_mode);

  uint8_t busy;
  do
  {
    ack |= LTC2946_read(LTC2946_I2C_ADDRESS, LTC2946_STATUS2_REG, &busy);
  }
  while (0x8 & busy);

  uint16_t VDD_code;
  ack |= LTC2946_read_12_bits(LTC2946_I2C_ADDRESS, LTC2946_VIN_MSB_REG, &VDD_code);

  float VDD;
  VDD = LTC2946_VIN_code_to_voltage(VDD_code, LTC2946_VIN_lsb);
  Serial.print(F("VDD: "));
  Serial.print(VDD, 4);
  Serial.print(F(" V\n\n"));
  return(ack);
}

//! Current - Snapshot Mode
int8_t menu_2_snapshot_mode_menu_4_Current()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  int8_t LTC2946_mode;
  LTC2946_mode = LTC2946_CHANNEL_CONFIG_SNAPSHOT | LTC2946_DELTA_SENSE;
  ack |= LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_CTRLA_REG, LTC2946_mode);

  uint8_t busy;
  do
  {
    ack |= LTC2946_read(LTC2946_I2C_ADDRESS, LTC2946_STATUS2_REG, &busy);        //!< Check to see if conversion is still in process
  }
  while (0x8 & busy);

  uint16_t current_code;
  ack |= LTC2946_read_12_bits(LTC2946_I2C_ADDRESS, LTC2946_DELTA_SENSE_MSB_REG, &current_code);

  float current;
  current = LTC2946_code_to_current(current_code, resistor, LTC2946_DELTA_SENSE_lsb);
  Serial.print(F("Current: "));
  Serial.print(current, 4);
  Serial.print(F(" A\n\n"));
  return(ack);
}


//! Alert and Threshold Menu
int8_t menu_3_alert(uint8_t VOLTAGE_SEL,   //!< Voltage Selection Variable.
                    float scale)   //!< Stores division ratio for resistive divider on ADIN pin.  Configured inside "Settings" menu.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  int8_t user_command;
  do
  {
    //Print Sub-Menu
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Set Power Alert Thresholds\n"));
    Serial.print(F("2-Set Current Alert Thresholds\n"));
    Serial.print(F("3-Set VIN Alert Thresholds\n"));
    Serial.print(F("4-Set ADIN Alert Thresholds\n"));
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
        ack |= menu_3_alert_menu_1_set_power_alerts(VOLTAGE_SEL, scale); // Set Power Alert Thresholds
        break;

      case 2:
        ack |= menu_3_alert_menu_2_set_current_alerts();            // Set Current Alert Thresholds
        break;

      case 3:
        ack |= menu_3_alert_menu_3_set_vin_alerts();                // Set VIN Alert Thresholds
        break;

      case 4:
        ack |= menu_3_alert_menu_4_set_adin_alerts(scale);               // Set ADIN Alert Thresholds
        break;

      case 5:
        ack |= menu_3_alert_menu_5_enable_disable_alerts();         // Enable/Disable Alert Menu
        break;

      case 6:
        ack |= menu_3_alert_menu_6_read_clear_faults();             // Read Fault Register
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

//! Set Power Alert Thresholds
int8_t menu_3_alert_menu_1_set_power_alerts(uint8_t VOLTAGE_SEL,   //!< Choose whether power multiplier uses ADIN pin or SENSE pin as voltage input (A0 bit in CONTROL Register A)
    float scale)   //!< Scale value based on resistive divider on the ADIN pin.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  Serial.print(F("Enter Max Power Threshold:"));

  float max_power_threshold;
  max_power_threshold = read_float();
  Serial.println(max_power_threshold, 4);

  int32_t max_power_threshold_code;
  if (VOLTAGE_SEL != LTC2946_ADIN)
    max_power_threshold_code = (max_power_threshold / LTC2946_Power_lsb) * resistor;
  else
    max_power_threshold_code = ((1.0/scale)*(max_power_threshold / LTC2946_ADIN_DELTA_SENSE_lsb)) * resistor;

  ack |= LTC2946_write_24_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_POWER_THRESHOLD_MSB2_REG, max_power_threshold_code);

  Serial.print(F("Enter Min Power Threshold:"));

  float min_power_threshold;
  min_power_threshold = read_float();
  Serial.println(min_power_threshold, 4);

  int32_t min_power_threshold_code;
  if (VOLTAGE_SEL != LTC2946_ADIN)
    min_power_threshold_code = (min_power_threshold / LTC2946_Power_lsb) * resistor;
  else
    min_power_threshold_code = ((1.0/scale)*(min_power_threshold / LTC2946_ADIN_DELTA_SENSE_lsb)) * resistor;
  ack |= LTC2946_write_24_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_POWER_THRESHOLD_MSB2_REG, min_power_threshold_code);
  return(ack);
}

//! Set Current Alert Thresholds
int8_t menu_3_alert_menu_2_set_current_alerts()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  Serial.print(F("Enter Max Current Threshold:"));

  float max_current_threshold;
  max_current_threshold = read_float();
  Serial.println(max_current_threshold, 4);

  int32_t max_current_threshold_code;
  max_current_threshold_code = (max_current_threshold / LTC2946_DELTA_SENSE_lsb) * resistor;

  ack |= LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_DELTA_SENSE_THRESHOLD_MSB_REG, (max_current_threshold_code << 4));

  Serial.print(F("Enter Min Current Threshold:"));

  float min_current_threshold;
  min_current_threshold = read_float();
  Serial.println(min_current_threshold, 4);

  int32_t min_current_threshold_code;
  min_current_threshold_code = (min_current_threshold / LTC2946_DELTA_SENSE_lsb) * resistor;
  ack |= LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_DELTA_SENSE_THRESHOLD_MSB_REG, (min_current_threshold_code << 4));
  return(ack);
}

//! Set VIN Alert Thresholds
int8_t menu_3_alert_menu_3_set_vin_alerts()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  Serial.print(F("Enter Max VIN Threshold:"));

  float max_VIN_threshold;
  max_VIN_threshold = read_float();
  Serial.println(max_VIN_threshold, 4);

  int32_t max_VIN_threshold_code;
  max_VIN_threshold_code = max_VIN_threshold / LTC2946_VIN_lsb;
  ack |= LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_VIN_THRESHOLD_MSB_REG, (max_VIN_threshold_code << 4));

  Serial.print(F("Enter Min VIN Threshold:"));

  float min_VIN_threshold;
  min_VIN_threshold = read_float();
  Serial.println(min_VIN_threshold, 4);

  int32_t min_VIN_threshold_code;
  min_VIN_threshold_code = min_VIN_threshold / LTC2946_VIN_lsb;
  ack |= LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_VIN_THRESHOLD_MSB_REG, (min_VIN_threshold_code << 4));
  return(ack);
}

//! Set ADIN Alert Thresholds
int8_t menu_3_alert_menu_4_set_adin_alerts(float scale     //!< Scale value based on resistive divider on the ADIN pin.
                                          )
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  Serial.print(F("Enter Max ADIN Threshold:"));

  float max_ADIN_threshold;
  max_ADIN_threshold = read_float();
  Serial.println(max_ADIN_threshold, 4);

  int32_t max_ADIN_threshold_code;
  max_ADIN_threshold_code = ((1.0/scale)*max_ADIN_threshold) / LTC2946_ADIN_lsb;
  ack |= LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_ADIN_THRESHOLD_MSB_REG, (max_ADIN_threshold_code << 4));

  Serial.print(F("Enter Min ADIN Threshold:"));

  float min_ADIN_threshold;
  min_ADIN_threshold = read_float();
  Serial.println(min_ADIN_threshold, 4);

  int32_t min_ADIN_threshold_code;
  min_ADIN_threshold_code = ((1.0/scale)*min_ADIN_threshold) / LTC2946_ADIN_lsb;
  ack |= LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_ADIN_THRESHOLD_MSB_REG, (min_ADIN_threshold_code << 4));
  return(ack);
}


//! Enable/Disable Alert Menu
int8_t menu_3_alert_menu_5_enable_disable_alerts()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  uint8_t user_command;
  do
  {
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Enable/Disable ALERT1 Alerts\n"));
    Serial.print(F("2-Enable/Disable ALERT2 Alerts\n"));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println('m');
    else
      Serial.println(user_command);
    switch (user_command)
    {
      case 1: // Enable/Disable Alerts in ALERT1 Register
        ack |= menu_3_alert_menu_5_enable_disable_alerts_menu_1_ALERT1();
        break;
      case 2: // Enable/Disable Alerts in ALERT2 Register
        ack |= menu_3_alert_menu_5_enable_disable_alerts_menu_2_ALERT2();
        break;
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }

  }
  while (user_command != 'm');

  return(ack);
}

//! Enable/Disable Alert in ALERT1 Register
int8_t menu_3_alert_menu_5_enable_disable_alerts_menu_1_ALERT1()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  uint8_t user_command;
  do
  {
    //Print Sub-Menu
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
        Serial.print(F("Max Power :"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert1_code = alert1_code | LTC2946_ENABLE_MAX_POWER_ALERT;
        else
          alert1_code = alert1_code & LTC2946_DISABLE_MAX_POWER_ALERT;

        Serial.print(F("Min Power :"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert1_code = alert1_code | LTC2946_ENABLE_MIN_POWER_ALERT;
        else
          alert1_code = alert1_code & LTC2946_DISABLE_MIN_POWER_ALERT;
        break;
      case 2:
        // ENABLE/Disable Current Alerts
        Serial.print(F("Max Current :"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert1_code = alert1_code | LTC2946_ENABLE_MAX_I_SENSE_ALERT;
        else
          alert1_code = alert1_code & LTC2946_DISABLE_MAX_I_SENSE_ALERT;

        Serial.print(F("Min Current :"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert1_code = alert1_code | LTC2946_ENABLE_MIN_I_SENSE_ALERT;
        else
          alert1_code = alert1_code & LTC2946_DISABLE_MIN_I_SENSE_ALERT;
        break;
      case 3:
        // ENABLE/Disable VIN Alerts
        Serial.print(F("Max VIN :"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert1_code = alert1_code | LTC2946_ENABLE_MAX_VIN_ALERT;
        else
          alert1_code = alert1_code & LTC2946_DISABLE_MAX_VIN_ALERT;

        Serial.print(F("Min VIN :"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert1_code = alert1_code | LTC2946_ENABLE_MIN_VIN_ALERT;
        else
          alert1_code = alert1_code & LTC2946_DISABLE_MIN_VIN_ALERT;
        break;
      case 4:
        // ENABLE/Disable ADIN Alerts
        Serial.print(F("Max ADIN :"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert1_code = alert1_code | LTC2946_ENABLE_MAX_ADIN_ALERT;
        else
          alert1_code = alert1_code & LTC2946_DISABLE_MAX_ADIN_ALERT;

        Serial.print(F("Min ADIN :"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert1_code = alert1_code | LTC2946_ENABLE_MIN_ADIN_ALERT;
        else
          alert1_code = alert1_code & LTC2946_DISABLE_MIN_ADIN_ALERT;
        break;
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }
  }
  while (user_command != 'm');
  ack |= LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_ALERT1_REG, alert1_code);
  return(ack);
}

// Enable/Disable alerts in ALERT2 register
int8_t menu_3_alert_menu_5_enable_disable_alerts_menu_2_ALERT2()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  uint8_t user_command;
  do
  {
    //Print Menu
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Enable/Disable ADC Conversion Alert\n"));
    Serial.print(F("2-Enable/Disable GPIO1 Input Alert\n"));
    Serial.print(F("3-Enable/Disable GPIO2 Input Alert\n"));
    Serial.print(F("4-Enable/Disable Stuck-Bus Timeout Wake-up Alert\n"));
    Serial.print(F("5-Enable/Disable Energy Overflow Alert\n"));
    Serial.print(F("6-Enable/Disable Charge Overflow Alert\n"));
    Serial.print(F("7-Enable/Disable Time Counter Overflow Alert\n"));
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
        // Enable/Disable ADC Conversion Alert
        Serial.print(F("ADC Conv Done: "));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert2_code = alert2_code | LTC2946_ENABLE_ADC_DONE_ALERT;
        else
          alert2_code = alert2_code & ~LTC2946_ENABLE_ADC_DONE_ALERT ;
        break;
      case 2:
        // Enable/Disable GPIO1 Input Alert
        Serial.print(F("GPIO1 Input Alert: "));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert2_code = alert2_code | LTC2946_ENABLE_GPIO_1_ALERT;
        else
          alert2_code = alert2_code & LTC2946_DISABLE_GPIO_1_ALERT;
        break;
      case 3:
        // Enable/Disable GPIO2 Input Alert
        Serial.print(F("GPIO2 Input Alert: "));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert2_code = alert2_code | LTC2946_ENABLE_GPIO_2_ALERT;
        else
          alert2_code = alert2_code & LTC2946_DISABLE_GPIO_2_ALERT;
        break;
      case 4:
        // Enable/Disable Stuck-Bus Timeout Wake-Up Alert
        Serial.print(F("Stuck-Bus Timeout Wake-Up Alert: "));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert2_code = alert2_code | LTC2946_ENABLE_STUCK_BUS_WAKE_ALERT;
        else
          alert2_code = alert2_code & LTC2946_DISABLE_STUCK_BUS_WAKE_ALERT;
        break;
      case 5:
        // Enable/Disable Energy Overflow Alert
        Serial.print(F("Energy Overflow Alert: "));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert2_code = alert2_code | LTC2946_ENABLE_ENERGY_OVERFLOW_ALERT;
        else
          alert2_code = alert2_code & LTC2946_DISABLE_ENERGY_OVERFLOW_ALERT;
        break;
      case 6:
        // Enable/Disable Charge Overflow Alert
        Serial.print(F("Charge Overflow Alert: "));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert2_code = alert2_code | LTC2946_ENABLE_CHARGE_OVERFLOW_ALERT;
        else
          alert2_code = alert2_code & LTC2946_DISABLE_CHARGE_OVERFLOW_ALERT;
        break;
      case 7:
        // Enable/Disable Time Counter Overflow Alert
        Serial.print(F("Time Counter Overflow Alert: "));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          alert2_code = alert2_code | LTC2946_ENABLE_COUNTER_OVERFLOW_ALERT;
        else
          alert2_code = alert2_code & LTC2946_DISABLE_COUNTER_OVERFLOW_ALERT;
        break;
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }
  }
  while (user_command != 'm');
  ack |= LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_ALERT2_REG, alert2_code);
  return(ack);
}

//! Read Faults and Clear
int8_t menu_3_alert_menu_6_read_clear_faults()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  uint8_t fault_code = 0;
  ack |= LTC2946_read(LTC2946_I2C_ADDRESS, LTC2946_FAULT1_REG, &fault_code);
  if (fault_code & 0x80) Serial.print(F("Power Overvalue Fault Occurred\n\n"));
  if (fault_code & 0x40) Serial.print(F("Power Undervalue Fault Occurred\n"));
  if (fault_code & 0x20) Serial.print(F("Current Overvalue Fault Occurred\n"));
  if (fault_code & 0x10) Serial.print(F("Current Undervalue Fault Occurred\n"));
  if (fault_code & 0x08) Serial.print(F("VIN Overvalue Fault Occurred\n"));
  if (fault_code & 0x04) Serial.print(F("VIN Undervalue Fault Occurred\n"));
  if (fault_code & 0x02) Serial.print(F("ADIN Overvalue Fault Occurred\n"));
  if (fault_code & 0x01) Serial.print(F("ADIN Undervalue Fault Occurred\n"));
  ack |= LTC2946_read(LTC2946_I2C_ADDRESS, LTC2946_FAULT2_REG, &fault_code);
  if (fault_code & 0x40) Serial.print(F("GPIO1 Input Fault Occurred\n"));
  if (fault_code & 0x20) Serial.print(F("GPIO2 Input Fault Occurred\n"));
  if (fault_code & 0x10) Serial.print(F("GPIO3 Input Fault Occurred\n"));
  if (fault_code & 0x08) Serial.print(F("Stuck-Bus Timeout Fault Occurred\n"));
  if (fault_code & 0x04) Serial.print(F("Energy Overflow Fault Occurred\n"));
  if (fault_code & 0x02) Serial.print(F("Charge Overflow Fault Occurred\n"));
  if (fault_code & 0x01) Serial.print(F("Time Counter Overflow Fault Occurred\n"));
  if (!fault_code) Serial.print(F("No Alarms Were Present\n\n"));
  else
  {
    fault_code = 0;
    ack |= LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_FAULT1_REG, fault_code);
    ack |= LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_FAULT2_REG, fault_code);
  }

  return(ack);
}

//! Toggle Shutdown Down Mode
int8_t menu_4_shutdown(uint8_t *CTRLB, uint8_t *shutdown)
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  if (*shutdown == 0)
  {
    *CTRLB = *CTRLB | LTC2946_ENABLE_SHUTDOWN;
    ack |= LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_CTRLB_REG, *CTRLB);
    *shutdown = 1;
    Serial.print(F("LTC2946 Has Been Shutdown\n\n"));
  }
  else
  {
    *CTRLB = *CTRLB & LTC2946_DISABLE_SHUTDOWN;
    ack |= LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_CTRLB_REG, *CTRLB);
    *shutdown = 0;
    Serial.print(F("LTC2946 Is No Longer in Shutdown\n\n"));
  }


  return(ack);
}



//! Change Settings
int8_t menu_5_settings(uint8_t *CTRLA,           //!< Local Copy of Configuration in CTRLA Register.
                       uint8_t *CTRLB,             //!< Local Copy of Configuration in CTRLB Register.
                       uint8_t *VOLTAGE_SEL,       //!< Local Copy of Voltage Selection Channel.
                       uint8_t *GPIO_CFG,          //!< Local Copy of GPIO_CFG Register. Allows Configuration of GPIO1 and GPIO2 Pin states.
                       uint8_t *GPIO3_CTRL,        //!< Local Copy of GPIO3_CTRL Register. Allows Configuration of GPIO3 Pin
                       float *scale)               //!< Stores division ratio for resistive divider on ADIN pin.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  uint8_t user_command;
  do
  {
    //Print Sub Menu
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Configure CTRLA Register\n"));
    Serial.print(F("2-Configure CTRLB Register\n"));
    Serial.print(F("3-Configure GPIO Pins\n"));
    Serial.print(F("4-Set scaling value for ADIN pin\n"));
    Serial.print(F("m-Main Menu\n\n"));
    Serial.print(F("Enter a command:"));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println('m');
    else
      Serial.println(user_command);
    switch (user_command)
    {
      case 1:
        ack |= menu_5_settings_menu_1_configure_CTRLA_reg(CTRLA, VOLTAGE_SEL); //Configure CTRLA Register.
        break;
      case 2:
        ack |= menu_5_settings_menu_2_configure_CTRLB_reg(CTRLB);              //Configure CTRLB Register.
        break;
      case 3:
        ack |= menu_5_settings_menu_3_configure_GPIO(GPIO_CFG, GPIO3_CTRL);    //Configure GPIO pins.
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }

  }
  while (user_command != 'm');

  return(ack);
}
//! Configure CTRLA Register.
int8_t menu_5_settings_menu_1_configure_CTRLA_reg(uint8_t *CTRLA,               //!<Local Copy of CTRLA Register.
    uint8_t *VOLTAGE_SEL)         //!<Local Copy of VOLTAGE_SEL. Indicates which voltage channel has been selected for power measurement.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.

{
  int8_t ack = 0;
  uint8_t user_command;
  Serial.println(*CTRLA);
  do
  {
    // Print Sub-Menu
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Configure ADIN reference\n"));
    Serial.print(F("2-Configure Offset Calibration\n"));
    Serial.print(F("3-Configure Voltage Selection\n"));
    Serial.print(F("4-Configure Measurement Channel\n"));
    Serial.print(F("m-Settings Menu\n\n"));
    Serial.print(F("Enter a command:"));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println('m');
    else
      Serial.println(user_command);
    switch (user_command)
    {
      case 1:
        menu_5_settings_menu_1_configure_CTRLA_reg_menu_1_ADIN_Configuration(CTRLA);     //!< Configure the ADIN reference to GND/INTVCC
        break;
      case 2:
        menu_5_settings_menu_1_configure_CTRLA_reg_menu_2_Offset_Configuration(CTRLA);    //!< Configure frequency of offset calibration
        break;
      case 3:
        menu_5_settings_menu_1_configure_CTRLA_reg_menu_3_Voltage_Configuration(CTRLA, VOLTAGE_SEL);  //!< Select the voltage measurement channel
        break;
      case 4:
        menu_5_settings_menu_1_configure_CTRLA_reg_menu_4_Channel_Configuration(CTRLA);    //!< Select duty cycle of measurments and channel configuration.
        break;
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }
  }
  while (user_command != 'm');

  ack |= LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_CTRLA_REG, *CTRLA);

  return(ack);
}


//! Configure the ADIN reference to GND/INTVCC
void menu_5_settings_menu_1_configure_CTRLA_reg_menu_1_ADIN_Configuration(uint8_t *CTRLA)
{

  uint8_t user_command;
  //Print Sub-Menu
  Serial.print(F("1-ADIN Measured with Respect to INTVCC\n"));
  Serial.print(F("2-ADIN Measured with Respect to GND\n\n"));
  Serial.print(F("Enter a command:"));
  user_command = read_int();
  if ((user_command > 2) || (user_command < 1))
    user_command = 2;
  Serial.println(user_command);
  if (user_command == 1)
  {
    *CTRLA = *CTRLA & LTC2946_CTRLA_ADIN_MASK;
    *CTRLA = *CTRLA | LTC2946_ADIN_INTVCC;
    Serial.print(F("CTRLA set to measure ADIN with respect to INTVCC\n"));
  }
  else
  {
    *CTRLA = *CTRLA & LTC2946_CTRLA_ADIN_MASK;
    *CTRLA = *CTRLA | LTC2946_ADIN_GND;
    Serial.print(F("CTRLA set to measure ADIN with respect to GND\n"));
  }

}

//! Configure frequency of offset calibration
void menu_5_settings_menu_1_configure_CTRLA_reg_menu_2_Offset_Configuration(uint8_t *CTRLA)
{
  uint8_t user_command;
  //Print Sub-Menu
  Serial.print(F("1-1st powerup or use last calibrated result\n"));
  Serial.print(F("2-Once every 128 conversions\n"));
  Serial.print(F("3-Once every 16 conversions\n"));
  Serial.print(F("4-Every conversion\n\n"));
  Serial.print(F("Enter a command:"));
  user_command = read_int();
  Serial.println(user_command);
  switch (user_command)
  {
    case 1:
      *CTRLA = *CTRLA & LTC2946_CTRLA_OFFSET_MASK;
      *CTRLA = *CTRLA | LTC2946_OFFSET_CAL_LAST;
      Serial.print(F("Offset Calibration set to use last calibrated result\n"));
      break;
    case 2:
      *CTRLA = *CTRLA & LTC2946_CTRLA_OFFSET_MASK;
      *CTRLA = *CTRLA | LTC2946_OFFSET_CAL_128;
      Serial.print(F("Offset Calibration set to once every 128 conversions\n"));
      break;
    case 3:
      *CTRLA = *CTRLA & LTC2946_CTRLA_OFFSET_MASK;
      *CTRLA = *CTRLA | LTC2946_OFFSET_CAL_16;
      Serial.print(F("Offset Calibration set to once every 16 conversions\n"));
      break;
    case 4:
      *CTRLA = *CTRLA & LTC2946_CTRLA_OFFSET_MASK;
      *CTRLA = *CTRLA | LTC2946_OFFSET_CAL_EVERY;
      Serial.print(F("Offset Calibration set to once every conversion\n"));
      break;
    default:
      if (user_command != 'm')
        Serial.println("Incorrect Option");
      break;
  }
}

//! Select the voltage measurement channel
void menu_5_settings_menu_1_configure_CTRLA_reg_menu_3_Voltage_Configuration(uint8_t *CTRLA,            //!< Local copy of CTRLA register.
    uint8_t *VOLTAGE_SEL)      //!< Voltage selection variable.
{
  uint8_t user_command;
  Serial.print(F("1-SENSE+:\n"));
  Serial.print(F("2-ADIN:\n"));
  Serial.print(F("3-VDD:\n\n"));
  Serial.print(F("Enter a command:"));
  user_command = read_int();
  if ((user_command > 3) || (user_command < 0))
    user_command = 1;
  Serial.println(user_command);

  switch (user_command)
  {
    case 1:
      *CTRLA = *CTRLA & LTC2946_CTRLA_VOLTAGE_SEL_MASK;
      *CTRLA = *CTRLA | LTC2946_SENSE_PLUS;
      *VOLTAGE_SEL = LTC2946_SENSE_PLUS;
      Serial.print(F("SENSE+ selected\n"));
      break;
    case 2:
      *CTRLA = *CTRLA & LTC2946_CTRLA_VOLTAGE_SEL_MASK;
      *CTRLA = *CTRLA | LTC2946_ADIN;
      *VOLTAGE_SEL = LTC2946_ADIN;
      Serial.print(F("ADIN selected\n"));
      break;
    case 3:
      *CTRLA = *CTRLA & LTC2946_CTRLA_VOLTAGE_SEL_MASK;
      *CTRLA = *CTRLA | LTC2946_VDD;
      *VOLTAGE_SEL = LTC2946_VDD;
      Serial.print(F("VDD selected\n"));
      break;
    default:
      if (user_command != 'm')
        Serial.println("Incorrect Option");
      break;
  }
}

//!Select duty cycle of measurments and channel configuration.
void menu_5_settings_menu_1_configure_CTRLA_reg_menu_4_Channel_Configuration(uint8_t *CTRLA)  //!< Local copy of CTRLA Register
{

  uint8_t user_command;
  //Print Sub-Menu
  Serial.print(F("1-Voltage Once Followed by Current Indefinetly\n"));
  Serial.print(F("2-ADIN,Voltage, Current at 1/256, 1/256 and 254/256 Duty Cycle, Respectively\n"));
  Serial.print(F("3-ADIN,Voltage, Current at 1/32, 1/32 and 30/32 Duty Cycle, Respectively\n"));
  Serial.print(F("4-Aleternate ADIN, Voltage and Current Measurement\n"));
  Serial.print(F("5-Voltage and Current at 1/128 and 127/128 Duty Cycle, Respectively\n"));
  Serial.print(F("6-Voltage and Current at 1/16 and 15/16 Duty Cycle, Respectively\n"));
  Serial.print(F("7-Alternate Voltage and Current Measurement\n\n"));
  Serial.print(F("Enter a command:"));
  user_command = read_int();
  Serial.println(user_command);
  switch (user_command)
  {
      Serial.print(F("Selected Mode = "));
    case 1:
      *CTRLA = *CTRLA & LTC2946_CTRLA_CHANNEL_CONFIG_MASK;
      *CTRLA = *CTRLA | LTC2946_CHANNEL_CONFIG_V_C;
      Serial.print(F("Voltage Once Followed by Current Indefinetly\n"));
      break;
    case 2:
      *CTRLA = *CTRLA & LTC2946_CTRLA_CHANNEL_CONFIG_MASK;
      *CTRLA = *CTRLA | LTC2946_CHANNEL_CONFIG_A_V_C_1;
      Serial.print(F("ADIN,Voltage, Current at 1/256, 1/256 and 254/256 Duty Cycle, Respectively\n"));
      break;
    case 3:
      *CTRLA = *CTRLA & LTC2946_CTRLA_CHANNEL_CONFIG_MASK;
      *CTRLA = *CTRLA | LTC2946_CHANNEL_CONFIG_A_V_C_2;
      Serial.print(F("ADIN,Voltage, Current at 1/32, 1/32 and 30/32 Duty Cycle, Respectively\n"));
      break;
    case 4:
      *CTRLA = *CTRLA & LTC2946_CTRLA_CHANNEL_CONFIG_MASK;
      *CTRLA = *CTRLA | LTC2946_CHANNEL_CONFIG_A_V_C_3;
      Serial.print(F("Aleternate ADIN, Voltage and Current Measurement\n"));
      break;
    case 5:
      *CTRLA = *CTRLA & LTC2946_CTRLA_CHANNEL_CONFIG_MASK;
      *CTRLA = *CTRLA | LTC2946_CHANNEL_CONFIG_V_C_1;
      Serial.print(F("Voltage and Current at 1/128 and 127/128 Duty Cycle, Respectively\n"));
      break;
    case 6:
      *CTRLA = *CTRLA & LTC2946_CTRLA_CHANNEL_CONFIG_MASK;
      *CTRLA = *CTRLA | LTC2946_CHANNEL_CONFIG_V_C_2;
      Serial.print(F("Voltage and Current at 1/16 and 15/16 Duty Cycle, Respectively\n"));
      break;
    case 7:
      *CTRLA = *CTRLA & LTC2946_CTRLA_CHANNEL_CONFIG_MASK;
      *CTRLA = *CTRLA | LTC2946_CHANNEL_CONFIG_V_C_3;
      Serial.print(F("Alternate Voltage and Current Measurement\n\n"));
      break;
    default:
      if (user_command != 'm')
        Serial.println("Incorrect Option");
      break;
  }

}

//!Configure CTRLB Register.
int8_t menu_5_settings_menu_2_configure_CTRLB_reg(uint8_t *CTRLB) //!< Local copy of CTRLB register.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  uint8_t user_command;
  do
  {
    //Print Sub-Menu
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Enable/Disable Alert Clear Enable\n"));
    Serial.print(F("2-Enable/Disable Shutdown\n"));
    Serial.print(F("3-Enable/Disable Clear On Read\n"));
    Serial.print(F("4-Enable/Disable Stuck Bus Timeout Auto Wake up\n"));
    Serial.print(F("5-Enable/Disable Accumulation\n"));
    Serial.print(F("6-Enable/Disable Auto Reset Mode\n"));
    Serial.print(F("m-Settings Menu\n\n"));
    Serial.print(F("Enter a command:"));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println('m');
    else
      Serial.println(user_command);

    switch (user_command)
    {

      case 1:
        Serial.print(F("1-Enable, 2-Disable\n"));
        // ENABLE/Disable ADIN Alerts
        Serial.print(F(" Alert Clear Enable:"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          *CTRLB = *CTRLB | LTC2946_ENABLE_ALERT_CLEAR;
        else
          *CTRLB = *CTRLB & LTC2946_DISABLE_ALERT_CLEAR;
        break;
      case 2:
        Serial.print(F("1-Enable, 2-Disable\n"));
        Serial.print(F(" Shutdown:"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          *CTRLB = *CTRLB | LTC2946_ENABLE_SHUTDOWN;
        else
          *CTRLB = *CTRLB & LTC2946_DISABLE_SHUTDOWN;
        break;
      case 3:
        Serial.print(F("1-Enable, 2-Disable\n"));
        Serial.print(F(" Cleared On Read Control:"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          *CTRLB = *CTRLB | LTC2946_ENABLE_CLEARED_ON_READ;
        else
          *CTRLB = *CTRLB & LTC2946_DISABLE_CLEARED_ON_READ;
        break;
      case 4:
        Serial.print(F("1-Enable, 2-Disable\n"));
        Serial.print(F(" Stuck Bus Timeout Auto Wake Up:"));
        user_command = read_int();
        if ((user_command > 2) || (user_command < 0))
          user_command = 2;
        Serial.println(user_command);
        if (user_command == 1)
          *CTRLB = *CTRLB | LTC2946_ENABLE_STUCK_BUS_RECOVER;
        else
          *CTRLB = *CTRLB & LTC2946_DISABLE_STUCK_BUS_RECOVER;
        break;
      case 5:
        Serial.print(F("1-Follow ACC State\n"));
        Serial.print(F("2-No Accumulate\n"));
        Serial.print(F("3-Accumulate\n"));
        Serial.print(F("Enter Command:"));
        user_command = read_int();
        Serial.println(user_command);
        switch (user_command)
        {
          case 1:
            *CTRLB = *CTRLB & LTC2946_CTRLB_ACC_MASK;
            *CTRLB = *CTRLB | LTC2946_ACC_PIN_CONTROL;
            break;
          case 2:
            *CTRLB = *CTRLB & LTC2946_CTRLB_ACC_MASK;
            *CTRLB = *CTRLB | LTC2946_DISABLE_ACC;
            break;
          case 3:
            *CTRLB = *CTRLB & LTC2946_CTRLB_ACC_MASK;
            *CTRLB = *CTRLB | LTC2946_ENABLE_ACC;
            break;
          default:
            Serial.println("Incorrect Option");
        }
        break;
      case 6:
        Serial.print(F("1-Reset All Registers\n"));
        Serial.print(F("2-Reset Accumulator\n"));
        Serial.print(F("3-Enable Auto-Reset\n"));
        Serial.print(F("4-Disable Auto-Reset\n"));
        Serial.print(F("Enter Command:"));
        user_command = read_int();
        Serial.println(user_command);
        switch (user_command)
        {
          case 1:
            *CTRLB = *CTRLB & LTC2946_CTRLB_RESET_MASK;
            *CTRLB = *CTRLB | LTC2946_RESET_ALL;
            break;
          case 2:
            *CTRLB = *CTRLB & LTC2946_CTRLB_RESET_MASK;
            *CTRLB = *CTRLB | LTC2946_RESET_ACC;
            break;
          case 3:
            *CTRLB = *CTRLB & LTC2946_CTRLB_RESET_MASK;
            *CTRLB = *CTRLB | LTC2946_ENABLE_AUTO_RESET;
            break;
          case 4:
            *CTRLB = *CTRLB & LTC2946_CTRLB_RESET_MASK;
            *CTRLB = *CTRLB | LTC2946_DISABLE_AUTO_RESET;
            break;
          default:
            Serial.println("Incorrect Option");
        }
        break;
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }
  }
  while (user_command != 'm');
  ack |= LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_CTRLB_REG, *CTRLB);
  return(ack);
}

//! Configure GPIO pin States
int8_t menu_5_settings_menu_3_configure_GPIO(uint8_t *GPIO_CFG,         //!< Local copy of GPIO_CFG register.
    uint8_t *GPIO3_CTRL)       //!< Local copy of GPIO3_CTRL register.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  uint8_t user_command;


  do
  {
    //Print Sub-Menu
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-GPIO1 Configure\n"));
    Serial.print(F("2-GPIO2 Configure\n"));
    Serial.print(F("3-GPIO3 Configure\n"));
    Serial.print(F("m-Settings Menu\n\n"));
    Serial.print(F("Enter a command:"));

    user_command = read_int();
    if (user_command == 'm')
      Serial.println('m');
    else
      Serial.println(user_command);

    switch (user_command)
    {
      case 1:   //Configure GPIO1 state
        Serial.print(F("1-General Purpose Input, Active High\n"));
        Serial.print(F("2-General Purpose Input, Active Low\n"));
        Serial.print(F("3-General Purpose Output, Hi-Z\n"));
        Serial.print(F("4-General Purpose Output, Pulls Low\n"));
        user_command = read_int();
        if (user_command == 'm')
          Serial.println('m');
        else
          Serial.println(user_command);
        switch (user_command)
        {
          case 1:
            *GPIO_CFG = *GPIO_CFG & LTC2946_GPIOCFG_GPIO1_MASK;
            *GPIO_CFG = *GPIO_CFG | LTC2946_GPIO1_IN_ACTIVE_HIGH;
            break;
          case 2:
            *GPIO_CFG = *GPIO_CFG & LTC2946_GPIOCFG_GPIO1_MASK;
            *GPIO_CFG = *GPIO_CFG | LTC2946_GPIO1_IN_ACTIVE_LOW;
            break;
          case 3:
            *GPIO_CFG = *GPIO_CFG & LTC2946_GPIOCFG_GPIO1_MASK;
            *GPIO_CFG = *GPIO_CFG | LTC2946_GPIO1_OUT_HIGH_Z;
            break;
          case 4:
            *GPIO_CFG = *GPIO_CFG & LTC2946_GPIOCFG_GPIO1_MASK;
            *GPIO_CFG = *GPIO_CFG | LTC2946_GPIO1_OUT_LOW;
            break;
          default:
            Serial.println("Incorrect Option");
        }
        Serial.println(*GPIO_CFG);
        break;
      case 2:   //Configure GPIO2 state
        Serial.print(F("1-General Purpose Input, Active High\n"));
        Serial.print(F("2-General Purpose Input, Active Low\n"));
        Serial.print(F("3-General Purpose Output, Hi-Z\n"));
        Serial.print(F("4-General Purpose Output, Pulls Low\n"));
        Serial.print(F("5-Accumulate Input\n"));
        user_command = read_int();
        if (user_command == 'm')
          Serial.println('m');
        else
          Serial.println(user_command);
        switch (user_command)
        {
          case 1:
            *GPIO_CFG = *GPIO_CFG & LTC2946_GPIOCFG_GPIO2_MASK;
            *GPIO_CFG = *GPIO_CFG | LTC2946_GPIO2_IN_ACTIVE_HIGH;
            break;
          case 2:
            *GPIO_CFG = *GPIO_CFG & LTC2946_GPIOCFG_GPIO2_MASK;
            *GPIO_CFG = *GPIO_CFG | LTC2946_GPIO2_IN_ACTIVE_LOW;
            break;
          case 3:
            *GPIO_CFG = *GPIO_CFG & LTC2946_GPIOCFG_GPIO2_MASK & LTC2946_GPIOCFG_GPIO2_OUT_MASK;
            *GPIO_CFG = *GPIO_CFG | LTC2946_GPIO2_OUT_HIGH_Z;
            break;
          case 4:
            *GPIO_CFG = *GPIO_CFG & LTC2946_GPIOCFG_GPIO2_MASK & LTC2946_GPIOCFG_GPIO2_OUT_MASK;
            *GPIO_CFG = *GPIO_CFG | LTC2946_GPIO2_OUT_LOW;
            break;
          case 5:
            *GPIO_CFG = *GPIO_CFG & LTC2946_GPIOCFG_GPIO2_MASK;
            *GPIO_CFG = *GPIO_CFG | LTC2946_GPIO2_IN_ACC;
            break;
          default:
            Serial.println("Incorrect Option");
        }
        Serial.println(*GPIO_CFG);
        break;
      case 3:   //Confugure GPIO3 state
        Serial.print(F("1-General Purpose Input, Active High\n"));
        Serial.print(F("2-General Purpose Input, Active Low\n"));
        Serial.print(F("3-General Purpose Output, Hi-Z\n"));
        Serial.print(F("4-General Purpose Output, Pulls Low\n"));
        Serial.print(F("5-ALERT/ Output\n"));
        user_command = read_int();
        if (user_command == 'm')
          Serial.println('m');
        else
          Serial.println(user_command);
        switch (user_command)
        {
          case 1:
            *GPIO_CFG = *GPIO_CFG & LTC2946_GPIOCFG_GPIO3_MASK;
            *GPIO_CFG = *GPIO_CFG | LTC2946_GPIO3_IN_ACTIVE_HIGH;
            break;
          case 2:
            *GPIO_CFG = *GPIO_CFG & LTC2946_GPIOCFG_GPIO3_MASK;
            *GPIO_CFG = *GPIO_CFG | LTC2946_GPIO3_IN_ACTIVE_LOW;
            break;
          case 3:
            *GPIO_CFG = *GPIO_CFG & LTC2946_GPIOCFG_GPIO3_MASK;
            *GPIO_CFG = *GPIO_CFG | LTC2946_GPIO3_OUT_REG_42;
            *GPIO3_CTRL = *GPIO3_CTRL & LTC2946_GPIO3_CTRL_GPIO3_MASK;
            *GPIO3_CTRL = *GPIO3_CTRL | LTC2946_GPIO3_OUT_HIGH_Z;
            break;
          case 4:
            *GPIO_CFG = *GPIO_CFG & LTC2946_GPIOCFG_GPIO3_MASK;
            *GPIO_CFG = *GPIO_CFG | LTC2946_GPIO3_OUT_REG_42;
            *GPIO3_CTRL = *GPIO3_CTRL & LTC2946_GPIO3_CTRL_GPIO3_MASK;
            *GPIO3_CTRL = *GPIO3_CTRL | LTC2946_GPIO3_OUT_LOW;
            break;
          case 5:
            *GPIO_CFG = *GPIO_CFG & LTC2946_GPIOCFG_GPIO3_MASK;
            *GPIO_CFG = *GPIO_CFG | LTC2946_GPIO3_OUT_ALERT;
            break;
          default:
            Serial.println("Incorrect Option");
        }
        Serial.println(*GPIO_CFG);
        Serial.println(*GPIO3_CTRL);
        break;
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }
    ack |= LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_GPIO_CFG_REG, *GPIO_CFG);
    ack |= LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_GPIO3_CTRL_REG, *GPIO3_CTRL);
  }
  while (user_command != 'm');
  return(ack);
}



//! Select duty cycle of measurments and channel configuration.
void menu_5_settings_menu_4_scaling_for_ADIN(uint8_t *scale)      //!< Stores division ratio for resistive divider on ADIN pin.
{

  float user_value;

  Serial.print(F("Enter desired maximum voltage for ADIN pin measurement\n"));
  Serial.print(F("Note: Ensure that you have the proper resistor divider ratio on the ADIN pin for this voltage level\n"));

  user_value = read_float();
  *scale = user_value/2.048;
  Serial.print(user_value);
  Serial.print(F("Scale = "));
  Serial.print(*scale);
}

//! Clear Min/Max
int8_t menu_6_clear_min_max()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;

  ack |= LTC2946_write_24_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_POWER_MSB2_REG, LTC2946_MAX_POWER_MSB2_RESET);
  ack |= LTC2946_write_24_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_POWER_MSB2_REG, LTC2946_MIN_POWER_MSB2_RESET);
  ack |= LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_DELTA_SENSE_MSB_REG, LTC2946_MAX_DELTA_SENSE_MSB_RESET);
  ack |= LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_DELTA_SENSE_MSB_REG, LTC2946_MIN_DELTA_SENSE_MSB_RESET);
  ack |= LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_VIN_MSB_REG, LTC2946_MAX_VIN_MSB_RESET);
  ack |= LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_VIN_MSB_REG, LTC2946_MIN_VIN_MSB_RESET);
  ack |= LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_ADIN_MSB_REG, LTC2946_MAX_ADIN_MSB_RESET);
  ack |= LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_ADIN_MSB_REG, LTC2946_MIN_ADIN_MSB_RESET);
  Serial.println();
  Serial.print(F("Min/Max Cleared.\n\n"));
  return(ack);
}

//! Used to manipulate EEPROM data.
union eeprom_data_union
{
  struct data_struct_type               //! EEPROM data structure
  {
    int16_t cal_key;                    //!< The key that keeps track of the calibration
    uint8_t alert1;                     //!< Used to store the ALERT1 settings
    uint8_t alert2;                     //!< Used to store the ALERT2 settings
    uint32_t max_power_threshold;        //!< Used to store max power alert settings
    uint32_t min_power_threshold;        //!< Used to store min power alert settings
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
    alert1_code = eeprom.data_struct.alert1;  // Global variable used to communicate with loop()
    alert2_code = eeprom.data_struct.alert2; // Global variable used to communicate with loop()
    LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_ALERT1_REG, eeprom.data_struct.alert1);
    LTC2946_write(LTC2946_I2C_ADDRESS, LTC2946_ALERT1_REG, eeprom.data_struct.alert2);
    LTC2946_write_24_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_POWER_THRESHOLD_MSB2_REG, eeprom.data_struct.max_power_threshold);
    LTC2946_write_24_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_POWER_THRESHOLD_MSB2_REG, eeprom.data_struct.min_power_threshold);
    LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_DELTA_SENSE_THRESHOLD_MSB_REG, eeprom.data_struct.max_delta_sense_thresh);
    LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_DELTA_SENSE_THRESHOLD_MSB_REG, eeprom.data_struct.min_delta_sense_thresh);
    LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_VIN_THRESHOLD_MSB_REG, eeprom.data_struct.max_Vin_thresh);
    LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_VIN_THRESHOLD_MSB_REG, eeprom.data_struct.min_Vin_thresh);
    LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_ADIN_THRESHOLD_MSB_REG, eeprom.data_struct.max_adin_thresh);
    LTC2946_write_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_ADIN_THRESHOLD_MSB_REG, eeprom.data_struct.min_adin_thresh);

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

  LTC2946_read(LTC2946_I2C_ADDRESS, LTC2946_ALERT1_REG, &eeprom.data_struct.alert1);
  LTC2946_read(LTC2946_I2C_ADDRESS, LTC2946_ALERT2_REG, &eeprom.data_struct.alert2);
  LTC2946_read_24_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_POWER_THRESHOLD_MSB2_REG, &eeprom.data_struct.max_power_threshold);
  LTC2946_read_24_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_POWER_THRESHOLD_MSB2_REG, &eeprom.data_struct.min_power_threshold);
  LTC2946_read_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_DELTA_SENSE_THRESHOLD_MSB_REG, &eeprom.data_struct.max_delta_sense_thresh);
  LTC2946_read_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_DELTA_SENSE_THRESHOLD_MSB_REG, &eeprom.data_struct.min_delta_sense_thresh);
  LTC2946_read_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_VIN_THRESHOLD_MSB_REG, &eeprom.data_struct.max_Vin_thresh);
  LTC2946_read_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_VIN_THRESHOLD_MSB_REG, &eeprom.data_struct.min_Vin_thresh);
  LTC2946_read_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MAX_ADIN_THRESHOLD_MSB_REG, &eeprom.data_struct.max_adin_thresh);
  LTC2946_read_16_bits(LTC2946_I2C_ADDRESS, LTC2946_MIN_ADIN_THRESHOLD_MSB_REG, &eeprom.data_struct.min_adin_thresh);

  eeprom_write_byte_array(EEPROM_I2C_ADDRESS, eeprom.byte_array, EEPROM_CAL_STATUS_ADDRESS, sizeof(eeprom_data_union));

  Serial.println("Alert Settings Stored to EEPROM");
}

