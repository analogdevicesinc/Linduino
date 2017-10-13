/*!
LTC6602: Dual, Matched, High Frequency Bandpass/Lowpass Filters

@verbatim

The LTC6602 is a dual, matched, programmable bandpass or lowpass filter
and differential driver. The selectivity of the LTC6602, combined with its
phase matching and dynamic range, make it ideal for filtering in RFID
systems. With two degree phase matching between channels, the LTC6602 can
be used in applications requiring highly matched filters, such as
transceiver I and Q channels. Gain programmability, and the fully
differential inputs and outputs, simplify implementation in most systems.

@endverbatim

http://www.linear.com/product/LTC6602

http://www.linear.com/product/LTC6602#demoboards

REVISION HISTORY
$Revision: 6237 $
$Date: 2016-12-20 15:09:16 -0800 (Tue, 20 Dec 2016) $

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

//! @ingroup RF_Timing
//! @{
//! @defgroup LTC6602 LTC6602: Dual Matched, High Frequency Bandpass/Lowpass Filters
//! @}

/*! @file
    @ingroup LTC6602
    Library for LTC6602: Dual Matched, High Frequency Bandpass/Lowpass Filters
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC6602.h"
#include <SPI.h>

void LTC6602_write(uint8_t cs,  uint8_t *tx, uint8_t length)
// Reads the LTC6602
{
  uint8_t rx[4];
  spi_transfer_block(cs, tx, rx,length);    // Transfer 4 bytes=
}

