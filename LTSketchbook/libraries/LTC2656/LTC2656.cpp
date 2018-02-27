/*!
LTC2656: Octal SPI 16-/12-Bit Rail-to-Rail DACs with 10ppm/C Max Reference

@verbatim

The LTC2656 is a family of octal 16-/12-bit rail-to-rail DACs with a precision
integrated reference. The DACs have built-in high performance, rail-to-rail,
output buffers and are guaranteed monotonic. The LTC2656-L has a full-scale
output of 2.5V with the integrated 10ppm/C reference and operates from a single
2.7V to 5.5V supply. The LTC2656-H has a full-scale output of 4.096V with the
integrated reference and operates from a 4.5V to 5.5V supply. Each DAC can also
operate with an external reference, which sets the DAC full-scale output to two
times the external reference voltage.

These DACs communicate via a SPI/MICROWIRE compatible 4-wire serial interface
which operates at clock rates up to 50MHz. The LTC2656 incorporates a power-on
reset circuit that is controlled by the PORSEL pin. If PORSEL is tied to GND the
DACs reset to zero-scale. If PORSEL is tied to VCC, the DACs reset to mid-scale.

@endverbatim

http://www.linear.com/product/LTC2656

http://www.linear.com/product/LTC2656#demoboards


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
//! @defgroup LTC2656 LTC2656: Octal SPI 16-/12-Bit Rail-to-Rail DACs with 10ppm/C Max Reference
//! @}

/*! @file
    @ingroup LTC2656
    Library for LTC2656 Octal SPI 16-/12-Bit Rail-to-Rail DACs with 10ppm/C Max Reference
*/

#include <stdint.h>
#include <math.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC2656.h"
#include <SPI.h>

void LTC2656_write(uint8_t cs, uint8_t dac_command, uint8_t dac_address, uint16_t dac_code)
// Write the 16-bit dac_code to the LTC2656
{
  uint8_t data_array[3], rx_array[3];
  LT_union_int16_2bytes data;

  data.LT_uint16 = dac_code;

  data_array[2] = dac_command | dac_address;
  data_array[1] = data.LT_byte[1];
  data_array[0] =data.LT_byte[0];

  spi_transfer_block(cs, data_array, rx_array, (uint8_t) 3);
}

uint16_t LTC2656_voltage_to_code(float dac_voltage, float LTC2656_lsb, int16_t LTC2656_offset)
// Calculate a LTC2656 DAC code given the desired output voltage and DAC address (0-7)
{
  int32_t dac_code;
  float float_code;
  float_code = dac_voltage / LTC2656_lsb;                                                             //! 1) Calculate the DAC code
  float_code = (float_code > (floor(float_code) + 0.5)) ? ceil(float_code) : floor(float_code);       //! 2) Round
  dac_code =(int32_t)float_code - LTC2656_offset;                                                     //! 3) Subtract offset
  if (dac_code < 0)                                                                                   //! 4) If DAC code < 0, Then DAC code = 0
    dac_code = 0;
  return ((uint16_t)dac_code);                                                                        //! 5) Cast DAC code as uint16_t
}

float LTC2656_code_to_voltage(uint16_t dac_code, float LTC2656_lsb, int16_t LTC2656_offset)
// Calculate the LTC2656 DAC output voltage given the DAC code and DAC address (0-7)
{
  float dac_voltage;
  dac_voltage = ((float)(dac_code + LTC2656_offset)* LTC2656_lsb);                                    //! 1) Calculates the dac_voltage
  return (dac_voltage);
}

void LTC2656_calibrate(uint16_t dac_code1, uint16_t dac_code2, float voltage1, float voltage2, float *LTC2656_lsb, int16_t *LTC2656_offset)
// Calculate the LTC2656 offset and LSB voltage given two measured voltages and their corresponding codes
{
  float temp_offset;
  *LTC2656_lsb = (voltage2 - voltage1) / ((float) (dac_code2 - dac_code1));                           //! 1) Calculate the LSB
  temp_offset = voltage1/(*LTC2656_lsb) - (float)dac_code1;                                           //! 2) Calculate the offset
  temp_offset = (temp_offset > (floor(temp_offset) + 0.5)) ? ceil(temp_offset) : floor(temp_offset);  //! 3) Round offset
  *LTC2656_offset = (int16_t)temp_offset;                                                             //! 4) Cast as int16_t
}
