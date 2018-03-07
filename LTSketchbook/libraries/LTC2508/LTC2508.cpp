/*!
LTC2508: 32-Bit Over-Sampling ADC with Configurable Digital Filter.

@verbatim

The LTC2508-32 is a low noise, low power, high performance 32-bit ADC
with an integrated configurable digital filter. Operating from a single
2.5V supply, the LTC2508-32 features a fully differential input range
up to ±VREF, with VREF ranging from 2.5V to 5.1V. The LTC2508-32 supports
a wide common mode range from 0V to VREF simplifying analog signal
conditioning requirements.

@endverbatim

http://www.linear.com/product/LTC2508

http://www.linear.com/product/LTC2508#demoboards


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
//! @defgroup LTC2508 LTC2508 : 32-Bit Over-Sampling ADC with Configurable Digital Filter.

//! @}

/*! @file
    @ingroup LTC2508
    Library for LTC2508 32-Bit Over-Sampling ADC with Configurable Digital Filter.
*/

#include <Arduino.h>
#include "LT_I2C.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "Linduino.h"
#include <Wire.h>
#include <stdint.h>
#include <SPI.h>

// Calculates the output voltage from the given digital code and reference voltage
float LTC2508_code_to_voltage(int32_t code, float vref)
{
  float voltage;
  voltage = (float)code / 2147483648 * vref;
  return voltage;
}

// Send n num of pulses on pin given
void send_pulses(uint8_t pin, uint16_t num_of_pulses)
{
  uint16_t i;
  output_high(pin);

  for (i = 0; i < num_of_pulses; ++i)
  {
    output_low(pin);                      // Pull CS low
    output_high(pin);                     // Pull CS high
  }
  output_low(pin);                        // Leave CS low
}

// Reads 5 bytes of data on SPI - D31:D0 + W7:W0
uint32_t LTC2508_read_data(uint8_t QUIKEVAL_CS, uint16_t *DF)
{
  uint8_t rx[5];
  uint8_t tx[5] = {0,0,0,0,0};
  uint32_t code = 0;


  spi_transfer_block(QUIKEVAL_CS, tx, rx, 5);       // Read 5 bytes on SPI port

  code = rx[4];
  code = (code << 8) | rx[3];
  code = (code << 8) | rx[2];
  code = (code << 8) | rx[1];

  switch (rx[0])
  {
    case 0x85:
      *DF = 256;
      break;
    case 0xA5:
      *DF = 1024;
      break;
    case 0xC5:
      *DF = 4096;
      break;
    case 0xE5:
      *DF = 16384;
      break;
    default:
      *DF = 0;
  }
  return code;
}