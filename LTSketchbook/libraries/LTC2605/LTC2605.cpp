/*!
LTC2605: Octal 16-/14-/12-Bit Rail-to Rail DACs in 16-Lead SSOP

@verbatim

The LTC2605/LTC2615/LTC2625 are octal 16-, 14- and 12-bit, 2.7V to 5.5V
rail-to-rail voltage-output DACs in 16-lead narrow SSOP packages. They have
built-in high performance output buffers and are guaranteed monotonic.

These parts establish new board-density benchmarks for 16-/14-bit DACs and
advance performance standards for output drive, crosstalk and load regulation
in single supply, voltage-output multiples.


@endverbatim


http://www.linear.com/product/LTC2605

http://www.linear.com/product/LTC2605#demoboards

REVISION HISTORY
$Revision: 6237 $
$Date: 2016-12-20 15:09:16 -0800 (Tue, 20 Dec 2016) $


Copyright (c) 2014, Linear Technology Corp.(LTC)
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

//! @ingroup Digital_to_Analog_Converters
//! @{
//! @defgroup LTC2605 LTC2605: Octal 16-/14-/12-Bit Rail-to Rail DACs in 16-Lead SSOP
//! @}

/*! @file
  @ingroup LTC2605
  Library for LTC2605: Octal 16-/14-/12-Bit Rail-to Rail DACs in 16-Lead SSOP
*/
#include <Arduino.h>
#include <stdint.h>
#include <math.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LTC2605.h"

// Write the dac_command byte and 16-bit dac_code to the LTC2605.
// The function returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2605_write(uint8_t i2c_address, uint8_t dac_command, uint8_t dac_address, uint16_t dac_code)
{
  int8_t ack;

  ack = i2c_write_word_data(i2c_address, dac_command | dac_address, dac_code);
  return(ack);
}

// Calculate a LTC26505 DAC code given the desired output voltage and DAC address (0-3)
uint16_t LTC2605_voltage_to_code(float dac_voltage, float LTC2605_lsb, int16_t LTC2605_offset)
{
  int32_t dac_code;
  float float_code;
  float_code = dac_voltage / LTC2605_lsb;                                                             //! 1) Calculate the DAC code
  float_code = (float_code > (floor(float_code) + 0.5)) ? ceil(float_code) : floor(float_code);       //! 2) Round
  dac_code = (int32_t)float_code - LTC2605_offset;                                                    //! 3) Subtract offset
  if (dac_code < 0)                                                                                   //! 4) If DAC code < 0, Then DAC code = 0
    dac_code = 0;
  return ((uint16_t)dac_code);                                                                        //! 5) Cast DAC code as uint16_t
}

// Calculate the LTC2605 DAC output voltage given the DAC code and DAC address (0-3)
float LTC2605_code_to_voltage(uint16_t dac_code, float LTC2605_lsb, int16_t LTC2605_offset)
{
  float dac_voltage;
  dac_voltage = ((float)(dac_code + LTC2605_offset)* LTC2605_lsb);                                    //! 1) Calculates the dac_voltage
  return (dac_voltage);
}