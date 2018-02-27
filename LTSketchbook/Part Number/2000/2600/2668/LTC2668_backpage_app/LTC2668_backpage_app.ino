/*!
Linear Technology DC2025A Demonstration Board.
LTC2668: 16 Channel SPI 16-/12-Bit Rail-to-Rail DACs with 10ppm/C Max Reference.

@verbatim
NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

   This program uses the DC1908A along with the DC2025A demo board.
   The program is based on an application note located near the end
   of the LTC2668 datasheet. Follow the schematic to properly connect
   the Linduino to the two demo boards.

   An external +- 15V power supply is required to power the circuit.

   The program displays calculated voltages which are based on the voltage
   of the reference used, be it internal or external. A precision voltmeter
   is needed to verify the actual measured voltages against the calculated
   voltage displayed. If an external reference is used, a precision voltage
   source is required to apply the external reference voltage. A
   precision voltmeter is also required to measure the external reference
   voltage.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2668

http://www.linear.com/product/LTC2668#demoboards


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
   @ingroup LTC2668
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC2668.h"
#include "LTC2378.h"
#include <SPI.h>
#include <Wire.h>

#define LTC2378_CS 9

// Function Decoration
void demo_board_test();
void print_title();
void print_prompt();

//! Initialize Linduino
void setup()
// Setup the program
{
  // Set The Linduino Auxiliary Pin To be The LTC2378 Chip Select Pin
  pinMode(LTC2378_CS, OUTPUT);
  digitalWrite(LTC2378_CS, HIGH);

  quikeval_SPI_init();          // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();       // Connect SPI to main data port
  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz
  Serial.begin(115200);         // Initialize the serial port to the PC
  print_title();                // Prints the title block when program first starts
}

//! Repeats Linduino loop
void loop()
{
  print_prompt();
  demo_board_test();
}

// Function Definitions

//! Displays The Prompt
void print_prompt()
{
  Serial.println(F("\nPress any key when ready to start\n"));
  Serial.flush();
  read_int();
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC2025 Demonstration Program                                  *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates the application noted at the end    *"));
  Serial.println(F("* of the LTC2668 16/12-bit DAC datasheet. The purpose of this   *"));
  Serial.println(F("* program is to demonstrate the use of the MUX pin on the       *"));
  Serial.println(F("* LTC2668.                                                      *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("*****************************************************************"));
}


//! Set DAC's From -10V to 10V, Reads DAC Channels Through MUX, and Displays The Results
void demo_board_test()
{
  int32_t adc_code[16];
  float voltage[16];
  float ideal_voltage;
  float full_scale = 10.24;

  LTC2668_write(LTC2668_CS,LTC2668_CMD_SPAN_ALL, 0, LTC2668_SPAN_PLUS_MINUS_10V);  // Set DAC's To +-10V

  // Set Channels From -10v to 10V Incrementing By Channel.
  for (uint8_t i = 0; i <= 15; ++i)
  {
    LTC2668_write(LTC2668_CS,LTC2668_CMD_WRITE_N_UPDATE_N, i, i * 4369);
  }

  LTC2668_write(LTC2668_CS,LTC2668_CMD_MUX, 0, LTC2668_MUX_ENABLE | 0); // Set To Channel 0
  LTC2378_read(LTC2378_CS, &adc_code[0]);   // Discard reading

  // Cycle Through the Channels of the MUX and Reads the MUX Voltage
  for (uint8_t i = 1; i <= 15; ++i)
  {
    LTC2668_write(LTC2668_CS,LTC2668_CMD_MUX, 0, LTC2668_MUX_ENABLE | i);
    LTC2378_read(LTC2378_CS, &adc_code[i-1]);
  }
  LTC2378_read(LTC2378_CS, &adc_code[15]);

  // Display The Results
  for (uint8_t i = 0; i<=15; ++i)
  {
    voltage[i] = LTC2378_code_to_voltage(adc_code[i], full_scale);
    ideal_voltage = LTC2668_code_to_voltage(i * 4369, -10.0, 10.0);
    Serial.print(F("Channel "));
    Serial.print(i);
    if (i < 10)
      Serial.print(" ");
    Serial.print(F(" : Ideal Voltage - "));
    if (ideal_voltage >= 0)
      Serial.print(" ");
    Serial.print(ideal_voltage, 6);
    if ((ideal_voltage < 10.0) && (ideal_voltage > -10.0))
      Serial.print(" ");
    Serial.print(F(" V Measured Voltage - "));
    if (voltage[i] >= 0)
      Serial.print(" ");
    Serial.print(voltage[i],6);
    if ((voltage[i] < 10.0) && (voltage[i] > -10.0))
      Serial.print(" ");
    Serial.print(F(" V difference - "));
    if ((ideal_voltage-voltage[i]) >= 0)
      Serial.print(" ");
    Serial.print((ideal_voltage-voltage[i]),6);
    Serial.println(" V");
  }

  Serial.println();
  Serial.println("Test Complete\n");
}