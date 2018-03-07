/*!
Linear Technology DC2071A Demonstration Board.

LTC2373: 16/18-bit 1Msps 8 channel SAR ADC
LTC2372: 16/18-bit 500ksps 8 channel SAR ADC
LTC2374: 16 bit 1.6Msps 8 channel SAR ADC

Max SCK rate is 100MHz.

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.
   Equipment required is a precision voltage source (null box) and a precision voltmeter (to monitor voltage source).
   No external power supply is required.
   Ensure JP1 is installed in the default position from the factory.

  How to test:
   The voltage source should be connected with positive and negative leads to the positive & negative ADC inputs. Ensure the differential voltage is within the
   range of -VREF to +VREF. Swapping input voltages results in a reversed polarity reading.

  How to calibrate:
   Enter menu item number "3 - Calibrate" and follow the prompts.
   Calibration is now stored in EEPROM. Upon startup the calibration values will be restored.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2373-16
http://www.linear.com/product/LTC2373_18
http://www.linear.com/product/LTC2372-16
http://www.linear.com/product/LTC2372-18
http://www.linear.com/product/LTC2374-16

http://www.linear.com/product/LTC2373-16#demoboards
http://www.linear.com/product/LTC2373_18#demoboards
http://www.linear.com/product/LTC2372-16#demoboards
http://www.linear.com/product/LTC2372-18#demoboards
http://www.linear.com/product/LTC2374-16#demoboards


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
    @ingroup LTC2373
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC2373.h"
#include <SPI.h>
#include <Wire.h>

// Function Declaration
void print_title();                                         // Print the title block
void print_prompt();                                        // Prompt the user for an input command
void print_user_command(uint8_t menu);                      // Display selected differential channels

void menu_1_read_adc();
void menu_2_program_sequencer();
void menu_3_read_sequencer();
void menu_4_select_bits();
void menu_5_select_range();
void menu_6_select_gain_compression();

// Global variables
static uint8_t adc_command;
static uint8_t user_command;
static uint32_t adc_code;                           // The LTC2373 code
static int32_t display_code;
float adc_voltage;                               // The LTC2373 voltage
float LTC2373_vref = 4.096;  //reference voltage in volts
static uint8_t LTC2373_range_select = LTC2373_RANGE_UNIPOLAR;    //!< Default set for single-ended unipolar mode
static uint8_t LTC2373_gain_compression = LTC2373_NO_COMPRESSION;   //!< Default set for no compression mode
static uint8_t LTC2373_bits = 18;                   //!< Default set for 18 bits
static uint8_t LTC2373_sequencer_bit = LTC2373_SEQUENCER_BIT;

// Constants
//! Lookup table to build the command for single-ended mode, input with respect to GND
const uint8_t COMMAND_SINGLE_ENDED[8] = {LTC2373_CH0, LTC2373_CH1, LTC2373_CH2, LTC2373_CH3,
                                        LTC2373_CH4, LTC2373_CH5, LTC2373_CH6, LTC2373_CH7
                                        }; //!< Builds the command for single-ended mode, input with respect to GND

//! Lookup table to build the command for differential mode
const uint8_t COMMAND_DIFF[8] = {LTC2373_CH0_1, LTC2373_CH2_3, LTC2373_CH4_5, LTC2373_CH6_7,
                                 LTC2373_CH1_0, LTC2373_CH3_2, LTC2373_CH5_4, LTC2373_CH7_6
                                }; //!< Builds the command for differential mode


//! Initialize Linduino
void setup()
{
  quikeval_I2C_init();           // Configure the EEPROM I2C port for 100kHz
  quikeval_SPI_init();           // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();        // Connect SPI to main data port
  Serial.begin(115200);          // Initialize the serial port to the PC
  print_title();
  print_prompt();
}


//! Repeats Linduino loop
void loop()
{
//  uint16_t user_command;
  {
    if (Serial.available())
    {
      user_command = read_int();        // Read the user command
      if (user_command != 'm')
        Serial.println(user_command);   // Prints the user command to com port
      switch (user_command)
      {
        case 1:
          menu_1_read_adc();
          break;
        case 2:
          menu_2_program_sequencer();
          break;
        case 3:
          menu_3_read_sequencer();
          break;
        case 4:
          menu_4_select_bits();
          break;
        case 5:
          menu_5_select_range();
          break;
        case 6:
          menu_6_select_gain_compression();
          break;
        default:
          Serial.println("Invalid Option");
          break;
      }
      Serial.println();
      print_prompt();
    }
  }
}


// Function Definitions
//! Read channels in single-ended mode
//! @return void
void menu_1_read_adc()
{
  uint8_t single_ended = 0;
  uint8_t i;                                                    //!< iteration variable

  i2c_write_byte(I2C_ADDRESS, I2C_COMMAND);  //disable the CPLD communication

  while (1)
  {
    switch (LTC2373_range_select)
    {
      case LTC2373_RANGE_UNIPOLAR:
        single_ended = 1;
        Serial.println("");
        Serial.println("");
        Serial.println(F("   Single-Ended, Unipolar mode, 0 to Vref:"));
        Serial.println(F("    0-CH0"));
        Serial.println(F("    1-CH1"));
        Serial.println(F("    2-CH2"));
        Serial.println(F("    3-CH3"));
        Serial.println(F("    4-CH4"));
        Serial.println(F("    5-CH5"));
        Serial.println(F("    6-CH6"));
        Serial.println(F("    7-CH7"));
        Serial.println(F("    8-ALL"));
        Serial.println(F("    m-Main Menu"));
        break;

      case LTC2373_RANGE_BIPOLAR:
        single_ended = 1;
        Serial.println("");
        Serial.println("");
        Serial.println(F("   Single-Ended, Bipolar mode, -Vref/2 to +Vref/2:"));
        Serial.println(F("    0-CH0"));
        Serial.println(F("    1-CH1"));
        Serial.println(F("    2-CH2"));
        Serial.println(F("    3-CH3"));
        Serial.println(F("    4-CH4"));
        Serial.println(F("    5-CH5"));
        Serial.println(F("    6-CH6"));
        Serial.println(F("    7-CH7"));
        Serial.println(F("    8-ALL"));
        Serial.println(F("    m-Main Menu"));
        break;

      case LTC2373_RANGE_DIFF_UNIPOLAR:
        Serial.println("");
        Serial.println("");
        Serial.println(F("   Differential, Unipolar mode, 0 to Vref:"));
        Serial.println(F("    0-0P-1N"));
        Serial.println(F("    1-2P-3N"));
        Serial.println(F("    2-4P-5N"));
        Serial.println(F("    3-6P-7N"));
        Serial.println(F("    4-1P-0N"));
        Serial.println(F("    5-3P-2N"));
        Serial.println(F("    6-5P_4N"));
        Serial.println(F("    7-7P-6N"));
        Serial.println(F("    8-ALL Even_P-Odd_N"));
        Serial.println(F("    m-Main Menu"));
        break;

      case LTC2373_RANGE_DIFF_BIPOLAR:
        Serial.println("");
        Serial.println("");
        Serial.println(F("   Differential Bipolar mode, -Vref to +Vref:"));
        Serial.println(F("    0-0P-1N"));
        Serial.println(F("    1-2P-3N"));
        Serial.println(F("    2-4P-5N"));
        Serial.println(F("    3-6P-7N"));
        Serial.println(F("    4-1P-0N"));
        Serial.println(F("    5-3P-2N"));
        Serial.println(F("    6-5P_4N"));
        Serial.println(F("    7-7P-6N"));
        Serial.println(F("    8-ALL"));
        Serial.println(F("    m-Main Menu"));
        break;

      default:
        Serial.println("    Invalid Option");
        break;
    }

    Serial.println();
    Serial.print(F("    Enter a Command: "));
    user_command = read_int();                                 // Read the single command
    if (user_command == 'm')
      return;

    switch (user_command)
    {
      case 0:
        ;
        break;
      case 1:
        ;
        break;
      case 2:
        ;
        break;
      case 3:
        ;
        break;
      case 4:
        ;
        break;
      case 5:
        ;
        break;
      case 6:
        ;
        break;
      case 7:
        ;
        break;
      case 8:
        ;
        break;
      default:
        {
          Serial.println("    Invalid Option");
          return;
        }
        break;
    }

    Serial.println(user_command);

    switch (single_ended)
    {
      case 0:  //differential mode
        if (user_command == 8)  //read all 8 channels
        {
          for (i = 0; i <= 7; i++)                                      // Read all channels in differential mode
          {
            adc_command = LTC2373_build_command(LTC2373_SEQUENCER_BIT, COMMAND_DIFF[(i) % 8], LTC2373_range_select, LTC2373_gain_compression);
            LTC2373_read(LTC2373_CS, adc_command, &adc_code);
            delay(100);
            LTC2373_read(LTC2373_CS, 0, &adc_code);

            display_code = adc_code >> (32 - LTC2373_bits);
            if (LTC2373_bits == 16) //grab the leftmost 16 bits
              display_code = display_code & 0xFFFF;
            else  //grab the leftmost 18 bits
              display_code = display_code & 0x3FFFF;

            print_user_command(i);
            Serial.print(F(" Received ADC Code: 0x"));
            Serial.println(display_code, HEX);

            // Convert the received code to voltage
            adc_voltage = LTC2373_code_to_voltage(adc_command, adc_code, LTC2373_vref);
            Serial.print(F("      Equivalent voltage: "));
            Serial.print(adc_voltage, 4);
            Serial.println(F("V"));
            Serial.println();
          }
        }
        else  //read 1 channel
        {
          i = user_command;
          adc_command = LTC2373_build_command(LTC2373_SEQUENCER_BIT, COMMAND_DIFF[i], LTC2373_range_select, LTC2373_gain_compression);
          LTC2373_read(LTC2373_CS, adc_command, &adc_code); // Throws out last reading
          delay(100);
          LTC2373_read(LTC2373_CS, 0, &adc_code);

          display_code = adc_code >> (32 - LTC2373_bits);
          if (LTC2373_bits == 16) //grab the leftmost 16 bits
            display_code = display_code & 0xFFFF;
          else  //grab the leftmost 18 bits
            display_code = display_code & 0x3FFFF;

          print_user_command(i);
          Serial.print(F(" Received ADC Code: 0x"));
          Serial.println(display_code, HEX);

          // Convert the received code to voltage
          adc_voltage = LTC2373_code_to_voltage(adc_command, adc_code, LTC2373_vref);
          Serial.print(F("      Equivalent voltage: "));
          Serial.print(adc_voltage, 4);
          Serial.println(F("V"));
          Serial.println();
        }
        break;

      case 1:  //single-ended mode
        if (user_command == 8)  //read all 8 channels
        {
          for (i = 0; i <= 7; i++)                                      // Read all channels in single-ended mode
          {
            adc_command = LTC2373_build_command(LTC2373_SEQUENCER_BIT, COMMAND_SINGLE_ENDED[(i) % 8], LTC2373_range_select, LTC2373_gain_compression);
            LTC2373_read(LTC2373_CS, adc_command, &adc_code);  // Program sequencer once
            delay(100);
            LTC2373_read(LTC2373_CS, 0, &adc_code);  //read adc

            display_code = adc_code >> (32 - LTC2373_bits);
            if (LTC2373_bits == 16) //grab the leftmost 16 bits
              display_code = display_code & 0xFFFF;
            else  //grab the leftmost 18 bits
              display_code = display_code & 0x3FFFF;

            Serial.print(F("      CH"));
            Serial.print(i);
            Serial.print(F(" Received ADC Code: 0x"));
            Serial.println(display_code, HEX);

            // Convert the received code to voltage
            adc_voltage = LTC2373_code_to_voltage(adc_command, adc_code, LTC2373_vref);
            Serial.print(F("      Equivalent voltage: "));
            Serial.print(adc_voltage, 4);
            Serial.println(F("V"));
            Serial.println();
          }
        }

        else  //read 1 channel
        {
          i = user_command;
          adc_command = LTC2373_build_command(LTC2373_SEQUENCER_BIT, COMMAND_SINGLE_ENDED[i], LTC2373_range_select, LTC2373_gain_compression);
          LTC2373_read(LTC2373_CS, adc_command, &adc_code); // Program sequencer once
          delay(100);
          LTC2373_read(LTC2373_CS, 0, &adc_code);  //read adc

          display_code = adc_code >> (32 - LTC2373_bits);
          if (LTC2373_bits == 16) //grab the leftmost 16 bits
            display_code = display_code & 0xFFFF;
          else  //grab the leftmost 18 bits
            display_code = display_code & 0x3FFFF;

          Serial.print(F("      CH"));
          Serial.print(i);
          Serial.print(F(" Received ADC Code: 0x"));
          Serial.println(display_code, HEX);

          // Convert the received code to voltage
          adc_voltage = LTC2373_code_to_voltage(adc_command, adc_code, LTC2373_vref);
          Serial.print(F("      Equivalent voltage: "));
          Serial.print(adc_voltage, 4);
          Serial.println(F("V"));
          Serial.println();
        }
        break;
    }
  }
}


//! Program the sequencer
//! @return void
void menu_2_program_sequencer()
{
  uint8_t temp_bits = 0;

  Serial.println();
  Serial.println(F("*****************************************************************************************"));
  Serial.println(F("* This routine demonstrates how to program the sequencer.                               *"));
  Serial.println(F("*                                                                                       *"));
  Serial.println(F("* A specific set of four configurations will be programmed:                             *"));
  Serial.println(F("*   A single-ended unipolar voltage on AIN1+                                            *"));
  Serial.println(F("*   A differential unipolar voltage on AIN1+/AIN1-, which feeds the ADC differentially  *"));
  Serial.println(F("*   A single-ended bipolar voltage on AIN3, which feeds the ADC differentially          *"));
  Serial.println(F("*   A differential bipolar voltage on AIN4+/AIN4-                                       *"));
  Serial.println(F("*                                                                                       *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.                        *"));
  Serial.println(F("*                                                                                       *"));
  Serial.println(F("*****************************************************************************************"));

//Form a four byte object to hold four bytes of dummy data
  LT_union_int32_4bytes adc_configuration;  //instantiate the union

  adc_configuration.LT_byte[0] = LTC2373_build_command(LTC2373_SEQUENCER_BIT, COMMAND_SINGLE_ENDED[0], LTC2373_RANGE_UNIPOLAR, LTC2373_NO_COMPRESSION);
  adc_configuration.LT_byte[1] = LTC2373_build_command(LTC2373_SEQUENCER_BIT, COMMAND_DIFF[4], LTC2373_RANGE_DIFF_UNIPOLAR, LTC2373_NO_COMPRESSION);
  adc_configuration.LT_byte[2] = LTC2373_build_command(LTC2373_SEQUENCER_BIT, COMMAND_DIFF[6], LTC2373_RANGE_DIFF_BIPOLAR, LTC2373_NO_COMPRESSION);
  adc_configuration.LT_byte[3] = LTC2373_build_command(LTC2373_SEQUENCER_BIT, COMMAND_DIFF[3], LTC2373_RANGE_DIFF_BIPOLAR, LTC2373_NO_COMPRESSION);

  LTC2373_configure(LTC2373_CS, adc_configuration.LT_uint32); // Program sequencer once

  Serial.println(F("  Write Codes: "));
  Serial.print(F("    Register C3 adc config code: "));
  Serial.println(adc_configuration.LT_byte[0] & 0x7F, BIN);
  Serial.print(F("    Register C2 adc config code: "));
  Serial.println(adc_configuration.LT_byte[1] & 0x7F, BIN);
  Serial.print(F("    Register C1 adc config code: "));
  Serial.println(adc_configuration.LT_byte[2] & 0x7F, BIN);
  Serial.print(F("    Register C0 adc config code: "));
  Serial.println(adc_configuration.LT_byte[3] & 0x7F, BIN);
  Serial.println("");
}


//! Program the sequencer
//! @return void
void menu_3_read_sequencer()
{
  uint8_t i, mode;

  for (i = 0; i < 4; i++)
  {
    LTC2373_read(LTC2373_CS, 0, &adc_code);  //Read the data

    display_code = adc_code >> (32 - LTC2373_bits);
    if (LTC2373_bits == 16) //grab the leftmost 16 bits
      display_code = display_code & 0xFFFF;
    else  //grab the leftmost 18 bits
      display_code = display_code & 0x3FFFF;

    adc_command = (adc_code >> 6) & 0x7F;

    Serial.print(F("    Register C"));
    Serial.println(i);
    mode = (0x06 & adc_command) >> 1;

    switch (mode)
    {
      case (0):
        Serial.println(F("    Unipolar, 0 to Vref"));
        break;
      case (1):
        Serial.println(F("    Bipolar, -Vref/2 to +Vref/2"));
        break;
      case (2):
        Serial.println(F("    Differential unipolar, 0 to Vref"));
        break;
      case (3):
        Serial.println(F("    Differential bipolar, -Vref to +Vref"));
        break;
    }

    Serial.print(F("    Readback adc config: "));
    Serial.println(adc_command, BIN);

    Serial.print(F("    Received Code: 0x"));
    Serial.println(display_code, HEX);

    // Convert the received code to voltage
    adc_voltage = LTC2373_code_to_voltage(adc_command, adc_code, LTC2373_vref);

    Serial.print(F("    Equivalent voltage: "));
    Serial.print(adc_voltage, 4);
    Serial.println(F("V"));
    Serial.println("");
    Serial.println("");
  }
}


//! Select number of bits
//! @return void
void menu_4_select_bits()
{
  uint8_t user_command;

  Serial.println();
  if (LTC2373_bits == 16)
    Serial.println(F("    16 bits selected"));
  else
    Serial.println(F("    18 bits selected"));

  Serial.println(F("    16 = 2373-16"));
  Serial.println(F("    18 = 2373-18"));
  Serial.println(F(""));
  Serial.print(F("    Enter a Command, based upon the resolution of the part under test: "));

  user_command = read_int();    // Read user input
  Serial.println(user_command);   // Prints the user command to com port
  switch (user_command)
  {
    case 16:
      LTC2373_bits = 16;
      break;
    case 18:
      LTC2373_bits = 18;
      break;
    default:
      {
        Serial.println("    Invalid Option");
        return;
      }
      break;
  }
}


//! Select range
//! @return void
void menu_5_select_range()
{
  uint8_t user_command;

  Serial.println(F("    0 = Unipolar, 0 to Vref"));
  Serial.println(F("    1 = Bipolar, -Vref/2 to +Vref/2"));
  Serial.println(F("    2 = Differential unipolar, 0 to Vref"));
  Serial.println(F("    3 = Differential bipolar, -Vref to +Vref"));
  Serial.println(F(""));
  Serial.print(F("    Enter a Command: "));
  user_command = read_int();    // Read user input for range select
  Serial.println(user_command);

  switch (user_command)
  {
    case 0:
      Serial.println(F("    Unipolar input range selected; set JP9 to 'GND' position"));
      LTC2373_range_select = LTC2373_RANGE_UNIPOLAR;
      break;
    case 1:
      Serial.println(F("    Bipolar input range selected; set JP9 to 'CM' position"));
      LTC2373_range_select = LTC2373_RANGE_BIPOLAR;
      break;
    case 2:
      Serial.println(F("    Differential Unipolar input range selected"));
      Serial.println(F("    Note that this range operates only for the following configurations:"));
      Serial.println(F("        CH 0P-1N, with AIN1+ connected to GND"));
      Serial.println(F("        CH 1P-0N, with AIN1- connected to GND"));
      LTC2373_range_select = LTC2373_RANGE_DIFF_UNIPOLAR;
      break;
    case 3:
      Serial.println(F("    Differential Bipolar input range selected"));
      LTC2373_range_select = LTC2373_RANGE_DIFF_BIPOLAR;
      break;
    default:
      {
        Serial.println("    Invalid Option");
        return;
      }
      break;
  }
}


//! Select gain compression
//! @return void
void menu_6_select_gain_compression()
{
  uint8_t user_command;
  Serial.println();
  if (LTC2373_gain_compression == 0)
    Serial.println(F("    No gain compression enabled"));
  else
    Serial.println(F("    Gain compression enabled"));

  Serial.println(F("    0 = No Gain Compression"));
  Serial.println(F("    1 = Gain Compression"));
  Serial.println(F(""));
  Serial.print(F("    Enter a Command: "));

  user_command = read_int();    // Read user input
  Serial.println(user_command);   // Prints the user command to com port
  switch (user_command)
  {
    case 0:
      LTC2373_gain_compression = 0;
      break;
    case 1:
      LTC2373_gain_compression = 1;
      break;
    default:
      {
        Serial.println("    Invalid Option");
        return;
      }
      break;
  }
}


//! Prints the title block when program first starts.
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC2071A Demonstration Program                                 *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to receive data                 *"));
  Serial.println(F("* from the LTC2373-16/18 ADC.                                   *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
}


//! Prints main menu.
void print_prompt()
{
  Serial.println("");
  Serial.println(F("  1-Read ADC Input"));
  Serial.println(F("  2-Program the Sequencer"));
  Serial.println(F("  3-Read the Sequencer"));
  Serial.println(F("  4-Select Number of Bits (Default is 18 bits)"));
  Serial.println(F("  5-Select Range (Default is Single-Ended Unipolar Range)"));
  Serial.println(F("  6-Select Gain Compression (Default is No Gain Compression)\n"));
  Serial.println(F(""));
  Serial.print(F("  Enter a command: "));
}


//! Display selected differential channels. Displaying single-ended channels is
//! straightforward; not so with differential because the inputs can take either polarity.
void print_user_command(uint8_t menu)
{
  switch (menu)
  {
    case 0:
      Serial.print(F("     CH 0P-1N"));
      break;
    case 1:
      Serial.print(F("     CH 2P-3N"));
      break;
    case 2:
      Serial.print(F("     CH 4P-5N"));
      break;
    case 3:
      Serial.print(F("     CH 6P-7N"));
      break;
    case 4:
      Serial.print(F("     CH 1P-0N"));
      break;
    case 5:
      Serial.print(F("     CH 3P-2N"));
      break;
    case 6:
      Serial.print(F("     CH 5P-4N"));
      break;
    case 7:
      Serial.print(F("     CH 7P-6N"));
      break;
  }
}

