/*!
LTC2461: 16-Bit I2C Delta Sigma ADCs with 10ppm/C Max Precision Reference.
LTC2463: Differential, 16-Bit I2C Delta Sigma ADCs with 10ppm/C Max Precision Reference.
LTC2453: Differential, 16-Bit Delta Sigma ADC With I2C Interface.

@verbatim

The LTC2461/LTC2463 are ultra tiny, 16-Bit analog-to-digital converters with an
integrated precision reference. They use a single 2.7V to 5.5V supply and
communicate through an I2C Interface. The LTC2461 is single-ended with a 0V to
1.25V input range and the LTC2463 is differential with a 1.25V input range. Both
ADCs include a 1.25V integrated reference with 2ppm/C drift performance and 0.1%
initial accuracy. The converters are available in a 12-pin 3mm x 3mm DFN package
or an MSOP-12 package. They include an integrated oscillator and perform
conversions with no latency for multiplexed applications. The LTC2461/LTC2463
include a proprietary input sampling scheme that reduces the average input
current several orders of magnitude when compared to conventional delta sigma
converters

@endverbatim

http://www.linear.com/product/LTC2461
http://www.linear.com/product/LTC2463
http://www.linear.com/product/LTC2453

http://www.linear.com/product/LTC2461#demoboards
http://www.linear.com/product/LTC2463#demoboards
http://www.linear.com/product/LTC2453#demoboards


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
//! @defgroup LTC2461 LTC2461: 16-Bit I2C Delta Sigma ADCs with 10ppm/C Max Precision Reference
//! @}

/*! @file
    @ingroup LTC2461
    Library for LTC2461: 16-Bit I2C Delta Sigma ADCs with 10ppm/C Max Precision Reference
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LTC2461.h"
#include <Wire.h>

int8_t LTC2461_read(uint8_t i2c_address, uint8_t adc_command, uint16_t *adc_code)
// Reads 16 bits.
{
  int32_t ack = 0;

  ack = i2c_read_word_data(i2c_address, adc_command, adc_code);

  return(ack);

}

int8_t LTC2461_command(uint8_t i2c_address, uint8_t adc_command)
// Write an 8-bit command to the ADC.
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
{
  int32_t ack = 0;

  ack = i2c_write_byte(i2c_address, adc_command);
  return(ack);
}

float LTC2461_code_to_voltage(uint16_t adc_code, float LTC2461_lsb, int32_t LTC2461_offset_code)
// Calculates the LTC2461 input unipolar voltage.
{
  float adc_voltage;
  adc_voltage = (adc_code + LTC2461_offset_code)*(LTC2461_lsb); //! 1) Calculate voltage from ADC code, lsb, offset.
  return(adc_voltage);
}

void LTC2461_cal_voltage(uint16_t zero_code, uint16_t fs_code, float zero_voltage, float fs_voltage, float *LTC2461_lsb, int32_t *LTC2461_offset_code)  // Function definition
// Calibrate the lsb
{
  float temp_offset;
  *LTC2461_lsb = (fs_voltage-zero_voltage)/((float)(fs_code - zero_code));                              //! 1) Calculate the LSB

  temp_offset = (zero_voltage/ *LTC2461_lsb) - zero_code;                                               //! 2) Calculate Unipolar offset
  temp_offset = (temp_offset > (floor(temp_offset) + 0.5)) ? ceil(temp_offset) : floor(temp_offset);    //! 3) Round
  *LTC2461_offset_code = (int32_t)temp_offset;                                                          //! 4) Cast as int32_t
}
