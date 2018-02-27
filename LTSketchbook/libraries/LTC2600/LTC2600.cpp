/*!
LTC2600: Octal 16-Bit Rail-to-Rail DACs in 16-Lead SSOP

@verbatim

The LTC2600/LTC2610/LTC2620 are octal 16-, 14- and 12-bit, 2.5V-to-5.5V
rail-to-rail voltage-output DACs in 16-lead narrow SSOP and 20-lead 4mm × 5mm
QFN packages. They have built-in high performance output buffers and are
guaranteed monotonic.

These parts establish advanced performance standards for output drive,
crosstalk and load regulation in single-supply, voltage output multiples.

@endverbatim


http://www.linear.com/product/LTC2600

http://www.linear.com/product/LTC2600#demoboards


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

//! @ingroup Digital_to_Analog_Converters
//! @{
//! @defgroup LTC2600 LTC2600: Octal 16-Bit Rail-to-Rail DACs in 16-Lead SSOP
//! @}

/*! @file
  @ingroup LTC2600
  Library for LTC2600: Octal 16-Bit Rail-to-Rail DACs in 16-Lead SSOP
*/

#include <stdint.h>
#include <math.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC2600.h"
#include <SPI.h>

// Write the 16-bit dac_code to the LTC2600
void LTC2600_write(uint8_t cs, uint8_t dac_command, uint8_t dac_address, uint16_t dac_code)
{
  uint8_t data_array[3], rx_array[3];
  LT_union_int16_2bytes data;

  data.LT_uint16 = dac_code;

  data_array[2] = dac_command | dac_address;
  data_array[1] = data.LT_byte[1];
  data_array[0] =data.LT_byte[0];

  spi_transfer_block(cs, data_array, rx_array, (uint8_t) 3);
}

// Calculate a LTC2600 DAC code given the desired output voltage and DAC address (0-3)
uint16_t LTC2600_voltage_to_code(float dac_voltage, float LTC2600_lsb, int16_t LTC2600_offset)
{
  int32_t dac_code;
  float float_code;
  float_code = dac_voltage / LTC2600_lsb;                                                             //! 1) Calculate the DAC code
  float_code = (float_code > (floor(float_code) + 0.5)) ? ceil(float_code) : floor(float_code);       //! 2) Round
  dac_code = (int32_t)float_code - LTC2600_offset;                                                    //! 3) Subtract offset
  if (dac_code < 0)                                                                                   //! 4) If DAC code < 0, Then DAC code = 0
    dac_code = 0;
  return ((uint16_t)dac_code);                                                                        //! 5) Cast DAC code as uint16_t
}

// Calculate the LTC2600 DAC output voltage given the DAC code and DAC address (0-3)
float LTC2600_code_to_voltage(uint16_t dac_code, float LTC2600_lsb, int16_t LTC2600_offset)
{
  float dac_voltage;
  dac_voltage = ((float)(dac_code + LTC2600_offset)* LTC2600_lsb);                                    //! 1) Calculates the dac_voltage
  return (dac_voltage);
}