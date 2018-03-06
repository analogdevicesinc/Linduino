/*!
LTC2309: 8-channel, 12-Bit SAR ADC with I2C interface
LTC2301: 1-Channel, 12-Bit ADCs with I2C Compatible Interface

@verbatim

The LTC2309 is a low noise, low power, 8-channel, 12-bit successive
approximation ADC with an I2C compatible serial interface. This ADC includes an
internal reference and a fully differential sample-and-hold circuit to reduce
common mode noise. The LTC2309 operates from an internal clock to achieve a fast
1.3 microsecond conversion time.

The LTC2309 operates from a single 5V supply and draws just 300 microamps at a
throughput rate of 1ksps. The ADC enters nap mode when not converting, reducing
the power dissipation.

@endverbatim

http://www.linear.com/product/LTC2309
http://www.linear.com/product/LTC2301

http://www.linear.com/product/LTC2309#demoboards
http://www.linear.com/product/LTC2301#demoboards


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
//! @defgroup LTC2309 LTC2309: 8-channel, 12-Bit SAR ADC with I2C interface
//! @}

/*! @file
    @ingroup LTC2309
    Library for LTC2309: 8-channel, 12-Bit SAR ADC with I2C interface
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LTC2309.h"
#include <Wire.h>

// Commands
// Construct a channel / uni/bipolar by bitwise ORing one choice from the channel configuration
// and one choice from the command.

// Example - read channel 3 single-ended
// adc_command = LTC2309_CH3 | LTC2309_UNIPOLAR_MODE;

// Example - read voltage between channels 5 and 4 with 4 as positive polarity and in bipolar mode.
// adc_command = LTC2309_P4_N5 | LTC2309_BIPOLAR_MODE;


// Reads 12 bits in binary format
int8_t LTC2309_read(uint8_t i2c_address, uint8_t adc_command, uint16_t *ptr_adc_code)
{
  int8_t ack = 0;
  LT_union_int16_2bytes data;

  data.LT_byte[1] = 0;
  data.LT_byte[0] = 0;

  ack = i2c_read_word_data(i2c_address, adc_command, &data.LT_uint16);

  *ptr_adc_code = data.LT_uint16; //note the 12 bits of data are left justified

  return(ack);
}


// Calculates the LTC2309 input unipolar voltage.
float LTC2309_code_to_voltage(uint16_t adc_code, float vref, uint8_t uni_bipolar)
{
  float voltage;
  float sign = 1;

  if (uni_bipolar == LTC2309_UNIPOLAR_MODE)
  {
    voltage = (float)adc_code;
    voltage = voltage / (pow(2,16)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
  }
  else
  {
    vref = vref/2;
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
