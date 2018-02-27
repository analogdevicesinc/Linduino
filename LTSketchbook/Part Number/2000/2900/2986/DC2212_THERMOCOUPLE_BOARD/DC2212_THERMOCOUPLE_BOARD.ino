/*!
LTC2986: Multi-Sensor High Accuracy Digital Temperature Measurement System
@verbatim

DC2212_THERMOCOUPLE_BOARD.ino:
Generated Linduino code from the LTC2986 Demo Software.
This code  is designed to be used by a Linduino,
but can also be used to understand how to program the LTC2986.


@endverbatim

http://www.linear.com/product/LTC2986

http://www.linear.com/product/LTC2986#demoboards

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
    @ingroup LTC2986

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

#include "configuration_constants_LTC2986.h"
#include "support_functions_LTC2986.h"
#include "table_coeffs_LTC2986.h"

#define CHIP_SELECT QUIKEVAL_CS  // Chip select pin

// Function prototypes
void configure_channels();
void configure_global_parameters();


// -------------- Configure the LTC2986 -------------------------------
void setup()
{
  char demo_name[]="DC2508";     // Demo Board Name stored in QuikEval EEPROM
  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz
  quikeval_SPI_init();          // Configure the spi port for 4MHz SCK
  quikeval_SPI_connect();       // Connect SPI to main data port
  pinMode(CHIP_SELECT, OUTPUT); // Configure chip select pin on Linduino

  Serial.begin(115200);         // Initialize the serial port to the PC
  print_title();
  discover_demo_board(demo_name);

  configure_channels();
  configure_global_parameters();
}


void configure_channels()
{
  uint8_t channel_number;
  uint32_t channel_assignment_data;

  // ----- Channel 1: Assign Type J Thermocouple -----
  channel_assignment_data =
    SENSOR_TYPE__TYPE_J_THERMOCOUPLE |
    TC_COLD_JUNCTION_CH__2 |
    TC_SINGLE_ENDED |
    TC_OPEN_CKT_DETECT__YES |
    TC_OPEN_CKT_DETECT_CURRENT__10UA;
  assign_channel(CHIP_SELECT, 1, channel_assignment_data);
  // ----- Channel 2: Assign Off-Chip Diode -----
  channel_assignment_data =
    SENSOR_TYPE__OFF_CHIP_DIODE |
    DIODE_SINGLE_ENDED |
    DIODE_NUM_READINGS__2 |
    DIODE_AVERAGING_OFF |
    DIODE_CURRENT__20UA_80UA_160UA |
    (uint32_t) 0x100C49 << DIODE_IDEALITY_FACTOR_LSB;   // diode - ideality factor(eta): 1.00299930572509765625
  assign_channel(CHIP_SELECT, 2, channel_assignment_data);
  // ----- Channel 3: Assign Type K Thermocouple -----
  channel_assignment_data =
    SENSOR_TYPE__TYPE_K_THERMOCOUPLE |
    TC_COLD_JUNCTION_CH__9 |
    TC_SINGLE_ENDED |
    TC_OPEN_CKT_DETECT__YES |
    TC_OPEN_CKT_DETECT_CURRENT__10UA;
  assign_channel(CHIP_SELECT, 3, channel_assignment_data);
  // ----- Channel 4: Assign Off-Chip Diode -----
  channel_assignment_data =
    SENSOR_TYPE__OFF_CHIP_DIODE |
    DIODE_SINGLE_ENDED |
    DIODE_NUM_READINGS__2 |
    DIODE_AVERAGING_OFF |
    DIODE_CURRENT__20UA_80UA_160UA |
    (uint32_t) 0x100C49 << DIODE_IDEALITY_FACTOR_LSB;   // diode - ideality factor(eta): 1.00299930572509765625
  assign_channel(CHIP_SELECT, 4, channel_assignment_data);
  // ----- Channel 5: Assign Type J Thermocouple -----
  channel_assignment_data =
    SENSOR_TYPE__TYPE_J_THERMOCOUPLE |
    TC_COLD_JUNCTION_CH__NONE |
    TC_SINGLE_ENDED |
    TC_OPEN_CKT_DETECT__YES |
    TC_OPEN_CKT_DETECT_CURRENT__10UA;
  assign_channel(CHIP_SELECT, 5, channel_assignment_data);
  // ----- Channel 7: Assign Sense Resistor -----
  channel_assignment_data =
    SENSOR_TYPE__SENSE_RESISTOR |
    (uint32_t) 0x1F4000 << SENSE_RESISTOR_VALUE_LSB;    // sense resistor - value: 2000.
  assign_channel(CHIP_SELECT, 7, channel_assignment_data);
  // ----- Channel 9: Assign RTD PT-100 -----
  channel_assignment_data =
    SENSOR_TYPE__RTD_PT_100 |
    RTD_RSENSE_CHANNEL__7 |
    RTD_NUM_WIRES__4_WIRE |
    RTD_EXCITATION_MODE__NO_ROTATION_NO_SHARING |
    RTD_EXCITATION_CURRENT__100UA |
    RTD_STANDARD__EUROPEAN;
  assign_channel(CHIP_SELECT, 9, channel_assignment_data);
  // ----- Channel 10: Assign Type J Thermocouple -----
  channel_assignment_data =
    SENSOR_TYPE__TYPE_J_THERMOCOUPLE |
    TC_COLD_JUNCTION_CH__NONE |
    TC_SINGLE_ENDED |
    TC_OPEN_CKT_DETECT__YES |
    TC_OPEN_CKT_DETECT_CURRENT__10UA;
  assign_channel(CHIP_SELECT, 10, channel_assignment_data);

}




void configure_global_parameters()
{
  // -- Set global parameters
  transfer_byte(CHIP_SELECT, WRITE_TO_RAM, 0xF0, TEMP_UNIT__C |
                REJECTION__50_60_HZ);
  // -- Set any extra delay between conversions (in this case, 0*100us)
  transfer_byte(CHIP_SELECT, WRITE_TO_RAM, 0xFF, 0);
}

// -------------- Run the LTC2986 -------------------------------------

void loop()
{
  measure_channel(CHIP_SELECT, 1, TEMPERATURE);      // Ch 1: Type J Thermocouple
  measure_channel(CHIP_SELECT, 2, TEMPERATURE);      // Ch 2: Off-Chip Diode
  measure_channel(CHIP_SELECT, 3, TEMPERATURE);      // Ch 3: Type K Thermocouple
  measure_channel(CHIP_SELECT, 4, TEMPERATURE);      // Ch 4: Off-Chip Diode
  measure_channel(CHIP_SELECT, 5, TEMPERATURE);      // Ch 5: Type J Thermocouple
  measure_channel(CHIP_SELECT, 9, TEMPERATURE);      // Ch 9: RTD PT-100
  measure_channel(CHIP_SELECT, 10, TEMPERATURE);     // Ch 10: Type J Thermocouple
}