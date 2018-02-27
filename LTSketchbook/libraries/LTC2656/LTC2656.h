/*!
LTC2656: Octal SPI 16-/12-Bit Rail-to-Rail DACs with 10ppm/C Max Reference

@verbatim

The LTC2656 is a family of octal 16-/12-bit rail-to-rail DACs with a precision
integrated reference. The DACs have built-in high performance, rail-to-rail,
output buffers and are guaranteed monotonic. The LTC2656-L has a full-scale
output of 2.5V with the integrated 10ppm/C reference and operates from a single
2.7V to 5.5V supply. The LTC2656-H has a full-scale output of 4.096V with the
integrated reference and operates from a 4.5V to 5.5V supply. Each DAC can also
operate with an external reference, which sets the DAC full-scale output to two
times the external reference voltage.

These DACs communicate via a SPI/MICROWIRE compatible 4-wire serial interface
which operates at clock rates up to 50MHz. The LTC2656 incorporates a power-on
reset circuit that is controlled by the PORSEL pin. If PORSEL is tied to GND the
DACs reset to zero-scale. If PORSEL is tied to VCC, the DACs reset to mid-scale.

SPI DATA FORMAT (MSB First):

24-Bit Load Sequence:

             Byte #1                   Byte #2                         Byte #3
             Command                   MSB                             LSB
LTC2656-16 : C3 C2 C1 C0 A3 A2 A1 A0   D15 D14 D13 D12 D11 D10 D9 D8   D7 D6 D5 D4 D3 D2 D1 D0
LTC2656-12 : C3 C2 C1 C0 A3 A2 A1 A0   D11 D10 D9  D8  D7  D6  D5 D4   D3 D2 D1 D0 X  X  X  X

32-Bit Load Sequence:

             Byte #1                   Byte #2                   Byte #3                         Byte #4
             Command                   MSB                                                       LSB
LTC2656-16 : X  X  X  X  X  X  X  X    C3 C2 C1 C0 A3 A2 A1 A0   D15 D14 D13 D12 D11 D10 D9 D8   D7 D6 D5 D4 D3 D2 D1 D0
LTC2656-12 : X  X  X  X  X  X  X  X    C3 C2 C1 C0 A3 A2 A1 A0   D11 D10 D9  D8  D7  D6  D5 D4   D3 D2 D1 D0 X  X  X  X

Cx   : DAC Command Code
Ax   : DAC Address (0=DACA, 1=DACB, 2=DACC, 3=DACD, 4=DACE, 5=DACF, 6=DACG, 7=DACH, 0xFF=All DACs)
Dx   : DAC Data Bits
X    : Don't care


Example Code:

Set DAC A to to 2V for 16-bit DAC.

    shift_count = 0;    // 16-bit DAC does not have to be shifted
    dac_voltage = 2.0;  // Sets dac voltage variable to 2v

    dac_code = LTC2656_voltage_to_code(dac_voltage, LTC2656_lsb, LTC2656_offset);       // Calculate DAC code from voltage, lsb, and offset
    LTC2656_write(LTC2656_CS, LTC2656_CMD_WRITE_UPDATE, LTC2656_DAC_A, dac_code << shift_count);    // Set DAC A with DAC code

@endverbatim

http://www.linear.com/product/LTC2656

http://www.linear.com/product/LTC2656#demoboards


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
    @ingroup LTC2656
    Header for LTC2656 Octal SPI 16-/12-Bit Rail-to-Rail DACs with 10ppm/C Max Reference
*/


#ifndef LTC2656_H
#define LTC2656_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC2656_CS
#define LTC2656_CS QUIKEVAL_CS
#endif

//! @name LTC2656 Command Codes
//! @{
//! OR'd together with the DAC address to form the command byte
#define  LTC2656_CMD_WRITE               0x00  //!< Write to input register n
#define  LTC2656_CMD_UPDATE              0x10  //!< Update (power up) DAC register n
#define  LTC2656_CMD_WRITE_UPDATE_ALL    0x20  //!< Write to input register n, update (power up) all
#define  LTC2656_CMD_WRITE_UPDATE        0x30  //!< Write to input register n, update (power up) all
#define  LTC2656_CMD_POWER_DOWN          0x40  //!< Power down n
#define  LTC2656_CMD_POWER_DOWN_ALL      0x50  //!< Power down chip (all DACs and reference)
#define  LTC2656_CMD_INTERNAL_REFERENCE  0x60  //!< Select internal reference (power up reference)
#define  LTC2656_CMD_EXTERNAL_REFERENCE  0x70  //!< Select external reference (power down internal reference)
#define  LTC2656_CMD_NO_OPERATION        0xF0  //!< No operation
//! @}

//! @name LTC2656 DAC Addresses
//! @{
//! Which DAC to operate on
#define  LTC2656_DAC_A     0x00
#define  LTC2656_DAC_B     0x01
#define  LTC2656_DAC_C     0x02
#define  LTC2656_DAC_D     0x03
#define  LTC2656_DAC_E     0x04
#define  LTC2656_DAC_F     0x05
#define  LTC2656_DAC_G     0x06
#define  LTC2656_DAC_H     0x07
#define  LTC2656_DAC_ALL   0x0F
//! @}

//! Write the 16-bit dac_code to the LTC2656
//! @return Void
void LTC2656_write(uint8_t cs,                            //!< Chip Select Pin
                   uint8_t dac_command,                   //!< Command Nibble, left-justified, lower nibble set to zero
                   uint8_t dac_address,                   //!< DAC Address Nibble, right justified, upper nibble set to zero
                   uint16_t dac_code                      //!< 16-bit DAC code
                  );

//! Calculate a LTC2656 DAC code given the desired output voltage and DAC address (0-7)
//! @return @return The 16-bit code to send to the DAC
uint16_t LTC2656_voltage_to_code(float dac_voltage,       //!< Voltage to send to DAC
                                 float LTC2656_lsb,       //!< LSB value (volts)
                                 int16_t LTC2656_offset   //!< Offset (volts)
                                );

//! Calculate the LTC2656 DAC output voltage given the DAC code, offset, and LSB value
//! @return Calculated voltage
float LTC2656_code_to_voltage(uint16_t dac_code,          //!< DAC code
                              float LTC2656_lsb,          //!< LSB value (volts)
                              int16_t LTC2656_offset        //!< Offset (volts)
                             );

//! Calculate the LTC2656 offset and LSB voltage given two measured voltages and their corresponding codes
//! @return Void
void LTC2656_calibrate(uint16_t dac_code1,                //!< First DAC code
                       uint16_t dac_code2,                //!< Second DAC code
                       float voltage1,                    //!< First voltage
                       float voltage2,                    //!< Second voltage
                       float *LTC2656_lsb,                //!< Returns resulting LSB (volts)
                       int16_t *LTC2656_offset            //!< Returns resulting Offset (volts)
                      );

#endif  // LTC2656_H
