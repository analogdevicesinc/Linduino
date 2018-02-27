/*!
LTC2485: 24-Bit Delta Sigma ADC with Easy Drive Input Current Cancellation and
I2C Interface

@verbatim

The LTC2485 combines a 24-bit plus sign No Latency Delta Sigma analog-to-digital
converter with patented Easy Drive technology and I2C digital interface. The
patented sampling scheme eliminates dynamic input current errors and the
shortcomings of on-chip buffering through automatic cancellation of differential
input current. This allows large external source impedances and input signals,
with rail-to-rail input range to be directly digitized while maintaining
exceptional DC accuracy.

@endverbatim


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
//! @defgroup LTC2485 LTC2485: 24-Bit Delta Sigma ADC with Easy Drive Input Current Cancellation and I2C Interface
//! @}

/*! @file
    @ingroup LTC2485
    Library for LTC2485: 24-Bit Delta Sigma ADC with Easy Drive Input Current Cancellation and I2C Interface
*/


#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LTC2485.h"

// Reads from LTC2485
int8_t LTC2485_read(uint8_t i2c_address, uint8_t adc_command, int32_t *adc_code, uint16_t eoc_timeout)
{
  int8_t ack;
  uint16_t timer_count = 0;        // Timer count to wait for ACK
  LT_union_int32_4bytes data;

  while (1)
  {
    ack = i2c_read_block_data(i2c_address, adc_command, 4, data.LT_byte);
    if (!ack)
      break; // !ack indicates success
    if (timer_count++>eoc_timeout)     // If timeout, return 1 (failure)
      return (1);
    else
      delay(1);
  }

  if (data.LT_byte[3]==0xC0)
  {

    *adc_code= 2147483647; //Positive Overflow
    return (ack);
  }

  if (data.LT_byte[3]==0x3F)
  {
    *adc_code=-2147483648; //Negative Overflow
    return (ack);
  }

  data.LT_byte[3]=data.LT_byte[3] & 0x7F; //Remove sign bit

  data.LT_int32 = data.LT_int32 << 1; //shift left by one bit to restore two's complement

  data.LT_int32/=256;  //Convert back to 24 bit value from 32 bits.
  *adc_code=data.LT_int32;

  return (ack); // Success
}
