/*!
LTC2497: 16-Bit, 16-Channel Delta Sigma ADCs with Easy Drive Input Current Cancellation.

@verbatim

The LTC2497 is a 16-channel (eight differential), 16-bit, No Latency Delta
Sigma TM ADC with Easy Drive technology and a 2-wire, I2C interface. The
patented sampling scheme eliminates dynamic input current errors and the
shortcomings of on-chip buffering through automatic cancellation of
differential input current. This allows large external source impedances
and rail-to-rail input signals to be directly digitized while maintaining
exceptional DC accuracy.

I2C DATA FORMAT (MSB FIRST):

Data In:
Byte #1                                     Byte #2

START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK   1  0  EN SGL ODD A2 A1 A0 NACK  Stop

Data Out:
Byte #1                                     Byte #2
                                            MSB
Start   SA6 SA5 SA4 SA3 SA2 SA1 SA0 R SACK  SGN D15 D14 D13 D12 D11 D10 D9 MACK

Byte #3                        Byte #4
                               LSB
D8 D7 D6 D5 D4 D3 D2 D1 MACK   D0 X X X X X X X MNACK   STOP

START       : I2C Start
Repeat Start: I2c Repeat Start
STOP        : I2C Stop
SAx         : I2C Address
SACK        : I2C Slave Generated Acknowledge (Active Low)
MACK        : I2C Master Generated Acknowledge (Active Low)
MNACK       : I2c Master Generated Not Acknowledge
W           : I2C Write (0)
R           : I2C Read  (1)
Cx          : Command Code
Dx          : Data Bits
X           : Don't care
EN          : Enable Bit
SGL         : Single Ended Bit
ODD         : Polarity Bit
SGN         : Sign Bit


Example Code:
Read differential with CH0 as positive and CH1 as negative:

    uint8_t ack;
    int32_t adc_code = 0;
    float adc_voltage;
    adc_command = LTC24XX_MULTI_CH_P0_N1;
    ack |= LTC2497_read(i2c_address, adc_command, &adc_code);
    adc_voltage = LTC2497_code_to_voltage(adc_code, LTC2497_vref);

@endverbatim

http://www.linear.com/product/LTC2497

http://www.linear.com/product/LTC2497#demoboards


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
    @ingroup LTC2497
    Header for LTC2497: 16-Bit, 16-Channel Delta Sigma ADCs with Easy Drive Input Current Cancellation
*/

#ifndef LTC2497_H
#define LTC2497_H

/*! @name I2C_Addresses
@{ */

//! I2C address of the LTC2497.
//! Configured by tying the ADR0, ADR1, and ADR2 pins high or low. See Table 1 of datasheet.
//! Uncomment LTC2497_I2C_ADDRESS to match demo board configuration.
//  Address assignment
// LTC2497 I2C Address                 //  AD2       AD1       AD0
// #define LTC2497_I2C_ADDRESS 0x14    //  Low       Low       Low
// #define LTC2497_I2C_ADDRESS 0x16    //  Low       Low       High
// #define LTC2497_I2C_ADDRESS 0x15    //  Low       Low       Float
// #define LTC2497_I2C_ADDRESS 0x26    //  Low       High      Low
// #define LTC2497_I2C_ADDRESS 0x34    //  Low       High      High
// #define LTC2497_I2C_ADDRESS 0x27    //  Low       High      Float
// #define LTC2497_I2C_ADDRESS 0x17    //  Low       Float     Low
// #define LTC2497_I2C_ADDRESS 0x25    //  Low       Float     High
// #define LTC2497_I2C_ADDRESS 0x24    //  Low       Float     Float
// #define LTC2497_I2C_ADDRESS 0x56    //  High      Low       Low
// #define LTC2497_I2C_ADDRESS 0x64    //  High      Low       High
// #define LTC2497_I2C_ADDRESS 0x57    //  High      Low       Float
// #define LTC2497_I2C_ADDRESS 0x74    //  High      High      Low
#define LTC2497_I2C_ADDRESS 0x76    //  High      High      High
// #define LTC2497_I2C_ADDRESS 0x75    //  High      High      Float
// #define LTC2497_I2C_ADDRESS 0x65    //  High      Float     Low
// #define LTC2497_I2C_ADDRESS 0x67    //  High      Float     High
// #define LTC2497_I2C_ADDRESS 0x66    //  High      Float     Float
// #define LTC2497_I2C_ADDRESS 0x35    //  Float     Low       Low
// #define LTC2497_I2C_ADDRESS 0x37    //  Float     Low       High
// #define LTC2497_I2C_ADDRESS 0x36    //  Float     Low       Float
// #define LTC2497_I2C_ADDRESS 0x47    //  Float     High      Low
// #define LTC2497_I2C_ADDRESS 0x55    //  Float     High      High
// #define LTC2497_I2C_ADDRESS 0x54    //  Float     High      Float
// #define LTC2497_I2C_ADDRESS 0x44    //  Float     Float     Low
// #define LTC2497_I2C_ADDRESS 0x46    //  Float     Float     High
// #define LTC2497_I2C_ADDRESS 0x45    //  Float     Float     Float

//! LTC2497 Global I2C Address.
#define LTC2497_I2C_GLOBAL_ADDRESS 0x77  //  Global Address
/*! @} */


//! Reads from LTC2497.
//! @return  1 if no acknowledge, 0 if acknowledge
uint8_t LTC2497_read(uint8_t i2c_address,   //!< I2C address (7-bit format) for part
                     uint8_t adc_command,   //!< Byte command written to LTC2497
                     int32_t *adc_code,     //!< 4 byte conversion code read from LTC2497
                     uint16_t timeout       //!< Timeout (in milliseconds)
                    );

//! Calculates the voltage corresponding to an ADC code, given the reference (in volts)
//! @return Returns voltage calculated from ADC code.
float LTC2497_code_to_voltage(int32_t adc_code,     //!< Code read from ADC
                              float vref            //!< VRef (in volts)
                             );

#endif  // LTC2497_H