/*!
LTC PMBus Support: Implementation for a LTC Fault Log

@verbatim

This API is shared with Linduino and RTOS code. End users should code to this
API to enable use of the PMBus code without modifications.

@endverbatim

REVISION HISTORY
$Revision: 5670 $
$Date: 2016-09-02 10:55:41 -0700 (Fri, 02 Sep 2016) $

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
    Library Header File for LT_FaultLog
*/

#ifndef LT_FaultLog_H_
#define LT_FaultLog_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "LT_SMBus.h"
#include "LT_PMBus.h"

#define FILE_TEXT_LINE_MAX 256

class LT_FaultLog
{
  public:
#pragma pack(push, 1)

    struct Lin16Word
    {
      public:
        uint8_t lo_byte;
        uint8_t hi_byte;
    };

    struct Lin16WordReverse
    {
      public:
        uint8_t hi_byte;
        uint8_t lo_byte;
    };

    struct Lin5_11Word
    {
      public:
        uint8_t lo_byte;
        uint8_t hi_byte;
    };

    struct Lin5_11WordReverse
    {
      public:
        uint8_t hi_byte;
        uint8_t lo_byte;
    };

    struct RawByte
    {
      public:
        uint8_t uint8_tValue;
    };

    struct RawWord
    {
      public:
        uint8_t lo_byte;
        uint8_t hi_byte;
    };

    struct RawWordReverse
    {
      public:
        uint8_t hi_byte;
        uint8_t lo_byte;
    };

    struct FaultLogTimeStamp
    {
      public:
        uint8_t shared_time_byte0;
        uint8_t shared_time_byte1;
        uint8_t shared_time_byte2;
        uint8_t shared_time_byte3;
        uint8_t shared_time_byte4;
        uint8_t shared_time_byte5;
    };

#pragma pack(pop)

  protected:
    LT_PMBus      *pmbus_;
    uint8_t readMfrStatusByte(uint8_t address);
    uint8_t readMfrFaultLogStatusByte(uint8_t address);
  public:
    LT_FaultLog(LT_PMBus *pmbus);
    virtual ~LT_FaultLog() {}

    bool hasFaultLog(uint8_t address);
    void enableFaultLog(uint8_t address);
    void disableFaultLog(uint8_t address);
    void clearFaultLog(uint8_t address);
    virtual void read(uint8_t address) = 0;
    virtual void print(Print *printer) = 0;
    virtual uint8_t *getBinary() = 0;
    virtual uint16_t getBinarySize() = 0;
    virtual void dumpBinary(Print *printer = 0) = 0;
    virtual void release() = 0;

    void dumpBin(Print *printer, uint8_t *log, uint8_t size);

    uint64_t getSharedTime200us(FaultLogTimeStamp time_stamp);
    float getTimeInMs(FaultLogTimeStamp time_stamp);
    uint8_t getRawByteVal(RawByte value);
    uint16_t getRawWordVal(RawWord value);
    uint16_t getRawWordReverseVal(RawWordReverse value);
    uint16_t getLin5_11WordVal(Lin5_11Word value);
    uint16_t getLin5_11WordReverseVal(Lin5_11WordReverse value);
    uint16_t getLin16WordVal(Lin16Word value);
    uint16_t getLin16WordReverseVal(Lin16WordReverse value);
    virtual void getNvmBlock(uint8_t address, uint16_t offset, uint16_t numWords, uint8_t command, uint8_t *data) = 0;
};

#endif /* LT_FaultLog_H_ */
