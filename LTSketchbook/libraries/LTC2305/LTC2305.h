/*!
LTC2305: 8-channel, 12-Bit SAR ADC with I2C interface

@verbatim

The LTC2305 is a low noise, low power, 8-channel, 12-bit successive
approximation ADC with an I2C compatible serial interface. This ADC includes an
internal reference and a fully differential sample-and-hold circuit to reduce
common mode noise. The LTC2305 operates from an internal clock to achieve a fast
1.3 microsecond conversion time.

The LTC2305 operates from a single 5V supply and draws just 300 microamps at a
throughput rate of 1ksps. The ADC enters nap mode when not converting, reducing
the power dissipation.

I2C DATA FORMAT (MSB First):


       Byte #1                             Byte #2
START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK  SD OS S1 S0 UNI SLP X X SACK

             Byte #3                             Byte #4                             Byte #5
Repeat Start SA6 SA5 SA4 SA3 SA2 SA1 SA0 R SACK  D11 D10 D9  D8  D7  D6  D5 D4 MACK  D3 D2 D1 D0 X  X  X  X  MNACK  STOP

SACK  : Slave Acknowledge
MACK  : Master Acknowledge
MNACK : Master Not Acknowledge
SD    : Single, Differential# Bit
OS    : ODD, Sign# Bit
Sx    : Address Select Bit
COM   : CH7/COM Configuration Bit
UNI   : Unipolar, Bipolar# Bit
SLP   : Sleep Mode Bit
Dx    : Data Bits
X     : Don't care

Example Code:

Read Channel 0 in Single-Ended Unipolar mode

    adc_command = LTC2305_CH0 | LTC2305_UNIPOLAR_MODE;                  // Build ADC command for channel 0

    ack |= LTC2305_read(LTC2305_I2C_ADDRESS, adc_command, &adc_code);   // Throws out last reading
    ack |= LTC2305_read(LTC2305_I2C_ADDRESS, adc_command, &adc_code);   // Obtains the current reading and stores to adc_code variable

    // Convert adc_code to voltage
    adc_voltage = LTC2305_unipolar_code_to_voltage(adc_code, LTC2305_lsb, LTC2305_offset_code);

@endverbatim

http://www.linear.com/product/LTC2305

http://www.linear.com/product/LTC2305#demoboards


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
    @ingroup LTC2305
    Header for LTC2305: 8-channel, 12-Bit SAR ADC with I2C interface
*/

#ifndef LTC2305_H
#define LTC2305_H

#include <Wire.h>

//! @name I2C addresses
//! @{
//! Un-comment the address corresponding to the LTC2305's address
//                                     //  Pin State
// LTC2305 I2C Address                 //  AD1       AD0

#define LTC2305_I2C_ADDRESS 0x08      //  LOW       LOW
// #define LTC2305_I2C_ADDRESS 0x09    //  LOW       Float
// #define LTC2305_I2C_ADDRESS 0x0A    //  LOW       HIGH
// #define LTC2305_I2C_ADDRESS 0x0B    //  Float     HIGH
// #define LTC2305_I2C_ADDRESS 0x18    //  Float     Float
// #define LTC2305_I2C_ADDRESS 0x19    //  Float     LOW
// #define LTC2305_I2C_ADDRESS 0x1A    //  HIGH      LOW
// #define LTC2305_I2C_ADDRESS 0x1B    //  HIGH      Float
// #define LTC2305_I2C_ADDRESS 0x14    //  High      HIGH
//!@}

//! @name Single-Ended Channel Configuration
//! @{
// Single-Ended Channel Configuration
#define LTC2305_CH0                0xC0
#define LTC2305_CH1                0x80
//!@}


//! @name LTC2305 Configuration Bits
//! @{
// LTC1867 Configuration Bits
#define LTC2305_SLEEP_MODE         0x04
#define LTC2305_EXIT_SLEEP_MODE    0x00
#define LTC2305_UNIPOLAR_MODE      0x08
#define LTC2305_BIPOLAR_MODE       0x00
#define LTC2305_SINGLE_ENDED_MODE  0x80
#define LTC2305_DIFFERENTIAL_MODE  0x00
#define LTC2305_P0_N1        0x00
#define LTC2305_P1_N0        0x40

#define LTC2301            1
#define LTC2305            0
//!@}

// Commands
// Construct a channel / uni/bipolar by bitwise ORing one choice from the channel configuration
// and one choice from the command.

// Example - read channel 3 single-ended
// adc_command = LTC2305_CH3 | LTC2305_UNIPOLAR_MODE;

// Example - read voltage between channels 5 and 4 with 4 as positive polarity and in bipolar mode.
// adc_command = LTC2305_P4_N5 | LTC2305_BIPOLAR_MODE;


//! Reads 12-bit code from LTC2305, programs channel and mode for next conversion.
//! @return Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC2305_read(uint8_t i2c_address,    //!< I2C address of device
                    uint8_t adc_command,    //!< ADC command / address bits
                    uint16_t *ptr_adc_code      //!< Returns code read from ADC
                   );


//! Calculates the LTC2305 input voltage.
//! @return Calculated voltage
float LTC2305_code_to_voltage(uint16_t adc_code,           //!< Code read from ADC
                              float LTC2305_vref,           //!< LSB value (volts)
                              uint8_t uni_bipolar
                             );

#endif  // LTC2305_H
