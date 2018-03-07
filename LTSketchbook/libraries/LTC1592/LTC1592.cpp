/*!
LTC1592: 16-bit SoftSpan DAC with Programmable Output Range

@verbatim

The LTC1592 is a 16-bit serial input DAC that operates on a single 5V supply.
It can be software-programmed for either unipolar or bipolar mode. It can
also be programmed for different output voltage ranges - two output ranges in
unipolar mode and four output ranges in bipolar mode.

The device includes an internal deglitcher circuit that reduces the glitch
impulse to less than 2nV-s (typ).

When the CS/LD is brought to a logic low, the data on the SDI input is loaded
into the shift register on the rising edge of the clock.  A 4-bit command word
(C3 C2 C1 C0), followed by four “don’t care” bits and 16 data bits(MSB-first)
is the minimum loading sequence required. When the CS/LD is brought to a logic
high, the clock is disabled internally and the command word is executed.

SPI DIN FORMAT (MSB First):

Byte #1                     Byte #2                        Byte #3
C3 C2 C1 C0  X  X   X   X   D15 D14 D13 D12 D11 D10 D9 D8  D7 D6 D5 D4 D3 D2 D1 D0

@endverbatim

http://www.linear.com/product/LTC1592

http://www.linear.com/product/LTC1592#demoboards

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

//! @ingroup Digital_to_Analog_Converters
//! @{
//! @defgroup LTC1592 LTC1592: 16-bit SoftSpan DAC with Programmable Output Range
//! @}

/*! @file
    @ingroup LTC1592
    Library for LTC1592: 16-bit SoftSpan DAC with Programmable Output Range
*/

#include "LT_SPI.h"
#include "Linduino.h"
#include <SPI.h>

// Transmits 24 bit input stream: 4-bit command + 4-bit don't-care + 16-bit data
void LTC1592_write(uint8_t cs, uint8_t dac_command, uint16_t data)
{
  output_low(cs);
  delay(100);
  spi_write(dac_command);
  spi_write((uint8_t)(data >> 8));  //D15:D8
  spi_write((uint8_t)data);     //D7:D0
  delay(100);
  output_high(cs);
}

// Calculates the voltage from ADC output data depending on the channel configuration
float LTC1592_code_to_voltage(uint16_t data, float RANGE_HIGH, float RANGE_LOW)
{
  float voltage;
  voltage = (RANGE_HIGH - RANGE_LOW) * data / 0xFFFF + RANGE_LOW;
  return voltage;
}

// Calculates the 16 bit data code from voltage
uint16_t LTC1592_voltage_to_code(float voltage, float RANGE_HIGH, float RANGE_LOW)
{
  float code;
  code = (voltage - RANGE_LOW) * (0xFFFF / (RANGE_HIGH - RANGE_LOW));
  return code;
}
