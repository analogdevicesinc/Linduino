/*!
Linear Technology DC1338 Demonstration Board.
LTC2990: 14-bit ADC Quad I2C Voltage, Current, and Temperature monitor.

@verbatim

 Setup:
    Set the terminal baud rate to 115200 and select the newline terminator.
    A precision voltage source (preferably low-noise) may be used to apply a voltage
    to input terminals V1-V8. A precision voltmeter may be used to verify applied
    voltages. An oscilloscope may be used to view the PWM output. Ensure JP5, JP6
    and JP7 are in the LOW position. Refer to Demo Manual DC1338A

  Explanation of Commands:

    1 - Single-Ended Voltage - Selects the Single-Ended Voltage Menu.
             1-4: Displays the measured single-ended voltage at one of the V1-V4
                  inputs. When measuring V1 and V8, ensure jumpers are set to VOLT
                  position.
               5: Vcc - Displays the measured Vcc voltage.
              6: ALL - Displays the measured voltages at all of the V1-V8 inputs
                  and Vcc.

    2 - Differential Voltage - Selects the Differential Voltage Menu.
        Maximum full scale differential voltage is 0.300V.

             1-2: Displays the measured differential voltage across one of the V1-V4
                  input pairs. The input common-mode range is 0V to Vcc.  It is
                  easiest to ground the lower input. When measuring V1 and V4,
                  ensure jumpers are set to VOLT position.
               3: ALL - Displays the measured differential voltages at all terminals.

    3 - Temperature - Selects the Temperature Menu
        To measure temperature using on-board transistors, set JP1, JP2, JP3 and JP4
        to TEMP position.
               1: V1-V2 - Measure temperature of Q1 (mounted to demo board) when JP1
                  and JP2 are in TEMP position.
               2: V3-V4 - Measure temperature of external transistor connected to V3
                  and V4 terminals.
               3: Internal - Measure temperature using the internal temperature
                  sensor.
               4: All - Displays temperatures at all connections as well as the
                  internal temperature sensor.

    4 - Settings - Selects the Settings Menu
        Toggles temperature units between degrees Celsius or degrees Kelvin.
               1: Toggle temperature units between degrees Celsius and degrees
                  Kelvin.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2990

http://www.linear.com/product/LTC2990#demoboards


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
    @ingroup LTC2990
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC2990.h"
#include <Wire.h>
#include <SPI.h>

// Function Declaration
void print_title();                     // Print the title block
void print_prompt();                    // Prompt the user for an input command

int8_t menu_1_single_ended_voltage();   // Sub-menu functions
int8_t menu_2_read_differential_voltage();
int8_t menu_3_read_temperature();
int8_t menu_4_settings();

// Global variables
static uint8_t demo_board_connected;    //!< Set to 1 if the board is connected
const uint16_t LTC2990_TIMEOUT=1000;    //!< Configures the maximum timeout allowed for an LTC2990 read.

// Calibration Variables
const float LTC2990_SINGLE_ENDED_lsb = 3.05176E-04;     //!< Typical single-ended LSB weight in volts
const float LTC2990_DIFFERENTIAL_lsb = 1.90735E-05;     //!< Typical differential LSB weight in volts
const float LTC2990_VCC_lsb = 3.05176E-04;              //!< Typical VCC LSB weight in volts
// Used for internal temperature as well as remote diode temperature measurements.
const float LTC2990_TEMPERATURE_lsb = 0.0625;           //!< Typical temperature LSB weight in degrees Celsius (and Kelvin).
// Used to readback diode voltage when in temperature measurement mode.
const float LTC2990_DIODE_VOLTAGE_lsb = 3.815E-05;      //!< Typical remote diode LSB weight in volts.

//! Initialize Linduino
void setup()
{
  char demo_name[] = "DC1338";  // Demo Board Name stored in QuikEval EEPROM
  int8_t ack=0;
  quikeval_I2C_init();          // Initializes Linduino I2C port.
  quikeval_I2C_connect();       // Connects I2C port to the QuikEval connector
  Serial.begin(115200);         // Initialize the serial port to the PC
  print_title();
  demo_board_connected = discover_demo_board(demo_name);  // Checks if correct demo board is connected.
  if (demo_board_connected)
  {
    print_prompt();
    ack |= LTC2990_register_write(LTC2990_I2C_ADDRESS, LTC2990_CONTROL_REG, LTC2990_ENABLE_ALL);   // Enables all channels
    ack |= LTC2990_register_write(LTC2990_I2C_ADDRESS, LTC2990_TRIGGER_REG, 0x01);
  }
}

//! Repeats Linduino loop
void loop()
{
  int8_t ack=0;

  uint8_t user_command;
  if (demo_board_connected)           // Does nothing if the demo board is not connected
  {
    if (Serial.available())           // Checks for user input
    {
      user_command = read_int();      // Reads the user command
      if (user_command != 'm')
        Serial.println(user_command);
      ack = 0;
      switch (user_command)           // Prints the appropriate submenu
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
  Serial.print(F("* DC1338B Demonstration Program                                 *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send and receive data from   *\n"));
  Serial.print(F("* the LTC2990 14-Bit Quad I2C Voltage, Current, and            *\n"));
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
    // Displays the single-ended voltage menu
    Serial.print(F("\nSingle-Ended Voltage\n\n"));
    Serial.print(F("  1-V1\n"));
    Serial.print(F("  2-V2\n"));
    Serial.print(F("  3-V3\n"));
    Serial.print(F("  4-V4\n"));
    Serial.print(F("  5-Vcc\n"));
    Serial.print(F("  6-ALL\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\n\nEnter a command: "));

    user_command = read_int();                              // Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
    {
      Serial.print(F("m\n"));
    }
    else
      Serial.println(user_command);                         // Print user command

    int16_t code;
    int8_t data_valid;
    float voltage;

    // Enable Single-Ended Mode
    ack |= LTC2990_register_set_clear_bits(LTC2990_I2C_ADDRESS, LTC2990_CONTROL_REG, LTC2990_V1_V2_V3_V4, LTC2990_VOLTAGE_MODE_MASK);


    // Reads single-ended voltage from ADC and prints it.
    switch (user_command)
    {
      case 1:
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V1_MSB_REG, &code, &data_valid, LTC2990_TIMEOUT);
        voltage = LTC2990_code_to_single_ended_voltage(code, LTC2990_SINGLE_ENDED_lsb);
        Serial.print(F("\n  V1: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 2:
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V2_MSB_REG, &code, &data_valid, LTC2990_TIMEOUT);
        voltage = LTC2990_code_to_single_ended_voltage(code, LTC2990_SINGLE_ENDED_lsb);
        Serial.print(F("\n  V2: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 3:
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V3_MSB_REG, &code, &data_valid, LTC2990_TIMEOUT);
        voltage = LTC2990_code_to_single_ended_voltage(code, LTC2990_SINGLE_ENDED_lsb);
        Serial.print(F("\n  V3: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 4:
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V4_MSB_REG, &code, &data_valid, LTC2990_TIMEOUT);
        voltage = LTC2990_code_to_single_ended_voltage(code, LTC2990_SINGLE_ENDED_lsb);
        Serial.print(F("\n  V4: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 5:
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_VCC_MSB_REG, &code, &data_valid, LTC2990_TIMEOUT);
        voltage = LTC2990_code_to_vcc_voltage(code, LTC2990_SINGLE_ENDED_lsb);
        Serial.print(F("\n  Vcc: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 6:
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V1_MSB_REG, &code, &data_valid, LTC2990_TIMEOUT);
        voltage = LTC2990_code_to_single_ended_voltage(code, LTC2990_SINGLE_ENDED_lsb);
        Serial.print(F("\n  V1: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        if (ack)
          break;
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V2_MSB_REG, &code, &data_valid, LTC2990_TIMEOUT);
        voltage = LTC2990_code_to_single_ended_voltage(code, LTC2990_SINGLE_ENDED_lsb);
        Serial.print(F("  V2: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        if (ack)
          break;
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V3_MSB_REG, &code, &data_valid, LTC2990_TIMEOUT);
        voltage = LTC2990_code_to_single_ended_voltage(code, LTC2990_SINGLE_ENDED_lsb);
        Serial.print(F("  V3: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        if (ack)
          break;
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V4_MSB_REG, &code, &data_valid, LTC2990_TIMEOUT);
        voltage = LTC2990_code_to_single_ended_voltage(code, LTC2990_SINGLE_ENDED_lsb);
        Serial.print(F("  V4: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        if (ack)
          break;
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_VCC_MSB_REG, &code, &data_valid, LTC2990_TIMEOUT);
        voltage = LTC2990_code_to_vcc_voltage(code, LTC2990_SINGLE_ENDED_lsb);
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
    // Display differential voltage menu.
    Serial.print(F("\nDifferential Voltage\n\n"));
    Serial.print(F("  1-V1-V2\n"));
    Serial.print(F("  2-V3-V4\n"));
    Serial.print(F("  3-ALL\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\n\nEnter a command: "));

    user_command = read_int();                              // Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
    {
      Serial.print(F("m\n"));
    }
    else
      Serial.println(user_command);                       // Print user command

    int8_t data_valid;
    int16_t code;
    float voltage;


    // Enables differential mode.
    ack |= LTC2990_register_set_clear_bits(LTC2990_I2C_ADDRESS, LTC2990_CONTROL_REG, LTC2990_V1V2_V3V4, LTC2990_VOLTAGE_MODE_MASK);
    // Triggers a conversion by writing any value to the trigger register
    ack |= LTC2990_register_write(LTC2990_I2C_ADDRESS, LTC2990_TRIGGER_REG, 0x00);

    // Flushes one reading following mode change.
    // Reads differential voltage from ADC and prints it.
    switch (user_command)
    {
      case 1:
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V2_MSB_REG, &code, &data_valid, LTC2990_TIMEOUT);
        voltage = LTC2990_code_to_differential_voltage(code, LTC2990_DIFFERENTIAL_lsb);
        Serial.print(F("\n  V1-V2: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 2:
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V4_MSB_REG, &code, &data_valid, LTC2990_TIMEOUT);
        voltage = LTC2990_code_to_differential_voltage(code, LTC2990_DIFFERENTIAL_lsb);
        Serial.print(F("\n  V3-V4: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        break;
      case 3:
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V2_MSB_REG, &code, &data_valid, LTC2990_TIMEOUT);
        voltage = LTC2990_code_to_differential_voltage(code, LTC2990_DIFFERENTIAL_lsb);
        Serial.print(F("\n  V1-V2: "));
        Serial.print(voltage, 4);
        Serial.print(F(" V\n"));
        if (ack)
          break;
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V4_MSB_REG, &code, &data_valid, LTC2990_TIMEOUT);
        voltage = LTC2990_code_to_differential_voltage(code, LTC2990_DIFFERENTIAL_lsb);
        Serial.print(F("  V3-V4: "));
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
  boolean isKelvin = false;     // Keeps track of the unit of measurement
  uint8_t user_command;
  do
  {
    // Displays temperature menu
    Serial.print(F("\nTemperature\n\n"));
    Serial.print(F("  1-V1-V2\n"));
    Serial.print(F("  2-V3-V4\n"));
    Serial.print(F("  3-Internal\n"));
    Serial.print(F("  4-ALL\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\n\nEnter a command: "));
    user_command = read_int();                                  // Reads the user command
    if (user_command == 'm')                                    // Print m if it is entered
    {
      Serial.print(F("m\n"));
    }
    else
      Serial.println(user_command);                             // Print user command

    // Read Temperature
    int8_t data_valid = 0;
    int16_t adc_code;
    uint8_t reg_data;
    float temperature;

    // Enables temperature mode
    ack |= LTC2990_register_set_clear_bits(LTC2990_I2C_ADDRESS, LTC2990_CONTROL_REG, LTC2990_TR1_TR2, LTC2990_VOLTAGE_MODE_MASK);
    // Triggers a conversion by writing any value to the trigger register
    ack |= LTC2990_register_write(LTC2990_I2C_ADDRESS, LTC2990_TRIGGER_REG, 0x01);
    // Flushes one reading following mode change.
    ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V1_MSB_REG, &adc_code, &data_valid, LTC2990_TIMEOUT);
    //ack |= LTC2990_register_write(LTC2990_I2C_ADDRESS, LTC2990_TRIGGER_REG, 0x01);
    //delay(165); //Wait max conversion time

    // Reads temperature from ADC and prints it.
    switch (user_command)
    {
      case 1:
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V1_MSB_REG, &adc_code, &data_valid, LTC2990_TIMEOUT);
        ack |= LTC2990_register_read(LTC2990_I2C_ADDRESS, LTC2990_CONTROL_REG, &reg_data);
        if (reg_data & LTC2990_KELVIN_ENABLE) isKelvin= true;
        else isKelvin=false;
        temperature = LTC2990_temperature(adc_code, LTC2990_TEMPERATURE_lsb, isKelvin);
        Serial.print(F("\n  V1-V2: "));
        Serial.print(temperature, 2);
        if (isKelvin) Serial.print(F(" K\n"));
        else Serial.print(F(" C\n"));
        break;
      case 2:
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V3_MSB_REG, &adc_code, &data_valid, LTC2990_TIMEOUT);
        ack |= LTC2990_register_read(LTC2990_I2C_ADDRESS, LTC2990_CONTROL_REG, &reg_data);
        if (reg_data & LTC2990_KELVIN_ENABLE) isKelvin=true;
        else isKelvin = false;
        temperature = LTC2990_temperature(adc_code, LTC2990_TEMPERATURE_lsb, isKelvin);
        Serial.print(F("\n  V3-V4: "));
        Serial.print(temperature, 2);
        if (isKelvin) Serial.print(F(" K\n"));
        else Serial.print(F(" C\n"));
        break;
      case 3:
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_TINT_MSB_REG, &adc_code, &data_valid, LTC2990_TIMEOUT);
        ack |= LTC2990_register_read(LTC2990_I2C_ADDRESS, LTC2990_CONTROL_REG, &reg_data);
        if (reg_data & LTC2990_KELVIN_ENABLE) isKelvin=true;
        else isKelvin=false;
        temperature = LTC2990_temperature(adc_code, LTC2990_TEMPERATURE_lsb,isKelvin);
        Serial.print(F("\n  Internal: "));
        Serial.print(temperature, 2);
        if (isKelvin) Serial.print(F(" K\n"));
        else Serial.print(F(" C\n"));
        break;
      case 4:
        // All Temperatures
        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V1_MSB_REG, &adc_code, &data_valid, LTC2990_TIMEOUT);
        ack |= LTC2990_register_read(LTC2990_I2C_ADDRESS, LTC2990_CONTROL_REG, &reg_data);
        if (reg_data & LTC2990_KELVIN_ENABLE) isKelvin= true;
        else isKelvin=false;
        temperature = LTC2990_temperature(adc_code, LTC2990_TEMPERATURE_lsb, isKelvin);
        Serial.print(F("\n  V1-V2: "));
        Serial.print(temperature, 2);
        if (isKelvin) Serial.print(F(" K\n"));
        else Serial.print(F(" C\n"));
        if (ack)
          break;

        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_V3_MSB_REG, &adc_code, &data_valid, LTC2990_TIMEOUT);
        ack |= LTC2990_register_read(LTC2990_I2C_ADDRESS, LTC2990_CONTROL_REG, &reg_data);
        if (reg_data & LTC2990_KELVIN_ENABLE) isKelvin=true;
        else isKelvin = false;
        temperature = LTC2990_temperature(adc_code, LTC2990_TEMPERATURE_lsb, isKelvin);
        Serial.print(F("\n  V3-V4: "));
        Serial.print(temperature, 2);
        if (isKelvin) Serial.print(F(" K\n"));
        else Serial.print(F(" C\n"));
        if (ack)
          break;

        // Flush one ADC reading in case it is stale.  Then, take a new fresh reading.
        ack |= LTC2990_adc_read_new_data(LTC2990_I2C_ADDRESS, LTC2990_TINT_MSB_REG, &adc_code, &data_valid, LTC2990_TIMEOUT);
        ack |= LTC2990_register_read(LTC2990_I2C_ADDRESS, LTC2990_CONTROL_REG, &reg_data);
        if (reg_data & LTC2990_KELVIN_ENABLE) isKelvin=true;
        else isKelvin=false;
        temperature = LTC2990_temperature(adc_code, LTC2990_TEMPERATURE_lsb,isKelvin);
        Serial.print(F("\n  Internal: "));
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

  Serial.println(F("\nSettings:"));
  Serial.println(F("0-Celsius"));
  Serial.println(F("1-Kelvin"));
  Serial.print(F("\nEnter a command: "));
  user_command = read_int();
  Serial.println(user_command);

  if (user_command)
  {
    // Set to Kelvin mode
    ack |= LTC2990_register_set_clear_bits(LTC2990_I2C_ADDRESS, LTC2990_CONTROL_REG, LTC2990_KELVIN_ENABLE, LTC2990_TEMP_FORMAT_MASK);
    Serial.println(F("The LTC2990 is in Kelvin Mode"));
  }
  else
  {
    // Set to Celsius mode
    ack |= LTC2990_register_set_clear_bits(LTC2990_I2C_ADDRESS, LTC2990_CONTROL_REG, LTC2990_CELSIUS_ENABLE, LTC2990_TEMP_FORMAT_MASK);
    Serial.println(F("The LTC2990 is in Celsius Mode"));
  }
  return(ack);
}