
/*!
LTC2460 Ultra-Tiny, 16-bit delta sigma ADCs with 10ppm/degree C Max Precision Reference

@verbatim

The LTC2460/LTC2462 are ultra tiny, 16-Bit analog-to- digital converters
with an integrated precision reference. They use a single 2.7V to 5.5V supply
and communicate through an SPI Interface. The LTC2460 is single-ended with
a 0V to VREF input range and the LTC2462 is differential with a ±VREF input
range. Both ADC’s include a 1.25V integrated reference with 2ppm/°C drift
performance and 0.1% initial accuracy. The converters are available in a
12-pin DFN 3mm × 3mm package or an MSOP-12 package. They include an integrated
oscillator and perform conversions with no latency for multiplexed applications.
The LTC2460/LTC2462 include a proprietary input sampling scheme that reduces
the average input current several orders of magnitude when compared to conventional
delta sigma converters.

@endverbatim

http://www.linear.com/product/LTC2460

http://www.linear.com/product/LTC2460#demoboards


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
//! @defgroup LTC2460 LTC2460: 24-Bit, 16-Channel Delta Sigma ADCs with Selectable Speed/Resolution
//! @}

/*! @file
    @ingroup LTC2460
    Library for LTC2460: 24-Bit, 16-Channel Delta Sigma ADCs with Selectable Speed/Resolution
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LTC2460.h"
#include "LTC24XX_general.h"

// Reads from LTC2460.
void LTC2460_read(uint8_t cs, uint8_t adc_command, int32_t *adc_code)
{
  int32_t data;
  LTC24XX_SPI_8bit_command_24bit_data(cs, adc_command, &data);    // Transfer arrays
  *adc_code = data;

}

// Calculates the voltage corresponding to an adc code, given the reference (in volts)
float LTC2460_code_to_voltage(int32_t adc_code, float vref)
{
  float adc_voltage;
  adc_voltage = vref * (((adc_code>>16) & 0x0000FFFF )/65535.0); //This part does not have an EOC or a sign bit.
  return(adc_voltage);
}

