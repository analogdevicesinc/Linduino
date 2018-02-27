/*!
Linear Technology DC1716A Demonstration Board.
LTC2473: 16-Bit, Delta Sigma ADC with I2C interface

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. Equipment
   required is a precision voltage source and a precision voltmeter. Additionally,
   an external power supply is required to provide a negative voltage for Amp V-.
   Set it to anywhere from -1V to -5V. Set Amp V+ to Vcc. Ensure the COM and REF-
   pins are connected to ground. The REF+ pin should be connected to +5V.

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

http://www.linear.com/product/LTC2473

http://www.linear.com/product/LTC2473#demoboards


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
    @ingroup LTC2473
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include <SPI.h>
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC2473.h"
#include "LTC24XX_general.h"

// Function Declaration
void print_title();                             // Print the title block
void print_prompt();                            // Prompt the user for an input command
void print_user_command(uint8_t menu);          // Display selected differential channels

int8_t menu_1_read_differential();
int8_t menu_2_set_sps();
uint8_t menu_3_sleep();
void menu_4_set_address();

// Global variables
static float LTC2473_vref = 1.25;                   //!< The nominal reference voltage
static uint8_t i2c_address = LTC2473_I2C_ADDRESS;   //!< I2C address in 7 bit format for part
static uint16_t timeout = 300;                      //!< The timeout in microseconds

//! Initialize Linduino
void setup()
{
  uint8_t demo_board_connected; // Set to 1 if the board is connected
  char demo_name[]="DC1716";    // Demo Board Name stored in QuikEval EEPROM
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
    print_prompt();
  }
}

//! Repeats Linduino loop
void loop()
{
  int16_t user_command;                 // The user input command
  uint8_t acknowledge = 0;
  if (Serial.available())             // Check for user input
  {
    user_command = read_int();        // Read the user command
    if (user_command != 'm')
      Serial.println(user_command);   // Prints the user command to com port
    Serial.flush();
    switch (user_command)
    {
      case 1:
        acknowledge |= menu_1_read_differential();
        break;
      case 2:
        acknowledge |= menu_2_set_sps();
        break;
      case 3:
        acknowledge |= menu_3_sleep();
        break;
      case 4:
        menu_4_set_address();
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

// Function Definitions

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC1716A Demonstration Program                                 *\n"));
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
  Serial.print(F("\n1-Read Differential\n"));
  Serial.print(F("2-Set SPS\n"));
  Serial.print(F("3-Sleep Mode\n"));
  Serial.print(F("4-Set I2C Address\n"));
  Serial.print(F("Enter a Command: "));
}

//! Read the ADC
//! @return 0 if successful, 1 is failure
int8_t menu_1_read_differential()
{
  int32_t adc_code = 0;     // The LTC2473 code
  float adc_voltage;        // The LTC2473 voltage
  uint8_t ack = 0;          // Acknowledge bit
  int16_t user_command;     // The user input command

  ack |= LTC2473_read(i2c_address, &adc_code, timeout);  // Throws out reading
  delay(100);
  ack |= LTC2473_read(i2c_address, &adc_code, timeout);  // Reads the ADC

  if (!ack)
  {
    Serial.print(F("Received Code: 0x"));
    Serial.println(adc_code >> 14, HEX);
    adc_voltage = LTC2473_code_to_voltage(adc_code, LTC2473_vref);
    Serial.print(F("\n  ****"));
    Serial.print(adc_voltage, 4);
    Serial.print(F("V\n"));
  }
  else
  {
    Serial.println(F("Device NAK'd, please check I2C address"));
    return 1;
  }
  return(0);
}

//! Set the SPS
int8_t menu_2_set_sps()
{
  uint8_t ack = 0;
  uint8_t adc_command;      // The LTC2473 command byte
  int16_t user_command;     // The user input command

  Serial.print(F("Samples Per Second Mode Settings\n\n"));
  Serial.print(F("0-208 SPS\n"));
  Serial.print(F("1-833 SPS\n"));
  Serial.print(F("Enter a Command: "));
  user_command = read_int();

  if (user_command == 0)
  {
    ack |= LTC2473_write(i2c_address, LTC2473_ENABLE_PROGRAMMING | LTC2473_SPS_208);
    Serial.print(F("208 SPS Mode \n"));
  }
  else
  {
    ack |= LTC2473_write(i2c_address, LTC2473_ENABLE_PROGRAMMING | LTC2473_SPS_833);
    Serial.print(F("833 SPS Mode \n"));
  }
  return(ack);
}


//! Set LTC2473 to sleep mode
uint8_t menu_3_sleep()
{
  uint8_t ack = 0;
  ack |= LTC2473_write(i2c_address, LTC2473_ENABLE_PROGRAMMING | LTC2473_SLEEP);

  Serial.println(F("\nThe LTC2473 is in sleep mode"));
  return(ack);
}

//! Set the I2C 7 bit address
void menu_4_set_address()
{
  int16_t user_command;
  Serial.print(F("What is the I2C address of the part?\n"));
  Serial.print(F("Please enter in 7-bit format, decimal\n"));

  user_command =read_int();
  Serial.println(user_command);
  i2c_address = user_command&0x7F;
}