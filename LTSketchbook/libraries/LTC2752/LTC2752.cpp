/*!
LTC2752: Dual Serial 16-Bit SoftSpan IOUT DAC

@verbatim

The LTC2752 is a dual, current-output, serial-input precision multiplying DAC
with selectable output ranges. Ranges can either be programmed in software for
maximum flexibility (each of the DACs can be programmed to any one of six output
ranges) or hardwired through pin-strapping. Two unipolar ranges are available
(0V to 5V and 0V to 10V), and four bipolar ranges (±2.5V, ±5V, ±10V and –2.5V
to 7.5V). These ranges are obtained when an external precision 5V reference is
used. The output ranges for other reference voltages are easy to calculate by
observing that each range is a multiple of the external reference voltage. The
ranges can then be expressed: 0 to 1×, 0 to 2×, ±0.5×, ±1×, ±2×, and –0.5× to 1.5×.

@endverbatim

http://www.linear.com/product/LTC2752

http://www.linear.com/product/LTC2752#demoboards


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
//! @defgroup LTC2752 LTC2752: Dual Serial 16-Bit SoftSpan IOUT DAC
//! @}

/*! @file
    @ingroup LTC2752
    Library for LTC2752 Dual Serial 16-Bit SoftSpan IOUT DAC
*/

#include <Arduino.h>
#include <stdint.h>
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "Linduino.h"
#include <SPI.h>

// Transmits 24 bit input stream: 4-bit command + 4-bit don't-care + 16-bit data
void LTC2752_write(uint8_t cs, uint8_t dac_command, uint8_t dac_address, uint16_t data)
{
  output_low(cs);
  spi_write(dac_command|dac_address);
  spi_write((uint8_t)((data >> 8) & 0xFF)); // D15:D8
  spi_write((uint8_t)(data & 0xFF));      // D7:D0
  output_high(cs);
}
// Calculate the LTC2752 DAC output voltage given the DAC code and and the minimum / maximum
// outputs for a given softspan range.
float LTC2752_code_to_voltage(uint16_t dac_code, float min_output, float max_output)
{
  float dac_voltage;
  dac_voltage = (((float) dac_code / 65535.0) * (max_output - min_output)) + min_output;            // Calculate the dac_voltage
  return (dac_voltage);
}

// Calculate a LTC2752 DAC code given the desired output voltage and the minimum / maximum
// outputs for a given softspan range.
uint16_t LTC2752_voltage_to_code(float dac_voltage, float min_output, float max_output)
{
  uint16_t dac_code;
  float float_code;
  float_code = 65535.0 * (dac_voltage - min_output) / (max_output - min_output);                    // Calculate the DAC code
  float_code = (float_code > (floor(float_code) + 0.5)) ? ceil(float_code) : floor(float_code);     // Round
  if (float_code < 0.0) float_code = 0.0;
  if (float_code > 65535.0) float_code = 65535.0;
  dac_code = (uint32_t) (float_code);                                                               // Convert to unsigned integer
  return (dac_code);
}