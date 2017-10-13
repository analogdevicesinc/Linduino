/*!
LTC PMBus Support: Implementation for a LTC Controller Fault Log

@verbatim

This API is shared with Linduino and RTOS code. End users should code to this
API to enable use of the PMBus code without modifications.

@endverbatim

REVISION HISTORY
$Revision: 4358 $
$Date: 2015-11-19 11:15:53 -0700 (Thu, 19 Nov 2015) $

Copyright (c) 2014, Linear Technology Corp.(LTC)
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
    @ingroup LT_FaultLog
    Library Header File for LT_CommandPlusFaultLog
*/

#ifndef LT_CommandPlusFaultLog_H_
#define LT_CommandPlusFaultLog_H_

#include "LT_FaultLog.h"


class LT_CommandPlusFaultLog : public LT_FaultLog
{
  public:

    LT_CommandPlusFaultLog(LT_PMBus *pmbus):LT_FaultLog(pmbus)
    {

    }

    void getNvmBlock(uint8_t address, uint16_t offset, uint16_t numWords, uint8_t command, uint8_t *data)
    {
      // Get device ready to give data using command plus
      pmbus_->smbus()->writeWord(address, command, 0x00EE);
      while ((pmbus_->smbus()->readByte(address, 0xEF) & 0x60) != 0x60); // Wait until not busy

      // Consume the size word.
      uint16_t w = pmbus_->smbus()->readWord(address, command + 1);
      while ((pmbus_->smbus()->readByte(address, 0xEF) & 0x60) != 0x60); // Wait until not busy

      // Consume words we need to ignore
      for (uint16_t i = 0; i < offset; i++)
      {
        while ((pmbus_->smbus()->readByte(address, 0xEF) & 0x60) != 0x60); // Wait until not busy
        pmbus_->smbus()->readWord(address, command + 1);
      }
      // Consume words of the fault log
      int pos = 0;
      for (uint16_t i = 0; i < numWords; i++)
      {
        while ((pmbus_->smbus()->readByte(address, 0xEF) & 0x60) != 0x60); // Wait until not busy
        w = pmbus_->smbus()->readWord(address, command + 1);
        // Endianess matters here
        data[pos] = 0xFF & w;
        pos++;
        data[pos] = 0xFF & (w >> 8);
        pos++;
      }
    }
};

#endif /* LT_CommandPlusFaultLog_H_ */
