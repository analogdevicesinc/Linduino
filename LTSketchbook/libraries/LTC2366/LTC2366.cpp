/*!
LTC2360-12: 12-Bit, 100Ksps ADC.
LTC2361-12: 12-Bit, 250Ksps ADC.
LTC2362-12: 12-Bit, 500Ksps ADC
LTC2365-12: 12-Bit, 1Msps ADC
LTC2366-12: 12-Bit, 3Msps ADC

@verbatim

The following parts (DUT) are pin-compatible, 12-bit A/D converters with serial I/O, and an internal reference:
LTC2360-12: 12-Bit, 100Ksps ADC.
LTC2361-12: 12-Bit, 250Ksps ADC.
LTC2362-12: 12-Bit, 500Ksps ADC
LTC2365-12: 12-Bit, 1Msps ADC
LTC2366-12: 12-Bit, 3Msps ADC

@endverbatim

http://www.linear.com/product/LTC2360-12
http://www.linear.com/product/LTC2361-12
http://www.linear.com/product/LTC2362-12
http://www.linear.com/product/LTC2365-12
http://www.linear.com/product/LTC2366-12

http://www.linear.com/product/LTC2360-12#demoboards
http://www.linear.com/product/LTC2361-12#demoboards
http://www.linear.com/product/LTC2362-12#demoboards
http://www.linear.com/product/LTC2365-12#demoboards
http://www.linear.com/product/LTC2366-12#demoboards

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
//! @defgroup LTC2366 LTC2366: 12-Bit 3Msps ADC
//! @}

/*! @file
    @ingroup LTC2366
    Library for LTC2366: 12-Bit 3Msps ADC
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC2366.h"
#include <SPI.h>


// Reads the ADC  and returns 16-bit data
void LTC2366_read(uint8_t cs, uint16_t *ptr_adc_code)
{
  uint16_t dummy_command = 0;

  spi_transfer_word(cs, dummy_command, ptr_adc_code);

  return;

}


// Calculates the voltage corresponding to an adc code in offset binary, given the reference voltage (in volts)
float LTC2366_code_to_voltage(uint16_t adc_code, float vref)
{
  float voltage;

  adc_code = adc_code << 2;  //the data is left justified to bit_13 of a 16 bit word

  voltage = (float)adc_code;
  voltage = voltage / (pow(2,16)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
  voltage = voltage * vref;           //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)

  return(voltage);
}
