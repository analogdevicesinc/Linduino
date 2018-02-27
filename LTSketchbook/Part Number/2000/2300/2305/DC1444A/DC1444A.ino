/*!
Linear Technology DC1337A Demonstration Board.
LTC2301: 1-Channel, 12-Bit SAR ADC with I2C Interface.
LTC2305: 2-Channel, 12-Bit SAR ADC with I2C Interface.

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. Equipment
   required is a voltage source (preferably low-noise) and a precision voltmeter.
   Ensure all jumpers on the demo board are installed in their default positions
   from the factory. Refer to Demo Manual DC1444A.

 LTC2301
  How to test Differential Mode:
   Bipolar Mode:
    The voltage source should be connected between inputs VIN+ and VIN-. Ensure both
    inputs are within their specified absolute input voltage range. (It is easiest
    to tie the voltage source negative terminal to COM.) Ensure the voltage
    source is set within the range of 0V to +4.096V (differential voltage range).
    (Swapping input voltages results in a reversed polarity reading.)

   Unipolar Mode:
    The voltage source should be connected between inputs VIN+ and VIN-. Ensure both
    inputs are within their specified absolute input voltage range. (It is easiest
    to tie the voltage source negative terminal to COM.) Ensure the voltage
    source is set within the range of 0V to +4.096V (differential voltage range).

LTC2305
  How to test Single-Ended mode:
   Bipolar Mode:
    The voltage source should be connected to input VCH0 or VCH1. Ensure the input is
    within its specified absolute input voltage range. (It is easiest
    to tie the voltage source negative terminal to COM.) Ensure the voltage
    source is set within the range of 0V to +4.096V.
    (Swapping input voltages results in a reversed polarity reading.)

   Unipolar Mode:
    The voltage source should be connected to input VCH0 or VCH1. Ensure the input is
    within its specified absolute input voltage range. (It is easiest
    to tie the voltage source negative terminal to COM.) Ensure the voltage
    source is set within the range of 0V to +4.096V.


USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2301
http://www.linear.com/product/LTC2305

http://www.linear.com/product/LTC2301#demoboards
http://www.linear.com/product/LTC2305#demoboards


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
    @ingroup LTC2305
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC2305.h"
#include <Wire.h>

// Function Declaration
void print_title();                                     // Print the title block
void print_prompt();                                    // Prompt the user for an input command
uint8_t print_user_command_single_ended();             // Display selected single ended channels
int8_t menu_1_read_input();                             // Read channels in single-ended mode
void menu_2_select_part();                              // Selects LTC2301/2305
void menu_3_select_uni_bipolar();                       // Sets LTC2305 to Bipolar or Unipolar
void menu_4_select_single_ended_differential();        // Selects single-ended or differential mode
void menu_5_select_polarity();                         // Selects polarity in differential mode
int8_t menu_6_sleep();                                  // Sets LTC2305 Sleep Mode
void menu_7_set_address();

// Global variables
static uint8_t demo_board_connected;                    //!< Set to 1 if the board is connected
static uint8_t single_ended_differential = LTC2305_SINGLE_ENDED_MODE;     //!< LTC2305 Single-Ended or Differential mode selection
static uint8_t uni_bipolar = LTC2305_UNIPOLAR_MODE;     //!< LTC2305 Unipolar or Bipolar mode selection
static uint8_t part = LTC2301;                          //!< LTC2301 or 2305 part selection
static uint8_t polarity = LTC2305_P0_N1;                //!< LTC230X Input polarity selection
static uint8_t channel = LTC2305_CH0;                   //!< LTC2305 Channel selection
static uint8_t i2c_address = LTC2305_I2C_ADDRESS;       //!< I2C address in 7 bit format for part
static uint8_t LTC2305_bits = 12;                      //!< Resolution (12 bits)
static float LTC2305_vref = 4.096;

// Constants
// Build the command for single-ended mode
const uint8_t BUILD_COMMAND_SINGLE_ENDED[2] = {LTC2305_CH0, LTC2305_CH1};        //!< Builds the command for single-ended mode

//! Initialize Linduino
void setup()
{
  quikeval_I2C_init();                    // Enable the I2C port
  quikeval_I2C_connect();                 // Connect I2C to main data port
  Serial.begin(115200);                   // Initialize the serial port to the PC
  print_title();
  print_prompt();
}


//! Repeats Linduino loop
void loop()
{
  int8_t user_command;              // The user input command
  uint8_t acknowledge = 0;              // I2C acknowledge bit

  if (Serial.available())             // Check for user input
  {
    user_command = read_int();        // Read the user command
    if (user_command != 'm')
      Serial.println(user_command);   // Prints the user command to com port
    Serial.flush();
    switch (user_command)
    {
      case 1:
        menu_1_read_input();
        break;
      case 2:
        menu_2_select_part();
        break;
      case 3:
        menu_3_select_uni_bipolar();
        break;
      case 4:
        menu_4_select_single_ended_differential();
        break;
      case 5:
        menu_5_select_polarity();
        break;
      case 6:
        menu_6_sleep();
        break;
      case 7:
        menu_7_set_address();
        break;
      default:
        Serial.println(F("Invalid Option"));
        break;
    }
    Serial.println();
    if (acknowledge != 0) Serial.println(F("Error: No Acknowledge. Check I2C Address."));
    Serial.println(F("*************************"));
    print_prompt();
  }
}


// Function Definitions
//! Read channels in single-ended mode
//! @return 1 if successful, 0 if not
int8_t menu_1_read_input()
{
  uint8_t user_command;
  uint16_t adc_command;         // The LTC2305 command byte
  uint16_t adc_code = 0;    // The LTC2305 code
  uint8_t x, y, startcount, endcount;
  uint16_t display_code;
  float adc_voltage;
  uint8_t acknowledge = 0;       // I2C acknowledge bit

  if (part == LTC2305)
  {
    if (single_ended_differential == LTC2305_SINGLE_ENDED_MODE)
    {
      if (uni_bipolar == LTC2305_UNIPOLAR_MODE)
      {
        Serial.println(F("  Read LTC2305 Input in Single-Ended Unipolar mode:"));
        Serial.println(F("  Note that in Unipolar mode, input voltages less than zero are reported as 0.0V"));
      }
      else
        Serial.println(F("  Read LTC2305 Input in Single-Ended Bipolar mode:"));
      user_command = print_user_command_single_ended();
    }
    else
    {
      if (uni_bipolar == LTC2305_UNIPOLAR_MODE)
      {
        Serial.println(F("  Read LTC2305 Input in Differential Unipolar mode:"));
        Serial.println(F("  Note that in Unipolar mode, input voltages less than zero are reported as 0.0V"));
      }
      else
        Serial.println(F("  Read LTC2305 Input in Differential Bipolar mode:"));
    }
  }

  else
  {
    if (uni_bipolar == LTC2305_UNIPOLAR_MODE)
    {
      Serial.println(F("  Read LTC2301 Input in Differential Unipolar mode:"));
      Serial.println(F("  Note that in Unipolar mode, input voltages less than zero are reported as 0.0V"));
    }
    else
      Serial.println(F("  Read LTC2301 Input in Differential Bipolar mode:"));
  }

  if (user_command == 8)  //read both channels
  {
    startcount = 0;
    endcount = 1;
  }
  else
  {
    startcount = user_command;
    endcount = user_command;
  }

  if (part == LTC2305) //read LTC2305
  {
    if (single_ended_differential == LTC2305_SINGLE_ENDED_MODE)
    {
      for (int8_t x = startcount; x <= endcount; x++)
      {
        adc_command = BUILD_COMMAND_SINGLE_ENDED[(x+1) % 2] | polarity | uni_bipolar; // Send channel config for the NEXT conversion to take place
        acknowledge |= LTC2305_read(LTC2305_I2C_ADDRESS, adc_command, &adc_code);     // Throws out last reading and starts CH0 conversion
        acknowledge |= LTC2305_read(LTC2305_I2C_ADDRESS, adc_command, &adc_code);   // Read previous channel conversion (x-1) and start next one (x)

        display_code = adc_code >> (16 - LTC2305_bits);
        display_code = display_code & 0xFFF;

        Serial.print(F("  Received Code: b"));
        Serial.println(display_code, BIN);

        adc_voltage = LTC2305_code_to_voltage(adc_code, LTC2305_vref, uni_bipolar);
        Serial.print(F("  Voltage read on "));
        Serial.print(F("Ch"));
        Serial.print(x);
        Serial.print(F(": "));
        Serial.print(adc_voltage, 4);
        Serial.println(F("V"));
        Serial.println();
      }
    }

    else  //differential reading
    {
      adc_command = polarity | uni_bipolar; // Send channel config for the NEXT conversion to take place
      acknowledge |= LTC2305_read(LTC2305_I2C_ADDRESS, adc_command, &adc_code);     // Throws out last reading and starts CH0 conversion
      acknowledge |= LTC2305_read(LTC2305_I2C_ADDRESS, adc_command, &adc_code);   // Read previous channel conversion (x-1) and start next one (x)

      display_code = adc_code >> (16 - LTC2305_bits);
      display_code = display_code & 0xFFF;

      Serial.print(F("  Received Code: b"));
      Serial.println(display_code, BIN);

      adc_voltage = LTC2305_code_to_voltage(adc_code, LTC2305_vref, uni_bipolar);
      Serial.print(F("  Voltage read on "));
      Serial.print(F("Ch"));
      Serial.print(x);
      Serial.print(F(": "));
      Serial.print(adc_voltage, 4);
      Serial.println(F("V"));
      Serial.println();
    }
  }

  else  //read LTC2301
  {
    adc_command = polarity | uni_bipolar;
    acknowledge |= LTC2305_read(LTC2305_I2C_ADDRESS, adc_command, &adc_code);     // Throws out last reading and starts new conversion
    acknowledge |= LTC2305_read(LTC2305_I2C_ADDRESS, adc_command, &adc_code);

    display_code = adc_code >> (16 - LTC2305_bits);
    display_code = display_code & 0xFFF;

    Serial.print(F("  Received Code: b"));
    Serial.println(display_code, BIN);

    adc_voltage = LTC2305_code_to_voltage(adc_code, LTC2305_vref, uni_bipolar);
    Serial.print(F("  Voltage read on "));
    Serial.print(F("Ch"));
    Serial.print(user_command);
    Serial.print(F(": "));
    Serial.print(adc_voltage, 4);
    Serial.println(F("V"));
    Serial.println();
  }
  return(acknowledge);
}


//! Sets LTC2305 to Single-Ended or Differential
void menu_2_select_part()
{
  uint8_t user_command;

  Serial.println(F("\n  0 = LTC2301"));
  Serial.println(F("  1 = LTC2305"));
  Serial.print(F("  Enter a Command: "));

  user_command = read_int();    // Read user input for uni_bipolar
  Serial.println(user_command);
  switch (user_command)
  {
    case 0:
      Serial.println(F("  LTC2301 selected"));
      part = LTC2301;
      break;
    case 1:
      Serial.println(F("  LTC2305 selected"));
      part = LTC2305;
      break;
    default:
      {
        Serial.println("  Invalid Option");
        return;
      }
      break;
  }
}


//! Sets LTC230X to Bipolar or Unipolar
void menu_3_select_uni_bipolar()
{
  uint8_t user_command;

  Serial.println(F("\n  0 = Unipolar"));
  Serial.println(F("  1 = Bipolar"));
  Serial.print(F("  Enter a Command: "));

  user_command = read_int();    // Read user input for uni_bipolar
  Serial.println(user_command);
  switch (user_command)
  {
    case 0:
      Serial.println(F("  Uniipolar mode selected"));
      uni_bipolar = LTC2305_UNIPOLAR_MODE;
      break;
    case 1:
      Serial.println(F("  Bipolar mode selected"));
      uni_bipolar = LTC2305_BIPOLAR_MODE;
      break;
    default:
      {
        Serial.println("  Invalid Option");
        return;
      }
      break;
  }
}


//! Sets LTC2305 single-ended or differential mode
void menu_4_select_single_ended_differential()
{
  uint8_t user_command;

  Serial.println(F("\n  0 = Single-Ended"));
  Serial.println(F("  1 = Differential"));
  Serial.print(F("  Enter a Command: "));

  user_command = read_int();    // Read user input for uni_bipolar
  Serial.println(user_command);
  switch (user_command)
  {
    case 0:
      Serial.println(F("  Single-ended mode selected"));
      single_ended_differential = LTC2305_SINGLE_ENDED_MODE;
//          menu_99_select_channel();
      break;
    case 1:
      Serial.println(F("  Differential mode selected"));
      single_ended_differential = LTC2305_DIFFERENTIAL_MODE;
      break;
    default:
      {
        Serial.println("  Invalid Option");
        return;
      }
      break;
  }
}


//! Sets LTC2305 polarity in differential mode
void menu_5_select_polarity()
{
  uint8_t user_command;

  Serial.println(F("\n  0 = 0P-1N"));
  Serial.println(F("  1 = 1P-0N"));
  Serial.print(F("  Enter a Command: "));

  user_command = read_int();    // Read user input for uni_bipolar
  Serial.println(user_command);
  switch (user_command)
  {
    case 0:
      Serial.println(F("  0P-1N selected"));
      polarity = LTC2305_P0_N1;
      break;
    case 1:
      Serial.println(F("  1P-0N selected"));
      polarity = LTC2305_P1_N0;
      break;
    default:
      {
        Serial.println("  Invalid Option");
        return;
      }
      break;
  }
}


//! Sets LTC2305 Sleep Mode
//! @return 1 if successful, 0 if not
int8_t menu_6_sleep()
{
  int8_t acknowledge = 0;           // I2C acknowledge bit
  uint16_t user_command;
  uint16_t adc_code;        // The LTC2305 code

  acknowledge |= LTC2305_read(LTC2305_I2C_ADDRESS, LTC2305_SLEEP_MODE, &adc_code);

  Serial.println();
  Serial.print(F("  ADC Command: b"));
  Serial.println(LTC2305_SLEEP_MODE, BIN);
  Serial.println(F("  LTC2305 is now in sleep mode"));
  Serial.println(F("  Enter RETURN to exit Sleep Mode"));

  user_command = read_int();

  acknowledge |= LTC2305_read(LTC2305_I2C_ADDRESS, LTC2305_EXIT_SLEEP_MODE, &adc_code);
  return(acknowledge);
}


//! Set the I2C 7 bit address
void menu_7_set_address()
{
  int16_t user_command;
  Serial.print(F("  Enter the I2C address of the part in decimal format, from 0 to 127 (default is 8)\n"));
  user_command = read_int();
  Serial.print(F("  Address entered: "));
  Serial.println(user_command);
  i2c_address = user_command&0x7F;
}


//! Prints the title block when the program first starts.
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC1444A Demonstration Program                                 *"));
  Serial.println(F("* This program demonstrates how to receive data                 *"));
  Serial.println(F("* from the following ADCs:                                      *"));
  Serial.println(F("*   LTC2301                                                     *"));
  Serial.println(F("*   LTC2305                                                     *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
}


//! Prints main menu.
void print_prompt()
{
  Serial.println(F("1-Read ADC Input "));
  Serial.println(F("2-Select LTC2301 / LTC2305 (default is LTC2301)"));
  Serial.println(F("3-Select Unipolar / Bipolar measurement (default is Unipolar)"));
  Serial.println(F("4-Select Single-Ended / Differential measurement, LTC2305 only (default is Single-Ended)"));
  Serial.println(F("5-Select Polarity (default is 0+/1-)"));
  Serial.println(F("6-Sleep Mode"));
  Serial.println(F("7-Set I2C address (default is 8)"));
  Serial.println();
  Serial.print(F("Enter a command:  "));
}


//! Display selected single-ended channels.
uint8_t print_user_command_single_ended()
{
  uint8_t user_command;
  Serial.println(F("*************************"));
  Serial.println(F("  0 = CH0"));
  Serial.println(F("  1 = CH1"));
  Serial.println(F("  2 = ALL"));
  Serial.println(F("  m = Main Menu"));
  Serial.println();
  Serial.print(F("  Enter a Command: "));

  user_command = read_int();  // Read the single-ended menu command
  Serial.println(user_command);
  if (user_command == 'm')
    return(0);

  switch (user_command)  //check for invalid selection
  {
    case 0:
      Serial.println(F("  CH0 selected"));
      break;
    case 1:
      Serial.println(F("  CH1 selected"));
      break;
    case 2:
      Serial.println(F("  All selected"));
      break;
    default:
      Serial.println(F("  Invalid Option"));
      break;
  }

  return(user_command);
}



