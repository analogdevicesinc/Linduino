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
