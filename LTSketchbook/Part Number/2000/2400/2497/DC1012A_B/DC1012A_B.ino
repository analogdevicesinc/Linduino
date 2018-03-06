/*!
Linear Technology DC1012A-B Demonstration Board.
LTC2497: 16-Bit, 16-Channel Delta Sigma ADCs with Easy Drive Input Current Cancellation

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


USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2497

http://www.linear.com/product/LTC2497#demoboards


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
    @ingroup LTC2497
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include <SPI.h>
#include "UserInterface.h"
#include "LTC24XX_general.h"
#include "LTC2497.h"
#include "USE_WIRE.h"

#ifdef USEWIRE
#include "LT_I2C_Wire.h"
#include "QuikEval_EEPROM_Wire.h"
#else
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#endif

// Function Declaration
void print_title();                             // Print the title block
void print_prompt();                            // Prompt the user for an input command
void print_user_command(uint8_t menu);          // Display selected differential channels

int8_t menu_1_read_single_ended();
int8_t menu_2_read_differential();
void menu_3_set_address();

// Global variables
static uint8_t demo_board_connected;                //!< Set to 1 if the board is connected
static float LTC2497_vref = 5.0;                    //!< The ideal reference voltage
static uint8_t i2c_address = LTC2497_I2C_ADDRESS;   //!< I2C address in 7 bit format for part
static uint16_t timeout = 300;                      //!< 300 ms timeout

// Constants

//! Lookup table to build the command for single-ended mode
const uint8_t BUILD_COMMAND_SINGLE_ENDED[16] = {LTC24XX_MULTI_CH_CH0, LTC24XX_MULTI_CH_CH1, LTC24XX_MULTI_CH_CH2, LTC24XX_MULTI_CH_CH3,
    LTC24XX_MULTI_CH_CH4, LTC24XX_MULTI_CH_CH5, LTC24XX_MULTI_CH_CH6, LTC24XX_MULTI_CH_CH7,
    LTC24XX_MULTI_CH_CH8, LTC24XX_MULTI_CH_CH9, LTC24XX_MULTI_CH_CH10, LTC24XX_MULTI_CH_CH11,
    LTC24XX_MULTI_CH_CH12, LTC24XX_MULTI_CH_CH13, LTC24XX_MULTI_CH_CH14, LTC24XX_MULTI_CH_CH15
                                               };    //!< Builds the command for single-ended mode

//! Lookup table to build the command for differential mode
const uint8_t BUILD_COMMAND_DIFF[16] = {LTC24XX_MULTI_CH_P0_N1, LTC24XX_MULTI_CH_P2_N3, LTC24XX_MULTI_CH_P4_N5, LTC24XX_MULTI_CH_P6_N7,
                                        LTC24XX_MULTI_CH_P8_N9, LTC24XX_MULTI_CH_P10_N11, LTC24XX_MULTI_CH_P12_N13, LTC24XX_MULTI_CH_P14_N15,
                                        LTC24XX_MULTI_CH_P1_N0, LTC24XX_MULTI_CH_P3_N2, LTC24XX_MULTI_CH_P5_N4, LTC24XX_MULTI_CH_P7_N6,
                                        LTC24XX_MULTI_CH_P9_N8, LTC24XX_MULTI_CH_P11_N10, LTC24XX_MULTI_CH_P13_N12, LTC24XX_MULTI_CH_P15_N14
                                       };      //!< Build the command for differential mode

//! Initialize Linduino
void setup()
{
  char demo_name[]="DC1012";    // Demo Board Name stored in QuikEval EEPROM
  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz
  quikeval_I2C_connect();       // Connect I2C to main data port
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
          menu_3_set_address();
          break;
        default:
          Serial.println(F("Incorrect Option"));
      }
      if (acknowledge)
        Serial.println(F("***** I2C ERROR *****"));
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
  Serial.print(F("* DC1012A-B Demonstration Program                               *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send data and receive data   *\n"));
  Serial.print(F("* from the 16-bit ADC.                                          *\n"));
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
  Serial.print(F("3-Set I2C Address\n"));
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
      Serial.print(F("4P-5N"));
      break;
    case 3:
      Serial.print(F("6P-7N"));
      break;
    case 4:
      Serial.print(F("8P-9N"));
      break;
    case 5:
      Serial.print(F("10P-11N"));
      break;
    case 6:
      Serial.print(F("12P-13N"));
      break;
    case 7:
      Serial.print(F("14P-15N"));
      break;
    case 8:
      Serial.print(F("1P-0N"));
      break;
    case 9:
      Serial.print(F("3P-2N"));
      break;
    case 10:
      Serial.print(F("5P-4N"));
      break;
    case 11:
      Serial.print(F("7P-6N"));
      break;
    case 12:
      Serial.print(F("9P-8N"));
      break;
    case 13:
      Serial.print(F("11P-10N"));
      break;
    case 14:
      Serial.print(F("13P-12N"));
      break;
    case 15:
      Serial.print(F("15P-14N"));
      break;
  }
  Serial.print(F(": "));
}

//! Read channels in single-ended mode
//! @return 0 if successful, 1 is failure
int8_t menu_1_read_single_ended()
{
  uint8_t adc_command;     // The LTC2497 command word
  int16_t user_command;    // The user input command
  int32_t adc_code = 0;    // The LTC2497 code
  float adc_voltage=0;     // The LTC2497 voltage

  while (1)
  {
    uint8_t ack = 0;

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
    Serial.print(F("Enter a Command: "));

    user_command = read_int();                                      // Read the single command
    if (user_command == 'm')
      return(0);
    Serial.println(user_command);

    if (user_command == 16)
    {
      Serial.print(F("ALL\n"));
      adc_command = BUILD_COMMAND_SINGLE_ENDED[0];                  // Build ADC command for channel 0
      ack |= LTC2497_read(i2c_address, adc_command, &adc_code, timeout);    // Throws out last reading

      for (int8_t x = 0; x <= 15; x++)                              // Read all channels in single-ended mode
      {
        delay(170);

        adc_command = BUILD_COMMAND_SINGLE_ENDED[(x + 1) % 16];

        ack |= LTC2497_read(i2c_address, adc_command, &adc_code, timeout);
        adc_voltage = LTC2497_code_to_voltage(adc_code, LTC2497_vref);
        if (!ack)
        {
          Serial.print(F("  ****"));
          Serial.print(F("CH"));
          Serial.print(x);
          Serial.print(F(": "));
          Serial.print(adc_voltage, 4);
          Serial.print(F("V\n\n"));
        }
        else
        {
          Serial.print(F("  ****"));
          Serial.print(F("CH"));
          Serial.print(x);
          Serial.print(F(": "));
          Serial.print(F("Error in read"));
          return 1;
        }
      }
    }
    else
    {
      adc_command = BUILD_COMMAND_SINGLE_ENDED[user_command];
      Serial.print(F("ADC Command: 0x"));
      Serial.println(adc_command, HEX);
      Serial.print(F("I2C address is: "));
      Serial.println(i2c_address,HEX);

      ack |= LTC2497_read(i2c_address, adc_command, &adc_code, timeout);     // Throws out last reading


      delay(170);

      ack |= LTC2497_read(i2c_address, adc_command, &adc_code, timeout);     // Now we're ready to read the desired data
      if (!ack)
      {
        Serial.print(F("Received Code: 0x"));
        Serial.println(adc_code, HEX);
        adc_voltage = LTC2497_code_to_voltage(adc_code, LTC2497_vref);
        Serial.print(F("  ****"));
        Serial.print(F("CH"));
        Serial.print(user_command);
        Serial.print(F(": "));
        Serial.print(adc_voltage, 4);
        Serial.print(F("V\n\n"));
      }
      else
      {
        Serial.println(F("Device NAK'd, please check address and try again"));
        return 1;
      }
    }
  }
  return(0);
}

//! Read channels in differential mode
//! @return 0 if successful, 1 is failure
int8_t menu_2_read_differential()
{
  int8_t y;                 // Offset into differential channel array to select polarity
  uint8_t adc_command;      // The LTC2497 command word
  int16_t user_command;     // The user input command
  int32_t adc_code = 0;     // The LTC2497 code
  float adc_voltage;        // The LTC2497 voltage

  while (1)
  {
    uint8_t ack = 0;

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

      adc_command = BUILD_COMMAND_DIFF[y];            // Build the channel 0 and 1 for differential mode

      ack |= LTC2497_read(i2c_address, adc_command, &adc_code, timeout);    // Throws out reading
      for (int8_t x = 0; x < 8; x++)                  // Read all channels in differential mode. All even channels are positive and odd channels are negative
      {
        delay(170);

        adc_command = BUILD_COMMAND_DIFF[((x + 1) % 8) + y];

        ack |= LTC2497_read(i2c_address, adc_command, &adc_code, timeout);
        if (!ack)
        {
          Serial.print(F("Received Code: 0x"));
          Serial.println(adc_code, HEX);
          adc_voltage = LTC2497_code_to_voltage(adc_code, LTC2497_vref);
          Serial.print(F("\n  ****"));
          print_user_command(x + y);
          Serial.print(F(": "));
          Serial.print(adc_voltage, 4);
          Serial.print(F("V\n"));
        }
        else
        {
          Serial.println(F("Device NAK'd, please check I2C address"));
          return 1;
        }
      }
    }
    else
    {
      // Reads and displays a selected channel
      adc_command = BUILD_COMMAND_DIFF[user_command];
      Serial.print(F("ADC Command: 0x"));
      Serial.println(adc_command, HEX);

      ack |= LTC2497_read(i2c_address, adc_command, &adc_code, timeout);     // Throws out last reading
      delay(170);

      ack |= LTC2497_read(i2c_address, adc_command, &adc_code, timeout);
      if (!ack)
      {
        Serial.print(F("Received Code: 0x"));
        Serial.println(adc_code, HEX);
        adc_voltage = LTC2497_code_to_voltage(adc_code, LTC2497_vref);
        Serial.print(F("\n  ****"));
        print_user_command(user_command);
        Serial.print(adc_voltage, 4);
        Serial.print(F("V\n"));
      }
      else
      {
        Serial.println(F("Device NAK'd, please check I2C address"));
        return 1;
      }
    }
  }
  return(0);
}

//! Set the I2C 7 bit address
void menu_3_set_address()
{
  int16_t user_command;
  Serial.print(F("What is the I2C address of the part?\n"));
  Serial.print(F("Please enter in 7-bit format, decimal\n"));

  user_command =read_int();
  Serial.println(user_command);
  i2c_address = user_command&0x7F;
}