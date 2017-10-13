/*!
LTC2360-12: 12-Bit, 100Ksps ADC.
LTC2361-12: 12-Bit, 250Ksps ADC.
LTC2362-12: 12-Bit, 500Ksps ADC
LTC2365-12: 12-Bit, 1Msps ADC
LTC2366-12: 12-Bit, 3Msps ADC

@verbatim

The following parts (DUT) are pin-compatible, 12-bit A/D converters with serial I/O, and an internal reference:
LTC2360-12: 12-Bit, 100Ksps ADC.
LTC2361-12: 12-Bit, 250Ksps ADC.
LTC2362-12: 12-Bit, 500Ksps ADC
LTC2365-12: 12-Bit, 1Msps ADC
LTC2366-12: 12-Bit, 3Msps ADC

Example Code:

Read ADC input.

    LTC2366_read(LTC2366_CS, &adc_code);   // Throws out last reading
    LTC2366_read(LTC2366_CS, &adc_code);   // Obtains the current reading and stores to adc_code variable

    // Convert adc_code to voltage
    adc_voltage = LTC2366_code_to_voltage(adc_code, vref);

@endverbatim

http://www.linear.com/product/LTC2360-12
http://www.linear.com/product/LTC2361-12
http://www.linear.com/product/LTC2362-12
http://www.linear.com/product/LTC2365-12
http://www.linear.com/product/LTC2366-12

http://www.linear.com/product/LTC2360-12#demoboards
http://www.linear.com/product/LTC2361-12#demoboards
http://www.linear.com/product/LTC2362-12#demoboards
http://www.linear.com/product/LTC2365-12#demoboards
http://www.linear.com/product/LTC2366-12#demoboards

REVISION HISTORY
$Revision: 3659 $
$Date: 2015-07-01 10:19:20 -0700 (Wed, 01 Jul 2015) $

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

/*! @file
    @ingroup LTC2366
    Header for LTC2366: 12-bit 3Msps ADC
*/

#ifndef LTC2366_H
#define LTC2366_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC2366_CS
#define LTC2366_CS QUIKEVAL_CS
#endif

//! @name LTC2366 Channel Address
//! @{
// Channel Address
#define LTC2366_ADDRESS             0x00
//!@}


//! Reads the LTC2366 and returns 32-bit data in offset binary format
//! @return void
void LTC2366_read(uint8_t cs,           //!< Chip Select Pin
                  uint16_t *ptr_adc_code    //!< Returns code read from ADC (from previous conversion)
                 );


//! Calculates the LTC2366 input voltage given the binary data and lsb weight.
//! @return Floating point voltage
float LTC2366_code_to_voltage(uint16_t adc_code,                   //!< Raw ADC code
                              float vref              //!< Reference voltage
                             );

#endif  //  LTC2366_H


