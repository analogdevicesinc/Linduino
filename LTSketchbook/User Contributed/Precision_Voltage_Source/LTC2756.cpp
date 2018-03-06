/*!
    LTC2756: Serial 18-Bit SoftSpan IOUT DAC

@verbatim

The LTC2756 is an 18-bit multiplying serial-input,
current-output digital-to-analog converter. LTC2756A
provides full 18-bit performance-INL and DNL of +/-1LSB
maximum-over temperature without any adjustments. 18-bit
monotonicity is guaranteed in all performance grades. This
SoftSpan(TM) DAC operates from a single 3V to 5V supply
and offers six output ranges (up to +/-10V) that can be
programmed through the 3-wire SPI serial interface or
pin-strapped for operation in a single range.

@endverbatim

http://www.linear.com/product/LTC2756

http://www.linear.com/product/LTC2756#demoboards

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

//! @ingroup User_Contributed
//! @{
//! @defgroup LTC2756 LTC2756: Serial 18-Bit SoftSpan IOUT DAC
//! @}

/*! @file
    @ingroup LTC2756 UserContributed
    Library for LTC2756 Serial 18-Bit SoftSpan IOUT DAC
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC2756.h"
#include <SPI.h>

// Transfer 4 bytes
void LTC2756_transfer_4bytes(uint8_t cs, uint8_t *tx, uint8_t *rx)
{
  spi_transfer_block(cs, tx, rx, (uint8_t)4);
}

// Convert voltage to code
// Returns the calculated code
uint32_t LTC2756_voltage_to_code(float dac_voltage, float LTC2756_lsb, int32_t LTC2756_offset)
{
  float float_code;
  int32_t dac_code;

  float_code = dac_voltage / LTC2756_lsb;                                                         // Calculate the DAC code
  float_code = (float_code > (floor(float_code) + 0.5)) ? ceil(float_code) : floor(float_code);   // Round
  dac_code = (int32_t)float_code - LTC2756_offset;                                                // Subtract offset
  if (dac_code < 0)
    dac_code = 0;
  if (dac_code > 262143)
    dac_code = 262143;
  return ((uint32_t)dac_code);
}

// Calculate the LTC2756 offset and LSB voltage given two measured voltages and their corresponding codes
void LTC2756_calibrate(uint32_t zero_code, uint32_t fs_code, float zero_voltage, float fs_voltage, float *LTC2756_lsb, int32_t *LTC2756_offset)
{
  float temp_offset;
  *LTC2756_lsb = (fs_voltage - zero_voltage) / ((float) (fs_code - zero_code));                       // 1) Calculate the LSB
  temp_offset = zero_voltage/(*LTC2756_lsb) - (float)zero_code;                                       // 2) Calculate the offset
  temp_offset = (temp_offset > (floor(temp_offset) + 0.5)) ? ceil(temp_offset) : floor(temp_offset);  // 3) Round offset
  *LTC2756_offset = (int32_t)temp_offset;                                                             // 4) Cast as int16_t
}

