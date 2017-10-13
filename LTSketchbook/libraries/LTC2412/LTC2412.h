/*!
LTC2412: 2-Channel Differential Input 24-Bit No Latency Delta Sigma ADC
LTC2413: 24-Bit No Latency Delta Sigma ADC with Simultaneous 50Hz/60Hz Rejection ADC

@verbatim

The LTC2412 is a 2-channel differential input micropower 24-bit No Latency
Delta-Sigma analog-to-digital converter with an integrated oscillator. It
provides 2ppm INL and 0.16ppm RMS noise over the entire supply range. The two
differential channels are converted alternately with channel ID included in
the conversion results.

The converter accepts any external differential reference voltage from 0.1V to
VCC for flexible ratiometric and remote sensingmeasurement configurations. The
full-scale differential input range is from –0.5VREF to 0.5VREF. The LTC2412
communicates through a flexible 3-wire digital interface which is compatible
with SPI and MICROWIRE protocols.

SPI DATA FORMAT (MSB First):

            Byte #1                            Byte #2                           Byte #3                    Byte #4

Data Out :  !EOC CH SIG  D23 D22 D21 D20 D19   D18 D17 D16 D15 D14 D13 D12 D11   D10 D9 D8 D7 D6 D5 D4 D3   D2 D1 D0  X  X  X  X  X

!EOC : End of Conversion Bit (Active Low)
CH   : Channel selected
SIG  : Sign Bit (1-data positive, 0-data negative)
Dx   : Data Bits


Example Code:

Read Channel 0 in Single-Ended mode

    uint16_t miso_timeout = 1000;

    if(LTC2412_EOC_timeout(LTC2412_CS, miso_timeout))    // Check for EOC
        return(1);
    LTC2412_read(LTC2412_CS, &adc_code);   // Obtains the current reading and stores to adc_code variable

    // Convert adc_code to voltage
    adc_voltage = LTC2412_code_to_voltage(adc_code, LTC2412_lsb , LTC2412_offset_code);

@endverbatim

http://www.linear.com/product/LTC2412
http://www.linear.com/product/LTC2413

http://www.linear.com/product/LTC2412#demoboards
http://www.linear.com/product/LTC2413#demoboards

REVISION HISTORY
$Revision: 5670 $
$Date: 2016-09-02 10:55:41 -0700 (Fri, 02 Sep 2016) $

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
    @ingroup LTC2412
    Header for LTC2412: 2-Channel Differential Input 24-Bit No Latency Delta Sigma ADC
*/

#ifndef LTC2412_H
#define LTC2412_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC2412_CS
#define LTC2412_CS QUIKEVAL_CS
#endif

//! Checks for EOC with a specified timeout
//! @return 0=successful, 1=unsuccessful (exceeded timeout)
int8_t LTC2412_EOC_timeout(uint8_t cs,           //!< Chip Select pin
                           uint16_t miso_timeout //!< Timeout (ms)
                          );

//! Reads the LTC2412 result
//! @return void
void LTC2412_read(uint8_t cs,           //!< Chip Select pin
                  uint32_t *adc_code    //!< Returns raw 32-bit code read from ADC
                 );

//! Calculates the LTC2412 input voltage
//! @return Calculated voltage
float LTC2412_code_to_voltage(int32_t adc_code,           //!< Raw ADC code
                              float LTC2412_lsb,          //!< LSB value (volts)
                              int32_t LTC2412_offset_code //!< Offset (Code)
                             );

#endif  // LTC2412_H
