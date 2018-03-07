/*!
Linear Technology DC14968B-B Demonstration Board.
LTC2942: Battery Gas Gauge with Temperature, Voltage Measurement

Linear Technology DC14968B-A Demonstration Board.
LTC2941: Battery Gas Gauge with I2C Interface

@verbatim

  Setup:
     Apply a voltage source to the V_BATT terminal and a load at the V_CHRG/LD terminal.
     Refer to the Demo Manual Guide for a detailed setup description. Ensure that JP1 is
     set to AL# position.

  Explanation of Commands:

     1 - Automatic Mode - Scans Voltage, Temperature and charge every 60ms.
         Scanning interval has been increased to 1s for ease of reading on the Serial Prompt.
         Displays an alert if it is set in the status register at the time of scan.

     2 - Manual Voltage Mode - Provides a SnapShot of the Voltage, and Accumulated Charge.
         After the initial SnapShot, the part goes into sleep mode where it only counts charge.
         Displays an alert if it is set in the status register at the time of scan.

     3 - Manual Temperature Mode - Provides a SnapShot of the Temperature, and Accumulated Charge.
        After the initial SnapShot, the part goes into sleep mode where it only counts charge.
        Displays an alert if it is set in the status register at the time of scan.

     4 - Sleep Mode - The ADC portion of the LTC2942 is shutdown. In this mode only charge is accumulated.

     5 - Shutdown Mode - The LTC2942 goes into low power mode.

     6 - Settings - Allows user to set alert thresholds, set prescalar, set AL#/CC# pin mode and change the temperature and charge units.

NOTES
 Setup:
 Set the terminal baud rate to 115200 and select the newline terminator.
 Requires a power supply.
 Refer to demo manual DC1812A-A.
 For use with LTC2941 ignore calls to temperature and voltage as these are not present in LTC2941.
 All of the functionality of LTC2941 is supported except Control Register B[7:6]

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2942
http://www.linear.com/product/LTC2941

http://www.linear.com/product/LTC2942#demoboards
http://www.linear.com/product/LTC2941#demoboards


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
    @ingroup LTC2942
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC2942.h"
#include <Wire.h>


// Function Declaration
void print_title();                 // Print the title block
void print_prompt();                // Print the Prompt
void store_alert_settings();        // Store the alert settings to the EEPROM
int8_t restore_alert_settings();    // Read the alert settings from EEPROM


#define AUTOMATIC_MODE_DISPLAY_DELAY 1000                  //!< The delay between readings in automatic mode
#define SCAN_MODE_DISPLAY_DELAY 10000                      //!< The delay between readings in scan mode
const float resistor = .100;                               //!< resistor value on demo board

// Error string
const char ack_error[] = "Error: No Acknowledge. Check I2C Address."; //!< Error message



// Global variables
static int8_t demo_board_connected;        //!< Set to 1 if the board is connected
static uint8_t alert_code = 0;             //!< Value stored or read from ALERT register.  Shared between loop() and restore_alert_settings()

//! Initialize Linduino
void setup()
{
  char demo_name[] = "DC1496";      //! Demo Board Name stored in QuikEval EEPROM

  quikeval_I2C_init();              //! Configure the EEPROM I2C port for 100kHz
  quikeval_I2C_connect();           //! Connects to main I2C port
  Serial.begin(115200);             //! Initialize the serial port to the PC
  print_title();
  demo_board_connected = discover_demo_board(demo_name);
  if (demo_board_connected)
  {
    print_prompt();
  }
  else
  {
    demo_board_connected = true;
    Serial.println("Did not read ID String, attempting to proceed anyway...\nPlease ensure I2C lines of Linduino are connected to the LTC device");
    print_prompt();
  }


}


//! Repeats Linduino loop
void loop()
{
  int8_t ack = 0;                               //! I2C acknowledge indicator
  static uint8_t user_command;                  //! The user input command
  static uint8_t mAh_or_Coulombs = 0;
  static uint8_t celcius_or_kelvin = 0;
  static uint16_t prescalar_mode = LTC2942_PRESCALAR_M_128;
  static uint16_t prescalarValue = 128;
  static uint16_t alcc_mode = LTC2942_ALERT_MODE;

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
          ack |= menu_1_automatic_mode(mAh_or_Coulombs, celcius_or_kelvin, prescalar_mode, prescalarValue, alcc_mode);  //! Automatic Mode
          break;
        case 2:
          ack |= menu_2_manual_voltage_mode(mAh_or_Coulombs, celcius_or_kelvin, prescalar_mode, prescalarValue, alcc_mode);      //! Manual Voltage Mode
          break;
        case 3:
          ack |= menu_3_manual_temperature_mode(mAh_or_Coulombs, celcius_or_kelvin, prescalar_mode, prescalarValue, alcc_mode);    //! Manual Temperature Mode
          break;
        case 4:
          ack |= menu_4_sleep_mode(mAh_or_Coulombs, prescalar_mode, prescalarValue, alcc_mode);                        //! Sleep Mode
          break;
        case 5:
          ack |= menu_5_shutdown_mode();                                                                                //! Shutdown Mode
          break;
        case 6:
          ack |= menu_6_settings(&mAh_or_Coulombs, &celcius_or_kelvin, &prescalar_mode, &prescalarValue, &alcc_mode);  //! Settings Mode
          break;
      }
      if (ack != 0)                                                       //! If ack is not recieved print an error.
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
  Serial.print(F("* DC1496B-B Demonstration Program                               *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program communicates with the LTC2942 Coulomb Counter    *\n"));
  Serial.print(F("* found on the DC1496B-B demo board.                            *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Print the Prompt
void print_prompt()
{
  Serial.print(F("\n1-Automatic Mode\n"));
  Serial.print(F("2-Manual Voltage Mode\n"));
  Serial.print(F("3-Manual Temperature Mode\n"));
  Serial.print(F("4-Sleep Mode\n"));
  Serial.print(F("5-Shutdown Mode\n"));
  Serial.print(F("6-Settings\n"));
  Serial.print(F("Enter a command: "));
}

//! Automatic Mode.
int8_t menu_1_automatic_mode(int8_t mAh_or_Coulombs, int8_t celcius_or_kelvin ,uint16_t prescalar_mode, uint16_t prescalarValue, uint16_t alcc_mode)
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t LTC2942_mode;
  int8_t ack = 0;
  LTC2942_mode = LTC2942_AUTOMATIC_MODE|prescalar_mode|alcc_mode ;               //! Set the control register of the LTC2942 to automatic mode as well as set prescalar and AL#/CC# pin values.
  Serial.println();
  ack |= LTC2942_write(LTC2942_I2C_ADDRESS, LTC2942_CONTROL_REG, LTC2942_mode);   //! Writes the set mode to the LTC2942 control register
  if (ack)
  {
    Serial.println(F("could not configure LTC2942"));
    return(ack);
  }
  do
  {
    Serial.print(F("*************************\n\n"));

    uint8_t status_code, hightemp_code, lowtemp_code;
    uint16_t charge_code, current_code, voltage_code, temperature_code;

    ack |= LTC2942_read_16_bits(LTC2942_I2C_ADDRESS, LTC2942_ACCUM_CHARGE_MSB_REG, &charge_code);     //! Read MSB and LSB Accumulated Charge Registers for 16 bit charge code
    ack |= LTC2942_read_16_bits(LTC2942_I2C_ADDRESS, LTC2942_VOLTAGE_MSB_REG, &voltage_code);         //! Read MSB and LSB Voltage Registers for 16 bit voltage code
    ack |= LTC2942_read_16_bits(LTC2942_I2C_ADDRESS, LTC2942_TEMPERATURE_MSB_REG, &temperature_code); //! Read MSB and LSB Temperature Registers for 16 bit temperature code
    ack |= LTC2942_read(LTC2942_I2C_ADDRESS, LTC2942_STATUS_REG, &status_code);                       //! Read Status Register for 8 bit status code

    Serial.print(F("CHARGE: "));
    Serial.println(charge_code,HEX);
    Serial.print(F("VOLTAGE: "));
    Serial.println(voltage_code, HEX);
    Serial.print(F("TEMPERATURE: "));
    Serial.println(temperature_code,HEX);

    float charge, voltage, temperature;
    if (mAh_or_Coulombs)
    {
      charge = LTC2942_code_to_coulombs(charge_code, resistor, prescalarValue); //! Convert charge code to Coulombs if Coulomb units are desired.
      Serial.print("Coulombs: ");
      Serial.print(charge, 4);
      Serial.print(F(" C\n"));
    }
    else
    {
      charge = LTC2942_code_to_mAh(charge_code, resistor, prescalarValue);      //! Convert charge code to mAh if mAh units are desired.
      Serial.print("mAh: ");
      Serial.print(charge, 4);
      Serial.print(F(" mAh\n"));
    }


    voltage = LTC2942_code_to_voltage(voltage_code);                          //! Convert voltage code to Volts


    Serial.print(F("Voltage "));
    Serial.print(voltage, 4);
    Serial.print(F(" V\n"));


    if (celcius_or_kelvin)
    {
      temperature = LTC2942_code_to_kelvin_temperature(temperature_code);   //! Convert temperature code to kelvin
      Serial.print(F("Temperature "));
      Serial.print(temperature, 4);
      Serial.print(F(" K\n"));
    }
    else
    {
      temperature = LTC2942_code_to_celcius_temperature(temperature_code);  //! Convert temperature code to celcius
      Serial.print(F("Temperature "));
      Serial.print(temperature, 4);
      Serial.print(F(" C\n"));
    }

    checkAlerts(status_code);                                                   //! Check status code for Alerts. If an Alert has been set, print out appropriate message in the Serial Prompt.

    Serial.print(F("m-Main Menu\n\n"));

    Serial.flush();
    delay(AUTOMATIC_MODE_DISPLAY_DELAY);                                      //! Delay for 1s before next polling
  }
  while (!Serial.available() && !(ack));                                 //! if Serial is not available and an NACK has not been recieved, keep polling the registers.
  read_int();  // clears the Serial.available
  return(ack);
}


//! Manual Voltage Mode
int8_t menu_2_manual_voltage_mode(int8_t mAh_or_Coulombs ,int8_t celcius_or_kelvin ,uint16_t prescalar_mode, uint16_t prescalarValue, uint16_t alcc_mode)
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
{
  int8_t LTC2942_mode;
  int8_t ack = 0;
  LTC2942_mode = LTC2942_MANUAL_VOLTAGE|prescalar_mode|alcc_mode ;                                    //! Set the control mode of the LTC2942 to manual mode as well as set prescalar and AL#/CC# pin values.
  Serial.println();
  ack |= LTC2942_write(LTC2942_I2C_ADDRESS, LTC2942_CONTROL_REG, LTC2942_mode);                     //! Writes the set mode to the LTC2942 control register

  int staleData = 0;                                                                                //! Stale Data Check variable. When set to 1 it indicates that stale data is being read from the voltage, current and temperature registers.

  do
  {
    Serial.print(F("*************************\n\n"));

    uint8_t status_code;
    uint16_t charge_code, voltage_code;


    ack |= LTC2942_read_16_bits(LTC2942_I2C_ADDRESS, LTC2942_ACCUM_CHARGE_MSB_REG, &charge_code);         //! Read MSB and LSB Accumulated Charge Registers for 16 bit charge code
    ack |= LTC2942_read_16_bits(LTC2942_I2C_ADDRESS, LTC2942_VOLTAGE_MSB_REG, &voltage_code);             //! Read MSB and LSB Voltage Registers for 16 bit voltage code
    ack |= LTC2942_read(LTC2942_I2C_ADDRESS, LTC2942_STATUS_REG, &status_code);                           //! Read Status Registers for 8 bit status code


    float charge, voltage, temperature;
    if (mAh_or_Coulombs)
    {
      charge = LTC2942_code_to_coulombs(charge_code, resistor, prescalarValue);                             //! Convert charge code to Coulombs if Coulomb units are desired.
      Serial.print("Coulombs: ");
      Serial.print(charge, 4);
      Serial.print(F(" C\n"));
    }
    else
    {
      charge = LTC2942_code_to_mAh(charge_code, resistor, prescalarValue);                                  //! Convert charge code to mAh if mAh units are desired.
      Serial.print("mAh: ");
      Serial.print(charge, 4);
      Serial.print(F(" mAh\n"));
    }


    voltage = LTC2942_code_to_voltage(voltage_code);                                                      //! Convert voltage code to Volts


    Serial.print(F("Voltage "));
    Serial.print(voltage, 4);
    Serial.print(F(" V"));
    if (staleData) Serial.print(F("     ***** Stale Data ******\n"));                                     //! If Stale data is inside the register after initial snapshot, Print Stale Data message.
    else Serial.println("");


    if (staleData) Serial.print(F("   ***** Stale Data ******\n"));
    else Serial.println("");


    checkAlerts(status_code);                                                                             //! Check status code for Alerts. If an Alert has been set, print out appropriate message in the Serial Prompt


    Serial.print(F("m-Main Menu\n\n"));

    staleData = 1;
    Serial.flush();
    delay(AUTOMATIC_MODE_DISPLAY_DELAY);
  }
  while (!Serial.available() && !(ack));
  read_int();  // clears the Serial.available
  return(ack);
}

//! Manual Voltage Mode
int8_t menu_3_manual_temperature_mode(int8_t mAh_or_Coulombs ,int8_t celcius_or_kelvin ,uint16_t prescalar_mode, uint16_t prescalarValue, uint16_t alcc_mode)
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
{
  int8_t LTC2942_mode;
  int8_t ack = 0;
  LTC2942_mode = LTC2942_MANUAL_TEMPERATURE|prescalar_mode|alcc_mode ;                                    //! Set the control mode of the LTC2942 to manual mode as well as set prescalar and AL#/CC# pin values.
  Serial.println();
  ack |= LTC2942_write(LTC2942_I2C_ADDRESS, LTC2942_CONTROL_REG, LTC2942_mode);                     //! Writes the set mode to the LTC2942 control register

  int staleData = 0;                                                                                //! Stale Data Check variable. When set to 1 it indicates that stale data is being read from the voltage, current and temperature registers.

  do
  {
    Serial.print(F("*************************\n\n"));

    uint8_t status_code;
    uint16_t charge_code, temperature_code;


    ack |= LTC2942_read_16_bits(LTC2942_I2C_ADDRESS, LTC2942_ACCUM_CHARGE_MSB_REG, &charge_code);         //! Read MSB and LSB Accumulated Charge Registers for 16 bit charge code
    ack |= LTC2942_read_16_bits(LTC2942_I2C_ADDRESS, LTC2942_TEMPERATURE_MSB_REG, &temperature_code);     //! Read MSB and LSB Temperature Registers for 16 bit temperature code
    ack |= LTC2942_read(LTC2942_I2C_ADDRESS, LTC2942_STATUS_REG, &status_code);                           //! Read Status Registers for 8 bit status code


    float charge,  temperature;
    if (mAh_or_Coulombs)
    {
      charge = LTC2942_code_to_coulombs(charge_code, resistor, prescalarValue);                             //! Convert charge code to Coulombs if Coulomb units are desired.
      Serial.print("Coulombs: ");
      Serial.print(charge, 4);
      Serial.print(F(" C\n"));
    }
    else
    {
      charge = LTC2942_code_to_mAh(charge_code, resistor, prescalarValue);                                  //! Convert charge code to mAh if mAh units are desired.
      Serial.print("mAh: ");
      Serial.print(charge, 4);
      Serial.print(F(" mAh\n"));
    }




    if (celcius_or_kelvin)
    {
      temperature = LTC2942_code_to_kelvin_temperature(temperature_code);                               //! Convert temperature code to Kelvin if Kelvin units are desired.
      Serial.print(F("Temperature "));
      Serial.print(temperature, 4);
      Serial.print(F(" K"));
    }
    else
    {
      temperature = LTC2942_code_to_celcius_temperature(temperature_code);                              //! Convert temperature code to Celcius if Celcius units are desired.
      Serial.print(F("Temperature "));
      Serial.print(temperature, 4);
      Serial.print(F(" C"));
    }
    if (staleData) Serial.print(F("   ***** Stale Data ******\n"));
    else Serial.println("");


    checkAlerts(status_code);                                                                             //! Check status code for Alerts. If an Alert has been set, print out appropriate message in the Serial Prompt


    Serial.print(F("m-Main Menu\n\n"));

    staleData = 1;
    Serial.flush();
    delay(AUTOMATIC_MODE_DISPLAY_DELAY);
  }
  while (!Serial.available() && !(ack));
  read_int();  // clears the Serial.available
  return(ack);
}

//! Sleep Mode
int8_t menu_4_sleep_mode(int8_t mAh_or_Coulombs ,uint16_t prescalar_mode, uint16_t prescalarValue, uint16_t alcc_mode)
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
{
  int8_t LTC2942_mode;
  int8_t ack = 0;
  LTC2942_mode = LTC2942_SLEEP_MODE|prescalar_mode|alcc_mode ;                            //! Set the control mode of the LTC2942 to sleep mode as well as set prescalar and AL#/CC# pin values.
  Serial.println();
  ack |= LTC2942_write(LTC2942_I2C_ADDRESS, LTC2942_CONTROL_REG, LTC2942_mode);            //! Writes the set mode to the LTC2942 control register


  do
  {
    Serial.print(F("*************************\n\n"));
    delay(100);
    uint8_t status_code;
    uint16_t charge_code;


    ack |= LTC2942_read_16_bits(LTC2942_I2C_ADDRESS, LTC2942_ACCUM_CHARGE_MSB_REG, &charge_code);     //! Read MSB and LSB Accumulated Charge Registers for 16 bit charge code
    ack |= LTC2942_read(LTC2942_I2C_ADDRESS, LTC2942_STATUS_REG, &status_code);                       //! Read Status Registers for 8 bit status code


    float charge;
    if (mAh_or_Coulombs)
    {
      charge = LTC2942_code_to_coulombs(charge_code, resistor, prescalarValue);                         //! Convert charge code to Coulombs if Coulomb units are desired.
      Serial.print("Coulombs: ");
      Serial.print(charge, 4);
      Serial.print(F(" C\n"));
    }
    else
    {
      charge = LTC2942_code_to_mAh(charge_code, resistor, prescalarValue);                              //! Convert charge code to mAh if mAh units are desired.
      Serial.print("mAh: ");
      Serial.print(charge, 4);
      Serial.print(F(" mAh\n"));
    }


    Serial.print(F("Voltage "));
    Serial.print(F("     ADC Sleep...\n"));

    Serial.print(F("Temperature "));
    Serial.print(F(" ADC Sleep...\n"));

    Serial.print(F("m-Main Menu\n\n"));

    checkAlerts(status_code);

    Serial.flush();
    delay(AUTOMATIC_MODE_DISPLAY_DELAY);
  }
  while (!Serial.available() && !(ack));
  read_int();  // clears the Serial.available
  return(ack);
}

//! Shutdown Mode
int8_t menu_5_shutdown_mode()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
{
  int8_t ack = 0;
  ack |= LTC2942_write(LTC2942_I2C_ADDRESS, LTC2942_CONTROL_REG, LTC2942_SHUTDOWN_MODE);                      //! Sets the LTC2942 into shutdown mode
  Serial.print("LTC2942 Has Been ShutDown\n");
  return(ack);
}
//! Settings Menu
int8_t menu_6_settings(uint8_t *mAh_or_Coulombs, uint8_t *celcius_or_kelvin, uint16_t *prescalar_mode, uint16_t *prescalarValue, uint16_t *alcc_mode)
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
{
  int8_t ack = 0;
  int8_t user_command;
  do
  {
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Set Alert Thresholds\n"));
    Serial.print(F("2-Set Prescalar Value\n"));
    Serial.print(F("3-Set AL#/CC# Pin State\n"));
    Serial.print(F("4-Set Units\n"));
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
        ack |= menu_6_settings_menu_1_set_alert_thresholds();                                          //! Settings Menu to set Alert Thresholds
        break;

      case 2:
        ack |= menu_6_settings_menu_2_set_prescalar_values(prescalar_mode, prescalarValue);            //! Settings Menu to set Prescalar Values
        break;

      case 3:
        ack |= menu_6_alert_menu_3_set_allcc_state(alcc_mode);                                        //! Settings Menu to set AL#/CC# mode
        break;

      case 4:
        ack |= menu_6_alert_menu_4_set_units(mAh_or_Coulombs, celcius_or_kelvin);                     //! Settings Menu to set Temperature and Charge Units
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

//! Alert Threshold Menu
int8_t menu_6_settings_menu_1_set_alert_thresholds()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
{
  int8_t ack = 0;
  int8_t user_command;
  do
  {
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Set Charge Thresholds\n"));
    Serial.print(F("2-Set Voltage Thresholds\n"));
    Serial.print(F("3-Set Temperature Thresholds\n"));
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
        ack |= menu_6_alert_menu_1_set_charge_thresholds();                   //! Set Max and Min Charge Thresholds. The ACR charge lsb size changes with respect to the prescalar and sense resistor value. Due to this variability, for the purpose of this demo enter values in hexadecimal.
        break;
      case 2:
        ack |= menu_6_alert_menu_2_set_voltage_thresholds();                  //! Set Max and Min Voltage Thresholds. Enter Values in Volts
        break;
      case 3:
        ack |= menu_6_alert_menu_3_set_temperature_thresholds();              //! Set Max and Min Temperature Thresholds. Enter Values in Celcius.
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
//! Set Charge Threshold Function
int8_t menu_6_alert_menu_1_set_charge_thresholds()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
{
  int8_t ack = 0;
  Serial.print(F("Enter RAW Max Charge Threshold:"));

  uint16_t max_charge_threshold;
  max_charge_threshold = read_int();                                                                              //! Read user entered value
  Serial.println(max_charge_threshold);

  ack |= LTC2942_write_16_bits(LTC2942_I2C_ADDRESS, LTC2942_CHARGE_THRESH_HIGH_MSB_REG, max_charge_threshold);    //! write user entered value to HIGH threshold register

  Serial.print(F("Enter RAW Min Charge Threshold:"));

  float min_charge_threshold;
  min_charge_threshold = read_int();                                                                              //! Read user entered value
  Serial.println(min_charge_threshold);

  ack |= LTC2942_write_16_bits(LTC2942_I2C_ADDRESS, LTC2942_CHARGE_THRESH_LOW_MSB_REG, min_charge_threshold);     //! write user entered value to HIGH threshold register
  return(ack);

}

//! Set Voltage Thresholds
int8_t menu_6_alert_menu_2_set_voltage_thresholds()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
{
  int8_t ack = 0;
  Serial.print(F("Enter Max Voltage Threshold:"));

  float max_voltage_threshold;
  max_voltage_threshold = read_float();                                                                                    //! Read user entered value
  Serial.print(max_voltage_threshold, 3);
  Serial.println("V");

  uint16_t max_voltage_threshold_code_16 =max_voltage_threshold*(0xFFFF)/(LTC2942_FULLSCALE_VOLTAGE);
  uint8_t max_voltage_threshold_code = max_voltage_threshold_code_16>>8;                                           //! Convert user entered voltage into adc code.

  ack |= LTC2942_write(LTC2942_I2C_ADDRESS, LTC2942_VOLTAGE_THRESH_HIGH_REG, max_voltage_threshold_code);       //! Write adc code to HIGH threshold register

  Serial.print(F("Enter Min Voltage Threshold:"));

  float min_voltage_threshold;
  min_voltage_threshold = read_float();                                                                                     //! Read user entered value
  Serial.println(min_voltage_threshold, 3);
  Serial.println("V");

  uint16_t min_voltage_threshold_code_16 =(min_voltage_threshold*(0xFFFF)/(LTC2942_FULLSCALE_VOLTAGE));
  uint8_t min_voltage_threshold_code = min_voltage_threshold_code_16>>8;                         //! Convert user entered voltage into adc code.

  ack |= LTC2942_write(LTC2942_I2C_ADDRESS, LTC2942_VOLTAGE_THRESH_LOW_REG, min_voltage_threshold_code);        //! Write adc code to LOW threshold register
  return(ack);
}

//! Set Temperature Thresholds
int8_t menu_6_alert_menu_3_set_temperature_thresholds()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
{
  int8_t ack = 0;
  Serial.print(F("Enter Max Temperature Threshold in Celcius:"));

  float max_temperature_threshold;
  max_temperature_threshold = read_float();                                                                                  //! Read user entered value
  Serial.print(max_temperature_threshold, 2);
  Serial.println("C");

  uint16_t max_temperature_threshold_code_16 =((max_temperature_threshold + 273.15)*(0xFFFF)/(LTC2942_FULLSCALE_TEMPERATURE));
  uint8_t max_temperature_threshold_code = max_temperature_threshold_code_16>>8;   //! Convert user entered temperature into adc code.


  ack |= LTC2942_write(LTC2942_I2C_ADDRESS, LTC2942_TEMPERATURE_THRESH_HIGH_REG, max_temperature_threshold_code);    //! Write adc code to HIGH threshold register

  Serial.print(F("Enter Min Temperature Threshold in Celcius:"));

  float min_temperature_threshold;
  min_temperature_threshold = read_float();                                                                                 //! Read user entered value
  Serial.print(min_temperature_threshold, 2);
  Serial.println("C");
  uint16_t min_temperature_threshold_code_16 = ((min_temperature_threshold + 273.15)*(0xFFFF)/(LTC2942_FULLSCALE_TEMPERATURE));
  uint8_t min_temperature_threshold_code = min_temperature_threshold_code_16>>8;  //! Convert user entered temperature into adc code.

  ack |= LTC2942_write_16_bits(LTC2942_I2C_ADDRESS, LTC2942_TEMPERATURE_THRESH_LOW_REG, min_temperature_threshold_code);    //! Write adc code to LOW threshold register
  return(ack);

}

//! Prescalar Menu
int8_t menu_6_settings_menu_2_set_prescalar_values(uint16_t *prescalar_mode, uint16_t *prescalarValue)
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
{
  int8_t ack = 0;
  int8_t user_command;
  do
  {
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Set Prescalar M = 1\n"));
    Serial.print(F("2-Set Prescalar M = 2\n"));
    Serial.print(F("3-Set Prescalar M = 4\n"));
    Serial.print(F("4-Set Prescalar M = 8\n"));
    Serial.print(F("5-Set Prescalar M = 16\n"));
    Serial.print(F("6-Set Prescalar M = 32\n"));
    Serial.print(F("7-Set Prescalar M = 64\n"));
    Serial.print(F("8-Set Prescalar M = 128\n"));
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
        *prescalar_mode = LTC2942_PRESCALAR_M_1;                                   //! Set Prescalar Value M = 1
        *prescalarValue = 1;
        Serial.println(F("\nPrescalar Set to 1\n"));
        break;
      case 2:
        *prescalar_mode = LTC2942_PRESCALAR_M_2;                                  //! Set Prescalar Value M = 2
        *prescalarValue = 2;
        Serial.println(F("\nPrescalar Set to 2\n"));
        break;
      case 3:
        *prescalar_mode = LTC2942_PRESCALAR_M_4;                                 //! Set Prescalar Value M = 4
        *prescalarValue = 4;
        Serial.println(F("\nPrescalar Set to 4\n"));
        break;
      case 4:
        *prescalar_mode = LTC2942_PRESCALAR_M_8;                                //! Set Prescalar Value M = 8
        *prescalarValue = 8;
        Serial.println(F("\nPrescalar Set to 8\n"));
        break;
      case 5:
        *prescalar_mode = LTC2942_PRESCALAR_M_16;                               //! Set Prescalar Value M = 16
        *prescalarValue = 16;
        Serial.println(F("\nPrescalar Set to 16\n"));
        break;
      case 6:
        *prescalar_mode = LTC2942_PRESCALAR_M_32;                              //! Set Prescalar Value M = 32
        *prescalarValue = 32;
        \
        Serial.println(F("\nPrescalar Set to 32\n"));
        break;
      case 7:
        *prescalar_mode = LTC2942_PRESCALAR_M_64;                              //! Set Prescalar Value M = 64
        *prescalarValue = 64;
        Serial.println(F("\nPrescalar Set to 64\n"));
        break;
      case 8:
        *prescalar_mode = LTC2942_PRESCALAR_M_128;                             //! Set Prescalar Value M = 128
        *prescalarValue =128;
        Serial.println(F("\nPrescalar Set to 128\n"));
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }
  }
  while (!((user_command == 'm') || (ack)));
  return(ack);
}

//! AL#/CC# Pin Mode Menu
uint8_t menu_6_alert_menu_3_set_allcc_state(uint16_t *alcc_mode)
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
{
  int8_t ack = 0;
  int8_t user_command;
  do
  {
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Enable Alert Mode\n"));
    Serial.print(F("2-Enable Charge Complete Mode\n"));
    Serial.print(F("3-Disable AL#/CC# Pin\n"));
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
        *alcc_mode = LTC2942_ALERT_MODE;                         //! Set AL#/CC# mode to Alert Mode
        Serial.println(F("\nAlert Mode Enabled\n"));
        break;
      case 2:
        *alcc_mode = LTC2942_CHARGE_COMPLETE_MODE;               //! Set AL#/CC# mode to Charge Complete Mode
        Serial.println(F("\nCharge Mode Enabled\n"));
        break;
      case 3:
        *alcc_mode = LTC2942_DISABLE_ALCC_PIN;                   //! Disable AL#/CC# pin.
        Serial.println(F("\nAL#/CC# Pin Disabled\n"));
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

//! Set Charge and Temperature Units Menu
uint8_t menu_6_alert_menu_4_set_units(uint8_t *mAh_or_Coulombs, uint8_t *celcius_or_kelvin)
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge
{
  int8_t ack = 0;
  int8_t user_command;
  do
  {
    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Set Charge Units to mAh\n"));
    Serial.print(F("2-Set Charge Units to Coulombs\n"));
    Serial.print(F("3-Set Temperature Units to Celsius\n"));
    Serial.print(F("4-Set Temperature Units to Kelvin\n"));
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
        *mAh_or_Coulombs = 0;
        Serial.println(F("\nCharge Units Set to mAh\n"));
        break;
      case 2:
        *mAh_or_Coulombs = 1;
        Serial.println(F("\nCharge Units Set to Coulombs\n"));
        break;
      case 3:
        *celcius_or_kelvin = 0;
        Serial.println(F("\nTemperature Units Set to Celcius\n"));
        break;
      case 4:
        *celcius_or_kelvin = 1;
        Serial.println(F("\nTemperature Units Set to Kelvin\n"));
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

//! Checks to see if a bit in a certain position is set.
bool isBitSet(uint8_t value, uint8_t position)
//! @return Returns the state of a bit at "position" in a byte. 1 = Set, 0 = Not Set
{
  return((1<<position)&value);
}
//! Check Alerts Function - Checks to see if an alert has been set in the status register. If an alert has been set, it prints out the appropriate message.
void checkAlerts(uint8_t status_code)
//! @return
{

  if (isBitSet(status_code,5))
  {
    Serial.print(F("\n***********************\n"));
    Serial.print(F("Alert: "));
    Serial.print(F("Charge Over/Under Flow Alert\n"));
    Serial.print(F("***********************\n"));
  }
  if (isBitSet(status_code,4))
  {
    Serial.print(F("\n***********************\n"));
    Serial.print(F("Alert: "));
    Serial.print(F("Temperature Alert\n"));
    Serial.print(F("***********************\n"));
  }
  if (isBitSet(status_code,3))
  {
    Serial.print(F("\n***********************\n"));
    Serial.print(F("Alert: "));
    Serial.print(F("Charge High Alert\n"));
    Serial.print(F("***********************\n"));
  }
  if (isBitSet(status_code,2))
  {
    Serial.print(F("\n***********************\n"));
    Serial.print(F("Alert: "));
    Serial.print(F("Charge Low Alert\n"));
    Serial.print(F("***********************\n"));
  }
  if (isBitSet(status_code,1))
  {
    Serial.print(F("\n***********************\n"));
    Serial.print(F("Alert: "));
    Serial.print(F("Voltage Alert\n"));
    Serial.print(F("***********************\n"));
  }
  if (isBitSet(status_code,0))
  {
    Serial.print(F("\n***********************\n"));
    Serial.print(F("Alert: "));
    Serial.print(F("UVLO Alert\n"));
    Serial.print(F("***********************\n"));
  }
}

