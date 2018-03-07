/*!
LTC2640: Single 12-/10-/8-Bit Rail-to-Rail DACs with 10ppm/C Reference.
LTC2630: Single 12-/10-/8-Bit Rail-to-Rail DACs with Integrated Reference in SC70.

@verbatim

The LTC2640 is a family of 12-, 10-, and 8-bit voltage-output DACs with an
integrated, high-accuracy, low-drift reference in an 8-lead TSOT-23 package. It
has a rail-to-rail output buffer that is guaranteed monotonic.

The LTC2640-L has a full-scale output of 2.5V, and operates from a single 2.7V
to 5.5V supply. The LTC2640-H has a full-scale output of 4.096V, and operates
from a 4.5V to 5.5V supply. A 10 ppm/C reference output is available at the REF
pin.

Each DAC can also operate in External Reference mode, in which a voltage
supplied to the REF pin sets the full- scale output. The LTC2640 DACs use a
SPI/MICROWIRE compatible 3-wire serial interface which operates at clock rates
up to 50 MHz.

The LTC2640 incorporates a power-on reset circuit. Options are available for
Reset to Zero Scale or Reset to Midscale after power-up.

SPI DATA FORMAT (MSB First):

            Byte #1                            Byte #2          Byte #3
(LTC2640-12)
Data In  :  C3 C2 C1 C0 X X X X    D11 D10 D9 D8 D7 D6 D5 D4    D3 D2 D1 D0 X X X X
(LTC2640-10)
Data In  :  C3 C2 C1 C0 X X X X    D9  D8  D7 D6 D5 D4 D3 D2    D1 D0 X  X  X X X X
(LTC2640-8)
Data In  :  C3 C2 C1 C0 X X X X    D7  D6  D5 D4 D3 D2 D1 D0    X  X  X  X  X X X X

Cx   : DAC Command Code
Dx   : Data Bits
X    : Don't care

Example Code:

Set DAC to 1V for 12-bit DAC.


    shift_count = 4; // Set shift count for 12-bit DAC

    dac_voltage = 1.0;  // Set dac voltage variable to 1V;
    dac_code = LTC2640_voltage_to_code(dac_voltage, LTC2640_lsb, LTC2640_offset); // Calculates DAC code from desired voltage

    LTC2640_write(LTC2640_CS, LTC2640_CMD_WRITE_UPDATE, dac_code << shift_count);

@endverbatim

http://www.linear.com/product/LTC2640
http://www.linear.com/product/LTC2630

http://www.linear.com/product/LTC2640#demoboards
http://www.linear.com/product/LTC2630#demoboards


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
    @ingroup LTC2640
    Header for LTC2640 Single 12-/10-/8-Bit Rail-to-Rail DACs with 10ppm/C Reference
*/

#ifndef LTC2640_H
#define LTC2640_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC2640_CS
#define LTC2640_CS QUIKEVAL_CS //! SPI Chip Select Pin
#endif

//! @name LTC2640 Command Codes (C3-C0)
//! @{
#define  LTC2640_CMD_WRITE               0x00  // Write to input register
#define  LTC2640_CMD_UPDATE              0x10  // Update (power up) DAC register
#define  LTC2640_CMD_WRITE_UPDATE        0x30  // Write to and update (power up) the DAC register
#define  LTC2640_CMD_POWER_DOWN          0x40  // Power down
#define  LTC2640_CMD_INTERNAL_REFERENCE  0x60  // Select internal reference (default at power up)
#define  LTC2640_CMD_EXTERNAL_REFERENCE  0x70  // Select the supply as the reference
//! @}

//! Write the dac_command and 16-bit dac_code to the LTC2640. The dac_code
//! must be left aligned before calling this function.
//! @return Void
void LTC2640_write(uint8_t cs,          //!< Chip Select Pin
                   uint8_t dac_command, //!< Command code nibble
                   uint16_t dac_code    //!< 12-bit DAC code, left justified
                  );

//! Calculate the LTC2640 DAC code given the desired output voltage
//! @return Code to send to DAC
uint16_t LTC2640_voltage_to_code(float dac_voltage,       //!< Voltage to send to DAC
                                 float LTC2640_lsb,       //!< LSB value (volts)
                                 int16_t LTC2640_offset   //!< Offset (volts)
                                );

//! Calculate the LTC2640 DAC output voltage given the DAC code, offset, and LSB value
//! @return Calculated voltage
float LTC2640_code_to_voltage(uint16_t dac_code,          //!< DAC code
                              float LTC2640_lsb,          //!< LSB value (volts)
                              int16_t LTC2640_offset      //!< Offset (volts)
                             );

//! Calculate the LTC2640 offset and LSB voltages given two measured voltages and their corresponding codes
//! @return Void
void LTC2640_calibrate(uint16_t dac_code1,                //!< First DAC code
                       uint16_t dac_code2,                //!< Second DAC code
                       float voltage1,                    //!< First voltage
                       float voltage2,                    //!< Second voltage
                       float *LTC2640_lsb,                //!< Returns resulting LSB (volts)
                       int16_t *LTC2640_offset            //!< Returns resulting Offset (volts)
                      );

#endif  // LTC2640_H
