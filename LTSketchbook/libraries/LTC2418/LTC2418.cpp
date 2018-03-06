/*!
LTC2418: LTC2418 - 16-Channel 24-Bit No Latency Delta-Sigma ADC

@verbatim

The LTC2414/LTC2418 are 8-/16-channel (4-/8-differential) micropower 24-bit
delta-sigma analog-to-digital converters. They operate from 2.7V to 5.5V and
include an integrated oscillator, 2ppm INL and 0.2ppm RMS noise. They use
delta-sigma technology and provide single cycle settling time for multiplexed
applications. Through a single pin, the LTC2414/LTC2418 can be configured for
better than 110dB differential mode rejection at 50Hz or 60Hz +/-2%, or they can
be driven by an external oscillator for a user-defined rejection frequency. The
internal oscillator requires no external frequency setting components.

@endverbatim

http://www.linear.com/product/LTC2418

http://www.linear.com/product/LTC2418#demoboards


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
//! @defgroup LTC2418 LTC2418: 16-Channel 24-Bit No Latency Delta-Sigma ADC
//! @}

/*! @file
    @ingroup LTC2418
    Library for LTC2418: 16-Channel 24-Bit No Latency Delta-Sigma ADC
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC2418.h"
#include <SPI.h>

int8_t LTC2418_EOC_timeout(uint8_t cs, uint16_t miso_timeout)
// Checks for EOC with a specified timeout
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
  return(0);
}

void LTC2418_read(uint8_t cs, uint8_t adc_command, uint32_t *adc_code)
// Reads the LTC2418
{
  LT_union_int32_4bytes data, command;
  command.LT_byte[3] = adc_command;
  command.LT_byte[2] = 0;
  command.LT_byte[1] = 0;
  command.LT_byte[0] = 0;

  output_low(cs);                                                       //! 1) Pull CS low
  spi_transfer_block(cs, command.LT_byte, data.LT_byte, (uint8_t)4);    //! 2) Transfer 4 bytes
  output_high(cs);                                                      //! 3) Pull CS high
  *adc_code = data.LT_int32;
}

float LTC2418_code_to_voltage(int32_t adc_code, float LTC2418_lsb , int32_t LTC2418_offset_code)
// Calculates the LTC2418 input bipolar voltage
{
  float adc_voltage;
  adc_code = adc_code>>6;                                           //! 1) Bit-shift ADC code to the right 6 bits
  adc_code -= 8388608;                                              //! 2) Convert ADC code from offset binary to binary
  adc_voltage=((float)adc_code+LTC2418_offset_code)*(LTC2418_lsb);  //! 3) Calculate voltage from ADC code, lsb, offset.
  return(adc_voltage);
}

void LTC2418_cal_voltage(int32_t zero_code, int32_t fs_code, float zero_voltage, float fs_voltage, float *LTC2418_lsb , int32_t *LTC2418_offset_code)
// Calibrate the lsb
{
  zero_code = zero_code >> 6;   //! 1) Bit-shift zero code to the right 6 bits
  zero_code -= 8388608;         //! 2) Convert zero code from offset binary to binary
  fs_code = fs_code >> 6;       //! 3) Bit-shift full scale code to the right 6 bits
  fs_code -= 8388608;           //! 4) Convert full scale code from offset binary to binary

  float temp_offset;
  *LTC2418_lsb = (fs_voltage-zero_voltage)/((float)(fs_code - zero_code));                              //! 5) Calculate the LSB

  temp_offset = (zero_voltage/ *LTC2418_lsb) - zero_code;                                               //! 6) Calculate Unipolar offset
  temp_offset = (temp_offset > (floor(temp_offset) + 0.5)) ? ceil(temp_offset) : floor(temp_offset);    //! 7) Round
  *LTC2418_offset_code = (int32_t)temp_offset;                                                          //! 8) Cast as int32_t
}
