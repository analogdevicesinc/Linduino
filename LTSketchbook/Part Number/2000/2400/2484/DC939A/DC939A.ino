
/*!
Linear Technology DC939A Demonstration Board.
LTC2484: 24-Bit Delta Sigma ADC with Easy Drive Input Current Cancellation.

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. Equipment
   required is a precision voltage source and a precision voltmeter. No external
   power supply is required. Ensure JP1 is in the +5V position.

  To Read data:
   The voltage source should be connected with positive lead to IN+ and negative
   lead to IN-. The voltage source negative output must also be connected to the GND
   pin in order to provide a ground-referenced voltage. Ensure voltage is within
   analog input voltage range -0.3V to +2.5V. Swapping input voltages results in a
   reversed polarity reading.

  How to calibrate:
    Short the inputs to ground to calibrate the offset. Next, hit ENTER (this takes
    the reading). Now apply approximately 2.49 volts to +IN, with -IN connected to
    ground. Measure this voltage with a precise voltmeter and enter this value.
    Calibration is now stored in EEPROM. Upon startup the calibration values will be
    restored.

    Explanation of Commands:

                           **** MAIN MENU ****

      0- Read- By entering this a voltage at the +IN, -IN terminals will be read.
      1- Set Rejection- Select this to access the Filter rejection menu. Follow
         command cues to enable desired rejection profile of on-chip digital filters.
      2- Select 2X Rate- This selection allows disabling of the autocalibration
         feature to achieve a benefit of twice the output rate.
      3- Calibrate Voltage- Follow the calibration cues to calibrate the device
         voltage.
      4- Calibrate Temperature- Select this and follow the cues if it is desired to
         calibrate the integrated
         temperature sensor.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2484

http://www.linear.com/product/LTC2484#demoboards


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
    @ingroup LTC2484
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC2484.h"
#include "LTC24XX_general.h"
#include <SPI.h>
#include <Wire.h>

// Rejection Constants
#define REJECTION50_60  0               //!< 50-60Hz rejection
#define REJECTION50     1               //!< 50Hz rejection
#define REJECTION60     2               //!< 60Hz rejection

// Function Declaration
void print_title();                             // Print the title block
void print_prompt();                            // Prompt the user for an input command
int8_t restore_calibration();                   // Read the DAC calibration from EEPROM, Return 1 if successful, 0 if not
void store_calibration();                       // Store the ADC calibration to the EEPROM
uint8_t build_adc_command(uint8_t temperature); // Build the ADC command byte

uint8_t menu_0_read();
void menu_1_set_rejection();
void menu_2_set_1X_2X();
uint8_t menu_3_calibrate_voltage();
uint8_t menu_4_calibrate_temperature();

// Global variables
static uint8_t demo_board_connected;            //!< Set to 1 if the board is connected
static int8_t  adc_rejection = REJECTION50_60;  //!< The LTC2484 rejection
static int8_t  adc_2x = 0;                      //!< The LTC2484 2x speed mode

// Calibration variables
static float LTC2484_lsb = 9.3132258E-9;  //!< Ideal LSB size, 5V/(2^29) for a 5V reference
static int32_t LTC2484_offset_code = 0;   //!< Ideal offset
static float LTC2484_t0 = 27.0;           //!< Nominal temperature
static float LTC2484_r0 = 45.097156E6;    //!< ADC code at the nominal temperature (420mV default)

const uint16_t MISO_TIMEOUT = 1000;       //!< The MISO timeout (ms)

//! Initialize Linduino
void setup()
// Setup the program
{
  char demo_name[] = "DC939";   // Demo Board Name stored in QuikEval EEPROM

  quikeval_SPI_init();          // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();       // Connect SPI to main data port
  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz
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
void loop()
{
  uint8_t user_command;             // The user input command
  uint8_t acknowledge = 0;
  if (demo_board_connected)
  {
    if (Serial.available())         // Check for user input
    {
      user_command = read_int();    // Read the user command
      if (user_command == 'm');
      else
        Serial.println(user_command);
      delay(50);                    // Allow the print to finish
      switch (user_command)
      {
        case 0:
          acknowledge |= menu_0_read();
          break;
        case 1:
          menu_1_set_rejection();
          break;
        case 2:
          menu_2_set_1X_2X();
          break;
        case 3:
          acknowledge |= menu_3_calibrate_voltage();
          break;
        case 4:
          acknowledge |= menu_4_calibrate_temperature();
          break;
        default:
          Serial.println("Incorrect Option");
          break;
      }
      if (acknowledge)
        Serial.println(F("***** SPI ERROR *****"));
      Serial.println(F("*****************************************************************"));
      print_prompt();
    }
  }
}

// Function Definitions

//! Prints the title block when program first starts.
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC939A Demonstration Program                                  *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data and receive data   *"));
  Serial.println(F("* from the 24-bit delta-sigma ADC.                              *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 select the newline terminator.    *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.println(F("\nPresent Values:"));
  Serial.print(F("  Rejection: "));
  switch (adc_rejection)
  {
    case REJECTION50_60:
      Serial.println(F("50-60Hz rejection"));
      break;
    case REJECTION50:
      Serial.println(F("50Hz rejection"));
      break;
    case REJECTION60:
      Serial.println(F("60Hz rejection"));
      break;
  }
  Serial.print(F("  2X Speed: "));
  Serial.println(adc_2x, DEC);
  Serial.print(F("  Offset Code="));
  Serial.println(LTC2484_offset_code);
  Serial.print(F("  LSB="));
  Serial.print(LTC2484_lsb * 1.0e9, 4);
  Serial.println(F("nV (32-bits)"));
  Serial.print(F("  R0="));
  Serial.println(LTC2484_r0, 0);
  Serial.print(F("  T0="));
  Serial.print(LTC2484_t0, 1);
  Serial.println(F("C"));
  Serial.println();
  Serial.println(F("Command Summary:"));
  Serial.println(F("  0-Read"));
  Serial.println(F("  1-Set Rejection"));
  Serial.println(F("  2-Set 2X Speed"));
  Serial.println(F("  3-Calibrate Voltage"));
  Serial.println(F("  4-Calibrate Temperature"));
  Serial.println();
  Serial.print(F("Enter a command:"));
}

//! Read stored calibration parameters from nonvolatile EEPROM on demo board
//! @return 0 if successful, 1 if failure
int8_t restore_calibration()
// Read the DAC calibration from EEPROM
{
  int16_t cal_key;
  // Read the cal key from the EEPROM
  eeprom_read_int16(EEPROM_I2C_ADDRESS, &cal_key, EEPROM_CAL_STATUS_ADDRESS);
  if (cal_key == EEPROM_CAL_KEY)
  {
    // Calibration has been stored, read offset and lsb
    eeprom_read_int32(EEPROM_I2C_ADDRESS, &LTC2484_offset_code, EEPROM_CAL_STATUS_ADDRESS + 2);  // Offset
    eeprom_read_float(EEPROM_I2C_ADDRESS, &LTC2484_lsb, EEPROM_CAL_STATUS_ADDRESS + 6);          // LSB
    eeprom_read_float(EEPROM_I2C_ADDRESS, &LTC2484_r0, EEPROM_CAL_STATUS_ADDRESS + 10);          // Temp r0
    eeprom_read_float(EEPROM_I2C_ADDRESS, &LTC2484_t0, EEPROM_CAL_STATUS_ADDRESS + 14);          // Temp t0
    Serial.println(F("Calibration Restored"));
    return (1);
  }
  else
  {
    Serial.println(F("Calibration not found"));
    return (0);
  }
}

//! Store measured calibration parameters to nonvolatile EEPROM on demo board
void store_calibration()
// Store the ADC calibration to the EEPROM
{
  eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, EEPROM_CAL_STATUS_ADDRESS);           // Cal key
  eeprom_write_int32(EEPROM_I2C_ADDRESS, LTC2484_offset_code, EEPROM_CAL_STATUS_ADDRESS + 2);  // Offset
  eeprom_write_float(EEPROM_I2C_ADDRESS, LTC2484_lsb, EEPROM_CAL_STATUS_ADDRESS + 6);          // LSB
  eeprom_write_float(EEPROM_I2C_ADDRESS, LTC2484_r0, EEPROM_CAL_STATUS_ADDRESS + 10);          // Temp r0
  eeprom_write_float(EEPROM_I2C_ADDRESS, LTC2484_t0, EEPROM_CAL_STATUS_ADDRESS + 14);          // Temp t0
  Serial.println(F("Calibration Stored to EEPROM"));
}

//! Construct ADC command from rejection, input, and 2X parameters
//! @return ADC command
uint8_t build_adc_command(uint8_t temperature)
// Build the ADC command byte
{
  uint8_t adc_command = 0;          // The LTC2484 command byte
  adc_command = LTC2484_ENABLE;     // Set ENABLE
  switch (adc_rejection)
  {
    case REJECTION50_60:                            // Set REJECTION
      adc_command |= LTC2484_REJECTION_50HZ_60HZ;
      break;
    case REJECTION50:
      adc_command |= LTC2484_REJECTION_50HZ;
      break;
    case REJECTION60:
      adc_command |= LTC2484_REJECTION_60HZ;
      break;
  }
  if (temperature)                                  // Set Temperature Input
  {
    adc_command |= LTC2484_TEMPERATURE_INPUT;
    adc_command |= LTC2484_AUTO_CALIBRATION;
  }
  else
  {
    adc_command |= LTC2484_EXTERNAL_INPUT;          // Set Voltage Input
    if (adc_2x)
      adc_command |= LTC2484_SPEED_2X;              // Set 2X Speed
    else
      adc_command |= LTC2484_AUTO_CALIBRATION;
  }
  return(adc_command);
}

//! Read ADC
//! @return 0 if successful, 1 if failure
uint8_t menu_0_read()
{
  // Read values
  int32_t adc_code = 0;     // The LTC2484 code
  float adc_voltage = 0.0;  // The LTC2484 input voltage
  float adc_temperature;    // The LTC2484 temperature sensor value
  uint8_t adc_command;      // The LTC2484 command byte

  adc_command = build_adc_command(0);                       // Build ADC command byte for voltage input
  if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2484_read(LTC2484_CS, adc_command, &adc_code);         // Throw away last reading
  adc_command = build_adc_command(1);                       // Build ADC command byte for temperature input
  if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2484_read(LTC2484_CS, adc_command, &adc_code);         // Read voltage
  Serial.println(F("\nVoltage Measurement"));
  Serial.print(F("  Received Code: 0x"));
  Serial.println(adc_code, HEX);
  Serial.print(F("  Voltage:"));
  adc_voltage = LTC2484_code_to_voltage(adc_code, LTC2484_lsb, LTC2484_offset_code);
  Serial.print(adc_voltage, 6);
  Serial.println(F("V "));
  adc_command = build_adc_command(1);                       // Build ADC command byte for voltage input
  if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2484_read(LTC2484_CS, adc_command, &adc_code);         // Read temperature
  Serial.println(F("\nTemperature Measurement"));
  Serial.print(F("  Received Code: 0x"));
  Serial.println(adc_code, HEX);
  Serial.print(F("  Sensor Voltage:"));
  adc_voltage = LTC2484_code_to_voltage(adc_code, LTC2484_lsb, LTC2484_offset_code);
  Serial.print(adc_voltage, 6);
  Serial.println(F("V"));
  Serial.print(F("  Temperature:"));
  adc_temperature = LTC2484_temperature(adc_code, LTC2484_t0, LTC2484_r0);
  Serial.print(adc_temperature, 1);
  Serial.println(F("C\n"));
  return(0);
}

//! Set rejection mode
void menu_1_set_rejection()
{
  uint8_t user_command; // The user input command

  // Set rejection
  Serial.println(F("Rejection :"));
  Serial.println(F("  0: 50-60Hz rejection"));
  Serial.println(F("  1: 50Hz rejection"));
  Serial.println(F("  2: 60Hz rejection"));
  Serial.println();
  Serial.print(F("Select Rejection:"));
  user_command = read_int();                              // Read the user command
  Serial.println(user_command);
  switch (user_command)
  {
    case 1:
      adc_rejection = REJECTION50;
      break;
    case 2:
      adc_rejection = REJECTION60;
      break;
    default:
      adc_rejection = REJECTION50_60;
      break;
  }
}

//! Select 1X or 2X mode
void menu_2_set_1X_2X()
{
  // Set 2X rate
  uint8_t user_command; // The user input command

  Serial.println();
  Serial.print(F("Select 2X Rate (0-OFF, 1-ON): "));
  user_command = read_int();                              // Read the user command
  Serial.println(user_command);
  switch (user_command)
  {
    case 0:
      adc_2x = 0;
      break;
    case 1:
      adc_2x = 1;
      break;
    default:
      adc_2x = 0;
      break;
  }
}

//! Calibrate ADC given two known inputs
//! @return 0 if successful, 1 if failure
uint8_t menu_3_calibrate_voltage()
{
  // Calibrate voltage measurement
  float zero_voltage;       // Measured cal voltage
  float fs_voltage;         // Measured cal voltage
  int32_t zero_code;        // Cal zero code
  int32_t fs_code;          // Cal full scale code
  uint8_t user_command;     // The user input command
  int32_t adc_code = 0;     // The LTC2484 code
  uint8_t adc_command;      // The LTC2484 command byte

  Serial.println(F("Short the inputs to ground calibrate the offset."));
  Serial.println(F("or apply a voltage for the lower point in two point calibration"));
  Serial.print(F("Enter the measured input voltage:"));
  zero_voltage = read_float();
  Serial.println(zero_voltage, 6);

  adc_command = build_adc_command(0);                       // Build ADC command byte for voltage input
  if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2484_read(LTC2484_CS, adc_command, &adc_code);         // Throw away previous reading
  if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2484_read(LTC2484_CS, adc_command, &zero_code);        // Measure zero

  Serial.println(F("Apply ~2.40V to +IN"));
  Serial.println(F("Enter the measured input voltage:"));
  fs_voltage = read_float();

  adc_command = build_adc_command(0);                       // Build ADC command byte for voltage input
  if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2484_read(LTC2484_CS, adc_command, &adc_code);         // Throw away previous reading
  if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2484_read(LTC2484_CS, adc_command, &fs_code);          // Measure full scale

  LTC2484_cal_voltage(zero_code, fs_code, zero_voltage, fs_voltage, &LTC2484_lsb, &LTC2484_offset_code);

  Serial.print(F("ADC offset : "));
  Serial.print(LTC2484_offset_code);
  Serial.print(F(" ADC lsb : "));
  Serial.print(LTC2484_lsb * 1.0e9, 4);
  Serial.println(F("nV (32-bits)"));
  store_calibration();
  return(0);
}

//! Calibrate Temperature given two known inputs
//! @return 0 if successful, 1 if failure
uint8_t menu_4_calibrate_temperature()
{
  float adc_cal_temperature;    // Measured cal temperature
  int32_t adc_code = 0;         // The LTC2484 code
  uint8_t adc_command;          // The LTC2484 command byte

  Serial.println(F("Enter the actual temperature(C):"));
  adc_cal_temperature = read_float();
  adc_command = build_adc_command(1);                       // Build ADC command byte for temperature input
  if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2484_read(LTC2484_CS, adc_command, &adc_code);         // Throw away previous reading
  if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2484_read(LTC2484_CS, adc_command, &adc_code);         // Measure temperature
  LTC2484_cal_temperature(adc_code, adc_cal_temperature, &LTC2484_t0, &LTC2484_r0);  // Cal temperature
  store_calibration();                                      // Store to eeprom
  return(0);
}