/*!
Linear Technology DC806A Demonstration Board.
LTC1867: 16-Bit, 8-Channel 200ksps ADC.

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. Equipment
   required is a precision voltage source and a precision voltmeter (to monitor
   voltage source). No external power supply is required. Ensure JP1 is installed in
   the default position from the factory.

  How to test Single-Ended mode:
   The voltage source should be connected to the ADC such that the negative lead is
   connected to the GND pin. The positive lead may be connected to any channel
   input. Ensure voltage is within analog input voltage range 0 to +4.096V.

  How to test Single-Ended mode with CH7 as COM:
   The voltage source should be connected to the ADC such that the negative lead is
   connected to both GND and the common (COM) pin. The positive lead may be
   connected to any channel inputs 0-6. Ensure voltage is within analog input
   voltage range 0 to +4.096V.

  How to test Differential Mode:
   Bipolar Mode:
     The voltage source should be connected with positive and negative leads to
     paired channels. The negative lead of the voltage source must also be connected
     to the GND pin in order to provide a ground-referenced voltage. Ensure voltage
     is within the range of 0 to +2.048V. Swapping input voltages results in a
     reversed polarity reading.

   Unipolar Mode:
     The voltage source should be connected with positive and negative leads to
     paired channels. The negative lead of the voltage source must also be connected
     to the GND pin in order to provide a ground-referenced voltage. Ensure voltage
     is within the range of 0 to +4.096V. Swapping input voltages results in a zero
     reading.

  How to calibrate:
   Apply 100mV to CH0 and connect CH1 to GND. Measure this voltage with a precise
   voltmeter and enter this value. (This takes the reading.) Now apply approximately
   4.00 volts to CH0. Measure this voltage with a precise voltmeter and enter this
   value. Short CH0 and CH1 to ground. Next, hit ENTER. (This takes the reading.)
   Calibration is now stored in EEPROM. Upon startup the calibration values will be
   restored.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC1867

http://www.linear.com/product/LTC1867#demoboards


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
    @ingroup LTC1867
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC1867.h"
#include <SPI.h>
#include <Wire.h>

// Function Declaration
void print_title();                                         // Print the title block
void print_prompt();                                        // Prompt the user for an input command
void print_user_command(uint8_t menu);                      // Display selected differential channels
void store_calibration();                                   // Store the ADC calibration to the EEPROM
int8_t restore_calibration();                               // Read the calibration from EEPROM Return 1 if successful, 0 if not

void menu_1_read_single_ended();
void menu_2_read_differential();
void menu_3_read_single_ended_com7();
void menu_4_calibrate();
void menu_5_sleep();
void menu_6_select_uni_bipolar();

// Global variables
static uint8_t demo_board_connected;                    //!< Set to 1 if the board is connected
static uint8_t uni_bi_polar = LTC1867_UNIPOLAR_MODE;    //!< The LTC1867 unipolar/bipolar mode selection
static float LTC1867_lsb = 6.25009537E-5;               //!< Ideal LSB voltage for a perfect part
static int32_t LTC1867_offset_unipolar_code = 0;        //!< Ideal unipolar offset for a perfect part
static int32_t LTC1867_offset_bipolar_code = 0;         //!< Ideal bipolar offset for a perfect part

// Constants

//! Lookup table to build the command for single-ended mode, input with respect to GND
const uint8_t BUILD_COMMAND_SINGLE_ENDED[8] = {LTC1867_CH0, LTC1867_CH1, LTC1867_CH2, LTC1867_CH3,
    LTC1867_CH4, LTC1867_CH5, LTC1867_CH6, LTC1867_CH7
                                              }; //!< Builds the command for single-ended mode, input with respect to GND

//! Lookup table to build the command for single-ended mode with channel 7 as common pin
const uint8_t BUILD_COMMAND_SINGLE_ENDED_COM7[7] = {LTC1867_CH0_7COM, LTC1867_CH1_7COM, LTC1867_CH2_7COM, LTC1867_CH3_7COM,
    LTC1867_CH4_7COM, LTC1867_CH5_7COM, LTC1867_CH6_7COM
                                                   };  //!< Builds the command for single-ended mode, input with respect to CH7

//! Lookup table to build the command for differential mode with the selected uni/bipolar mode
const uint8_t BUILD_COMMAND_DIFF[8] = {LTC1867_P0_N1, LTC1867_P2_N3, LTC1867_P4_N5, LTC1867_P6_N7,
                                       LTC1867_P1_N0, LTC1867_P3_N2, LTC1867_P5_N4, LTC1867_P7_N6
                                      }; //!< Build the command for differential mode

//! Initialize Linduino
void setup()
{
  char demo_name[]="DC806";      //!< Demo Board Name stored in QuikEval EEPROM
  uint16_t adc_code;

  quikeval_I2C_init();           // Configure the EEPROM I2C port for 100kHz
  quikeval_SPI_init();           // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();        // Connect SPI to main data port
  Serial.begin(115200);          // Initialize the serial port to the PC
  print_title();
  demo_board_connected = discover_demo_board(demo_name);
  if (demo_board_connected)
  {
    restore_calibration();
    LTC1867_read(LTC1867_CS, BUILD_COMMAND_SINGLE_ENDED[0], &adc_code); // Wakes up ADC if it was in sleep mode
    print_prompt();
  }
}

//! Repeats Linduino loop
void loop()
{
  uint16_t user_command;
  if (demo_board_connected)
  {
    if (Serial.available())
    {
      user_command = read_int();        // Read the user command
      if (user_command != 'm')
        Serial.println(user_command);   // Prints the user command to com port
      switch (user_command)
      {
        case 1:
          menu_1_read_single_ended();
          break;
        case 2:
          menu_2_read_differential();
          break;
        case 3:
          menu_3_read_single_ended_com7();
          break;
        case 4:
          menu_4_calibrate();
          break;
        case 5:
          menu_5_sleep();
          break;
        case 6:
          menu_6_select_uni_bipolar();
          break;
        default:
          Serial.println("Incorrect Option");
          break;
      }
      Serial.println();
      Serial.println(F("*************************"));
      print_prompt();
    }
  }
}

// Function Definitions

//! Read channels in single-ended mode
//! @return void
void menu_1_read_single_ended()
{
  uint8_t user_command;
  uint8_t adc_command;                             // The LTC1867 command byte
  uint16_t adc_code = 0;                           // The LTC1867 code
  float adc_voltage;                               // The LTC1867 voltage
  while (1)
  {
    if (uni_bi_polar == LTC1867_UNIPOLAR_MODE)
      Serial.println(F("Single-Ended, Unipolar mode:"));
    else
      Serial.println(F("Single-Ended, Bipolar mode:"));

    Serial.println(F("*************************"));            // Display single-ended menu
    Serial.println();
    Serial.println(F("0-CH0"));
    Serial.println(F("1-CH1"));
    Serial.println(F("2-CH2"));
    Serial.println(F("3-CH3"));
    Serial.println(F("4-CH4"));
    Serial.println(F("5-CH5"));
    Serial.println(F("6-CH6"));
    Serial.println(F("7-CH7"));
    Serial.println(F("8-ALL"));
    Serial.println(F("m-Main Menu"));
    Serial.print(F("Enter a Command: "));

    user_command = read_int();                                 // Read the single command
    if (user_command == 'm')
      return;

    Serial.println(user_command);

    if (user_command == 8)
    {
      Serial.println(F("ALL"));
      adc_command = BUILD_COMMAND_SINGLE_ENDED[0] | uni_bi_polar;   // Build ADC command for channel 0
      LTC1867_read(LTC1867_CS, adc_command, &adc_code);             // Throws out last reading
      delay(100);
      uint8_t x;                                                    //!< iteration variable
      for (x = 0; x <= 7; x++)                                      // Read all channels in single-ended mode
      {
        adc_command = BUILD_COMMAND_SINGLE_ENDED[(x + 1) % 8] | uni_bi_polar;
        LTC1867_read(LTC1867_CS, adc_command, &adc_code);
        if (uni_bi_polar == LTC1867_UNIPOLAR_MODE)
          adc_voltage = LTC1867_unipolar_code_to_voltage(adc_code, LTC1867_lsb, LTC1867_offset_unipolar_code);
        else
          adc_voltage = LTC1867_bipolar_code_to_voltage(adc_code, LTC1867_lsb, LTC1867_offset_bipolar_code);
        Serial.print(F("  ****"));
        Serial.print(F("CH"));
        Serial.print(x);
        Serial.print(F(": "));
        Serial.print(adc_voltage, 4);
        Serial.println(F("V"));
        Serial.println();
      }
    }
    else
    {
      adc_command = BUILD_COMMAND_SINGLE_ENDED[user_command] | uni_bi_polar;
      Serial.println();
      Serial.print(F("ADC Command: B"));
      Serial.println(adc_command, BIN);
      LTC1867_read(LTC1867_CS, adc_command, &adc_code); // Throws out last reading
      delay(100);
      LTC1867_read(LTC1867_CS, adc_command, &adc_code);
      Serial.print(F("Received Code: 0x"));
      Serial.println(adc_code, HEX);

      if (uni_bi_polar == LTC1867_UNIPOLAR_MODE)
        adc_voltage = LTC1867_unipolar_code_to_voltage(adc_code, LTC1867_lsb, LTC1867_offset_unipolar_code);
      else
        adc_voltage = LTC1867_bipolar_code_to_voltage(adc_code, LTC1867_lsb, LTC1867_offset_bipolar_code);
      Serial.print(F("  ****"));
      Serial.print(F("CH"));
      Serial.print(user_command);
      Serial.print(F(": "));
      Serial.print(adc_voltage, 4);
      Serial.println(F("V"));
      Serial.println();
    }
  }
}

//! Read channels in differential mode
//! @return void
void menu_2_read_differential()
{
  uint8_t user_command;
  uint8_t adc_command;                             // The LTC1867 command byte
  uint16_t adc_code = 0;                           // The LTC1867 code
  float adc_voltage;                               // The LTC1867 voltage
  while (1)
  {
    if (uni_bi_polar == LTC1867_UNIPOLAR_MODE)
      Serial.println(F("Differential, Unipolar mode:"));
    else
      Serial.println(F("Differential, Bipolar mode:"));

    Serial.println(F("*************************"));
    Serial.println(F("0-0P-1N"));
    Serial.println(F("1-2P-3N"));
    Serial.println(F("2-4P-5N"));
    Serial.println(F("3-6P-7N"));
    Serial.println(F("4-1P-0N"));
    Serial.println(F("5-3P-2N"));
    Serial.println(F("6-5P_4N"));
    Serial.println(F("7-7P-6N"));
    Serial.println(F("8-ALL Even_P-Odd_N"));
    Serial.println(F("9-ALL Odd_P-Even_N"));
    Serial.println(F("m-Main Menu"));
    Serial.println(F("Enter a Command: "));

    user_command = read_int();  // Read the channel for differential bipolar mode

    if (user_command == 'm')
      return;
    Serial.println(user_command);
    int8_t y;                   //!< Variable to add to argument to BUILD_COMMAND_DIFF to select polarity.

    if ((user_command == 8) | (user_command == 9))
    {
      if (user_command == 8)    // Cycles through options 0-3
      {
        Serial.println(F("ALL Even_P-Odd_N"));
        y = 0;
      }
      else
      {
        Serial.println(F("ALL Odd_P-Even_N"));
        y = 4;
      }

      adc_command = BUILD_COMMAND_DIFF[y] | uni_bi_polar;   // Build ADC command for first channel in bipolar mode
      LTC1867_read(LTC1867_CS, adc_command, &adc_code);     // Throws out last reading
      delay(100);
      uint8_t x;                                            //!< iteration variable
      for (x = 0; x <= 3; x++)                              // Read all channels in bipolar mode. All even channels are positive and odd channels are negative
      {
        adc_command = BUILD_COMMAND_DIFF[((x + 1) % 4) + y] | uni_bi_polar;
        LTC1867_read(LTC1867_CS, adc_command, &adc_code);
        if (uni_bi_polar == LTC1867_UNIPOLAR_MODE)
          adc_voltage = LTC1867_unipolar_code_to_voltage(adc_code, LTC1867_lsb, LTC1867_offset_unipolar_code);
        else
          adc_voltage = LTC1867_bipolar_code_to_voltage(adc_code, LTC1867_lsb, LTC1867_offset_bipolar_code);
        Serial.println();
        Serial.print(F("  ****"));
        print_user_command(x + y);
        Serial.print(F(": "));
        Serial.print(adc_voltage, 4);
        Serial.println(F("V"));
      }
    }
    else
    {
      // Read and display a selected channel
      adc_command = BUILD_COMMAND_DIFF[user_command] | uni_bi_polar;
      Serial.print(F("ADC Command: B"));
      Serial.println(adc_command, BIN);
      LTC1867_read(LTC1867_CS, adc_command, &adc_code); // Throws out last reading
      delay(100);
      LTC1867_read(LTC1867_CS, adc_command, &adc_code);
      Serial.print(F("Received Code: 0x"));
      Serial.println(adc_code, HEX);
      if (uni_bi_polar == LTC1867_UNIPOLAR_MODE)
        adc_voltage = LTC1867_unipolar_code_to_voltage(adc_code, LTC1867_lsb, LTC1867_offset_unipolar_code);
      else
        adc_voltage = LTC1867_bipolar_code_to_voltage(adc_code, LTC1867_lsb, LTC1867_offset_bipolar_code);
      Serial.println();
      Serial.print(F("  ****"));
      print_user_command(user_command);
      Serial.print(adc_voltage, 4);
      Serial.println(F("V"));
    }
  }
}


//! Read Channels in Single-Ended mode with Ch7 as COM
//! @return void
void menu_3_read_single_ended_com7()
{
  uint8_t user_command;
  uint8_t adc_command;                             // The LTC1867 command byte
  uint16_t adc_code = 0;                           // The LTC1867 code
  float adc_voltage;                               // The LTC1867 voltage
  while (1)
  {
    if (uni_bi_polar == LTC1867_UNIPOLAR_MODE)
      Serial.println(F("Single-Ended (COM=CH7), Unipolar mode:"));
    else
      Serial.println(F("Single-Ended (COM=CH7), Bipolar mode:"));

    Serial.println();                              // Display single-ended menu
    Serial.println(F("*************************"));
    Serial.println();
    Serial.println(F("CH7 as COM\n"));
    Serial.println(F("0-CH0"));
    Serial.println(F("1-CH1"));
    Serial.println(F("2-CH2"));
    Serial.println(F("3-CH3"));
    Serial.println(F("4-CH4"));
    Serial.println(F("5-CH5"));
    Serial.println(F("6-CH6"));
    Serial.println(F("7-ALL"));
    Serial.println(F("m-Main Menu"));
    Serial.print(F("Enter a Command: "));

    user_command = read_int();                     // Read the single command
    if (user_command == 'm')
      return;

    Serial.println(user_command);
    Serial.println();

    if (user_command == 7)
    {
      Serial.println(F("ALL with CH7 as COM"));

      adc_command = BUILD_COMMAND_SINGLE_ENDED_COM7[0] | uni_bi_polar;  // Build ADC command for channel 0 with channel 7 as a common pin
      LTC1867_read(LTC1867_CS, adc_command, &adc_code);                 // Throws out last reading
      delay(100);
      uint8_t x;                                                        //!< iteration variable
      for (x = 0; x <= 6; x++)                                          // Read all channels in single-ended mode with channel 7 as a common pin
      {
        adc_command = BUILD_COMMAND_SINGLE_ENDED_COM7[(x + 1) % 7] | uni_bi_polar;
        LTC1867_read(LTC1867_CS, adc_command, &adc_code);
        if (uni_bi_polar == LTC1867_UNIPOLAR_MODE)
          adc_voltage = LTC1867_unipolar_code_to_voltage(adc_code, LTC1867_lsb, LTC1867_offset_bipolar_code);
        else
          adc_voltage = LTC1867_bipolar_code_to_voltage(adc_code, LTC1867_lsb, LTC1867_offset_bipolar_code);
        Serial.println();
        Serial.print(F("  ****"));
        Serial.print(F("CH"));
        Serial.print(x);
        Serial.print(F(" with CH7 as COM: "));
        Serial.print(adc_voltage, 4);
        Serial.println(F("V"));
      }
    }
    else
    {
      adc_command = BUILD_COMMAND_SINGLE_ENDED_COM7[user_command]| uni_bi_polar;
      Serial.println();
      Serial.print(F("ADC Command: B"));
      Serial.println(adc_command, BIN);
      LTC1867_read(LTC1867_CS, adc_command, &adc_code);                 // Throws out last reading
      delay(100);
      LTC1867_read(LTC1867_CS, adc_command, &adc_code);
      Serial.print(F("Received Code: 0x"));
      Serial.println(adc_code, HEX);
      if (uni_bi_polar == LTC1867_UNIPOLAR_MODE)
        adc_voltage = LTC1867_unipolar_code_to_voltage(adc_code, LTC1867_lsb, LTC1867_offset_bipolar_code);
      else
        adc_voltage = LTC1867_bipolar_code_to_voltage(adc_code, LTC1867_lsb, LTC1867_offset_bipolar_code);
      Serial.println();
      Serial.print(F("  ****"));
      Serial.print(F("CH"));
      Serial.print(user_command);
      Serial.print(F(": "));
      Serial.print(adc_voltage, 4);
      Serial.println(F("V"));
    }
  }
}

//! Calibrate ADC given two known inputs
//! @ return void
void menu_4_calibrate()
{
  uint8_t user_command;
  uint8_t adc_command;                             // The LTC1867 command byte
  float fs_voltage;                                // Measured cal voltage
  float zero_voltage = 0.0;                        // Zero Voltage
  uint16_t zero_bipolar_code;                      // Cal zero code
  uint16_t zero_unipolar_code;                     // Cal zero code
  uint16_t fs_code;                                // Cal full scale code

  // Used to wake up the ADC if it is in sleep mode.
  LTC1867_read(LTC1867_CS, BUILD_COMMAND_SINGLE_ENDED[0], &zero_unipolar_code);
  delay(500);

  // Calibration
  // Accuracy: +- 2 lsb between channels
  Serial.println(F("Apply 100mV to CH0 and connect CH1 to GND."));
  Serial.println(F("Enter the measured input voltage for CH0:"));
  zero_voltage = read_float();
  Serial.println(zero_voltage, 8);

  adc_command = BUILD_COMMAND_SINGLE_ENDED[0]| LTC1867_UNIPOLAR_MODE;   // Build ADC command byte for voltage input
  LTC1867_read(LTC1867_CS, adc_command, &zero_unipolar_code);           // Throw away previous reading
  delay(200);
  LTC1867_read(LTC1867_CS, adc_command, &zero_unipolar_code);           // Measure zero

  adc_command = BUILD_COMMAND_DIFF[0] | LTC1867_BIPOLAR_MODE;           // Build ADC command byte for CH0 and CH1
  LTC1867_read(LTC1867_CS, adc_command, &zero_bipolar_code);            // Throw away previous reading
  delay(200);
  LTC1867_read(LTC1867_CS, adc_command, &zero_bipolar_code);            // Measure zero

  Serial.println(F("Apply ~4.00V input voltage to CH0."));
  Serial.println(F("Enter the measured input voltage:"));
  fs_voltage = read_float();
  Serial.println(fs_voltage, 8);

  adc_command = BUILD_COMMAND_SINGLE_ENDED[0] | LTC1867_UNIPOLAR_MODE;  // Build ADC command byte for voltage input
  LTC1867_read(LTC1867_CS, adc_command, &fs_code);                      // Throw away previous reading
  delay(200);
  LTC1867_read(LTC1867_CS, adc_command, &fs_code);                      // Measure full scale

  LTC1867_cal_voltage(zero_unipolar_code, zero_bipolar_code, fs_code, zero_voltage, fs_voltage, &LTC1867_lsb, &LTC1867_offset_unipolar_code, &LTC1867_offset_bipolar_code);

  Serial.print(F("ADC unipolar offset : "));
  Serial.println(LTC1867_offset_unipolar_code);
  Serial.print(F("ADC bipolar offset : "));
  Serial.println(LTC1867_offset_bipolar_code);
  Serial.print(F("ADC lsb : "));
  Serial.print(LTC1867_lsb*1.0e9, 4);
  Serial.println(F("nV (32-bits)"));
  store_calibration();
}

//! Put LTC1867 to sleep (low power)
//! @return void
void menu_5_sleep()
{
  // Sleep Mode
  uint16_t user_command;
  uint16_t adc_code = 0;                                        // The LTC1867 code
  LTC1867_read(LTC1867_CS, LTC1867_SLEEP_MODE, &adc_code);      // Build ADC command for sleep mode
  Serial.println();
  Serial.print(F("ADC Command: B"));
  Serial.println(LTC1867_SLEEP_MODE, BIN);
  Serial.println(F("LTC1867 Is Now In Sleep Mode"));
  Serial.println(F("Enter RETURN to exit Sleep Mode"));
  user_command = read_int();
  LTC1867_read(LTC1867_CS, LTC1867_EXIT_SLEEP_MODE, &adc_code); // Exit Sleep Mode
}

//! Select unipolar (0-REFCOMP) or bipolar (+/- 0.5 x REFCOMP) mode
//! @return void
void menu_6_select_uni_bipolar()
{
  uint8_t user_command;
  Serial.println(F("\n0-Bipolar, 1=Unipolar"));
  Serial.println(F("Enter a Command: "));

  user_command = read_int();    // Read user input for uni_bi_polar

  if (user_command == 1)
    uni_bi_polar = LTC1867_UNIPOLAR_MODE;
  else
    uni_bi_polar = LTC1867_BIPOLAR_MODE;
}

//! Store measured calibration parameters to nonvolatile EEPROM on demo board
//! @return void
void store_calibration()
// Store the ADC calibration to the EEPROM
{
  eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, EEPROM_CAL_STATUS_ADDRESS);                    // cal key
  eeprom_write_int32(EEPROM_I2C_ADDRESS, LTC1867_offset_unipolar_code, EEPROM_CAL_STATUS_ADDRESS+2);    // offset
  eeprom_write_int32(EEPROM_I2C_ADDRESS, LTC1867_offset_bipolar_code, EEPROM_CAL_STATUS_ADDRESS+6);     // offset
  eeprom_write_float(EEPROM_I2C_ADDRESS, LTC1867_lsb, EEPROM_CAL_STATUS_ADDRESS+10);                    // lsb
  Serial.println(F("Calibration Stored to EEPROM"));
}

//! Read stored calibration parameters from nonvolatile EEPROM on demo board
//! @return Return 1 if successful, 0 if not
int8_t restore_calibration()
// Read the calibration from EEPROM
// Return 1 if successful, 0 if not
{
  int16_t cal_key;
  // read the cal key from the EEPROM
  eeprom_read_int16(EEPROM_I2C_ADDRESS, &cal_key, EEPROM_CAL_STATUS_ADDRESS);
  if (cal_key == EEPROM_CAL_KEY)
  {
    // Calibration has been stored, read offset and lsb
    eeprom_read_int32(EEPROM_I2C_ADDRESS, &LTC1867_offset_unipolar_code, EEPROM_CAL_STATUS_ADDRESS+2);    // offset
    eeprom_read_int32(EEPROM_I2C_ADDRESS, &LTC1867_offset_bipolar_code, EEPROM_CAL_STATUS_ADDRESS+6);     // offset
    eeprom_read_float(EEPROM_I2C_ADDRESS, &LTC1867_lsb, EEPROM_CAL_STATUS_ADDRESS+10);                    // lsb
    Serial.println(F("Calibration Restored"));
    return(1);
  }
  else
  {
    Serial.println(F("Calibration not found"));
    return(0);
  }
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC806A Demonstration Program                                  *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data and receive data   *"));
  Serial.println(F("* from the 16-bit ADC.                                          *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.println();
  Serial.println(F("1-Read Single-Ended "));
  Serial.println(F("2-Read Differential"));
  Serial.println(F("3-Read Single-Ended with Ch7 as COM"));
  Serial.println(F("4-Calibration"));
  Serial.println(F("5-Sleep Mode"));
  Serial.println(F("6-Select Unipolar / Bipolar\n"));
  Serial.println(F("Selected Uni / Bip mode:"));
  if (uni_bi_polar == LTC1867_UNIPOLAR_MODE)
    Serial.println(F("Unipolar"));
  else
    Serial.println(F("Bipolar"));
  Serial.println();
  Serial.print(F("Enter a command:"));
}

//! Display selected differential channels. Displaying single-ended channels is
//! straightforward; not so with differential because the inputs can take either polarity.
void print_user_command(uint8_t menu)
{
  switch (menu)
  {
    case 0:
      Serial.print(F("0P-1N"));
      break;
    case 1:
      Serial.print(F("2P-3N"));
      break;
    case 2:
      Serial.print(F("4P-5N"));
      break;
    case 3:
      Serial.print(F("6P-7N"));
      break;
    case 4:
      Serial.print(F("1P-0N"));
      break;
    case 5:
      Serial.print(F("3P-2N"));
      break;
    case 6:
      Serial.print(F("5P-4N"));
      break;
    case 7:
      Serial.print(F("7P-6N"));
      break;
  }
  Serial.print(F(": "));
}