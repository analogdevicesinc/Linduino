/*!
LTC2752: Dual Serial 16-Bit SoftSpan IOUT DAC

@verbatim

The LTC2752 is a dual, current-output, serial-input precision multiplying DAC
with selectable output ranges. Ranges can either be programmed in software for
maximum flexibility (each of the DACs can be programmed to any one of six output
ranges) or hardwired through pin-strapping. Two unipolar ranges are available
(0V to 5V and 0V to 10V), and four bipolar ranges (±2.5V, ±5V, ±10V and –2.5V
to 7.5V). These ranges are obtained when an external precision 5V reference is
used. The output ranges for other reference voltages are easy to calculate by
observing that each range is a multiple of the external reference voltage. The
ranges can then be expressed: 0 to 1×, 0 to 2×, ±0.5×, ±1×, ±2×, and –0.5× to 1.5×.

The LTC2752 has 5 internal registers for each DAC, a total of 10 registers (see
Block Diagram). Each DAC channel has two sets of double-buffered registers, one
set for the code data, and one for the output range of the DAC, plus one readback
register.

SPI DATA FORMAT (MSB First):

                       Byte #1                         Byte #2                         Byte #3
Data In (Write code):  C3  C2  C1  C0  A3  A2  A1  A0  D15 D14 D13 D12 D11 D10 D9  D8  D7  D6  D5  D4  D3  D2  D1  D0
Data In (Wrire Span):  C3  C2  C1  C0  A3  A2  A1  A0  X   X   X   X   X   X   X   X   X   X   X   X   S3  S2  S1  S0

Cx   : Command Code Bits
Ax   : Address Code Bits
Sx   : Span Select Bits
Dx   : Data Bits
X    : Don't care

Example Code:

  LTC2752_write(LTC2752_CS, LTC2752_WRITE_SPAN_DAC, DAC_SELECTED, span);
  data = LTC2752_voltage_to_code(voltage, DAC_RANGE_LOW, DAC_RANGE_HIGH);
  LTC2752_write(LTC2752_CS, LTC2752_WRITE_CODE_UPDATE_DAC, DAC_SELECTED, data);

@endverbatim

http://www.linear.com/product/LTC2752

http://www.linear.com/product/LTC2752#demoboards


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
    @ingroup LTC2752
    Header for LTC2752: Dual Serial 16-Bit SoftSpan IOUT DAC
*/

#ifndef LTC2752_H
#define LTC2752_H

//! Define the SPI CS pin
#ifndef LTC2752_CS
#define LTC2752_CS QUIKEVAL_CS
#endif

//! @name LTC2752 DAC Addresses
#define ADDRESS_DACA        0x00
#define ADDRESS_DACB        0x02
#define ADDRESS_DAC_ALL     0x0E
//! @}

//! @name LTC2752 Command Codes
//! OR'd together with the DAC address to form the command byte
#define  LTC2752_WRITE_SPAN_DAC           0x20  //!< Write Span DAC n
#define  LTC2752_WRITE_CODE_DAC           0x30  //!< Write Code DAC n
#define  LTC2752_UPDATE_DAC               0x40  //!< Update DAC n
#define  LTC2752_UPDATE_ALL               0x50  //!< Update All DACs 
#define  LTC2752_WRITE_SPAN_UPDATE_DAC    0x60  //!< Write Span DAC n and Update DAC n
#define  LTC2752_WRITE_CODE_UPDATE_DAC    0x70  //!< Write Code DAC n and Update DAC n

#define  LTC2752_WRITE_SPAN_UPDATE_ALL    0x80  //!< Write Span DAC n and Update All DACs
#define  LTC2752_WRITE_CODE_UPDATE_ALL    0x90  //!< Write Code DAC n and Update All DACs
#define  LTC2752_READ_INPUT_SPAN_REG      0xA0  //!< Read Input Span Register DAC n
#define  LTC2752_READ_INPUT_CODE_REG      0xB0  //!< Read Input Code Register DAC n
#define  LTC2752_READ_DAC_SPAN_REG        0xC0  //!< Read DAC Span Register DAC n
#define  LTC2752_READ_DAC_CODE_REG        0xD0  //!< Read DAC Code Register DAC n
#define  LTC2752_PREVIOUS_CMD             0xF0  //!< Set by previous command
//! @}

//! Transmits 24 bit input stream: 4-bit command + 4-bit don't-care + 16-bit data
//! @return void
void LTC2752_write(uint8_t cs, uint8_t dac_command, uint8_t dac_address, uint16_t data);

//! Calculate the LTC2752 DAC output voltage given the DAC code and and the minimum / maximum
//! outputs for a given softspan range.
//! @return Floating point voltage
float LTC2752_code_to_voltage(uint16_t dac_code, float min_output, float max_output);

//! Calculate a LTC2752 DAC code given the desired output voltage and the minimum / maximum
//! outputs for a given softspan range.
//! @return 16 bit data
uint16_t LTC2752_voltage_to_code(float dac_voltage, float min_output, float max_output);

#endif  //  LTC2752_H