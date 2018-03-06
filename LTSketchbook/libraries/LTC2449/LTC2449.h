/*!
LTC2449: 24-Bit, 16-Channel Delta Sigma ADCs with Selectable Speed/Resolution.
LTC2442: 24-Bit, 4-Channel Delta Sigma ADC with Integrated Amplifier
LTC2448: 24-Bit, 8-/16-Channel Delta Sigma ADCs with Selectable Speed/Resolution

@verbatim

The LTC2444/LTC2445/LTC2448/LTC2449 are 8-/16-channel (4-/8-differential)
high speed 24-bit No Latency Delta Sigma ADCs. They use a proprietary
delta-sigma architecture enabling variable speed/resolution. Through a
simple 4-wire serial interface, ten speed/resolution combinations
6.9Hz/280nVRMS to 3.5kHz/25uVRMS (4kHz with external oscillator) can be
selected with no latency between conversion results or shift in DC accuracy
(offset, full-scale, linearity, drift). Additionally, a 2X speed mode can
be selected enabling output rates up to 7kHz (8kHz if an external
oscillator is used) with one cycle latency.


SPI DATA FORMAT (MSB First):

            Byte #1                            Byte #2

Data Out :  !EOC DMY SIG D28 D27 D26 D25 D24   D23  D22  D21  D20  D19 D18 D17 D16
Data In  :  1    0   EN  SGL OS  S2  S1  S0    OSR3 OSR2 OSR1 OSR1 SPD X   X   X

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
Sx   : Address Select Bit
0SRX : Over Sampling Rate Bits
SPD  : Double Output Rate Select Bit (0-Normal rate, auto-calibration on, 2x rate, auto_calibration off)

Command Byte #1
1    0    EN   SGL  OS   S2   S1   S0   Comments
1    0    0    X    X    X    X    X    Keep Previous Mode
1    0    1    0    X    X    X    X    Differential Mode
1    0    1    1    X    X    X    X    Single-Ended Mode

|  Coversion Rate    |  RMS  | ENOB | OSR  | Latency
Command Byte #2          |Internal | External | Noise |      |      |
|  9MHz   | 10.24MHz |       |      |      |
OSR3 OSR2 OSR1 OSR1 SPD  | Clock   |  Clock   |       |      |      |
0    0    0    0    0      Keep Previous Speed/Resolution
0    0    0    1    0      3.52kHz   4kHz       23uV    17     64     none
0    0    1    0    0      1.76kHz   2kHz       3.5uV   20.1   128    none
0    0    1    1    0      880Hz     1kHz       2uV     21.3   256    none
0    1    0    0    0      440Hz     500Hz      1.4uV   21.8   512    none
0    1    0    1    0      220Hz     250Hz      1uV     22.4   1024   none
0    1    1    0    0      110Hz     125Hz      750nV   22.9   2048   none
0    1    1    1    0      55Hz      62.5Hz     510nV   23.4   4096   none
1    0    0    0    0      27.5Hz    31.25Hz    375nV   24     8192   none
1    0    0    1    0      13.75Hz   15.625Hz   250nV   24.4   16384  none
1    1    1    1    0      6.87kHz   7.8125Hz   200nV   24.6   32768  none
0    0    0    0    1      Keep Previous Speed/Resolution
OSR3 OSR2 OSR1 OSR1 1      2X Mode  *all clock speeds double

Example Code:

Read Channel 0 in Single-Ended with OSR of 65536

    uint16_t miso_timeout = 1000;
    adc_command = LTC2449_CH0 | LTC2449_OSR_32768 | LTC2449_SPEED_2X;   // Build ADC command for channel 0
                                                                        // OSR = 32768*2 = 65536

    if(LTC2449_EOC_timeout(LTC2449_CS, miso_timeout))    // Check for EOC
        return;                                          // Exit if timeout is reached
    LTC2449_read(LTC2449_CS, adc_command, &adc_code);    // Throws out last reading

    if(LTC2449_EOC_timeout(LTC2449_CS, miso_timeout))    // Check for EOC
        return;                                          // Exit if timeout is reached
    LTC2449_read(LTC2449_CS, adc_command, &adc_code);    // Obtains the current reading and stores to adc_code variable

    // Convert adc_code to voltage
    adc_voltage = LTC2449_code_to_voltage(adc_code, LTC2449_lsb, LTC2449_offset_code);

@endverbatim

http://www.linear.com/product/LTC2449
http://www.linear.com/product/LTC2442
http://www.linear.com/product/LTC2448

http://www.linear.com/product/LTC2449#demoboards
http://www.linear.com/product/LTC2442#demoboards
http://www.linear.com/product/LTC2448#demoboards


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
    @ingroup LTC2449
    Header for LTC2449: 24-Bit, 16-Channel Delta Sigma ADCs with Selectable Speed/Resolution
*/

#ifndef LTC2449_H
#define LTC2449_H

//! Define the SPI CS pin
#ifndef LTC2449_CS
#define LTC2449_CS QUIKEVAL_CS
#endif

/*! @name Mode Configuration
 @{
*/
#define LTC2449_KEEP_PREVIOUS_MODE              0x8000
#define LTC2449_KEEP_PREVIOUS_SPEED_RESOLUTION  0x0000
#define LTC2449_SPEED_1X                        0x0000
#define LTC2449_SPEED_2X                        0x0008
/*!
 @}
*/

/*! @name Single-Ended Channels Configuration
@{ */
#define LTC2449_CH0            0xB000
#define LTC2449_CH1            0xB800
#define LTC2449_CH2            0xB100
#define LTC2449_CH3            0xB900
#define LTC2449_CH4            0xB200
#define LTC2449_CH5            0xBA00
#define LTC2449_CH6            0xB300
#define LTC2449_CH7            0xBB00
#define LTC2449_CH8            0xB400
#define LTC2449_CH9            0xBC00
#define LTC2449_CH10           0xB500
#define LTC2449_CH11           0xBD00
#define LTC2449_CH12           0xB600
#define LTC2449_CH13           0xBE00
#define LTC2449_CH14           0xB700
#define LTC2449_CH15           0xBF00
/*! @} */

/*! @name Differential Channel Configuration
@{ */
#define LTC2449_P0_N1          0xA000
#define LTC2449_P1_N0          0xA800

#define LTC2449_P2_N3          0xA100
#define LTC2449_P3_N2          0xA900

#define LTC2449_P4_N5          0xA200
#define LTC2449_P5_N4          0xAA00

#define LTC2449_P6_N7          0xA300
#define LTC2449_P7_N6          0xAB00

#define LTC2449_P8_N9          0xA400
#define LTC2449_P9_N8          0xAC00

#define LTC2449_P10_N11        0xA500
#define LTC2449_P11_N10        0xAD00

#define LTC2449_P12_N13        0xA600
#define LTC2449_P13_N12        0xAE00

#define LTC2449_P14_N15        0xA700
#define LTC2449_P15_N14        0xAF00
/*! @} */

/*Commands
Construct a channel / resolution control word by bitwise ORing one choice from the channel configuration
and one choice from the Oversample ratio configuration. You can also enable 2Xmode, which will increase
sample rate by a factor of 2 but introduce one cycle of latency.

Example - read channel 3 single-ended at OSR2048, with 2X mode enabled.
adc_command = (LTC2449_CH3 | LTC2449_OSR_2048) | LTC2449_SPEED_2X;
*/

/*! @name Oversample Ratio (OSR) Commands
@{ */
#define LTC2449_OSR_64         0xA010
#define LTC2449_OSR_128        0xA020
#define LTC2449_OSR_256        0xA030
#define LTC2449_OSR_512        0xA040
#define LTC2449_OSR_1024       0xA050
#define LTC2449_OSR_2048       0xA060
#define LTC2449_OSR_4096       0xA070
#define LTC2449_OSR_8192       0xA080
#define LTC2449_OSR_16384      0xA090
#define LTC2449_OSR_32768      0xA0F0
/*! @}*/

//! Checks for EOC with a specified timeout
//! @return Returns 0=successful, 1=unsuccessful (exceeded timeout)
int8_t LTC2449_EOC_timeout(uint8_t cs,           //!< Chip Select pin
                           uint16_t miso_timeout  //!< Timeout (in milliseconds)
                          );

//! Reads from LTC2449.
//! @return  void
void LTC2449_read(uint8_t cs,               //!< Chip Select pin
                  uint16_t adc_command,     //!< 2 byte command written to LTC2449
                  uint32_t *adc_code        //!< 4 byte conversion code read from LTC2449
                 );

//! Calculates the voltage corresponding to an adc code, given lsb weight (in volts) and the calibrated
//! ADC offset code (zero code that is subtracted from adc_code).
//! @return Returns voltage calculated from ADC code.
float LTC2449_code_to_voltage(int32_t adc_code,             //!< Code read from adc
                              float LTC2449_lsb,            //!< LSB weight (in volts)
                              int32_t LTC2449_offset_code   //!< The calibrated offset code (This is the adc code zero code that will be subtraced from adc_code)
                             );

//! Calculate the lsb weight and offset code given a full-scale code and a measured zero-code.
//! @return Void
void LTC2449_cal_voltage(int32_t zero_code,             //!< Measured code with the inputs shorted to ground
                         int32_t fs_code,               //!< Measured code at nearly full-scale
                         float zero_voltage,            //!< Measured zero voltage
                         float fs_voltage,              //!< Voltage measured at input (with voltmeter) when fs_code was read from adc
                         float *LTC2449_lsb,            //!< Overwritten with lsb weight (in volts)
                         int32_t *LTC2449_offset_code   //!< Overwritten with offset code (zero code)
                        );

#endif  // LTC2449_H