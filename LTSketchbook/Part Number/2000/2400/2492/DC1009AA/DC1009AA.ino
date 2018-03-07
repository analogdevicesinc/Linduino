/*!
Linear Technology DC1009A-A Demonstration Board.
LTC2492: 24-Bit, 4-Channel Delta Sigma ADC with SPI interface

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. Equipment
   required is a precision voltage source and a precision voltmeter. Ensure the COM
   and REF- pins are connected to ground. The REF+ pin should be connected to +5V.

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

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2492

http://www.linear.com/product/LTC2492#demoboards


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
    @ingroup LTC2492
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include <SPI.h>
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC24XX_general.h"
#include "LTC2492.h"

// Function Declaration
void print_title();                             // Print the title block
void print_prompt();                            // Prompt the user for an input command
void print_user_command(uint8_t menu);          // Display selected differential channels

uint8_t menu_1_read_single_ended();
uint8_t menu_2_read_differential();
void menu_3_set_1X2X();

// Global variables
static uint8_t demo_board_connected;                //!< Set to 1 if the board is connected
static uint8_t two_x_mode = LTC2492_SPEED_1X;       //!< The LTC2492 2X Mode settings
static uint8_t rejection_mode = LTC2492_R50_R60;    //!< The LTC2492 rejection mode settings
static float LTC2492_vref = 5.0;                    //!< The LTC2492 ideal reference voltage
static uint16_t eoc_timeout = 350;                  //!< timeout in ms
// Constants

//! Lookup table to build the command for single-ended mode
const uint8_t BUILD_COMMAND_SINGLE_ENDED[4] = {LTC2492_CH0, LTC2492_CH1, LTC2492_CH2, LTC2492_CH3
                                              };    //!< Builds the command for single-ended mode

//! Lookup table to build the command for differential mode
const uint8_t BUILD_COMMAND_DIFF[4] = {LTC2492_P0_N1, LTC2492_P2_N3, LTC2492_P1_N0, LTC2492_P3_N2
                                      };      //!< Build the command for differential mode

//! Lookup table to build 1X / 2X bits
const uint8_t BUILD_1X_2X_COMMAND[2] = {LTC2492_SPEED_1X, LTC2492_SPEED_2X};   //!< Build the command for 1x or 2x mode

//! Lookup table to build 1X / 2X bits
const uint8_t BUILD_FREQ_REJ_COMMAND[3] = {LTC2492_R50, LTC2492_R60, LTC2492_R50_R60};   //!< Build the command for 1x or 2x mode

//! Initialize Linduino
void setup()
{
  char demo_name[]="DC1009";    // Demo Board Name stored in QuikEval EEPROM
  quikeval_SPI_init();          // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();       // Connect SPI to main data port
  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz
  Serial.begin(115200);         // Initialize the serial port to the PC
  print_title();

  demo_board_connected = discover_demo_board(demo_name);
  if (demo_board_connected)
  {
    print_prompt();
  }
  else
  {
    Serial.println(F("EEPROM not detected, will attempt to proceed"));
    demo_board_connected = 1;
    print_prompt();
  }
  quikeval_SPI_connect();       //Initialize for SPI
}

//! Repeats Linduino loop
void loop()
{
  int16_t user_command;                 // The user input command
  uint8_t ack_EOC = 0;                  // Keeps track of the EOC timeout
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
          ack_EOC |= menu_1_read_single_ended();
          break;
        case 2:
          ack_EOC |= menu_2_read_differential();
          break;
        case 3:
          menu_3_set_1X2X();
          break;
        case 4:
          menu_4_select_rejection_freq();
          break;
        default:
          Serial.println(F("Incorrect Option"));
      }
      if (ack_EOC)
        Serial.println(F("\n******SPI ERROR******\n"));
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
  Serial.print(F("* DC1009A-A Demonstration Program                               *\n"));
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
  Serial.print(F("3-2X Mode Settings\n"));
  Serial.print(F("4-Frequency Rejection Settings\n"));
  Serial.print(F("Enter a Command: "));
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
      Serial.print(F("1P-0N"));
      break;
    case 3:
      Serial.print(F("3P-2N"));
      break;
  }
  Serial.print(F(": "));
}

//! Read channels in single-ended mode
//! @return Returns 0=successful, 1=unsuccessful (exceeded timeout)
uint8_t menu_1_read_single_ended()
{
  uint8_t adc_command_high; // The LTC2492 command high byte
  uint8_t adc_command_low;  // The LTC2492 command low byte
  int16_t user_command;     // The user input command
  int32_t adc_code = 0;     // The LTC2492 code
  float adc_voltage = 0;    // The LTC2492 voltage

  while (1)
  {
    Serial.print(F("*************************\n\n"));
    Serial.print(F("0-CH0\n"));
    Serial.print(F("1-CH1\n"));
    Serial.print(F("2-CH2\n"));
    Serial.print(F("3-CH3\n"));
    Serial.print(F("4-ALL\n"));
    Serial.print(F("m-Main Menu\n"));
    Serial.print(F("Enter a Command: "));

    user_command = read_int();                              // Read the single command
    if (user_command == 'm')
      break;
    Serial.println(user_command);
    if (user_command == 4)
    {
      Serial.print(F("ALL\n"));
      adc_command_high = BUILD_COMMAND_SINGLE_ENDED[0];               // Build ADC command for channel 0
      adc_command_low = rejection_mode | two_x_mode;
      if (LTC2492_EOC_timeout(LTC2492_CS, eoc_timeout))             // Checks for EOC with a timeout
        return 1;
      LTC2492_read(LTC2492_CS, adc_command_high, adc_command_low, &adc_code);   // Throws out last reading

      for (int8_t x = 0; x <= 3; x++)                                 // Read all channels in single-ended mode
      {
        if (two_x_mode)
        {
          if (LTC2492_EOC_timeout(LTC2492_CS, eoc_timeout))
            return 1;
          LTC2492_read(LTC2492_CS, adc_command_high, adc_command_low, &adc_code); // Throws out an extra reading in 2x mode
        }
        adc_command_high = BUILD_COMMAND_SINGLE_ENDED[(x + 1) % 4];
        if (LTC2492_EOC_timeout(LTC2492_CS, eoc_timeout))
          return 1;
        LTC2492_read(LTC2492_CS, adc_command_high, adc_command_low, &adc_code);
        adc_voltage = LTC2492_code_to_voltage(adc_code, LTC2492_vref);

        Serial.print(F("  ****"));
        Serial.print(F("CH"));
        Serial.print(x);
        Serial.print(F(": "));
        Serial.print(adc_voltage, 4);
        Serial.print(F("V\n\n"));
      }
    }
    else
    {
      adc_command_high = BUILD_COMMAND_SINGLE_ENDED[user_command];          // Build ADC command
      adc_command_low = rejection_mode | two_x_mode;
      Serial.print(F("ADC Command: 0x"));
      Serial.println((adc_command_high<<8) | adc_command_low, HEX);
      if (LTC2492_EOC_timeout(LTC2492_CS, eoc_timeout))               // Checks for EOC with a timeout
        return 1;
      LTC2492_read(LTC2492_CS, adc_command_high, adc_command_low, &adc_code);       // Throws out last reading
      if (two_x_mode)
      {
        if (LTC2492_EOC_timeout(LTC2492_CS, eoc_timeout))
          return 1;
        LTC2492_read(LTC2492_CS, adc_command_high, adc_command_low, &adc_code);   // Throws out an extra reading in 2x mode
      }
      if (LTC2492_EOC_timeout(LTC2492_CS, eoc_timeout))
        return 1;
      LTC2492_read(LTC2492_CS, adc_command_high, adc_command_low, &adc_code);       // Now we're ready to read the desired data

      Serial.print(F("Received Code: 0x"));
      Serial.println(adc_code, HEX);
      adc_voltage = LTC2492_code_to_voltage(adc_code, LTC2492_vref);
      Serial.print(F("  ****"));
      Serial.print(F("CH"));
      Serial.print(user_command);
      Serial.print(F(": "));
      Serial.print(adc_voltage, 4);
      Serial.print(F("V\n\n"));
    }
  }
  return 0;
}

//! Read channels in differential mode
//! @return Returns 0=successful, 1=unsuccessful (exceeded timeout)
uint8_t menu_2_read_differential()
{
  int8_t y;                 // Offset into differential channel array to select polarity
  uint8_t adc_command_high; // The LTC2492 command high byte
  uint8_t adc_command_low;  // The LTC2492 command low byte
  int16_t user_command;     // The user input command
  int32_t adc_code = 0;    // The LTC2492 code
  float adc_voltage;        // The LTC2492 voltage

  while (1)
  {
    Serial.print(F("\n*************************\n\n")); // Display  differential menu
    Serial.print(F("0-0P-1N\n"));
    Serial.print(F("1-2P-3N\n"));
    Serial.print(F("2-1P-0N\n"));
    Serial.print(F("3-3P-2N\n"));
    Serial.print(F("m-Main Menu\n"));

    user_command = read_int();                              // Read the single command
    if (user_command == 'm')
      break;
    Serial.println(user_command);

    // Reads and displays a selected channel
    adc_command_high = BUILD_COMMAND_DIFF[user_command];
    adc_command_low = rejection_mode | two_x_mode;
    Serial.print(F("ADC Command: 0x"));
    Serial.println((adc_command_high<<8) | adc_command_low, HEX);

    if (LTC2492_EOC_timeout(LTC2492_CS, eoc_timeout))
      return 1;
    LTC2492_read(LTC2492_CS, adc_command_high, adc_command_low, &adc_code);     // Throws out last reading
    if (two_x_mode)
    {
      if (LTC2492_EOC_timeout(LTC2492_CS, eoc_timeout))
        return 1;
      LTC2492_read(LTC2492_CS, adc_command_high, adc_command_low, &adc_code);   // Throws out an extra reading in 2x mode
    }
    if (LTC2492_EOC_timeout(LTC2492_CS, eoc_timeout))
      return 1;
    LTC2492_read(LTC2492_CS, adc_command_high, adc_command_low, &adc_code);     // Now we're ready to read the desired data

    Serial.print(F("Received Code: 0x"));
    Serial.println(adc_code, HEX);
    adc_voltage = LTC2492_code_to_voltage(adc_code, LTC2492_vref);
    Serial.print(F("\n  ****"));
    print_user_command(user_command);
    Serial.print(adc_voltage, 4);
    Serial.print(F("V\n"));
  }
  return 0;
}

//! Set 1X or 2X mode
void menu_3_set_1X2X()
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
    two_x_mode = LTC2492_SPEED_1X;
    Serial.print(F("2X Mode Disabled\n"));
  }
  else
  {
    two_x_mode = LTC2492_SPEED_2X;
    Serial.print(F("2X Mode Enabled\n"));
  }
}


//! Set 1X or 2X mode
void menu_4_select_rejection_freq()
{
  int16_t user_command; // The user input command

  // 2X Mode
  Serial.print(F("Rejection Frequency\n\n"));
  Serial.print(F("0-50 Hz\n"));
  Serial.print(F("1-60 Hz\n"));
  Serial.print(F("2-50/60 Hz\n"));
  Serial.print(F("Enter a Command: "));
  user_command = read_int();
  Serial.println(user_command);

  if (user_command == 0)
  {
    rejection_mode = LTC2492_R50;
    Serial.print(F("50 Hz rejection\n"));
  }
  else if (user_command == 1)
  {
    rejection_mode = LTC2492_R60;
    Serial.print(F("60 Hz rejection\n"));
  }
  else
  {
    rejection_mode = LTC2492_R50_R60;
    Serial.print(F("50/60 Hz rejection\n"));
  }
}