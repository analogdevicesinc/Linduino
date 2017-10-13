/*!
LTC2983: Multi-Sensor High Accuracy Digital Temperature Measurement System
@verbatim

DATASHEET_CUSTOM_RTD.ino:
Generated Linduino code from the LTC2983 Demo Software.
This code  is designed to be used by a Linduino,
but can also be used to understand how to program the LTC2983.


@endverbatim

http://www.linear.com/product/LTC2983

http://www.linear.com/product/LTC2983#demoboards

$Revision: 5797 $
$Date: 2016-09-23 10:30:32 -0700 (Fri, 23 Sep 2016) $
Copyright (c) 2014, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/



/*! @file
    @ingroup LTC2983

*/




#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include "SPI.h"
#include "Wire.h"
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "stdio.h"
#include "math.h"

#include "configuration_constants_LTC2983.h"
#include "support_functions_LTC2983.h"
#include "table_coeffs_LTC2983.h"

#define CHIP_SELECT QUIKEVAL_CS  // Chip select pin

// Function prototypes
void configure_memory_table();
void configure_channels();
void configure_global_parameters();


// -------------- Configure the LTC2983 -------------------------------
void setup()
{
  char demo_name[]="DC2209";     // Demo Board Name stored in QuikEval EEPROM
  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz
  quikeval_SPI_init();          // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();       // Connect SPI to main data port
  pinMode(CHIP_SELECT, OUTPUT); // Configure chip select pin on Linduino

  Serial.begin(115200);         // Initialize the serial port to the PC
  print_title();
  discover_demo_board(demo_name);

  configure_channels();
  configure_memory_table();
  configure_global_parameters();
}


void configure_channels()
{
  uint8_t channel_number;
  uint32_t channel_assignment_data;

  // ----- Channel 4: Assign Sense Resistor -----
  channel_assignment_data =
    SENSOR_TYPE__SENSE_RESISTOR |
    (uint32_t) 0x4E20CC << SENSE_RESISTOR_VALUE_LSB;    // sense resistor - value: 5000.19921875
  assign_channel(CHIP_SELECT, 4, channel_assignment_data);
  // ----- Channel 6: Assign RTD Custom -----
  channel_assignment_data =
    SENSOR_TYPE__RTD_CUSTOM |
    RTD_RSENSE_CHANNEL__4 |
    RTD_NUM_WIRES__4_WIRE |
    RTD_EXCITATION_MODE__ROTATION_SHARING |
    RTD_EXCITATION_CURRENT__5UA |
    (uint32_t) 0xA << RTD_CUSTOM_ADDRESS_LSB |    // rtd - custom address: 10.
    (uint32_t) 0x9 << RTD_CUSTOM_LENGTH_1_LSB;    // rtd - custom length-1: 9.
  assign_channel(CHIP_SELECT, 6, channel_assignment_data);
  // ----- Channel 11: Assign Sense Resistor -----
  channel_assignment_data =
    SENSOR_TYPE__SENSE_RESISTOR |
    (uint32_t) 0x4E20CC << SENSE_RESISTOR_VALUE_LSB;    // sense resistor - value: 5000.19921875
  assign_channel(CHIP_SELECT, 11, channel_assignment_data);

}


void configure_memory_table()
{
  uint16_t start_address;
  uint16_t table_length;
  // int i;

  // -- Channel 6 custom table --
  table_coeffs ch_6_coefficients[] =
  {
    { 0, 114995 },  // -- 0.0, 112.3
    { 163840, 205373 }, // -- 80.0, 200.56
    { 307200, 279716 }, // -- 150.0, 273.16
    { 527073, 386304 }, // -- 257.36, 377.25
    { 694723, 501412 }, // -- 339.22, 489.66
    { 795156, 609505 }, // -- 388.26, 595.22
    { 1050604, 714619 },  // -- 512.99, 697.87
    { 1356390, 783503 },  // -- 662.3, 765.14
    { 1522688, 820449 },  // -- 743.5, 801.22
    { 4099871, 922112 }   // -- 2001.89, 900.5
  };
  start_address = (uint16_t) 652; // Real address = 6*10 + 0x250 = 652
  table_length = (uint8_t) 10;  // Real table length = 9 + 1 = 10
  write_custom_table(CHIP_SELECT, ch_6_coefficients, start_address, table_length);


}



void configure_global_parameters()
{
  // -- Set global parameters
  transfer_byte(CHIP_SELECT, WRITE_TO_RAM, 0xF0, TEMP_UNIT__C |
                REJECTION__50_60_HZ);
  // -- Set any extra delay between conversions (in this case, 0*100us)
  transfer_byte(CHIP_SELECT, WRITE_TO_RAM, 0xFF, 0);
}

// -------------- Run the LTC2983 -------------------------------------

void loop()
{
  measure_channel(CHIP_SELECT, 6, TEMPERATURE);      // Ch 6: RTD Custom
}