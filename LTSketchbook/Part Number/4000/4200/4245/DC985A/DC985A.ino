/*!
Linear Technology DC985A Demonstration Board.
LTC4245: Hot Swap Controller with I2C Compatible Monitoring

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. Provide
   the board with an external power supply of 12 V.   Ensure all jumpers on the
   demo board are installed in their default positions from the factory. Refer to
   Demo Manual DC985A.

   This program has options to measure voltage at ADIN pin (input voltage), SOURCE
   VOLTAGE (output voltage), and SENSE CURRENT (current through sense resisitor).
   There are also options to read and manipulate CONTROL register, ALERT register,
   and FAULT register.

   Mass write option can be achieved using Device Address = 0xBE. Refer to datasheet
   LTC4245.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC4245

http://www.linear.com/product/LTC4245#demoboards


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
    @ingroup LTC4245
*/

// Headerfiles
#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include <Wire.h>
#include <SPI.h>
#include "LTC4245.h"

// Function Declarations
void print_title();
void print_prompt();
int8_t main_menu_1_continuous_mode();
int8_t main_menu_2_read_and_clear_faults();
int8_t main_menu_3_send_ARA();
int8_t main_menu_4_manage_alerts();
int8_t main_menu_5_settings();
int8_t main_menu_6_read_all_registers();

//! Initialize Linduino
void setup()
{
  char demo_name[] = "DC985";             // Demo Board Name stored in QuikEval EEPROM
  quikeval_I2C_init();                    // Enable the I2C port
  quikeval_I2C_connect();
  Serial.begin(115200);                   // Initialize the serial port to the PC
  print_title();
  discover_DC985A(demo_name);
  print_prompt();
}

//! Repeats Linduino loop
void loop()
{
  if (1)
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

      print_prompt();
    }
  }
}

//! Read the ID string from the EEPROM and determine if the correct board is connected.
//! Returns 1 if successful, 0 if not successful
uint8_t discover_DC985A(char *demo_name)
{
  Serial.print(F("\nChecking EEPROM contents..."));
  read_quikeval_id_string(&ui_buffer[0]);
  ui_buffer[48] = 0;
  Serial.println(ui_buffer);

  if (!strcmp(demo_board.name, demo_name))
  {
    Serial.print("Demo Board Name: ");
    Serial.println(demo_board.name);
    Serial.print("Product Name: ");
    Serial.println(demo_board.product_name);
    if (demo_board.option)
    {
      Serial.print("Demo Board Option: ");
      Serial.println(demo_board.option);
    }
    Serial.println(F("Demo board connected"));
    Serial.println(F("\n\n\t\t\t\tPress Enter to Continue..."));
    read_int();
    return 1;
  }
  else
  {
    Serial.print("Demo board ");
    Serial.print(demo_name);
    Serial.print(" not found, \nfound ");
    Serial.print(demo_board.name);
    Serial.println(" instead. \nConnect the correct demo board, then press the reset button.");
    return 0;
  }
}

//! Print the title block
void print_title()
{
  Serial.println(F("\n*****************************************************************"));
  Serial.print(F("* DC874A Demonstration Program                                  *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send and receive data from   *\n"));
  Serial.print(F("* LTC4245 Hot Swap Controller with I2C Compatible Monitoring    *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Print the main menu
void print_prompt()
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
  int8_t choice;
  int8_t ack = 0;
  float sense_voltage, vin_voltage, vout_voltage;
  uint8_t sense_code, vin_code, vout_code;
  // Set voltage divider ratio to 1, for no resistor divider circuit.
  float vin_full_scale_12V = 14.025;
  float vin_full_scale_5V = 5.61;
  float vin_full_scale_3_3V = 3.825;
  float vin_full_scale_NEG12V = -14.025;

  float sense_full_scale_12V = (63.75/1000);
  float sense_full_scale_5V = (31.875/1000);
  float sense_full_scale_3_3V = (31.875/1000);
  float sense_full_scale_NEG12V = (-63.75/1000);

  float vout_full_scale_12V = 14.025;
  float vout_full_scale_5V = 5.61;
  float vout_full_scale_3_3V = 3.825;
  float vout_full_scale_NEG12V = -14.025;

  float SENSE_RESISTOR_12V = 0.008;
  float SENSE_RESISTOR_5V = 0.0035;
  float SENSE_RESISTOR_3_3V = 0.0025;
  float SENSE_RESISTOR_NEG12V = 0.004;

  Serial.println(F("1. Read in continous mode"));
  Serial.print(F("2. Read in snapshot mode"));
  choice = read_int();

  if (choice == 1)
  {
    do
    {
      Serial.print(F("********** Press Enter to Exit ***********\n\n"));
      ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_12VIN_REG, &vin_code);
      ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_12VSENSE_REG, &sense_code);
      ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_12VOUT_REG, &vout_code);

      vin_voltage = LTC4245_code_to_voltage(vin_code, vin_full_scale_12V);
      sense_voltage = LTC4245_code_to_voltage(sense_code, sense_full_scale_12V);
      vout_voltage = LTC4245_code_to_voltage(vout_code, vout_full_scale_12V);

      Serial.print(F("VIN: "));
      Serial.println(vin_voltage);
      Serial.print(F("SENSE VOLTAGE CODE: "));
      Serial.println(sense_code);
      Serial.print(F("SENSE CURRENT CALCULATED: "));
      Serial.println((sense_voltage / SENSE_RESISTOR_12V), 4);
      Serial.print(F("VOUT: "));
      Serial.println(vout_voltage);
      Serial.print("\n");


      ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_5VIN_REG, &vin_code);
      ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_5VSENSE_REG, &sense_code);
      ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_5VOUT_REG, &vout_code);

      vin_voltage = LTC4245_code_to_voltage(vin_code, vin_full_scale_5V);
      sense_voltage = LTC4245_code_to_voltage(sense_code, sense_full_scale_5V);
      vout_voltage = LTC4245_code_to_voltage(vout_code, vout_full_scale_5V);

      Serial.print(F("VIN: "));
      Serial.println(vin_voltage);
      Serial.print(F("SENSE VOLTAGE CODE: "));
      Serial.println(sense_code);
      Serial.print(F("SENSE CURRENT CALCULATED: "));
      Serial.println((sense_voltage / SENSE_RESISTOR_5V), 4);
      Serial.print(F("VOUT: "));
      Serial.println(vout_voltage);
      Serial.print("\n");


      ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_3_3VIN_REG, &vin_code);
      ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_3_3VSENSE_REG, &sense_code);
      ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_3_3VOUT_REG, &vout_code);

      vin_voltage = LTC4245_code_to_voltage(vin_code, vin_full_scale_3_3V);
      sense_voltage = LTC4245_code_to_voltage(sense_code, sense_full_scale_3_3V);
      vout_voltage = LTC4245_code_to_voltage(vout_code, vout_full_scale_3_3V);

      Serial.print(F("VIN: "));
      Serial.println(vin_voltage);
      Serial.print(F("SENSE VOLTAGE CODE: "));
      Serial.println(sense_code);
      Serial.print(F("SENSE CURRENT CALCULATED: "));
      Serial.println((sense_voltage / SENSE_RESISTOR_3_3V), 4);
      Serial.print(F("VOUT: "));
      Serial.println(vout_voltage);
      Serial.print("\n");


      ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_VEEIN_REG, &vin_code);
      ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_VEESENSE_REG, &sense_code);
      ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_VEEOUT_REG, &vout_code);

      vin_voltage = LTC4245_code_to_voltage(vin_code, vin_full_scale_NEG12V);
      sense_voltage = LTC4245_code_to_voltage(sense_code, sense_full_scale_NEG12V);
      vout_voltage = LTC4245_code_to_voltage(vout_code, vout_full_scale_NEG12V);

      Serial.print(F("VIN: "));
      Serial.println(vin_voltage);
      Serial.print(F("SENSE VOLTAGE CODE: "));
      Serial.println(sense_code);
      Serial.print(F("SENSE CURRENT CALCULATED: "));
      Serial.println((sense_voltage / SENSE_RESISTOR_NEG12V), 4);
      Serial.print(F("VOUT: "));
      Serial.println(vout_voltage);
      Serial.print("\n");
      delay(2000);
    }
    while (Serial.available()==false && ack== 0);
    if (Serial.available())
      read_int(); // clears the Serial.available()
  }

  return (ack);
}


//! Function to read and clear fault register
int8_t main_menu_2_read_and_clear_faults()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;

  uint8_t faults;
  ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_FAULT1_REG, &faults);
  if (faults != 0)
  {
    Serial.println(F("Faults Detected :"));
    if (faults & LTC4245_NEG12V_OVERCURRENT_FAULT)
      Serial.println(F("  Overcurrent Fault Occurred on –12V Supply"));
    if (faults & LTC4245_3_3V_OVERCURRENT_FAULT)
      Serial.println(F("  Overcurrent Fault Occurred on 3.3V Supply"));
    if (faults & LTC4245_5V_OVERCURRENT_FAULT)
      Serial.println(F("  Overcurrent Fault Occurred on 5V Supply"));
    if (faults & LTC4245_12V_OVERCURRENT_FAULT)
      Serial.println(F("  Overcurrent Fault Occurred on 12V Supply"));
    if (faults & LTC4245_NEG12V_UNDERVOLTAGE_FAULT)
      Serial.println(F("  Undervoltage Fault Occurred on –12V Supply"));
    if (faults & LTC4245_3_3V_UNDERVOLTAGE_FAULT)
      Serial.println(F("  Undervoltage Fault Occurred on 3.3V Supply"));
    if (faults & LTC4245_5V_UNDERVOLTAGE_FAULT)
      Serial.println(F("  Undervoltage Fault Occurred on 5V Supply"));
    if (faults & LTC4245_12V_UNDERVOLTAGE_FAULT)
      Serial.println(F("  Undervoltage Fault Occurred on 12V Supply"));
  }
  faults = 0;
  ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_FAULT2_REG, &faults);
  if (faults != 0)
  {
    if (faults & LTC4245_GPIO1_STATE_CHANGE)
      Serial.println(F("  GPIO1 Pin Changed State"));
    if (faults & LTC4245_BD_SEL_STATE_CHANGE)
      Serial.println(F("  BD_SEL# Pin Changed State"));
    if (faults & LTC4245_FET_SHORT_FAULT)
      Serial.println(F("  FET Short Detected on at Least One Supply"));
    if (faults & LTC4245_PGI_FAULT)
      Serial.println(F("  PGI Fault Occurred"));
    if (faults & LTC4245_NEG12V_POWER_BAD_FAULT)
      Serial.println(F("   –12V Power Bad When VEEOUT Went High"));
    if (faults & LTC4245_3_3V_POWER_BAD_FAULT)
      Serial.println(F("  3.3V Power Bad When 3VOUT Went Low"));
    if (faults & LTC4245_5V_POWER_BAD_FAULT)
      Serial.println(F("  5V Power Bad When 5VOUT Went Low"));
    if (faults & LTC4245_12V_POWER_BAD_FAULT)
      Serial.println(F("  12V Power Bad When 12VOUT Went Low"));
  }
  ack |= LTC4245_write(LTC4245_I2C_ADDRESS, LTC4245_FAULT1_REG, 0x00);
  ack |= LTC4245_write(LTC4245_I2C_ADDRESS, LTC4245_FAULT2_REG, 0x00);
  Serial.print(F("\nALL FAULTS CLEARED \n\n"));

  return (ack);
}

//! Function to send alert response (0001100) and read back the address of device that pulled ALERT pin low.
int8_t main_menu_3_send_ARA()
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int8_t ack = 0;
  uint8_t address;
  ack |= LTC4245_ARA(LTC4245_I2C_ALERT_RESPONSE, &address);// send ARA
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
    Serial.println(F("GPIO1 STATE CHANGE ALERT"));
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
      alert_settings |= LTC4245_GPIO1_STATE_CHANGE_ENABLE;
    else if (user_command ==2)
      alert_settings &= LTC4245_GPIO1_STATE_CHANGE_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    Serial.println(F("BD_SEL# STATE CHANGE ALERT"));
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
      alert_settings |= LTC4245_BD_SEL_STATE_CHANGE_ENABLE;
    else if (user_command ==2)
      alert_settings &= LTC4245_BD_SEL_STATE_CHANGE_DISABLE;
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
      alert_settings |= LTC4245_FET_SHORT_ENABLE;
    else if (user_command ==2)
      alert_settings &= LTC4245_FET_SHORT_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    Serial.println(F("PGI FAULT ALERT"));
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
      alert_settings |= LTC4245_PGI_FAULT_ENABLE;
    else if (user_command ==2)
      alert_settings &= LTC4245_PGI_FAULT_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    Serial.println(F("ALERT# Pin Open-Drain Output State"));
    Serial.println(F("  1. Pulls Low"));
    Serial.println(F("  2. High Impedance"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      alert_settings |= LTC4245_ALERT_PRESENT_ENABLE;
    else if (user_command ==2)
      alert_settings &= LTC4245_ALERT_PRESENT_DISABLE;
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
      alert_settings |= LTC4245_POWER_BAD_ENABLE;
    else if (user_command ==2)
      alert_settings &= LTC4245_POWER_BAD_DISABLE;
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
      alert_settings |= LTC4245_OVERCURRENT_ENABLE;
    else if (user_command ==2)
      alert_settings &= LTC4245_OVERCURRENT_DISABLE;
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
      alert_settings |= LTC4245_UNDERVOLTAGE_ENABLE;
    else if (user_command ==2)
      alert_settings &= LTC4245_UNDERVOLTAGE_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    ack |= LTC4245_write(LTC4245_I2C_ADDRESS, LTC4245_ALERT_REG, alert_settings);
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
    Serial.println(F("ADC Free Running Operation to Allow On-Demand Measurement and Writes to ADC Registers"));
    Serial.println(F("  1. ADC Free Running"));
    Serial.println(F("  2. Halt ADC Free Running"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      settings |= LTC4245_ADC_FREE_RUN_ENABLE;
    else if (user_command ==2)
      settings &= LTC4245_ADC_FREE_RUN_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    Serial.println(F("Turn-On in a Set Sequence"));
    Serial.println(F("  1. Sequencing Enabled"));
    Serial.println(F("  2. Sequencing Disabled"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      settings |= LTC4245_SEQUENCING_ENABLE;
    else if (user_command ==2)
      settings &= LTC4245_SEQUENCING_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    Serial.println(F("MASS WRITE"));
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
      settings |= LTC4245_MASS_WRITE_ENABLE;
    else if (user_command ==2)
      settings &= LTC4245_MASS_WRITE_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    Serial.println(F("PGI Fault Autoretry"));
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
      settings |= LTC4245_PGI_FAULT_AUTO_RETRY_ENABLE;
    else if (user_command ==2)
      settings &= LTC4245_PGI_FAULT_AUTO_RETRY_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    Serial.println(F(" PGI Pin"));
    Serial.println(F("  1. PGI Pin Enabled"));
    Serial.println(F("  2. PGI Pin Disabled"));
    Serial.print(F("Enter a command: "));
    user_command = read_int();
    if (user_command == 'm')
      Serial.println("m");
    else
      Serial.println(user_command);
    Serial.println();
    if (user_command == 1)
      settings |= LTC4245_PGI_ENABLE;
    else if (user_command ==2)
      settings &= LTC4245_PGI_DISABLE;
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
      settings |= LTC4245_OVERCURRENT_AUTO_RETRY_ENABLE;
    else if (user_command ==2)
      settings &= LTC4245_OVERCURRENT_AUTO_RETRY_DISABLE;
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
      settings |= LTC4245_UNDERVOLTAGE_AUTO_RETRY_ENABLE;
    else if (user_command ==2)
      settings &= LTC4245_UNDERVOLTAGE_AUTO_RETRY_DISABLE;
    else if (user_command != 'm')
      Serial.println("Incorrect Option\n");
    else
      return(ack);

    ack |= LTC4245_write(LTC4245_I2C_ADDRESS, LTC4245_CONTROL_REG, settings);
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
  uint8_t register_val;
  ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_STATUS_REG, &register_val);
  Serial.print("  STATUS REGISTER   : 0b");
  Serial.println(register_val, BIN);
  ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_ALERT_REG, &register_val);
  Serial.print("  ALERT REGISTER    : 0b");
  Serial.println(register_val, BIN);
  ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_CONTROL_REG, &register_val);
  Serial.print("  CONTROL REGISTER  : 0b");
  Serial.println(register_val, BIN);
  ack |= LTC4245_read(LTC4245_I2C_ADDRESS, LTC4245_ON_REG, &register_val);
  Serial.print("  ON REGISTER       : 0b");
  Serial.println(register_val, BIN);
  Serial.println();
  return ack;
}