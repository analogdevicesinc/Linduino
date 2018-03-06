/*!
LTC2480: 16-Bit Delta-Sigma ADC with Easy Drive Input Current Cancellation.

@verbatim

The LTC2480 is a 16-Bit Delta-Sigma ADC with Easy Drive Input Current Cancellation.
It includes on-chip programmable gain and an oscillator. The LTC2480 can be
configured to provide a programmable gain from 1 to 256 in 8 steps, measure an
external signal or internal temperature sensor and reject line frequencies. 50Hz,
60Hz or simultaneous 50Hz/60Hz line frequency rejection can be selected as well
as a 2x speed-up mode.

@endverbatim

http://www.linear.com/product/LTC2480

http://www.linear.com/product/LTC2480#demoboards


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
//! @defgroup LTC2480 LTC2480: 16-Bit Delta-Sigma ADC with Easy Drive Input Current Cancellation.
//! @}

/*! @file
    @ingroup LTC2480
    Library for LTC2480: 16-Bit Delta-Sigma ADC with Easy Drive Input Current Cancellation.
*/

#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"

// Function to set the input voltage gain and frequency rejection mode
void LTC2480_set_modes(uint8_t cs, uint8_t gain_selection, uint8_t rejection_mode)
{
  uint8_t rx;
  uint8_t mode = gain_selection | rejection_mode;
  mode = mode | 0x80;  // Enable
  spi_transfer_byte(cs, mode, &rx);
}

// Reads the LTC2480 and returns 24-bit data
void LTC2480_read(uint8_t cs, uint32_t *ptr_adc_code)
{
  LT_union_int32_4bytes data, command;  // LTC24XX data and command
  command.LT_uint32 = 0;
  data.LT_uint32 = 0;

  spi_transfer_block(cs, command.LT_byte, data.LT_byte, (uint8_t)3);

  *ptr_adc_code = data.LT_uint32;
}

// Calculates the LTC2480 input voltage given the binary data, reference voltage and input gain.
float LTC2480_code_to_voltage(uint32_t adc_code, float vref, uint8_t gain)
{
  float voltage;

  voltage = (float)adc_code;
  voltage = voltage / (pow(2,16)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
  voltage = voltage * vref;             //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)
  voltage = voltage / gain;       //! 4) Divide by input gain

  return(voltage);
}