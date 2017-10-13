/*!
LTC2482: 16-Bit Delta-Sigma ADC with Easy Drive Input Current Cancellation.

@verbatim

The LTC2480 is a 16-Bit Delta-Sigma ADC with Easy Drive Input Current Cancellation.
The LTC2482 allows a wide common mode input range (0V to VCC) independent of the
reference voltage. The reference can be as low as 100mV or can be tied directly to
VCC. The noise level is 600nV RMS independent of VREF . This allows direct
digitization of low level signals with 16-bit accuracy. The LTC2482 includes an on-chip
trimmed oscillator, eliminating the need for external crystals or oscillators and
provides 87dB rejection of 50Hz and 60Hz line frequency noise. Absolute accuracy and low
drift are automatically maintained through continuous, transparent, offset and
full-scale calibration.

@endverbatim

http://www.linear.com/product/LTC2482

http://www.linear.com/product/LTC2482#demoboards

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
//! @defgroup LTC2482 LTC2482: 16-Bit Delta-Sigma ADC with Easy Drive Input Current Cancellation.
//! @}

/*! @file
    @ingroup LTC2482
    Library for LTC2482: 16-Bit Delta-Sigma ADC with Easy Drive Input Current Cancellation.
*/

#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"

// Reads the LTC2482 and returns 24-bit data
void LTC2482_read(uint8_t cs, uint32_t *ptr_adc_code)
{
  LT_union_int32_4bytes data, command;  // LTC24XX data and command
  command.LT_uint32 = 0;
  data.LT_uint32 = 0;

  spi_transfer_block(cs, command.LT_byte, data.LT_byte, (uint8_t)3);

  *ptr_adc_code = data.LT_uint32;
}

// Calculates the LTC2482 input voltage given the binary data, reference voltage and input gain.
float LTC2482_code_to_voltage(uint32_t adc_code, float vref)
{
  float voltage;

  voltage = (float)adc_code;
  voltage = voltage / (pow(2,16)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
  voltage = voltage * vref;             //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)

  return(voltage);
}