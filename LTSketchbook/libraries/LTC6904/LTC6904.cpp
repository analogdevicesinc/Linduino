/*!
LTC6904: 1kHz to 68MHz Serial Port Programmable Oscillator

@verbatim

The LTC6903/LTC6904 are low power self contained digital
frequency sources providing a precision frequency from
1kHz to 68MHz, set through a serial port. The LTC6903/
LTC6904 require no external components other than a
power supply bypass capacitor, and they operate over a
single wide supply range of 2.7V to 5.5V.

@endverbatim

http://www.linear.com/product/LTC6904

http://www.linear.com/product/LTC6904#demoboards


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

//! @ingroup RF_Timing
//! @{
//! @defgroup LTC6904 LTC6904: 1kHz to 68MHz Serial Port Programmable Oscillator
//! @}

/*! @file
    @ingroup LTC6904
    Library for LTC6904: 1kHz to 68MHz Serial Port Programmable Oscillator
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LTC6904.h"
#include <Wire.h>
#include "math.h"

// Writes 2 bytes
uint8_t LTC6904_write(uint8_t address, uint16_t code)
{
  uint8_t ack;
  ack = i2c_write_byte_data(address, (0x0FF00 & code)>>8, 0x0FF & code);
  return(ack);
}

// Calculates the code necessary to create the clock frequency
// Returns the Code for the LTC6904
uint16_t LTC6904_frequency_to_code(float frequency, uint8_t clk)
{
  uint8_t oct;
  double oct_double;
  float float_dac;
  uint16_t dac;

  // Calculate OCT
  oct_double = log10((double)((frequency*1000000)/1039));
  oct_double *= 3.322;

  // Keep OCT within range
  if (oct_double>15)
    oct = 15;
  if (oct_double<0)
    oct = 0;
  oct = (uint8_t)oct_double;  // Cast as uint8_t , round down

  // Calculate DAC code
  float_dac = 2048-(2078*pow(2, (10+ oct)))/(frequency*1000000);   // Calculate the dac code
  float_dac = (float_dac > (floor(float_dac) + 0.5)) ? ceil(float_dac) : floor(float_dac);  // Round

  // Keep DAC within range
  if (float_dac>1023)
    float_dac = 1023;
  if (float_dac<0)
    float_dac = 0;

  dac = (uint16_t)float_dac;  // Cast as uint16_t

  return((uint16_t)((oct<<12)| (dac<<2) | clk));
}