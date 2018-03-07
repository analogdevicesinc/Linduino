/*!
LTC SMBus Support: API for a shared SMBus layere

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

//! @ingroup PMBus_SMBus
//! @{
//! @defgroup LT_SMBusGroup LT_SMBusGroup: SMBus Group Protocol
//! @}

/*! @file
    @ingroup LT_SMBusGroup
    Library Header File for LT_SMBusGroup
*/

#include <Arduino.h>
#include "LT_SMBusGroup.h"

LT_SMBusGroup::LT_SMBusGroup(LT_SMBus *smbus) : LT_SMBusBase()
{
  executor = smbus;
  queueing = false;
  head = NULL;
  tail = NULL;
}

LT_SMBusGroup::LT_SMBusGroup(LT_SMBus *smbus, uint32_t speed) : LT_SMBusBase(speed)
{
  executor = smbus;
  queueing = false;
  head = NULL;
  tail = NULL;
}

void LT_SMBusGroup::writeByte(uint8_t address, uint8_t command, uint8_t data)
{
  if (queueing)
    addToQueue(new WriteByte(executor, address, command, data));
  else
    executor->writeByte(address, command, data);
}

void LT_SMBusGroup::writeBytes(uint8_t *addresses, uint8_t *commands, uint8_t *data, uint8_t no_addresses)
{
  if (queueing)
    addToQueue(new WriteBytes(executor, addresses, commands, data, no_addresses));
  else
    executor->writeBytes(addresses, commands, data, no_addresses);
}

uint8_t LT_SMBusGroup::readByte(uint8_t address, uint8_t command)
{
  if (queueing)
    return executor->readByte(address, command);
  else
    return executor->readByte(address, command);
}

void LT_SMBusGroup::writeWord(uint8_t address, uint8_t command, uint16_t data)
{
  if (queueing)
    addToQueue(new WriteWord(executor, address, command, data));
  else
    executor->writeWord(address, command, data);
}

uint16_t LT_SMBusGroup::readWord(uint8_t address, uint8_t command)
{
  if (queueing)
    return executor->readWord(address, command);
  else
    return executor->readWord(address, command);
}

void LT_SMBusGroup::writeBlock(uint8_t address, uint8_t command,
                               uint8_t *block, uint16_t block_size)
{
  if (queueing)
    addToQueue(new WriteBlock(executor, address, command, block, block_size));
  else
    executor->writeBlock(address, command, block, block_size);
}

uint8_t LT_SMBusGroup::writeReadBlock(uint8_t address, uint8_t command,
                                      uint8_t *block_out, uint16_t block_out_size, uint8_t *block_in, uint16_t block_in_size)
{
  if (queueing)
    return 0;
  else
    return executor->writeReadBlock(address, command, block_out, block_out_size, block_in, block_in_size);
}

uint8_t LT_SMBusGroup::readBlock(uint8_t address, uint8_t command,
                                 uint8_t *block, uint16_t block_size)
{
  if (queueing)
    return 0;
  else
    return executor->readBlock(address, command, block, block_size);
}

void LT_SMBusGroup::sendByte(uint8_t address, uint8_t command)
{
  if (queueing)
    addToQueue(new SendByte(executor, address, command));
  else
    executor->sendByte(address, command);
}


void LT_SMBusGroup::beginStoring()
{
  queueing = true;
  head = new Node();
  tail = head;
}

bool LT_SMBusGroup::addToQueue(Executable *ex)
{
  if (!ex)
    return false;

  Node *newNode = new Node();
  if (!newNode)
  {
    delete ex;
    return false;
  }
  newNode->executable = ex;
  tail->next = newNode;
  tail = newNode;
  return true;
}

void LT_SMBusGroup::execute()
{
  executor->i2cbus()->startGroupProtocol();
  tail = head;
  while ((tail = head->next) != NULL)
  {
    delete head;
    head = tail;
    if (!head->next)
      executor->i2cbus()->endGroupProtocol();
    head->executable->execute();
    delete head->executable;
  }
  delete head;
  head = 0;
  tail = 0;
  queueing = false;
}


//private class methods

LT_SMBusGroup::Executable::Executable(LT_SMBus *e)
{
  executor = e;
}

LT_SMBusGroup::WriteByte::WriteByte(LT_SMBus *e, uint8_t a, uint8_t c, uint8_t d) : Executable(e)
{
  address = a;
  command = c;
  data = d;
}
void LT_SMBusGroup::WriteByte::execute()
{
  executor->writeByte(address, command, data);
}

LT_SMBusGroup::WriteBytes::WriteBytes(LT_SMBus *e, uint8_t *a, uint8_t *c, uint8_t *d, uint8_t n) : Executable(e)
{
  addresses = a;
  commands = c;
  data = d;
  no_addresses = n;
}
void LT_SMBusGroup::WriteBytes::execute()
{
  executor->writeBytes(addresses, commands, data, no_addresses);
};

LT_SMBusGroup::WriteWord::WriteWord(LT_SMBus *e, uint8_t a, uint8_t c, uint16_t d) : Executable(e)
{
  address = a;
  command = c;
  data = d;
}
void LT_SMBusGroup::WriteWord::execute()
{
  executor->writeWord(address, command, data);
};

LT_SMBusGroup::WriteBlock::WriteBlock(LT_SMBus *e, uint8_t a, uint8_t c, uint8_t *b, uint16_t bl) : Executable(e)
{
  address = a;
  command = c;
  block = b;
  block_size = bl;
}
void LT_SMBusGroup::WriteBlock::execute()
{
  executor->writeBlock(address, command, block, block_size);
};

LT_SMBusGroup::SendByte::SendByte(LT_SMBus *e, uint8_t a, uint8_t c) : Executable(e)
{
  address = a;
  command = c;
};
void LT_SMBusGroup::SendByte::execute()
{
  executor->sendByte(address, command);
};
