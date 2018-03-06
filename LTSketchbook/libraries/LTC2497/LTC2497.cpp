/*!
LTC2497: 16-Bit, 16-Channel Delta Sigma ADCs with Easy Drive Input Current Cancellation.

@verbatim

The LTC2497 is a 16-channel (eight differential), 16-bit, No Latency Delta
Sigma TM ADC with Easy Drive technology and a 2-wire, I2C interface. The
patented sampling scheme eliminates dynamic input current errors and the
shortcomings of on-chip buffering through automatic cancellation of
differential input current. This allows large external source impedances
and rail-to-rail input signals to be directly digitized while maintaining
exceptional DC accuracy.

@endverbatim

http://www.linear.com/product/LTC2497

http://www.linear.com/product/LTC2497#demoboards


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
//! @defgroup LTC2497 LTC2497: 16-Bit, 16-Channel Delta Sigma ADCs with Easy Drive Input Current Cancellation
//! @}

/*! @file
    @ingroup LTC2497
    Library for LTC2497: 16-Bit, 16-Channel Delta Sigma ADCs with Easy Drive Input Current Cancellation
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LTC2497.h"
#include "LTC24XX_general.h"
#include "USE_WIRE.h"

#ifdef USEWIRE
#include "LT_I2C_Wire.h"
#else
#include "LT_I2C.h"
#endif

// Reads from LTC2497.
uint8_t LTC2497_read(uint8_t i2c_address, uint8_t adc_command, int32_t *adc_code, uint16_t timeout)
{
  return (LTC24XX_I2C_8bit_command_24bit_data(i2c_address, adc_command, adc_code, timeout));
}

// Calculates the voltage corresponding to an ADC code, given the reference (in volts)
float LTC2497_code_to_voltage(int32_t adc_code, float vref)
{
  return (LTC24XX_diff_code_to_voltage(adc_code, vref));
}
