/*!
LTC6603: Dual, Matched, High Frequency Bandpass/Lowpass Filters

@verbatim

The LTC®6603 is a dual, matched, programmable lowpass
filter for communications receivers and transmitters. The
selectivity of the LTC6603, combined with its linear phase,
phase matching and dynamic range, make it suitable for
filtering in many communications systems. With 1.5°
phase matching between channels, the LTC6603 can be
used in applications requiring pairs of matched filters,
such as transceiver I and Q channels. Furthermore, the
differential inputs and outputs provide a simple interface
for most communications systems.

@endverbatim


http://www.analog.com/en/products/amplifiers/adc-drivers/fully-differential-amplifiers/ltc6603.html

http://www.analog.com/en/design-center/evaluation-hardware-and-software/evaluation-boards-kits/dc1304a-b.html

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

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/

//! @ingroup Analog_to_Digital_Converters
//! @{
//! @defgroup LTC6603 LTC6603: Dual Matched, High Frequency Bandpass/Lowpass Filters
//! @}

/*! @file
    @ingroup LTC6603
    Library for LTC6603: Dual Matched, High Frequency Bandpass/Lowpass Filters
*/





#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC6603.h"
#include <SPI.h>

void LTC6603_write(uint8_t cs,  uint8_t *tx, uint8_t length)
// Reads the LTC6603
{
  uint8_t rx[4];
  spi_transfer_block(cs, tx, rx,length);    // Transfer 4 bytes=
}

