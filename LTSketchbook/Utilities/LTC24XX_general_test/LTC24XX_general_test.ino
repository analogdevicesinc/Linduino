/*!

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
    @ingroup LTC24XX_general
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "UserInterface.h"
#include <SPI.h>
#include "LT_SPI.h"


#include "LTC24XX_general.h"
#include "LTC2668.h"

#include "USE_WIRE.h"
#ifdef USEWIRE
#include "LT_I2C_Wire.h"
#include "QuikEval_EEPROM_Wire.h"
#else
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#endif

#define CS QUIKEVAL_CS      //!< The chip select 

#define LTC2668_CS_ADDR 0   //!< Decoder address for the LTC2668 chip select
#define LTC2439_CS_ADDR 1   //!< Decoder address for the LTC2439 chip select
#define LTC2418_CS_ADDR 2   //!< Decoder address for the LTC2418 chip select
#define LTC2449_CS_ADDR 3   //!< Decoder address for the LTC2449 chip select
#define LTC2494_CS_ADDR 4   //!< Decoder address for the LTC2494 chip select
#define LTC2498_CS_ADDR 5   //!< Decoder address for the LTC2498 chip select
#define LTC2496_CS_ADDR 6   //!< Decoder address for the LTC2496 chip select
#define LTC2412_CS_ADDR 7   //!< Decoder address for the LTC2412 chip select
#define LTC2436_CS_ADDR 8   //!< Decoder address for the LTC2436 chip select
#define LTC2400_CS_ADDR 9   //!< Decoder address for the LTC2400 chip select
#define LTC2420_CS_ADDR 10  //!< Decoder address for the LTC2420 chip select


//! @name I2C Addresses
// ADDRESS     CA2   CA1    CA0
#define LTC2495_ADDR 0b0010100 // LOW   LOW    LOW
#define LTC2497_ADDR 0b0100110 // LOW   HIGH   LOW
#define LTC2499_ADDR 0b1010110 // HIGH  LOW    LOW
//! @}

#define TEST_LTC2439
#define TEST_LTC2418
#define TEST_LTC2449
#define TEST_LTC2494
#define TEST_LTC2496
#define TEST_LTC2498

#define TEST_LTC2495
#define TEST_LTC2497
#define TEST_LTC2499

#define TEST_LTC2436
#define TEST_LTC2412

#define TEST_LTC2400
#define TEST_LTC2420


// Constants

//! Lookup table to build the command for single-ended mode
const uint8_t SINGLE_ENDED_CONFIG_ARRAY[16] = {LTC24XX_MULTI_CH_CH0, LTC24XX_MULTI_CH_CH1, LTC24XX_MULTI_CH_CH2, LTC24XX_MULTI_CH_CH3,
    LTC24XX_MULTI_CH_CH4, LTC24XX_MULTI_CH_CH5, LTC24XX_MULTI_CH_CH6, LTC24XX_MULTI_CH_CH7,
    LTC24XX_MULTI_CH_CH8, LTC24XX_MULTI_CH_CH9, LTC24XX_MULTI_CH_CH10, LTC24XX_MULTI_CH_CH11,
    LTC24XX_MULTI_CH_CH12, LTC24XX_MULTI_CH_CH13, LTC24XX_MULTI_CH_CH14, LTC24XX_MULTI_CH_CH15
                                              };    //!< Builds the command for single-ended mode

//! Lookup table to build the command for differential mode
const uint8_t DIFF_CONFIG_ARRAY[16] = {LTC24XX_MULTI_CH_P0_N1, LTC24XX_MULTI_CH_P2_N3, LTC24XX_MULTI_CH_P4_N5, LTC24XX_MULTI_CH_P6_N7,
                                       LTC24XX_MULTI_CH_P8_N9, LTC24XX_MULTI_CH_P10_N11, LTC24XX_MULTI_CH_P12_N13, LTC24XX_MULTI_CH_P14_N15,
                                       LTC24XX_MULTI_CH_P1_N0, LTC24XX_MULTI_CH_P3_N2, LTC24XX_MULTI_CH_P5_N4, LTC24XX_MULTI_CH_P7_N6,
                                       LTC24XX_MULTI_CH_P9_N8, LTC24XX_MULTI_CH_P11_N10, LTC24XX_MULTI_CH_P13_N12, LTC24XX_MULTI_CH_P15_N14
                                      };      //!< Build the command for differential mode


//! Lookup table to build the OSR command
const uint8_t OSR_CONFIG_ARRAY[16] = {LTC24XX_MULTI_CH_OSR_32768, LTC24XX_MULTI_CH_OSR_16384, LTC24XX_MULTI_CH_OSR_8192,
                                      LTC24XX_MULTI_CH_OSR_4096, LTC24XX_MULTI_CH_OSR_2048, LTC24XX_MULTI_CH_OSR_1024, LTC24XX_MULTI_CH_OSR_512, LTC24XX_MULTI_CH_OSR_256,
                                      LTC24XX_MULTI_CH_OSR_128, LTC24XX_MULTI_CH_OSR_64, LTC24XX_MULTI_CH_OSR_32768, LTC24XX_MULTI_CH_OSR_32768,
                                      LTC24XX_MULTI_CH_OSR_32768, LTC24XX_MULTI_CH_OSR_32768, LTC24XX_MULTI_CH_OSR_32768, LTC24XX_MULTI_CH_OSR_32768
                                     };      //!< Build the command for different OSR modes

// Prototypes
void set_CS_decoder(uint8_t CS_addr);
void print_all(float *se_results, float *diff_results);
void test_LTC2400(float vref, uint16_t eoc_timeout);
void test_LTC2412(float vref, uint16_t eoc_timeout);
void test_LTC2418(float vref, uint16_t eoc_timeout);
void test_LTC2420(float vref, uint16_t eoc_timeout);
void test_LTC2436(float vref, uint16_t eoc_timeout);
void test_LTC2439(float vref, uint16_t eoc_timeout);
void test_LTC2449(float vref, uint16_t eoc_timeout);
void test_LTC2494(float vref, uint16_t eoc_timeout);
void test_LTC2495(float vref, uint16_t eoc_timeout);
void test_LTC2496(float vref, uint16_t eoc_timeout);
void test_LTC2497(float vref, uint16_t eoc_timeout);
void test_LTC2498(float vref, uint16_t eoc_timeout);
void test_LTC2499(float vref, uint16_t eoc_timeout);
void test_all(float vref, uint16_t eoc_timeout);


// Globals
char demo_name[] = "DCXXXX";     //!< Demo Board Name stored in QuikEval EEPROM

//! Initialize Linduino
void setup()
{
  Serial.begin(115200);     // Initialize the serial port to the PC
  Serial.println(F("LTC24XX Test Suite\n"));
  delay(200);

  quikeval_I2C_init();      // Enable the I2C port
  quikeval_I2C_connect();   // Connect I2C to main data port
  quikeval_SPI_init();      // Configure the spi port for 4volts SCK
  quikeval_SPI_connect();   // Connect SPI to main data port

  pinMode(2, OUTPUT);       // Set up the CS# decoder address lines
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  discover_demo_board(demo_name);


} // End of setup()

//! Repeats Linduino loop
void loop()
{
  static float LTC24XX_vref = 5.0;
  static uint16_t EOC_timout = 250;

  // Uses the last two digits of the LTC number for uniqueness
  uint16_t demo_board_product_name_key = *(uint16_t *)(demo_board.product_name + 5);

  switch (demo_board_product_name_key)
  {
    case 0x3030:
      // LTC2400
      test_LTC2400(LTC24XX_vref, EOC_timout);
      break;
    case 0x3231:
      // LTC2412
      test_LTC2412(LTC24XX_vref, EOC_timout);
      break;
    case 0x3831:
      // LTC2418
      test_LTC2418(LTC24XX_vref, EOC_timout);
      break;
    case 0x3933:
      // LTC2439
      test_LTC2439(LTC24XX_vref, EOC_timout);
      break;
    case 0x3934:
      // LTC2449
      test_LTC2449(LTC24XX_vref, EOC_timout);
      break;
    case 0x3439:
      // LTC2494
      test_LTC2494(LTC24XX_vref, EOC_timout);
      break;
    case 0x3539:
      // LTC2495
      test_LTC2495(LTC24XX_vref, EOC_timout);
      break;
    case 0x3639:
      // LTC2496
      test_LTC2496(LTC24XX_vref, EOC_timout);
      break;
    case 0x3739:
      // LTC2497
      test_LTC2497(LTC24XX_vref, EOC_timout);
      break;
    case 0x3839:
      // LTC2498
      test_LTC2498(LTC24XX_vref, EOC_timout);
      break;
    case 0x3939:
      // LTC2499
      test_LTC2499(LTC24XX_vref, EOC_timout);
      break;
    default:
      // Test all
      Serial.println(F("\nTEST ALL\n"));
      test_all(LTC24XX_vref, EOC_timout);
      break;
  }
  delay(500);
}

//! Displays the the data in the arrays
void print_all(float *se_results, float *diff_results)
{
  uint8_t i;
  Serial.println(F("\nSingle Ended Results..."));
  for (i=0; i<=15; i++)
  {
    Serial.print(F("CH "));
    Serial.print(i);
    Serial.print(F(": "));
    Serial.print(se_results[i], 8);
    if (!((i+1)%4)) Serial.println();
    else Serial.print(F("  "));
  }

  Serial.println(F("\nDifferential Results..."));
  for (i=0; i<=7; i++)
  {
    Serial.print(F("CH "));
    Serial.print(i*2);
    Serial.print(F("-"));
    Serial.print((i*2) + 1);
    Serial.print(F(": "));
    Serial.print(diff_results[i], 8);
    if (!((i+1)%4)) Serial.println();
    else Serial.print(F("  "));
  }
}

// Brute force.
void set_CS_decoder(uint8_t CS_addr)
{
  if (CS_addr & 0b00000001)
  {
    digitalWrite(2, HIGH);
  }
  else
  {
    digitalWrite(2, LOW);
  }
  if (CS_addr & 0b00000010)
  {
    digitalWrite(3, HIGH);
  }
  else
  {
    digitalWrite(3, LOW);
  }
  if (CS_addr & 0b00000100)
  {
    digitalWrite(4, HIGH);
  }
  else
  {
    digitalWrite(4, LOW);
  }
  if (CS_addr & 0b00001000)
  {
    digitalWrite(5, HIGH);
  }
  else
  {
    digitalWrite(5, LOW);
  }
}

//! Reads the LTC2400 and displays the results
void test_LTC2400(float vref, uint16_t eoc_timeout)
{
  float voltage = 0.0;
  int32_t adc_code = 0;
  int i;

  quikeval_SPI_connect();                               // Connect SPI to main data port
  set_CS_decoder(LTC2400_CS_ADDR);                      // Set the decoder for the LTC2400 chip select

  Serial.println(F("\nTesting LTC2400 with a 32 bit read using LTC24XX_SPI_32bit_data..."));

  LTC24XX_EOC_timeout(CS, eoc_timeout);                 // First flush power-on conversion that probably happened before
  LTC24XX_SPI_32bit_data(CS, &adc_code);                // the DAC output was set

  LTC24XX_EOC_timeout(CS, eoc_timeout);                 // Wait for end of conversion
  LTC24XX_SPI_32bit_data(CS, &adc_code);                // Read the ADC
  voltage = LTC24XX_SE_code_to_voltage(adc_code, vref); // Convert ADC code to voltage

  // Display results
  Serial.print(F("  Formatted LTC2400 ADC code: 0x"));
  Serial.println(adc_code, HEX);
  Serial.print(F("  Voltage:"));
  Serial.print(voltage, 8);
  Serial.println(F(" V"));
}

//! Reads the LTC2412 and displays the results
void test_LTC2412(float vref, uint16_t eoc_timeout)
{
  float voltage = 0.0;
  int32_t adc_code = 0;
  uint8_t channel;
  int i;

  quikeval_SPI_connect();                   // Connect SPI to main data port
  set_CS_decoder(LTC2412_CS_ADDR);          // Set the decoder for the LTC2412 chip select

  Serial.println(F("\nTesting LTC2412 with a 32 bit read..."));

  LTC24XX_EOC_timeout(CS, eoc_timeout);                             // First flush power-on conversion that probably happened before
  LTC24XX_SPI_2ch_ping_pong_32bit_data(CS, &channel, &adc_code);    // the DAC output was set

  for (i=0; i <= 1; ++i)
  {
    LTC24XX_EOC_timeout(CS, eoc_timeout);                           // Wait for end of conversion
    LTC24XX_SPI_2ch_ping_pong_32bit_data(CS, &channel, &adc_code);  // Read the ADC and channel
    voltage = LTC24XX_diff_code_to_voltage(adc_code, vref);         // Convert ADC code to voltage

    // Display results
    Serial.print(F("  Formatted LTC2412 ADC code CH"));
    Serial.print(channel);
    Serial.print(F(": 0x"));
    Serial.println(adc_code, HEX);
    Serial.print(F(" Voltage: "));
    Serial.print(voltage, 8);
    Serial.println(F(" V"));
  }
}

//! Reads the LTC2418 and displays the results
void test_LTC2418(float vref, uint16_t eoc_timeout)
{
  float se_results[16], diff_results[16];
  int32_t adc_code = 0;
  uint8_t i;

  quikeval_SPI_connect();                       // Connect SPI to main data port
  set_CS_decoder(LTC2418_CS_ADDR);              // Set the decoder for the LTC2418 chip select

  Serial.println(F("\nTesting LTC2418, 16 bit ADC with a 19 bit output word using LTC24XX_SPI_8bit_command_32bit_data() function"));
  Serial.println(F("Reading Single-Ended channels..."));

  LTC24XX_EOC_timeout(CS, eoc_timeout);         // Wait for end of conversion
  LTC24XX_SPI_8bit_command_32bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[0], &adc_code);             // Prime with CH0

  for (i=0; i <= 15; i++)
  {
    LTC24XX_EOC_timeout(CS, eoc_timeout);       // Wait for end of conversion
    LTC24XX_SPI_8bit_command_32bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[(i+1)%16], &adc_code);    // Read the ADC
    se_results[i%16] = LTC24XX_diff_code_to_voltage(adc_code, vref);                            // Convert ADC code to voltage
  }

  Serial.println(F("Reading out all Differential channels..."));

  LTC24XX_EOC_timeout(CS, eoc_timeout);         // Wait for end of conversion
  LTC24XX_SPI_8bit_command_32bit_data(CS, DIFF_CONFIG_ARRAY[0], &adc_code);                     // Prime with CH_P0_N1

  for (i=0; i <= 7; i++)
  {
    LTC24XX_EOC_timeout(CS, eoc_timeout);       // Wait for end of conversion
    LTC24XX_SPI_8bit_command_32bit_data(CS, DIFF_CONFIG_ARRAY[(i+1)%16], &adc_code);            // Read the ADC
    diff_results[i%8] = LTC24XX_diff_code_to_voltage(adc_code, vref);                           // Convert ADC code to voltage
  }

  print_all(se_results, diff_results);          // Display results
}

//! Reads the LTC2420 and displays the results
void test_LTC2420(float vref, uint16_t eoc_timeout)
{
  float voltage = 0.0;
  int32_t adc_code = 0;
  uint8_t i;

  quikeval_SPI_connect();                               // Connect SPI to main data port
  set_CS_decoder(LTC2420_CS_ADDR);                      // Set the decoder for the LTC2420 chip select

  Serial.println(F("\nTesting LTC2420 with a 24 bit read using LTC24XX_SPI_24bit_data..."));

  LTC24XX_EOC_timeout(CS, eoc_timeout);                 // First flush power-on conversion that probably happened before
  LTC24XX_SPI_24bit_data(CS, &adc_code);                // the DAC output was set

  LTC24XX_EOC_timeout(CS, eoc_timeout);                 // Wait for end of conversion
  LTC24XX_SPI_24bit_data(CS, &adc_code);                // Read the ADC
  voltage = LTC24XX_SE_code_to_voltage(adc_code, vref); // Convert ADC code to voltage

  // Display results
  Serial.print(F("  Formatted LTC2412 ADC code: "));
  Serial.println(adc_code, HEX);
  Serial.print(F("  Voltage: "));
  Serial.print(voltage, 8);
  Serial.println(F(" V"));
}

//! Reads the LTC2436 and displays the results
void test_LTC2436(float vref, uint16_t eoc_timeout)
{
  float voltage = 0.0;
  int32_t adc_code = 0;
  uint8_t channel;
  uint8_t i;

  quikeval_SPI_connect();                                           // Connect SPI to main data port
  set_CS_decoder(LTC2436_CS_ADDR);                                  // Set the decoder for the LTC2436 chip select

  Serial.println(F("\nTesting LTC2436 with a 24 bit read..."));

  LTC24XX_EOC_timeout(CS, eoc_timeout);                             // First flush power-on conversion that probably happened before
  LTC24XX_SPI_2ch_ping_pong_32bit_data(CS, &channel, &adc_code);    // the DAC output was set

  for (i=0; i <= 1; ++i)
  {
    LTC24XX_EOC_timeout(CS, eoc_timeout);                           // Wait for end of conversion
    LTC24XX_SPI_2ch_ping_pong_24bit_data(CS, &channel, &adc_code);  // Read the ADC and channel
    voltage = LTC24XX_diff_code_to_voltage(adc_code, vref);         // Convert ADC code to voltage

    // Display results
    Serial.print(F("  Formatted LTC2436 ADC code CH"));
    Serial.print(channel);
    Serial.print(F(": 0x"));
    Serial.println(adc_code, HEX);
    Serial.print(F(" Voltage: "));
    Serial.print(voltage, 8);
    Serial.println(F(" V"));
  }
}

//! Reads the LTC2439 and displays the results
void test_LTC2439(float vref, uint16_t eoc_timeout)
{
  float se_results[16], diff_results[16];
  int32_t adc_code = 0;
  uint8_t i;

  quikeval_SPI_connect();                       // Connect SPI to main data port
  set_CS_decoder(LTC2439_CS_ADDR);              // Set the decoder for the LTC2439 chip select

  Serial.println(F("\nTesting LTC2439, 16 bit ADC with a 19 bit output word using LTC24XX_SPI_8bit_command_24bit_data() function"));
  Serial.println(F("Reading out all Single-Ended channels..."));

  LTC24XX_EOC_timeout(CS, eoc_timeout);         // Wait for end of conversion
  LTC24XX_SPI_8bit_command_24bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[0], &adc_code);             // Prime with CH0

  for (i=0; i<=15; i++)
  {
    LTC24XX_EOC_timeout(CS, eoc_timeout);       // Wait for end of conversion
    LTC24XX_SPI_8bit_command_24bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[(i+1)%16], &adc_code);    // Read the ADC
    se_results[i%16] = LTC24XX_diff_code_to_voltage(adc_code, vref);                            // Convert ADC code to voltage
  }

  Serial.println(F("Reading out all Differential channels..."));

  LTC24XX_EOC_timeout(CS, eoc_timeout);         // Wait for end of conversion
  LTC24XX_SPI_8bit_command_24bit_data(CS, DIFF_CONFIG_ARRAY[0], &adc_code);                     // Prime with CH_P0_N1

  for (i=0; i<=7; i++)
  {
    LTC24XX_EOC_timeout(CS, eoc_timeout);       // Wait for end of conversion
    LTC24XX_SPI_8bit_command_24bit_data(CS, DIFF_CONFIG_ARRAY[(i+1)%16], &adc_code);            // Read the ADC
    diff_results[i%8] = LTC24XX_diff_code_to_voltage(adc_code, vref);                           // Convert ADC code to voltage
  }

  print_all(se_results, diff_results);          // Display results
}

//! Reads the LTC2449 and displays the results
void test_LTC2449(float vref, uint16_t eoc_timeout)
{
  float se_results[16], diff_results[16];
  int32_t adc_code = 0;
  uint8_t i;

  quikeval_SPI_connect();                       // Connect SPI to main data port
  set_CS_decoder(LTC2449_CS_ADDR);              // Set the decoder for the LTC2449 chip select

  Serial.println(F("\nTesting LTC2449, 24 bit ADC with a 24 bit output word using LTC24XX_SPI_16bit_command_32bit_data() function"));
  Serial.println(F("Reading out all Single-Ended channels..."));

  LTC24XX_EOC_timeout(CS, eoc_timeout);         // Wait for end of conversion
  LTC24XX_SPI_16bit_command_32bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[0], OSR_CONFIG_ARRAY[0], &adc_code);   // Prime with CH0

  for (i=0; i<=15; i++)
  {
    LTC24XX_EOC_timeout(CS, eoc_timeout);       // Wait for end of conversion
    LTC24XX_SPI_16bit_command_32bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[(i+1)%16], OSR_CONFIG_ARRAY[(i+1)%16], &adc_code);   // Read the ADC
    se_results[i%16] = LTC24XX_diff_code_to_voltage(adc_code, vref);                            // Convert ADC code to voltage
  }

  Serial.println(F("Reading out all Differential channels..."));

  LTC24XX_EOC_timeout(CS, eoc_timeout);         // Wait for end of conversion
  LTC24XX_SPI_16bit_command_32bit_data(CS, DIFF_CONFIG_ARRAY[0], OSR_CONFIG_ARRAY[0], &adc_code);   // Prime with CH_P0_N1

  for (i=0; i<=7; i++)
  {
    LTC24XX_EOC_timeout(CS, eoc_timeout);       // Wait for end of conversion
    LTC24XX_SPI_16bit_command_32bit_data(CS, DIFF_CONFIG_ARRAY[(i+1)%16], OSR_CONFIG_ARRAY[(i+1)%16], &adc_code);   // Read the ADC
    diff_results[i%8] = LTC24XX_diff_code_to_voltage(adc_code, vref);                           // Convert ADC code to voltage
  }

  print_all(se_results, diff_results);          // Display results
}

//! Reads the LTC2494 and displays the results
void test_LTC2494(float vref, uint16_t eoc_timeout)
{
  float se_results[16], diff_results[16];
  int32_t adc_code = 0;
  uint8_t i;

  quikeval_SPI_connect();                       // Connect SPI to main data port
  set_CS_decoder(LTC2494_CS_ADDR);              // Set the decoder for the LTC2494 chip select

  Serial.println(F("\nTesting LTC2494, 16 bit ADC with a 19 bit output word using LTC24XX_SPI_8bit_command_24bit_data() function"));
  Serial.println(F("Reading out all Single-Ended channels..."));

  LTC24XX_EOC_timeout(CS, eoc_timeout);         // Wait for end of conversion
  LTC24XX_SPI_8bit_command_24bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[0], &adc_code);             // Prime with CH0

  for (i=0; i<=15; i++)
  {
    LTC24XX_EOC_timeout(CS, eoc_timeout);       // Wait for end of conversion
    LTC24XX_SPI_8bit_command_24bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[(i+1)%16], &adc_code);    // Read the ADC
    se_results[i%16] = LTC24XX_diff_code_to_voltage(adc_code, vref);                            // Convert ADC code to voltage
  }

  Serial.println(F("Reading out all Differential channels..."));

  LTC24XX_EOC_timeout(CS, eoc_timeout);         // Wait for end of conversion
  LTC24XX_SPI_8bit_command_24bit_data(CS, DIFF_CONFIG_ARRAY[0], &adc_code);                     // Prime with CH_P0_N1

  for (i=0; i<=7; i++)
  {
    LTC24XX_EOC_timeout(CS, eoc_timeout);       // Wait for end of conversion
    LTC24XX_SPI_8bit_command_24bit_data(CS, DIFF_CONFIG_ARRAY[(i+1)%16], &adc_code);            // Read the ADC
    diff_results[i%8] = LTC24XX_diff_code_to_voltage(adc_code, vref);                           // Convert ADC code to voltage
  }

  print_all(se_results, diff_results);          // Display results
}

//! Reads the LTC2495 and displays the results
void test_LTC2495(float vref, uint16_t eoc_timeout)
{
  int32_t adc_code = 0;
  int8_t ack, i;
  float se_results[16], diff_results[16];

  quikeval_I2C_connect();                                               // Connect I2C to main data port

  Serial.println(F("\nTesting LTC2495, 24 bit output using LTC24XX_I2C_16bit_command_32bit_data..."));
  Serial.println(F("Reading out all Single-Ended channels..."));

  ack = LTC24XX_I2C_16bit_command_32bit_data(LTC2495_ADDR, SINGLE_ENDED_CONFIG_ARRAY[0], 0, &adc_code, eoc_timeout);            // Prime with CH0

  for (i=0; i <= 15; i++)
  {
    ack = LTC24XX_I2C_16bit_command_32bit_data(LTC2495_ADDR, SINGLE_ENDED_CONFIG_ARRAY[(i+1)%16], 0, &adc_code, eoc_timeout);    // Read the ADC
    se_results[i%16] = LTC24XX_diff_code_to_voltage(adc_code, vref);     // Convert ADC code to voltage
  }

  Serial.println(F("Reading out all Differential channels..."));

  ack = LTC24XX_I2C_16bit_command_32bit_data(LTC2495_ADDR, DIFF_CONFIG_ARRAY[0], 0, &adc_code, eoc_timeout);                    // Prime with CH_P0_N1
  for (i=0; i <= 7; i++)
  {
    ack = LTC24XX_I2C_16bit_command_32bit_data(LTC2495_ADDR, DIFF_CONFIG_ARRAY[(i+1)%16], 0, &adc_code, eoc_timeout);           // Read the ADC
    diff_results[i%8] = LTC24XX_diff_code_to_voltage(adc_code, vref);   // Convert ADC code to voltage
  }

  if (!ack)
    print_all(se_results, diff_results);                                // Display results
  else
    Serial.println(F("LTC2497 Nacked, check address."));
}

//! Reads the LTC2496 and displays the results
void test_LTC2496(float vref, uint16_t eoc_timeout)
{
  float se_results[16], diff_results[16];
  int32_t adc_code = 0;
  uint8_t i;

  quikeval_SPI_connect();                       // Connect SPI to main data port
  set_CS_decoder(LTC2496_CS_ADDR);              // Set the decoder for the LTC2496 chip select

  Serial.println(F("\nTesting LTC2496, 16 bit ADC with a 19 bit output word using LTC24XX_SPI_8bit_command_32bit_data() function"));
  Serial.println(F("Reading out all Single-Ended channels..."));

  LTC24XX_EOC_timeout(CS, eoc_timeout);         // Wait for end of conversion
  LTC24XX_SPI_8bit_command_32bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[0], &adc_code);             // Prime with CH0

  for (i=0; i<=15; i++)
  {
    LTC24XX_EOC_timeout(CS, eoc_timeout);       // Wait for end of conversion
    LTC24XX_SPI_8bit_command_32bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[(i+1)%16], &adc_code);    // Read the ADC
    se_results[i%16] = LTC24XX_diff_code_to_voltage(adc_code, vref);                            // Convert ADC code to voltage
  }

  Serial.println(F("Reading out all Differential channels..."));

  LTC24XX_EOC_timeout(CS, eoc_timeout);         // Wait for end of conversion
  LTC24XX_SPI_8bit_command_32bit_data(CS, DIFF_CONFIG_ARRAY[0], &adc_code);                     // Prime with CH_P0_N1

  for (i=0; i<=7; i++)
  {
    LTC24XX_EOC_timeout(CS, eoc_timeout);       // Wait for end of conversion
    LTC24XX_SPI_8bit_command_32bit_data(CS, DIFF_CONFIG_ARRAY[(i+1)%16], &adc_code);            // Read the ADC
    diff_results[i%8] = LTC24XX_diff_code_to_voltage(adc_code, vref);                           // Convert ADC code to voltage
  }

  print_all(se_results, diff_results);          // Display results
}

//! Reads the LTC2497 and displays the results
void test_LTC2497(float vref, uint16_t eoc_timeout)
{
  int32_t adc_code = 0;
  int8_t ack, i;
  float se_results[16], diff_results[16];

  quikeval_I2C_connect();                   // Connect I2C to main data port

  Serial.println(F("\nTesting LTC2497, 16 bit output using LTC24XX_I2C_8bit_command_24bit_data..."));
  Serial.println(F("Reading out all Single-Ended channels..."));

  ack = LTC24XX_I2C_8bit_command_24bit_data(LTC2497_ADDR, SINGLE_ENDED_CONFIG_ARRAY[0], &adc_code, eoc_timeout);            // Prime with CH0

  for (i=0; i <= 15; i++)
  {
    ack = LTC24XX_I2C_8bit_command_24bit_data(LTC2497_ADDR, SINGLE_ENDED_CONFIG_ARRAY[(i+1)%16], &adc_code, eoc_timeout);   // Read the ADC
    se_results[i%16] = LTC24XX_diff_code_to_voltage(adc_code, vref);                                                        // Convert ADC code to voltage
  }

  Serial.println(F("Reading out all Differential channels..."));

  ack = LTC24XX_I2C_8bit_command_24bit_data(LTC2497_ADDR, DIFF_CONFIG_ARRAY[0], &adc_code, eoc_timeout);                    // Prime with CH_P0_N1

  for (i=0; i <= 7; i++)
  {
    ack = LTC24XX_I2C_8bit_command_24bit_data(LTC2497_ADDR, DIFF_CONFIG_ARRAY[(i+1)%16], &adc_code, eoc_timeout);           // Read the ADC
    diff_results[i%8] = LTC24XX_diff_code_to_voltage(adc_code, vref);                                                       // Convert ADC code to voltage
  }

  if (!ack)
    print_all(se_results, diff_results);        // Display results
  else
    Serial.println(F("LTC2497 Nacked, check address."));
}

//! Reads the LTC2498 and displays the results
void test_LTC2498(float vref, uint16_t eoc_timeout)
{
  float se_results[16], diff_results[16];
  int32_t adc_code = 0;
  uint8_t i;

  quikeval_SPI_connect();                   // Connect SPI to main data port
  set_CS_decoder(LTC2498_CS_ADDR);          // Set the decoder for the LTC2498 chip select

  Serial.println(F("\nTesting LTC2498, 32 bit output using LTC24XX_SPI_16bit_command_32bit_data..."));

  Serial.println(F("Reading out all Single-Ended channels..."));
  LTC24XX_EOC_timeout(CS, eoc_timeout);     // Wait for end of conversion
  LTC24XX_SPI_16bit_command_32bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[0], 0, &adc_code);             // Prime with CH0
  for (i=0; i<=15; i++)
  {
    LTC24XX_EOC_timeout(CS, eoc_timeout);   // Wait for end of conversion
    LTC24XX_SPI_16bit_command_32bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[(i+1)%16], 0, &adc_code);    // Read the ADC
    se_results[i%16] = LTC24XX_diff_code_to_voltage(adc_code, vref);                                // Convert ADC code to voltage
  }

  Serial.println(F("Reading out all Differential channels..."));
  LTC24XX_EOC_timeout(CS, eoc_timeout);     // Wait for end of conversion
  LTC24XX_SPI_16bit_command_32bit_data(CS, DIFF_CONFIG_ARRAY[0], 0, &adc_code); //Prime with CH_P0_N1
  for (i=0; i<=7; i++)
  {
    LTC24XX_EOC_timeout(CS, eoc_timeout);   // Wait for end of conversion
    LTC24XX_SPI_16bit_command_32bit_data(CS, DIFF_CONFIG_ARRAY[(i+1)%16], 0, &adc_code);            // Read the ADC
    diff_results[i%8] = LTC24XX_diff_code_to_voltage(adc_code, vref);                               // Convert ADC code to voltage
  }

  print_all(se_results, diff_results);      // Display results
}

//! Reads the LTC2499 and displays the results
void test_LTC2499(float vref, uint16_t eoc_timeout)
{
  int32_t adc_code = 0;
  int8_t ack, i;
  float se_results[16], diff_results[16];

  quikeval_I2C_connect();                   // Connect I2C to main data port

  Serial.println(F("\nTesting LTC2499, 24 bit output using LTC24XX_I2C_16bit_command_32bit_data..."));
  Serial.println(F("Reading out all Single-Ended channels..."));

  ack = LTC24XX_I2C_16bit_command_32bit_data(LTC2499_ADDR, SINGLE_ENDED_CONFIG_ARRAY[0], 0, &adc_code, eoc_timeout);    // Prime with CH0

  for (i=0; i <= 15; i++)
  {
    ack = LTC24XX_I2C_16bit_command_32bit_data(LTC2499_ADDR, SINGLE_ENDED_CONFIG_ARRAY[(i+1)%16], 0, &adc_code, eoc_timeout);   // Read the ADC
    se_results[i%16] = LTC24XX_diff_code_to_voltage(adc_code, vref);                                                    // Convert ADC code to voltage
  }

  Serial.println(F("Reading out all Differential channels..."));

  ack = LTC24XX_I2C_16bit_command_32bit_data(LTC2499_ADDR, DIFF_CONFIG_ARRAY[0], 0, &adc_code, eoc_timeout);            // Prime with CH_P0_N1

  for (i=0; i <= 7; i++)
  {
    ack = LTC24XX_I2C_16bit_command_32bit_data(LTC2499_ADDR, DIFF_CONFIG_ARRAY[(i+1)%16], 0, &adc_code, eoc_timeout);   // Read the ADC
    diff_results[i%8] = LTC24XX_diff_code_to_voltage(adc_code, vref);                                                   // Convert ADC code to voltage
  }

  if (!ack)
    print_all(se_results, diff_results);        // Display results
  else
    Serial.println(F("LTC2497 Nacked, check address."));
}

//! Sets the LTC6998, reads all the ADCs, and displays the results
void test_all(float vref, uint16_t eoc_timeout)
{
  uint8_t i;
  int32_t adc_code = 0;
  float voltage;

  // Write a ramp of voltages
  set_CS_decoder(LTC2668_CS_ADDR);  // And point to the LTC2668 first...
  quikeval_SPI_connect();           // Connect SPI to main data port
  for (i=0; i <= 15; ++i)
  {
    LTC2668_write(LTC2668_CS,LTC2668_CMD_WRITE_N_UPDATE_N, i, (i + 1) * 1312);
  }

  // Read all the ADCs
  test_LTC2400(vref, eoc_timeout);
  test_LTC2412(vref, eoc_timeout);
  test_LTC2418(vref, eoc_timeout);
  test_LTC2420(vref, eoc_timeout);
  test_LTC2436(vref, eoc_timeout);
  test_LTC2439(vref, eoc_timeout);
  test_LTC2449(vref, eoc_timeout);
  test_LTC2494(vref, eoc_timeout);
  test_LTC2495(vref, eoc_timeout);
  test_LTC2496(vref, eoc_timeout);
  test_LTC2497(vref, eoc_timeout);
  test_LTC2498(vref, eoc_timeout);
  test_LTC2499(vref, eoc_timeout);

  delay(150);

  Serial.println(F("Done!"));
  while (1) {}
}

/*** main() ********************************************************************
Main program initializes microcontroller registers, checks to see if calibration
mode is selected, reads calibration constants from nonvolatile memory.
Main loop reads ADC input and PTAT voltage, calculates temperatures and voltages,
then sends this information to the display.
*******************************************************************************/
/*
void loop()
{
  // Write a ramp of voltages
  set_CS_decoder(LTC2668_CS_ADDR);  // And point to the LTC2668 first...
  quikeval_SPI_connect();           // Connect SPI to main data port
  for(i=0; i <= 15; ++i)
  {
    LTC2668_write(LTC2668_CS,LTC2668_CMD_WRITE_N_UPDATE_N, i, (i + 1) * 1312);
  }

//  Uncomment the following lines to test code to voltage function:
//  Serial.print("Testing - Voltage is ");
//  voltage = LTC24XX_diff_code_to_voltage(adc_code, LTC24XX_vref);
//  Serial.println(voltage, 6);

#ifdef TEST_LTC2400 // LTC24XX_SPI_32bit_data

  quikeval_SPI_connect();                   // Connect SPI to main data port
  set_CS_decoder(LTC2400_CS_ADDR);
  Serial.println("\nTesting LTC2400 with a 32 bit read using LTC24XX_SPI_32bit_data...");

  LTC24XX_EOC_timeout(CS, 250);             // First flush power-on conversion that probably happened before
  LTC24XX_SPI_32bit_data(CS, &adc_code);    // the DAC output was set

  LTC24XX_EOC_timeout(CS, 250);             // First flush power-on conversion that probably happened before
  LTC24XX_SPI_32bit_data(CS, &adc_code);    // the DAC output was set

  Serial.print("Got formatted code ");
  Serial.print(adc_code, HEX);
  Serial.print(", Which is a voltage of ");
  voltage = LTC24XX_SE_code_to_voltage(adc_code, LTC24XX_vref);
  Serial.println(voltage, 8);

#endif // TEST_LTC2400


#ifdef TEST_LTC2418
  set_CS_decoder(LTC2418_CS_ADDR); //LTC24XX_SPI_8bit_command_32bit_data

  quikeval_SPI_connect();         // Connect SPI to main data port
  Serial.println("\nTesting LTC2418, 16 bit ADC with a 19 bit output word using LTC24XX_SPI_8bit_command_32bit_data() function");

  Serial.println("Reading out all Single-Ended channels...");
  LTC24XX_EOC_timeout(CS, 250);
  LTC24XX_SPI_8bit_command_32bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[0], &adc_code); //Prime with CH0
  for(i=0; i<=15; i++)
  {
    LTC24XX_EOC_timeout(CS, 250);
    LTC24XX_SPI_8bit_command_32bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[(i+1)%16], &adc_code);
    voltage = LTC24XX_diff_code_to_voltage(adc_code, LTC24XX_vref);
    se_results[i%16] = voltage;
  }

  Serial.println("Reading out all Differential channels...");
  LTC24XX_EOC_timeout(CS, 250);
  LTC24XX_SPI_8bit_command_32bit_data(CS, DIFF_CONFIG_ARRAY[0], &adc_code); //Prime with CH_P0_N1
  for(i=0; i<=7; i++)
  {
    LTC24XX_EOC_timeout(CS, 250);
    LTC24XX_SPI_8bit_command_32bit_data(CS, DIFF_CONFIG_ARRAY[(i+1)%16], &adc_code);
    voltage = LTC24XX_diff_code_to_voltage(adc_code, LTC24XX_vref);
    diff_results[i%8] = voltage;
  }

  print_all();

#endif // TEST_LTC2418


#ifdef TEST_LTC2498 //LTC24XX_SPI_16bit_command_32bit_data

  set_CS_decoder(LTC2498_CS_ADDR);

  quikeval_SPI_connect();         // Connect SPI to main data port
  Serial.println("\nTesting LTC2498, 32 bit output using LTC24XX_SPI_16bit_command_32bit_data...");

  Serial.println("Reading out all Single-Ended channels...");
  LTC24XX_EOC_timeout(CS, 250);
  LTC24XX_SPI_16bit_command_32bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[0], 0, &adc_code); //Prime with CH0
  for(i=0; i<=15; i++)
  {
    LTC24XX_EOC_timeout(CS, 250);
    LTC24XX_SPI_16bit_command_32bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[(i+1)%16], 0, &adc_code);
    voltage = LTC24XX_diff_code_to_voltage(adc_code, LTC24XX_vref);
    se_results[i%16] = voltage;
  }

  Serial.println("Reading out all Differential channels...");
  LTC24XX_EOC_timeout(CS, 250);
  LTC24XX_SPI_16bit_command_32bit_data(CS, DIFF_CONFIG_ARRAY[0], 0, &adc_code); //Prime with CH_P0_N1
  for(i=0; i<=7; i++)
  {
    LTC24XX_EOC_timeout(CS, 250);
    LTC24XX_SPI_16bit_command_32bit_data(CS, DIFF_CONFIG_ARRAY[(i+1)%16], 0, &adc_code);
    voltage = LTC24XX_diff_code_to_voltage(adc_code, LTC24XX_vref);
    diff_results[i%8] = voltage;
  }

  print_all();

#endif // TEST_LTC2498

#ifdef TEST_LTC2439 //LTC24XX_SPI_8bit_command_24bit_data
  set_CS_decoder(LTC2439_CS_ADDR);

  quikeval_SPI_connect();         // Connect SPI to main data port
  Serial.println("\nTesting LTC2439, 16 bit ADC with a 19 bit output word using LTC24XX_SPI_8bit_command_24bit_data() function");

  Serial.println("Reading out all Single-Ended channels...");
  LTC24XX_EOC_timeout(CS, 250);
  LTC24XX_SPI_8bit_command_24bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[0], &adc_code); //Prime with CH0
  for(i=0; i<=15; i++)
  {
    LTC24XX_EOC_timeout(CS, 250);
    LTC24XX_SPI_8bit_command_24bit_data(CS, SINGLE_ENDED_CONFIG_ARRAY[(i+1)%16], &adc_code);
    voltage = LTC24XX_diff_code_to_voltage(adc_code, LTC24XX_vref);
    se_results[i%16] = voltage;
  }

  Serial.println("Reading out all Differential channels...");
  LTC24XX_EOC_timeout(CS, 250);
  LTC24XX_SPI_8bit_command_24bit_data(CS, DIFF_CONFIG_ARRAY[0], &adc_code); //Prime with CH_P0_N1
  for(i=0; i<=7; i++)
  {
    LTC24XX_EOC_timeout(CS, 250);
    LTC24XX_SPI_8bit_command_24bit_data(CS, DIFF_CONFIG_ARRAY[(i+1)%16], &adc_code);
    voltage = LTC24XX_diff_code_to_voltage(adc_code, LTC24XX_vref);
    diff_results[i%8] = voltage;
  }

  print_all();
#endif // TEST_LTC2439

#ifdef TEST_LTC2420 // LTC24XX_SPI_32bit_data

  quikeval_SPI_connect();         // Connect SPI to main data port
  set_CS_decoder(LTC2420_CS_ADDR);
  Serial.println("\nTesting LTC2420 with a 24 bit read using LTC24XX_SPI_24bit_data...");

  LTC24XX_EOC_timeout(CS, 250);   // First flush power-on conversion that probably happened before
  LTC24XX_SPI_24bit_data(CS, &adc_code);  // the DAC output was set

  LTC24XX_EOC_timeout(CS, 250);   // First flush power-on conversion that probably happened before
  LTC24XX_SPI_24bit_data(CS, &adc_code);  // the DAC output was set

  Serial.print("Got formatted code "); Serial.print(adc_code, HEX);
  Serial.print(", Which is a voltage of ");
  voltage = LTC24XX_SE_code_to_voltage(adc_code, LTC24XX_vref);
  Serial.println(voltage, 8);

#endif // TEST_LTC2420

#ifdef TEST_LTC2497
  quikeval_I2C_connect();

  Serial.println("\nTesting LTC2497, 24 bit output using LTC24XX_I2C_8bit_command_24bit_data...");
  Serial.println("Reading out all Single-Ended channels...");

  ack = LTC24XX_I2C_8bit_command_24bit_data(LTC2497_ADDR, SINGLE_ENDED_CONFIG_ARRAY[0], &adc_code, 150); //Prime with CH0
  for(i=0; i<=15; i++)
  {
    ack = LTC24XX_I2C_8bit_command_24bit_data(LTC2497_ADDR, SINGLE_ENDED_CONFIG_ARRAY[(i+1)%16], &adc_code, 150);
    voltage = LTC24XX_diff_code_to_voltage(adc_code, LTC24XX_vref);
    se_results[i%16] = voltage;
  }

  Serial.println("Reading out all Differential channels...");

  ack = LTC24XX_I2C_8bit_command_24bit_data(LTC2497_ADDR, DIFF_CONFIG_ARRAY[0], &adc_code, 150); //Prime with CH_P0_N1
  for(i=0; i<=7; i++)
  {
    LTC24XX_EOC_timeout(CS, 250);
    ack = LTC24XX_I2C_8bit_command_24bit_data(LTC2497_ADDR, DIFF_CONFIG_ARRAY[(i+1)%16], &adc_code, 150);
    voltage = LTC24XX_diff_code_to_voltage(adc_code, LTC24XX_vref);
    diff_results[i%8] = voltage;
  }

  if(!ack)
    print_all();
  else
  Serial.println("LTC2497 Nacked, check address.");}

#endif // TEST_LTC2497

#ifdef TEST_LTC2499
//int8_t LTC24XX_I2C_16bit_command_32bit_data(uint8_t i2c_address,uint8_t adc_command_high,
//                                            uint8_t adc_command_low,int32_t *adc_code,uint16_t eoc_timeout)


#endif // TEST_LTC2499



#ifdef TEST_LTC2436
  quikeval_SPI_connect();         // Connect SPI to main data port
  set_CS_decoder(LTC2436_CS_ADDR);
  Serial.println("\nTesting LTC2436 with a 24 bit read...");

  LTC24XX_EOC_timeout(CS, 250);   // First flush power-on conversion that probably happened before
  LTC24XX_SPI_2ch_ping_pong_32bit_data(CS, &channel, &adc_code);  // the DAC output was set

  for(i=0; i<=1; ++i)
  {
    LTC24XX_EOC_timeout(CS, 250);
    LTC24XX_SPI_2ch_ping_pong_24bit_data(CS, &channel, &adc_code);
    Serial.print("Got formatted code "); Serial.print(adc_code, HEX);
    Serial.print("From Channel "); Serial.print(channel);
    Serial.print(", Which is a voltage of ");
    voltage = LTC24XX_diff_code_to_voltage(adc_code, LTC24XX_vref);
    Serial.println(voltage, 8);
  }

#endif // TEST_LTC2436

#ifdef TEST_LTC2412
  quikeval_SPI_connect();         // Connect SPI to main data port
  set_CS_decoder(LTC2412_CS_ADDR);
  Serial.println("\nTesting LTC2412 with a 32 bit read...");

  LTC24XX_EOC_timeout(CS, 250);   // First flush power-on conversion that probably happened before
  LTC24XX_SPI_2ch_ping_pong_32bit_data(CS, &channel, &adc_code);  // the DAC output was set

  for(i=0; i<=1; ++i)
  {
    LTC24XX_EOC_timeout(CS, 250);
    LTC24XX_SPI_2ch_ping_pong_32bit_data(CS, &channel, &adc_code);
    Serial.print("Got formatted code "); Serial.print(adc_code, HEX);
    Serial.print("From Channel "); Serial.print(channel);
    Serial.print(", Which is a voltage of ");
    voltage = LTC24XX_diff_code_to_voltage(adc_code, LTC24XX_vref);
    Serial.println(voltage, 8);
  }

#endif // TEST_LTC2412

delay(150);

Serial.println("Done!");
while(1){}

}
*/