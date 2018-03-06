/*!
LTC2499: 24-Bit, 16-Channel Delta Sigma ADCs with Easy Drive Input Current Cancellation

@verbatim

The LTC2499 is a 16-channel (eight differential), 24-bit, No Latency DS TM
ADC with Easy Drive technology and a 2-wire, I2C interface. The patented
sampling scheme eliminates dynamic input current errors and the
shortcomings of on-chip buffering through automatic cancellation of
differential input current. This allows large external source impedances
and rail-to-rail input signals to be directly digitized while maintaining
exceptional DC accuracy.

I2C DATA FORMAT (MSB FIRST);

Data Out:
Byte #1                                     Byte #2

START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 R SACK   SGN MSB D23 D22 D21 D20 D19 D18 MACK

Byte #3                                Byte #4
D17 D16 D15 D14 D13 D12 D11 D10 MACK   D9 D8 D7 D6 D5 D4 D3 D2

Byte #5
MACK D1 D0 SUB5 SUB4 SUB3 SUB2 SUB1 SUB0 MNACK STOP


Data In:
Byte #1                                                   Byte #2
                                                          MSB
START  SA6 SA5 SA4 SA3 SA2 SA1 SA0 W SACK   C12 C11 C10 C9 C8 C7 C6 C5 SACK

Byte #3
C4 C3 C2 C1 C0 X X X SACK STOP

START        : I2C Start
REPEAT Start : I2C Repeat Start
STOP         : I2C Stop
SACK         : I2C Slave Generated Acknowledge (Active Low)
MACK         : I2C Master Generated Acknowledge (Active Low)
MNACK        : I2C Master Generated Not Acknowledge
SAx  : I2C Address
W    : I2C Write (0)
R    : I2C Read  (1)
Cx   : Command Code
Dx   : Data Bits
X    : Don't care
MSB  : Most Significant Bit

Example Code:

Read Channel 0 in Single-Ended with 1X speed mode and rejection frequency of 50Hz

    adc_command_high = LTC2499_CH0;                     // Build ADC command for channel 0
    adc_command_low = LTC2499_R50 | LTC2499_SPEED_1X;   // Set to 1X mode with 50hz rejection
    ack |= LTC2499_read(i2c_address, adc_command_high, adc_command_low, &adc_code);       // Throws out last reading

    ack |= LTC2499_read(i2c_address, adc_command_high, adc_command_low, &adc_code);       // Obtains the current reading and stores to adc_code variable

    if(!ack)
      adc_voltage = LTC2499_code_to_voltage(adc_code, LTC2499_vref);  // Convert adc_code to voltage

@endverbatim

http://www.linear.com/product/LTC2499

http://www.linear.com/product/LTC2499#demoboards


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
    @ingroup LTC2499
    Header for LTC2499: 24-Bit, 16-Channel Delta Sigma ADCs with Easy Drive Input Current Cancellation
*/

#ifndef LTC2499_H
#define LTC2499_H

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

/*! @name Mode Configuration
 @{
*/
#define LTC2499_KEEP_PREVIOUS_MODE              LTC24XX_HS_MULTI_KEEP_PREVIOUS_MODE
#define LTC2499_KEEP_PREVIOUS_SPEED_RESOLUTION  LTC24XX_HS_MULTI_KEEP_PREVIOUS_SPEED_RESOLUTION
#define LTC2499_SPEED_1X                        LTC24XX_HS_MULTI_SPEED_1X
#define LTC2499_SPEED_2X                        LTC24XX_HS_MULTI_SPEED_2X
#define LTC2499_INTERNAL_TEMP                   0xC0

// Select rejection frequency - 50 and 60, 50, or 60Hz
#define LTC2499_R50         LTC24XX_EZ_MULTI_R50
#define LTC2499_R60         LTC24XX_EZ_MULTI_R60
#define LTC2499_R50_R60     LTC24XX_EZ_MULTI_R55
/*!
 @}
*/

/*! @name Single-Ended Channels Configuration
@{ */
#define LTC2499_CH0            LTC24XX_MULTI_CH_CH0
#define LTC2499_CH1            LTC24XX_MULTI_CH_CH1
#define LTC2499_CH2            LTC24XX_MULTI_CH_CH2
#define LTC2499_CH3            LTC24XX_MULTI_CH_CH3
#define LTC2499_CH4            LTC24XX_MULTI_CH_CH4
#define LTC2499_CH5            LTC24XX_MULTI_CH_CH5
#define LTC2499_CH6            LTC24XX_MULTI_CH_CH6
#define LTC2499_CH7            LTC24XX_MULTI_CH_CH7
#define LTC2499_CH8            LTC24XX_MULTI_CH_CH8
#define LTC2499_CH9            LTC24XX_MULTI_CH_CH9
#define LTC2499_CH10           LTC24XX_MULTI_CH_CH10
#define LTC2499_CH11           LTC24XX_MULTI_CH_CH11
#define LTC2499_CH12           LTC24XX_MULTI_CH_CH12
#define LTC2499_CH13           LTC24XX_MULTI_CH_CH13
#define LTC2499_CH14           LTC24XX_MULTI_CH_CH14
#define LTC2499_CH15           LTC24XX_MULTI_CH_CH15
/*! @} */

/*! @name Differential Channel Configuration
@{ */
#define LTC2499_P0_N1          LTC24XX_MULTI_CH_P0_N1
#define LTC2499_P1_N0          LTC24XX_MULTI_CH_P1_N0

#define LTC2499_P2_N3          LTC24XX_MULTI_CH_P2_N3
#define LTC2499_P3_N2          LTC24XX_MULTI_CH_P3_N2

#define LTC2499_P4_N5          LTC24XX_MULTI_CH_P4_N5
#define LTC2499_P5_N4          LTC24XX_MULTI_CH_P5_N4

#define LTC2499_P6_N7          LTC24XX_MULTI_CH_P6_N7
#define LTC2499_P7_N6          LTC24XX_MULTI_CH_P7_N6

#define LTC2499_P8_N9          LTC24XX_MULTI_CH_P8_N9
#define LTC2499_P9_N8          LTC24XX_MULTI_CH_P9_N8

#define LTC2499_P10_N11        LTC24XX_MULTI_CH_P10_N11
#define LTC2499_P11_N10        LTC24XX_MULTI_CH_P11_N10

#define LTC2499_P12_N13        LTC24XX_MULTI_CH_P12_N13
#define LTC2499_P13_N12        LTC24XX_MULTI_CH_P13_N12

#define LTC2499_P14_N15        LTC24XX_MULTI_CH_P14_N15
#define LTC2499_P15_N14        LTC24XX_MULTI_CH_P15_N14
/*! @} */

/*Commands
Construct a channel / resolution control word by bitwise ORing one choice from the channel configuration
and one choice from the Oversample ratio configuration. You can also enable 2Xmode, which will increase
sample rate by a factor of 2 but introduce an offset of up to 2mV (refer to datasheet EC table.)

Example - read channel 3 single-ended, with 2X mode enabled.
adc_command = (LTC2499_CH3 | LTC2499_SPEED_2X);
*/

//! Reads from LTC2499.
//! @return  1 if no acknowledge, 0 if acknowledge
uint8_t LTC2499_read(uint8_t i2c_address,   //!< I2C address (7-bit format) for part
                     uint8_t adc_command_high, //!< High byte command written to LTC2499
                     uint8_t adc_command_low,  //!< Low byte command written to LTC2499
                     int32_t *adc_code,        //!< 4 byte conversion code read from LTC2499
                     uint16_t timeout          //!< Timeout in ms
                    );

//! Calculates the voltage corresponding to an adc code, given the reference (in volts)
//! @return Returns voltage calculated from ADC code.
float LTC2499_code_to_voltage(int32_t adc_code,     //!< Code read from adc
                              float vref            //!< VRef (in volts)
                             );

#endif  // LTC2499_H