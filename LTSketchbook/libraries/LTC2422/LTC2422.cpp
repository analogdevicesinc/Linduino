/*!
LTC2422: 1-/2-Channel 20-Bit uPower No Latency Delta-Sigma ADC in MSOP-10

@verbatim

The LTC2421/LTC2422 are 1- and 2-channel 2.7V to 5.5V micropower 20-bit analog-
to-digital converters with an integrated oscillator, 8ppm INL and 1.2ppm RMS
noise. These ultrasmall devices use delta-sigma technology and a new digital
filter architecture that settles in a single cycle. This eliminates the latency
found in conventional delta-sigma converters and simplifies multiplexed
applications. Through a single pin, the LTC2421/LTC2422 can be configured for
better than 110dB rejection at 50Hz or 60Hz +/-2%, or can be driven by an
external oscillator for a user defined rejection frequency in the range 1Hz to
120Hz. The internal oscillator requires no external frequency setting
components.

@endverbatim

http://www.linear.com/product/LTC2422

http://www.linear.com/product/LTC2422#demoboard

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
//! @defgroup LTC2422 LTC2422: 1-/2-Channel 20-Bit uPower No Latency Delta-Sigma ADC in MSOP-10
//! @}

/*! @file
    @ingroup LTC2422
    Library for LLTC2422: 1-/2-Channel 20-Bit uPower No Latency Delta-Sigma ADC in MSOP-10
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "LTC2422.h"
#include <SPI.h>

uint8_t LTC2422_EOC_timeout(uint8_t cs, uint16_t miso_timeout)
// Checks for EOC with a specified timeout (ms)
{
  uint16_t timer_count = 0;             // Timer count for MISO
  output_low(cs);                       //! 1) Pull CS low
  while (1)                             //! 2) Wait for SDO (MISO) to go low
  {
    if (input(MISO) == 0) break;        //! 3) If SDO is low, break loop
    if (timer_count++>miso_timeout)     // If timeout, return 1 (failure)
    {
      output_high(cs);                  // Pull CS high
      return(1);
    }
    else
      delay(1);
  }
  output_high(cs);                      // Pull CS high
  return(0);
}

// Read ADC code from the LTC2422. Does not wait for end-of-conversion.
// To automatically wait for conversion to complete, use the LTC2422_EOC_timeout before this function..
void LTC2422_adc_read(uint8_t cs, uint8_t *adc_channel, int32_t *code)
{
  LT_union_int32_4bytes data, command;      // LTC2422 data

  command.LT_int32 = 0x00000000;

  spi_transfer_block(LTC2422_CS, command.LT_byte , data.LT_byte, (uint8_t)3);
  if (data.LT_byte[2] & 0x40) // Obtains Channel Number
  {
    *adc_channel = 1;
  }
  else
  {
    *adc_channel = 0;
  }
  data.LT_byte[2] &= 0x3F;                  // Clear channel bit here so code to voltage function doesn't have to.
  data.LT_byte[3] = 0x00;                   // Make sure MS Byte is zero, data is right-justified.
  *code = data.LT_int32;                    // Return data
}

// Calculates the voltage given the ADC code and lsb weight.
float LTC2422_code_to_voltage(int32_t adc_code, float LTC2422_lsb)
{
  float adc_voltage;
  adc_code -= 0x00200000;                         //! 1) Subtract offset
  adc_voltage=((float)adc_code)*LTC2422_lsb;      //! 2) Calculate voltage from ADC code and lsb
  return(adc_voltage);
}

// Calculates the lsb weight from the given reference voltage.
void LTC2422_calculate_lsb(float LTC2422_reference_voltage, float *LTC2422_lsb)
{
  *LTC2422_lsb = LTC2422_reference_voltage/(1048575);        //! 1) Calculate the LSB, ref_voltage /(2^20-1)
}


