/*!
LTC2482: 16-Bit Delta-Sigma ADC with Easy Drive Input Current Cancellation.

@verbatim

The LTC2480 is a 16-Bit Delta-Sigma ADC with Easy Drive Input Current Cancellation.
The LTC2482 allows a wide common mode input range (0V to VCC) independent of the
reference voltage. The reference can be as low as 100mV or can be tied directly to
VCC. The noise level is 600nV RMS independent of VREF . This allows direct
digitization of low level signals with 16-bit accuracy. The LTC2482 includes an on-chip
trimmed oscillator, eliminating the need for external crystals or oscillators and
provides 87dB rejection of 50Hz and 60Hz line frequency noise. Absolute accuracy and low
drift are automatically maintained through continuous, transparent, offset and
full-scale calibration.

Example Code:

Read ADC input.

    LTC2482_read(LTC2482_CS, &adc_code);   // Throws out last reading
    LTC2482_read(LTC2482_CS, &adc_code);   // Obtains the current reading and stores to adc_code variable

  display_code = adc_code >> 4;
    display_code = display_code & 0xFFFF;

    // Convert adc_code to voltage
    adc_voltage = LTC2482_code_to_voltage(display_code, vref);

@endverbatim

http://www.linear.com/product/LTC2482

http://www.linear.com/product/LTC2482#demoboards


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
    @ingroup LTC2482
    Header for LTC2482: 16-Bit Delta-Sigma ADC with Easy Drive Input Current Cancellation.
*/

#ifndef LTC2482_H
#define LTC2482_H

//! Define the SPI CS pin
#ifndef LTC2482_CS
#define LTC2482_CS QUIKEVAL_CS
#endif

//! Reads the LTC2482 and returns 24-bit data
//! @return void
void LTC2482_read(uint8_t cs,         //!< Chip Select Pin
                  uint32_t *ptr_adc_code  //!< Returns code read from ADC (from previous conversion)
                 );

//! Calculates the LTC2482 input voltage given the binary data, reference voltage and input gain.
//! @return Floating point voltage
float LTC2482_code_to_voltage(uint32_t adc_code,      //!< Raw ADC code
                              float vref          //!< Reference voltage
                             );


#endif  //  LTC2482_H