/*!
LTC2461: 16-Bit I2C Delta Sigma ADCs with 10ppm/C Max Precision Reference.
LTC2463: Differential, 16-Bit I2C Delta Sigma ADCs with 10ppm/C Max Precision Reference.
LTC2453: Differential, 16-Bit Delta Sigma ADC With I2C Interface.

@verbatim

The LTC2461/LTC2463 are ultra tiny, 16-Bit analog-todigital converters with an
integrated precision reference. They use a single 2.7V to 5.5V supply and
communicate through an I2C Interface. The LTC2461 is single-ended with a 0V to
1.25V input range and the LTC2463 is differential with a 1.25V input range. Both
ADCs include a 1.25V integrated reference with 2ppm/C drift performance and 0.1%
initial accuracy. The converters are available in a 12-pin 3mm x 3mm DFN package
or an MSOP-12 package. They include an integrated oscillator and perform
conversions with no latency for multiplexed applications. The LTC2461/LTC2463
include a proprietary input sampling scheme that reduces the average input
current several orders of magnitude when compared to conventional delta sigma
converters

I2C DATA FORMAT (MSB First):

       Byte #1                             Byte #2                              Byte #3

START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK  EN1 EN2 SPD SLP X   X   X  X   SACK  STOP

START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 R SACK  D15 D14 D13 D12 D11 D10 D9 D8  MACK  D7 D6  D5  D4  D3  D2  D1  D0 MNACK STOP

SACK  : Slave Acknowledge
MACK  : Master Acknowledge
MNACK : Master Not Acknowledge
Sx    : Address Select Bit
EN1   : Enable 1 : to program, EN1 = High, EN2 = Low
EN2   : Enable 2
SPD   : Speed Bit- 0 for 60Hz, 1 for 30Hz
SLP   : Sleep Mode Bit
X     : Don't care
Dx    : Data Bits


Example Code:

Read ADC in 60Hz Speed Mode

    ack |= LTC2461_read(LTC2461_I2C_ADDRESS, LTC2461_60HZ_SPEED_MODE, &adc_code_60Hz);  // Throws out last reading
    ack |= LTC2461_read(LTC2461_I2C_ADDRESS, LTC2461_60HZ_SPEED_MODE, &adc_code_60Hz);  // Obtains the current reading and
                                                                                        // stores to adc_code_60Hz variable

    // Convert adc_code_60Hz to voltage
    adc_voltage_60Hz = LTC2461_code_to_voltage(adc_code_60Hz, LTC2461_lsb, LTC2461_offset_code);

@endverbatim

http://www.linear.com/product/LTC2461
http://www.linear.com/product/LTC2463
http://www.linear.com/product/LTC2453

http://www.linear.com/product/LTC2461#demoboards
http://www.linear.com/product/LTC2463#demoboards
http://www.linear.com/product/LTC2453#demoboards


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
    @ingroup LTC2461
    Header for LTC2461: 16-Bit I2C Delta Sigma ADCs with 10ppm/C Max Precision Reference
*/

#ifndef LTC2461_H
#define LTC2461_H

#include <Wire.h>

// Address Choices:
// To choose an address, comment out all options except the
// configuration on the demo board:

//! @name I2C addresses
//! @{
//! Un-comment the address corresponding to the LTC2309's address

// LTC2461 I2C Addresses
#define LTC2461_I2C_ADDRESS    0x14   //  A0 = LOW : this is demo board default
// #define LTC2461_I2C_ADDRESS    0x54   //  A0 = HIGH
//!@}

//! @name Modes
//! @{
// Command Constants
#define LTC2461_30HZ_SPEED_MODE    0xA0     // SPD=1
#define LTC2461_60HZ_SPEED_MODE    0x80     // SPD=0
#define LTC2461_SLEEP_MODE         0x90     // SLP=1 SPD=XX
//!@}

//! Reads the ADC  and returns 16-bit data
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2461_read(uint8_t i2c_address,    //!< I2C address of the LTC2461.
                    uint8_t adc_command,    //!< ADC command to be written to the LTC2461.
                    uint16_t *adc_code      //!< Returns code read from ADC.
                   );


//! Write a 16-bit command to the ADC. Derived from 2655.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2461_command(uint8_t i2c_address, //!< I2C address of the LTC2461.
                       uint8_t adc_command  //!< ADC command to be written to the LTC2461.
                      );

//! Calculates the LTC2309 input unipolar voltage.
//! @return Floating point voltage
float LTC2461_code_to_voltage(uint16_t adc_code,            //!< Raw ADC code
                              float LTC2461_lsb,            //!< LSB value (volts)
                              int32_t LTC2461_offset_code   //!< Offset code
                             );

//! Calibrate the lsb
//! @return Void
void LTC2461_cal_voltage(uint16_t zero_code,            //!< Code from zero reading
                         uint16_t fs_code,              //!< Full-scale code
                         float zero_voltage,            //!< Measured zero voltage
                         float fs_voltage,              //!< Measured full-scale voltage
                         float *LTC2461_lsb,            //!< Return LSB value (volts)
                         int32_t *LTC2461_offset_code   //!< Return Offset code
                        );


#endif  // LTC2461_H
