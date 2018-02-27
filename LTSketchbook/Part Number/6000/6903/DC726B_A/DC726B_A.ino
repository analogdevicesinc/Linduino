/*!
Linear Technology DC726B-A (or DC726A-A) Demonstration Board.
LTC6903: 1kHz to 68MHz Serial Port Programmable Oscillator

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator. Equipment
   required is a precision voltage source and a precision voltmeter (to monitor
   voltage source). No external power supply is required. Ensure JP1 is installed in
   the default position from the factory.

  Demo Board Configuration:
   To properly use this program with the DC72B-A (or the DC726A-A) demo board, JMP 2
   needs to be set to QuikEval.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC6903

http://www.linear.com/product/LTC6903#demoboards


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
    @ingroup LTC6903
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LT_I2C.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC6903.h"
#include <SPI.h>


// Function Declaration
void print_title();
void print_prompt();
void settings();
void set_frequency();
void manually_set_reg();


// Global variables
static uint8_t output_config = LTC6903_CLK_ON_CLK_INV_ON;  //!< Keeps track of output configuration.

//! Initialize Linduino
void setup()
{
  quikeval_SPI_init();           // Configure the EEPROM I2C port for 100kHz
  quikeval_SPI_connect();        // Connect SPI to main data port
  Serial.begin(115200);          // Initialize the serial port to the PC
  print_title();
  print_prompt();
}

//! Repeats Linduino loop
void loop()
{
  int16_t user_command;

  user_command = read_int();

  Serial.println(user_command);

  switch (user_command)
  {
    case 1:
      set_frequency();
      break;
    case 2:
      manually_set_reg();
      break;
    case 3:
      settings();
      break;
    default:
      Serial.println(F("Invalid command"));
      break;
  }
  print_prompt();
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC726B Demonstration Program                                  *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates how to send data to the             *"));
  Serial.println(F("* programmable oscillator                                       *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.println();
  Serial.println(F("1-Set frequency out"));
  Serial.println(F("2-Manually set OCT and DAC code"));
  Serial.println(F("3-Settings"));
  Serial.println();
  Serial.print(F("Enter a command:"));
}

//! Sets the output frequency.
void set_frequency()
{
  float freq;
  uint16_t clock_code;
  uint8_t ack;

  Serial.print(F("Enter the desired clock freq (KHz):"));

  freq = read_float();

  Serial.println(freq, 4);

  // Calculates the code necessary to create the clock frequency
  clock_code = LTC6903_frequency_to_code(freq/1000, output_config);


  LTC6903_write(LTC6903_CS, (uint16_t)clock_code);
}

//! Manually Sets OCT and DAC Code
void manually_set_reg()
{
  uint8_t oct;
  uint8_t ack;
  uint16_t dac_code;

  Serial.print(F("Enter the OCT:"));

  oct = read_int();
  if (oct > 15)
    oct = 15;
  Serial.println(oct);

  Serial.println();
  Serial.print(F("Enter the DAC code:"));

  dac_code = read_int();

  Serial.println(dac_code);
  Serial.println();

  // Manually set registers
  LTC6903_write(LTC6903_CS, (uint16_t)((oct<<12)|(dac_code<<2)|output_config));
}

//! Configures the output
void settings()
{
  int16_t user_command;

  Serial.println();
  Serial.println(F("Output Configuration"));
  Serial.println(F("1-CLK On and CLK INV ON"));
  Serial.println(F("2-CLK Off and CLK INV ON"));
  Serial.println(F("3-CLK On and CLK INV Off"));
  Serial.println(F("4-Power Down"));
  Serial.println();
  Serial.print(F("Enter a command:"));

  user_command = read_int();

  Serial.println(user_command);

  switch (user_command)
  {
    case 1:
      output_config = LTC6903_CLK_ON_CLK_INV_ON;
      break;
    case 2:
      output_config = LTC6903_CLK_OFF_CLK_INV_ON;
      break;
    case 3:
      output_config = LTC6903_CLK_ON_CLK_INV_OFF;
      break;
    case 4:
      output_config = LTC6903_POWER_DOWN;
      break;
    default:
      Serial.println(F("Invalid command"));
  }
}
