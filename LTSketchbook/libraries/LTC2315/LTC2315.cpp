/*!
LTC2312-12: 12-Bit, 500Ksps ADC.
LTC2312-14: 14-Bit, 500Ksps ADC.
LTC2313-12: 12-Bit, 2.5Msps ADC
LTC2313-14: 14-Bit, 2.5Msps ADC
LTC2314-14: 14-Bit, 4.5Msps ADC
LTC2315-12: 12-Bit, 5Msps ADC

@verbatim

The following parts (DUT) are pin-compatible, 12/14-bit A/D converters with serial I/O, and an internal reference:
LTC2312-12: 12-Bit, 500Ksps ADC.
LTC2312-14: 14-Bit, 500Ksps ADC.
LTC2313-12: 12-Bit, 2.5Msps ADC
LTC2313-14: 14-Bit, 2.5Msps ADC
LTC2314-14: 14-Bit, 4.5Msps ADC
LTC2315-12: 12-Bit, 5Msps ADC

@endverbatim

http://www.linear.com/product/LTC2312-12
http://www.linear.com/product/LTC2312-14
http://www.linear.com/product/LTC2313-12
http://www.linear.com/product/LTC2313-14
http://www.linear.com/product/LTC2314-14
http://www.linear.com/product/LTC2315-12

http://www.linear.com/product/LTC2312-12#demoboards
http://www.linear.com/product/LTC2312-14#demoboards
http://www.linear.com/product/LTC2313-12#demoboards
http://www.linear.com/product/LTC2313-14#demoboards
http://www.linear.com/product/LTC2314-14#demoboards
http://www.linear.com/product/LTC2315-12#demoboards


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
//! @defgroup LTC2315 LTC2315: 12/14-Bit 1Msps ADC
//! @}

/*! @file
    @ingroup LTC2315
    Library for LTC2315: 12/14-Bit 1Msps ADC
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC2315.h"
#include <SPI.h>


// Reads the ADC  and returns 16-bit data
void LTC2315_read(uint8_t cs, uint16_t *adc_code)
{
  uint16_t dummy_command = 0;

  spi_transfer_word(cs, dummy_command, adc_code);
}


// Calculates the voltage corresponding to an adc code in offset binary, given the reference voltage (in volts)
float LTC2315_code_to_voltage(uint16_t adc_code, uint8_t shift, float vref)
{
  float voltage;

  adc_code = adc_code << shift;  //the data is left justified to bit_14 of a 16 bit word

  voltage = (float)adc_code;
  voltage = voltage / (pow(2,16)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
  voltage = voltage * vref;           //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)

  return(voltage);
}
