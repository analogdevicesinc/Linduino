/*!
LTC2605: Octal 16-/14-/12-Bit Rail-to Rail DACs in 16-Lead SSOP

@verbatim

The LTC2605/LTC2615/LTC2625 are octal 16-, 14- and 12-bit, 2.7V to 5.5V
rail-to-rail voltage-output DACs in 16-lead narrow SSOP packages. They have
built-in high performance output buffers and are guaranteed monotonic.

These parts establish new board-density benchmarks for 16-/14-bit DACs and
advance performance standards for output drive, crosstalk and load regulation
in single supply, voltage-output multiples.


I2C DATA FORMAT (MSB First):

                    Byte #1                             Byte #2
                    MSB
LTC2655-16 : START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK  C3 C2 C1 C0 A3 A2 A1 A0 SACK
LTC2655-12 : START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK  C3 C2 C1 C0 A3 A2 A1 A0 SACK


Byte #3                             Byte #4
                                    LSB
D15 D14 D13 D12 D11 D10 D9 D8 SACK  D7 D6 D5 D4 D3 D2 D1 D0 SACK  STOP
D11 D10 D9  D8  D7  D6  D5 D4 SACK  D3 D2 D1 D0 X  X  X  X  SACK  STOP

START: I2C Start
SAx  : I2C Address
W    : I2C Write (0)
SACK : I2C Slave Generated Acknowledge (Active Low)
Cx   : DAC Command Code
Ax   : DAC Address (0=DACA, 1=DACB, 2=DACC, 3=DACD, 0xFF=All DACs)
Dx   : DAC Data Bits
X    : Don't care
STOP : I2C Stop


Example Code:

Set DAC A to to 2V for 16-bit DAC.

    shift_count = 0;    // 16-bit DAC does not have to be shifted
    dac_voltage = 2.0;  // Sets dac voltage variable to 2v

    dac_code = LTC2605_voltage_to_code(dac_voltage, LTC2605_lsb, LTC2605_offset);   // Calculate DAC code from voltage, lsb, and offset
    ack = LTC2655_write(LTC2605_I2C_ADDRESS, LTC2605_CMD_WRITE_UPDATE, LTC2605_DAC_A, dac_code);    // Set DAC A with DAC code

@endverbatim


http://www.linear.com/product/LTC2605

http://www.linear.com/product/LTC2605#demoboards




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
  @ingroup LTC2605
  Header for LTC2605: Octal 16-/14-/12-Bit Rail-to Rail DACs in 16-Lead SSOP
*/

#ifndef LTC2605_H
#define LTC2605_H

#include <Wire.h>
//!Use table to select address
/*!
|LTC2604 I2C Address Assignment| Value  |  AD2       | AD1     | AD0      |
| :--------------------------: | :------: | :------: |:------: | :------: |
|  LTC2605_I2C_ADDRESS         |  0x10    |  GND     |   GND   |   GND    |
|  LTC2605_I2C_ADDRESS         |  0x11    |  GND     |   GND   |   Float  |
|  LTC2605_I2C_ADDRESS         |  0x12    |  GND     |   GND   |   Vcc    |
|  LTC2605_I2C_ADDRESS         |  0x13    |  GND     |   Float |   GND    |
|  LTC2605_I2C_ADDRESS         |  0x20    |  GND     |   Float |   Float  |
|  LTC2605_I2C_ADDRESS         |  0x21    |  GND     |   Float |   Vcc    |
|  LTC2605_I2C_ADDRESS         |  0x22    |  GND     |   Vcc   |   GND    |
|  LTC2605_I2C_ADDRESS         |  0x23    |  GND     |   Vcc   |   Float  |
|  LTC2605_I2C_ADDRESS         |  0x30    |  GND     |   Vcc   |   Vcc    |
|  LTC2605_I2C_ADDRESS         |  0x31    |  Float   |   GND   |   GND    |
|  LTC2605_I2C_ADDRESS         |  0x32    |  Float   |   GND   |   Float  |
|  LTC2605_I2C_ADDRESS         |  0x33    |  Float   |   GND   |   Vcc    |
|  LTC2605_I2C_ADDRESS         |  0x40    |  Float   |   Float |   GND    |
|  LTC2605_I2C_ADDRESS         |  0x41    |  Float   |   Float |   Float  |
|  LTC2605_I2C_ADDRESS         |  0x42    |  Float   |   Float |   Vcc    |
|  LTC2605_I2C_ADDRESS         |  0x43    |  Float   |   Vcc   |   GND    |
|  LTC2605_I2C_ADDRESS         |  0x50    |  Float   |   Vcc   |   Float  |
|  LTC2605_I2C_ADDRESS         |  0x51    |  Float   |   Vcc   |   Vcc    |
|  LTC2605_I2C_ADDRESS         |  0x52    |  Vcc     |   GND   |   GND    |
|  LTC2605_I2C_ADDRESS         |  0x53    |  Vcc     |   GND   |   Float  |
|  LTC2605_I2C_ADDRESS         |  0x60    |  Vcc     |   GND   |   Vcc    |
|  LTC2605_I2C_ADDRESS         |  0x61    |  Vcc     |   Float |   GND    |
|  LTC2605_I2C_ADDRESS         |  0x62    |  Vcc     |   Float |   Float  |
|  LTC2605_I2C_ADDRESS         |  0x63    |  Vcc     |   Float |   Vcc    |
|  LTC2605_I2C_ADDRESS         |  0x70    |  Vcc     |   Vcc   |   GND    |
|  LTC2605_I2C_ADDRESS         |  0x71    |  Vcc     |   Vcc   |   Float  |
|  LTC2605_I2C_ADDRESS         |  0x72    |  Vcc     |   Vcc   |   Vcc    |
| LTC2605_I2C_GLOBAL_ADDRESS   |  0x73    |    X     |    X    |    X     |
*/
/*! @name LTC2604 Address Assignments
@{ */

// I2C Address Choices:
// To choose an address, comment out all options except the
// configuration on the demo board.

//  Address assignment
// LTC2605 I2C Address                //  AD2       AD1       AD0
// #define LTC2605_I2C_ADDRESS 0x10      //  GND       GND       GND
// #define LTC2605_I2C_ADDRESS 0x11    //  GND       GND       Float
// #define LTC2605_I2C_ADDRESS 0x12    //  GND       GND       Vcc
// #define LTC2605_I2C_ADDRESS 0x13    //  GND       Float     GND
// #define LTC2605_I2C_ADDRESS 0x20    //  GND       Float     Float
// #define LTC2605_I2C_ADDRESS 0x21    //  GND       Float     Vcc
// #define LTC2605_I2C_ADDRESS 0x22    //  GND       Vcc       GND
// #define LTC2605_I2C_ADDRESS 0x23    //  GND       Vcc       Float
// #define LTC2605_I2C_ADDRESS 0x30    //  GND       Vcc       Vcc
// #define LTC2605_I2C_ADDRESS 0x31    //  Float     GND       GND
// #define LTC2605_I2C_ADDRESS 0x32    //  Float     GND       Float
// #define LTC2605_I2C_ADDRESS 0x33    //  Float     GND       Vcc
// #define LTC2605_I2C_ADDRESS 0x40    //  Float     Float     GND
// #define LTC2605_I2C_ADDRESS 0x41    //  Float     Float     Float
// #define LTC2605_I2C_ADDRESS 0x42    //  Float     Float     Vcc
// #define LTC2605_I2C_ADDRESS 0x43    //  Float     Vcc       GND
// #define LTC2605_I2C_ADDRESS 0x50    //  Float     Vcc       Float
// #define LTC2605_I2C_ADDRESS 0x51    //  Float     Vcc       Vcc
// #define LTC2605_I2C_ADDRESS 0x52    //  Vcc       GND       GND
// #define LTC2605_I2C_ADDRESS 0x53    //  Vcc       GND       Float
// #define LTC2605_I2C_ADDRESS 0x60    //  Vcc       GND       Vcc
// #define LTC2605_I2C_ADDRESS 0x61    //  Vcc       Float     GND
// #define LTC2605_I2C_ADDRESS 0x62    //  Vcc       Float     Float
// #define LTC2605_I2C_ADDRESS 0x63    //  Vcc       Float     Vcc
// #define LTC2605_I2C_ADDRESS 0x70    //  Vcc       Vcc       GND
// #define LTC2605_I2C_ADDRESS 0x71    //  Vcc       Vcc       Float
#define LTC2605_I2C_ADDRESS 0x72    //  Vcc       Vcc       Vcc
//#define LTC2605_I2C_GLOBAL_ADDRESS  0x73






//! @name LTC2655 Command Codes
//! @{
//! OR'd together with the DAC address to form the command byte
#define  LTC2605_CMD_WRITE               0x00   //!< Write to input register n
#define  LTC2605_CMD_UPDATE              0x10   //!< Update (power up) DAC register n
#define  LTC2605_CMD_WRITE_UPDATE        0x30   //!< Write to input register n, update (power up) all 
#define  LTC2605_CMD_POWER_DOWN          0x40   //!< Power down n
#define  LTC2605_CMD_POWER_DOWN_ALL      0x50   //!< Power down chip (all DACs and reference)
#define  LTC2605_CMD_INTERNAL_REFERENCE  0x60   //!< Select internal reference (power up reference)
#define  LTC2605_CMD_EXTERNAL_REFERENCE  0x70   //!< Select external reference (power down internal reference)
#define  LTC2605_CMD_NO_OPERATION        0xF0   //!< No operation
//! @}

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
#define  LTC2605_DAC_A     0x00
#define  LTC2605_DAC_B     0x01
#define  LTC2605_DAC_C     0x02
#define  LTC2605_DAC_D     0x03
#define  LTC2605_DAC_E     0x04
#define  LTC2605_DAC_F     0x05
#define  LTC2605_DAC_G     0x06
#define  LTC2605_DAC_H     0x07
#define  LTC2605_DAC_ALL   0x0F


//! Write a 16-bit dac_code to the LTC2605.
//! @return ACK bit (0=acknowledge, 1=no acknowledge)
int8_t  LTC2605_write(uint8_t  i2c_address,                   //!< I2C address of DAC
                      uint8_t  dac_command,                   //!< Command Nibble, left-justified, lower nibble set to zero
                      uint8_t  dac_address,                   //!< DAC Address Nibble, right justified, upper nibble set to zero
                      uint16_t dac_code                       //!< 16-bit DAC code
                     );

//! Calculate a LTC2605 DAC code given the desired output voltage
//! @return The 16-bit code to send to the DAC
uint16_t LTC2605_voltage_to_code(float dac_voltage,       //!< Voltage to send to DAC
                                 float LTC2605_lsb,       //!< LSB value
                                 int16_t LTC2605_offset   //!< Offset
                                );

//! Calculate the LTC2605 DAC output voltage given the DAC code, offset, and LSB value
//! @return Calculated voltage
float LTC2605_code_to_voltage(uint16_t dac_code,          //!< DAC code
                              float LTC2605_lsb,          //!< LSB value
                              int16_t LTC2605_offset      //!< Offset
                             );

#endif //LTC2605_H

