/*!
LTC2484: 24-Bit Delta Sigma ADC with Easy Drive Input Current Cancellation

@verbatim

The LTC2484 combines a 24-bit no latency delta-sigma analog-to-digital
converter with patented Easy Drive technology. The patented sampling scheme
eliminates dynamic input current errors and the shortcomings of on-chip
buffering through automatic cancellation of differential input current. This
allows large external source impedances and input signals with rail-to-rail
input range to be directly digitized while maintaining exceptional DC accuracy.

The LTC2484 includes an on-chip oscillator. The LTC2484 can be configured to
reject line frequencies. 50Hz, 60Hz or simultaneous 50Hz/60Hz line frequency
rejection can be selected as well as a 2x speed-up mode.

The LTC2484 allows a wide common mode input range (0V to VCC) independent of the
reference voltage. The reference can be as low as 100mV or can be tied directly
to VCC. The LTC2484 includes an on-chip trimmed oscillator, eliminating the need
for external crystals or oscillators. Absolute accuracy and low drift are
automatically maintained through continuous, transparent, offset and full-scale
calibration.

@endverbatim

http://www.linear.com/product/LTC2484

http://www.linear.com/product/LTC2484#demoboards


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

//! @ingroup Analog_to_Digital_Converters
//! @{
//! @defgroup LTC2484 LTC2484: 24-Bit Delta Sigma ADC with Easy Drive Input Current Cancellation
//! @}

/*! @file
    @ingroup LTC2484
    Library for LTC2484: 24-Bit Delta Sigma ADC with Easy Drive Input Current Cancellation
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC2484.h"
#include "LTC24XX_general.h"
#include <SPI.h>

// Checks for EOC with a specified timeout
int8_t LTC2484_EOC_timeout(uint8_t cs, uint16_t miso_timeout)
{

  return LTC24XX_EOC_timeout(cs, miso_timeout);
}

void LTC2484_read(uint8_t cs, uint8_t adc_command, int32_t *adc_code)
// Reads the LTC2484
{
  LTC24XX_SPI_8bit_command_32bit_data(cs, adc_command, adc_code);
}

float LTC2484_code_to_voltage(int32_t adc_code, float LTC2484_lsb, int32_t LTC2484_offset_code)
// Calculates the LTC2484 input bipolar voltage
{
  return(LTC24XX_diff_code_to_calibrated_voltage(adc_code,  LTC2484_lsb, LTC2484_offset_code));
}

float LTC2484_temperature(int32_t adc_code, float LTC2484_t0, float LTC2484_r0)
// Calculate the LTC2484 temperature.
{
  adc_code -= 0x20000000;                                                   // Converts offset binary to binary
  return (((((float) adc_code) / LTC2484_r0) * (LTC2484_t0 + 273)) - 273);  // Calculate temperature from ADC code, t0, r0.
}

void LTC2484_cal_voltage(int32_t zero_code, int32_t fs_code, float zero_voltage, float fs_voltage, float *LTC2484_lsb, int32_t *LTC2484_offset_code)
// Calibrate the lsb
{
  zero_code -= 0x20000000;                                              // Converts zero code from offset binary to binary
  fs_code -= 0x20000000;                                                // Converts full scale from offset binary to binary

  float temp_offset;
  *LTC2484_lsb = (fs_voltage-zero_voltage)/((float)(fs_code - zero_code));                              // Calculate the LSB

  temp_offset = (zero_voltage/ *LTC2484_lsb) - zero_code;                                               // Calculate Unipolar offset
  temp_offset = (temp_offset > (floor(temp_offset) + 0.5)) ? ceil(temp_offset) : floor(temp_offset);    // Round
  *LTC2484_offset_code = (int32_t)temp_offset;                                                          // Cast as int32_t
}

void LTC2484_cal_temperature(int32_t adc_code, float temperature,  float *LTC2484_t0, float *LTC2484_r0)
// Calibrate temperature
{
  adc_code -= 0x20000000;              // Converts offset binary to binary
  *LTC2484_r0 = (float) adc_code;      // Convert the adc_code to a float value
  *LTC2484_t0 = temperature;           // Store the calibration temperature
}
