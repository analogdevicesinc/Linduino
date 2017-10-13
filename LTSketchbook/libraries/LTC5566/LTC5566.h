/*!
LTC5566: Dual Programmable Gain Downconverting Mixer

@verbatim

The LTC5566 dual programmable gain downconverting mixer
is ideal for diversity and MIMO receivers that require precise
gain setting. Each channel incorporates an active mixer and a
digital IF VGA with 15.5dB gain control range. The IF gain of
each channel is programmed in 0.5dB steps through the SPI.

SPI DATA FORMAT (MSB First):

            Byte #1                                                  Byte #2
Data In  :  RP2 RT2[1] RT2[0] IF2[4] IF2[3] IF2[2] IF2[1] IF2[0]     RP1 RT1[1] RT1[0] IF1[4] IF1[3] IF1[2] IF1[1] IF1[0]
            |---- ATT, TUNE, POWER CONTROL WORD FOR MIXER 2 ---|     |---- ATT, TUNE, POWER CONTROL WORD FOR MIXER 1 ---|

RPx : Reduced Power Mode Bit
RTx : RF Input Tune Control Bits
IFx : IF Attenuation Control Bits

@endverbatim

http://www.linear.com/product/LTC5566

http://www.linear.com/product/LTC5566#demoboards

REVISION HISTORY
$Revision: 5670 $
$Date: 2016-09-02 10:55:41 -0700 (Fri, 02 Sep 2016) $

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
    @ingroup LTC5566
    Header for LTC5566: Dual Programmable Gain Downconverting Mixer
*/

#ifndef LTC5566_H
#define LTC5566_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC5566_CS
#define LTC5566_CS QUIKEVAL_CS
#endif

//! @name LTC5566 reduced power mode configuration bit.
//! @{
// reduced power mode configuration bit.
#define LTC5566_FULL_POWER      0x00
#define LTC5566_REDUCED_POWER   0x80
//! @}

//! @name LTC5566 shutdown/powerup and GPO configuration bits.
//! @{
// shutdown/powerup and GPO configuration bits.
#define LTC5566_RF_TUNE_00      0x00
#define LTC5566_RF_TUNE_01      0x20
#define LTC5566_RF_TUNE_10      0x40
#define LTC5566_RF_TUNE_11      0x60
//! @}

//! @name LTC5566 gain configuration bits.
//! @{
// gain configuration bits.
#define LTC5566_ATT_0_0dB      0x00
#define LTC5566_ATT_0_5dB      0x01
#define LTC5566_ATT_1_0dB      0x02
#define LTC5566_ATT_1_5dB      0x03
#define LTC5566_ATT_2_0dB      0x04
#define LTC5566_ATT_2_5dB      0x05
#define LTC5566_ATT_3_0dB      0x06
#define LTC5566_ATT_3_5dB      0x07
#define LTC5566_ATT_4_0dB      0x08
#define LTC5566_ATT_4_5dB      0x09
#define LTC5566_ATT_5_0dB      0x0A
#define LTC5566_ATT_5_5dB      0x0B
#define LTC5566_ATT_6_0dB      0x0C
#define LTC5566_ATT_6_5dB      0x0D
#define LTC5566_ATT_7_0dB      0x0E
#define LTC5566_ATT_7_5dB      0x0F
#define LTC5566_ATT_8_0dB      0x10
#define LTC5566_ATT_8_5dB      0x11
#define LTC5566_ATT_9_0dB      0x12
#define LTC5566_ATT_9_5dB      0x13
#define LTC5566_ATT_10_0dB     0x14
#define LTC5566_ATT_10_5dB     0x15
#define LTC5566_ATT_11_0dB     0x16
#define LTC5566_ATT_11_5dB     0x17
#define LTC5566_ATT_12_0dB     0x18
#define LTC5566_ATT_12_5dB     0x19
#define LTC5566_ATT_13_0dB     0x1A
#define LTC5566_ATT_13_5dB     0x1B
#define LTC5566_ATT_14_0dB     0x1C
#define LTC5566_ATT_14_5dB     0x1D
#define LTC5566_ATT_15_0dB     0x1E
#define LTC5566_ATT_15_5dB     0x1F
//! @}

//! Writes to the LTC5566 twice and reads back the last two bytes to make sure
//! the LTC5566 was loaded properly
//! @return void
void LTC5566_write(uint8_t cs,                  //! Chip Select pin
                   uint16_t tx,                 //! Word to be transmitted
                   uint16_t *rx                 //! Output pointer
                  );

//! Function to duplicate settings for both LTC5566 channels
//! @return @tbd
uint8_t LTC5566_dupl_settings();

//! Function to apply unique settings for each LTC5566 channel
//! @return @tbd
uint16_t LTC5566_diff_settings();

//! Function to get data from user for power mode
//! @return @tbd
uint8_t LTC5566_get_power_mode(char *prompt   //! @tbd
                              );

//! Function to get data from user for RF input tune mode
//! @return @tbd
uint8_t LTC5566_get_tune_mode(char *prompt    //! @tbd
                             );

//! Function to get data from user for IF attenuation
//! @return @tbd
uint8_t LTC5566_get_att(char *prompt      //! @tbd
                       );

//! Decode the register value read from the LTC5555
//! @return void
void LTC5566_decode_output(uint8_t output   //! @tbd
                          );

#endif  // LTC5566_H
