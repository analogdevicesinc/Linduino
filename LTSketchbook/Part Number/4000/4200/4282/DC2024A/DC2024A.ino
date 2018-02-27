/*!
Linear Technology DC2024A Demonstration Board.
LTC4282: High Current Hotswap Controller with I2C Compatible Monitoring

@verbatim

Setting the Alarm Thresholds:
    1. Select the Alarm option from the main menu.
    2. Then enter the minimum and maximum
       values.
    3. Once you are done, the new settings will be uploaded to the Device.
    4. Lastly, go back to the main menu and start reading values in Continuous Mode
       Note: Alerts only respond when conversion is done. Therefore, in continuous
       mode the alerts will constantly be updated.

Reading and Clearing Alarm Faults:
    1. Select the View/Clear option from the main menu.
    2. Select the Clear Faults option. This will clear all faults.

NOTES
 Setup:
 Set the terminal baud rate to 115200 and select the newline terminator.
 Requires a power supply.
 Refer to demo manual DC2024.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC4282

http://www.linear.com/product/LTC4282#demoboards


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
    @ingroup LTC4282
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC4282.h"
#include <Wire.h>
#include <SPI.h>

// Function Declaration
void print_title();                 // Print the title block
void print_prompt();                // Print the Prompt
int8_t menu_1_continuous_mode(uint8_t CTRLA, uint8_t ILIM_ADJUST, uint8_t CLK_DECIMATOR, float resister);   //! Continuous Mode Measurement
int8_t menu_2_alarm_thresholds(uint8_t CTRLREG_LSB);   //! Set Alarm Thresholds
int8_t menu_3_view_clear_faults();    //! View/Clear Faults
int8_t menu_3_view_faults_menu_1();    //! View Faults
int8_t menu_3_clear_faults_menu_2();   //! Clear Faults
int8_t menu_4_set_alert_registers();    //! Set Alert Registers
int8_t menu_4_set_alert_msb_register_menu_1();  //! Set Alert MSB Register
int8_t menu_4_set_alert_lsb_register_menu_2();   //! Set Alert LSB Register
int8_t menu_5_settings(uint8_t *CTRLREG_MSB, uint8_t *CTRLREG_LSB, uint8_t *ILIM_ADJUST, uint8_t *CLK_DECIMATOR);     //! Settings
int8_t menu_5_set_control_msb_register_menu_1(uint8_t *CTRLREG_MSB);  //! Control MSB Register Menu. Configure OV/UV/OC Autoretry, FET ON, Mass Write Enable, ON/ENB, ON DELAY and ON FAULT MASK.
int8_t menu_5_set_control_lsb_register_menu_2(uint8_t *CTRLREG_LSB);  //! Control LSB Register Menu. Configure Thresholds for POWER GOOD/OV/UV as well as for VIN MODE.
uint8_t menu_5_set_ilim_adjust_register_menu_3(uint8_t *ILIM_ADJUST); // ILIM ADJUST Register Menu. Configure Current Limit/Foldback Mode/ADC Voltage Monitoring/16 or 12-bit Mode
uint8_t menu_5_configure_GPIO_pins_menu_4();  //! Configure State of GPIO Pins. Set GPIO1 as POWERGOOD/POWERBAD/GPO/GPI, OverFlow Alert, GPIO2 Stress Mode etc.
uint8_t menu_5_write_register_values_to_EEPROM_menu_5();   //! Write Values currently present in the Registers of the LTC4282 to the EEPROM
uint8_t menu_5_restore_register_values_from_EEPROM_menu_6(); //! Restore Values Currently Present in the EEPROM to the Registers of the LTC4282



#define CONTINUOUS_MODE_DISPLAY_DELAY     2000        //!< The delay between readings

const float resistor = 343.7E-6;         //!< resistor value on demo board
const int CLK_DIVIDER= 0x08;             //!< CLK Frequency
float voltage_fullscale;                 //!< Fullscale Voltage Setting Shared Across Functions For Polling Routines

//static float LTC4282_TIME_lsb = 16.39543E-3;                       //!< Static variable which is based off of the default clk frequency of 250KHz.

// Error string
const char ack_error[] = "Error: No Acknowledge. Check I2C Address."; //!< Error message

// Global variables
static int8_t demo_board_connected;        //!< Set to 1 if the board is connected

// Function Definitions
//! Print the title block
void print_title()
{
  Serial.println(F("\n*****************************************************************"));
  Serial.print(F("* DC2024 Demonstration Program                                  *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program communicates with the LTC4282 High Current       *\n"));
  Serial.print(F("* Hot-Swap Controller with I2C Monitoring Capability found on   *\n"));
  Serial.print(F("* the DC2024 Demoboard.                                         *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Print the Prompt
void print_prompt()
{
  Serial.print(F("\n1-Continuous Mode\n"));
  Serial.print(F("2-Set Alarm Thresholds\n"));
  Serial.print(F("3-View/Clear Faults\n"));
  Serial.print(F("4-Set Alert Registers\n"));
  Serial.print(F("5-Device Settings\n"));
  Serial.print(F("Enter a command: "));
}



//! Initialize Linduino
void setup()
{
  char demo_name[] = "DC2024A";      // Demo Board Name stored in QuikEval EEPROM

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
    print_prompt();
  }
}


void loop()
{
  int8_t ack = 0;                               // I2C acknowledge indicator

  static uint8_t user_command;                  //!< Initialize User Input Vairiable.


//! Initialize Default Values in Control and Configuration Registers. See Datasheet for more information.
  static uint8_t CTRLREG_MSB = LTC4282_ON_FAULT_MASK|LTC4282_ON_ENB|LTC4282_MASS_WRITE|LTC4282_FET_ON|LTC4282_UV_AUTORETRY|LTC4282_OV_AUTORETRY;
  static uint8_t CTRLREG_LSB = LTC4282_VIN_MODE_12_V;
  static uint8_t ILIM_ADJUST = (LTC4282_ILIM_ADJUST_25_V_0|LTC4282_FOLDBACK_MODE_12_V_0|LTC4282_ADC_VSOURCE|LTC4282_ADC_GPIO2_MODE)& ~LTC4282_ADC_16_BIT;
  static uint8_t CLK_DECIMATOR = (CLK_DIVIDER)&~LTC4282_COULOMB_METER&~LTC4282_TICK_OUT&~LTC4282_INT_CLK_OUT;
  static uint8_t GPIO_CONFIG = LTC4282_GPIO1_CONFIG_INPUT;

  if (demo_board_connected)                                   //! Do nothing if the demo board is not connected
  {
    ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_CONTROL_MSB_REG, CTRLREG_MSB);
    ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_CONTROL_LSB_REG, CTRLREG_LSB);
    ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_ILIM_ADJUST_REG, ILIM_ADJUST);
    ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_CLK_DEC_REG, CLK_DECIMATOR);
    ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_GPIO_CONFIG_REG, GPIO_CONFIG);

    if (Serial.available())                                   //! Do nothing if serial is not available
    {
      user_command = read_int();                              //! Read user input command
      if (user_command != 'm')
        Serial.println(user_command);
      Serial.println();
      ack = 0;
      switch (user_command)
      {
        case 1:
          ack |= menu_1_continuous_mode(CTRLREG_LSB, ILIM_ADJUST, CLK_DECIMATOR, resistor);  //! Continuous Mode Measurement
          break ;
        case 2:
          ack |= menu_2_alarm_thresholds(CTRLREG_LSB);         //! Set Alarm Thresholds
          break;
        case 3:
          ack |= menu_3_view_clear_faults();                //! View/Clear Faults
          break;
        case 4:
          ack |= menu_4_set_alert_registers();              //! Set Alert Registers
          break;
        case 5:
          menu_5_settings(&CTRLREG_MSB, &CTRLREG_MSB, &ILIM_ADJUST, &CLK_DECIMATOR);  //! Settings
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

//! Continious Mode
int8_t menu_1_continuous_mode(uint8_t CTRLREG_LSB,    //!< Control LSB Register Code. Necessary to Check which Voltage Mode the Part is set to. Configured in Settings Menu.
                              uint8_t ILIM_ADJUST,    //!< ILIM_ADJUST Register Code. Necessary to Check Whether the Part is Configured as a Coulomb Meter or an Energy Meter. Also Checks Resolution Mode. Configured in Settings Menu.
                              uint8_t CLK_DECIMATOR,  //!< CLK_DECIMATOR Register Code. Sets the Clock Rate for the Energy/Coulomb Meter. Configured in the Settings Menu.
                              float resistor)
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
{

  int8_t ack = 0;

  uint16_t voltage_code;
  uint16_t current_code;
  uint16_t power_code;
  uint32_t time_code;
  uint64_t meter_code;

  float tConv; //!< Conversion Time Period

  //! Check to see if the part is in 16-bit or 12-bit mode. Then Set the Time Period Accordingly.
  if (ILIM_ADJUST & LTC4282_ADC_16_BIT)
  {
    tConv = 1.0486;
  }
  else
  {
    tConv = 65.535E-3;
  }

  //! Check to See if VIN Mode is Set to 3.3V, 5V, 12V and/or 12V. Then Set the Appropriate Fullscale Voltage
  if ((CTRLREG_LSB & LTC4282_VIN_MODE_24_V) == LTC4282_VIN_MODE_24_V)
  {

    voltage_fullscale = LTC4282_VSOURCE_24V_FS;

  }
  else if ((CTRLREG_LSB & LTC4282_VIN_MODE_12_V) == LTC4282_VIN_MODE_12_V)
  {

    voltage_fullscale = LTC4282_VSOURCE_12V_FS;

  }
  else if ((CTRLREG_LSB & LTC4282_VIN_MODE_5_V) == LTC4282_VIN_MODE_5_V)
  {

    voltage_fullscale = LTC4282_VSOURCE_5V_FS;

  }
  else
  {

    voltage_fullscale = LTC4282_VSOURCE_3V3_FS;

  }

  //! Keep Polling Till the User Presses 'm'
  do
  {

    uint8_t adc_alert_log;
    ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_ADC_ALERT_LOG_REG, &adc_alert_log);  //!<Read ADC Alert Log To Keep Track of Alerts

    float vsource, vsource_min, vsource_max;

    ack |= LTC4282_read_16_bits(LTC4282_I2C_ADDRESS, LTC4282_VSOURCE_MSB_REG, &voltage_code);         //!< Read Voltage Code From VSOURCE Register
    vsource = LTC4282_code_to_voltage(voltage_code, voltage_fullscale);                               //!< Convert Voltage Code to Current Source Voltage
    ack |= LTC4282_read_16_bits(LTC4282_I2C_ADDRESS, LTC4282_VSOURCE_MAX_MSB_REG, &voltage_code);     //!< Read Voltage Code From VSOURCE MAX Register
    vsource_max = LTC4282_code_to_voltage(voltage_code, voltage_fullscale);                           //!< Convert Voltage Code to Maximum Source Voltage
    ack |= LTC4282_read_16_bits(LTC4282_I2C_ADDRESS, LTC4282_VSOURCE_MIN_MSB_REG, &voltage_code);     //!< Read Voltage Code From VSOURCE MIN Register
    vsource_min = LTC4282_code_to_voltage(voltage_code, voltage_fullscale);                           //!< Convert Voltage Code to Minimum Source Voltage


    float vgpio, vgpio_min, vgpio_max;

    ack |= LTC4282_read_16_bits(LTC4282_I2C_ADDRESS, LTC4282_VGPIO_MSB_REG, &voltage_code);           //!< Read Voltage Code From VGPIO Register
    vgpio = LTC4282_code_to_VGPIO(voltage_code);                                                      //!< Convert Voltage Code to VGPIO Voltage
    ack |= LTC4282_read_16_bits(LTC4282_I2C_ADDRESS, LTC4282_VGPIO_MAX_MSB_REG, &voltage_code);       //!< Read Voltage Code From VGPIO MAX Register
    vgpio_max = LTC4282_code_to_VGPIO(voltage_code);                                                  //!< Convert Voltage Code to Maximum VGPIO Voltage
    ack |= LTC4282_read_16_bits(LTC4282_I2C_ADDRESS, LTC4282_VGPIO_MIN_MSB_REG, &voltage_code);       //!< Read Voltage Code From VGPIO MIN Register
    vgpio_min = LTC4282_code_to_VGPIO(voltage_code);                                                  //!< Convert Voltage Code to Minimum VGPIO Voltage


    float current, current_min, current_max;

    ack |= LTC4282_read_16_bits(LTC4282_I2C_ADDRESS, LTC4282_VSENSE_MSB_REG, &current_code);           //!< Read Current Code From VSENSE Register
    current = LTC4282_code_to_current(current_code, resistor);                                         //!< Convert Current Code to Current
    ack |= LTC4282_read_16_bits(LTC4282_I2C_ADDRESS, LTC4282_VSENSE_MAX_MSB_REG, &current_code);       //!< Read Current Code From VSENSE MAX Register
    current_max = LTC4282_code_to_current(current_code, resistor);                                     //!< Convert Current Code to Maximum Current
    ack |= LTC4282_read_16_bits(LTC4282_I2C_ADDRESS, LTC4282_VSENSE_MIN_MSB_REG, &current_code);       //!< Read Current Code From VSENSE MIN Register
    current_min = LTC4282_code_to_current(current_code, resistor);                                     //!< Convert Current Code to Minimum Current


    float power, power_min, power_max;

    ack |= LTC4282_read_16_bits(LTC4282_I2C_ADDRESS, LTC4282_POWER_MSB_REG, &power_code);               //!< Read Power Code From Power Register
    power = LTC4282_code_to_power(power_code,voltage_fullscale, resistor);                             //!< Convert Power Code to Power
    ack |= LTC4282_read_16_bits(LTC4282_I2C_ADDRESS, LTC4282_POWER_MAX_MSB_REG, &power_code);          //!< Read Power Code From POWER MAX Register
    power_max = LTC4282_code_to_power(current_code, voltage_fullscale, resistor);                      //!< Convert Power Code to Maximum Power
    ack |= LTC4282_read_16_bits(LTC4282_I2C_ADDRESS, LTC4282_POWER_MIN_MSB_REG, &power_code);          //!< Read Power Code From POWER MIN Register
    power_min = LTC4282_code_to_power(current_code, voltage_fullscale, resistor);                      //!< Convert Power Code to Minimum Power


    ack|= LTC4282_read_32_bits(LTC4282_I2C_ADDRESS, LTC4282_TICK_COUNTER_MSB3_REG, &time_code);        //!< Read Minimum Time Code

    //! Display Min, Current And Max Voltage
    Serial.println(F("***********Voltage************"));
    Serial.print(F("Min Voltage: "));
    Serial.print(vsource_min);
    Serial.print(F("V\n"));
    Serial.print(F("Reading:     "));
    Serial.print(vsource);
    Serial.print(F("V\n"));
    Serial.print(F("Max Voltage: "));
    Serial.print(vsource_max);
    Serial.println(F("V"));
    Serial.print(F("\n"));

    //! If the Minimum or Maximum Threshold has been Exceeded Then Alert the User
    if (adc_alert_log & LTC4282_VSOURCE_ALARM_HIGH)
    {
      Serial.print(F("ALERT - VOLTAGE MAXIMUM THRESHOLD EXCEEDED\n"));
    }
    if (adc_alert_log & LTC4282_VSOURCE_ALARM_LOW)
    {
      Serial.print(F("ALERT - VOLTAGE MINIMUM THRESHOLD EXCEEDED\n"));
    }

    Serial.print(F("\n"));

    //! Display Min, Current And Max Current
    Serial.println(F("***********Current************"));
    Serial.print(F("Min Current: "));
    Serial.print(current_min);
    Serial.println(F("A"));
    Serial.print(F("Reading :    "));
    Serial.print(current);
    Serial.println(F("A"));
    Serial.print(F("Max Current: "));
    Serial.print(current_max);
    Serial.println(F("A"));

    //! If the Minimum or Maximum Threshold has been Exceeded Then Alert the User
    if (adc_alert_log & LTC4282_VSENSE_ALARM_HIGH)
    {
      Serial.print(F("ALERT - CURRENT MAXIMUM THRESHOLD EXCEEDED\n"));
    }
    if (adc_alert_log & LTC4282_VSENSE_ALARM_LOW)
    {
      Serial.print(F("ALERT - CURRENT MINIMUM THRESHOLD EXCEEDED\n"));
    }

    Serial.print(F("\n"));

    //! Display Min, Current And Max VGPIO
    Serial.println(F("***********VGPIO************"));
    Serial.print(F("Min VGPIO: "));
    Serial.print(vgpio_min);
    Serial.println(F("V"));
    Serial.print(F("Reading :  "));
    Serial.print(vgpio);
    Serial.println(F("V"));
    Serial.print(F("Max VGPIO: "));
    Serial.print(vgpio_max);
    Serial.println(F("V"));

    //! If the Minimum or Maximum Threshold has been Exceeded Then Alert the User
    if (adc_alert_log & LTC4282_VGPIO_ALARM_HIGH)
    {
      Serial.print(F("ALERT - VGPIO MAXIMUM THRESHOLD EXCEEDED\n"));
    }
    if (adc_alert_log & LTC4282_VGPIO_ALARM_LOW)
    {
      Serial.print(F("ALERT - VGPIO MINIMUM THRESHOLD EXCEEDED\n"));
    }

    Serial.print(F("\n"));

    //! Display Min, Current And Max VGPIO
    Serial.println(F("***********Power************"));
    Serial.print(F("Min Power: "));
    Serial.print(power_min);
    Serial.println(F("W"));
    Serial.print(F("Reading :  "));
    Serial.print(power);
    Serial.println(F("W"));
    Serial.print(F("Max Power: "));
    Serial.print(power_max);
    Serial.println(F("W"));

    //! If the Minimum or Maximum Threshold has been Exceeded Then Alert the User
    if (adc_alert_log & LTC4282_POWER_ALARM_HIGH)
    {
      Serial.print(F("ALERT - POWER MAXIMUM THRESHOLD EXCEEDED\n"));
    }
    if (adc_alert_log & LTC4282_POWER_ALARM_LOW)
    {
      Serial.print(F("ALERT - POWER MINIMUM THRESHOLD EXCEEDED\n"));
    }

    Serial.print(F("\n"));

    //! If Device Is Set To Accumulate Coulombs, Then Calculate Coulombs and Average Current
    if (CLK_DECIMATOR & LTC4282_COULOMB_METER)
    {

      float coulombs;
      ack |= LTC4282_read_48_bits(LTC4282_I2C_ADDRESS, LTC4282_METER_MSB5_REG, &meter_code);
      coulombs = LTC4282_code_to_coulombs(meter_code, resistor, tConv);

      float avg_current;
      avg_current = LTC4282_code_to_avg_current(time_code, coulombs, tConv);
      Serial.println(F("***********Coulombs************"));
      Serial.print(F("Meter: "));
      Serial.print(coulombs);
      Serial.println(F("C"));
      Serial.print(F("Avg Current :  "));
      Serial.print(avg_current);
      Serial.println(F("A"));

    }
    //! Else Calculate Energy and Average Power
    else
    {
      float energy;
      ack |= LTC4282_read_48_bits(LTC4282_I2C_ADDRESS, LTC4282_METER_MSB5_REG, &meter_code);
      energy = LTC4282_code_to_energy(meter_code, voltage_fullscale, resistor, tConv);

      float avg_power;
      avg_power = LTC4282_code_to_avg_power(time_code, energy, tConv);

      Serial.println(F("***********Energy************"));
      Serial.print(F("Meter: "));
      Serial.print(energy);
      Serial.println(F("J"));
      Serial.print(F("Avg Power :  "));
      Serial.print(avg_power);
      Serial.println(F("W"));

    }

    Serial.println(F("\n"));

    //! Display Time
    Serial.println(F("***********Time************"));
    Serial.print(F("Internal Time: "));
    Serial.print(time_code*tConv);
    Serial.println(F("S"));

    Serial.println(F("\n"));

    Serial.flush();
    delay(CONTINUOUS_MODE_DISPLAY_DELAY);

  }
  while (Serial.available() == false);
  read_int();
  return(ack);

}

//! Set Alarm Thresholds
int8_t menu_2_alarm_thresholds(uint8_t CTRLREG_LSB                         //!< Control LSB Value Passed On to Determine VIN MODE For Proper Voltage Conversion Of Thresholds.                     )
                              )
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
{
  //! Initializing Variables
  uint8_t ack = 0;
  uint8_t alarm_reg_code = 0;
  float alarmValue, voltage_fullscale;

  //! Determine LTC4282 VIN MODE And Set Appropriate FullScale Voltage
  if ((CTRLREG_LSB & LTC4282_VIN_MODE_24_V) == LTC4282_VIN_MODE_24_V)
  {
    voltage_fullscale = LTC4282_VSOURCE_24V_FS;
  }
  else if ((CTRLREG_LSB & LTC4282_VIN_MODE_12_V) == LTC4282_VIN_MODE_12_V)
  {
    voltage_fullscale = LTC4282_VSOURCE_12V_FS;
  }
  else if ((CTRLREG_LSB & LTC4282_VIN_MODE_5_V) == LTC4282_VIN_MODE_5_V)
  {
    voltage_fullscale = LTC4282_VSOURCE_5V_FS;
  }
  else
  {
    voltage_fullscale = LTC4282_VSOURCE_3V3_FS;
  }


  //! Display Thresholds Currently Present Within The LTC4282 Threshold Registers
  Serial.println(F("****CURRENT THRESHOLDS****"));
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_VSOURCE_ALARM_MAX_REG, &alarm_reg_code);
  alarmValue = LTC4282_code_to_volt_alarm(alarm_reg_code, voltage_fullscale);
  Serial.print(F("Voltage Maximum Alarm = "));
  Serial.print(alarmValue);
  Serial.println(F("V"));

  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_VSOURCE_ALARM_MIN_REG, &alarm_reg_code);
  alarmValue = LTC4282_code_to_volt_alarm(alarm_reg_code, voltage_fullscale);
  Serial.print(F("Voltage Minimum Alarm = "));
  Serial.print(alarmValue);
  Serial.println(F("V"));

  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_VSENSE_ALARM_MAX_REG, &alarm_reg_code);
  alarmValue = LTC4282_code_to_current_alarm(alarm_reg_code, resistor);
  Serial.print(F("Current Maximum Alarm = "));
  Serial.print(alarmValue);
  Serial.println(F("A"));

  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_VSENSE_ALARM_MIN_REG, &alarm_reg_code);
  alarmValue = LTC4282_code_to_current_alarm(alarm_reg_code, resistor);
  Serial.print(F("Current Minimum Alarm = "));
  Serial.print(alarmValue);
  Serial.println(F("A"));

  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_VGPIO_ALARM_MAX_REG, &alarm_reg_code);
  alarmValue = LTC4282_code_to_GPIO_alarm(alarm_reg_code);
  Serial.print(F("VGPIO Maximum Alarm = "));
  Serial.print(alarmValue);
  Serial.println(F("V"));

  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_VGPIO_ALARM_MIN_REG, &alarm_reg_code);
  alarmValue = LTC4282_code_to_GPIO_alarm(alarm_reg_code);
  Serial.print(F("VGPIO Minimum Alarm = "));
  Serial.print(alarmValue);
  Serial.println(F("V\n"));

  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_POWER_ALARM_MAX_REG, &alarm_reg_code);
  alarmValue = LTC4282_code_to_power_alarm(alarm_reg_code, voltage_fullscale, resistor);
  Serial.print(F("Power Maximum Alarm = "));
  Serial.print(alarmValue);
  Serial.println(F("W\n"));

  Serial.println(alarm_reg_code);

  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_POWER_ALARM_MIN_REG, &alarm_reg_code);
  alarmValue = LTC4282_code_to_power_alarm(alarm_reg_code, voltage_fullscale, resistor);
  Serial.print(F("Power Minimum Alarm = "));
  Serial.print(alarmValue);
  Serial.println(F("W\n"));

  //! Enter New Thresholds to Store Within the LTC4282 Alarm Thresholds.
  Serial.println(F("****ENTER THRESHOLD VALUES****"));
  Serial.print(F("Enter Voltage Maximum Alarm = "));
  float voltage_maximum = read_float();
  Serial.println(voltage_maximum);
  Serial.print(F("Enter Voltage Minimum Alarm = "));
  float voltage_minimum = read_float();
  Serial.println(voltage_minimum);
  Serial.print(F("Enter Current Maximum Alarm = "));
  float current_maximum = read_float();
  Serial.println(current_maximum);
  Serial.print(F("Enter Current Minimum Alarm = "));
  float current_minimum = read_float();
  Serial.println(current_minimum);
  Serial.print(F("Enter VGPIO Maximum Alarm = "));
  float vgpio_maximum = read_float();
  Serial.println(vgpio_maximum);
  Serial.print(F("Enter VGPIO Minimum Alarm = "));
  float vgpio_minimum = read_float();
  Serial.println(vgpio_minimum);
  Serial.print(F("Enter Power Maximum Alarm = "));
  float power_maximum = read_float();
  Serial.println(power_maximum);
  Serial.print(F("Enter Power Minimum Alarm = "));
  float power_minimum = read_float();
  Serial.println(power_minimum);

  uint8_t adc_code = 0;
  //! Convert Values Entered By User Into ADC Codes And Then Write To Appropriate Registers
  adc_code = LTC4282_volt_to_code_alarm(voltage_maximum, voltage_fullscale);
  ack |=  LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_VSOURCE_ALARM_MAX_REG, adc_code);
  adc_code = LTC4282_volt_to_code_alarm(voltage_minimum, voltage_fullscale);
  ack |=  LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_VSOURCE_ALARM_MIN_REG, adc_code);

  adc_code = LTC4282_current_to_code_alarm(current_maximum, resistor);
  ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_VSENSE_ALARM_MAX_REG, adc_code);
  adc_code = LTC4282_current_to_code_alarm(current_minimum, resistor);
  ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_VSENSE_ALARM_MIN_REG, adc_code);

  adc_code = LTC4282_VGPIO_to_code_alarm(vgpio_maximum);
  ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_VGPIO_ALARM_MAX_REG, adc_code);
  adc_code = LTC4282_VGPIO_to_code_alarm(vgpio_minimum);
  ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_VGPIO_ALARM_MIN_REG, adc_code);

  adc_code = LTC4282_power_to_code_alarm(power_maximum, resistor, voltage_fullscale);
  //Serial.println(adc_code);
  ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_POWER_ALARM_MAX_REG, adc_code);
  adc_code = LTC4282_power_to_code_alarm(power_minimum, resistor, voltage_fullscale);
  ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_POWER_ALARM_MIN_REG, adc_code);

  adc_code = 00;
  ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_ADC_ALERT_LOG_REG, adc_code);

  return ack;

}

//! View Faults And Clear Them
int8_t menu_3_view_clear_faults()
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
{

  int8_t ack = 0;

  uint8_t user_command;

  do
  {
    Serial.print(F("****Fault Menu****\n\n"));
    Serial.print(F("1 - View Faults\n"));
    Serial.print(F("2 - Clear All Faults\n"));
    Serial.print(F("m-Main Menu\n"));

    Serial.print(F("\n\nEnter a Command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
    {
      Serial.println(user_command);
    }

    Serial.println();
    switch (user_command)
    {
      case 1:  // View Faults
        ack |= menu_3_view_faults_menu_1();
        break;

      case 2:  // Clear Faults
        ack |= menu_3_clear_faults_menu_2();
        break;
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }

  }
  while (!((user_command == 'm') || (ack)));

  return ack;
}

//! View Faults
int8_t menu_3_view_faults_menu_1()
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
{

  int8_t ack = 0;

  uint8_t fault_code = 0;
  uint8_t adc_alert_code = 0;

  //! Read Both Fault Log Register and ADC Alert Log Register
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_FAULT_LOG_REG, &fault_code);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_ADC_ALERT_LOG_REG, &adc_alert_code);

//! Display FAULT LOG Register
  Serial.print(F("****FAULT LOG****\n"));

  if (fault_code == 0)
  {
    Serial.println(F("NO FAULTS LOGGED"));
  }
  else
  {
    if ((fault_code & LTC4282_EEPROM_DONE_ALERT) != 0)
    {
      Serial.println(F("EEPROM DONE = TRUE"));
    }
    if ((fault_code & LTC4282_FET_BAD_FAULT_ALERT) != 0)
    {
      Serial.println(F("FET BAD FAULT = TRUE"));
    }
    if ((fault_code & LTC4282_FET_SHORT_ALERT) != 0)
    {
      Serial.println(F("FET SHORT FAULT = TRUE"));
    }
    if ((fault_code & LTC4282_ON_ALERT) != 0)
    {
      Serial.println(F("ON PIN FAULT = True"));
    }
    if ((fault_code & LTC4282_PB_ALERT) != 0)
    {
      Serial.println(F("POWER BAD FAULT = TRUE"));
    }
    if ((fault_code & LTC4282_OC_ALERT) != 0)
    {
      Serial.println(F("OVER CURRENT FAULT = TRUE"));
    }
    if ((fault_code & LTC4282_UV_ALERT) != 0)
    {
      Serial.println(F("UNDER VOLTAGE FAULT = TRUE"));
    }
    if ((fault_code & LTC4282_OV_ALERT) != 0)
    {
      Serial.println(F("OVER VOLTAGE FAULT = TRUE"));
    }
  }

  //! Display ADC Alert Log Register
  Serial.print(F("\n****ADC ALERT LOG****\n"));

  if (adc_alert_code == 0)
  {
    Serial.println(F("NO ALERTS LOGGED"));
  }
  else
  {
    if ((adc_alert_code & LTC4282_POWER_ALARM_HIGH) != 0)
    {
      Serial.println(F("ALERT - POWER ALARM HIGH"));
    }
    if ((adc_alert_code & LTC4282_POWER_ALARM_LOW) != 0)
    {
      Serial.println(F("ALERT - POWER ALARM LOW"));
    }
    if ((adc_alert_code & LTC4282_VSENSE_ALARM_HIGH ) != 0)
    {
      Serial.println(F("ALERT - VSENSE ALARM HIGH"));
    }
    if ((adc_alert_code & LTC4282_VSENSE_ALARM_LOW ) != 0)
    {
      Serial.println(F("ALERT - VSENSE ALARM LOW"));
    }
    if ((adc_alert_code & LTC4282_VSOURCE_ALARM_HIGH) != 0)
    {
      Serial.println(F("ALERT - VSOURCE ALARM HIGH"));
    }
    if ((adc_alert_code & LTC4282_VSOURCE_ALARM_LOW) != 0)
    {
      Serial.println(F("ALERT - VSOURCE ALARM LOW"));
    }
    if ((adc_alert_code & LTC4282_VGPIO_ALARM_HIGH) != 0)
    {
      Serial.println(F("ALERT - VGPIO ALARM HIGH"));
    }
    if ((adc_alert_code & LTC4282_VGPIO_ALARM_LOW ) != 0)
    {
      Serial.println(F("ALERT VGPIO ALARM LOW\n"));
    }
  }

  Serial.println(F("\n***************************************\n"));

  return ack;
}

//! Clear Faults
int8_t menu_3_clear_faults_menu_2()
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
{

  int8_t ack = 0;
  uint8_t fault_code = 0x00;
  uint8_t adc_alert_code = 0x00;
  ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_FAULT_LOG_REG, fault_code);
  ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_ADC_ALERT_LOG_REG, adc_alert_code);

  Serial.println("ALL FAULTS CLEARED");

  return ack;
}

//! Set Alert Register Menu
int8_t menu_4_set_alert_registers()
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
{

  uint8_t ack = 0;

  uint8_t user_command;

  uint16_t alert_code;

  do
  {

    Serial.print(F("****Alert Register Menu****\n\n"));
    Serial.print(F("1 - Set Alert MSB Register\n"));
    Serial.print(F("2 - Set Alert LSB Register\n"));
    Serial.print(F("m-Main Menu\n"));

    Serial.print(F("\n\nEnter a Command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
    {
      Serial.println(user_command);
    }

    Serial.println();
    switch (user_command)
    {
      case 1:  // Set Alert MSB Register.
        ack |= menu_4_set_alert_msb_register_menu_1();
        break;

      case 2:  // Set Alert LSB Register
        ack |= menu_4_set_alert_lsb_register_menu_2();
        break;
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }

  }
  while (!((user_command == 'm') || (ack)));

  return ack;


}

//! Set Alert MSB Register
int8_t menu_4_set_alert_msb_register_menu_1()
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
{

  int8_t ack = 0;

  uint8_t ALERTREG = 0;

  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_ALERT_MSB_REG, &ALERTREG);

  Serial.println(F("****SET ALERT REGISTERS****"));
  Serial.println(F("Use Numeric Commands: No = 0,  Yes = 1"));
  Serial.print(F("ENABLE EEPROM DONE ALERT? = "));
  int alertEnable = read_int();
  Serial.println(alertEnable);
  if (alertEnable)
  {
    ALERTREG = ALERTREG | LTC4282_EEPROM_DONE_ALERT;
  }
  else
  {
    ALERTREG = ALERTREG & ~LTC4282_EEPROM_DONE_ALERT;
  }

  Serial.print(F("ENABLE FET BAD FAULT ALERT? = "));
  alertEnable = read_int();
  Serial.println(alertEnable);
  if (alertEnable)
  {
    ALERTREG = ALERTREG | LTC4282_FET_BAD_FAULT_ALERT;
  }
  else
  {
    ALERTREG = ALERTREG & ~LTC4282_FET_BAD_FAULT_ALERT;
  }

  Serial.print(F("ENABLE FET SHORT FAULT? = "));
  alertEnable = read_int();
  Serial.println(alertEnable);
  if (alertEnable)
  {
    ALERTREG = ALERTREG | LTC4282_FET_SHORT_ALERT;
  }
  else
  {
    ALERTREG = ALERTREG & ~LTC4282_FET_SHORT_ALERT;
  }

  Serial.print(F("ENABLE ON ALERT? = "));
  alertEnable = read_int();
  Serial.println(alertEnable);
  if (alertEnable)
  {
    ALERTREG = ALERTREG | LTC4282_ON_ALERT;
  }
  else
  {
    ALERTREG = ALERTREG & ~LTC4282_ON_ALERT;
  }

  Serial.print(F("ENABLE POWER BAD FAULT ALERT? = "));
  alertEnable = read_int();
  Serial.println(alertEnable);
  if (alertEnable)
  {
    ALERTREG = ALERTREG | LTC4282_PB_ALERT;
  }
  else
  {
    ALERTREG = ALERTREG & ~LTC4282_PB_ALERT;
  }

  Serial.print(F("ENABLE OVER CURRENT FAULT ALERT? = "));
  alertEnable = read_int();
  Serial.println(alertEnable);
  if (alertEnable)
  {
    ALERTREG = ALERTREG | LTC4282_OC_ALERT;
  }
  else
  {
    ALERTREG = ALERTREG & ~LTC4282_OC_ALERT;
  }

  Serial.print(F("ENABLE UNDERVOLTAGE FAULT ALERT? = "));
  alertEnable = read_int();
  Serial.println(alertEnable);
  if (alertEnable)
  {
    ALERTREG = ALERTREG | LTC4282_UV_ALERT;
  }
  else
  {
    ALERTREG = ALERTREG & ~LTC4282_UV_ALERT;
  }

  Serial.print(F("ENABLE OVERVOLTAGE FAULT ALERT? = "));
  alertEnable = read_int();
  Serial.println(alertEnable);
  if (alertEnable)
  {
    ALERTREG = ALERTREG | LTC4282_OV_ALERT;
  }
  else
  {
    ALERTREG = ALERTREG & ~LTC4282_OV_ALERT;
  }

  ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_ALERT_MSB_REG, ALERTREG);

  Serial.println(F("Alert MSB Register Updated\n"));

  return ack;

}

// Set Alert LSB Register
int8_t menu_4_set_alert_lsb_register_menu_2()
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
{
  int ack = 0;
  uint8_t ALERTREG = 0;

  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_ALERT_LSB_REG, &ALERTREG);

  Serial.println(F("****SET ALERT REGISTERS****"));
  Serial.println(F("Use Numeric Commands: No = 0,  Yes = 1"));
  Serial.print(F("ENABLE POWER ALARM HIGH ALERT? = "));
  int alertEnable = read_int();
  Serial.println(alertEnable);
  if (alertEnable)
  {
    ALERTREG = ALERTREG | LTC4282_POWER_ALARM_HIGH;
  }
  else
  {
    ALERTREG = ALERTREG & ~LTC4282_POWER_ALARM_HIGH;
  }

  Serial.print(F("ENABLE POWER ALARM LOW ALERT? = "));
  alertEnable = read_int();
  Serial.println(alertEnable);
  if (alertEnable)
  {
    ALERTREG = ALERTREG | LTC4282_POWER_ALARM_LOW;
  }
  else
  {
    ALERTREG = ALERTREG & ~LTC4282_POWER_ALARM_LOW;
  }

  Serial.print(F("ENABLE VSENSE ALARM HIGH ALERT? = "));
  alertEnable = read_int();
  Serial.println(alertEnable);
  if (alertEnable)
  {
    ALERTREG = ALERTREG | LTC4282_VSENSE_ALARM_HIGH;
  }
  else
  {
    ALERTREG = ALERTREG & ~LTC4282_VSENSE_ALARM_HIGH;
  }

  Serial.print(F("ENABLE VSENSE ALARM LOW ALERT? = "));
  alertEnable = read_int();
  Serial.println(alertEnable);
  if (alertEnable)
  {
    ALERTREG = ALERTREG | LTC4282_VSENSE_ALARM_LOW;
  }
  else
  {
    ALERTREG = ALERTREG & ~LTC4282_VSENSE_ALARM_LOW;
  }

  Serial.print(F("ENABLE VSOURCE ALARM HIGH ALERT? = "));
  alertEnable = read_int();
  Serial.println(alertEnable);
  if (alertEnable)
  {
    ALERTREG = ALERTREG | LTC4282_VSOURCE_ALARM_HIGH;
  }
  else
  {
    ALERTREG = ALERTREG & ~LTC4282_VSOURCE_ALARM_HIGH;
  }

  Serial.print(F("ENABLE VSOURCE ALARM LOW ALERT? = "));
  alertEnable = read_int();
  Serial.println(alertEnable);
  if (alertEnable)
  {
    ALERTREG = ALERTREG | LTC4282_VSOURCE_ALARM_LOW;
  }
  else
  {
    ALERTREG = ALERTREG & ~LTC4282_VSOURCE_ALARM_LOW;
  }

  Serial.print(F("ENABLE VGPIO ALARM HIGH ALERT? = "));
  alertEnable = read_int();
  Serial.println(alertEnable);
  if (alertEnable)
  {
    ALERTREG = ALERTREG | LTC4282_VGPIO_ALARM_HIGH;
  }
  else
  {
    ALERTREG = ALERTREG & ~LTC4282_VGPIO_ALARM_HIGH;
  }

  Serial.print(F("ENABLE VGPIO ALARM LOW ALERT? = "));
  alertEnable = read_int();
  Serial.println(alertEnable);
  if (alertEnable)
  {
    ALERTREG = ALERTREG | LTC4282_VGPIO_ALARM_LOW;
  }
  else
  {
    ALERTREG = ALERTREG & ~LTC4282_VGPIO_ALARM_LOW;
  }

  ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_ALERT_LSB_REG, ALERTREG);

  Serial.println(F("Alert LSB Register Updated\n"));

  return ack;
}

//! Settings Menu
int8_t menu_5_settings(uint8_t *CTRLREG_MSB,              //!< Local Control MSB Register to Change Through Settings. This is shared across multiple functions such as Continious mode.
                       uint8_t *CTRLREG_LSB,              //!< Local Control LSB Register to Change Through Settings. This is shared across multiple functions such as Continious mode.
                       uint8_t *ILIM_ADJUST,              //!< Local ILIM ADJUST Register to Change Through Settings. This is shared across multiple functions such as Continious mode.
                       uint8_t *CLK_DECIMATOR             //!< Local ILIM ADJUST Register to Change Through Settings. This is shared across multiple functions such as Continious mode.
                      )
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
{

  int8_t ack = 0;

  uint8_t user_command;

  uint16_t alert_code;

  do
  {
    Serial.print(F("****Settings Menu****\n\n"));
    Serial.print(F("1 - Set Control MSB Register\n"));
    Serial.print(F("2 - Set Control LSB Register\n"));
    Serial.print(F("3 - Set ILIM Register\n"));
    Serial.print(F("4 - Configure GPIO Pins\n"));
    Serial.print(F("5 - Write Register Values to EEPROM\n"));
    Serial.print(F("6 - Update Register Values From EEPROM\n"));
    Serial.print(F("m-Main Menu\n"));

    Serial.print(F("\n\nEnter a Command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
    {
      Serial.println(user_command);
    }

    Serial.println();
    switch (user_command)
    {
      case 1:  // Control MSB Register Menu. Configure OV/UV/OC Autoretry, FET ON, Mass Write Enable, ON/ENB, ON DELAY and ON FAULT MASK.
        ack |= menu_5_set_control_msb_register_menu_1(CTRLREG_MSB);
        break;
      case 2:  // Control LSB Register Menu. Configure Thresholds for POWER GOOD/OV/UV as well as for VIN MODE.
        ack |= menu_5_set_control_lsb_register_menu_2(CTRLREG_LSB);
        break;
      case 3:  // ILIM ADJUST Register Menu. Configure Current Limit/Foldback Mode/ADC Voltage Monitoring/16 or 12-bit Mode
        ack |= menu_5_set_ilim_adjust_register_menu_3(ILIM_ADJUST);
        break;
      case 4:  // Configure State of GPIO Pins. Set GPIO1 as POWERGOOD/POWERBAD/GPO/GPI, OverFlow Alert, GPIO2 Stress Mode etc.
        ack |= menu_5_configure_GPIO_pins_menu_4();
        break;
      case 5:  // Write Values currently present in the Registers of the LTC4282 to the EEPROM
        ack |= menu_5_write_register_values_to_EEPROM_menu_5();
        break;
      case 6:  // Restore Values Currently Present in the EEPROM to the Registers of the LTC4282
        ack |= menu_5_restore_register_values_from_EEPROM_menu_6();
        break;
      default:
        if (user_command != 'm')
          Serial.println(F("Incorrect Option"));
        break;
    }

  }
  while (!((user_command == 'm') || (ack)));

  return ack;
}

//! Control MSB Register Menu. Configure OV/UV/OC Autoretry, FET ON, Mass Write Enable, ON/ENB, ON DELAY and ON FAULT MASK.
int8_t menu_5_set_control_msb_register_menu_1(uint8_t *CTRLREG_MSB           //!< Local Control MSB Register to Change Through Settings. This is shared across multiple functions such as Continious mode.
                                             )
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
{

  int ack = 0;
  Serial.println(F("****SET MSB CONTROL REGISTERS****"));
  Serial.println(F("Use Numeric Commands: DISABLE = 0,  ENABLE = 1"));
  Serial.print(F("ENABLE ON FAULT MASK? = "));
  int controlEnable = read_int();
  Serial.println(controlEnable);
  if (controlEnable)
  {
    *CTRLREG_MSB = *CTRLREG_MSB | LTC4282_ON_FAULT_MASK;
  }
  else
  {
    *CTRLREG_MSB = *CTRLREG_MSB & ~LTC4282_ON_FAULT_MASK;
  }


  Serial.print(F("ENABLE 50ms ON DELAY? = "));
  controlEnable = read_int();
  Serial.println(controlEnable);
  if (controlEnable)
  {
    *CTRLREG_MSB = *CTRLREG_MSB | LTC4282_ON_DELAY;
  }
  else
  {
    *CTRLREG_MSB = *CTRLREG_MSB & ~LTC4282_ON_DELAY;
  }

  Serial.print(F("ENABLE ON ACTIVE HIGH? = "));
  controlEnable = read_int();
  Serial.println(controlEnable);
  if (controlEnable)
  {
    *CTRLREG_MSB = *CTRLREG_MSB | LTC4282_ON_ENB;
  }
  else
  {
    *CTRLREG_MSB = *CTRLREG_MSB & ~LTC4282_ON_ENB;
  }

  Serial.print(F("ENABLE MASS WRITE? = "));
  controlEnable = read_int();
  Serial.println(controlEnable);
  if (controlEnable)
  {
    *CTRLREG_MSB = *CTRLREG_MSB | LTC4282_MASS_WRITE;
  }
  else
  {
    *CTRLREG_MSB = *CTRLREG_MSB & ~LTC4282_MASS_WRITE;
  }

  Serial.print(F("ENABLE FET TURN ON? = "));
  controlEnable = read_int();
  Serial.println(controlEnable);
  if (controlEnable)
  {
    *CTRLREG_MSB = *CTRLREG_MSB | LTC4282_FET_ON;
  }
  else
  {
    *CTRLREG_MSB = *CTRLREG_MSB & ~LTC4282_FET_ON;
  }

  Serial.print(F("ENABLE OVERCURRENT AUTORETRY? = "));
  controlEnable = read_int();
  Serial.println(controlEnable);
  if (controlEnable)
  {
    *CTRLREG_MSB = *CTRLREG_MSB | LTC4282_OC_AUTORETRY;
  }
  else
  {
    *CTRLREG_MSB = *CTRLREG_MSB & ~LTC4282_OC_AUTORETRY;
  }

  Serial.print(F("ENABLE UNDERVOLTAGE AUTORETRY? = "));
  controlEnable = read_int();
  Serial.println(controlEnable);
  if (controlEnable)
  {
    *CTRLREG_MSB = *CTRLREG_MSB | LTC4282_UV_AUTORETRY;
  }
  else
  {
    *CTRLREG_MSB = *CTRLREG_MSB & ~LTC4282_UV_AUTORETRY;
  }

  Serial.print(F("ENABLE OVERVOLTAGE AUTORETRY? = "));
  controlEnable = read_int();
  Serial.println(controlEnable);
  if (controlEnable)
  {
    *CTRLREG_MSB = *CTRLREG_MSB | LTC4282_OV_AUTORETRY;
  }
  else
  {
    *CTRLREG_MSB = *CTRLREG_MSB & ~LTC4282_OV_AUTORETRY;
  }


  ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_CONTROL_MSB_REG, *CTRLREG_MSB);

  Serial.println(F("Control MSB Register Updated\n"));

  return ack;

}
// Control LSB Register Menu. Configure Thresholds for POWER GOOD/OV/UV as well as for VIN MODE.
int8_t menu_5_set_control_lsb_register_menu_2(uint8_t *CTRLREG_LSB                //!< Local Control LSB Register to Change Through Settings. This is shared across multiple functions such as Continious mode.
                                             )
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
{


  int ack = 0;
  uint8_t user_command;
  Serial.println(F("****SET LSB CONTROL REGISTERS****"));
  Serial.print(F("SELECT POWER GOOD THRESHOLD:\n"));
  Serial.print(F("       1. EXTERNAL\n"));
  Serial.print(F("       2. 5%\n"));
  Serial.print(F("       3. 10%\n"));
  Serial.print(F("       4. 15%\n"));

  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.print(user_command);
  Serial.print(F("\n"));
  switch (user_command)
  {

    case 1:
      *CTRLREG_LSB = (*CTRLREG_LSB & ~LTC4282_FB_MODE_MASK) | (LTC4282_FB_MODE_EXTERNAL & LTC4282_FB_MODE_MASK);
      break;
    case 2:
      *CTRLREG_LSB = (*CTRLREG_LSB & ~LTC4282_FB_MODE_MASK) | (LTC4282_FB_MODE_5 & LTC4282_FB_MODE_MASK);
      break;
    case 3:
      *CTRLREG_LSB = (*CTRLREG_LSB & ~LTC4282_FB_MODE_MASK) | (LTC4282_FB_MODE_10 & LTC4282_FB_MODE_MASK);
      break;
    case 4:
      *CTRLREG_LSB = (*CTRLREG_LSB & ~LTC4282_FB_MODE_MASK) | (LTC4282_FB_MODE_15 & LTC4282_FB_MODE_MASK);
      break;
    default:
      Serial.println(F("Invalid entry. Exiting Menu.."));
      return 0;
  }

  Serial.print(F("SELECT UV THRESHOLD:\n"));
  Serial.print(F("       1. EXTERNAL\n"));
  Serial.print(F("       2. 5%\n"));
  Serial.print(F("       3. 10%\n"));
  Serial.print(F("       4. 15%\n"));

  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.print(user_command);
  Serial.print(F("\n"));
  switch (user_command)
  {

    case 1:
      *CTRLREG_LSB = (*CTRLREG_LSB & ~LTC4282_UV_MODE_MASK) | (LTC4282_UV_MODE_EXTERNAL & LTC4282_UV_MODE_MASK);
      break;
    case 2:
      *CTRLREG_LSB = (*CTRLREG_LSB & ~LTC4282_UV_MODE_MASK) | (LTC4282_UV_MODE_5 & LTC4282_UV_MODE_MASK);
      break;
    case 3:
      *CTRLREG_LSB = (*CTRLREG_LSB & ~LTC4282_UV_MODE_MASK) | (LTC4282_UV_MODE_10 & LTC4282_UV_MODE_MASK);
      break;
    case 4:
      *CTRLREG_LSB = (*CTRLREG_LSB & ~LTC4282_UV_MODE_MASK) | (LTC4282_UV_MODE_15 & LTC4282_UV_MODE_MASK);
      break;
    default:
      Serial.println(F("Invalid entry. Exiting Menu.."));
      return 0;
  }

  Serial.print(F("SELECT OV THRESHOLD:\n"));
  Serial.print(F("       1. EXTERNAL\n"));
  Serial.print(F("       2. 5%\n"));
  Serial.print(F("       3. 10%\n"));
  Serial.print(F("       4. 15%\n"));

  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.print(user_command);
  Serial.print(F("\n"));
  switch (user_command)
  {

    case 1:
      *CTRLREG_LSB = (*CTRLREG_LSB & ~LTC4282_OV_MODE_MASK) | (LTC4282_OV_MODE_EXTERNAL & LTC4282_OV_MODE_MASK);
      break;
    case 2:
      *CTRLREG_LSB = (*CTRLREG_LSB & ~LTC4282_OV_MODE_MASK) | (LTC4282_OV_MODE_5 & LTC4282_OV_MODE_MASK);
      break;
    case 3:
      *CTRLREG_LSB = (*CTRLREG_LSB & ~LTC4282_OV_MODE_MASK) | (LTC4282_OV_MODE_10 & LTC4282_OV_MODE_MASK);
      break;
    case 4:
      *CTRLREG_LSB = (*CTRLREG_LSB & ~LTC4282_OV_MODE_MASK) | (LTC4282_OV_MODE_15 & LTC4282_OV_MODE_MASK);
      break;
    default:
      Serial.println(F("Invalid entry. Exiting Menu.."));
      return 0;
  }

  Serial.print(F("SELECT VIN MODE:\n"));
  Serial.print(F("       1. 3.3V\n"));
  Serial.print(F("       2. 5V\n"));
  Serial.print(F("       3. 12V\n"));
  Serial.print(F("       4. 24V\n"));

  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.print(user_command);
  Serial.print(F("\n"));
  switch (user_command)
  {

    case 1:
      *CTRLREG_LSB = (*CTRLREG_LSB & ~LTC4282_VIN_MODE_MASK) | (LTC4282_VIN_MODE_3_V_3 & LTC4282_VIN_MODE_MASK);
      break;
    case 2:
      *CTRLREG_LSB = (*CTRLREG_LSB & ~LTC4282_VIN_MODE_MASK) | (LTC4282_VIN_MODE_5_V & LTC4282_VIN_MODE_MASK);
      break;
    case 3:
      *CTRLREG_LSB = (*CTRLREG_LSB & ~LTC4282_VIN_MODE_MASK) | (LTC4282_VIN_MODE_12_V & LTC4282_VIN_MODE_MASK);
      break;
    case 4:
      *CTRLREG_LSB = (*CTRLREG_LSB & ~LTC4282_VIN_MODE_MASK) | (LTC4282_VIN_MODE_24_V & LTC4282_VIN_MODE_MASK);
      break;
    default:
      Serial.println(F("Invalid entry. Exiting Menu.."));
      return 0;
  }

  ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_CONTROL_LSB_REG, *CTRLREG_LSB);

  Serial.println(F("Control LSB Register Updated\n"));

  return ack;

}
// ILIM ADJUST Register Menu. Configure Current Limit/Foldback Mode/ADC Voltage Monitoring/16 or 12-bit Mode
uint8_t menu_5_set_ilim_adjust_register_menu_3(uint8_t *ILIM_ADJUST           //!< Local ILIM ADJUST Register to Change Through Settings. This is shared across multiple functions such as Continious mode.
                                              )
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
{

  int8_t ack = 0;

  uint8_t user_command;
  Serial.println(F("****SET ILIM ADJUST REGISTER****"));
  Serial.print(F("SELECT CURRENT LIMIT THRESHOLD:\n"));
  Serial.print(F("       1. 34.3mV\n"));
  Serial.print(F("       2. 31.25mV\n"));
  Serial.print(F("       3. 28.12mV\n"));
  Serial.print(F("       4. 25mV\n"));
  Serial.print(F("       5. 21.87mV\n"));
  Serial.print(F("       6. 18.75mV\n"));
  Serial.print(F("       7. 15.62mV\n"));
  Serial.print(F("       8. 12.5mV\n"));
  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.println(user_command);
  Serial.print(F("\n"));
  switch (user_command)
  {

    case 1:
      *ILIM_ADJUST = (*ILIM_ADJUST & ~LTC4282_ILIM_ADJUST_MASK) | (LTC4282_ILIM_ADJUST_34_V_3 & LTC4282_ILIM_ADJUST_MASK);
      break;
    case 2:
      *ILIM_ADJUST = (*ILIM_ADJUST & ~LTC4282_ILIM_ADJUST_MASK) | (LTC4282_ILIM_ADJUST_31_V_2 & LTC4282_ILIM_ADJUST_MASK);
      break;
    case 3:
      *ILIM_ADJUST = (*ILIM_ADJUST & ~LTC4282_ILIM_ADJUST_MASK) | (LTC4282_ILIM_ADJUST_28_V_1 & LTC4282_ILIM_ADJUST_MASK);
      break;
    case 4:
      *ILIM_ADJUST = (*ILIM_ADJUST & ~LTC4282_ILIM_ADJUST_MASK) | (LTC4282_ILIM_ADJUST_25_V_0 & LTC4282_ILIM_ADJUST_MASK);
      break;
    case 5:
      *ILIM_ADJUST = (*ILIM_ADJUST & ~LTC4282_ILIM_ADJUST_MASK) | (LTC4282_ILIM_ADJUST_21_V_8 & LTC4282_ILIM_ADJUST_MASK);
      break;
    case 6:
      *ILIM_ADJUST = (*ILIM_ADJUST & ~LTC4282_ILIM_ADJUST_MASK) | (LTC4282_ILIM_ADJUST_18_V_7 & LTC4282_ILIM_ADJUST_MASK);
      break;
    case 7:
      *ILIM_ADJUST = (*ILIM_ADJUST & ~LTC4282_ILIM_ADJUST_MASK) | (LTC4282_ILIM_ADJUST_15_V_6 & LTC4282_ILIM_ADJUST_MASK);
      break;
    case 8:
      *ILIM_ADJUST = (*ILIM_ADJUST & ~LTC4282_ILIM_ADJUST_MASK) | (LTC4282_ILIM_ADJUST_12_V_5 & LTC4282_ILIM_ADJUST_MASK);
      break;
    default:
      Serial.println(F("Invalid entry. Exiting Menu.."));
      return 0;
  }

  Serial.print(F("SELECT FOLDBACK MODE:\n"));
  Serial.print(F("       1. 3.3V\n"));
  Serial.print(F("       2. 5V\n"));
  Serial.print(F("       3. 12V\n"));
  Serial.print(F("       4. 24V\n"));
  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.println(user_command);
  Serial.print(F("\n"));
  switch (user_command)
  {

    case 1:
      *ILIM_ADJUST = (*ILIM_ADJUST & ~LTC4282_FOLDBACK_MASK) | (LTC4282_FOLDBACK_MODE_3_V_3 & LTC4282_FOLDBACK_MASK);
      break;
    case 2:
      *ILIM_ADJUST = (*ILIM_ADJUST & ~LTC4282_FOLDBACK_MASK) | (LTC4282_FOLDBACK_MODE_5_V_0 & LTC4282_FOLDBACK_MASK);
      break;
    case 3:
      *ILIM_ADJUST = (*ILIM_ADJUST & ~LTC4282_FOLDBACK_MASK) | (LTC4282_FOLDBACK_MODE_12_V_0 & LTC4282_FOLDBACK_MASK);
      break;
    case 4:
      *ILIM_ADJUST = (*ILIM_ADJUST & ~LTC4282_FOLDBACK_MASK) | (LTC4282_FOLDBACK_MODE_24_V_0 & LTC4282_FOLDBACK_MASK);
      break;
    default:
      Serial.println(F("Invalid entry. Exiting Menu.."));
      return 0;
  }

  Serial.print(F("SELECT ADC VOLTAGE MONITOR MODE:\n"));
  Serial.print(F("       1. SOURCE\n"));
  Serial.print(F("       2. VDD\n"));
  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.println(user_command);
  Serial.print(F("\n"));
  switch (user_command)
  {
    case 1:
      *ILIM_ADJUST = *ILIM_ADJUST | LTC4282_ADC_VSOURCE;
      break;
    case 2:
      *ILIM_ADJUST = *ILIM_ADJUST & ~LTC4282_ADC_VSOURCE;
      break;
    default:
      Serial.println(F("Invalid entry. Exiting Menu.."));
      return 0;
  }

  Serial.print(F("SELECT GPIO MONITOR MODE:\n"));
  Serial.print(F("       1. GPIO2\n"));
  Serial.print(F("       2. GPIO3\n"));
  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.println(user_command);
  Serial.print(F("\n"));
  switch (user_command)
  {
    case 1:
      *ILIM_ADJUST = *ILIM_ADJUST | LTC4282_ADC_GPIO2_MODE;
      break;
    case 2:
      *ILIM_ADJUST = *ILIM_ADJUST & ~LTC4282_ADC_GPIO2_MODE;
      break;
    default:
      Serial.println(F("Invalid entry. Exiting Menu.."));
      return 0;
  }


  Serial.print(F("SELECT ADC RESOLUTION:\n"));
  Serial.print(F("       1. 16-bit\n"));
  Serial.print(F("       2. 12-bit\n"));
  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.println(user_command);
  Serial.print(F("\n"));
  switch (user_command)
  {
    case 1:
      *ILIM_ADJUST = *ILIM_ADJUST | LTC4282_ADC_16_BIT;
      break;
    case 2:
      *ILIM_ADJUST = *ILIM_ADJUST & ~LTC4282_ADC_16_BIT;
      break;
    default:
      Serial.println(F("Invalid entry. Exiting Menu.."));
      return 0;
  }

  ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_ILIM_ADJUST_REG, *ILIM_ADJUST);

  Serial.println(F("Control LSB Register Updated\n"));
  return ack;
}

// Configure State of GPIO Pins. Set GPIO1 as POWERGOOD/POWERBAD/GPO/GPI, OverFlow Alert, GPIO2 Stress Mode etc.
uint8_t menu_5_configure_GPIO_pins_menu_4()
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
{

  uint8_t ack = 0;

  uint8_t gpio_config_code;

  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_GPIO_CONFIG_REG, &gpio_config_code);

  uint8_t user_command;
  Serial.println(F("****SET GPIO_CONFIG REGISTER****"));
  Serial.print(F("Set GPIO3 Pulldown: \n"));
  Serial.print(F("       1. Enable\n"));
  Serial.print(F("       2. Disable\n"));
  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.println(user_command);
  Serial.print(F("\n"));
  switch (user_command)
  {
    case 1:
      gpio_config_code |= LTC4282_GPIO3_PD;
      break;
    case 2:
      gpio_config_code = gpio_config_code &~LTC4282_GPIO3_PD;
      break;
    default:
      Serial.print(F("Invalid Entry. No Changes Made\n"));
  }

  Serial.print(F("Set GPIO2 Pulldown: \n"));
  Serial.print(F("       1. Enable\n"));
  Serial.print(F("       2. Disable\n"));
  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.println(user_command);
  Serial.print(F("\n"));
  switch (user_command)
  {
    case 1:
      gpio_config_code |= LTC4282_GPIO2_PD;
      break;
    case 2:
      gpio_config_code = gpio_config_code &~LTC4282_GPIO2_PD;
      break;
    default:
      Serial.print(F("Invalid Entry. No Changes Made\n"));
  }

  Serial.print(F("Configure GPIO1: \n"));
  Serial.print(F("       1. Power Good\n"));
  Serial.print(F("       2. Power Bad\n"));
  Serial.print(F("       3. General Purpose Output\n"));
  Serial.print(F("       4. General Purpose Input\n"));
  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.println(user_command);
  Serial.print(F("\n"));
  switch (user_command)
  {
    case 1:
      gpio_config_code = (gpio_config_code & ~LTC4282_GPIO1_CONFIG_MASK) | ( LTC4282_GPIO1_CONFIG_POWER_GOOD & LTC4282_GPIO1_CONFIG_MASK);
      break;
    case 2:
      gpio_config_code = (gpio_config_code & ~LTC4282_GPIO1_CONFIG_MASK) | ( LTC4282_GPIO1_CONFIG_POWER_BAD & LTC4282_GPIO1_CONFIG_MASK);
      break;
    case 3:
      {
        gpio_config_code = (gpio_config_code & ~LTC4282_GPIO1_CONFIG_MASK) | ( LTC4282_GPIO1_CONFIG_OUTPUT & LTC4282_GPIO1_CONFIG_MASK);
        Serial.print(F("GPIO1 Configured as output..\n"));
        Serial.print(F("Set Pull Down: \n"));
        Serial.print(F("       1. Enable\n"));
        Serial.print(F("       2. Disable\n"));
        user_command = read_int();
        Serial.print(F("Selected Value: "));
        Serial.println(user_command);
        Serial.print(F("\n"));
        switch (user_command)
        {
          case 1:
            gpio_config_code = gpio_config_code & ~LTC4282_GPIO1_OUTPUT;
            break;
          case 2:
            gpio_config_code |= LTC4282_GPIO1_OUTPUT;
            break;
          default:
            Serial.print(F("Invalid Entry. No Changes Made\n"));
        }
      }
      break;
    case 4:
      gpio_config_code = (gpio_config_code & ~LTC4282_GPIO1_CONFIG_MASK) | ( LTC4282_GPIO1_CONFIG_INPUT & LTC4282_GPIO1_CONFIG_MASK);
      break;
    default:
      Serial.print(F("Invalid Entry. No Changes Made\n"));
  }

  Serial.print(F("ADC Conversion Alerts: \n"));
  Serial.print(F("       1. Enable\n"));
  Serial.print(F("       2. Disable\n"));
  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.println(user_command);
  Serial.print(F("\n"));
  switch (user_command)
  {
    case 1:
      gpio_config_code |= LTC4282_ADC_CONV_ALERT;
      break;
    case 2:
      gpio_config_code = gpio_config_code &~LTC4282_ADC_CONV_ALERT;
      break;
    default:
      Serial.print(F("Invalid Entry. No Changes Made\n"));
  }

  Serial.print(F("GPIO2 Pull Low When MOSFET Under Stress: \n"));
  Serial.print(F("       1. Enable\n"));
  Serial.print(F("       2. Disable\n"));
  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.println(user_command);
  Serial.print(F("\n"));
  switch (user_command)
  {
    case 1:
      gpio_config_code |= LTC4282_STRESS_TO_GPIO2;
      break;
    case 2:
      gpio_config_code = gpio_config_code &~LTC4282_STRESS_TO_GPIO2;
      break;
    default:
      Serial.print(F("Invalid Entry. No Changes Made\n"));
  }

  Serial.print(F("Set Energy And Timer Overflow Alert: \n"));
  Serial.print(F("       1. Enable\n"));
  Serial.print(F("       2. Disable\n"));
  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.println(user_command);
  Serial.print(F("\n"));
  switch (user_command)
  {
    case 1:
      gpio_config_code |= LTC4282_METER_OF_ALERT;
      break;
    case 2:
      gpio_config_code = gpio_config_code &~LTC4282_METER_OF_ALERT;
      break;
    default:
      Serial.print(F("Invalid Entry. No Changes Made\n"));
  }

  ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_GPIO_CONFIG_REG, gpio_config_code);
  Serial.print(F("\n"));
  return ack;
}

// Write Values currently present in the Registers of the LTC4282 to the EEPROM
uint8_t menu_5_write_register_values_to_EEPROM_menu_5()
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
{

  uint8_t ack =0;

  uint8_t CTRLREG_MSB;
  uint8_t CTRLREG_LSB;
  uint8_t ALRTREG_MSB;
  uint8_t ALRTREG_LSB;
  uint8_t FAULT_REG;
  uint8_t ADC_ALERT_REG;
  uint8_t FET_BAD_TIME;
  uint8_t GPIO_CONFIG;
  uint8_t VGPIO_ALARM_MIN;
  uint8_t VGPIO_ALARM_MAX;
  uint8_t VSOURCE_ALARM_MIN;
  uint8_t VSOURCE_ALARM_MAX;
  uint8_t VSENSE_ALARM_MIN;
  uint8_t VSENSE_ALARM_MAX;
  uint8_t POWER_ALARM_MIN;
  uint8_t POWER_ALARM_MAX;
  uint8_t CLK_DECIMATOR;
  uint8_t ILIM_ADJUST;

  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_CONTROL_MSB_REG, &CTRLREG_MSB);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_CONTROL_LSB_REG, &CTRLREG_LSB);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_ALERT_MSB_REG, &ALRTREG_MSB);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_ALERT_LSB_REG, &ALRTREG_LSB);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_FAULT_LOG_REG, &FAULT_REG);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_ADC_ALERT_LOG_REG, &ADC_ALERT_REG);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_FET_BAD_FAULT_TIME_REG, &FET_BAD_TIME);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_GPIO_CONFIG_REG, &GPIO_CONFIG);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_VGPIO_ALARM_MIN_REG, &VGPIO_ALARM_MIN);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_VGPIO_ALARM_MAX_REG, &VGPIO_ALARM_MAX);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_VSOURCE_ALARM_MIN_REG, &VSOURCE_ALARM_MIN);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_VSOURCE_ALARM_MAX_REG, &VSOURCE_ALARM_MAX);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_VSENSE_ALARM_MIN_REG, &VSENSE_ALARM_MIN);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_VSENSE_ALARM_MAX_REG, &VSENSE_ALARM_MAX);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_POWER_ALARM_MIN_REG, &POWER_ALARM_MIN);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_POWER_ALARM_MAX_REG, &POWER_ALARM_MAX);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_CLK_DEC_REG, &CLK_DECIMATOR);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_ILIM_ADJUST_REG, &ILIM_ADJUST);


  uint8_t user_command;
  Serial.println(F("****CURRENT REGISTER VALUES TO BE COPIED TO EEPROM****"));
  Serial.print(F("CONTROL MSB: "));
  Serial.println(CTRLREG_MSB);
  Serial.print(F("CONTROL LSB: "));
  Serial.println((CTRLREG_LSB));
  Serial.print(F("ALERT MSB: "));
  Serial.println((ALRTREG_MSB));
  Serial.print(F("ALERT LSB: "));
  Serial.println((ALRTREG_LSB));
  Serial.print(F("FAULT LOG: "));
  Serial.println((FAULT_REG));
  Serial.print(F("ADC ALERT: "));
  Serial.println((ADC_ALERT_REG));
  Serial.print(F("FET BAD TIME: "));
  Serial.println((FET_BAD_TIME));
  Serial.print(F("GPIO CONFIG: "));
  Serial.println((GPIO_CONFIG));
  Serial.print(F("VGPIO ALARM MIN: "));
  Serial.println((VGPIO_ALARM_MIN));
  Serial.print(F("VGPIO ALARM MAX: "));
  Serial.println((VGPIO_ALARM_MAX));
  Serial.print(F("VSOURCE ALARM MIN: "));
  Serial.println((VSOURCE_ALARM_MIN));
  Serial.print(F("VSOURCE ALARM MAX: "));
  Serial.println((VSOURCE_ALARM_MAX));
  Serial.print(F("VSENSE ALARM MIN: "));
  Serial.println((VSENSE_ALARM_MIN));
  Serial.print(F("VSENSE ALARM MAX: "));
  Serial.println((VSENSE_ALARM_MAX));
  Serial.print(F("POWER ALARM MIN: "));
  Serial.println((POWER_ALARM_MIN));
  Serial.print(F("POWER ALARM MAX: "));
  Serial.println((POWER_ALARM_MAX));
  Serial.print(F("CLOCK DECIMATOR: "));
  Serial.println((CLK_DECIMATOR));
  Serial.print(F("ILIM ADJUST: "));
  Serial.println((ILIM_ADJUST));
  Serial.print(F("Do You Want to Enter These Values To EEPROM?\n"));
  Serial.print(F("       1. Yes\n"));
  Serial.print(F("       2. No\n"));
  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.println(user_command);
  Serial.print(F("\n"));

  switch (user_command)
  {

    case 1:
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_CONTROL_MSB_REG, CTRLREG_MSB);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_CONTROL_LSB_REG, CTRLREG_LSB);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_ALERT_MSB_REG, ALRTREG_MSB);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_ALERT_LSB_REG, ALRTREG_LSB);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_FAULT_REG, FAULT_REG);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_ADC_ALERT_LOG_REG, ADC_ALERT_REG);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_FET_BAD_FAULT_TIME_REG, FET_BAD_TIME);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_GPIO_CONFIG_REG, GPIO_CONFIG);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_VGPIO_ALARM_MIN_REG, VGPIO_ALARM_MIN);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_VGPIO_ALARM_MAX_REG, VGPIO_ALARM_MAX);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_VSOURCE_ALARM_MIN_REG, VSOURCE_ALARM_MIN);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_VSOURCE_ALARM_MAX_REG, VSOURCE_ALARM_MAX);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_VSENSE_ALARM_MIN_REG, VSENSE_ALARM_MIN);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_VSENSE_ALARM_MAX_REG, VSENSE_ALARM_MAX);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_POWER_ALARM_MIN_REG, POWER_ALARM_MIN);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_POWER_ALARM_MAX_REG, POWER_ALARM_MAX);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_CLK_DEC_REG, CLK_DECIMATOR);
      delay(50);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_EE_ILIM_ADJUST_REG, ILIM_ADJUST);

      Serial.println(F("Values Written to EEPROM\n"));
      break;
    case 2:
      Serial.print(F("Nothing Written To EEPROM\n"));
      break;
    default:
      Serial.print(F("Invalid Entry. No Changes Made\n"));
  }
  Serial.print(F("\n"));
  return ack;
}

// Restore Values Currently Present in the EEPROM to the Registers of the LTC4282
uint8_t menu_5_restore_register_values_from_EEPROM_menu_6()
//! @return Returns the state of the acknowledge bit after the I2C write/read. 0 = acknowledge, 1 = no acknowledge.
{

  uint8_t ack =0;

  uint8_t CTRLREG_MSB;
  uint8_t CTRLREG_LSB;
  uint8_t ALRTREG_MSB;
  uint8_t ALRTREG_LSB;
  uint8_t FAULT_REG;
  uint8_t ADC_ALERT_REG;
  uint8_t FET_BAD_TIME;
  uint8_t GPIO_CONFIG;
  uint8_t VGPIO_ALARM_MIN;
  uint8_t VGPIO_ALARM_MAX;
  uint8_t VSOURCE_ALARM_MIN;
  uint8_t VSOURCE_ALARM_MAX;
  uint8_t VSENSE_ALARM_MIN;
  uint8_t VSENSE_ALARM_MAX;
  uint8_t POWER_ALARM_MIN;
  uint8_t POWER_ALARM_MAX;
  uint8_t CLK_DECIMATOR;
  uint8_t ILIM_ADJUST;

  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_CONTROL_MSB_REG, &CTRLREG_MSB);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_CONTROL_LSB_REG, &CTRLREG_LSB);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_ALERT_MSB_REG, &ALRTREG_MSB);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_ALERT_LSB_REG, &ALRTREG_LSB);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_FAULT_REG, &FAULT_REG);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_ADC_ALERT_LOG_REG, &ADC_ALERT_REG);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_FET_BAD_FAULT_TIME_REG, &FET_BAD_TIME);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_GPIO_CONFIG_REG, &GPIO_CONFIG);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_VGPIO_ALARM_MIN_REG, &VGPIO_ALARM_MIN);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_VGPIO_ALARM_MAX_REG, &VGPIO_ALARM_MAX);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_VSOURCE_ALARM_MIN_REG, &VSOURCE_ALARM_MIN);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_VSOURCE_ALARM_MAX_REG, &VSOURCE_ALARM_MAX);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_VSENSE_ALARM_MIN_REG, &VSENSE_ALARM_MIN);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_VSENSE_ALARM_MAX_REG, &VSENSE_ALARM_MAX);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_POWER_ALARM_MIN_REG, &POWER_ALARM_MIN);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_POWER_ALARM_MAX_REG, &POWER_ALARM_MAX);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_CLK_DEC_REG, &CLK_DECIMATOR);
  ack |= LTC4282_read(LTC4282_I2C_ADDRESS, LTC4282_EE_ILIM_ADJUST_REG, &ILIM_ADJUST);


  uint8_t user_command;
  Serial.println(F("****RESTORE FOLLOWING VALUES FROM EEPROM****"));
  Serial.print(F("CONTROL MSB: "));
  Serial.println(CTRLREG_MSB);
  Serial.print(F("CONTROL LSB: "));
  Serial.println((CTRLREG_LSB));
  Serial.print(F("ALERT MSB: "));
  Serial.println((ALRTREG_MSB));
  Serial.print(F("ALERT LSB: "));
  Serial.println((ALRTREG_LSB));
  Serial.print(F("FAULT LOG: "));
  Serial.println((FAULT_REG));
  Serial.print(F("ADC ALERT: "));
  Serial.println((ADC_ALERT_REG));
  Serial.print(F("FET BAD TIME: "));
  Serial.println((FET_BAD_TIME));
  Serial.print(F("GPIO CONFIG: "));
  Serial.println((GPIO_CONFIG));
  Serial.print(F("VGPIO ALARM MIN: "));
  Serial.println((VGPIO_ALARM_MIN));
  Serial.print(F("VGPIO ALARM MAX: "));
  Serial.println((VGPIO_ALARM_MAX));
  Serial.print(F("VSOURCE ALARM MIN: "));
  Serial.println((VSOURCE_ALARM_MIN));
  Serial.print(F("VSOURCE ALARM MAX: "));
  Serial.println((VSOURCE_ALARM_MAX));
  Serial.print(F("VSENSE ALARM MIN: "));
  Serial.println((VSENSE_ALARM_MIN));
  Serial.print(F("VSENSE ALARM MAX: "));
  Serial.println((VSENSE_ALARM_MAX));
  Serial.print(F("POWER ALARM MIN: "));
  Serial.println((POWER_ALARM_MIN));
  Serial.print(F("POWER ALARM MAX: "));
  Serial.println((POWER_ALARM_MAX));
  Serial.print(F("CLOCK DECIMATOR: "));
  Serial.println((CLK_DECIMATOR));
  Serial.print(F("ILIM ADJUST: "));
  Serial.println((ILIM_ADJUST));
  Serial.print(F("Do You Want to Restore these Values from EEPROM?\n"));
  Serial.print(F("       1. Yes\n"));
  Serial.print(F("       2. No\n"));
  user_command = read_int();
  Serial.print(F("Selected Value: "));
  Serial.println(user_command);
  Serial.print(F("\n"));

  switch (user_command)
  {

    case 1:
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_CONTROL_MSB_REG, CTRLREG_MSB);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_CONTROL_LSB_REG, CTRLREG_LSB);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_ALERT_MSB_REG, ALRTREG_MSB);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_ALERT_LSB_REG, ALRTREG_LSB);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_FAULT_LOG_REG, FAULT_REG);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_ADC_ALERT_LOG_REG, ADC_ALERT_REG);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_FET_BAD_FAULT_TIME_REG, FET_BAD_TIME);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_GPIO_CONFIG_REG, GPIO_CONFIG);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_VGPIO_ALARM_MIN_REG, VGPIO_ALARM_MIN);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_VGPIO_ALARM_MAX_REG, VGPIO_ALARM_MAX);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_VSOURCE_ALARM_MIN_REG, VSOURCE_ALARM_MIN);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_VSOURCE_ALARM_MAX_REG, VSOURCE_ALARM_MAX);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_VSENSE_ALARM_MIN_REG, VSENSE_ALARM_MIN);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_VSENSE_ALARM_MAX_REG, VSENSE_ALARM_MAX);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_POWER_ALARM_MIN_REG, POWER_ALARM_MIN);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_POWER_ALARM_MAX_REG, POWER_ALARM_MAX);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_CLK_DEC_REG, CLK_DECIMATOR);
      ack |= LTC4282_write(LTC4282_I2C_ADDRESS, LTC4282_ILIM_ADJUST_REG, ILIM_ADJUST);

      Serial.println(F("Values Restored From EEPROM\n"));
      break;
    case 2:
      Serial.print(F("Nothing Restored.\n"));
      break;
    default:
      Serial.print(F("Invalid Entry. No Changes Made\n"));
  }

  Serial.print(F("\n"));

  return ack;

}

