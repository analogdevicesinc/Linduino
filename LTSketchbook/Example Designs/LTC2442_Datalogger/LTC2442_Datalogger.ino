/*!
LTC2442: 24-Bit High Speed 4-Channel Delta Sigma ADCs with Integrated Amplifier

@verbatim

The LTC2442 is a 4-channel (2-differential) high speed 24-bit No Latency Delta Sigma ADCs.
They use a proprietary delta-sigma architecture enabling variable speed/resolution. Through
a simple 4-wire serial interface, ten speed/resolution combinations 6.9Hz/280nVRMS to
3.5kHz/25uVRMS (4kHz with external oscillator) can be selected with no latency between
conversion results or shift in DC accuracy (offset, full-scale, linearity, drift).
Additionally, a 2X speed mode can be selected enabling output rates up to 7kHz (8kHz if an
external oscillator is used) with one cycle latency.

@endverbatim

http://www.linear.com/product/LTC2442

http://www.linear.com/product/LTC2442#demoboards


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

///////////////////////////////////////////////////////////////
// Begin program behavior setup ///////////////////////////////
///////////////////////////////////////////////////////////////

// Lookup tables of channels to scan.
// UN-comment one section below: First section is odd channels as negative input
//                               Second section is even channels as negative input.
//                               Third section is single-ended channels with respec to COM.

const uint8_t CHANNEL_ARRAY[2] = {LTC24XX_MULTI_CH_P0_N1, LTC24XX_MULTI_CH_P2_N3};          //!< Build the command for differential mode

//const uint8_t CHANNEL_ARRAY[2] = {LTC24XX_MULTI_CH_P1_N0, LTC24XX_MULTI_CH_P3_N2};          //!< Build the command for differential mode

//const uint8_t CHANNEL_ARRAY[4] = {LTC24XX_MULTI_CH_CH0, LTC24XX_MULTI_CH_CH1, LTC24XX_MULTI_CH_CH2, LTC24XX_MULTI_CH_CH3};  //!< Builds the command for single-ended mode


//UN-comment one line below to set the OSR mode:
//const uint8_t OSR = LTC24XX_MULTI_CH_OSR_32768;
const uint8_t OSR = LTC24XX_MULTI_CH_OSR_16384;
//const uint8_t OSR = LTC24XX_MULTI_CH_OSR_8192;
//const uint8_t OSR = LTC24XX_MULTI_CH_OSR_4096;
//const uint8_t OSR = LTC24XX_MULTI_CH_OSR_2048;
//const uint8_t OSR = LTC24XX_MULTI_CH_OSR_1024;
//const uint8_t OSR = LTC24XX_MULTI_CH_OSR_512;
//const uint8_t OSR = LTC24XX_MULTI_CH_OSR_256;
//const uint8_t OSR = LTC24XX_MULTI_CH_OSR_128;
//const uint8_t OSR = LTC24XX_MULTI_CH_OSR_64;

float LTC2442_vref = 5.0;                // Reference voltage
uint16_t EOC_timout = 250;               // End of conversion timeout (must be larger than tconv(max))
uint16_t loop_delay = 0;                 // the delay between loops
uint16_t channel_read_delay = 0;         // The delay between channel readings

// Display format. UN-comment one line below.
#define FLOATING
//#define RAW

// UN-comment to prime the ADC before taking data for a given scan, purging stale data from previous scan.
// May be useful for single scans with a long delay between scans.
//#define PRIME_BEFORE_SCAN

///////////////////////////////////////////////////////////////
// End program behavior setup   ///////////////////////////////
///////////////////////////////////////////////////////////////


uint8_t array_size = sizeof(CHANNEL_ARRAY); // Get size of array (in bytes, each element is a byte)

// Prototypes
void read_LTC2442(float vref, uint16_t eoc_timeout, uint16_t channel_delay);
void print_raw(int32_t *rawresults);
void print_all(float *results);
void print_prompt();


//! Initialize Linduino
void setup()
{
  char demo_name[] = "DC979";                       // Demo Board Name stored in QuikEval EEPROM
  int8_t demo_board_connected;                      // Set to 1 if the board is connected

  Serial.begin(115200);                             // Initialize the serial port to the PC
  Serial.println(F("Welcome to the LTC2442 Test Program\n"));
  delay(200);

  quikeval_I2C_init();                              // Enable the I2C port
  quikeval_SPI_init();                              // Configure the spi port for 4volts SCK
  quikeval_SPI_connect();                           // Connect SPI to main data port

  demo_board_connected = discover_demo_board(demo_name);

  // If demo board is not connected loop forever
  if (!demo_board_connected)
    Serial.println(F("Standard demo board not detected, verify hardware connections to your circuit."));

  print_prompt();                                   // Prints the prompt
} // End of setup()


//! Repeats Linduino loop
void loop()
{
  static char user_command;                         // The user input command
  user_command = read_char();                       // Reads the users input
//  Serial.println(user_command);                     // Displays the user input

  switch (user_command)
  {
    case 'S':
    case 's':
      // Single loop
      read_LTC2442(LTC2442_vref, EOC_timout, channel_read_delay);   // Read all the channels
      break;
    case 'C':
    case 'c':
      // Continous loop
      do
      {
        read_LTC2442(LTC2442_vref, EOC_timout, channel_read_delay); // Read all the channels
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

//! Reads the LTC2442 and displays the results
void read_LTC2442(float vref, uint16_t eoc_timeout, uint16_t channel_delay)
{
  float floatresults[16];
  int32_t rawresults[16];
  int32_t adc_code = 0;
  uint8_t i;

#ifdef PRIME_BEFORE_SCAN // If defined, prime first channel so that data is "less stale"
  LTC24XX_EOC_timeout(CS, eoc_timeout);             // Wait for end of conversion
  LTC24XX_SPI_16bit_command_32bit_data(CS, CHANNEL_ARRAY[0], OSR, &adc_code);           // Prime with first channel
#endif

  for (i=0; i<array_size; i++)
  {
    delay(channel_delay);
    LTC24XX_EOC_timeout(CS, eoc_timeout);           // Wait for end of conversion
    LTC24XX_SPI_16bit_command_32bit_data(CS, CHANNEL_ARRAY[(i+1)%array_size], OSR, &adc_code);  // Read the ADC
    rawresults[i%array_size] = adc_code;
    floatresults[i%array_size] = LTC24XX_diff_code_to_voltage(adc_code, vref);   // Convert ADC code to voltage
  }
#ifdef FLOATING
  print_all(floatresults);                               // Display results
#else
  print_raw(rawresults);
#endif
}

//! Displays the the data in the array
void print_all(float *floatresults)
{
  uint8_t i;
  for (i=0; i<array_size; i++)
  {
    Serial.print(floatresults[i], 8);
    if (!((i+1)%array_size)) Serial.println();
    else Serial.print(F(" , "));
  }
}

void print_raw(int32_t *rawresults)
{
  uint8_t i;
  for (i=0; i<array_size; i++)
  {
    Serial.print(rawresults[i]);
    if (!((i+1)%array_size)) Serial.println();
    else Serial.print(F(" , "));
  }
}