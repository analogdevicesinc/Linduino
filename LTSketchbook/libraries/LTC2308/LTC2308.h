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