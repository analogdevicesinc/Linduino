/*!
Linear Technology DC1011A-A Demonstration Board.
LTC2498: 24-Bit, 16-Channel Delta Sigma ADC with SPI interface

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

http://www.linear.com/product/LTC2498

http://www.linear.com/product/LTC2498#demoboards


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
    @ingroup LTC2498
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
#include "LTC2498.h"

// Function Declaration
void print_title();                             // Print the title block
void print_prompt();                            // Prompt the user for an input command
void print_user_command(uint8_t menu);          // Display selected differential channels

uint8_t menu_1_read_single_ended();
uint8_t menu_2_read_differential();
uint8_t menu_3_read_temperature();
void menu_4_set_1X2X();

// Global variables
static uint8_t demo_board_connected;                //!< Set to 1 if the board is connected
static uint8_t two_x_mode = LTC2498_SPEED_1X;       //!< The LTC2498 2X Mode settings
static uint8_t rejection_mode = LTC2498_R50_R60;    //!< The LTC2498 rejection mode settings
static float LTC2498_vref = 5.0;                    //!< The LTC2498 ideal reference voltage
static uint16_t eoc_timeout = 250;                  //!< timeout in ms
// Constants

//! Lookup table to build the command for single-ended mode
const uint8_t BUILD_COMMAND_SINGLE_ENDED[16] = {LTC2498_CH0, LTC2498_CH1, LTC2498_CH2, LTC2498_CH3,
    LTC2498_CH4, LTC2498_CH5, LTC2498_CH6, LTC2498_CH7,
    LTC2498_CH8, LTC2498_CH9, LTC2498_CH10, LTC2498_CH11,
    LTC2498_CH12, LTC2498_CH13, LTC2498_CH14, LTC2498_CH15
                                               };    //!< Builds the command for single-ended mode

//! Lookup table to build the command for differential mode
const uint8_t BUILD_COMMAND_DIFF[16] = {LTC2498_P0_N1, LTC2498_P2_N3, LTC2498_P4_N5, LTC2498_P6_N7,
                                        LTC2498_P8_N9, LTC2498_P10_N11, LTC2498_P12_N13, LTC2498_P14_N15,
                                        LTC2498_P1_N0, LTC2498_P3_N2, LTC2498_P5_N4, LTC2498_P7_N6,
                                        LTC2498_P9_N8, LTC2498_P11_N10, LTC2498_P13_N12, LTC2498_P15_N14
                                       };      //!< Build the command for differential mode

//! Lookup table to build 1X / 2X bits
const uint8_t BUILD_1X_2X_COMMAND[2] = {LTC2498_SPEED_1X, LTC2498_SPEED_2X};   //!< Build the command for 1x or 2x mode

//! Initialize Linduino
void setup()
{
  char demo_name[]="DC1011";    // Demo Board Name stored in QuikEval EEPROM
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
          ack_EOC |= menu_3_read_temperature();
          break;
        case 4:
          menu_4_set_1X2X();
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
  Serial.print(F("* DC1011A-A Demonstration Program                               *\n"));
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
  Serial.print(F("3-Read Temperature\n"));
  Serial.print(F("4-2X Mode Settings\n"));
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
//! @return Returns 0=successful, 1=unsuccessful (exceeded timeout)
uint8_t menu_1_read_single_ended()
{
  uint8_t adc_command_high; // The LTC2498 command high byte
  uint8_t adc_command_low;  // The LTC2498 command low byte
  int16_t user_command;     // The user input command
  int32_t adc_code = 0;     // The LTC2498 code
  float adc_voltage = 0;    // The LTC2498 voltage

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
    Serial.print(F("Enter a Command: "));

    user_command = read_int();                              // Read the single command
    if (user_command == 'm')
      break;
    Serial.println(user_command);

    if (user_command == 16)
    {
      Serial.print(F("ALL\n"));
      adc_command_high = BUILD_COMMAND_SINGLE_ENDED[0];     // Build ADC command for channel 0
      adc_command_low = rejection_mode | two_x_mode;
      if (LTC2498_EOC_timeout(LTC2498_CS, eoc_timeout))
        return 1;
      LTC2498_read(LTC2498_CS, adc_command_high, adc_command_low, &adc_code);   // Throws out last reading

      for (int8_t x = 0; x <= 15; x++)                      // Read all channels in single-ended mode
      {


        adc_command_high = BUILD_COMMAND_SINGLE_ENDED[(x + 1) % 16];
        if (LTC2498_EOC_timeout(LTC2498_CS, eoc_timeout))
          return 1;
        LTC2498_read(LTC2498_CS, adc_command_high, adc_command_low, &adc_code);
        adc_voltage = LTC2498_code_to_voltage(adc_code, LTC2498_vref);

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
      adc_command_high = BUILD_COMMAND_SINGLE_ENDED[user_command];
      adc_command_low = rejection_mode | two_x_mode;
      Serial.print(F("ADC Command: 0x"));
      Serial.println((adc_command_high<<8) | adc_command_low, HEX);

      quikeval_SPI_connect();
      if (LTC2498_EOC_timeout(LTC2498_CS, eoc_timeout))
        return 1;
      LTC2498_read(LTC2498_CS, adc_command_high, adc_command_low, &adc_code);     // Throws out last reading
      if (LTC2498_EOC_timeout(LTC2498_CS, eoc_timeout))
        return 1;
      LTC2498_read(LTC2498_CS, adc_command_high, adc_command_low, &adc_code);     // Now we're ready to read the desired data

      Serial.print(F("Received Code: 0x"));
      Serial.println(adc_code, HEX);
      adc_voltage = LTC2498_code_to_voltage(adc_code, LTC2498_vref);
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
  uint8_t adc_command_high; // The LTC2498 command high byte
  uint8_t adc_command_low;  // The LTC2498 command low byte
  int16_t user_command;     // The user input command
  int32_t adc_code = 0;    // The LTC2498 code
  float adc_voltage;        // The LTC2498 voltage

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

    user_command = read_int();                              // Read the single command
    if (user_command == 'm')
      break;
    Serial.println(user_command);

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

      adc_command_high = BUILD_COMMAND_DIFF[y];             // Build the channel 0 and 1 for differential mode
      adc_command_low = rejection_mode |  two_x_mode;

      if (LTC2498_EOC_timeout(LTC2498_CS, eoc_timeout))
        return 1;
      LTC2498_read(LTC2498_CS, adc_command_high, adc_command_low, &adc_code);   // Throws out reading
      for (int8_t x = 0; x < 8; x++)                        // Read all channels in differential mode. All even channels are positive and odd channels are negative
      {




        adc_command_high = BUILD_COMMAND_DIFF[((x + 1) % 8) + y];

        if (LTC2498_EOC_timeout(LTC2498_CS, eoc_timeout))
          return 1;
        LTC2498_read(LTC2498_CS, adc_command_high, adc_command_low, &adc_code);

        Serial.print(F("Received Code: 0x"));
        Serial.println((adc_command_high<<8) | adc_command_low, HEX);
        adc_voltage = LTC2498_code_to_voltage(adc_code, LTC2498_vref);
        Serial.print(F("\n  ****"));
        print_user_command(x + y);
        Serial.print(F(": "));
        Serial.print(adc_voltage, 4);
        Serial.print(F("V\n"));

      }
    }
    else
    {
      // Reads and displays a selected channel
      adc_command_high = BUILD_COMMAND_DIFF[user_command];
      adc_command_low = rejection_mode | two_x_mode;
      Serial.print(F("ADC Command: 0x"));
      Serial.println((adc_command_high<<8) | adc_command_low, HEX);

      if (LTC2498_EOC_timeout(LTC2498_CS, eoc_timeout))
        return 1;
      LTC2498_read(LTC2498_CS, adc_command_high, adc_command_low, &adc_code);     // Throws out last reading



      if (LTC2498_EOC_timeout(LTC2498_CS, eoc_timeout))
        return 1;
      LTC2498_read(LTC2498_CS, adc_command_high, adc_command_low, &adc_code);     // Now we're ready to read the desired data

      Serial.print(F("Received Code: 0x"));
      Serial.println(adc_code, HEX);
      adc_voltage = LTC2498_code_to_voltage(adc_code, LTC2498_vref);
      Serial.print(F("\n  ****"));
      print_user_command(user_command);
      Serial.print(adc_voltage, 4);
      Serial.print(F("V\n"));

    }
  }
  return 0;
}

//! Read Temperature
//! @return Returns 0=successful, 1=unsuccessful (exceeded timeout)
uint8_t menu_3_read_temperature()
{
  uint8_t adc_command_high; // The LTC2498 command high byte
  uint8_t adc_command_low;  // The LTC2498 command low byte

  int32_t adc_code = 0;            // The LTC2498 code
  float adc_voltage;                // The LTC2498 voltage

  //Command byte for temperature read
  adc_command_high = BUILD_COMMAND_DIFF[0]; // Any channel can be selected
  adc_command_low = LTC2498_INTERNAL_TEMP | rejection_mode;

  if (LTC2498_EOC_timeout(LTC2498_CS, eoc_timeout))
    return 1;
  LTC2498_read(LTC2498_CS,adc_command_high, adc_command_low, &adc_code);     // Throws out last reading
  if (LTC2498_EOC_timeout(LTC2498_CS, eoc_timeout))
    return 1;
  LTC2498_read(LTC2498_CS,adc_command_high, adc_command_low, &adc_code);     // Now we're ready to read the desired data

  Serial.print(F("Received Code: 0x"));
  Serial.println(adc_code, HEX);
  adc_voltage = LTC2498_code_to_voltage(adc_code, LTC2498_vref);
  Serial.print(F("\n  ****"));
  Serial.print(adc_voltage, 4);
  Serial.print(F("V\n"));
  Serial.print(F("This equals to: "));
  float temperature = (adc_voltage/93.5e-6);
  Serial.print(temperature, DEC);
  Serial.print(F(" K\n"));
  return 0;
}

//! Set 1X or 2X mode
void menu_4_set_1X2X()
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
    two_x_mode = LTC2498_SPEED_1X;
    Serial.print(F("2X Mode Disabled\n"));
  }
  else
  {
    two_x_mode = LTC2498_SPEED_2X;
    Serial.print(F("2X Mode Enabled\n"));
  }
}
