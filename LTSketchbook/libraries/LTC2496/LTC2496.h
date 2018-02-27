/*!
LTC2496: 16-Bit 8-/16-Channel Delta Sigma ADC with Easy Drive Input Current Cancellation

@verbatim

The LTC2496 is a 16-channel (8-differential) 16-bit No Latency ΔΣ ADC with Easy
Drive technology. The patented sampling scheme eliminates dynamic input current
errors and the shortcomings of on-chip buffering through automatic cancellation
of differential input current. This allows large external source impedances,
and rail-to-rail input signals to be directly digitized while maintaining
exceptional DC accuracy.

The LTC2496 includes an integrated oscillator. This device can be configured to
measure an external signal (from combinations of 16 analog input channels
operating in single ended or differential modes). It automatically rejects
line frequencies of 50Hz and 60Hz, simultaneously.

SPI DATA FORMAT (MSB First):

            Byte #1                            Byte #2                           Byte #3

Data Out :  !EOC DMY SIG MSB D15 D14 D13 D12   D11 D10 D9  D8  D7  D6  D5  D4    D3  D2  D1  D0  -   -   -   -
Data In  :  1    0   EN  SGL OS  S2  S1  S0    X   X   X   X   X   X   X   X     X   X   X   X   X   X   X   X

!EOC : End of Conversion Bit (Active Low)
DMY  : Dummy Bit (Always 0)
SIG  : Sign Bit (1-data positive, 0-data negative)
MSB  : Most Significant Bit (Provides under range and over range indication)
Dx   : Data Bits
EN   : Enable Bit (0-keep previous mode, 1-change mode)
SGL  : Enable Single-Ended Bit (0-differential, 1-single-ended)
OS   : ODD/Sign Bit
Sx   : Address Select Bit

Command Byte
1    0    EN   SGL  OS   S2   S1   S0   Comments
1    0    0    X    X    X    X    X    Keep Previous Mode
1    0    1    0    X    X    X    X    Differential Mode
1    0    1    1    X    X    X    X    Single-Ended Mode


Example Code:

Read Channel 0 in Single-Ended mode

    uint16_t miso_timeout = 1000;
    adc_command = LTC2496_CH0;                          // Build ADC command for channel 0

    if(LTC2496_EOC_timeout(LTC2496_CS, miso_timeout))    // Check for EOC
        return(1);
    LTC2496_read(LTC2496_CS, adc_command, &adc_code);   // Throws out last reading

    if(LTC2496_EOC_timeout(LTC2496_CS, miso_timeout))    // Check for EOC
        return(1);
    LTC2496_read(LTC2496_CS, adc_command, &adc_code);   // Obtains the current reading and stores to adc_code variable

    // Convert adc_code to voltage
    adc_voltage = LTC2496_code_to_voltage(adc_code, LTC2496_lsb , LTC2496_offset_code);

@endverbatim

http://www.linear.com/product/LTC2496

http://www.linear.com/product/LTC2496#demoboards



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
  @ingroup LTC2496
  Header for LTC2496 LTC2496: 16-Bit 8-/16-Channel Delta Sigma ADC with Easy Drive Input Current Cancellation
*/

#ifndef LTC2496_H
#define LTC2496_H

//! define the SPI CS pin
#ifndef LTC2496_CS
#define LTC2496_CS QUIKEVAL_CS
#endif

//! @name LTC2496 Command Constraints
//!@{
//! Command Constants.
#define LTC2496_DISABLE     0x80
#define LTC2496_ENABLE      0xA0


//!Channel Selection OR with ENABLE or select DISABLE to use previous selection
/*!
//!Use table to select address
/*!
|LTC2496 Channel Selection     | Value    |
| :--------------------------: | :------: |
| LTC2496_P0_N1                |    0x00  |
| LTC2496_P1_N0                |    0x08  |
|                              |          |
| LTC2496_P2_N3                |    0x01  |
| LTC2496_P3_N2                |    0x09  |
|                              |          |
| LTC2496_P4_N5                |    0x02  |
| LTC2496_P5_N4                |    0x0A  |
|                              |          |
| LTC2496_P6_N7                |    0x03  |
| LTC2496_P7_N6                |    0x0B  |
|                              |          |
| LTC2496_P8_N9                |    0x04  |
| LTC2496_P9_N8                |    0x0C  |
|                              |          |
| LTC2496_P10_N11              |    0x05  |
| LTC2496_P11_N10              |    0x0D  |
|                              |          |
| LTC2496_P12_N13              |    0x06  |
| LTC2496_P13_N12              |    0x0E  |
|                              |          |
| LTC2496_P14_N15              |    0x07  |
| LTC2496_P15_N14              |    0x0F  |
*/
/*! @name LTC2496 Differential Channel Selection
@{*/

#define LTC2496_P0_N1          0x00
#define LTC2496_P1_N0          0x08

#define LTC2496_P2_N3          0x01
#define LTC2496_P3_N2          0x09

#define LTC2496_P4_N5          0x02
#define LTC2496_P5_N4          0x0A

#define LTC2496_P6_N7          0x03
#define LTC2496_P7_N6          0x0B

#define LTC2496_P8_N9          0x04
#define LTC2496_P9_N8          0x0C

#define LTC2496_P10_N11        0x05
#define LTC2496_P11_N10        0x0D

#define LTC2496_P12_N13        0x06
#define LTC2496_P13_N12        0x0E

#define LTC2496_P14_N15        0x07
#define LTC2496_P15_N14        0x0F

/*!
|LTC2496 Channel Selection     | Value    |
| :--------------------------: | :------: |
| LTC2496_CH0                  |  0x10    |
| LTC2496_CH1                  |  0x18    |
| LTC2496_CH2                  |  0x11    |
| LTC2496_CH3                  |  0x19    |
| LTC2496_CH4                  |  0x12    |
| LTC2496_CH5                  |  0x1A    |
| LTC2496_CH6                  |  0x13    |
| LTC2496_CH7                  |  0x1B    |
| LTC2496_CH8                  |  0x14    |
| LTC2496_CH9                  |  0x1C    |
| LTC2496_CH10                 |  0x15    |
| LTC2496_CH11                 |  0x1D    |
| LTC2496_CH12                 |  0x16    |
| LTC2496_CH13                 |  0x1E    |
| LTC2496_CH14                 |  0x17    |
| LTC2496_CH15                 |  0x1F    |
*/
/*! @name LTC2496 Single-Ended Channel Selection
@{ */

#define LTC2496_CH0            0x10
#define LTC2496_CH1            0x18
#define LTC2496_CH2            0x11
#define LTC2496_CH3            0x19
#define LTC2496_CH4            0x12
#define LTC2496_CH5            0x1A
#define LTC2496_CH6            0x13
#define LTC2496_CH7            0x1B
#define LTC2496_CH8            0x14
#define LTC2496_CH9            0x1C
#define LTC2496_CH10           0x15
#define LTC2496_CH11           0x1D
#define LTC2496_CH12           0x16
#define LTC2496_CH13           0x1E
#define LTC2496_CH14           0x17
#define LTC2496_CH15           0x1F


//! Checks for EOC with a specified timeout
//! @return Returns 0=successful, 1=unsuccessful (exceeded timeout)
int8_t LTC2496_EOC_timeout(uint8_t cs,            //!< Chip Select pin
                           uint16_t miso_timeout  //!< Timeout (in millisends)
                          );

//! Read LTC2496 result, program configuration for next conversion
// Example - read channel external input with 60Hz rejection and 2X enabled.
// adc_command = LTC2496_ENABLE|LTC2496_P0_N1;
//! @return void
void LTC2496_read(uint8_t cs,           //!< Chip Select pin
                  uint8_t adc_command,  //!< Command byte
                  int32_t *adc_code     //!< Returns raw 32-bit code read from ADC
                 );


//! Calculates the LTC2496 input voltage
//! @return Calculated voltage
float LTC2496_code_to_voltage(int32_t adc_code,   //!< Raw ADC code
                              float vref          //!< the reference voltage to the ADC
                             );
#endif