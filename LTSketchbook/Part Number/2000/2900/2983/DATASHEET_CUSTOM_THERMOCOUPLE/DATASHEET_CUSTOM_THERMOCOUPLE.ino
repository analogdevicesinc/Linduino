/*!
LTC2983: Multi-Sensor High Accuracy Digital Temperature Measurement System
@verbatim

DATASHEET_CUSTOM_THERMOCOUPLE.ino:
Generated Linduino code from the LTC2983 Demo Software.
This code  is designed to be used by a Linduino,
but can also be used to understand how to program the LTC2983.


@endverbatim

http://www.linear.com/product/LTC2983

http://www.linear.com/product/LTC2983#demoboards

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

  // ----- Channel 1: Assign Custom Thermocouple -----
  channel_assignment_data =
    SENSOR_TYPE__CUSTOM_THERMOCOUPLE |
    TC_COLD_JUNCTION_CH__2 |
    TC_SINGLE_ENDED |
    TC_OPEN_CKT_DETECT__NO |
    TC_OPEN_CKT_DETECT_CURRENT__10UA |
    (uint32_t) 0x0 << TC_CUSTOM_ADDRESS_LSB |   // tc - custom address: 0.
    (uint32_t) 0x9 << TC_CUSTOM_LENGTH_1_LSB;   // tc - custom length-1: 9.
  assign_channel(CHIP_SELECT, 1, channel_assignment_data);
  // ----- Channel 2: Assign Off-Chip Diode -----
  channel_assignment_data =
    SENSOR_TYPE__OFF_CHIP_DIODE |
    DIODE_SINGLE_ENDED |
    DIODE_NUM_READINGS__3 |
    DIODE_AVERAGING_ON |
    DIODE_CURRENT__20UA_80UA_160UA |
    (uint32_t) 0x100C49 << DIODE_IDEALITY_FACTOR_LSB;   // diode - ideality factor(eta): 1.00299930572509765625
  assign_channel(CHIP_SELECT, 2, channel_assignment_data);

}


void configure_memory_table()
{
  uint16_t start_address;
  uint16_t table_length;
  // int i;

  // -- Channel 1 custom table --
  table_coeffs ch_1_coefficients[] =
  {
    { 15954412, 0 },  // -- -50.22, 0.0
    { 16282419, 101478 }, // -- -30.2, 99.1
    { 16690381, 138650 }, // -- -5.3, 135.4
    { 0, 279706 },  // -- 0.0, 273.15
    { 658637, 369869 }, // -- 40.2, 361.2
    { 906035, 534630 }, // -- 55.3, 522.1
    { 1446707, 737587 },  // -- 88.3, 720.3
    { 2165965, 830669 },  // -- 132.2, 811.2
    { 3091661, 944640 },  // -- 188.7, 922.5
    { 7543194, 1024000 }  // -- 460.4, 1000.0
  };
  start_address = (uint16_t) 592; // Real address = 6*0 + 0x250 = 592
  table_length = (uint8_t) 10;  // Real table length = 9 + 1 = 10
  write_custom_table(CHIP_SELECT, ch_1_coefficients, start_address, table_length);


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
  measure_channel(CHIP_SELECT, 1, TEMPERATURE);      // Ch 1: Custom Thermocouple
  measure_channel(CHIP_SELECT, 2, TEMPERATURE);      // Ch 2: Off-Chip Diode
}