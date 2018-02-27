/*!
DC1208A
High Voltage I2C Current and Voltage Monitor

@verbatim

  Setup:
    Set the terminal baud rate to 115200 and select the newline terminator. Refer
    to Demo Manual DC1208A. Ensure all jumpers are installed in the factory default
    positions. A power supply and load resistor are required. A DVM may be used to
    verify displayed voltage results. A precision voltage source (preferably
    low-noise) may be used to apply a voltage to the ADIN pin. For all tests,
    connect power supply between VIN and GND, 7V-75V. Connect a load resistor to
    Vout and GND.

  Command Description:

                             *****Main Menu*****

    1- Read Continuous Mode- If selected, program keeps taking readings every second
       or so. In this mode, the program displays Load Current, VIN Voltage and ADIN
       voltage.

    2- Snapshot Mode- Selecting this option causes display of the Snapshot Mode
       Menu. Readings are taken just once for each Snapshot Mode menu entry.

                    ***** SNAPSHOT MODE MENU COMMANDS *****

      1- Load Current- Selecting this option causes load current to be displayed.

      2- V_IN Voltage- Selecting this option causes VIN voltage to be displayed.

      3- ADIN Voltage- Selecting this option causes the voltage at the ADIN pin to
         be displayed.

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
    @ingroup LTC4151
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC4151.h"
#include <Wire.h>
#include <SPI.h>

// Function Declaration
void print_title();                                                // Print the title block
void print_prompt();                                               // Print the main menu
int8_t LTC4151_snapshot(uint8_t i2c_address, uint8_t channel);     // Build and send the snapshot command

int8_t menu_1_continuous_mode();                                   // Sub-menu functions
int8_t menu_2_snapshot_mode();

// Global variables
static int8_t demo_board_connected;     //!< Set to 1 if the board is connected

const float resistor = .02;             //!< Sense resistor value

const float LTC4151_sense_lsb = 20e-6;  //!< Typical sense lsb weight in volts
const float LTC4151_vin_lsb = 25e-3;    //!< Typical Vin lsb weight in volts
const float LTC4151_adin_lsb = 0.5;     //!< Typical ADIN lsb weight in mV

//! Initialize Linduino
void setup()
{
  char demo_name[] = "DC1208";    // Demo Board Name stored in QuikEval EEPROM
  quikeval_I2C_init();            //! Initializes Linduino I2C port to 100kHz
  quikeval_I2C_connect();         //! Connects I2C port to the QuikEval connector
  Serial.begin(115200);           //! Initialize the serial port to the PC
  print_title();
  demo_board_connected = discover_demo_board(demo_name);    //! Checks if correct demo board is connected.
  if (demo_board_connected)       // Do nothing if the demo board is not connected
    print_prompt();
}

//! Repeats Linduino loop
void loop()
{
  int8_t ack = 0;
  uint8_t user_command;
  if (demo_board_connected)               //! Does nothing if the demo board is not connected
  {
    if (Serial.available())             // Checks for user input
    {
      user_command = read_int();      //! Reads the user command
      if (user_command != 'm')
        Serial.println(user_command);
      ack = 0;
      switch (user_command)           //! Prints the appropriate submenu
      {
        case 1:
          ack = menu_1_continuous_mode(); // Execute continuous mode
          break;
        case 2:
          ack = menu_2_snapshot_mode();   // Execute snapshot mode
          break;
        default:
          Serial.println("Incorrect Option");
          break;
      }
      if (ack != 0)
        Serial.println("Error: No Acknowledge. Check I2C Address.");
      Serial.println();
      Serial.println("***************************");
      print_prompt();
    }
  }
}

// Function Definitions

//! Prints the title block when program first starts.
void print_title()
{
  Serial.println("");
  Serial.println("*****************************************************************");
  Serial.println("* DC1208 Demonstration Program                                  *");
  Serial.println("*                                                               *");
  Serial.println("* This program communicates with the LTC4151 High Voltage I2C   *");
  Serial.println("* Current and Voltage Monitor found on the DC1208A demo board.  *");
  Serial.println("* Set the baud rate to 115200 and select the newline terminator.*");
  Serial.println("*                                                               *");
  Serial.println("*****************************************************************");
}

//! Prints main menu.
void print_prompt()
{
  Serial.println();
  Serial.println("1-Read Continuous Mode");
  Serial.println("2-Read Snapshot Mode");
  Serial.println();
  Serial.print("Enter a command: ");
}

//! Build and send the snapshot command
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC4151_snapshot(uint8_t i2c_address, //!< I2C address of the LTC4151.
                        uint8_t channel)     //!< Desired channel from the header
{
  int8_t ack = 0;
  ack |= LTC4151_write(i2c_address, LTC4151_CONTROL_REG, LTC4151_CONTINUOUS_MODE);              //! Disable previous snapshot mode to allow a new snapshot
  ack |= LTC4151_write(i2c_address, LTC4151_CONTROL_REG, (channel | LTC4151_SNAPSHOT_MODE));    //! Re-enable snapshot mode on selected channel
  return(ack);
}

//! Reads all inputs in continuous mode
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t menu_1_continuous_mode()
{
  int8_t ack = 0;

  // Read Continuous Mode
  ack |= LTC4151_write(LTC4151_I2C_ADDRESS, LTC4151_CONTROL_REG, LTC4151_CONTINUOUS_MODE);  //! Set LTC4151 to continuous mode
  do
  {
    uint16_t current_sense_adc_code, vin_adc_code, adin_adc_code;

    ack |= LTC4151_read_12_bits(LTC4151_I2C_ADDRESS, LTC4151_SENSE_MSB_REG, &current_sense_adc_code);  //! Reads two bytes for SENSE, MSB register followed by LSB register
    ack |= LTC4151_read_12_bits(LTC4151_I2C_ADDRESS, LTC4151_VIN_MSB_REG, &vin_adc_code);              //! Reads two bytes for VIN, MSB register followed by LSB register
    ack |= LTC4151_read_12_bits(LTC4151_I2C_ADDRESS, LTC4151_ADIN_MSB_REG, &adin_adc_code);            //! Reads two bytes for ADIN, MSB register followed by LSB register

    float current, VIN_voltage, ADIN_voltage;
    current = LTC4151_code_to_sense_current(current_sense_adc_code, resistor, LTC4151_sense_lsb);
    VIN_voltage = LTC4151_code_to_vin_voltage(vin_adc_code, LTC4151_vin_lsb);
    ADIN_voltage = LTC4151_code_to_ADIN_voltage(adin_adc_code, LTC4151_adin_lsb);

    //! Display current, VIN voltage, and ADIN Voltage

    Serial.println();
    Serial.println("***************************");
    Serial.print("Load Current: ");
    Serial.print(current);
    Serial.println(" A\n");

    Serial.print("VIN Voltage: ");
    Serial.print(VIN_voltage);
    Serial.println(" V\n");

    Serial.print("ADIN Voltage: ");
    Serial.print(ADIN_voltage);
    Serial.println(" mV\n");

    Serial.println("m-Main Menu");
    delay(2000);
  }
  while (Serial.available() == false); //! Repeats until user enters a character
  read_int();  // Clears Serial.available
  return(ack);
}

//! Reads inputs in snapshot mode
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t menu_2_snapshot_mode()
{
  int8_t ack = 0;
  uint8_t user_command;

  // Snapshot Mode
  do
  {
    //! Displays the Snapshot Mode menu
    Serial.println("***************************");
    Serial.println("Snapshot Mode");
    Serial.println("1-Load Current:");
    Serial.println("2-V_IN Voltage:");
    Serial.println("3-ADIN Voltage:");
    Serial.println("m-Main Menu");
    Serial.print("Enter a command: ");

    user_command = read_int();      //! Reads the user command

    if (user_command == 'm')        // Print m if it is entered
      Serial.println("m");
    else
      Serial.println(user_command); // Print user command
    Serial.println();

    uint16_t current_sense_adc_code, vin_adc_code, adin_adc_code;
    float current, VIN_voltage, ADIN_voltage;

    //! Reads sense resistor current, Vin voltage, or ADIN voltage
    switch (user_command)
    {
      case 1:
        ack |= LTC4151_snapshot(LTC4151_I2C_ADDRESS, LTC4151_SENSE_CHANNEL_REG);
        delay(200); //Wait for conversion to finish.  Could watch busy bit instead to reduce wait time.
        ack |= LTC4151_read_12_bits(LTC4151_I2C_ADDRESS, LTC4151_SENSE_MSB_REG, &current_sense_adc_code);
        current = LTC4151_code_to_sense_current(current_sense_adc_code, resistor, LTC4151_sense_lsb);
        Serial.print("Load Current: ");
        Serial.print(current);
        Serial.println(" A\n");
        break;
      case 2:
        ack |=LTC4151_snapshot(LTC4151_I2C_ADDRESS, LTC4151_VIN_CHANNEL_REG);
        delay(100); //Wait for conversion to finish.  Could watch busy bit instead to reduce wait time.
        ack |= LTC4151_read_12_bits(LTC4151_I2C_ADDRESS, LTC4151_VIN_MSB_REG, &vin_adc_code);
        VIN_voltage = LTC4151_code_to_vin_voltage(vin_adc_code, LTC4151_vin_lsb);
        Serial.print("VIN Voltage: ");
        Serial.print(VIN_voltage);
        Serial.println(" V\n");
        break;
      case 3:
        ack |= LTC4151_snapshot(LTC4151_I2C_ADDRESS, LTC4151_ADIN_CHANNEL_REG);
        delay(100); //Wait for conversion to finish.  Could watch busy bit instead to reduce wait time.
        ack |= LTC4151_read_12_bits(LTC4151_I2C_ADDRESS, LTC4151_ADIN_MSB_REG, &adin_adc_code);
        ADIN_voltage = LTC4151_code_to_ADIN_voltage(adin_adc_code, LTC4151_adin_lsb);
        Serial.print("ADIN Voltage: ");
        Serial.print(ADIN_voltage);
        Serial.println(" mV\n");
        break;
      default:
        if (user_command != 'm')
          Serial.println("Incorrect Option");
        break;
    }
    ack = LTC4151_write(LTC4151_I2C_ADDRESS, LTC4151_CONTROL_REG, LTC4151_CONTINUOUS_MODE);  // Disable snapshot mode
  }
  while ((user_command != 'm') && (ack == 0));
  return(ack);
}
