/*!
Linear Technology DC571A Demonstration Board.
LTC2412: 2-Channel Differential Input 24-Bit No Latency Delta Sigma ADC

Linear Technology DC346A Demonstration Board.
LTC2413: 24-Bit No Latency Delta Sigma ADC with Simultaneous 50Hz/60Hz Rejection ADC

@verbatim

NOTES
  Setup:
    Set the terminal baud rate to 115200 and select the newline terminator.
    Equipment required is a precision voltage source and a precision
    voltmeter. No external power supply is required. Ensure all jumpers on
    the demo board are installed in their default positions from the
    factory.

  How to test Single-Ended mode:
    The voltage source should be connected to the ADC such that the negative
    lead is connected to ground. The positive lead may be connected to any
    channel input. Ensure voltage is within analog input voltage range -0.3V to
    +2.5V.

  How to test Differential Mode:
    The voltage source should be connected with positive and negative leads to
    paired channels. The voltage source negative output must also be connected to
    the COM pin in order to provide a ground-referenced voltage. Ensure voltage is
    within analog input voltage range -0.3V to +2.5V. Swapping input voltages
    results in a reversed polarity reading.

  How to calibrate:
    Apply 100mV to CH0 with respect to COM pin. Next, Measure this voltage with a
    precise voltmeter and enter this value. (This takes the reading.) Apply a higher
    voltage than the first voltage or approximately 2.40 volts to CH0. Anything
    above 2.40 is not reccomened. It is very likely to reach the ADC's full-scale
    and poduce invalide lsb and offset. Measure this voltage with a precise
    voltmeter and enter this value. Calibration is now stored in EEPROM. Upon
    startup the calibration values will be restored.

USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim

http://www.linear.com/product/LTC2412
http://www.linear.com/product/LTC2413

http://www.linear.com/product/LTC2412#demoboards
http://www.linear.com/product/LTC2413#demoboards


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
    @ingroup LTC2412
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LT_I2C.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC2412.h"
#include <SPI.h>
#include <Wire.h>

// Function Declaration
void print_title();                                 // Print the title block
void print_prompt();                                // Prompt the user for an input command

int8_t menu_1_read_single_ended();
void menu_2_change_ref_voltage();

// Global variables
static uint8_t demo_board_connected;                //!< Set to 1 if the board is connected
static float LTC2412_lsb = 2.98023241E-7;           //!< Ideal LSB voltage for a perfect part
static int32_t LTC2412_offset_code = 0;             //!< Ideal offset for a perfect part
static float reference_voltage = 5.0;

//Constants

const uint16_t MISO_TIMEOUT = 1000;                 //!< The MISO timeout (ms)

//! Initialize Linduino
void setup()
{
  char demo_name[6]="DC746";        // Demo Board Name stored in QuikEval EEPROM
  quikeval_SPI_init();              // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();           // Connect SPI to main data port
  quikeval_I2C_init();              // Configure the EEPROM I2C port for 100kHz
  Serial.begin(115200);             // Initialize the serial port to the PC
  print_title();
  demo_board_connected = discover_demo_board(demo_name);
  if (demo_board_connected)
  {
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
      Serial.println(user_command);
      switch (user_command)
      {
        case 1:
          menu_1_read_single_ended();
          break;
        case 2:
          menu_2_change_ref_voltage();
          break;
        default:
          Serial.println("Incorrect Option");
          break;
      }
      if (acknowledge)
        Serial.println(F("***** SPI ERROR ******"));
      Serial.print(F("\n*************************\n"));
      print_prompt();
    }
  }
}

//! Repeats Linduino loop
int8_t menu_1_read_single_ended()
{
  int16_t user_command; // The user input command
  uint32_t adc_code;    // The LTC2418 code
  float adc_voltage;    // The LTC2418 voltage
  uint8_t channel;

  if (LTC2412_EOC_timeout(LTC2412_CS, MISO_TIMEOUT))     // Check for EOC
    return(1);
  LTC2412_read(LTC2412_CS, &adc_code);
  Serial.print(F("Received Code: 0x"));
  Serial.println(adc_code, HEX);
  adc_voltage = LTC2412_code_to_voltage(adc_code, LTC2412_lsb , LTC2412_offset_code);
  channel = adc_code >> 30;
  channel = channel & 1;
  Serial.print(F("CH"));
  Serial.print(channel);
  Serial.print(F(": "));
  Serial.print(adc_voltage, 4);
  Serial.print(F("V\n"));

  if (LTC2412_EOC_timeout(LTC2412_CS, MISO_TIMEOUT))     // Check for EOC
    return(1);
  LTC2412_read(LTC2412_CS, &adc_code);
  Serial.print(F("Received Code: 0x"));
  Serial.println(adc_code, HEX);
  adc_voltage = LTC2412_code_to_voltage(adc_code, LTC2412_lsb , LTC2412_offset_code);
  channel = adc_code >> 30;
  channel = channel & 1;
  Serial.print(F("CH"));
  Serial.print(channel);
  Serial.print(F(": "));
  Serial.print(adc_voltage, 4);
  Serial.print(F("V\n"));
  return(0);
}

void menu_2_change_ref_voltage()
{
  float refp, refn;
  Serial.print(F("Enter REF+ : "));
  refp = read_float();
  Serial.print(refp);
  Serial.println(F(" V"));
  Serial.print(F("Enter REF- : "));
  refn = read_float();
  Serial.print(refn);
  Serial.println(F(" V"));
  reference_voltage = refp - refn;
  LTC2412_lsb = reference_voltage / 16777216;
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* DC746A Demonstration Program                                  *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* This program demonstrates how to send data and receive data   *\n"));
  Serial.print(F("* from the 24-bit ADC.                                          *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("\nReference voltage: "));
  Serial.print(reference_voltage);
  Serial.println(F("V"));
  Serial.print(F("Input Voltage Range: "));
  Serial.print((-1) * reference_voltage/2);
  Serial.print(" - ");
  Serial.print(reference_voltage/2);
  Serial.println(F(" V"));
  Serial.println(F("Please Verify!"));

  Serial.print(F("\n1-Read Both Channels\n"));
  Serial.print(F("2-Change Reference Voltage\n"));
  Serial.println();
  Serial.print(F("Enter a command:"));
}
