/*!
LTC2449: 24-Bit High Speed 16-Channel Delta Sigma ADCs with Selectable Speed/Resolution

@verbatim

The LTC2449 is a 16-channel (8-differential) high speed 24-bit No Latency Delta Sigma ADCs.
They use a proprietary delta-sigma architecture enabling variable speed/resolution. Through
a simple 4-wire serial interface, ten speed/resolution combinations 6.9Hz/280nVRMS to
3.5kHz/25uVRMS (4kHz with external oscillator) can be selected with no latency between
conversion results or shift in DC accuracy (offset, full-scale, linearity, drift).
Additionally, a 2X speed mode can be selected enabling output rates up to 7kHz (8kHz if an
external oscillator is used) with one cycle latency.

@endverbatim

http://www.linear.com/product/LTC2449

http://www.linear.com/product/LTC2449#demoboards


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

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "QuikEval_EEPROM.h"
#include "UserInterface.h"
#include <SPI.h>
#include "LT_SPI.h"
#include <Wire.h>
#include "LT_I2C.h"
#include "LTC24XX_general.h"

#define CS QUIKEVAL_CS                              //!< The chip select 

// Constants

//! Lookup table to build the command for single-ended mode
const uint8_t SINGLE_ENDED_CONFIG_ARRAY[16] = {LTC24XX_MULTI_CH_CH0, LTC24XX_MULTI_CH_CH1, LTC24XX_MULTI_CH_CH2, LTC24XX_MULTI_CH_CH3,
    LTC24XX_MULTI_CH_CH4, LTC24XX_MULTI_CH_CH5, LTC24XX_MULTI_CH_CH6, LTC24XX_MULTI_CH_CH7,
    LTC24XX_MULTI_CH_CH8, LTC24XX_MULTI_CH_CH9, LTC24XX_MULTI_CH_CH10, LTC24XX_MULTI_CH_CH11,
    LTC24XX_MULTI_CH_CH12, LTC24XX_MULTI_CH_CH13, LTC24XX_MULTI_CH_CH14, LTC24XX_MULTI_CH_CH15
                                              };  //!< Builds the command for single-ended mode

//! Lookup table to build the command for differential mode
const uint8_t DIFF_CONFIG_ARRAY[16] = {LTC24XX_MULTI_CH_P0_N1, LTC24XX_MULTI_CH_P2_N3, LTC24XX_MULTI_CH_P4_N5, LTC24XX_MULTI_CH_P6_N7,
                                       LTC24XX_MULTI_CH_P8_N9, LTC24XX_MULTI_CH_P10_N11, LTC24XX_MULTI_CH_P12_N13, LTC24XX_MULTI_CH_P14_N15,
                                       LTC24XX_MULTI_CH_P1_N0, LTC24XX_MULTI_CH_P3_N2, LTC24XX_MULTI_CH_P5_N4, LTC24XX_MULTI_CH_P7_N6,
                                       LTC24XX_MULTI_CH_P9_N8, LTC24XX_MULTI_CH_P11_N10, LTC24XX_MULTI_CH_P13_N12, LTC24XX_MULTI_CH_P15_N14
                                      };          //!< Build the command for differential mode


//! Lookup table to build the OSR command
const uint8_t OSR_CONFIG_ARRAY[10] = {LTC24XX_MULTI_CH_OSR_32768, LTC24XX_MULTI_CH_OSR_16384, LTC24XX_MULTI_CH_OSR_8192,
                                      LTC24XX_MULTI_CH_OSR_4096, LTC24XX_MULTI_CH_OSR_2048, LTC24XX_MULTI_CH_OSR_1024, LTC24XX_MULTI_CH_OSR_512, LTC24XX_MULTI_CH_OSR_256,
                                      LTC24XX_MULTI_CH_OSR_128, LTC24XX_MULTI_CH_OSR_64
                                     };            //!< Build the command for different OSR modes

// Prototypes
void read_LTC2449(float vref, uint16_t eoc_timeout, uint16_t channel_delay);
void print_all(float *results);
void print_prompt();
void print_all(float *results);

//! Initialize Linduino
void setup()
{
  char demo_name[] = "DC742";                       // Demo Board Name stored in QuikEval EEPROM
  int8_t demo_board_connected;                      // Set to 1 if the board is connected

  Serial.begin(115200);                             // Initialize the serial port to the PC
  Serial.println(F("Welcome to the LTC2499 Test Program\n"));
  delay(200);

  quikeval_I2C_init();                              // Enable the I2C port
  quikeval_SPI_init();                              // Configure the spi port for 4volts SCK
  quikeval_SPI_connect();                           // Connect SPI to main data port

  demo_board_connected = discover_demo_board(demo_name);

  // If demo board is not connected loop forever
  if (!demo_board_connected)
    Serial.println(F("Standard demo board not detected, verify hardware connections to your circuit."));

} // End of setup()


//! Repeats Linduino loop
void loop()
{
  static float LTC2499_vref = 5.0;                  // Reference voltage
  static uint16_t EOC_timout = 250;                 // End of conversion timeout
  static uint16_t loop_delay = 500;                 // the delay between loops
  static uint16_t channel_read_delay = 20;          // The delay between channel readings
  static char user_command;                         // The user input command

  print_prompt();                                   // Prints the prompt

  user_command = read_char();                       // Reads the users input
  Serial.println(user_command);                     // Displays the user input

  switch (user_command)
  {
    case 'S':
    case 's':
      // Single loop
      read_LTC2449(LTC2499_vref, EOC_timout, channel_read_delay);   // Read all the channels
      break;
    case 'C':
    case 'c':
      // Continous loop
      do
      {
        read_LTC2449(LTC2499_vref, EOC_timout, channel_read_delay); // Read all the channels
        delay(loop_delay);                                          // Delay before repeating the loop
      }
      while (Serial.available() == false);                          // Check to see is anything was entered
      read_int();                                                   // Clears the Serial.available
      break;
    default:
      Serial.println(F("Incorrect Option"));
      break;
  }
}

//! Print the Prompt
void print_prompt()
{
  Serial.println();
  Serial.println(F("S-Single Loop"));
  Serial.println(F("C-Continuous Loop"));
  Serial.println(F("    Enter any character to exit Continuous Loop"));

  Serial.print(F("Enter a command: "));
}

//! Reads the LTC2449 and displays the results
void read_LTC2449(float vref, uint16_t eoc_timeout, uint16_t channel_delay)
{
  float results[16];
  int32_t adc_code = 0;
  uint8_t i;

  LTC24XX_EOC_timeout(CS, eoc_timeout);             // Wait for end of conversion
  LTC24XX_SPI_16bit_command_32bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[0], OSR_CONFIG_ARRAY[7], &adc_code);           // Prime with CH0

  for (i=0; i<=15; i++)
  {
    delay(channel_delay);
    LTC24XX_EOC_timeout(CS, eoc_timeout);           // Wait for end of conversion
    LTC24XX_SPI_16bit_command_32bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[(i+1)%16], OSR_CONFIG_ARRAY[7], &adc_code);  // Read the ADC
    results[i%16] = LTC24XX_diff_code_to_voltage(adc_code, vref);   // Convert ADC code to voltage
  }

  print_all(results);                               // Display results
}

//! Displays the the data in the array
void print_all(float *results)
{
  uint8_t i;
  for (i=0; i<=15; i++)
  {
    Serial.print(results[i], 8);
    if (!((i+1)%16)) Serial.println();
    else Serial.print(F(" , "));
  }
}