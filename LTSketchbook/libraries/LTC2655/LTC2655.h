/*!
LTC2655: Quad I2C 16-/12-Bit Rail-to-Rail DACs with 10ppm/C Max Reference

@verbatim

The LTC2655 is a family of Quad I2C 16-/12-Bit Rail-to-Rail DACs with
integrated 10ppm/C max reference. The DACs have built-in high performance,
rail-to-rail, output buffers and are guaranteed monotonic. The LTC2655-L has a
full-scale output of 2.5V with the integrated reference and operates from a
single 2.7V to 5.5V supply. The LTC2655-H has a full-scale output of 4.096V with
the integrated reference and operates from a 4.5V to 5.5V supply. Each DAC can
also operate with an external reference, which sets the full-scale output to 2
times the external reference voltage.

The parts use the 2-wire I2C compatible serial interface. The LTC2655 operates
in both the standard mode (maximum clock rate of 100kHz) and the fast mode
(maximum clock rate of 400kHz). The LTC2655 incorporates a power-on reset
circuit that is controlled by the PORSEL pin. If PORSEL is tied to GND the DACs
power-on reset to zero-scale. If PORSEL is tied to VCC, the DACs power-on reset
to mid-scale.

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

    dac_code = LTC2655_voltage_to_code(dac_voltage, LTC2655_lsb, LTC2655_offset);   // Calculate DAC code from voltage, lsb, and offset
    ack = LTC2655_write(LTC2655_I2C_ADDRESS, LTC2655_CMD_WRITE_UPDATE, LTC2655_DAC_A, dac_code);    // Set DAC A with DAC code
@endverbatim

http://www.linear.com/product/LTC2655

http://www.linear.com/product/LTC2655#demoboards

REVISION HISTORY
$Revision: 3659 $
$Date: 2015-07-01 10:19:20 -0700 (Wed, 01 Jul 2015) $

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
    @ingroup LTC2655
    Header for LTC2655: Quad I2C 16-/12-Bit Rail-to-Rail DACs with 10ppm/C Max Reference
*/

#ifndef LTC2655_H
#define LTC2655_H

#include <Wire.h>

//! @name I2C_Addresses
//!@{
// I2C Address Choices:
// To choose an address, comment out all options except the
// configuration on the demo board.

//  Address assignment
// LTC2655 I2C Address                //  AD2       AD1       AD0
#define LTC2655_I2C_ADDRESS 0x10      //  GND       GND       GND
// #define LTC2655_I2C_ADDRESS 0x11    //  GND       GND       Float
// #define LTC2655_I2C_ADDRESS 0x12    //  GND       GND       Vcc
// #define LTC2655_I2C_ADDRESS 0x13    //  GND       Float     GND
// #define LTC2655_I2C_ADDRESS 0x20    //  GND       Float     Float
// #define LTC2655_I2C_ADDRESS 0x21    //  GND       Float     Vcc
// #define LTC2655_I2C_ADDRESS 0x22    //  GND       Vcc       GND
// #define LTC2655_I2C_ADDRESS 0x23    //  GND       Vcc       Float
// #define LTC2655_I2C_ADDRESS 0x30    //  GND       Vcc       Vcc
// #define LTC2655_I2C_ADDRESS 0x31    //  Float     GND       GND
// #define LTC2655_I2C_ADDRESS 0x32    //  Float     GND       Float
// #define LTC2655_I2C_ADDRESS 0x33    //  Float     GND       Vcc
// #define LTC2655_I2C_ADDRESS 0x40    //  Float     Float     GND
// #define LTC2655_I2C_ADDRESS 0x41    //  Float     Float     Float
// #define LTC2655_I2C_ADDRESS 0x42    //  Float     Float     Vcc
// #define LTC2655_I2C_ADDRESS 0x43    //  Float     Vcc       GND
// #define LTC2655_I2C_ADDRESS 0x50    //  Float     Vcc       Float
// #define LTC2655_I2C_ADDRESS 0x51    //  Float     Vcc       Vcc
// #define LTC2655_I2C_ADDRESS 0x52    //  Vcc       GND       GND
// #define LTC2655_I2C_ADDRESS 0x53    //  Vcc       GND       Float
// #define LTC2655_I2C_ADDRESS 0x60    //  Vcc       GND       Vcc
// #define LTC2655_I2C_ADDRESS 0x61    //  Vcc       Float     GND
// #define LTC2655_I2C_ADDRESS 0x62    //  Vcc       Float     Float
// #define LTC2655_I2C_ADDRESS 0x63    //  Vcc       Float     Vcc
// #define LTC2655_I2C_ADDRESS 0x70    //  Vcc       Vcc       GND
// #define LTC2655_I2C_ADDRESS 0x71    //  Vcc       Vcc       Float
// #define LTC2655_I2C_ADDRESS 0x72    //  Vcc       Vcc       Vcc

#define LTC2655_I2C_GLOBAL_ADDRESS  0x73
//! @}

//! @name LTC2655 Command Codes
//! @{
//! OR'd together with the DAC address to form the command byte
#define  LTC2655_CMD_WRITE               0x00   // Write to input register n
#define  LTC2655_CMD_UPDATE              0x10   // Update (power up) DAC register n
#define  LTC2655_CMD_WRITE_UPDATE        0x30   // Write to input register n, update (power up) all
#define  LTC2655_CMD_POWER_DOWN          0x40   // Power down n
#define  LTC2655_CMD_POWER_DOWN_ALL      0x50   // Power down chip (all DACs and reference)
#define  LTC2655_CMD_INTERNAL_REFERENCE  0x60   // Select internal reference (power up reference)
#define  LTC2655_CMD_EXTERNAL_REFERENCE  0x70   // Select external reference (power down internal reference)
#define  LTC2655_CMD_NO_OPERATION        0xF0   // No operation
//! @}

//! @name LTC2655 DAC Addresses
//! @{
//! Which DAC to operate on
#define  LTC2655_DAC_A     0x00
#define  LTC2655_DAC_B     0x01
#define  LTC2655_DAC_C     0x02
#define  LTC2655_DAC_D     0x03
#define  LTC2655_DAC_ALL   0x0F
//! @}

// Command Example - write to DAC address D and update all.
// dac_command = LTC2655_CMD_WRITE_UPDATE | LTC2655_DAC_D;

//! Write a 16-bit dac_code to the LTC2655.
//! @return ACK bit (0=acknowledge, 1=no acknowledge)
int8_t  LTC2655_write(uint8_t  i2c_address,                   //!< I2C address of DAC
                      uint8_t  dac_command,                   //!< Command Nibble, left-justified, lower nibble set to zero
                      uint8_t  dac_address,                   //!< DAC Address Nibble, right justified, upper nibble set to zero
                      uint16_t dac_code                       //!< 16-bit DAC code
                     );

//! Calculate a LTC2655 DAC code given the desired output voltage
//! @return The 16-bit code to send to the DAC
uint16_t LTC2655_voltage_to_code(float dac_voltage,       //!< Voltage to send to DAC
                                 float LTC2655_lsb,       //!< LSB value
                                 int16_t LTC2655_offset   //!< Offset
                                );

//! Calculate the LTC2655 DAC output voltage given the DAC code, offset, and LSB value
//! @return Calculated voltage
float LTC2655_code_to_voltage(uint16_t dac_code,          //!< DAC code
                              float LTC2655_lsb,          //!< LSB value
                              int16_t LTC2655_offset      //!< Offset
                             );

//! Calculate the LTC2655 offset and LSB voltages given two measured voltages and their corresponding codes
//! @return Void
void LTC2655_calibrate(uint16_t dac_code1,                //!< First DAC code
                       uint16_t dac_code2,                //!< Second DAC code
                       float voltage1,                    //!< First voltage
                       float voltage2,                    //!< Second voltage
                       float *LTC2655_lsb,                //!< Returns resulting LSB
                       int16_t *LTC2655_offset            //!< Returns resulting Offset
                      );

#endif  // LTC2655_H
