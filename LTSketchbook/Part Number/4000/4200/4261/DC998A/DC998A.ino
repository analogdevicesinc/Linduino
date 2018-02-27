/*!
Linear Technology DC998A Demonstration Board.
LTC4261: Negative Voltage Hot Swap Controllers with ADC and I2C Monitoring.

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. Provide
   the board with an external power supply of -48 V. Ensure all jumpers on the
   demo board are installed in their default positions from the factory. Refer to
   Demo Manual DC998A.

   This program has options to measure voltage at ADIN pin (input voltage), SOURCE
   VOLTAGE (output voltage), and SENSE CURRENT (current through sense resisitor).
   There are also options to read and manipulate CONTROL register, ALERT register,
   and FAULT register.

   Mass write option can be achieved using Device Address = 0xBE. Refer to datasheet
   LTC4261.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC4261

http://www.linear.com/product/LTC4261#demoboards


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
    @ingroup LTC4261
*/

// Headerfiles
#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC4261.h"
#include <Wire.h>
#include <SPI.h>

// Function Declarations
void print_title();
void print_main_menu();
int8_t main_menu_1_continuous_mode();
int8_t main_menu_2_read_and_clear_faults();
int8_t main_menu_3_send_ARA();
int8_t main_menu_4_manage_alerts();
int8_t main_menu_5_settings();
int8_t main_menu_6_read_all_registers();

// Global Constants
const float resistor = .008;
const float current_lsb = 62.5E-06;
const float adin_lsb =  2.56/1023;    // Vref of ADC = 2.56V and 2^10 - 1 = 1023 (10 bit ADC.)
const float resistive_ratio = 432.2 / 10.2;
// Global Variables
static int8_t demo_board_connected; //!< Set to 1 if the board is connected
//static uint8_t alert_code = 0;      //!< Value stored or read from ALERT register.  Shared between loop() and restore_alert_settings()
const char ack_error[] = "Error: No Acknowledge. Check I2C Address."; //!< Error message

//! Initialize Linduino
void setup()
{
  char demo_name[] = "DC998A";      // Demo Board Name stored in QuikEval EEPROM
  quikeval_I2C_init();              //! Configure the EEPROM I2C port for 100kHz
  quikeval_I2C_connect();           //! Connects to main I2C port
  Serial.begin(115200);             //! Initialize the serial port to the PC
  print_title();
  demo_board_connected = discover_demo_board(demo_name);
  print_main_menu();
}

//! Repeats Linduino loop
void loop()
{
  if (demo_board_connected)
  {
    int8_t ack = 0;                            // I2C acknowledge indicator
    static uint8_t user_command;               // The user input command
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
          ack |= main_menu_1_continuous_mode();       // continous mode
          break;
        case 2:
          ack |= main_menu_2_read_and_clear_faults(); // read and clear faults
          break;
        case 3:
          ack |= main_menu_3_send_ARA();// read address of alert
          break;
        case 4:
          ack |= main_menu_4_manage_alerts();         // manage alerts
          break;
        case 5:
          ack |= main_menu_5_settings();              // settings
          break;
        case 6:
          ack |= main_menu_6_read_all_registers();
          break;
        default:
          Serial.println("Incorrect Option");
          break;
      }
      if (ack != 0)
        Serial.println(ack_error);
      Serial.print(F("*****************************************"));
      print_main_menu();
    }
  }
}

//! Print the title block
void print_title()
{
  Serial.println(F("\n*****************************************************************"));
  Serial.print(F("* DC998A Demonstration Program                                  *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send and receive data from   *\n"));
  Serial.print(F("* the Negative Voltage Hot Swap Controllers                     *\n"));
  Serial.print(F("* with ADC and I2C Monitoring                                   *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Print the main menu
void print_main_menu()
{
  Serial.print(F("\n\nMain Menu\n"));
  Serial.print(F("  1. Read current and voltage on continous mode\n"));
  Serial.print(F("  2. Read and clear faults\n"));
  Serial.print(F("  3. Request for alert response address (ARA)\n"));
  Serial.print(F("  4. Manage alerts\n"));
  Serial.print(F("  5. Settings\n"));
  Serial.print(F("  6. Read all registers\n\n"));
  Serial.print(F("Enter a command: "));
}

//! Function to read Rsense current, ADIN voltage and ADIN2 voltage in continous mode
int8_t main_menu_1_continuous_mode()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  float current, adin_voltage, adin2_voltage;
  uint16_t current_code, adin_code, adin2_code;

  do
  {
    uint8_t faults;
    Serial.print(F("********** Press Enter to Exit ***********\n\n"));
    ack |= LTC4261_read(LTC4261_I2C_Address, LTC4261_FAULT_REG, &faults);
    if (faults != 0)
    {
      Serial.println(F("Faults Detected :"));
      if (faults & LTC4261_EXTERNAL_FAULT )
        Serial.println(F("  EXTERNAL FAULT DETECTED"));
      if (faults & LTC4261_PGIO_INPUT_HIGH )
        Serial.println(F("  PGIO INPUT HIGH DETECTED"));
      if (faults & LTC4261_FET_SHORT_FAULT )
        Serial.println(F("  FET SHORT DETECTED"));
      if (faults & LTC4261_EN_STATE_CHANGE)
        Serial.println(F("  !EN STATE CHANGE DETECTED"));
      if (faults & LTC4261_POWER_BAD_FAULT )
        Serial.println(F("  BAD POWER DETECTED"));
      if (faults & LTC4261_OVERCURRENT_FAULT )
        Serial.println(F("  OVERCURRENT DETECTED"));
      if (faults & LTC4261_UNDERVOLTAGE_FAULT )
        Serial.println(F("  UNDERVOLTAGE DETECTED"));
      if (faults & LTC4261_OVERVOLTAGE_FAULT )
        Serial.println(F("  OVERVOLTAGE DETECTED"));
    }
    else
      Serial.println(F("  NO FAULTS DETECTED"));

    ack |= LTC4261_read_10_bits(LTC4261_I2C_Address, LTC4261_SENSE_MSB_REG, &current_code);
    ack |= LTC4261_read_10_bits(LTC4261_I2C_Address, LTC4261_ADIN_MSB_REG, &adin_code);
    ack |= LTC4261_read_10_bits(LTC4261_I2C_Address, LTC4261_ADIN2_MSB_REG, &adin2_code);

    current = LTC4261_code_to_current(current_code, resistor, current_lsb);
    adin_voltage = LTC4261_ADIN_code_to_voltage(adin_code, adin_lsb, resistive_ratio);
    adin2_voltage = LTC4261_ADIN_code_to_voltage(adin2_code, adin_lsb, resistive_ratio);

    Serial.print(F("\nCURRENT CODE: "));
    Serial.println(current_code);
    Serial.print(F("CURRENT CALCULATED: "));
    Serial.println(current,4);
    Serial.print(F("\nADIN CODE: "));
    Serial.println(adin_code, HEX);
    Serial.print(F("VOLTAGE CALCULATED BY ADIN: "));
    Serial.println(adin_voltage,4);
    Serial.print(F("\nADIN2 CODE: "));
    Serial.println(adin2_code, HEX);
    Serial.print(F("VOLTAGE CALCULATED BY ADIN2: "));
    Serial.println(adin2_voltage,4);
    Serial.print("\n");
    delay(2000);
  }
  while (Serial.available()==false && ack== 0);
  if (Serial.available())
    read_int(); // clears the Serial.available()
  return (ack);
}

//! Function to read and clear fault register
int8_t main_menu_2_read_and_clear_faults()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;

  uint8_t faults;
  ack |= LTC4261_read(LTC4261_I2C_Address, LTC4261_FAULT_REG, &faults);
  if (faults != 0)
  {
    Serial.println(F("Faults Detected :"));
    if (faults & LTC4261_EXTERNAL_FAULT)
      Serial.println(F("  EXTERNAL FAULT DETECTED"));
    if (faults & LTC4261_PGIO_INPUT_HIGH)
      Serial.println(F("  PGIO INPUT HIGH DETECTED"));
    if (faults & LTC4261_FET_SHORT_FAULT)
      Serial.println(F("  FET SHORT DETECTED"));
    if (faults & LTC4261_EN_STATE_CHANGE)
      Serial.println(F("  !EN STATE CHANGE DETECTED"));
    if (faults & LTC4261_POWER_BAD_FAULT)
      Serial.println(F("  BAD POWER DETECTED"));
    if (faults & LTC4261_OVERCURRENT_FAULT)
      Serial.println(F("  OVERCURRENT DETECTED"));
    if (faults & LTC4261_UNDERVOLTAGE_FAULT)
      Serial.println(F("  UNDERVOLTAGE DETECTED"));
    if (faults & LTC4261_OVERVOLTAGE_FAULT)
      Serial.println(F("  OVERVOLTAGE DETECTED"));
  }
  ack |= LTC4261_write(LTC4261_I2C_Address, LTC4261_FAULT_REG, 0x00);
  Serial.print(F("  ALL FAULTS CLEARED \n\n"));

  return (ack);
}
//! Function to send alert response (0001100) and read back the address of device that pulled ALERT pin low.
int8_t main_menu_3_send_ARA()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  uint8_t address;
  ack |=LTC4261_ARA(LTC4261_I2C_ALERT_RESPONSE, &address);// send ARA
  Serial.print(F("  ALERT RESPONSE ADDRESS : 0x"));
  Serial.println(address,HEX);
  if (address == 0xFF && ack == 1)
  {
    ack = 0;
    Serial.print(F("  NO RESPONSE\n\n"));
  }
  return(ack);
}

//! Function to update alert register bits
int8_t main_menu_4_manage_alerts()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  uint8_t alert_settings = 0;
  int8_t user_command;
  do
  {
    Serial.println(F("EXTERNAL FAULT ALERT"));
    Serial.println(F("  1. Enable"));
    Serial.println(F("  2. Disable"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      alert_settings |= LTC4261_EXTERNAL_FAULT_ENABLE;
    else if (user_command ==2)
      alert_settings &= LTC4261_EXTERNAL_FAULT_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    Serial.println(F("PGIO OUTPUT"));
    Serial.println(F("  1. Enable"));
    Serial.println(F("  2. Disable"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      alert_settings |= LTC4261_PGIO_OUTPUT_ENABLE;
    else if (user_command ==2)
      alert_settings &= LTC4261_PGIO_OUTPUT_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    Serial.println(F("FET SHORT ALERT"));
    Serial.println(F("  1. Enable"));
    Serial.println(F("  2. Disable"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      alert_settings |= LTC4261_FET_SHORT_ENABLE;
    else if (user_command ==2)
      alert_settings &= LTC4261_FET_SHORT_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);


    Serial.println(F("!EN STATE CHANGE ALERT"));
    Serial.println(F("  1. Enable"));
    Serial.println(F("  2. Disable"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      alert_settings |= LTC4261_EN_STATE_ENABLE;
    else if (user_command ==2)
      alert_settings &= LTC4261_EN_STATE_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    Serial.println(F("POWER BAD ALERT"));
    Serial.println(F("  1. Enable"));
    Serial.println(F("  2. Disable"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      alert_settings |= LTC4261_POWER_BAD_ENABLE;
    else if (user_command ==2)
      alert_settings &= LTC4261_POWER_BAD_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    Serial.println(F("OVERCURRENT ALERT"));
    Serial.println(F("  1. Enable"));
    Serial.println(F("  2. Disable"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      alert_settings |= LTC4261_OVERCURRENT_ENABLE;
    else if (user_command ==2)
      alert_settings &= LTC4261_OVERCURRENT_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    Serial.println(F("UNDERVOLTAGE ALERT"));
    Serial.println(F("  1. Enable"));
    Serial.println(F("  2. Disable"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      alert_settings |= LTC4261_UNDERVOLTAGE_ENABLE;
    else if (user_command ==2)
      alert_settings &= LTC4261_UNDERVOLTAGE_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    Serial.println(F("OVERVOLTAGE ALERT"));
    Serial.println(F("  1. Enable"));
    Serial.println(F("  2. Disable"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      alert_settings |= LTC4261_OVERVOLTAGE_ENABLE;
    else if (user_command ==2)
      alert_settings &= LTC4261_OVERVOLTAGE_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);
    ack |= LTC4261_write(LTC4261_I2C_Address, LTC4261_ALERT_REG, alert_settings);
    Serial.print(F("\n  ALERTS UPDATED\n\n"));
    Serial.println(F("  m. Main Menu"));
    Serial.println(F("  1. Repeat"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
  }
  while (!((user_command == 'm') || (ack)));
  return(ack);

}

//! Function to update control register bits
int8_t main_menu_5_settings()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  uint8_t settings =0;
  int8_t user_command;
  do
  {
    Serial.println(F("PGIO"));
    Serial.println(F("  1. !POWER GOOD"));
    Serial.println(F("  2. POWER GOOD"));
    Serial.println(F("  3. General Purpose Output"));
    Serial.println(F("  4. General Purpose Input"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
    {
      Serial.println("m");
      return(ack);
    }
    else
      Serial.println(user_command);
    Serial.println();
    switch (user_command)
    {
      case 1:
        settings |= LTC4261_PGIO_POWER_GOODX;
        break;
      case 2:
        settings |= LTC4261_PGIO_POWER_GOOD;
        break;
      case 3:
        settings |= LTC4261_PGIO_GENERAL_PURPOSE_OUTPUT;
        break;
      case 4:
        settings |= LTC4261_PGIO_GENERAL_PURPOSE_INPUT;
        break;
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option\n");
        break;
    }

    Serial.println(F("TEST MODE ENABLE"));
    Serial.println(F("  1. Enable"));
    Serial.println(F("  2. Disable"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      settings |= LTC4261_TEST_MODE_ENABLE;
    else if (user_command ==2)
      settings &= LTC4261_TEST_MODE_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    Serial.println(F("POWER BAD AUTO RETRY"));
    Serial.println(F("  1. Enable"));
    Serial.println(F("  2. Disable"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      settings |= LTC4261_POWER_BAD_AUTO_RETRY_ENABLE;
    else if (user_command ==2)
      settings &= LTC4261_POWER_BAD_AUTO_RETRY_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);


    Serial.println(F("FET STATUS"));
    Serial.println(F("  1. ON"));
    Serial.println(F("  2. OFF"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      settings |= LTC4261_FET_ON;
    else if (user_command ==2)
      settings &= LTC4261_FET_OFF;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    Serial.println(F("OVERCURRENT AUTO RETRY"));
    Serial.println(F("  1. Enable"));
    Serial.println(F("  2. Disable"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      settings |= LTC4261_OVERCURRENT_AUTO_RETRY_ENABLE;
    else if (user_command ==2)
      settings &= LTC4261_OVERCURRENT_AUTO_RETRY_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    Serial.println(F("UNDERVOLTAGE AUTO RETRY"));
    Serial.println(F("  1. Enable"));
    Serial.println(F("  2. Disable"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      settings |= LTC4261_UNDERVOLTAGE_AUTO_RETRY_ENABLE;
    else if (user_command ==2)
      settings &= LTC4261_UNDERVOLTAGE_AUTO_RETRY_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);


    Serial.println(F("OVERVOLTAGE AUTO RETRY"));
    Serial.println(F("  1. Enable"));
    Serial.println(F("  2. Disable"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      settings |= LTC4261_OVERVOLTAGE_AUTO_RETRY_ENABLE;
    else if (user_command ==2)
      settings &= LTC4261_OVERVOLTAGE_AUTO_RETRY_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);
    ack |= LTC4261_write(LTC4261_I2C_Address, LTC4261_CONTROL_REG, settings);
    Serial.print(F("\nSETTINGS UPDATED\n\n"));
    Serial.println(F("  m. Main Menu"));
    Serial.println(F("  1. Repeat"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
  }
  while (!((user_command == 'm') || (ack)));
  return(ack);
}

//! Function to read all registers.
int8_t main_menu_6_read_all_registers()
//! @return Returns the state of the acknowledge bit after the I2C address read. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  uint8_t faults;
  ack |= LTC4261_read(LTC4261_I2C_Address, LTC4261_FAULT_REG, &faults);
  Serial.print("  FAULT REGISTER : 0b");
  Serial.println(faults, BIN);
  ack |= LTC4261_read(LTC4261_I2C_Address, LTC4261_STATUS_REG, &faults);
  Serial.print("  STATUS REGISTER : 0b");
  Serial.println(faults, BIN);
  ack |= LTC4261_read(LTC4261_I2C_Address, LTC4261_ALERT_REG, &faults);
  Serial.print("  ALERT REGISTER : 0b");
  Serial.println(faults, BIN);
  ack |= LTC4261_read(LTC4261_I2C_Address, LTC4261_CONTROL_REG, &faults);
  Serial.print("  CONTROL REGISTER : 0b");
  Serial.println(faults, BIN);
  Serial.println();
  return ack;
}