/*!
LTC5556: Dual Programmable Downconverting Mixer with IF DVGA

@verbatim

The LTC5556 dual programmable gain downconverting mixer
is ideal for diversity and MIMO receivers that require
precise gain setting. Each channel incorporates an active
mixer and a digital IF VGA with 15.5dB gain control range.
The IF gain of each channel is programmed in 0.5dB steps
through the SPI. A reduced power mode is also available
for each channel.

SPI DATA FORMAT (MSB First):

Data In  :   RP2[0] X  X  IF2[4] IF2[3] IF2[2] IF2[1] IF2[0]  RP1[0] X  X  IF1[4] IF1[3] IF1[2] IF1[1] IF1[0]
            |-------- CONTROL BITS FOR LTC5556 CH2 --------|  |-------- CONTROL BITS FOR LTC5556 CH1 --------|

RP : Reduced Power Mode Control Bit
IF : IF Attenuation Control Bits

@endverbatim

http://www.linear.com/product/LTC5556

http://www.linear.com/product/LTC5556#demoboards

REVISION HISTORY
$Revision: 4634 $
$Date: 2016-01-28 11:51:17 -0700 (Thu, 28 Jan 2016) $

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
    @ingroup LTC5556
    Header for LTC5556: Dual Programmable Downconverting Mixer with IF DVGAs
*/

#ifndef LTC5556_H
#define LTC5556_H

#include <SPI.h>

//! Define the SPI CS pin
#ifndef LTC5556_CS
#define LTC5556_CS QUIKEVAL_CS
#endif

//! @name LTC5556 Reduced power mode configuration bits.
//! @{
// LTC5556 Reduced power mode configuration bits
#define LTC5556_FULL_POWER      0x00
#define LTC5556_REDUCED_POWER   0x80
//! @}

//! Writes to the LTC5556 twice and reads back the last byte to make sure the 
//! LTC5556 was loaded properly
//! @return void
void LTC5556_write(uint8_t cs,                  //! Chip Select pin
                   uint16_t tx,                 //! Word to be transmitted
                   uint16_t *rx                 //! Output pointer
				   );

//! Function to duplicate settings for both LTC5556 channels
//! @return @tbd
uint8_t LTC5556_dupl_settings();

//! Function to apply unique settings for each LTC5556 channel
//! @return @tbd
uint16_t LTC5556_diff_settings();
				   
//! Take inputs and apply the settings to the mixer
//! @return @tbd				   
uint8_t LTC5556_settings();

//! Function to get data from user for power mode
//! @return @tbd
uint8_t LTC5556_get_power_mode(char *prompt   //! @tbd
                              );

//! Get attenuation value from user
//! @return @tbd
uint8_t LTC5556_get_att(char *prompt			//! @tbd
					    );          

//! Decode the register value read from the LTC5556
//! @return void						
void LTC5556_decode_output(uint8_t output		//! @tbd
						   );     
#endif  // LTC5556_H

