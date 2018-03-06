/*!
LTC2418: 16-Channel 24-Bit No Latency Delta-Sigma ADC

@verbatim

The LTC2414/LTC2418 are 8-/16-channel (4-/8-differential) micropower 24-bit
delta-sigma analog-to-digital converters. They operate from 2.7V to 5.5V and
include an integrated oscillator, 2ppm INL and 0.2ppm RMS noise. They use
delta-sigma technology and provide single cycle settling time for multiplexed
applications. Through a single pin, the LTC2414/LTC2418 can be configured for
better than 110dB differential mode rejection at 50Hz or 60Hz +/-2%, or they can
be driven by an external oscillator for a user-defined rejection frequency. The
internal oscillator requires no external frequency setting components.

SPI DATA FORMAT (MSB First):

            Byte #1                            Byte #2                           Byte #3                    Byte #4

Data Out :  !EOC DMY SIG D22 D21 D20 D19 D18   D17 D16 D15 D14 D13 D12 D11 D10   D9 D8 D7 D6 D5 D4 D3  D2   D1 D0 SGL OS S2 S1 S0 PAR
Data In  :  1    0   EN  SGL OS  S2  S1  S0    X   X   X   X   X   X   X   X     X  X  X  X  X  X   X  X    X  X  X   X  X  X  X  X

!EOC : End of Conversion Bit (Active Low)
DMY  : Dummy Bit (Always 0)
SIG  : Sign Bit (1-data positive, 0-data negative)
Dx   : Data Bits
EN   : Enable Bit (0-keep previous mode, 1-change mode)
SGL  : Enable Single-Ended Bit (0-differential, 1-single-ended)
OS   : ODD/Sign Bit
Sx   : Address Select Bit
PAR  : Parity Bit

Command Byte
1    0    EN   SGL  OS   S2   S1   S0   Comments
1    0    0    X    X    X    X    X    Keep Previous Mode
1    0    1    0    X    X    X    X    Differential Mode
1    0    1    1    X    X    X    X    Single-Ended Mode


Example Code:

Read Channel 0 in Single-Ended mode

    uint16_t miso_timeout = 1000;
    adc_command = LTC2418_CH0;                          // Build ADC command for channel 0

    if(LTC2418_EOC_timeout(LTC2418_CS, miso_timeout))    // Check for EOC
        return(1);
    LTC2418_read(LTC2418_CS, adc_command, &adc_code);   // Throws out last reading

    if(LTC2418_EOC_timeout(LTC2418_CS, miso_timeout))    // Check for EOC
        return(1);
    LTC2418_read(LTC2418_CS, adc_command, &adc_code);   // Obtains the current reading and stores to adc_code variable

    // Convert adc_code to voltage
    adc_voltage = LTC2418_code_to_voltage(adc_code, LTC2418_lsb , LTC2418_offset_code);

@endverbatim

http://www.linear.com/product/LTC2418

http://www.linear.com/product/LTC2418#demoboards


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
    @ingroup LTC2418
    Header for LTC2418: 16-Channel 24-Bit No Latency Delta-Sigma ADC
*/

#ifndef LTC2418_H
#define LTC2418_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC2418_CS
#define LTC2418_CS QUIKEVAL_CS
#endif

//! @name LTC2418 Single-Ended configuration bytes
//! @{
//! Address bytes with enable sequence prepended
#define LTC2418_CH0            0xB0
#define LTC2418_CH1            0xB8
#define LTC2418_CH2            0xB1
#define LTC2418_CH3            0xB9
#define LTC2418_CH4            0xB2
#define LTC2418_CH5            0xBA
#define LTC2418_CH6            0xB3
#define LTC2418_CH7            0xBB
#define LTC2418_CH8            0xB4
#define LTC2418_CH9            0xBC
#define LTC2418_CH10           0xB5
#define LTC2418_CH11           0xBD
#define LTC2418_CH12           0xB6
#define LTC2418_CH13           0xBE
#define LTC2418_CH14           0xB7
#define LTC2418_CH15           0xBF
//! @}

//! @name LTC2418 Differential configuration bytes
//! @{
//! Address bytes with enable sequence prepended
#define LTC2418_P0_N1          0xA0
#define LTC2418_P1_N0          0xA8

#define LTC2418_P2_N3          0xA1
#define LTC2418_P3_N2          0xA9

#define LTC2418_P4_N5          0xA2
#define LTC2418_P5_N4          0xAA

#define LTC2418_P6_N7          0xA3
#define LTC2418_P7_N6          0xAB

#define LTC2418_P8_N9          0xA4
#define LTC2418_P9_N8          0xAC

#define LTC2418_P10_N11        0xA5
#define LTC2418_P11_N10        0xAD

#define LTC2418_P12_N13        0xA6
#define LTC2418_P13_N12        0xAE

#define LTC2418_P14_N15        0xA7
#define LTC2418_P15_N14        0xAF
//! @}

//! Checks for EOC with a specified timeout
//! @return 0=successful, 1=unsuccessful (exceeded timeout)
int8_t LTC2418_EOC_timeout(uint8_t cs,           //!< Chip Select pin
                           uint16_t miso_timeout //!< Timeout (ms)
                          );

//! Reads the LTC2418 result and programs the configuration for the next conversion.
//! @return void
void LTC2418_read(uint8_t cs,           //!< Chip Select pin
                  uint8_t adc_command,  //!< Command byte
                  uint32_t *adc_code    //!< Returns raw 32-bit code read from ADC
                 );

//! Calculates the LTC2418 input bipolar voltage
//! @return Calculated voltage
float LTC2418_code_to_voltage(int32_t adc_code,           //!< Raw ADC code
                              float LTC2418_lsb,          //!< LSB value (volts)
                              int32_t LTC2418_offset_code //!< Offset (Code)
                             );

//! Calibrate the lsb
//! @return Void
void LTC2418_cal_voltage(int32_t zero_code,               //!< Offset (Code)
                         int32_t fs_code,                 //!< Code measured with full-scale input applied
                         float zero_voltage,              //!< Measured zero voltage
                         float fs_voltage,                //!< Measured full-scale voltage
                         float *LTC2418_lsb,              //!< Returns LSB value (volts)
                         int32_t *LTC2418_offset_code     //!< Returns Offset (Code)
                        );

#endif  // LTC2418_H
