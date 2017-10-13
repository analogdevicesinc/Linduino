/*!
LTC2326-16: 16-Bit, 250ksps Pseudo-differential input ADC.
LTC2326-18: 18-Bit, 250ksps Pseudo-differential input ADC.
LTC2327-16: 16-Bit, 500ksps Pseudo-differential input ADC
LTC2327-18: 18-Bit, 500ksps Pseudo-differential input ADC
LTC2328-16: 16-Bit, 1Msps Pseudo-differential input ADC
LTC2328-18: 18-Bit, 1Msps Pseudo-differential input ADC
LTC2336-18: 18-Bit, 250ksps Fully-differential input ADC
LTC2337-18: 18-Bit, 500ksps Fully-differential ADC
LTC2338-18: 18-Bit, 1Msps Fully-differential input ADC

@verbatim

The following parts (DUT) are pin-compatible, 16/18-bit A/D converters with serial I/O, and an internal reference:
LTC2326-16: 16-Bit, 250ksps Pseudo-differential input ADC.
LTC2326-18: 18-Bit, 250ksps Pseudo-differential input ADC.
LTC2327-16: 16-Bit, 500ksps Pseudo-differential input ADC
LTC2327-18: 18-Bit, 500ksps Pseudo-differential input ADC
LTC2328-16: 16-Bit, 1Msps Pseudo-differential input ADC
LTC2328-18: 18-Bit, 1Msps Pseudo-differential input ADC
LTC2336-18: 18-Bit, 250ksps Fully-differential input ADC
LTC2337-18: 18-Bit, 500ksps Fully-differential ADC
LTC2338-18: 18-Bit, 1Msps Fully-differential input ADC

Example Code:

Read ADC input.

    LTC2338_read(LTC2338_CS, &adc_code);   // Throws out last reading
    LTC2338_read(LTC2338_CS, &adc_code);   // Obtains the current reading and stores to adc_code variable

    // Convert adc_code to voltage
    adc_voltage = LTC2338_code_to_voltage(adc_code, vref);

@endverbatim

http://www.linear.com/product/LTC2326-16
http://www.linear.com/product/LTC2326-18
http://www.linear.com/product/LTC2327-16
http://www.linear.com/product/LTC2327-18
http://www.linear.com/product/LTC2328-16
http://www.linear.com/product/LTC2328-18
http://www.linear.com/product/LTC2336-18
http://www.linear.com/product/LTC2337-18
http://www.linear.com/product/LTC2338-18

http://www.linear.com/product/LTC2326-16#demoboards
http://www.linear.com/product/LTC2326-18#demoboards
http://www.linear.com/product/LTC2327-16#demoboards
http://www.linear.com/product/LTC2327-18#demoboards
http://www.linear.com/product/LTC2328-16#demoboards
http://www.linear.com/product/LTC2328-18#demoboards
http://www.linear.com/product/LTC2336-18#demoboards
http://www.linear.com/product/LTC2337-18#demoboards
http://www.linear.com/product/LTC2338-18#demoboards

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
    @ingroup LTC2338
    Header for LTC2338: 16/18-bit 1Msps ADC
*/

#ifndef LTC2338_H
#define LTC2338_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC2338_CS
#define LTC2338_CS QUIKEVAL_CS
#endif

//! @name LTC2338 Channel Address
//! @{
// Channel Address
#define LTC2338_ADDRESS             0x00
//!@}


//! Reads the LTC2338 and returns 32-bit data in 2's complement format
//! @return void
void LTC2338_read(uint8_t cs,           //!< Chip Select Pin
                  int32_t *ptr_adc_code    //!< Returns code read from ADC (from previous conversion)
                 );


//! Calculates the LTC2338 input voltage given the binary data and lsb weight.
//! @return Floating point voltage
float LTC2338_code_to_voltage(int32_t adc_code,                   //!< Raw ADC code
                              float vref              //!< Reference voltage
                             );

#endif  //  LTC2338_H


