/*!
LTC2668: 16-Channel SPI 16-/12-Bit +/-10V Vout SoftSpan DACs with 10ppm/C Max Reference

@verbatim

The LTC2668 is a family of 16-channel, 12/16-bit +/-10V digital-to-analog converters with integrated
precision references. They are guaranteed monotonic and have built-in rail-to-rail output buffers.
These SoftSpan DACs offer five output ranges up to +/-10V. The range of each channel is independently
programmable, or the part can be hardware-configured for operation in a fixed range.

@endverbatim

http://www.linear.com/product/LTC2668

http://www.linear.com/product/LTC2668#demoboards

REVISION HISTORY
$Revision: 6237 $
$Date: 2016-12-20 15:09:16 -0800 (Tue, 20 Dec 2016) $

Copyright (c) 2013, Linear Technology Corp.(LTC)
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
//! @defgroup LTC2668 LTC2668 16-Channel SPI 16-/12-Bit +/-10V Vout SoftSpan DACs with 10ppm/C Max Reference
//! @}

/*! @file
    @ingroup LTC2668
    LTC2668 16-Channel SPI 16-/12-Bit +/-10V Vout SoftSpan DACs with 10ppm/C Max Reference
*/

#include <stdint.h>
#include <math.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC2668.h"
#include <SPI.h>

int8_t LTC2668_write(uint8_t cs, uint8_t dac_command, uint8_t dac_address, uint16_t dac_code)
// Write the 16-bit dac_code to the LTC2668
{
  static uint8_t last_data_array[4];
  uint8_t data_array[4], rx_array[4];
  int8_t ret;
  LT_union_int16_2bytes data;

  data.LT_int16 = dac_code;                              // Copy DAC code to union
  data_array[3] = 0;                                     // Only required for 32 byte readback transaction
  data_array[2] = dac_command | dac_address;             // Build command / address byte
  data_array[1] = data.LT_byte[1];                       // MS Byte
  data_array[0] = data.LT_byte[0];                       // LS Byte

  spi_transfer_block(cs, data_array, rx_array, (uint8_t) 4);

  // Compare data read back to data that was sent the previous time this function was called
  if ((rx_array[2] == last_data_array[2]) && (rx_array[1] == last_data_array[1]) && (rx_array[0] == last_data_array[0]))
  {
    ret = 0;
  }
  else
  {
    ret = 1;
  }

  last_data_array[0] = data_array[0]; // Copy data array to a static array to compare
  last_data_array[1] = data_array[1]; // the next time the function is called
  last_data_array[2] = data_array[2];

  return(ret);
}

uint16_t LTC2668_voltage_to_code(float dac_voltage, float min_output, float max_output)
// Calculate a LTC2668 DAC code given the desired output voltage and the minimum / maximum
// outputs for a given softspan range.
{
  uint16_t dac_code;
  float float_code;
  float_code = 65535.0 * (dac_voltage - min_output) / (max_output - min_output);                    // Calculate the DAC code
  float_code = (float_code > (floor(float_code) + 0.5)) ? ceil(float_code) : floor(float_code);     // Round
  if (float_code < 0.0) float_code = 0.0;
  if (float_code > 65535.0) float_code = 65535.0;
  dac_code = (uint16_t) (float_code);                                                               // Convert to unsigned integer
  return (dac_code);
}

float LTC2668_code_to_voltage(uint16_t dac_code, float min_output, float max_output)
// Calculate the LTC2668 DAC output voltage given the DAC code and and the minimum / maximum
// outputs for a given softspan range.
{
  float dac_voltage;
  dac_voltage = (((float) dac_code / 65535.0) * (max_output - min_output)) + min_output;            // Calculate the dac_voltage
  return (dac_voltage);
}