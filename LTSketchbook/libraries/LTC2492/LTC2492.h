/*!
LTC2492: 24-Bit, 4-Channel Delta Sigma ADCs with Easy Drive Input Current Cancellation

@verbatim

The LTC2492 is a 4-channel (2-differential) 24-bit No Latency Delta Sigma
TM ADC with Easy Drive technology. The patented sampling scheme eliminates
dynamic input current errors and the shortcomings of on-chip buffering
through automatic cancellation of differential input current. This allows
large external source impedances, and rail-to-rail input signals to be
directly digitized while maintaining exceptional DC accuracy.


SPI DATA FORMAT (MSB First):

            Byte #1                            Byte #2

Data Out :  !EOC DMY SIG D28 D27 D26 D25 D24   D23  D22  D21  D20  D19 D18 D17 D16
Data In  :  1    0   EN  SGL OS  A2  A1  A0    EN2  IM   FA   FB   SPD X   X   X

Byte #3                        Byte #4
D15 D14 D13 D12 D11 D10 D9 D8  D7 D6 D5 D4 *D3 *D2 *D1 *D0
X   X   X   X   X   X   X  X   X  X  X  X   X  X   X   X

!EOC : End of Conversion Bit (Active Low)
DMY  : Dummy Bit (Always 0)
SIG  : Sign Bit (1-data positive, 0-data negative)
Dx   : Data Bits
*Dx  : Data Bits Below lsb
EN   : Enable Bit (0-keep previous mode, 1-change mode)
SGL  : Enable Single-Ended Bit (0-differential, 1-single-ended)
OS   : ODD/Sign Bit
Ax   : Address Select Bit
IM   : Internal Temperature Sensor Bit(0-ADC , 1-Temperature Sensor)
Fx   : Frequency Rejection Bits
SPD  : Speed Mode Bit (0-1x, 1-2x)

FIMSPD  : Double Output Rate Select Bit (0-Normal rate, auto-calibration on, 2x rate, auto_calibration off)

Command Byte #1
1    0    EN   SGL  OS   S2   S1   S0   Comments
1    0    0    X    X    X    X    X    Keep Previous Mode
1    0    1    0    X    X    X    X    Differential Mode
1    0    1    1    X    X    X    X    Single-Ended Mode

Example Code:

Read Channel 0 in Single-Ended

    uint16_t miso_timeout = 1000;
    adc_command = LTC2492_CH0 | LTC2492_OSR_32768 | LTC2492_SPEED_2X;   // Build ADC command for channel 0
                                                                        // OSR = 32768*2 = 65536

    if(LTC2492_EOC_timeout(LTC2492_CS, miso_timeout))    // Check for EOC
        return;                                          // Exit if timeout is reached
    LTC2492_read(LTC2492_CS, adc_command, &adc_code);    // Throws out last reading

    if(LTC2492_EOC_timeout(LTC2492_CS, miso_timeout))    // Check for EOC
        return;                                          // Exit if timeout is reached
    LTC2492_read(LTC2492_CS, adc_command, &adc_code);    // Obtains the current reading and stores to adc_code variable

    // Convert adc_code to voltage
    adc_voltage = LTC2492_code_to_voltage(adc_code, LTC2492_lsb, LTC2492_offset_code);

@endverbatim

http://www.linear.com/product/LTC2492

http://www.linear.com/product/LTC2492#demoboards


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
    @ingroup LTC2492
    Header for LTC2492: 24-Bit, 4-Channel Delta Sigma ADCs with Selectable Speed/Resolution
*/

#ifndef LTC2492_H
#define LTC2492_H

//! Define the SPI CS pin
#ifndef LTC2492_CS
#define LTC2492_CS QUIKEVAL_CS
#endif

/*! @name Mode Configuration
 @{
*/
#define LTC2492_KEEP_PREVIOUS_MODE              LTC24XX_HS_MULTI_KEEP_PREVIOUS_MODE
#define LTC2492_KEEP_PREVIOUS_SPEED_RESOLUTION  LTC24XX_HS_MULTI_KEEP_PREVIOUS_SPEED_RESOLUTION
#define LTC2492_SPEED_1X                        LTC24XX_HS_MULTI_SPEED_1X
#define LTC2492_SPEED_2X                        LTC24XX_HS_MULTI_SPEED_2X
#define LTC2492_INTERNAL_TEMP                   LTC24XX_EZ_MULTI_PTAT

// Select rejection frequency - 50 and 60, 50, or 60Hz
#define LTC2492_R50         LTC24XX_EZ_MULTI_R50
#define LTC2492_R60         LTC24XX_EZ_MULTI_R60
#define LTC2492_R50_R60     LTC24XX_EZ_MULTI_R55
/*!
 @}
*/

/*! @name Single-Ended Channels Configuration
@{ */
#define LTC2492_CH0            LTC24XX_MULTI_CH_CH0
#define LTC2492_CH1            LTC24XX_MULTI_CH_CH1
#define LTC2492_CH2            LTC24XX_MULTI_CH_CH2
#define LTC2492_CH3            LTC24XX_MULTI_CH_CH3
/*! @} */

/*! @name Differential Channel Configuration
@{ */
#define LTC2492_P0_N1          LTC24XX_MULTI_CH_P0_N1
#define LTC2492_P1_N0          LTC24XX_MULTI_CH_P1_N0

#define LTC2492_P2_N3          LTC24XX_MULTI_CH_P2_N3
#define LTC2492_P3_N2          LTC24XX_MULTI_CH_P3_N2
/*! @} */

/*Commands
Construct a channel / resolution control word by bitwise ORing one choice from the channel configuration
and one choice from the Oversample ratio configuration. You can also enable 2Xmode, which will increase
sample rate by a factor of 2 but introduce an offset of up to 2mV (refer to datasheet EC table.)

Example - read channel 3 single-ended at OSR2048, with 2X mode enabled.
adc_command = (LTC2492_CH3 | LTC2492_SPEED_2X;
*/

//! Checks for EOC with a specified timeout.
//! @return Returns 0=successful, 1=unsuccessful (exceeded timeout)
int8_t LTC2492_EOC_timeout(uint8_t cs,              //!< Chip Select pin
                           uint16_t miso_timeout    //!< Timeout (in milliseconds)
                          );

//! Reads from LTC2492.
void LTC2492_read(uint8_t cs,               //!< Chip select
                  uint8_t adc_command_high, //!< High byte command written to LTC2492
                  uint8_t adc_command_low,  //!< Low byte command written to LTC2492
                  int32_t *adc_code         //!< 4 byte conversion code read from LTC2492
                 );

//! Calculates the voltage corresponding to an adc code, given the reference (in volts)
//! @return Returns voltage calculated from ADC code.
float LTC2492_code_to_voltage(int32_t adc_code,     //!< Code read from adc
                              float vref            //!< VRef (in volts)
                             );

#endif  // LTC2492_H