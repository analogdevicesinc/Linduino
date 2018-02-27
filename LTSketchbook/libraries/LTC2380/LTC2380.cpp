/*!
LTC2376-16: 16-Bit, 250Ksps ADC
LTC2376-18: 18-Bit, 250Ksps ADC
LTC2376-20: 20-Bit, 250Ksps ADC
LTC2377-16: 16-Bit, 500Ksps ADC
LTC2377-18: 18-Bit, 500Ksps ADC
LTC2377-20: 20-Bit, 500Ksps ADC
LTC2378-16: 16-Bit, 1Msps ADC
LTC2378-18: 18-Bit, 1Msps ADC
LTC2378-20: 20-Bit, 1Msps ADC
LTC2379-18: 18-Bit, 1.6Msps ADC
LTC2380-16: 16-Bit, 2Msps ADC

@verbatim

The following parts (DUT) are pin-compatible, 16/18/20-bit A/D converters with serial I/O, and an internal reference:
LTC2376-16: 16-Bit, 250Ksps ADC
LTC2376-18: 18-Bit, 250Ksps ADC
LTC2376-20: 20-Bit, 250Ksps ADC
LTC2377-16: 16-Bit, 500Ksps ADC
LTC2377-18: 18-Bit, 500Ksps ADC
LTC2377-20: 20-Bit, 500Ksps ADC
LTC2378-16: 16-Bit, 1Msps ADC
LTC2378-18: 18-Bit, 1Msps ADC
LTC2378-20: 20-Bit, 1Msps ADC
LTC2379-18: 18-Bit, 1.6Msps ADC
LTC2380-16: 16-Bit, 2Msps ADC

The DUTs typically draw only 8.4 mA from a single 2.5V supply. The automatic nap and sleep modes benefit power sensitive applications.

The DUTs DC performance is outstanding with a +/-0.5 ppm INL specification and
no missing codes over temperature. The signal-to-noise ratio (SNR) for the LTC2380 is typically 104 dB, with the internal reference.

@endverbatim

http://www.linear.com/product/LTC2376-16
http://www.linear.com/product/LTC2376-18
http://www.linear.com/product/LTC2376-20
http://www.linear.com/product/LTC2377-16
http://www.linear.com/product/LTC2377-18
http://www.linear.com/product/LTC2377-20
http://www.linear.com/product/LTC2378-16
http://www.linear.com/product/LTC2378-18
http://www.linear.com/product/LTC2378-20
http://www.linear.com/product/LTC2379-18
http://www.linear.com/product/LTC2380-16

http://www.linear.com/product/LTC2376-16#demoboards
http://www.linear.com/product/LTC2376-18#demoboards
http://www.linear.com/product/LTC2376-20#demoboards
http://www.linear.com/product/LTC2377-16#demoboards
http://www.linear.com/product/LTC2377-18#demoboards
http://www.linear.com/product/LTC2377-20#demoboards
http://www.linear.com/product/LTC2378-16#demoboards
http://www.linear.com/product/LTC2378-18#demoboards
http://www.linear.com/product/LTC2378-20#demoboards
http://www.linear.com/product/LTC2379-18#demoboards
http://www.linear.com/product/LTC2380-16#demoboards


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
//! @defgroup LTC2380 LTC2380: 16/18/20-Bit 1Msps ADC
//! @}

/*! @file
    @ingroup LTC2380
    Library for LTC2380: 16/18/20-Bit 1Msps ADC
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC2380.h"
#include <SPI.h>


// Reads from a SPI LTC2380-XX device that has no configuration word and a 32 bit output word in 2's complement format.
void LTC2380_read(uint8_t cs, int32_t *ptr_adc_code)
{
  LT_union_int16_2bytes command;    // LTC2380 data and command
  command.LT_uint16 = 0;                // Set to zero, not necessary but avoids
  // random data in scope shots.

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


// Calculates the voltage corresponding to an adc code in 2's complement, given the reference voltage (in volts)
float LTC2380_code_to_voltage(int32_t adc_code, uint8_t gain_compression, float vref)
{
  float voltage;

  if (gain_compression == 1)
    vref = 0.8*vref;

  voltage = (float)adc_code;
  voltage = voltage / (pow(2,31)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
  voltage = voltage * vref + 0.1;           //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)

  return(voltage);
}
