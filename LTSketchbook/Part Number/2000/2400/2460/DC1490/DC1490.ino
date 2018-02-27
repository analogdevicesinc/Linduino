/*!
Linear Technology DC1490A-A Demonstration Board.
LTC2460: 24-Bit, 16-Channel Delta Sigma ADC with SPI interface

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

http://www.linear.com/product/LTC2460

http://www.linear.com/product/LTC2460#demoboards


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
    @ingroup LTC2460
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
#include "LTC2460.h"

// Function Declaration
void print_title();                             // Print the title block
void print_prompt();                            // Prompt the user for an input command

void menu_1_read_single_ended();
void menu_2_set_1X2X();

// Global variables
static uint8_t demo_board_connected;            //!< Set to 1 if the board is connected
static int16_t two_x_mode = LTC2460_SPEED_1X;   //!< The LTC2460 2X Mode settings
static float LTC2460_vref = 1.25;
static uint16_t eoc_timeout = 25;
// Constants

//! Lookup table to build 1X / 2X bits
const uint16_t BUILD_1X_2X_COMMAND[2] = {LTC2460_SPEED_1X, LTC2460_SPEED_2X};   //!< Build the command for 1x or 2x mode


//! Initialize Linduino
void setup()
{
  char demo_name[]="DC1490";    // Demo Board Name stored in QuikEval EEPROM
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
          menu_1_read_single_ended();
          break;
        case 2:
          menu_2_set_1X2X();
          break;
        case 3:
          menu_3_sleep();
          break;
        default:
          Serial.println(F("Incorrect Option"));
      }
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
  Serial.print(F("* DC1490A Demonstration Program                               *\n"));
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
  Serial.print(F("2-2X Mode Settings\n"));
  Serial.print(F("3-Sleep\n"));
  Serial.print(F("Enter a Command: "));
}

//! Read channels in single-ended mode
void menu_1_read_single_ended()
{
  uint16_t adc_command;     // The LTC2460 command word
  int16_t user_command;     // The user input command
  int32_t adc_code = 0;     // The LTC2460 code
  float adc_voltage=0;      // The LTC2460 voltage

  while (1)
  {

    Serial.print(F("*************************\n\n"));
    Serial.print(F("1-Read\n"));
    Serial.print(F("m-Main Menu\n"));
    Serial.print(F("Enter a Command: "));

    user_command = read_int();                                      // Read the single command
    if (user_command == 'm')
    {
      break;
    }

    else
    {
      Serial.println(user_command);
      adc_command = two_x_mode;
      Serial.print(F("ADC Command: 0x"));
      Serial.println(adc_command, HEX);

      quikeval_SPI_connect();
      LTC2460_read(LTC2460_CS, adc_command, &adc_code);     // Throws out last reading

      delay(50);

      LTC2460_read(LTC2460_CS, adc_command, &adc_code);     // Now we're ready to read the desired data

      Serial.print(F("Received Code: 0x"));
      Serial.println(((adc_code>>16) & 0x0000FFFF), HEX);
      adc_voltage = LTC2460_code_to_voltage(adc_code, LTC2460_vref);
      Serial.print(F("  ****"));
      Serial.print(F("Voltage"));
      Serial.print(F(": "));
      Serial.print(adc_voltage, 4);
      Serial.print(F("V\n\n"));
    }


  }


}

//! Set 1X or 2X mode
void menu_2_set_1X2X()
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
    two_x_mode = LTC2460_SPEED_1X;
    Serial.print(F("2X Mode Disabled, offset calibration enabled\n"));
  }
  else
  {
    two_x_mode = LTC2460_SPEED_2X;
    Serial.print(F("2X Mode Enabled, offset calibration disabled\n"));
  }
}

void menu_3_sleep()
{
  int32_t adc_code = 0;     // The LTC2460 code
  Serial.print("Putting LTC2460 into sleep mode");
  LTC2460_read(LTC2460_CS, 0x90, &adc_code); // Send sleep bit
}