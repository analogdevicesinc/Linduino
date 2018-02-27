/*!
LTC1859: 16-bit 8-channel 100ksps ADC

@verbatim

The LTC1857/1858/LTC1859 are pin-compatible, 8-channel 12-/14-/16-bit A/D converters
with serial I/O, and an internal reference. The ADCs typically draw only 1.3mA
from a single 5V supply. The 8-channel input multiplexer can be configured for
either single-ended or differential inputs and unipolar or bipolar conversions
(or combinations thereof). The automatic nap and sleep modes benefit power
sensitive applications.

The LTC1859's DC performance is outstanding with a +/-2LSB INL specification and
no missing codes over temperature. The signal-to-noise ratio (SNR) for the
LTC1859 is typically 89dB, with the internal reference.

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

    adc_command = LTC1859_CH0 | LTC1859_UNIPOLAR_MODE | LTC1859_LOW_GAIN_MODE | LTC1859_NORMAL_MODE;     // Single-ended, CH0, unipolar, low gain, normal mode.
    LTC1859_read(LTC1859_CS, adc_command, &adc_code);   // Throws out last reading
    LTC1859_read(LTC1859_CS, adc_command, &adc_code);   // Obtains the current reading and stores to adc_code variable

    // Convert adc_code to voltage
    adc_voltage = LTC1859_unipolar_code_to_voltage(adc_code, LTC1859_lsb, LTC1859_offset_unipolar_code);

@endverbatim

http://www.linear.com/product/LTC1859

http://www.linear.com/product/LTC1859#demoboards


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
    @ingroup LTC1859
    Header for LTC1859: 16-bit 8-channel 100ksps ADC
*/

#ifndef LTC1859_H
#define LTC1859_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC1859_CS
#define LTC1859_CS QUIKEVAL_CS
#endif

//! @name LTC1859 Single-Ended Channel Addresses
//! @{
// Single-Ended Channel Address
#define LTC1859_CH0             0x80
#define LTC1859_CH1             0xC0
#define LTC1859_CH2             0x90
#define LTC1859_CH3             0xD0
#define LTC1859_CH4             0xA0
#define LTC1859_CH5             0xE0
#define LTC1859_CH6             0xB0
#define LTC1859_CH7             0xF0
//!@}

//! @name LTC1859 Differential Channel Addresses
//! @{
// Differential Channel Address
#define LTC1859_P0_N1           0x00
#define LTC1859_P1_N0           0x40

#define LTC1859_P2_N3           0x10
#define LTC1859_P3_N2           0x50

#define LTC1859_P4_N5           0x20
#define LTC1859_P5_N4           0x60

#define LTC1859_P6_N7           0x30
#define LTC1859_P7_N6           0x70
//!@}

//! @name LTC1859 Uni/GAIN config bits
//! @{
// Unipolar Mode Command
#define LTC1859_UNIPOLAR_MODE   0x08
#define LTC1859_BIPOLAR_MODE    0x00
// Single-Ended Mode Command
#define LTC1859_SINGLE_ENDED_MODE   0x80
#define LTC1859_DIFFERENTIAL_MODE  0x00
// Gain Mode Command
#define LTC1859_LOW_RANGE_MODE   0x00
#define LTC1859_HIGH_RANGE_MODE  0x04
//!@}

//! @name LTC1859 Sleep/Nap config bits
//! @{
// Sleep Mode Command
#define LTC1859_SLEEP_MODE      0x01
#define LTC1859_NORMAL_MODE     0x00
// Nap Mode Command
#define LTC1859_NAP_MODE      0x02
//!@}

/*
  Example command
  adc_command = LTC1859_CH0 | LTC1859_UNIPOLAR_MODE | LTC1859_LOW_GAIN_MODE | LTC1859_NORMAL_MODE;     // Single-ended, CH0, unipolar, low gain, normal mode.
 */


//! Builds the ADC command and returns an 8 bit command
//! @return  8 bit command
uint8_t LTC1859_build_command(uint8_t ch_designate,
                              uint8_t uni_bipolar,
                              uint8_t range_low_high
                             );


//! Reads the ADC and returns 16-bit data
//! @return void
void LTC1859_read(uint8_t cs,           //!< Chip Select Pin
                  uint8_t adc_command,  //!< Channel address, config bits ORed together
                  uint16_t *adc_code    //!< Returns code read from ADC (from previous conversion)
                 );


//! Calculates the LTC1859 input's unipolar voltage given the binary data and lsb weight.
//! @return Floating point voltage
float LTC1859_code_to_voltage(uint16_t adc_code,                   //!< Raw ADC code
                              float vref,
                              uint8_t range_low_high,       //!<)
                              uint8_t uni_bipolar  //!<
                             );

#endif  //  LTC1859_H