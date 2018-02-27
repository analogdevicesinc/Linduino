/*!
Linear Technology DC570A Demonstration Board.
LTC2440: 24-Bit, Differential Delta Sigma ADCs with Selectable Speed/Resolution

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

http://www.linear.com/product/LTC2440

http://www.linear.com/product/LTC2440#demoboards


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
    @ingroup LTC2440
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
#include "LTC2440.h"

// Function Declaration
void print_title();                             // Print the title block
void print_prompt();                            // Prompt the user for an input command
void print_user_command(uint8_t menu);          // Display selected differential channels

void menu_1_read_differential();
void menu_2_set_OSR();

// Global variables
static uint8_t demo_board_connected;            //!< Set to 1 if the board is connected
static int16_t OSR_mode = LTC2440_OSR_32768;    //!< The LTC2440 OSR mode settings
static float LTC2440_vref = 5.0;

//! Initialize Linduino
void setup()
{
  char demo_name[]="DC570";     // Demo Board Name stored in QuikEval EEPROM
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
          menu_1_read_differential();
          break;
        case 2:
          menu_set_OSR();
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
  Serial.print(F("* DC570A Demonstration Program                                  *\n"));
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
  Serial.print(F("\n1-Read Differential\n"));
  Serial.print(F("2-OSR Mode Settings\n"));
  Serial.print(F("Enter a Command: "));
}


//! Read channels in differential mode
void menu_1_read_differential()
{
  uint8_t adc_command;      // The LTC2440 command word
  int32_t adc_code = 0;     // The LTC2440 code
  float adc_voltage;        // The LTC2440 voltage
  uint16_t miso_timeout = 1000; // Used to wait for EOC

  adc_command = OSR_mode; // Build the OSR command code

  if (!LTC2440_EOC_timeout(LTC2440_CS, miso_timeout)) // Check for EOC
    LTC2440_read(LTC2440_CS, adc_command, &adc_code);     // Throws out reading
  else
  {
    Serial.println(F("\n   ***SPI Error*** \n"));
    return;
  }

  if (!LTC2440_EOC_timeout(LTC2440_CS, miso_timeout)) // Check for EOC
    LTC2440_read(LTC2440_CS, adc_command, &adc_code);
  else
  {
    Serial.println(F("\n   ***SPI Error*** \n"));
    return;
  }

  Serial.print(F("Received Code: 0x"));
  Serial.println(adc_code, HEX);
  adc_voltage = LTC2440_code_to_voltage(adc_code, LTC2440_vref);
  Serial.print(F("\n  ****"));
  Serial.print(adc_voltage, 4);
  Serial.print(F("V\n"));
}

//! Set the OSR
void menu_set_OSR()
{
  int16_t user_command; // The user input command

  // OSR Mode
  Serial.print(F("OSR Settings\n\n"));
  Serial.print(F("0-64\n"));
  Serial.print(F("1-128\n"));
  Serial.print(F("2-256\n"));
  Serial.print(F("3-512\n"));
  Serial.print(F("4-1024\n"));
  Serial.print(F("5-2048\n"));
  Serial.print(F("6-4096\n"));
  Serial.print(F("7-8192\n"));
  Serial.print(F("8-16384\n"));
  Serial.print(F("9-32768\n"));
  Serial.print(F("Enter a Command: "));
  user_command = read_int();
  Serial.println(user_command);

  switch (user_command)
  {
    case 0:
      OSR_mode = LTC2440_OSR_64;
      break;
    case 1:
      OSR_mode = LTC2440_OSR_128;
      break;
    case 2:
      OSR_mode = LTC2440_OSR_256;
      break;
    case 3:
      OSR_mode = LTC2440_OSR_512;
      break;
    case 4:
      OSR_mode = LTC2440_OSR_1024;
      break;
    case 5:
      OSR_mode = LTC2440_OSR_2048;
      break;
    case 6:
      OSR_mode = LTC2440_OSR_4096;
      break;
    case 7:
      OSR_mode = LTC2440_OSR_8192;
      break;
    case 8:
      OSR_mode = LTC2440_OSR_16384;
      break;
    case 9:
      OSR_mode = LTC2440_OSR_32768;
      break;
  }
}
