/*!
LTC2496: 16-Bit 8-/16-Channel Delta Sigma ADC with Easy Drive Input Current Cancellation

@verbatim

The LTC2496 is a 16-channel (8-differential) 16-bit No Latency ΔΣ ADC with Easy
Drive technology. The patented sampling scheme eliminates dynamic input current
errors and the shortcomings of on-chip buffering through automatic cancellation
of differential input current. This allows large external source impedances,
and rail-to-rail input signals to be directly digitized while maintaining
exceptional DC accuracy.

The LTC2496 includes an integrated oscillator. This device can be configured to
measure an external signal (from combinations of 16 analog input channels
operating in single ended or differential modes). It automatically rejects
line frequencies of 50Hz and 60Hz, simultaneously.

@endverbatim

http://www.linear.com/product/LTC2496

http://www.linear.com/product/LTC2496#demoboards

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

//! @ingroup Analog_to_Digital_Converters
//! @{
//! @defgroup LTC2496 LTC2496: 16-Bit 8-/16-Channel Delta Sigma ADC with Easy Drive Input Current Cancellation
//! @}

/*! @file
  @ingroup LTC2496
  Library for LTC2496 LTC2496: 16-Bit 8-/16-Channel Delta Sigma ADC with Easy Drive Input Current Cancellation
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC2496.h"
#include "LTC24XX_general.h"
#include <SPI.h>

// Checks for EOC with a specified timeout
int8_t LTC2496_EOC_timeout(uint8_t cs, uint16_t miso_timeout)
{

  return LTC24XX_EOC_timeout(cs,miso_timeout);
}

// reads the LTC2496
void LTC2496_read(uint8_t cs, uint8_t adc_command, int32_t *adc_code)
{

  LTC24XX_SPI_8bit_command_24bit_data(cs, adc_command, adc_code);
}


float LTC2496_code_to_voltage(int32_t adc_code, float vref)
{

  return(LTC24XX_diff_code_to_voltage(adc_code,vref));
}