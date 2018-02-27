/*!
LTC2756: Serial 18-Bit SoftSpan IOUT DAC

@verbatim

The LTC2756 is an 18-bit multiplying serial-input,
current-output digital-to-analog converter. LTC2756A
provides full 18-bit performance-INL and DNL of +/-1LSB
maximum-over temperature without any adjustments. 18-bit
monotonicity is guaranteed in all performance grades. This
SoftSpan(TM) DAC operates from a single 3V to 5V supply
and offers six output ranges (up to +/-10V) that can be
programmed through the 3-wire SPI serial interface or
pin-strapped for operation in a single range.

SPI DATA FORMAT (MSB First):

Transfer Sequence:
      Byte #1                    Byte #2
MOSI: C3 C2 C1 C0 0  0  0  0   D17 D16 D15 D14 D13 D12 D11 D10
MISO: 0  0  0  0  0  0  0  0   D17 D16 D15 D14 D13 D12 D11 D10

      Byte #3                    Byte #4
MOSI: D9 D8 D7 D6 D5 D4 D3 D2   D1  D0  X  X  X  X  X  X
MISO: D9 D8 D7 D6 D5 D4 D3 D2   D1  D0  0  0  0  0  0  0

Write SPAN Sequence:
      Byte #1                    Byte #2
MOSI: C3 C2 C1 C0 0  0  0  0   X  X  X  X  X  X  X  X
MISO: 0  0  0  0  0  0  0  0   0  0  0  0  0  0  0  0

      Byte #3                    Byte #4
MOSI: X  X  X  X  S3 S2 S1 S0   X  X  X  X  X  X  X  X
MISO: 0  0  0  0  S3 S2 S1 S0   0  0  0  0  0  0  0  0


Cx   : DAC Command bits
Sx   : SPAN bits
Dx   : DAC Data Bits
X    : Don't care

Example Code:

Set DAC to -5v to +5v  and set output to 2.5v

uint8_t tx[4],rx[4];

// Set DAC to -5v to +5v
tx[3] = LTC2756_WRITE_SPAN;
tx[2] = 0x00;
tx[1] = LTC2756_BIPOLAR_N5_P5;
tx[0] = 0x00;

LTC2756_transfer_4bytes(LTC2756_CS, tx, rx);

float voltage = 2.5;
float LTC2756_lsb = 3.8146972656e-5;            // Typical lsb for -5V to +5v bipolar mode only
int32_t LTC2756_offset = 0x20000;               // Typical offset for -5V to +5v bipolar mode only

// Convert voltage to code
uint32_t code;
code = LTC2756_voltage_to_code(voltage, LTC2756_lsb, LTC2756_offset);

uint8_t tx[4],rx[4];
union
{
    uint8_t byte[4];
    uint32_t code;
} data;

// Load tx array with write command and code
data.code = code<<6;
tx[3] = LTC2756_WRITE_CODE;
tx[2] = data.byte[2];
tx[1] = data.byte[1];
tx[0] = data.byte[0];

LTC2756_transfer_4bytes(LTC2756_CS, tx, rx);

// Load tx arra with update DAC command
tx[3] = LTC2756_UPDATE;
tx[2] = 0x00;
tx[1] = 0x00;
tx[0] = 0x00;

LTC2756_transfer_4bytes(LTC2756_CS, tx, rx);

@endverbatim

http://www.linear.com/product/LTC2756

http://www.linear.com/product/LTC2756#demoboards

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
    @ingroup LTC2756
    Library Header File for LTC2756: Serial 18-Bit SoftSpan IOUT DAC
*/

#ifndef LTC2756_H
#define LTC2756_H

//! Define the CS pin
#ifndef LTC2756_CS
#define LTC2756_CS QUIKEVAL_CS  //! SPI Chip Select Pin
#endif

#include <SPI.h>

//! @name LTC2756 DAC Cammands
//! @{
#define LTC2756_WRITE_SPAN              0x20
#define LTC2756_WRITE_CODE              0x30
#define LTC2756_UPDATE                  0x40
#define LTC2756_WRITE_SPAN_UPDATE       0x60
#define LTC2756_WRITE_CODE_UPDATE       0x70
#define LTC2756_READ_INPUTE_SPAN_REG    0xA0
#define LTC2756_READ_INPUT_CODE_REG     0xB0
#define LTC2756_READ_DAC_SPAN_REG       0xC0
#define LTC2756_READ_DAC_CODE_REG       0xD0
//! @}

//! @name LTC2756 SPAN Options
//! @{
#define LTC2756_UNIPOLAR_0_P5       0x00
#define LTC2756_UNIPOLAR_0_P10      0x01
#define LTC2756_BIPOLAR_N5_P5       0x02
#define LTC2756_BIPOLAR_N10_P10     0x03
#define LTC2756_BIPOLAR_N2_5_P2_5   0x04
#define LTC2756_BIPOLAR_N2_5_P7_5   0x05
//! @}

//! Transfers four bytes to the LTC2756
//! @return void
void LTC2756_transfer_4bytes(uint8_t cs,
                             uint8_t *tx,
                             uint8_t *rx
                            );

//! Converts voltage to code
//! @return the calculated code
uint32_t LTC2756_voltage_to_code(float dac_voltage,
                                 float LTC2756_lsb,
                                 int32_t LTC2756_offset
                                );
//! Calculate the LTC2756 offset and LSB voltage given two measured voltages and their corresponding codes
//! @returns void
void LTC2756_calibrate(uint32_t zero_code,
                       uint32_t fs_code,
                       float zero_voltage,
                       float fs_voltage,
                       float *LTC2756_lsb,
                       int32_t *LTC2756_offset
                      );

#endif