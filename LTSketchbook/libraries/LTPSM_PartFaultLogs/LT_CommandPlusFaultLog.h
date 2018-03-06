/*!
LTC PMBus Support: Implementation for a LTC Controller Fault Log

@verbatim

This API is shared with Linduino and RTOS code. End users should code to this
API to enable use of the PMBus code without modifications.

@endverbatim


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
