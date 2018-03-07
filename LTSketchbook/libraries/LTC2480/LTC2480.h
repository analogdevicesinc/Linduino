/*!
LTC2480: 16-Bit Delta-Sigma ADC with Easy Drive Input Current Cancellation.

@verbatim

The LTC2480 is a 16-Bit Delta-Sigma ADC with Easy Drive Input Current Cancellation.
It includes on-chip programmable gain and an oscillator. The LTC2480 can be
configured to provide a programmable gain from 1 to 256 in 8 steps, measure an
external signal or internal temperature sensor and reject line frequencies. 50Hz,
60Hz or simultaneous 50Hz/60Hz line frequency rejection can be selected as well
as a 2x speed-up mode.

Example Code:

Read ADC input.

    LTC2480_read(LTC2480_CS, &adc_code);   // Throws out last reading
    LTC2480_read(LTC2480_CS, &adc_code);   // Obtains the current reading and stores to adc_code variable

  display_code = adc_code >> 4;
    display_code = display_code & 0xFFFF;

  // Convert adc_code to voltage
    adc_voltage = LTC2480_code_to_voltage(display_code, vref, gain);

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

/*! @file
    @ingroup LTC2480
    Header for LTC2480: 16-Bit Delta-Sigma ADC with Easy Drive Input Current Cancellation.
*/

#ifndef LTC2480_H
#define LTC2480_H

//! Define the SPI CS pin
#ifndef LTC2480_CS
#define LTC2480_CS QUIKEVAL_CS
#endif

//! @name LTC480 Input Gain
//! @{
// Input Gain
#define LTC2480_GAIN_1 0x00
#define LTC2480_GAIN_4 0x10
#define LTC2480_GAIN_8 0x20
#define LTC2480_GAIN_16 0x30
#define LTC2480_GAIN_32 0x40
#define LTC2480_GAIN_64 0x50
#define LTC2480_GAIN_128 0x60
#define LTC2480_GAIN_256 0x70
//!@}

//! Function to set the input voltage gain and frequency rejection mode
//! @return void
void LTC2480_set_modes(uint8_t cs,        //!< Chip Select Pin
                       uint8_t gain,       //!< Input voltage gain
                       uint8_t rejection_mode);  //!< Frequency rejection mode

//! Reads the LTC2480 and returns 24-bit data
//! @return void
void LTC2480_read(uint8_t cs,         //!< Chip Select Pin
                  uint32_t *ptr_adc_code  //!< Returns code read from ADC (from previous conversion)
                 );

//! Calculates the LTC2480 input voltage given the binary data, reference voltage and input gain.
//! @return Floating point voltage
float LTC2480_code_to_voltage(uint32_t adc_code,      //!< Raw ADC code
                              float vref,           //!< Reference voltage
                              uint8_t gain          //!< Input voltage gain
                             );


#endif  //  LTC2480_H