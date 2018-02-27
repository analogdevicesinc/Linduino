/*!
Linear Technology DC1186A Demonstration Board.
LTC2308: 12-Bit, 8-Channel 500ksps SAR ADC with SPI Interface.

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. Equipment
   required is a voltage source (preferably low-noise) and a precision voltmeter. No
   external power supply is required. Ensure all jumpers on the demo board are
   installed in their default positions from the factory. Refer to Demo Manual
   DC1186A.

  How to test Single-Ended mode:
   Bipolar Mode:
    The voltage source should be connected to one of the inputs CH0,...CH7. Ensure the input is
    within its specified absolute input voltage range. (It is easiest
    to tie the voltage source negative terminal to COM.) Ensure the voltage
    source is set within the range of 0V to +4.096V.
    (Swapping input voltages results in a reversed polarity reading.)

   Unipolar Mode:
    The voltage source should be connected to one of the inputs CH0,...CH7. Ensure the input is
    within its specified absolute input voltage range. (It is easiest
    to tie the voltage source negative terminal to COM.) Ensure the voltage
    source is set within the range of 0V to +4.096V.

  How to test Differential Mode:
   Bipolar Mode:
     The voltage source should be connected between paired channels. Ensure both
     inputs are within their specified absolute input voltage range. (It is easiest
     to tie the voltage source negative terminal to COM or GND.) Ensure the voltage
     source is set within the range of 0 to +4.096V (differential voltage range).
     (Swapping input voltages results in a reversed polarity reading.)

   Unipolar Mode:
     The voltage source should be connected between paired channels. (It is easiest
     to tie the voltage source negative terminal to COM or GND.) Ensure both inputs
     are within their specified absolute input voltage range.  (It is easiest to tie
     the voltage source negative terminal to COM or GND.) Ensure the voltage source
     is set within the range of 0 to +4.096V (differential voltage range).

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2308

http://www.linear.com/product/LTC2308#demoboards


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
    @ingroup LTC2308
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC2308.h"
#include <SPI.h>
#include <Wire.h>

// Function Declaration
void print_title();                                     // Print the title block
void print_prompt();                                    // Prompt the user for an input command
uint8_t print_user_command_differential();             // Display selected differential channels
uint8_t print_user_command_single_ended();             // Display selected single ended channels
void print_channel_selection(uint8_t menu);

void menu_1_read_input();                             // Read channels
void menu_2_select_single_ended_differential();       // Sets LTC2308 to Single-Ended or Differential
void menu_3_select_uni_bipolar();                       // Sets LTC2308 to Bipolar or Unipolar
void menu_4_sleep();

// Global variables
static uint8_t uni_bipolar = LTC2308_UNIPOLAR_MODE;    //!< Default set for unipolar mode
static uint8_t single_ended_differential = LTC2308_SINGLE_ENDED_MODE;    //!< LTC2308 Unipolar or Bipolar mode selection
static uint8_t LTC2308_bits = 12;                      //!< Resolution (12 bits)
static float LTC2308_vref = 4.096;

// Constants
//! Lookup table to build the command for single-ended mode, input with respect to GND
const uint8_t BUILD_COMMAND_SINGLE_ENDED[8] = {LTC2308_CH0, LTC2308_CH1, LTC2308_CH2, LTC2308_CH3,
    LTC2308_CH4, LTC2308_CH5, LTC2308_CH6, LTC2308_CH7
                                              }; //!< Builds the command for single-ended mode, input with respect to GND

//! Lookup table to build the command for differential mode with the selected uni/bipolar mode
const uint8_t BUILD_COMMAND_DIFF[8] = {LTC2308_P0_N1, LTC2308_P2_N3, LTC2308_P4_N5, LTC2308_P6_N7,
                                       LTC2308_P1_N0, LTC2308_P3_N2, LTC2308_P5_N4, LTC2308_P7_N6
                                      }; //!< Build the command for differential mode


//! Initialize Linduino
void setup()
{
  uint16_t adc_code;

  quikeval_SPI_init();           // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();        // Connect SPI to main data port
  Serial.begin(115200);          // Initialize the serial port to the PC
  print_title();
  print_prompt();
}


//! Repeats Linduino loop
void loop()
{
  uint16_t user_command;

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
        menu_2_select_single_ended_differential();
        break;
      case 3:
        menu_3_select_uni_bipolar();
        break;
      case 4:
        menu_4_sleep();
        break;
      default:
        Serial.println(F("Invalid Option"));
        break;
    }
    Serial.println();
    Serial.println(F("*************************"));
    print_prompt();
  }
}


// Function Definitions
//! Read channels
void menu_1_read_input()
{
  uint8_t user_command;
  uint16_t adc_command;         // The LTC2308 command byte
  uint16_t adc_code = 0;    // The LTC2308 code
  uint16_t display_code;
  uint8_t x, y, startcount, endcount;
  float adc_voltage;

  if (uni_bipolar == LTC2308_UNIPOLAR_MODE)
  {
    if (single_ended_differential == LTC2308_SINGLE_ENDED_MODE)
    {
      Serial.println(F("  Read Input in Single-Ended, Unipolar mode:"));
      Serial.println(F("  Note that in Unipolar mode, input voltages less than zero are reported as 0.0V"));
      user_command = print_user_command_single_ended();
    }
    else
    {
      Serial.println(F("  Read Input in Differential, Unipolar mode:"));
      Serial.println(F("  Note that in Unipolar mode, input voltages less than zero are reported as 0.0V"));
      user_command = print_user_command_differential();
    }
  }
  else
  {
    if (single_ended_differential == LTC2308_SINGLE_ENDED_MODE)
    {
      Serial.println(F("  Read Input in Single-Ended, Bipolar mode:"));
      user_command = print_user_command_single_ended();
    }
    else
    {
      Serial.println(F("  Read Input in Differential, Bipolar mode:"));
      user_command = print_user_command_differential();
    }
  }

  if (user_command == 8)  //read all channels
  {
    startcount = 0;
    endcount = 7;
  }
  else
  {
    startcount = user_command;
    endcount = user_command;
  }

  if (single_ended_differential == LTC2308_SINGLE_ENDED_MODE) //read single-ended
  {
    adc_command = BUILD_COMMAND_SINGLE_ENDED[0] | uni_bipolar;
    LTC2308_read(LTC2308_CS, adc_command, &adc_code);     // Throws out last reading and starts CH0 conversion
    for (x = startcount; x <= endcount; x++)
    {
      adc_command = BUILD_COMMAND_SINGLE_ENDED[x % 8] | uni_bipolar; // Send channel config for the NEXT conversion to take place
      LTC2308_read(LTC2308_CS, adc_command, &adc_code);     // Throws out last reading and starts CH0 conversion
      LTC2308_read(LTC2308_CS, adc_command, &adc_code);     // Throws out last reading and starts CH0 conversion

      display_code = adc_code >> (16 - LTC2308_bits);
      display_code = display_code & 0xFFF;

      Serial.print(F("  Received Code: b"));
      Serial.println(display_code, BIN);

      adc_voltage = LTC2308_code_to_voltage(adc_code, LTC2308_vref, uni_bipolar);
      Serial.print(F("  Voltage read on "));
      Serial.print(F("Ch"));
      Serial.print(x);
      Serial.print(F(": "));
      Serial.print(adc_voltage, 4);
      Serial.println(F("V"));
      Serial.println();
    }
  }

  else  //read differential
  {
    adc_command = BUILD_COMMAND_SINGLE_ENDED[0] | uni_bipolar;
    LTC2308_read(LTC2308_CS, adc_command, &adc_code);     // Throws out last reading and starts CH0 conversion
    for (x = startcount; x <= endcount; x++)
    {
      adc_command = BUILD_COMMAND_DIFF[x % 8] | uni_bipolar; // Send channel config for the NEXT conversion to take place
      LTC2308_read(LTC2308_CS, adc_command, &adc_code);     // Throws out last reading and starts CH0 conversion
      LTC2308_read(LTC2308_CS, adc_command, &adc_code);   // Read previous channel conversion (x-1) and start next one (x)

      display_code = adc_code >> (16 - LTC2308_bits);
      display_code = display_code & 0xFFF;

      Serial.print(F("  Received Code: b"));
      Serial.println(display_code, BIN);

      adc_voltage = LTC2308_code_to_voltage(adc_code, LTC2308_vref, uni_bipolar);
      Serial.print(F("  Voltage read between Chs "));
      print_channel_selection(x + y);
      Serial.print(adc_voltage, 4);
      Serial.println(F("V"));
      Serial.println();
    }
  }
}


//! Sets LTC2308 to Single-Ended or Differential
void menu_2_select_single_ended_differential()
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
      Serial.println(F("  Single-Ended mode selected"));
      single_ended_differential = LTC2308_SINGLE_ENDED_MODE;
      break;
    case 1:
      Serial.println(F("  Differential mode selected"));
      single_ended_differential = LTC2308_DIFFERENTIAL_MODE;
      break;
    default:
      {
        Serial.println("  Invalid Option");
        return;
      }
      break;
  }
}


//! Select unipolar (0-REFCOMP) or bipolar (+/- 0.5 x REFCOMP) mode
//! @return void
void menu_3_select_uni_bipolar()
{
  uint8_t user_command;

  Serial.println(F("\n  0 = Bipolar"));
  Serial.println(F("  1 = Unipolar"));
  Serial.print(F("  Enter a Command: "));

  user_command = read_int();    // Read user input for uni_bipolar
  Serial.println(user_command);
  switch (user_command)
  {
    case 0:
      Serial.println(F("  Bipolar mode selected"));
      uni_bipolar = LTC2308_BIPOLAR_MODE;
      break;
    case 1:
      Serial.println(F("  Unipolar mode selected"));
      uni_bipolar = LTC2308_UNIPOLAR_MODE;
      break;
    default:
      Serial.println("  Invalid Option");
      return;
      break;
  }
}


//! Put LTC2308 to sleep (low power)
//! @return void
void menu_4_sleep()
{
  // Sleep Mode
  uint16_t user_command;
  uint16_t adc_code = 0;                                        // The LTC2308 code
  LTC2308_read(LTC2308_CS, LTC2308_SLEEP_MODE, &adc_code);      // Build ADC command for sleep mode
  Serial.println();
  Serial.print(F("  ADC Command: B"));
  Serial.println(LTC2308_SLEEP_MODE, BIN);
  Serial.println(F("  LTC2308 Is Now In Sleep Mode"));
  Serial.println(F("  Enter RETURN to exit Sleep Mode"));
  user_command = read_int();
  LTC2308_read(LTC2308_CS, LTC2308_NORMAL_MODE, &adc_code); // Exit Sleep Mode
}//! Prints the title block when program first starts.
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC1186A Demonstration Program                                 *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data and receive data   *"));
  Serial.println(F("* from the LTC2308 12-bit ADC.                                  *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
}


//! Prints main menu.
void print_prompt()
{
  Serial.println(F("1-Read ADC Input "));
  Serial.println(F("2-Select Single-Ended / Differential measurement (default is Single_Ended)"));
  Serial.println(F("3-Select Unipolar / Bipolar measurement (default is Unipolar)"));
  Serial.println(F("4-Select Sleep Mode"));
  Serial.println();
  Serial.print(F("Enter a command:  "));
}


//! Display selected differential channels. Displaying single-ended channels is
//! straightforward; not so with differential because the inputs can take either polarity.
uint8_t print_user_command_differential()
{
  uint8_t user_command;

  Serial.println(F("  *************************"));
  Serial.println(F("  0 = 0P-1N"));
  Serial.println(F("  1 = 2P-3N"));
  Serial.println(F("  2 = 4P-5N"));
  Serial.println(F("  3 = 6P-7N"));
  Serial.println(F("  4 = 1P-0N"));
  Serial.println(F("  5 = 3P-2N"));
  Serial.println(F("  6 = 5P_4N"));
  Serial.println(F("  7 = 7P = 6N"));
  Serial.println(F("  8 = ALL Even_P-Odd_N"));
  Serial.println(F("  m = Main Menu"));
  Serial.println();
  Serial.print(F("  Enter a Command: "));

  user_command = read_int();  // Read the menu command
  Serial.println(user_command);
  if (user_command == 'm')
    return(0);

  switch (user_command)  //check for invalid selection
  {
    case 0:
      Serial.println(F("  0P-1N selected"));
      break;
    case 1:
      Serial.println(F("  2P-3N selected"));
      break;
    case 2:
      Serial.println(F("  4P-5N selected"));
      break;
    case 3:
      Serial.println(F("  6P-7N selected"));
      break;
    case 4:
      Serial.println(F("  1P-0N selected"));
      break;
    case 5:
      Serial.println(F("  3P-2N selected"));
      break;
    case 6:
      Serial.println(F("  5P-4N selected"));
      break;
    case 7:
      Serial.println(F("  7P-6N selected"));
      break;
    case 8:
      Serial.println(F("  ALL Even_P-Odd_N selected"));
      break;
    default:
      Serial.println(F("  Invalid Option"));
      break;
  }
  return(user_command);
}


//! Display selected differential channels. Displaying single-ended channels is
//! straightforward; not so with differential because the inputs can take either polarity.
void print_channel_selection(uint8_t menu)
{
  switch (menu)
  {
    case 0:
      Serial.print(F(" 0P-1N:"));
      break;
    case 1:
      Serial.print(F(" 2P-3N:"));
      break;
    case 2:
      Serial.print(F(" 4P-5N:"));
      break;
    case 3:
      Serial.print(F(" 6P-7N:"));
      break;
    case 4:
      Serial.print(F(" 1P-0N:"));
      break;
    case 5:
      Serial.print(F(" 3P-2N:"));
      break;
    case 6:
      Serial.print(F(" 5P-4N:"));
      break;
    case 7:
      Serial.print(F(" 7P-6N:"));
      break;
  }
}


//! Display selected single-ended channel.
uint8_t print_user_command_single_ended()
{
  uint8_t user_command;
  Serial.println(F("  *************************"));
  Serial.println(F("  0 = CH0"));
  Serial.println(F("  1 = CH1"));
  Serial.println(F("  2 = CH2"));
  Serial.println(F("  3 = CH3"));
  Serial.println(F("  4 = CH4"));
  Serial.println(F("  5 = CH5"));
  Serial.println(F("  6 = CH6"));
  Serial.println(F("  7 = CH7"));
  Serial.println(F("  8 = ALL"));
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
      Serial.println(F("  CH2 selected"));
      break;
    case 3:
      Serial.println(F("  CH3 selected"));
      break;
    case 4:
      Serial.println(F("  CH4 selected"));
      break;
    case 5:
      Serial.println(F("  CH5 selected"));
      break;
    case 6:
      Serial.println(F("  CH6 selected"));
      break;
    case 7:
      Serial.println(F("  CH7 selected"));
      break;
    case 8:
      Serial.println(F("  All selected"));
      break;
    default:
      Serial.println(F("  Invalid Option"));
      break;
  }

  return(user_command);
}

