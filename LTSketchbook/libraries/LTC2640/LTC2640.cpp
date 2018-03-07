/*!
LTC2640: Single 12-/10-/8-Bit Rail-to-Rail DACs with 10ppm/C Reference.
LTC2630: Single 12-/10-/8-Bit Rail-to-Rail DACs with Integrated Reference in SC70.

@verbatim

The LTC2640 is a family of 12-, 10-, and 8-bit voltage-output DACs with an
integrated, high-accuracy, low-drift reference in an 8-lead TSOT-23 package. It
has a rail-to-rail output buffer that is guaranteed monotonic.

The LTC2640-L has a full-scale output of 2.5V, and operates from a single 2.7V
to 5.5V supply. The LTC2640-H has a full-scale output of 4.096V, and operates
from a 4.5V to 5.5V supply. A 10 ppm/C reference output is available at the REF
pin.

Each DAC can also operate in External Reference mode, in which a voltage
supplied to the REF pin sets the full- scale output. The LTC2640 DACs use a
SPI/MICROWIRE compatible 3-wire serial interface which operates at clock rates
up to 50 MHz.

The LTC2640 incorporates a power-on reset circuit. Options are available for
Reset to Zero Scale or Reset to Midscale after power-up.

@endverbatim

http://www.linear.com/product/LTC2640
http://www.linear.com/product/LTC2630

http://www.linear.com/product/LTC2640#demoboards
http://www.linear.com/product/LTC2630#demoboards


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
//! @defgroup LTC2640 LTC2640: Single 12-/10-/8-Bit Rail-to-Rail DACs with 10ppm/C Reference
//! @}

/*! @file
    @ingroup LTC2640
    Library for LTC2640: Single 12-/10-/8-Bit Rail-to-Rail DACs with 10ppm/C Reference
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include <math.h>
#include "LT_SPI.h"
#include "LTC2640.h"
#include <SPI.h>

// Write the dac_command and 16-bit dac_code to the LTC2640. The dac_code
// must be left aligned before calling this function.
void LTC2640_write(uint8_t cs, uint8_t dac_command, uint16_t dac_code)
{
  LT_union_int16_2bytes data;
  data.LT_uint16 = dac_code;

  uint8_t data_array[3], rx_array[3];
  data_array[2] = dac_command;
  data_array[1] = data.LT_byte[1];
  data_array[0] = data.LT_byte[0];

  spi_transfer_block(cs, data_array, rx_array, 3);
}

// Calculate the LTC2640 DAC code given the desired output voltage
uint16_t LTC2640_voltage_to_code(float dac_voltage, float LTC2640_lsb, int16_t LTC2640_offset)
{
  int32_t dac_code;
  dac_code = dac_voltage / LTC2640_lsb;                                                               //! 1) Calculate DAC code as float from  lsb, offset and DAC voltage
  dac_code = (dac_code > (floor(dac_code) + 0.5)) ? ceil(dac_code) : floor(dac_code);                 //! 2) Round
  dac_code = dac_code - LTC2640_offset;                                                               //! 3) Subtract offset
  if (dac_code < 0)                                                                                   //! 4) If DAC code < 0, Then DAC code = 0
    dac_code = 0;
  if (dac_code > 4095)
    dac_code = 4095;
  return ((uint16_t)dac_code);                                                                        //! 5) Cast DAC code as uint16_t
}

// Calculate the LTC2640 DAC output voltage given the DAC code
float LTC2640_code_to_voltage(uint16_t dac_code, float LTC2640_lsb, int16_t LTC2640_offset)
{
  float dac_voltage;
  dac_voltage = ((float)(dac_code + LTC2640_offset)* LTC2640_lsb);                                    //! 1) Calculate voltage from DAC code, lsb, and offset
  return (dac_voltage);
}

// Calculate the LTC2640 offset and LSB voltages given two measured voltages and their corresponding codes
void LTC2640_calibrate(uint16_t dac_code1, uint16_t dac_code2, float voltage1, float voltage2, float *LTC2640_lsb, int16_t *LTC2640_offset)
{
  int16_t temp_offset;
  *LTC2640_lsb = (voltage2 - voltage1) / ((float) (dac_code2 - dac_code1));                           //! 1) Calculate LSB
  temp_offset = voltage1/(*LTC2640_lsb) - dac_code1;                                                  //! 2) Calculate the offset
  temp_offset = (temp_offset > (floor(temp_offset) + 0.5)) ? ceil(temp_offset) : floor(temp_offset);  //! 3) Round offset
  *LTC2640_offset = (int16_t)temp_offset;                                                             //! 4) Cast as int16_t
}
