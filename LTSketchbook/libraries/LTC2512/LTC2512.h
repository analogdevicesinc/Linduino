/*!
LTC2512: 24-Bit Over-Sampling ADC with Configurable Digital Filter.

@verbatim

The LTC2512-24 is a low noise, low power, high performance 24-bit ADC
with an integrated configurable digital filter. Operating from a single
2.5V supply, the LTC2512-24 features a fully differential input range
up to ±VREF, with VREF ranging from 2.5V to 5.1V. The LTC2512-24 supports
a wide common mode range from 0V to VREF simplifying analog signal
conditioning requirements.

@endverbatim

http://www.linear.com/product/LTC2512-24

http://www.linear.com/product/LTC2512-24#demoboards


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

/*! @file
    @ingroup LTC2512
    Header for LTC2512: 24-Bit Over-Sampling ADC with Configurable Digital Filter
*/

#ifndef LTC2512_H
#define LTC2512_H


#define SNEAKER_PORT_ADDRESS  0x20
#define MCLK_pin        QUIKEVAL_CS

#define CONFIG_DF_4       0x8000
#define CONFIG_DF_8       0x8400
#define CONFIG_DF_16      0x8200
#define CONFIG_DF_32      0x8600

#define CS_LOW          0x00
#define CS_HIGH         0x08

//! Calculates the output voltage from the given digital code and reference voltage
float LTC2512_code_to_voltage(int32_t code, float vref);

//! Send n num of pulses on pin given
void send_pulses(uint8_t pin, uint16_t num_of_pulses);

//! Reads 4 bytes of data on SPI - D23:D0 + W7:W0
uint32_t LTC2512_read_data(uint8_t QUIKEVAL_CS, uint16_t *DF);

#endif