/*!
LTC6602: Dual, Matched, High Frequency Bandpass/Lowpass Filters

@verbatim

The LTC6602 is a dual, matched, programmable bandpass or lowpass filter
and differential driver. The selectivity of the LTC6602, combined with its
phase matching and dynamic range, make it ideal for filtering in RFID
systems. With two degree phase matching between channels, the LTC6602 can
be used in applications requiring highly matched filters, such as
transceiver I and Q channels. Gain programmability, and the fully
differential inputs and outputs, simplify implementation in most systems.

SPI DATA FORMAT (MSB First):

            Byte #1                                      Byte #2

Data In  :  GAIN0 GAIN1 LPF0 LPF1 HPF0 HPF1 SHDN OUT     GAIN0 GAIN1 LPF0 LPF1 HPF0 HPF1 SHDN OUT
            |---- GAIN, BW CONTROL WORD FOR #2 ----|     |---- GAIN, BW CONTROL WORD FOR #1 ----|

GAINx : Gain Control Bits
LPFx  : Lowpass Cutoff Frequency Bits
HPFx  : Highpass Cutoff Frequency Bits
SHDN  : Shutdown Bit
OUT   : General Purpose Output Pin

@endverbatim

http://www.linear.com/product/LTC6602

http://www.linear.com/product/LTC6602#demoboards

REVISION HISTORY
$Revision: 4047 $
$Date: 2015-09-22 17:01:37 -0700 (Tue, 22 Sep 2015) $

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
    @ingroup LTC6602
    Header for LTC6602: Dual Matched, High Frequency Bandpass/Lowpass Filters
*/

#ifndef LTC6602_H
#define LTC6602_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC6602_CS
#define LTC6602_CS QUIKEVAL_CS
#endif

//! @name LTC6602 gain configuration bits.
//! @{
//! Refer to Table 1 in Data Sheet
#define LTC6602_GAIN_0dB        0x00
#define LTC6602_GAIN_12dB       0x80
#define LTC6602_GAIN_24dB       0x40
#define LTC6602_GAIN_30dB       0xC0
#define LTC6602_GAIN_MASK   0x3F
//! @}


//! @name LTC6602 lowpass cutoff frequency configuration bits.
//! @{
//! Refer to Table 2 and 3 for Bandwidth
#define LTC6602_LPF0           0x00
#define LTC6602_LPF1           0x20
#define LTC6602_LPF2           0x10
#define LTC6602_LPF3           0x30
#define LTC6602_LPF_MASK     0xCF
//! @}


//! @name LTC6602 Highpass cutoff frequency configuration bits.
//! @{
//! Refer to Table 2 and 3 for Bandwidth
#define LTC6602_HPF0           0x00
#define LTC6602_HPF1           0x08
#define LTC6602_HPF2           0x04
#define LTC6602_HPF3           0x0C
#define LTC6602_HPF_MASK     0xF3
//! @}


//! @name LTC6602 shutdown/powerup and GPO configuration bits.
#define LTC6602_SHDN           0x02 //!< Shuts down LTC6602
#define LTC6602_PRUP           0x00 //!< Turns on LTC6602
#define LTC6602_ONOFF_MASK     0xFD
#define LTC6602_GPO_HIGH       0x01 //!< Sets the general purpose 
#define LTC6602_GPO_LOW        0x00 //!<
#define LTC6602_GPO_MASK     0xFE


//! Sends Data to the LTC6602
//! @return void
void LTC6602_write(uint8_t cs,      //!< Chip Select pin
                   uint8_t *tx,     //!< Byte array to be transmitted
                   uint8_t length   //!< Length of array
                  );

#endif  // LTC6602_H
