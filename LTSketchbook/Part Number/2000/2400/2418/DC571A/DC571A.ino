/*!
Linear Technology DC571A Demonstration Board.
LTC2418: 24-bit No Latency Delta Sigma 16-channel ADC

@verbatim

NOTES
  Setup:
    Set the terminal baud rate to 115200 and select the newline terminator.
    Equipment required is a precision voltage source and a precision
    voltmeter. No external power supply is required. Ensure all jumpers on
    the demo board are installed in their default positions from the
    factory.

  How to test Single-Ended mode:
    The voltage source should be connected to the ADC such that the negative
    lead is connected to ground. The positive lead may be connected to any
    channel input. Ensure voltage is within analog input voltage range -0.3V to
    +2.5V.

  How to test Differential Mode:
    The voltage source should be connected with positive and negative leads to
    paired channels. The voltage source negative output must also be connected to
    the COM pin in order to provide a ground-referenced voltage. Ensure voltage is
    within analog input voltage range -0.3V to +2.5V. Swapping input voltages
    results in a reversed polarity reading.

  How to calibrate:
    Apply 100mV to CH0 with respect to COM pin. Next, Measure this voltage with a
    precise voltmeter and enter this value. (This takes the reading.) Apply a higher
    voltage than the first voltage or approximately 2.40 volts to CH0. Anything
    above 2.40 is not reccomened. It is very likely to reach the ADC's full-scale
    and poduce invalide lsb and offset. Measure this voltage with a precise
    voltmeter and enter this value. Calibration is now stored in EEPROM. Upon
    startup the calibration values will be restored.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2418

http://www.linear.com/product/LTC2418#demoboards


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
    @ingroup LTC2418
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LT_I2C.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC2418.h"
#include <SPI.h>
#include <Wire.h>

// Function Declaration
void print_title();                                 // Print the title block
void print_prompt();                                // Prompt the user for an input command
void print_user_command(uint8_t menu);              // Display selected differential channels
void store_calibration();                           // store the ADC calibration to the EEPROM
int8_t restore_calibration();                       // read the calibration from EEPROM return 1 if successful, 0 if not

int8_t menu_1_read_single_ended();
int8_t menu_2_read_differential();
int8_t menu_3_calibrate();

// Global variables
static uint8_t demo_board_connected;                //!< Set to 1 if the board is connected
static float LTC2418_lsb = 5.9604652E-7;            //!< Ideal LSB voltage for a perfect part
static int32_t LTC2418_offset_code = 0;             //!< Ideal offset for a perfect part


//Constants
//! Lookup table to build the command for single-ended mode channels
const uint8_t BUILD_COMMAND_SINGLE_ENDED[16] = {LTC2418_CH0, LTC2418_CH1, LTC2418_CH2, LTC2418_CH3,
    LTC2418_CH4, LTC2418_CH5, LTC2418_CH6, LTC2418_CH7,
    LTC2418_CH8, LTC2418_CH9, LTC2418_CH10, LTC2418_CH11,
    LTC2418_CH12, LTC2418_CH13, LTC2418_CH14, LTC2418_CH15
                                               };   //!< Builds the command for single-ended mode

//! Lookup table to build the command for differential mode channels
const uint8_t BUILD_COMMAND_DIFF[16] = {LTC2418_P0_N1, LTC2418_P2_N3, LTC2418_P4_N5, LTC2418_P6_N7,
                                        LTC2418_P8_N9, LTC2418_P10_N11, LTC2418_P12_N13, LTC2418_P14_N15,
                                        LTC2418_P1_N0, LTC2418_P3_N2, LTC2418_P5_N4, LTC2418_P7_N6,
                                        LTC2418_P9_N8, LTC2418_P11_N10, LTC2418_P13_N12, LTC2418_P15_N14
                                       };           //!< Build the command for differential

const uint16_t MISO_TIMEOUT = 1000;                 //!< The MISO timeout (ms)

//! Initialize Linduino
void setup()
{
  char demo_name[6]="DC571";        // Demo Board Name stored in QuikEval EEPROM
  quikeval_SPI_init();              // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();           // Connect SPI to main data port
  quikeval_I2C_init();              // Configure the EEPROM I2C port for 100kHz
  Serial.begin(115200);             // Initialize the serial port to the PC
  print_title();
  demo_board_connected = discover_demo_board(demo_name);
  if (!(demo_board_connected))      // Resolves EEPROM string variation
  {
    Serial.print(F("Checking for string variation\n"));
    demo_name[0]='D';
    demo_name[1]='C';
    demo_name[2]='5';
    demo_name[3]='7';
    demo_name[4]='1';
    demo_name[5]='-';
    demo_board_connected = discover_demo_board(demo_name);
  }
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
        default:
          Serial.println("Incorrect Option");
          break;
      }
      if (acknowledge)
        Serial.println(F("***** SPI ERROR ******"));
      Serial.print(F("\n*************************\n"));
      print_prompt();
    }
  }
}

// Function Definitions

//! Read channels in single-ended mode
//! @return 0 when m is entered into menu, 1 if timeout for EOC
int8_t menu_1_read_single_ended()
{
  uint8_t  adc_command; // The LTC2418 command byte
  int16_t user_command; // The user input command
  uint32_t adc_code;    // The LTC2418 code
  float adc_voltage;    // The LTC2418 voltage

  while (1)
  {
    Serial.print(F("*************************\n\n"));      // Display single-ended menu
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
    Serial.print(F("\nEnter a Command: "));
    user_command = read_int();                              // Read the single command
    if (user_command == 'm')
      return(0);
    else
      Serial.println(user_command);
    Serial.println();

    if (user_command == 16)
    {
      Serial.print(F("ALL\n"));
      adc_command = BUILD_COMMAND_SINGLE_ENDED[0];          // Build ADC command for channel 0

      if (LTC2418_EOC_timeout(LTC2418_CS, MISO_TIMEOUT))     // Check for EOC
        return(1);
      LTC2418_read(LTC2418_CS, adc_command, &adc_code);     // Throws out last reading

      for (int8_t x = 0; x < 15; x++)
      {
        adc_command = BUILD_COMMAND_SINGLE_ENDED[(x + 1) % 16]; // Read all channels in single-ended mode
        if (LTC2418_EOC_timeout(LTC2418_CS, MISO_TIMEOUT))       // Check for EOC
          return(1);
        LTC2418_read(LTC2418_CS, adc_command, &adc_code);
        adc_voltage = LTC2418_code_to_voltage(adc_code, LTC2418_lsb , LTC2418_offset_code);
        Serial.print(F("  ****"));
        Serial.print(F("CH"));
        Serial.print(x);
        Serial.print(F(": "));
        Serial.print(adc_voltage, 4);
        Serial.print(F("V\n"));
      }
    }
    else if (user_command != 'm')                           // Read selected channel
    {
      adc_command = BUILD_COMMAND_SINGLE_ENDED[user_command];
      Serial.print(F("\nADC Command: B"));
      Serial.println(adc_command, BIN);
      if (LTC2418_EOC_timeout(LTC2418_CS, MISO_TIMEOUT))     // Check for EOC
        return(1);
      LTC2418_read(LTC2418_CS, adc_command, &adc_code);     // Throws out last reading
      if (LTC2418_EOC_timeout(LTC2418_CS, MISO_TIMEOUT))     // Check for EOC
        return(1);
      LTC2418_read(LTC2418_CS, adc_command, &adc_code);
      Serial.print(F("Received Code: 0x"));
      Serial.println(adc_code, HEX);
      adc_voltage = LTC2418_code_to_voltage(adc_code, LTC2418_lsb , LTC2418_offset_code);
      Serial.print(F("  ****"));
      Serial.print(F("CH"));
      Serial.print(user_command);
      Serial.print(F(": "));
      Serial.print(adc_voltage, 4);
      Serial.print(F("V\n"));
    }
  }
}

//! Read channels in differential mode
//! @return 0 when m is entered into menu, 1 if timeout for EOC
int8_t menu_2_read_differential()
{
  int8_t y;             // Offset into differential channel array to select polarity
  uint8_t  adc_command; // The LTC2418 command byte
  int16_t user_command; // The user input command
  uint32_t adc_code;    // The LTC2418 code
  float adc_voltage;    // The LTC2418 voltage

  while (1)
  {
    // Display differential menu
    Serial.print(F("\n*************************\n\n"));
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
    Serial.print(F("\nEnter a Command: "));
    user_command = read_int();
    if (user_command == 'm')
      return(0);
    Serial.println(user_command);
    Serial.println();

    if ((user_command == 16) || (user_command == 17))
    {
      if (user_command == 16)
      {
        Serial.print(F("ALL Even_P-Odd_N\n"));              // Cycles through options 0-7
        y = 0;
      }
      if (user_command == 17)
      {
        Serial.print(F("ALL Odd_P-Even_N\n"));              // Cycles through options 8-15
        y = 8;
      }
      adc_command = BUILD_COMMAND_DIFF[y];                  // Set up first channel
      if (LTC2418_EOC_timeout(LTC2418_CS, MISO_TIMEOUT))     // check for EOC
        return(1);
      LTC2418_read(LTC2418_CS, adc_command, &adc_code);     // Throws out last reading
      for (int8_t x = 0; x <= 7; x++)                       // Read all channels. All even channels are positive and odd channels are negative
      {
        adc_command = BUILD_COMMAND_DIFF[((x+1) % 8) + y];
        if (LTC2418_EOC_timeout(LTC2418_CS, MISO_TIMEOUT))   // Check for EOC
          return(1);
        LTC2418_read(LTC2418_CS, adc_command, &adc_code);
        adc_voltage = LTC2418_code_to_voltage(adc_code, LTC2418_lsb , LTC2418_offset_code);
        Serial.println();
        Serial.print(F("  ****"));
        print_user_command(x + y);
        Serial.print(F(": "));
        Serial.print(adc_voltage, 4);
        Serial.print(F("V\n"));
      }
    }
    else                                                    // Read selected channels
    {
      // Reads and displays a selected channel
      adc_command = BUILD_COMMAND_DIFF[user_command];
      Serial.print(F("ADC Command: B"));
      Serial.println(adc_command, BIN);
      if (LTC2418_EOC_timeout(LTC2418_CS, MISO_TIMEOUT))     // Check for EOC
        return(1);
      LTC2418_read(LTC2418_CS, adc_command, &adc_code);     // Throws out last reading
      if (LTC2418_EOC_timeout(LTC2418_CS, MISO_TIMEOUT))     // Check for EOC
        return(1);
      LTC2418_read(LTC2418_CS, adc_command, &adc_code);
      Serial.print(F("Received Code: 0x"));
      Serial.println(adc_code, HEX);
      adc_voltage = LTC2418_code_to_voltage(adc_code, LTC2418_lsb , LTC2418_offset_code);
      Serial.println();
      Serial.print(F("  ****"));
      Serial.print(F("CH"));
      Serial.print(user_command);
      Serial.print(F(": "));
      Serial.print(adc_voltage, 4);
      Serial.print(F("V"));
      Serial.println();
    }
  }
}

//! Calibrate ADC given two known inputs
//! @return 0 if successful, 1 if timeout for EOC
int8_t menu_3_calibrate()
{
  // Calibration
  float fs_voltage;     // Measured full-scale voltage
  float zero_voltage;   // Measured zero voltage
  uint32_t zero_code;   // Cal zero code
  uint32_t fs_code;     // Cal full scale code
  uint8_t  adc_command; // The LTC2418 command byte
  uint32_t adc_code;    // The LTC2418 code

  Serial.println("Apply  100mV to CH0 with respect to COM");
  Serial.println("or apply a voltage greater than GND for the");
  Serial.println("lower point in two point calibration");
  Serial.print("Enter the measured input voltage:");
  zero_voltage = read_float();
  Serial.println(zero_voltage,4);
  adc_command = BUILD_COMMAND_SINGLE_ENDED[0];          // Build ADC command byte for voltage input
  if (LTC2418_EOC_timeout(LTC2418_CS, MISO_TIMEOUT))     // Check for EOC
    return(1);
  LTC2418_read(LTC2418_CS, adc_command, &adc_code);     // Throw away previous reading
  delay(100);
  if (LTC2418_EOC_timeout(LTC2418_CS, MISO_TIMEOUT))     // Check for EOC
    return(1);
  LTC2418_read(LTC2418_CS, adc_command, &zero_code);    // Measure zero

  Serial.println("Apply ~2.40V input voltage to CH0");
  Serial.println("or apply a voltage greater that first");
  Serial.println("voltage and less than 2.40V.");
  Serial.print("Enter the measured input voltage: ");

  fs_voltage = read_float();
  Serial.println(fs_voltage, 4);
  adc_command = BUILD_COMMAND_SINGLE_ENDED[0];          // Build ADC command byte for voltage input
  if (LTC2418_EOC_timeout(LTC2418_CS, MISO_TIMEOUT))     // Check for EOC
    return(1);
  LTC2418_read(LTC2418_CS, adc_command, &adc_code);     // Throw away previous reading
  delay(100);
  if (LTC2418_EOC_timeout(LTC2418_CS, MISO_TIMEOUT))     // Check for EOC
    return(1);
  LTC2418_read(LTC2418_CS, adc_command, &fs_code);      // Measure full scale
  LTC2418_cal_voltage(zero_code, fs_code, zero_voltage, fs_voltage, &LTC2418_lsb , &LTC2418_offset_code);

  Serial.print("ADC offset : ");
  Serial.println(LTC2418_offset_code);
  Serial.print("ADC lsb : ");
  Serial.print(LTC2418_lsb*1.0e9, 4);
  Serial.println("nV (32-bits)");
  store_calibration();
  return(0);
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC571A Demonstration Program                                  *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send data and receive data   *\n"));
  Serial.print(F("* from the 24-bit ADC.                                          *\n"));
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
  Serial.println();
  Serial.print(F("Enter a command:"));
}


//! Display selected differential channels. Displaying Single-Ended channels is
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

//! Store measured calibration parameters to nonvolatile EEPROM on demo board
void store_calibration()
// store the ADC calibration to the EEPROM
{
  eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, EEPROM_CAL_STATUS_ADDRESS);          // Cal key
  eeprom_write_int32(EEPROM_I2C_ADDRESS, LTC2418_offset_code, EEPROM_CAL_STATUS_ADDRESS+2);   // Offset
  eeprom_write_float(EEPROM_I2C_ADDRESS, LTC2418_lsb, EEPROM_CAL_STATUS_ADDRESS+6);           // LSB
  Serial.println("Calibration Stored to EEPROM");
}

//! Read stored calibration parameters from nonvolatile EEPROM on demo board
//! @return return 1 if successful, 0 if not
int8_t restore_calibration()
{
  int16_t cal_key;
  // read the cal key from the EEPROM
  eeprom_read_int16(EEPROM_I2C_ADDRESS, &cal_key, EEPROM_CAL_STATUS_ADDRESS);
  if (cal_key == EEPROM_CAL_KEY)
  {
    // calibration has been stored, read offset and lsb
    eeprom_read_int32(EEPROM_I2C_ADDRESS, &LTC2418_offset_code, EEPROM_CAL_STATUS_ADDRESS+2);  // Offset
    eeprom_read_float(EEPROM_I2C_ADDRESS, &LTC2418_lsb, EEPROM_CAL_STATUS_ADDRESS+6);          // LSB
    Serial.println("Calibration Restored");
    return(1);
  }
  else
  {
    Serial.println("Calibration not found");
    return(0);
  }
}