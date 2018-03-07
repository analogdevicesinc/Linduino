
/*!
LTC1867: 16-Bit 8-Channel 200ksps ADC

@verbatim

The LTC1863/LTC1867 are pin-compatible, 8-channel 12-/16-bit A/D converters with
serial I/O, and an internal reference. The ADCs typically draw only 1.3mA from a
single 5V supply. The 8-channel input multiplexer can be configured for either
single-ended or differential inputs and unipolar or bipolar conversions (or
combinations thereof). The automatic nap and sleep modes benefit power sensitive
applications.

The LTC1867's DC performance is outstanding with a +/-2LSB INL specification and
no missing codes over temperature. The signal-to-noise ratio (SNR) for the
LTC1867 is typically 89dB, with the internal reference.

@endverbatim

http://www.linear.com/product/LTC1867

http://www.linear.com/product/LTC1867#demoboards


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
//! @defgroup LTC1867 LTC1867: 16-Bit 8-Channel 200ksps ADC
//! @}

/*! @file
    @ingroup LTC1867
    Library for LTC1867: 16-Bit 8-Channel 200ksps ADC
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC1867.h"
#include <SPI.h>


// Reads the ADC  and returns 16-bit data
void LTC1867_read(uint8_t cs, uint8_t adc_command, uint16_t *adc_code)
{
  spi_transfer_word(cs, (uint16_t)(adc_command<<8), adc_code);
}

// Calculates the LTC1867 input's unipolar voltage given the binary data and lsb weight.
float LTC1867_unipolar_code_to_voltage(uint16_t adc_code, float LTC1867_lsb, int32_t LTC1867_offset_unipolar_code)
{
  float adc_voltage;
  adc_voltage=((float)(adc_code+LTC1867_offset_unipolar_code))*LTC1867_lsb;   //! 1) Calculate voltage from ADC code, lsb, offset.
  return(adc_voltage);
}

// Calculates the LTC1867 input's bipolar voltage given the two's compliment data and lsb weight
float LTC1867_bipolar_code_to_voltage(uint16_t adc_code, float LTC1867_lsb, int32_t LTC1867_offset_bipolar_code)
{
  float adc_voltage, sign = 1.0;
  if (adc_code>>15)
  {
    adc_code = (adc_code ^ 0xFFFF)+1;                                           //! 1) Convert ADC code from two's complement to binary
    sign = -1;
  }
  adc_voltage=((float)(adc_code+LTC1867_offset_bipolar_code))*LTC1867_lsb*sign; //! 2) Calculate voltage from ADC code, lsb, offset.
  return(adc_voltage);
}

// Calibrate the lsb
void LTC1867_cal_voltage(uint16_t zero_unipolar_code, uint16_t zero_bipolar_code, uint16_t fs_code, float zero_voltage, float fs_voltage, float *LTC1867_lsb, int32_t *LTC1867_offset_unipolar_code, int32_t *LTC1867_offset_bipolar_code)
{
  float temp_offset;
  *LTC1867_lsb = (fs_voltage-zero_voltage)/((float)(fs_code - zero_unipolar_code));                     //! 1) Calculate the LSB

  temp_offset = (zero_voltage/ *LTC1867_lsb) - zero_unipolar_code;                                      //! 2) Calculate Unipolar offset
  temp_offset = (temp_offset > (floor(temp_offset) + 0.5)) ? ceil(temp_offset) : floor(temp_offset);    //! 3) Round
  *LTC1867_offset_unipolar_code = (int32_t)temp_offset;                                                 //! 4) Cast as int32_t

  temp_offset = (zero_voltage / *LTC1867_lsb) - zero_bipolar_code ;                                     //! 5) Calculate Bipolar offset
  temp_offset = (temp_offset > (floor(temp_offset) + 0.5)) ? ceil(temp_offset) : floor(temp_offset);    //! 6) Round
  *LTC1867_offset_bipolar_code = (int32_t)temp_offset;                                                  //! 7) cast as int32_t
}
