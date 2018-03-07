/*!
LTC2422: 1-/2-Channel 20-Bit uPower No Latency Delta-Sigma ADC in MSOP-10

@verbatim

The LTC2421/LTC2422 are 1- and 2-channel 2.7V to 5.5V micropower 20-bit analog-
to-digital converters with an integrated oscillator, 8ppm INL and 1.2ppm RMS
noise. These ultrasmall devices use delta-sigma technology and a new digital
filter architecture that settles in a single cycle. This eliminates the latency
found in conventional delta-sigma converters and simplifies multiplexed
applications. Through a single pin, the LTC2421/LTC2422 can be configured for
better than 110dB rejection at 50Hz or 60Hz +/-2%, or can be driven by an
external oscillator for a user defined rejection frequency in the range 1Hz to
120Hz. The internal oscillator requires no external frequency setting
components.

SPI DATA FORMAT (MSB First):

            Byte #1                            Byte #2

Data Out :  !EOC CH SIG EXR D19 D18 D17 D16   D15  D14  D13  D12  D11  D10 D9  D8
Data In  :  X    X   X   X   X   X   X   X     X    X    X    X    X    X   X   X

Byte #3
D7 D6 D5 D4 D3 D2 D1 D0
X  X  X  X  X  X  X  X

!EOC : End of Conversion Bit (Active Low)
CH   : Channel Indicator Bit (0 - CH0 , 1 - CH1)
SIG  : Sign Bit (1-data positive, 0-data negative)
EXR  : Extebded Input Range Indicator Bit
Dx   : Data Bits

Example Code:

Read Channel 0.

    uint16_t miso_timeout = 1000;
    if(LTC2422_EOC_timeout(LTC2422_CS, miso_timeout))        // Check for EOC
        return; // Handle exception if timeout exceeded without EOC
    LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);  // Read ADC

    if(adc_channel != 0)
    {
        if(LTC2422_EOC_timeout(LTC2422_CS, miso_timeout))    // Check for EOC
            return; // Handle exception if timeout exceeded without EOC
        LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code); // Reads the ADC again if the first reading was not CH0
    }
    adc_voltage = LTC2422_code_to_voltage(adc_code, LTC2422_lsb);

@endverbatim

http://www.linear.com/product/LTC2422

http://www.linear.com/product/LTC2422#demoboard


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
    @ingroup LTC2422
    Library Header File for LTC2422: 1-/2-Channel 20-Bit uPower No Latency Delta-Sigma ADC in MSOP-10
*/

#ifndef LTC2422_H
#define LTC2422_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC2422_CS
#define LTC2422_CS QUIKEVAL_CS
#endif

const float LTC2422_TYPICAL_lsb = 4.7683761E-6; //!< The LTC2422 typical least significant bit value with 5V full-scale

//! Checks for EOC with a specified timeout
//! @return success or failure on timeout
uint8_t LTC2422_EOC_timeout(uint8_t cs,           //!< Chip Select pin
                            uint16_t miso_timeout //!< Timeout (ms)
                           );

//! Read ADC code from the LTC2422. Does not wait for end-of-conversion.
//! To automatically wait for conversion to complete, use the LTC2422_EOC_timeout before this function.
//! @return void
void LTC2422_adc_read(uint8_t cs,           //!< Chip Select pin
                      uint8_t *adc_channel, //!< Returns channel number read.
                      int32_t *code        //!< Returns the ADC code read.
                     );

//! Calculates the voltage given the ADC code and lsb weight.
//! @return calculated voltage (based on ADC code and lsb weight).
float LTC2422_code_to_voltage(int32_t adc_code,    //!< ADC code read from LTC2422.
                              float LTC2422_lsb     //!< LSB weight (determined by reference voltage).
                             );

//! Calculates the lsb weight from the given reference voltage.
//! @return Void
void LTC2422_calculate_lsb(float LTC2422_reference_voltage, //!< Measured reference voltage.
                           float *LTC2422_lsb               //!< Overwritten with the lsb weight in volts.
                          );

#endif  //  LTC2422_H
