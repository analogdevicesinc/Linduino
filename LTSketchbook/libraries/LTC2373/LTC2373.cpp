/*!
LTC2373: 16/18-bit 1Msps 8 channel SAR ADC
LTC2372: 16/18-bit 500ksps 8 channel SAR ADC
LTC2374: 16 bit 1.6Msps 8 channel SAR ADC

@verbatim

The LTC2373_16/18 are pin-compatible, 16/18-bit A/D converters with
serial I/O, and an internal reference.

@endverbatim

http://www.linear.com/product/LTC2373-16
http://www.linear.com/product/LTC2373_18
http://www.linear.com/product/LTC2372-16
http://www.linear.com/product/LTC2372-18
http://www.linear.com/product/LTC2374-16

http://www.linear.com/product/LTC2373-16#demoboards
http://www.linear.com/product/LTC2373_18#demoboards
http://www.linear.com/product/LTC2372-16#demoboards
http://www.linear.com/product/LTC2372-18#demoboards
http://www.linear.com/product/LTC2374-16#demoboards


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
//! @defgroup LTC2373 LTC2373: 16/18-bit 1Msps 8 channel SAR ADC
//! @}

/*! @file
    @ingroup LTC2373
    Library for LTC2373: 16/18-bit 1Msps 8 channel SAR ADC
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "LTC2373.h"
#include <SPI.h>

// Builds the ADC command
uint8_t LTC2373_build_command(uint8_t sequencer_bit, uint8_t ch_designate, uint8_t range_select, uint8_t gain_compression)
{
  uint8_t adc_command;
  adc_command = sequencer_bit | ch_designate | range_select | gain_compression;

  return (adc_command);
}


// Reads from a SPI LTC2373-XX device that has a configuration word and a 20 bit output word.
void LTC2373_read(uint8_t cs, uint8_t adc_command, uint32_t *ptr_adc_code)
{
//Form a four byte object to hold four bytes of command
  LT_union_int32_4bytes command;  //instantiate the union
  command.LT_byte[3] = adc_command;
  command.LT_byte[2] = 0;
  command.LT_byte[1] = 0;
  command.LT_byte[0] = 0;

//Form a four byte object to hold four bytes of data
  LT_union_int32_4bytes data; //instantiate the union
  data.LT_byte[3] = 0;
  data.LT_byte[2] = 0;
  data.LT_byte[1] = 0;
  data.LT_byte[0] = 0;

  spi_transfer_block(cs,command.LT_byte,data.LT_byte,4);

  *ptr_adc_code = data.LT_int32;

  return;
}


// Programs multiple 8 bit control words to an SPI LTC2373-XX device.
void LTC2373_configure(uint8_t cs, uint32_t adc_configure)
{
//Form a four byte object to hold four bytes of command
  LT_union_int32_4bytes command;  //instantiate the union
  command.LT_uint32 = adc_configure;

//Form a four byte object to hold four bytes of dummy data
  LT_union_int32_4bytes data; //instantiate the union
  data.LT_byte[3] = 0;
  data.LT_byte[2] = 0;
  data.LT_byte[1] = 0;
  data.LT_byte[0] = 0;

  spi_transfer_block(cs,command.LT_byte,data.LT_byte,4);

  return;
}


// Calculates the voltage corresponding to an adc code, given the reference voltage (in volts)
float LTC2373_code_to_voltage(uint8_t adc_command, uint32_t adc_code, float vref)
{
  float voltage, range;
  float sign = 1;
  uint8_t config;

  config = (adc_command >> 1) & 0x03;

  switch (config)
  {
    case (0): //unipolar, 0 to vref
      voltage = (float)adc_code;
      voltage = voltage / (pow(2,32)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
      voltage = voltage * vref;           //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)
      break;

    case (1): //bipolar, +/- vref/2, twos complement out
      if ((adc_code>>31) == 1)  //number is < 0
      {
        adc_code = (adc_code ^ 0xFFFFFFFF)+1;                                    //! Convert ADC code from two's complement to binary
        sign = -1;
      }

      voltage = (float)adc_code;
      voltage = voltage / (pow(2,31)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
      voltage = sign * voltage * vref/2;           //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)
      break;

    case (2): //differential unipolar, 0 to vref
      voltage = (float)adc_code;
      voltage = voltage / (pow(2,32)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
      voltage = sign * voltage * vref;           //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)
      break;

    case (3): //differential bipolar, +/- vref, twos complement out
      if ((adc_code>>31) == 1)  //number is < 0
      {
        adc_code = (adc_code ^ 0xFFFFFFFF)+1;                                    //! Convert ADC code from two's complement to binary
        sign = -1;
      }

      voltage = (float)adc_code;
      voltage = voltage / (pow(2,31)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
      voltage = sign * voltage * vref;           //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)
      break;
  }

  return(voltage);
}
