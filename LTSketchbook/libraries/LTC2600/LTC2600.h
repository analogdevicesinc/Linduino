/*!
LTC2600: Octal 16-Bit Rail-to-Rail DACs in 16-Lead SSOP

@verbatim

The LTC2600/LTC2610/LTC2620 are octal 16-, 14- and 12-bit, 2.5V-to-5.5V
rail-to-rail voltage-output DACs in 16-lead narrow SSOP and 20-lead 4mm × 5mm
QFN packages. They have built-in high performance output buffers and are
guaranteed monotonic.

These parts establish advanced performance standards for output drive,
crosstalk and load regulation in single-supply, voltage output multiples.

SPI DATA FORMAT (MSB First):

24-Bit Load Sequence:

             Byte #1                   Byte #2                         Byte #3
             Command                   MSB                             LSB
LTC2600-16 : C3 C2 C1 C0 A3 A2 A1 A0   D15 D14 D13 D12 D11 D10 D9 D8   D7 D6 D5 D4 D3 D2 D1 D0
LTC2610-14 : C3 C2 C1 C0 A3 A2 A1 A0   D13 D12 D11 D10 D9  D8  D7 D6   D5 D4 D3 D2 D1 D0 X  X
LTC2620-12 : C3 C2 C1 C0 A3 A2 A1 A0   D11 D10 D9  D8  D7  D6  D5 D4   D3 D2 D1 D0 X  X  X  X

32-Bit Load Sequence:

             Byte #1                   Byte #2                   Byte #3                         Byte #4
             Command                   MSB                                                       LSB
LTC2600-16 : X  X  X  X  X  X  X  X    C3 C2 C1 C0 A3 A2 A1 A0   D15 D14 D13 D12 D11 D10 D9 D8   D7 D6 D5 D4 D3 D2 D1 D0
LTC2610-14 : X  X  X  X  X  X  X  X    C3 C2 C1 C0 A3 A2 A1 A0   D13 D12 D11 D10 D9  D8  D7 D6   D5 D4 D3 D2 D1 D0 X  X
LTC2620-12 : X  X  X  X  X  X  X  X    C3 C2 C1 C0 A3 A2 A1 A0   D11 D10 D9  D8  D7  D6  D5 D4   D3 D2 D1 D0 X  X  X  X

Cx   : DAC Command Code
Ax   : DAC Address (0=DACA, 1=DACB, 2=DACC, 3=DACD, 0xFF=All DACs)
Dx   : DAC Data Bits
X    : Don't care


Example Code:

Set DAC A to to 2V for 16-bit DAC.

    shift_count = 0;    // 16-bit DAC does not have to be shifted
    dac_voltage = 2.0;  // Sets dac voltage variable to 2v

    dac_code = LTC2600_voltage_to_code(dac_voltage, LTC2600_lsb, LTC2600_offset);       // Calculate DAC code from voltage, lsb, and offset
    LTC2600_write(LTC2600_CS, LTC2600_CMD_WRITE_UPDATE, LTC2600_DAC_A, dac_code << shift_count);    // Set DAC A with DAC code

@endverbatim


http://www.linear.com/product/LTC2600

http://www.linear.com/product/LTC2600#demoboards



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
  @ingroup LTC2600
  Header for LTC2600: Octal 16-Bit Rail-to-Rail DACs in 16-Lead SSOP
*/


#ifndef LTC2600_H
#define LTC2600_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC2600_CS
#define LTC2600_CS QUIKEVAL_CS
#endif


//! @name LTC2600 Command Codes
//!@{
//! OR'd together with the DAC address to form the command byte
#define  LTC2600_CMD_WRITE               0x00  //!< Write to input register n
#define  LTC2600_CMD_UPDATE              0x10  //!< Update (power up) DAC register n
#define  LTC2600_CMD_WRITE_UPDATE_ALL    0x20  //!< Write to input register n, update (power up) all
#define  LTC2600_CMD_WRITE_UPDATE        0x30  //!< Write to input register n, update (power up) all
#define  LTC2600_CMD_POWER_DOWN          0x40  //!< Power down n
#define  LTC2600_CMD_NO_OPERATION        0xF0  //!< No operation
//!@}

/*!
| DAC Addresses                | Value    |
| :--------------------------: | :------: |
|  LTC2605_DAC_A               |  0x00    |
|  LTC2605_DAC_B               |  0x01    |
|  LTC2605_DAC_C               |  0x02    |
|  LTC2605_DAC_D               |  0x03    |
|  LTC2605_DAC_E               |  0x04    |
|  LTC2605_DAC_F               |  0x05    |
|  LTC2605_DAC_G               |  0x06    |
|  LTC2605_DAC_H               |  0x07    |
|  LTC2605_DAC_ALL             |  0x0F    |
*/
/*! @name Dac Addresses
@{*/

// Which DAC to operate on
#define  LTC2600_DAC_A     0x00
#define  LTC2600_DAC_B     0x01
#define  LTC2600_DAC_C     0x02
#define  LTC2600_DAC_D     0x03
#define  LTC2600_DAC_E     0x04
#define  LTC2600_DAC_F     0x05
#define  LTC2600_DAC_G     0x06
#define  LTC2600_DAC_H     0x07
#define  LTC2600_DAC_ALL   0x0F
//

//! Write the 16-bit dac_code to the LTC2600
//! @return Void
void LTC2600_write(uint8_t cs,                            //!< Chip Select Pin
                   uint8_t dac_command,                   //!< Command Nibble, left-justified, lower nibble set to zero
                   uint8_t dac_address,                   //!< DAC Address Nibble, right justified, upper nibble set to zero
                   uint16_t dac_code                      //!< 16-bit DAC code
                  );

//! Calculate a LTC2600 DAC code given the desired output voltage and DAC address (0-3)
//! @return The 16-bit code to send to the DAC
uint16_t LTC2600_voltage_to_code(float dac_voltage,       //!< Voltage to send to DAC
                                 float LTC2600_lsb,       //!< LSB value (volts)
                                 int16_t LTC2600_offset   //!< Offset (volts)
                                );

//! Calculate the LTC2600 DAC output voltage given the DAC code, offset, and LSB value
//! @return Calculated voltage
float LTC2600_code_to_voltage(uint16_t dac_code,          //!< DAC code
                              float LTC2600_lsb,          //!< LSB value (volts)
                              int16_t LTC2600_offset      //!< Offset (volts)
                             );

#endif //LTC2600_H