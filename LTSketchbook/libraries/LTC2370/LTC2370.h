/*!
LTC2364-16: 16-Bit, 250Ksps ADC.
LTC2364-18: 18-Bit, 250Ksps ADC.
LTC2367-16: 16-Bit, 500Ksps ADC
LTC2367-18: 18-Bit, 500Ksps ADC
LTC2368-16: 16-Bit, 1Msps ADC
LTC2368-18: 18-Bit, 1Msps ADC
LTC2369-18: 18-Bit, 1.6Msps ADC
LTC2370-16: 16-Bit, 2Msps ADC

@verbatim

The following parts (DUT) are pin-compatible, 16/18-bit A/D converters with serial I/O, and an internal reference:
LTC2364-16: 16-Bit, 250Ksps ADC
LTC2364-18: 18-Bit, 250Ksps ADC
LTC2367-16: 16-Bit, 500Ksps ADC
LTC2367-18: 18-Bit, 500Ksps ADC
LTC2368-16: 16-Bit, 1Msps ADC
LTC23678-18: 18-Bit, 1Msps ADC
LTC2369-18: 18-Bit, 1.6Msps ADC
LTC2370-16: 16-Bit, 2Msps ADC

Example Code:

Read ADC input.

    LTC2370_read(LTC2370_CS, &adc_code);   // Throws out last reading
    LTC2370_read(LTC2370_CS, &adc_code);   // Obtains the current reading and stores to adc_code variable

    // Convert adc_code to voltage
    adc_voltage = LTC2370_code_to_voltage(adc_code, vref);

@endverbatim

http://www.linear.com/product/LTC2364-16
http://www.linear.com/product/LTC2364-18
http://www.linear.com/product/LTC2367-16
http://www.linear.com/product/LTC2367-18
http://www.linear.com/product/LTC2368-16
http://www.linear.com/product/LTC2368-18
http://www.linear.com/product/LTC2369-18
http://www.linear.com/product/LTC2370-16

http://www.linear.com/product/LTC2364-16#demoboards
http://www.linear.com/product/LTC2364-18#demoboards
http://www.linear.com/product/LTC2367-16#demoboards
http://www.linear.com/product/LTC2367-18#demoboards
http://www.linear.com/product/LTC2368-16#demoboards
http://www.linear.com/product/LTC2368-18#demoboards
http://www.linear.com/product/LTC2369-18#demoboards
http://www.linear.com/product/LTC2370-16#demoboards


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
    @ingroup LTC2370
    Header for LTC2370: 16/18-bit 1Msps ADC
*/

#ifndef LTC2370_H
#define LTC2370_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC2370_CS
#define LTC2370_CS QUIKEVAL_CS
#endif

//! @name LTC2370 Channel Address
//! @{
// Channel Address
#define LTC2370_ADDRESS             0x00
//!@}


//! Reads the LTC2370 and returns 32-bit data in offset binary format
//! @return void
void LTC2370_read(uint8_t cs,           //!< Chip Select Pin
                  uint32_t *ptr_adc_code    //!< Returns code read from ADC (from previous conversion)
                 );


//! Calculates the LTC2370 input voltage given the binary data and lsb weight.
//! @return Floating point voltage
float LTC2370_code_to_voltage(uint32_t adc_code,                   //!< Raw ADC code
                              float vref              //!< Reference voltage
                             );

#endif  //  LTC2370_H


