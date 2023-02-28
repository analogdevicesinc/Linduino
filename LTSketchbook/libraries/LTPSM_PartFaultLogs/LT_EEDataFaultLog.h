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
    Library Header File for LT_EEDataFaultLog
*/

#ifndef LT_EEDataFaultLog_H_
#define LT_EEDataFaultLog_H_

#include "LT_FaultLog.h"

class LT_EEDataFaultLog : public LT_FaultLog
{
  public:

    LT_EEDataFaultLog(LT_PMBus *pmbus):LT_FaultLog(pmbus)
    {

    }

    void getNvmBlock(uint8_t address, uint16_t offset, uint16_t numWords, bool crc, uint8_t *data) 
    {
      bool pecMatch = true;
      uint16_t v;
      uint16_t skip;
      bool shift;

      skip = offset / 2;
      shift = (offset % 2) == 1 ? true : false;

      // Tell the device to get ready and set the index at the beginning of EEPROM
      while ((pmbus_->smbus()->readByte(address, 0xEF) & 0x60) != 0x60); // Wait until not busy
      pmbus_->smbus()->writeByte(address, 0xBD, 0x2B);
      while ((pmbus_->smbus()->readByte(address, 0xEF) & 0x60) != 0x60); // Wait until not busy
      pmbus_->smbus()->writeByte(address, 0xBD, 0x91);
      while ((pmbus_->smbus()->readByte(address, 0xEF) & 0x60) != 0x60); // Wait until not busy
      pmbus_->smbus()->writeByte(address, 0xBD, 0xE4);
      // Read the ID
      while ((pmbus_->smbus()->readByte(address, 0xEF) & 0x60) != 0x60); // Wait until not busy
      v = pmbus_->smbus()->readWord(address, 0xBF);
      // Read the size and ingore it because we know it will not change
      while ((pmbus_->smbus()->readByte(address, 0xEF) & 0x60) != 0x60); // Wait until not busy
      v = pmbus_->smbus()->readWord(address, 0xBF);
      // Consume words we need to ignore
      for (uint16_t i = 0; i < skip; i++)
      {
        while ((pmbus_->smbus()->readByte(address, 0xEF) & 0x60) != 0x60); // Wait until not busy
        v = pmbus_->smbus()->readWord(address, 0xBF);
      }
      // Consume words of the fault log and check the CRC
      int pos = 0;
      pmbus_->smbus()->pecClear();
      for (uint16_t i = 0; i < numWords; i++)
      {
        while ((pmbus_->smbus()->readByte(address, 0xEF) & 0x60) != 0x60); // Wait until not busy
        uint16_t w = pmbus_->smbus()->readWord(address, 0xBF);

        if (crc && ((i + 1) % 16 == 0))
        {
          data[pos] = 0xFF & w; // If this is not obvious, consider the endianess. This is the 32nd byte.
          pos++;
        }
        else   // And endianess applies here too.
        {
          if ((i != 0) || !shift)
          {
            data[pos] = 0xFF & w;
            pos++;
          }
          data[pos] = 0xFF & (w >> 8);
          pos++;
        }
      }

      pmbus_->smbus()->writeByte(address, 0xBD, 0x00);
      while ((pmbus_->smbus()->readByte(address, 0xEF) & 0x60) != 0x60); // Wait until not busy
    }
};

#endif /* LT_EEDataFaultLog_H_ */
