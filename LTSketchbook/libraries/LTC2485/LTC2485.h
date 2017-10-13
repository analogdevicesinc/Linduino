/*!
LTC2485: 24-Bit Delta Sigma ADC with Easy Drive Input Current Cancellation and
I2C Interface

@verbatim

The LTC2485 combines a 24-bit plus sign No Latency Delta Sigma analog-to-digital
converter with patented Easy Drive technology and I2C digital interface. The
patented sampling scheme eliminates dynamic input current errors and the
shortcomings of on-chip buffering through automatic cancellation of differential
input current. This allows large external source impedances and input signals,
with rail-to-rail input range to be directly digitized while maintaining
exceptional DC accuracy.

Example Code:

Read input with 1X speed mode and rejection frequency of 60Hz

    adc_command = LTC2485_R60 | LTC2485_SPEED_1X;                   // Set to 1X mode with 60hz rejection
    ack |= LTC2485_read(i2c_address, adc_command, &adc_code);       // Throws out last reading
    ack |= LTC2485_read(i2c_address, adc_command, &adc_code);       // Obtains the current reading and stores to adc_code variable

@endverbatim

http://www.linear.com/product/LTC2485

http://www.linear.com/product/LTC2485#demoboards

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
    @ingroup LTC2485
    Header for LTC2485: 24-Bit Delta Sigma ADC with Easy Drive Input Current Cancellation and I2C Interface
*/

#ifndef LTC2485_H
#define LTC2485_H

#include "Linduino.h"

/*! @name I2C_Addresses
@{ */

//! I2C address of the LTC2485.
//! Configured by tying the CA1 and CA0 pins high, low, or left floating. See Table 4 of datasheet.
//! Uncomment LTC2485_I2C_ADDRESS to match demo board configuration.
//  Address assignment
// LTC2485 I2C Address                      //  CA1       CA0
// #define LTC2485_I2C_ADDRESS 0b0010100    //  Low       High
// #define LTC2485_I2C_ADDRESS 0b0010101    //  Low       Float
// #define LTC2485_I2C_ADDRESS 0b0010111    //  Float     High
// #define LTC2485_I2C_ADDRESS 0b0100100    //  Float     Float
// #define LTC2485_I2C_ADDRESS 0b0100110    //  High      High
// #define LTC2485_I2C_ADDRESS 0b0100111    //  High      Float

//! LTC2485 Global I2C Address.
#define LTC2485_I2C_GLOBAL_ADDRESS 0b1110111  //  Global Address
/*! @} */

/*! @name Mode Configuration
 @{
 See Table 1 of datasheet.
*/
#define LTC2485_SPEED_1X                        0x00
#define LTC2485_SPEED_2X                        0x01
#define LTC2485_INTERNAL_TEMP                   0x08

// Select rejection frequency - 50 and 60, 50, or 60Hz
#define LTC2485_R50                             0x02
#define LTC2485_R60                             0x04
#define LTC2485_R50_R60                         0x00
/*!
 @}
*/

/*Commands
Construct the control word by bitwise ORing the MODE configuration options.
See Table 1 of datasheet. (C3,C2,C1,C1=X11X is reserved.)

Example - reject 60Hz with 2X mode enabled.
adc_command = (LTC2485_R60  | LTC2485_SPEED_2X);
*/

//! Reads from LTC2485 ADC that accepts an 8 bit configuration and returns a 24 bit result.
//! @return  Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2485_read(uint8_t i2c_address, //!< I2C address (7-bit format) for part
                    uint8_t adc_command, //!< Command byte written to LTC2485. Example: (LTC2485_R60  | LTC2485_SPEED_2X)
                    int32_t *adc_code,   //!< Conversion code read from LTC2485. The LTC2485's 32-bit code is converted to 24-bits inside this function.
                    uint16_t eoc_timeout //!< Timeout in ms
                   );

#endif  // LTC2485_H