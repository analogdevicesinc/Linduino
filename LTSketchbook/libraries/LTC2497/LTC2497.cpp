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
//! @defgroup LTC2497 LTC2497: 16-Bit, 16-Channel Delta Sigma ADCs with Easy Drive Input Current Cancellation
//! @}

/*! @file
    @ingroup LTC2497
    Library for LTC2497: 16-Bit, 16-Channel Delta Sigma ADCs with Easy Drive Input Current Cancellation
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LTC2497.h"
#include "LTC24XX_general.h"

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
