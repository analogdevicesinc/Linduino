/*!
LTC1859: 16-Bit 8-Channel 200ksps ADC

@verbatim

The LTC1863/LTC1859 are pin-compatible, 8-channel 12-/16-bit A/D converters with
serial I/O, and an internal reference. The ADCs typically draw only 1.3mA from a
single 5V supply. The 8-channel input multiplexer can be configured for either
single-ended or differential inputs and unipolar or bipolar conversions (or
combinations thereof). The automatic nap and sleep modes benefit power sensitive
applications.

The LTC1859's DC performance is outstanding with a +/-2LSB INL specification and
no missing codes over temperature. The signal-to-noise ratio (SNR) for the
LTC1859 is typically 89dB, with the internal reference.

@endverbatim

http://www.linear.com/product/LTC1859

http://www.linear.com/product/LTC1859#demoboards

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
//! @defgroup LTC1859 LTC1859: 16-Bit 8-Channel 200ksps ADC
//! @}

/*! @file
    @ingroup LTC1859
    Library for LTC1859: 16-Bit 8-Channel 200ksps ADC
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC1859.h"
#include <SPI.h>


// Builds the ADC command
uint8_t LTC1859_build_command(uint8_t ch_designate, uint8_t uni_bipolar, uint8_t range_low_high)
{
  uint8_t adc_command;
  adc_command = ch_designate | uni_bipolar | range_low_high;
  return (adc_command);
}


// Reads the ADC  and returns 16-bit data
void LTC1859_read(uint8_t cs, uint8_t adc_command, uint16_t *adc_code)
{
  spi_transfer_word(cs, (uint16_t)(adc_command<<8), adc_code);
}


// Calculates the LTC1859 input voltage given the data, vref, bits, and unipolar/bipolar status.
float LTC1859_code_to_voltage(uint16_t adc_code, float vref, uint8_t range_low_high, uint8_t uni_bipolar)
{
  float voltage;
  float sign = 1;

  if (range_low_high == LTC1859_LOW_RANGE_MODE)
    vref = vref/2;

  if (uni_bipolar == LTC1859_UNIPOLAR_MODE)
  {
    voltage = (float)adc_code;
    voltage = voltage / (pow(2,16)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
  }
  else
  {
    if ((adc_code & 0x8000) == 0x8000)  //adc code is < 0
    {
      adc_code = (adc_code ^ 0xFFFF)+1;                                    //! Convert ADC code from two's complement to binary
      sign = -1;
    }
    voltage = sign*(float)adc_code;
    voltage = voltage / (pow(2,15)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
  }

  voltage = voltage * vref;           //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)

  return(voltage);
}
