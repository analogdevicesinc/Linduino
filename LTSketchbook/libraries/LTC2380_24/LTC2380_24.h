/*!
LTC2380-24: Low Noise, High Speed, 24-Bit SAR ADC With Digital Filter

@verbatim



@endverbatim

http://www.linear.com/product/LTC2380-24

REVISION HISTORY
$Revision: 4437 $
$Date: 2015-12-01 08:26:42 -0800 (Tue, 01 Dec 2015) $

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
    @ingroup LTC2380-24
    Header for LTC2380-24: Low Noise, High Speed, 24-Bit SAR ADC With Digital Filter
*/

#ifndef LTC2380_H
#define LTC2380_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC2380_CS
#define LTC2380_CS QUIKEVAL_CS
#endif

//! @name LTC2380 Channel Address
//! @{
// Channel Address
#define LTC2380_ADDRESS             0x00
//!@}


//! Reads the LTC2380 and returns 32-bit data in 2's complement format
//! @return void
void LTC2380_read(int32_t *ptr_adc_code,    //!< Returns code read from ADC (from previous conversion)
                  int16_t *ptr_cycles
                 );


//! Calculates the LTC2380 input voltage given the binary data and lsb weight.
//! @return Floating point voltage
float LTC2380_code_to_voltage(int32_t adc_code,                   //!< Raw ADC code
                              uint8_t gain_compression,
                              float vref              //!< Reference voltage
                             );

#endif  //  LTC2380_H


