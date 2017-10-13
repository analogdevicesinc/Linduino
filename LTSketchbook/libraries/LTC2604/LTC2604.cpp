/*!
LTC2604: Quad 16-Bit Rail-to-Rail DACs in 16-Lead SSOP

@verbatim

The LTC2604/LTC2614/LTC2624 are quad 16-,14- and 12-bit 2.5V to 5.5V
rail-to-rail voltage output DACs in 16-lead narrow SSOP packages. These
parts have separate reference inputs for each DAC. They have built-in
high performance output buffers and are guaranteed monotonic.

These parts establish advanced performance standards for output drive,
crosstalk and load regulation in single-supply, voltage output multiples.

@endverbatim


http://www.linear.com/product/LTC2604

http://www.linear.com/product/LTC2604#demoboards

REVISION HISTORY
$Revision: 6237 $
$Date: 2016-12-20 15:09:16 -0800 (Tue, 20 Dec 2016) $

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

//! @ingroup Digital_to_Analog_Converters
//! @{
//! @defgroup LTC2604 LTC2604: Quad 16-Bit Rail-to-Rail DACs in 16-Lead SSOP
//! @}

/*! @file
  @ingroup LTC2604
  Library for LTC2604: Quad 16-Bit Rail-to-Rail DACs in 16-Lead SSOP
*/

#include <stdint.h>
#include <math.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC2604.h"
#include <SPI.h>

// Write the 16-bit dac_code to the LTC2604
void LTC2604_write(uint8_t cs, uint8_t dac_command, uint8_t dac_address, uint16_t dac_code)
{
  uint8_t data_array[3], rx_array[3];
  LT_union_int16_2bytes data;

  data.LT_uint16 = dac_code;

  data_array[2] = dac_command | dac_address;
  data_array[1] = data.LT_byte[1];
  data_array[0] =data.LT_byte[0];

  spi_transfer_block(cs, data_array, rx_array, (uint8_t) 3);
}

// Calculate a LTC2604 DAC code given the desired output voltage and DAC address (0-3)
uint16_t LTC2604_voltage_to_code(float dac_voltage, float LTC2604_lsb, int16_t LTC2604_offset)
{
  int32_t dac_code;
  float float_code;
  float_code = dac_voltage / LTC2604_lsb;                                                             //! 1) Calculate the DAC code
  float_code = (float_code > (floor(float_code) + 0.5)) ? ceil(float_code) : floor(float_code);       //! 2) Round
  dac_code = (int32_t)float_code - LTC2604_offset;                                                    //! 3) Subtract offset
  if (dac_code < 0)                                                                                   //! 4) If DAC code < 0, Then DAC code = 0
    dac_code = 0;
  return ((uint16_t)dac_code);                                                                        //! 5) Cast DAC code as uint16_t
}

// Calculate the LTC2604 DAC output voltage given the DAC code and DAC address (0-3)
float LTC2604_code_to_voltage(uint16_t dac_code, float LTC2604_lsb, int16_t LTC2604_offset)
{
  float dac_voltage;
  dac_voltage = ((float)(dac_code + LTC2604_offset)* LTC2604_lsb);                                    //! 1) Calculates the dac_voltage
  return (dac_voltage);
}