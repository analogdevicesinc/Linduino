/*!
REVISION HISTORY
$Revision: 3659 $
$Date: 2015-07-01 10:19:20 -0700 (Wed, 01 Jul 2015) $

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
    @ingroup LTPSM_InFlightUpdate
    Library File
*/

#include "nvm_data_helpers.h"

#define MAX_RECORD_SIZE 600

static uint8_t record_data[MAX_RECORD_SIZE];
static uint8_t record_data_hold[MAX_RECORD_SIZE];
static uint16_t *words;
static uint16_t nWords;
static uint8_t nvram_somethingToVerify = 0; // Simple flag to make sure you are not verifying something you have not buffered or written

int getMaxRecordSize()
{
  return MAX_RECORD_SIZE;
}

uint8_t *getRecordData()
{
  return record_data;
}

uint8_t *getRecordHoldData()
{
  return record_data_hold;
}

uint8_t *holdRecord()
{
  memcpy(record_data_hold, record_data, MAX_RECORD_SIZE);
  return record_data_hold;
}

// This function replaced the older one above. It reads through the whole linked list
// and writes the nodes word by word. It also sets the 'nvramListTopVerify' pointer to
// the root node of the linked list it just wrote. This allows the verify function
// to know what to verify.
uint8_t writeNvmData(t_RECORD_NVM_DATA *pRecord)
{
  uint8_t allGood = 1; // Eventually use this for a timeout
  int8_t busy;
  int16_t count;

  nvram_somethingToVerify = 1;

  for (uint16_t i = 0; i < nWords; i++)
  {
    if (allGood)
    {
      if (pRecord->detailedRecordHeader.UsePec)
        smbusPec__->writeWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
                              pRecord->detailedRecordHeader.CommandCode,
                              words[i]);
      else
        smbusNoPec__->writeWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress,
                                pRecord->detailedRecordHeader.CommandCode,
                                words[i]);

      count = 0;
      do
      {
        if (pRecord->detailedRecordHeader.UsePec)
          busy = smbusPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress, 0xef);
        else
          busy = smbusNoPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress, 0xef);

        busy = (busy & 0x40)==0;
      }
      while (busy && (count++ < 4096));
      if (count == 4097)
      {
        Serial.println(F("NVM Write Timeout"));
        allGood = 0;
      }
    }
  }
  return allGood;
}

// This function is called to store the block of NVRAM data in the bitstream
// into a linked list to be used later.
uint8_t bufferNvmData(t_RECORD_NVM_DATA *pRecord)
{
  nvram_somethingToVerify = 1;

  nWords = (uint16_t)((pRecord->baseRecordHeader.Length-8)/2);
  words = (uint16_t *) ((uint16_t)(holdRecord()+8)); // Change (UINT16) to the size of an address on the target machine.

  return 1;
}

// This function reads the NVRAM back from the device and compares it against
// what was buffered. If any word does not match, it returns a fail flag.
uint8_t readThenVerifyNvmData(t_RECORD_NVM_DATA *pRecord)
{
  uint8_t allGood = 1;
  uint16_t actual_value;
  uint16_t expected_value;
  uint8_t busy;
  uint16_t count;

  if (nvram_somethingToVerify == 0)
  {
    Serial.println(F("No NVM Data"));
    return 1;
  }

  nvram_somethingToVerify = 0;

  for (uint16_t i = 0; i < nWords; i++)
  {
    expected_value = words[i];
    actual_value = 0;

    if (allGood)
    {
      if (pRecord->detailedRecordHeader.UsePec)
        actual_value = smbusPec__->readWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode);
      else
        actual_value = smbusNoPec__->readWord((uint8_t) pRecord->detailedRecordHeader.DeviceAddress, pRecord->detailedRecordHeader.CommandCode);


      if (actual_value != expected_value)
      {

        Serial.print(F("Failed verify: Address "));
        Serial.print(pRecord->detailedRecordHeader.DeviceAddress, HEX);
        Serial.print(F(" Command "));
        Serial.print(pRecord->detailedRecordHeader.CommandCode, HEX);
        Serial.print(F(" Index "));
        Serial.print(i);
        Serial.print(F(" Expected "));
        Serial.print(expected_value, HEX);
        Serial.print(F(" Actual "));
        Serial.println(actual_value, HEX);

        allGood = 0;
      }

      count = 0;
      do
      {
        if (pRecord->detailedRecordHeader.UsePec)
          busy = smbusPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress, 0xef);
        else
          busy = smbusNoPec__->readByte((uint8_t) pRecord->detailedRecordHeader.DeviceAddress, 0xef);

        //      Serial.println(F("Loop NVM Data"));

        busy = (busy & 0x40)==0;
      }
      while (busy && (count++ < 4096));
      if (count == 4097)
      {
        Serial.println(F("NVM Read Timeout"));
        allGood = 0;
      }

    }
//    else
//        Serial.println(F("Good"));

  }

  return allGood;
}
