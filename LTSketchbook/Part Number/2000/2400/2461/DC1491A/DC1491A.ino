/*!
Linear Technology DC1491A Demonstration Board.
LTC2461: 16-Bit I2C Delta Sigma ADCs with 10ppm/C Max Precision Reference.

Linear Technology DC1493A Demonstration Board.
LTC2463: Differential, 16-Bit I2C Delta Sigma ADCs with 10ppm/C Max Precision Reference.

Linear Technology DC1266A Demonstration Board.
LTC2453: Differential, 16-Bit Delta Sigma ADC With I2C Interface

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. A voltage
   source (preferably low-noise) and a precision voltmeter are required. Ensure all
   jumpers on the demo board are installed in their default positions from the
   factory. Refer to Demo Manual DC1491A.
   * Run menu entry 4 if the voltage readings are not accurate. The DC1491A is not
     calibrated in the factory. Default values will be used until first customer
     calibration.
   If the voltage readings are in error, run menu entry 4 again. The previous
   calibration by the user may have been performed improperly.

  Menu Entry 1: Read ADC Voltage
   Ensure the applied voltage is within the analog input voltage range of 0V to
   +1.25V. Connect the voltage source between the DC1491A's IN terminal and GND
   terminal. This menu entry configures the LTC2461 to operate in 30 Hz mode.

  Menu Entry 2: Sleep Mode
   Enables sleep mode. Note that the REFOUT voltage will go to zero. Any
   subsequent read command will wake up the LTC2461.

  Menu Entry 3: 60 Hz Speed Mode
   Setup is the same as for Menu Entry 1, except the LTC2461 operates in 60 Hz mode,
   and continuous background offset calibration is not performed.

  Menu Entry 4: Calibration
   Follow the command cues to alternately enter voltages with VIN at GND and with
   VIN near full scale voltage, approximately 1.20V. Upon startup, the calibration
   values will be restored.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2461
http://www.linear.com/product/LTC2463
http://www.linear.com/product/LTC2453

http://www.linear.com/product/LTC2461#demoboards
http://www.linear.com/product/LTC2463#demoboards
http://www.linear.com/product/LTC2453#demoboards


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
    @ingroup LTC2461
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h" // Needs this library to connect to main I2C
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC2461.h"
#include <SPI.h>
#include <Wire.h>

// Function Declaration
void print_title();
void print_prompt();
int8_t restore_calibration();
void store_calibration();
int8_t menu_1_read_30Hz_mode();
int8_t menu_2_sleep_mode();
int8_t menu_3_read_60Hz_mode();
int8_t menu_4_calibrate();

// Global variables
static uint8_t demo_board_connected;        //!< Set to 1 if the board is connected

// Calibration Variables
static float LTC2461_lsb = 1.907377E-05;    //!< Ideal LSB voltage for a perfect part (Vref/(2^16))
static int32_t LTC2461_offset_code = 0;     //!< Ideal offset for a perfect part

//! Initialize Linduino
void setup()
{
  char demo_name[]="DC1491";    // Demo Board Name stored in QuikEval EEPROM

  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz
  quikeval_I2C_connect();       // Connects to Main I2C
  Serial.begin(115200);         // Initialize the serial port to the PC
  print_title();

  demo_board_connected = discover_demo_board(demo_name);
  if (demo_board_connected)
  {
    restore_calibration();
    print_prompt();
  }
}

//! Repeats Linduino loop
void loop()  // ***** BEGIN MAIN CONTROL LOOP *****
{
  int8_t ack = 0;                   // Set I2C acknowledge bit to zero at start of every loop.
  if (demo_board_connected)
  {
    if (Serial.available())         // Check for user input
    {
      uint16_t user_command;
      user_command = read_int();    // Read the user command
      Serial.println(user_command);

      switch (user_command)
      {
          // 30 Hz Standard Read Mode
        case 1:
          ack |= menu_1_read_30Hz_mode();
          break;

        case 2:
          ack |= menu_2_sleep_mode();
          break;

        case 3:
          // 60 Hz Read Mode
          ack |= menu_3_read_60Hz_mode();
          break;

        case 4:
          // Calibration
          ack |= menu_4_calibrate();
          break;

        default:
          Serial.println("***** INCORRECT ENTRY *****");  // Input validation
      }
      Serial.println();
      if (ack != 0)
      {
        Serial.println("Error: No Acknowledge. Check I2C Address.");
        Serial.println("***** ACK ERROR *****");
      }

      Serial.println("\n*****************************************************************");
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
  Serial.println("* DC1491A Demonstration Program                                 *");
  Serial.println("*                                                               *");
  Serial.println("* This program demonstrates how to send data and receive data   *");
  Serial.println("* from the LTC2461 16-bit ADC.                                  *");
  Serial.println("*                                                               *");
  Serial.println("*                                                               *");
  Serial.println("* Set the baud rate to 115200 and select the newline terminator.*");
  Serial.println("*                                                               *");
  Serial.println("*****************************************************************");
}

//! Prints main menu.
void print_prompt()    // This function prints menu
{
  Serial.println("");
  Serial.println("1-Read ADC Voltage");
  Serial.println("2-Sleep Mode");
  Serial.println("3-60 Hz Speed Mode");
  Serial.println("4-Calibration");
  Serial.println();
  Serial.print("Enter a command:");
}

//! Store measured calibration parameters to nonvolatile EEPROM on demo board
void store_calibration()
// Store the ADC calibration to the EEPROM
{
  eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, EEPROM_CAL_STATUS_ADDRESS);         // Cal key
  eeprom_write_int32(EEPROM_I2C_ADDRESS, LTC2461_offset_code, EEPROM_CAL_STATUS_ADDRESS+2);  // Store Offset
  eeprom_write_float(EEPROM_I2C_ADDRESS, LTC2461_lsb, EEPROM_CAL_STATUS_ADDRESS+6);          // Store LSB
  Serial.println("Calibration Stored to EEPROM");
}

//! Read stored calibration parameters from nonvolatile EEPROM on demo board
//! @return Return 1 if successful, 0 if not
int8_t restore_calibration()
{
// Read the calibration from EEPROM
// Return 1 if successful, 0 if not

  int16_t cal_key;
  // Read the cal key from the EEPROM
  eeprom_read_int16(EEPROM_I2C_ADDRESS, &cal_key, EEPROM_CAL_STATUS_ADDRESS);
  if (cal_key == EEPROM_CAL_KEY)
  {
    // Calibration has been stored, read offset and lsb
    eeprom_read_int32(EEPROM_I2C_ADDRESS, &LTC2461_offset_code, EEPROM_CAL_STATUS_ADDRESS+2);  // Offset
    eeprom_read_float(EEPROM_I2C_ADDRESS, &LTC2461_lsb, EEPROM_CAL_STATUS_ADDRESS+6);          // LSB
    Serial.println("Calibration Restored");
    return(1);
  }
  else
  {
    Serial.println("Calibration not found");
    return(0);
  }
}

//! Read ADC in 30 Hz mode
//! @return ACK bit (0=acknowledge, 1=no acknowledge)
int8_t menu_1_read_30Hz_mode()
{
  uint16_t adc_code_30Hz;
  int8_t ack = 0;
  Serial.println();
  Serial.print("ADC Command: b");
  Serial.println(LTC2461_60HZ_SPEED_MODE, BIN);
  ack |= LTC2461_read(LTC2461_I2C_ADDRESS, LTC2461_30HZ_SPEED_MODE, &adc_code_30Hz);  // Throw out this reading
  delay(100);
  ack |= LTC2461_read(LTC2461_I2C_ADDRESS, LTC2461_30HZ_SPEED_MODE, &adc_code_30Hz);
  delay(100);
  Serial.print("Received Code: 0x");
  Serial.println(adc_code_30Hz, HEX);

  // Use the calibration values stored on the DC1491A EEPROM to convert the raw_adc_reading to a (float) voltage value which is stored in adc_voltage_30Hz.
  // Run menu entry 4 if the voltage readings are not accurate.  The DC1491A is not calibrated in the factory.  Default values will be used until first customer calibration.
  // If the voltage readings are in error, run menu 4 again.  The previous calibration by the user may have been performed improperly.

  float adc_voltage_30Hz;
  adc_voltage_30Hz = LTC2461_code_to_voltage(adc_code_30Hz, LTC2461_lsb, LTC2461_offset_code);
  Serial.println();
  Serial.print("Voltage Reading: ");
  Serial.print(adc_voltage_30Hz, 4);
  Serial.println("V");
  return(ack);
}

//! Sleep Mode
//! @return ACK bit (0=acknowledge, 1=no acknowledge)
int8_t menu_2_sleep_mode()
{
  int8_t ack = 0;
  ack |= LTC2461_command(LTC2461_I2C_ADDRESS, LTC2461_SLEEP_MODE);
  delay(100);
  Serial.println();
  Serial.print("ADC Command: b");
  Serial.println(LTC2461_SLEEP_MODE, BIN);
  ack |= LTC2461_command(LTC2461_I2C_ADDRESS, LTC2461_SLEEP_MODE);    // send sleep mode command
  delay(100);
  Serial.println();
  Serial.println("***** LTC2461 In Sleep Mode ******");
  return(ack);
}

//! Read ADC in 60 Hz mode
//! @return ACK bit (0=acknowledge, 1=no acknowledge)
int8_t menu_3_read_60Hz_mode()
{
  // 60 Hz Read Mode
  uint16_t adc_code_60Hz;
  int8_t ack = 0;
  Serial.println();
  Serial.print("ADC Command: b");
  Serial.println(LTC2461_30HZ_SPEED_MODE, BIN);
  ack |= LTC2461_read(LTC2461_I2C_ADDRESS, LTC2461_60HZ_SPEED_MODE, &adc_code_60Hz);     // Throw out this reading
  delay(100);
  ack |= LTC2461_read(LTC2461_I2C_ADDRESS, LTC2461_60HZ_SPEED_MODE, &adc_code_60Hz);
  delay(100);
  Serial.print("Received Code: 0x");
  Serial.println(adc_code_60Hz, HEX);

  // Use the calibration values stored on the DC1491A EEPROM to convert the raw_adc_reading to a (float) voltage value which is stored in adc_voltage_60Hz
  // Run menu entry 4 if the voltage readings are not accurate.  The DC1491A is not calibrated in the factory.  Default values will be used until first customer calibration.
  // If the voltage readings are in error, run menu 4 again.  The previous calibration by the user may have been performed improperly.

  float adc_voltage_60Hz;
  adc_voltage_60Hz = LTC2461_code_to_voltage(adc_code_60Hz, LTC2461_lsb, LTC2461_offset_code);
  Serial.println();
  Serial.print("Voltage Reading: ");
  Serial.print(adc_voltage_60Hz, 4);
  Serial.println("V");
  Serial.println("*** 60 Hz Speed Mode ***");
  return(ack);
}

//! Calibrate the LTC2461
//! @return ACK bit (0=acknowledge, 1=no acknowledge)
int8_t menu_4_calibrate()
{
  // Calibration
  uint16_t zero_code;   // Cal zero code
  float zero_voltage;
  int8_t ack = 0;

  Serial.println("Apply 100mV to the input.");
  Serial.println("or apply a voltage for the lower point in two point calibration");
  Serial.print("Enter the measured input voltage:");
  zero_voltage = read_float();
  Serial.println(zero_voltage, 6);

  ack |= LTC2461_read(LTC2461_I2C_ADDRESS, LTC2461_30HZ_SPEED_MODE, &zero_code);  // Throw out this reading
  delay(100);
  ack |= LTC2461_read(LTC2461_I2C_ADDRESS, LTC2461_30HZ_SPEED_MODE, &zero_code);  // Measure zero

  Serial.println("Apply ~1.20V input voltage to input.");
  Serial.println("Enter the measured input voltage:");
  float fs_voltage;               // Measured cal voltage
  fs_voltage = read_float();      // read function in UserInterface.h: reads float from serial input
  Serial.println(fs_voltage, 6);

  uint16_t fs_code;               // Cal full scale code
  ack |= LTC2461_read(LTC2461_I2C_ADDRESS, LTC2461_30HZ_SPEED_MODE, &fs_code);     // Throw out this reading
  delay(100);
  ack |= LTC2461_read(LTC2461_I2C_ADDRESS, LTC2461_30HZ_SPEED_MODE, &fs_code);     // Measure full scale
  LTC2461_cal_voltage(zero_code, fs_code, zero_voltage, fs_voltage, &LTC2461_lsb, &LTC2461_offset_code);  // Modifies offset code and lsb size using linear interpolation

  Serial.print("ADC offset code: ");
  Serial.print(LTC2461_offset_code);
  Serial.println();
  Serial.print("ADC calibrated lsb size : ");
  Serial.print(LTC2461_lsb * 1.0e6, 4);
  Serial.println(" uV (16-bits)");
  store_calibration();             // Store cal factors in EEPROM
  return(ack);
}