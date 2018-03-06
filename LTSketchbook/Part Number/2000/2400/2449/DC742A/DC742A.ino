/*!
Linear Technology DC742A Demonstration Board.
LTC2449: 24-Bit, 16-Channel Delta Sigma ADC with Selectable Speed/Resolution.

Linear Technology DC979A Demonstration Board.
LTC2442: 24-Bit, 4-Channel Delta Sigma ADC with Integrated Amplifier

Linear Technology DC845A Demonstration Board.
LTC2448: 24-Bit, 8-/16-Channel Delta Sigma ADCs with Selectable Speed/Resolution

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. Equipment
   required is a precision voltage source and a precision voltmeter. Additionally,
   an external power supply is required to provide a negative voltage for Amp V-.
   Set it to anywhere from -1V to -5V. Set Amp V+ to Vcc. Ensure the COM and REF-
   pins are connected to ground. The REF+ pin should be connected to +5V.

  How to test Single-Ended mode:
   The voltage source should be connected to the ADC such that the negative lead is
   connected to the COM(common) pin. The positive lead may be connected to any
   channel input. Ensure voltage is within analog input voltage range -0.3 to 2.5V.

  How to test Differential Mode:
   The voltage source should be connected with positive and negative leads to paired
   channels. The voltage source negative output must also be connected to the COM
   pin in order to provide a ground-referenced voltage. Ensure voltage is within
   analog input voltage range -0.3V to +2.5V. Swapping input voltages results in a
   reversed polarity reading.

  How to calibrate:
   Apply 100mV CH0 with respect to COM. Next, measure this voltage with a precise
   voltmeter and enter this value. (This takes the reading.) Now apply approximately
   2.40 volts to CH0. Measure this voltage with a precise voltmeter and enter this
   value. Calibration is now stored in EEPROM. Upon start-up the calibration values
   will be restored.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2449
http://www.linear.com/product/LTC2442
http://www.linear.com/product/LTC2448

http://www.linear.com/product/LTC2449#demoboards
http://www.linear.com/product/LTC2442#demoboards
http://www.linear.com/product/LTC2448#demoboards


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
    @ingroup LTC2449
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC2449.h"
#include <SPI.h>
#include <Wire.h>

// Function Declaration
void print_title();                             // Print the title block
void print_prompt();                            // Prompt the user for an input command
void print_user_command(uint8_t menu);          // Display selected differential channels
void store_calibration();                       // Store the ADC calibration to the EEPROM
int8_t restore_calibration();                   // Read the calibration from EEPROM, return 1 if successful, 0 if not

int8_t menu_1_read_single_ended();
int8_t menu_2_read_differential();
int8_t menu_3_calibrate();
void menu_4_set_OSR();
void menu_5_set_1X2X();
void menu_6_en_dis_cal();

// Global variables
static uint8_t demo_board_connected;            //!< Set to 1 if the board is connected
static int16_t OSR_mode = LTC2449_OSR_32768;    //!< The LTC2449 OSR setting
static int16_t two_x_mode = LTC2449_SPEED_1X;   //!< The LTC2449 2X Mode settings
static float LTC2449_lsb = 9.3132258E-9;        //!< Ideal LSB voltage for a perfect part
static int32_t LTC2449_offset_code = 0;         //!< Ideal offset for a perfect part

// Constants

//! Lookup table to build the command for single-ended mode
const uint16_t BUILD_COMMAND_SINGLE_ENDED[16] = {LTC2449_CH0, LTC2449_CH1, LTC2449_CH2, LTC2449_CH3,
    LTC2449_CH4, LTC2449_CH5, LTC2449_CH6, LTC2449_CH7,
    LTC2449_CH8, LTC2449_CH9, LTC2449_CH10, LTC2449_CH11,
    LTC2449_CH12, LTC2449_CH13, LTC2449_CH14, LTC2449_CH15
                                                };    //!< Builds the command for single-ended mode

//! Lookup table to build the command for differential mode
const uint16_t BUILD_COMMAND_DIFF[16] = {LTC2449_P0_N1, LTC2449_P2_N3, LTC2449_P4_N5, LTC2449_P6_N7,
                                        LTC2449_P8_N9, LTC2449_P10_N11, LTC2449_P12_N13, LTC2449_P14_N15,
                                        LTC2449_P1_N0, LTC2449_P3_N2, LTC2449_P5_N4, LTC2449_P7_N6,
                                        LTC2449_P9_N8, LTC2449_P11_N10, LTC2449_P13_N12, LTC2449_P15_N14
                                        };      //!< Build the command for differential mode

//! Lookup table to build the command for OSR
const uint16_t BUILD_OSR_COMMAND[10] = {LTC2449_OSR_32768, LTC2449_OSR_64, LTC2449_OSR_128, LTC2449_OSR_256, LTC2449_OSR_512,
                                        LTC2449_OSR_1024, LTC2449_OSR_2048, LTC2449_OSR_4096, LTC2449_OSR_8192, LTC2449_OSR_16384
                                       };       //!< Build the command for OSR

//! Lookup table to build 1X / 2X bits
const uint16_t BUILD_1X_2X_COMMAND[2] = {LTC2449_SPEED_1X, LTC2449_SPEED_2X};   //!< Build the command for 1x or 2x mode

//! MISO timeout constant
const uint16_t MISO_TIMEOUT = 1000;

//! Initialize Linduino
void setup()
{
  char demo_name[]="DC742";     // Demo Board Name stored in QuikEval EEPROM

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
  int16_t user_command;                 // The user input command
  uint8_t acknowledge = 0;
  if (demo_board_connected)
  {
    if (Serial.available())             // Check for user input
    {
      user_command = read_int();        // Read the user command
      if (user_command != 'm')
        Serial.println(user_command);   // Prints the user command to com port
      Serial.flush();
      switch (user_command)
      {
        case 1:
          acknowledge |= menu_1_read_single_ended();
          break;
        case 2:
          acknowledge |= menu_2_read_differential();
          break;
        case 3:
          acknowledge |= menu_3_calibrate();
          break;
        case 4:
          menu_4_set_OSR();
          break;
        case 5:
          menu_5_set_1X2X();
          break;
        case 6:
          menu_6_en_dis_cal();
          break;
        default:
          Serial.println("Incorrect Option");
      }
      if (acknowledge)
        Serial.println("***** SPI ERROR *****");
      Serial.print(F("\n*************************\n"));
      print_prompt();
    }
  }
}

// Function Definitions

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC742A Demonstration Program                                  *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send data and receive data   *\n"));
  Serial.print(F("* from the 24-bit ADC.                                          *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("\n1-Read Single-Ended\n"));
  Serial.print(F("2-Read Differential\n"));
  Serial.print(F("3-Calibration\n"));
  Serial.print(F("4-Oversample Ratio Settings\n"));
  Serial.print(F("5-2X Mode Settings\n"));
  Serial.print(F("6-Enable / Disable Calibration Key\n\n"));
  Serial.print(F("Enter a Command: "));
}

//! Display selected differential channels. Displaying single-ended channels is
//! straightforward; not so with differential because the inputs can take either polarity.
void print_user_command(uint8_t menu)
{
  switch (menu)
  {
    case 0:
      Serial.print("0P-1N");
      break;
    case 1:
      Serial.print("2P-3N");
      break;
    case 2:
      Serial.print("4P-5N");
      break;
    case 3:
      Serial.print("6P-7N");
      break;
    case 4:
      Serial.print("8P-9N");
      break;
    case 5:
      Serial.print("10P-11N");
      break;
    case 6:
      Serial.print("12P-13N");
      break;
    case 7:
      Serial.print("14P-15N");
      break;
    case 8:
      Serial.print("1P-0N");
      break;
    case 9:
      Serial.print("3P-2N");
      break;
    case 10:
      Serial.print("5P-4N");
      break;
    case 11:
      Serial.print("7P-6N");
      break;
    case 12:
      Serial.print("9P-8N");
      break;
    case 13:
      Serial.print("11P-10N");
      break;
    case 14:
      Serial.print("13P-12N");
      break;
    case 15:
      Serial.print("15P-14N");
      break;
  }
  Serial.print(": ");
}

//! Read channels in single-ended mode
//! @return 0 if successful, 1 is failure
int8_t menu_1_read_single_ended()
{
  uint16_t adc_command;     // The LTC2449 command word
  int16_t user_command;     // The user input command
  uint32_t adc_code = 0;    // The LTC2449 code
  float adc_voltage;        // The LTC2449 voltage

  while (1)
  {
    Serial.print(F("*************************\n\n"));
    Serial.print(F("0-CH0  8-CH8\n"));
    Serial.print(F("1-CH1  9-CH9\n"));
    Serial.print(F("2-CH2  10-CH10\n"));
    Serial.print(F("3-CH3  11-CH11\n"));
    Serial.print(F("4-CH4  12-CH12\n"));
    Serial.print(F("5-CH5  13-CH13\n"));
    Serial.print(F("6-CH6  14-CH14\n"));
    Serial.print(F("7-CH7  15-CH15\n"));
    Serial.print(F("16-ALL\n"));
    Serial.print(F("m-Main Menu\n"));
    Serial.print("Enter a Command: ");

    user_command = read_int();                          // Read the single command
    if (user_command == 'm')
      return(0);
    Serial.println(user_command);

    if (user_command == 16)
    {
      Serial.print(F("ALL\n"));
      adc_command = BUILD_COMMAND_SINGLE_ENDED[0] | OSR_mode | two_x_mode;  // Build ADC command for channel 0
      if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT)) // Checks for EOC with a timeout
        return(1);
      LTC2449_read(LTC2449_CS, adc_command, &adc_code); // Throws out last reading

      for (int8_t x = 0; x <= 15; x++)                  // Read all channels in single-ended mode
      {
        if (two_x_mode)
        {
          LTC2449_read(LTC2449_CS, adc_command, &adc_code); // Throws out an extra reading in 2x mode
          if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT)) // Checks for EOC with a timeout
            return(1);
        }
        adc_command = BUILD_COMMAND_SINGLE_ENDED[(x + 1) % 16] | OSR_mode | two_x_mode;
        if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT))  // Checks for EOC with a timeout
          return(1);
        LTC2449_read(LTC2449_CS, adc_command, &adc_code);
        adc_voltage = LTC2449_code_to_voltage(adc_code, LTC2449_lsb, LTC2449_offset_code);
        Serial.print("  ****");
        Serial.print("CH");
        Serial.print(x);
        Serial.print(": ");
        Serial.print(adc_voltage, 4);
        Serial.print(F("V\n\n"));
      }
    }
    else
    {
      adc_command = BUILD_COMMAND_SINGLE_ENDED[user_command] | OSR_mode | two_x_mode;
      Serial.print("\nADC Command: B");
      Serial.println(adc_command, BIN);
      if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT))    // Checks for EOC with a timeout
        return(1);
      LTC2449_read(LTC2449_CS, adc_command, &adc_code);     // Throws out last reading
      if (two_x_mode)
      {
        LTC2449_read(LTC2449_CS, adc_command, &adc_code);   // Throws out an extra reading in 2x mode
        if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT))  // Checks for EOC with a timeout
          return(1);
      }
      if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT))    // Checks for EOC with a timeout
        return(1);
      LTC2449_read(LTC2449_CS, adc_command, &adc_code);     // Now we're ready to read the desired data
      Serial.print("Received Code: 0x");
      Serial.println(adc_code, HEX);
      adc_voltage = LTC2449_code_to_voltage(adc_code, LTC2449_lsb, LTC2449_offset_code);
      Serial.print("  ****");
      Serial.print("CH");
      Serial.print(user_command);
      Serial.print(": ");
      Serial.print(adc_voltage, 4);
      Serial.print(F("V\n\n"));
    }
  }
  return(0);
}

//! Read channels in differential mode
//! @return 0 if successful, 1 is failure
int8_t menu_2_read_differential()
{
  int8_t y;                 // Offset into differential channel array to select polarity
  uint16_t adc_command;     // The LTC2449 command word
  int16_t user_command;     // The user input command
  uint32_t adc_code = 0;    // The LTC2449 code
  float adc_voltage;        // The LTC2449 voltage

  while (1)
  {
    Serial.print(F("\n*************************\n\n")); // Display  differential menu
    Serial.print(F("0-0P-1N     8-1P-0N\n"));
    Serial.print(F("1-2P-3N     9-3P-2N\n"));
    Serial.print(F("2-4P-5N     10-5P-4N\n"));
    Serial.print(F("3-6P-7N     11-7P-6N\n"));
    Serial.print(F("4-8P-9N     12-9P-8N\n"));
    Serial.print(F("5-10P-11N   13-11P-10N\n"));
    Serial.print(F("6-12P_13N   14-13P-12N\n"));
    Serial.print(F("7-14P-15N   15-15P-14N\n"));
    Serial.print(F("16-ALL Even_P-Odd_N\n"));
    Serial.print(F("17-ALL Odd_P-Even_N\n"));
    Serial.print(F("m-Main Menu\n"));

    user_command = read_int();                        // Read the single command
    if (user_command == 'm')
      return(0);
    Serial.println(user_command);
    if ((user_command == 16) || (user_command == 17))
    {
      if (user_command == 16)
      {
        Serial.print(F("ALL Even_P-Odd_N\n"));        // Cycles through options 0-7
        y = 0;
      }
      if (user_command == 17)
      {
        Serial.print(F("ALL Odd_P-Even_N\n"));        // Cycles through options 8-15
        y = 8;
      }

      adc_command = BUILD_COMMAND_DIFF[y] | OSR_mode | two_x_mode;  // Build the channel 0 and 1 for differential mode
      if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT))            // Checks for EOC with a timeout
        return(1);
      LTC2449_read(LTC2449_CS, adc_command, &adc_code);             // Throws out reading
      for (int8_t x = 0; x < 7; x++)                                // Read all channels in differential mode. All even channels are positive and odd channels are negative
      {
        if (two_x_mode)
        {
          LTC2449_read(LTC2449_CS, adc_command, &adc_code);         // Throws out an extra reading in 2x mode
          if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT))        // Checks for EOC with a timeout
            return(1);
        }
        adc_command = BUILD_COMMAND_DIFF[((x + 1) % 8) + y] | OSR_mode | two_x_mode;
        if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT))          // Checks for EOC with a timeout
          return(1);
        LTC2449_read(LTC2449_CS, adc_command, &adc_code);
        Serial.print("Received Code: 0x");
        Serial.println(adc_code, HEX);
        adc_voltage = LTC2449_code_to_voltage(adc_code, LTC2449_lsb, LTC2449_offset_code);
        Serial.print("\n  ****");
        print_user_command(x + y);
        Serial.print(": ");
        Serial.print(adc_voltage, 4);
        Serial.print(F("V\n"));
      }
    }
    else
    {
      // Reads and displays a selected channel
      adc_command = BUILD_COMMAND_DIFF[user_command] | OSR_mode | two_x_mode;
      Serial.print("ADC Command: B");
      Serial.println(adc_command, BIN);
      if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT))            // Checks for EOC with a timeout
        return(1);
      LTC2449_read(LTC2449_CS, adc_command, &adc_code);             // Throws out last reading
      if (two_x_mode)
      {
        if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT))          // Checks for EOC with a timeout
          return(1);
        LTC2449_read(LTC2449_CS, adc_command, &adc_code);           // Throws out an extra reading in 2x mode
      }
      if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT))            // Checks for EOC with a timeout
        return(1);
      LTC2449_read(LTC2449_CS, adc_command, &adc_code);
      Serial.print("Received Code: 0x");
      Serial.println(adc_code, HEX);
      adc_voltage = LTC2449_code_to_voltage(adc_code, LTC2449_lsb, LTC2449_offset_code);
      Serial.print("\n  ****");
      print_user_command(user_command);
      Serial.print(adc_voltage, 4);
      Serial.print(F("V\n"));
    }
  }
  return(0);
}

//! Calibrate ADC given two known inputs
//! @return 0 if successful, 1 is failure
int8_t menu_3_calibrate()
{
  uint16_t adc_command;     // The LTC2449 command word
  int16_t user_command;     // The user input command
  uint32_t adc_code = 0;    // The LTC2449 code
  float zero_voltage;       // Measured cal voltage
  float fs_voltage;         // Measured cal voltage
  uint32_t zero_code;       // Cal zero code
  uint32_t fs_code;         // Cal full scale code

  // Calibration
  Serial.println("Apply 100mV to CH0 with respect to COM");
  Serial.println("or apply a voltage for the lower point in two point calibration");
  Serial.print("Enter the measured input voltage:");
  zero_voltage = read_float();
  Serial.println(zero_voltage, 6);

  // Set OSR to 32768
  adc_command = BUILD_OSR_COMMAND[9];                  // Build OSR command
  if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT))    // Checks for EOC with a timeout
    return(1);
  LTC2449_read(LTC2449_CS, adc_command, &adc_code);     // Send OSR command

  adc_command = BUILD_COMMAND_SINGLE_ENDED[0] | LTC2449_OSR_32768 | LTC2449_SPEED_1X;   // Build ADC command byte for voltage input
  if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT))    // Checks for EOC with a timeout
    return(1);
  LTC2449_read(LTC2449_CS, adc_command, &adc_code);     // Throw away previous reading
  delay(100);
  if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT))    // Checks for EOC with a timeout
    return(1);
  LTC2449_read(LTC2449_CS, adc_command, &zero_code);    // Measure zero

  Serial.println("Apply ~2.40V input voltage to CH0.");
  Serial.println("Enter the measured input voltage:");

  fs_voltage = read_float();
  if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT))    // Checks for EOC with a timeout
    return(1);
  LTC2449_read(LTC2449_CS, adc_command, &adc_code);     // Throw away previous reading
  delay(100);
  if (LTC2449_EOC_timeout(LTC2449_CS, MISO_TIMEOUT))    // Checks for EOC with a timeout
    return(1);
  LTC2449_read(LTC2449_CS, adc_command, &fs_code);      // Measure full scale
  LTC2449_cal_voltage(zero_code, fs_code, zero_voltage, fs_voltage, &LTC2449_lsb, &LTC2449_offset_code);

  Serial.print("ADC offset : ");
  Serial.println(LTC2449_offset_code);
  Serial.print(" ADC lsb : ");
  Serial.print(LTC2449_lsb*1.0e9, 4);
  Serial.println("nV (32-bits)");
  store_calibration();
  return(0);
}

//! Set Oversample Ratio (OSR)
//! @return void
void menu_4_set_OSR()
{
  int16_t user_command; // The user input command

  // Oversample Ratio Settings
  Serial.print(F("Oversample Ratio Settings\n"));   // Display  OSR menu
  Serial.print(F("1-64\n"));
  Serial.print(F("2-128\n"));
  Serial.print(F("3-256\n"));
  Serial.print(F("4-512\n"));
  Serial.print(F("5-1024\n"));
  Serial.print(F("6-2048\n"));
  Serial.print(F("7-4096\n"));
  Serial.print(F("8-8192\n"));
  Serial.print(F("9-16384\n"));
  Serial.print(F("10-32768\n"));
  Serial.print("\nEnter a Command: ");

  user_command = read_int();
  Serial.println(user_command);

  OSR_mode = BUILD_OSR_COMMAND[user_command];       // Build OSR command

  Serial.print("\nADC OSR Command: B");
  Serial.println(OSR_mode, BIN);
}

//! Set 1X or 2X mode
//! @return void
void menu_5_set_1X2X()
{
  int16_t user_command; // The user input command

  // 2X Mode
  Serial.print(F("2X Mode Settings\n\n"));
  Serial.print(F("0-Disable\n"));
  Serial.print(F("1-Enable\n"));
  Serial.print(F("Enter a Command: "));
  user_command = read_int();
  Serial.println(user_command);

  if (user_command == 0)
  {
    two_x_mode = LTC2449_SPEED_1X;
    Serial.print(F("2X Mode Disabled\n"));
  }
  else
  {
    two_x_mode = LTC2449_SPEED_2X;
    Serial.print(F("2X Mode Enabled\n"));
  }
}

//! Store measured calibration parameters to nonvolatile EEPROM on demo board
//! @return void
void store_calibration()
// store the ADC calibration to the EEPROM
{
  eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, EEPROM_CAL_STATUS_ADDRESS);           // Cal key
  eeprom_write_int32(EEPROM_I2C_ADDRESS, LTC2449_offset_code, EEPROM_CAL_STATUS_ADDRESS+2);    // Offset
  eeprom_write_float(EEPROM_I2C_ADDRESS, LTC2449_lsb, EEPROM_CAL_STATUS_ADDRESS+6);            // LSB
  Serial.println("Calibration Stored to EEPROM");
}

//! Enable / Disable calibration without actually touching stored calibration values
//! @return void
void menu_6_en_dis_cal()
{
  int16_t user_command; // The user input command

  Serial.println(F("Enable / Disable calibration key"));
  Serial.println(F("0-Disable\n"));
  Serial.println(F("1-Enable - use ONLY if you know a calibration has been performed previously\n"));
  Serial.println(F("Enter a Command: "));
  user_command = read_int();

  if (user_command == 0)
  {
    eeprom_write_int16(EEPROM_I2C_ADDRESS, 0xFFFF, EEPROM_CAL_STATUS_ADDRESS);           // Reset cal key
    Serial.print(F("Calibration Disabled\n"));
  }
  else
  {
    eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, EEPROM_CAL_STATUS_ADDRESS);   // Set cal key
    Serial.print(F("Calibration Enabled\n"));
  }
  restore_calibration();
}

//! Read stored calibration parameters from nonvolatile EEPROM on demo board
//! @return return 1 if successful, 0 if not
int8_t restore_calibration()
// read the calibration from EEPROM
{
  int16_t cal_key;
  // read the cal key from the EEPROM
  eeprom_read_int16(EEPROM_I2C_ADDRESS, &cal_key, EEPROM_CAL_STATUS_ADDRESS);
  if (cal_key == EEPROM_CAL_KEY)
  {
    // calibration has been stored, read offset and lsb
    eeprom_read_int32(EEPROM_I2C_ADDRESS, &LTC2449_offset_code, EEPROM_CAL_STATUS_ADDRESS+2);  // offset
    eeprom_read_float(EEPROM_I2C_ADDRESS, &LTC2449_lsb, EEPROM_CAL_STATUS_ADDRESS+6);          // lsb
    Serial.println("Calibration Restored");
    return(1);
  }
  else
  {
    Serial.println("Calibration not found");
    return(0);
  }
}
