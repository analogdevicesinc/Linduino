/*!
LTC PMBus Support: Implementation for a LTC Fault Log

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
