/*!
LTC2308: 16-bit 8-channel 100ksps ADC

@verbatim

The LTC2308 is an 8-channel 12-bit A/D converter with
with serial I/O, and an internal reference. The ADCs typically draw only 1.3mA
from a single 5V supply. The 8-channel input multiplexer can be configured for
either single-ended or differential inputs and unipolar or bipolar conversions
(or combinations thereof). The automatic nap and sleep modes benefit power
sensitive applications.

The LTC2308's DC performance is outstanding with a +/-2LSB INL specification and
no missing codes over temperature. The signal-to-noise ratio (SNR) for the
LTC2308 is typically 89dB, with the internal reference.

SPI DATA FORMAT (MSB First):

            Byte #1                           Byte #2
Data Out :  D15 D14 D13 D12 D11 D10  D9  D8    D7  D6  D5  D4  D3  D2  D1  D0
Data In  :  SD  OS  S1  S0  UNI GAIN NAP SLP   X   X   X   X   X   X   X   X

SD   : Single/Differential Bit
OS   : ODD/Sign Bit
Sx   : Address Select Bit
UNI  : Unipolar/Bipolar Bit
GAIN : Input Span Bit
NAP  : Nap Mode Bit
SLP  : Sleep Mode Bit
Dx   : Data Bits
X    : Don't care


Example Code:

Read Channel 0 in Single-Ended Unipolar mode when input is with respect to GND

    adc_command = LTC2308_CH0 | LTC2308_UNIPOLAR_MODE | LTC2308_LOW_GAIN_MODE | LTC2308_NORMAL_MODE;     // Single-ended, CH0, unipolar, low gain, normal mode.
    LTC2308_read(LTC2308_CS, adc_command, &adc_code);   // Throws out last reading
    LTC2308_read(LTC2308_CS, adc_command, &adc_code);   // Obtains the current reading and stores to adc_code variable

    // Convert adc_code to voltage
    adc_voltage = LTC2308_unipolar_code_to_voltage(adc_code, LTC2308_lsb, LTC2308_offset_unipolar_code);

@endverbatim

http://www.linear.com/product/LTC2308

http://www.linear.com/product/LTC2308#demoboards

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
    @ingroup LTC2308
    Header for LTC2308: 16-bit 8-channel 100ksps ADC
*/

#ifndef LTC2308_H
#define LTC2308_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC2308_CS
#define LTC2308_CS QUIKEVAL_CS
#endif

//! @name LTC2308 Single-Ended Channel Addresses
//! @{
// Single-Ended Channel Address
#define LTC2308_CH0             0x80
#define LTC2308_CH1             0xC0
#define LTC2308_CH2             0x90
#define LTC2308_CH3             0xD0
#define LTC2308_CH4             0xA0
#define LTC2308_CH5             0xE0
#define LTC2308_CH6             0xB0
#define LTC2308_CH7             0xF0
//!@}

//! @name LTC2308 Differential Channel Addresses
//! @{
// Differential Channel Address
#define LTC2308_P0_N1           0x00
#define LTC2308_P1_N0           0x40

#define LTC2308_P2_N3           0x10
#define LTC2308_P3_N2           0x50

#define LTC2308_P4_N5           0x20
#define LTC2308_P5_N4           0x60

#define LTC2308_P6_N7           0x30
#define LTC2308_P7_N6           0x70
//!@}

//! @name LTC2308 Uni/GAIN config bits
//! @{
// Unipolar Mode Command
#define LTC2308_UNIPOLAR_MODE   0x08
#define LTC2308_BIPOLAR_MODE    0x00
// Single-Ended Mode Command
#define LTC2308_SINGLE_ENDED_MODE   0x80
#define LTC2308_DIFFERENTIAL_MODE  0x00
//!@}

//! @name LTC2308 Sleep/Nap config bits
//! @{
// Sleep Mode Command
#define LTC2308_SLEEP_MODE      0x01
#define LTC2308_NORMAL_MODE     0x00
// Nap Mode Command
#define LTC2308_NAP_MODE      0x02
//!@}

/*
  Example command
  adc_command = LTC2308_CH0 | LTC2308_UNIPOLAR_MODE | LTC2308_LOW_GAIN_MODE | LTC2308_NORMAL_MODE;     // Single-ended, CH0, unipolar, low gain, normal mode.
 */


//! Reads the ADC and returns 16-bit data
//! @return void
void LTC2308_read(uint8_t cs,           //!< Chip Select Pin
                  uint8_t adc_command,  //!< Channel address, config bits ORed together
                  uint16_t *adc_code    //!< Returns code read from ADC (from previous conversion)
                 );


//! Calculates the LTC2308 input's unipolar voltage given the binary data and lsb weight.
//! @return Floating point voltage
float LTC2308_code_to_voltage(uint16_t adc_code,                  //!< Raw ADC code
                              float vref,     //!<
                              uint8_t uni_bipolar        //!<
                             );

#endif  //  LTC2308_H