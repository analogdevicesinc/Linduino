/*!
LTC2473: 16-Bit, Delta Sigma ADC with I2C interface.

@verbatim

The LTC2471/LTC2473 are small, 16-bit analog-to-digital converters with an
integrated precision reference and a selectable 208sps or 833sps output
rate. They use a single 2.7V to 5.5V supply and communicate through an I2C
Interface. The LTC2471 is single-ended with a 0V to VREF input range and
the LTC2473 is differential with a +-VREF input range. Both ADC's include
a 1.25V integrated reference with 2ppm/C drift performance and 0.1% initial
accuracy. They include an integrated oscillator and perform conversions with
no latency for multiplexed applications. The LTC2471/LTC2473 include a
proprietary input sampling scheme that reduces the average input current
several orders of magnitude when compared to conventional delta sigma
converters.

@endverbatim

http://www.linear.com/product/LTC2473

http://www.linear.com/product/LTC2473#demoboards

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
//! @defgroup LTC2473 LTC2473: 16-Bit, Delta Sigma ADC with I2C interface
//! @}

/*! @file
    @ingroup LTC2473
    Library for LTC2473: 16-Bit, Delta Sigma ADC with I2C interface
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LTC2473.h"
#include "LTC24XX_general.h"

// Reads from LTC2473.
uint8_t LTC2473_read(uint8_t i2c_address, int32_t *adc_code, uint16_t timeout)
{
  uint8_t ack = 0;
  ack = LTC24XX_I2C_32bit_data( i2c_address, adc_code, timeout);    // Read data
  return ack;
}

// Write to the LTC2473
uint8_t LTC2473_write(uint8_t i2c_address, uint8_t adc_command)
{
  int32_t data;
  uint8_t ack = 0;
  ack = LTC24XX_I2C_8bit_command_32bit_data(i2c_address, adc_command, &data, 300); // Write command
  return ack;
}

// Calculates the voltage corresponding to an adc code, given the reference (in volts)
float LTC2473_code_to_voltage(int32_t adc_code, float vref)
{
  float adc_voltage;
  adc_voltage =  LTC24XX_diff_code_to_voltage(adc_code, vref);
  return(adc_voltage);
}
