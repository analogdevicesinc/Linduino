/*!
LTC6603: Dual, Matched, High Frequency Bandpass/Lowpass Filters

@verbatim

The LTC6603 is a dual, matched, programmable bandpass or lowpass filter
and differential driver. The selectivity of the LTC6603, combined with its
phase matching and dynamic range, make it ideal for filtering in RFID
systems. With two degree phase matching between channels, the LTC6603 can
be used in applications requiring highly matched filters, such as
transceiver I and Q channels. Gain programmability, and the fully
differential inputs and outputs, simplify implementation in most systems.

SPI DATA FORMAT (MSB First):

            Byte #1                                      Byte #2

Data In  :  GAIN0 GAIN1 LPF0 LPF1 NOFUNC0 NOFUNC1 SHDN OUT     GAIN0 GAIN1 LPF0 LPF1 NOFUNC0 NOFUNC1 SHDN OUT
            |---- GAIN, BW CONTROL WORD FOR #2 ----|     |---- GAIN, BW CONTROL WORD FOR #1 ----|

GAINx : Gain Control Bits
LPFx  : Lowpass Cutoff Frequency Bits
NOFUNCx  : Highpass Cutoff Frequency Bits
SHDN  : Shutdown Bit
OUT   : General Purpose Output Pin

@endverbatim


http://www.analog.com/en/products/amplifiers/adc-drivers/fully-differential-amplifiers/ltc6603.html

http://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/dc1304a-b.html



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
    @ingroup LTC6603
    Header for LTC6603: Dual Matched, High Frequency Bandpass/Lowpass Filters
*/

#ifndef LTC6603_H
#define LTC6603_H

#include <SPI.h>

//! Define external frequency range
#define LTC6603_FREQ_MIN        12 //!<12 MHz  
#define LTC6603_FREQ_MAX        80 //!<80 Mhz

//! Define the SPI CS pin
#ifndef LTC6603_CS
#define LTC6603_CS QUIKEVAL_CS
#endif

//! @name LTC6603 gain configuration bits.
//! @{
//! Refer to Table 1 in Data Sheet
#define LTC6603_GAIN_0dB        0x00
#define LTC6603_GAIN_6dB        0x80
#define LTC6603_GAIN_12dB       0x40
#define LTC6603_GAIN_24dB       0xC0
#define LTC6603_GAIN_MASK   0x3F
//! @}


//! @name LTC6603 lowpass cutoff frequency configuration bits.
//! @{
//! Refer to Table 2 and 3 for Bandwidth
#define LTC6603_LPF0           0x00
#define LTC6603_LPF1           0x20
#define LTC6603_LPF2           0x10
#define LTC6603_LPF3           0x30
#define LTC6603_LPF_MASK     0xCF
//! @}


//! @name LTC6603 Highpass cutoff frequency configuration bits.
//! @{
//! Refer to Table 2 and 3 for Bandwidth
#define LTC6603_NOFUNC0           0x00
#define LTC6603_NOFUNC1           0x08
#define LTC6603_NOFUNC2           0x04
#define LTC6603_NOFUNC3           0x0C
#define LTC6603_NOFUNC_MASK     0xF3
//! @}


//! @name LTC6603 shutdown/powerup and GPO configuration bits.
#define LTC6603_SHDN           0x02 //!< Shuts down LTC6603
#define LTC6603_PRUP           0x00 //!< Turns on LTC6603
#define LTC6603_ONOFF_MASK     0xFD
#define LTC6603_GPO_HIGH       0x01 //!< Sets the general purpose 
#define LTC6603_GPO_LOW        0x00 //!<
#define LTC6603_GPO_MASK     0xFE


//! Sends Data to the LTC6603
//! @return void
void LTC6603_write(uint8_t cs,      //!< Chip Select pin
                   uint8_t *tx,     //!< Byte array to be transmitted
                   uint8_t length   //!< Length of array
                  );

#endif  
