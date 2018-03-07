/*!
LTC2440: 24-Bit, Differential Delta Sigma ADCs with Selectable Speed/Resolution.

@verbatim

The LTC2440 is a high speed 24-bit No Latency Delta Sigma TM ADC with 5ppm
INL and 5uV offset. It uses proprietary delta-sigma architecture enabling
variable speed and resolution with no latency. Ten speed/resolution
combinations (6.9Hz/200nVRMS to 3.5kHz/25uVRMS) are programmed through a
simple serial interface. Alternatively, by tying a single pin HIGH or
LOW, a fast (880Hz/2uVRMS) or ultralow noise (6.9Hz, 200nVRMS, 50/60Hz
rejection) speed/resolution combination can be easily selected. The
accuracy (offset, full-scale, linearity, drift) and power dissipation are
independent of the speed selected. Since there is no latency, a
speed/resolution change may be made between conversions with no
degradation in performance.

@endverbatim

http://www.linear.com/product/LTC2440

http://www.linear.com/product/LTC2440#demoboards


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
//! @defgroup LTC2440 LTC2440: 24-Bit, Differential Delta Sigma ADCs with Selectable Speed/Resolution
//! @}

/*! @file
    @ingroup LTC2440
    Library for LTC2440: 24-Bit, Differential Delta Sigma ADCs with Selectable Speed/Resolution
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "LTC2440.h"
#include "LTC24XX_general.h"


// Checks for EOC with a specified timeout.
// Returns 0=successful, 1=unsuccessful (exceeded timeout)
int8_t LTC2440_EOC_timeout(uint8_t cs, uint16_t miso_timeout)
{
  return(LTC24XX_EOC_timeout(cs, miso_timeout));
}

// Reads from LTC2440.
void LTC2440_read(uint8_t cs, uint8_t adc_command, int32_t *adc_code)
{
  LTC24XX_SPI_8bit_command_32bit_data(cs, adc_command, adc_code);    // Transfer data
}

// Calculates the voltage corresponding to an ADC code, given the reference (in volts)
float LTC2440_code_to_voltage(int32_t adc_code, float vref)
{
  return(LTC24XX_diff_code_to_voltage(adc_code, vref));
}
