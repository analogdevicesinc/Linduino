/*!
LTC SMBus Support: API for a shared SMBus layer

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
//! @defgroup LT_SMBusBase LT_SMBusBase: Implementation of SMBus
//! @}

/*! @file
    @ingroup LT_SMBusBase
    Library Header File for LT_SMBusBase
*/

#include "LT_SMBusBase.h"

#define USE_BLOCK_TRANSACTION 0
#define FOUND_SIZE 0x79

bool LT_SMBusBase::open_ = false;
uint8_t LT_SMBusBase::found_address_[FOUND_SIZE + 1];

LT_SMBusBase::LT_SMBusBase()
{
  i2cbus_ = new LT_I2CBus();

  if (open_ == false)
  {
    i2cbus_->quikevalI2CInit();          //! Initializes Linduino I2C port.
    i2cbus_->quikevalI2CConnect();       //! Connects I2C port to the QuikEval connector

    open_ = true;
  }
}

LT_SMBusBase::LT_SMBusBase(uint32_t speed)
{
  i2cbus_ = new LT_I2CBus(speed);

  if (open_ == false)
  {
    i2cbus_->quikevalI2CInit();          //! Initializes Linduino I2C port.
    i2cbus_->quikevalI2CConnect();       //! Connects I2C port to the QuikEval connector

    open_ = true;
  }
}

LT_SMBusBase::~LT_SMBusBase()
{
  delete i2cbus_;
}

uint8_t LT_SMBusBase::readAlert(void)
{
  uint8_t address;

  if (i2cbus_->readByte(0x0C, &address))
    Serial.print(F("Read Alert: fail.\n"));

  return (address >> 1);
}

uint8_t LT_SMBusBase::waitForAck(uint8_t address, uint8_t command)
{
  uint8_t data;
  // A real application should timeout at 4.1 seconds.
  uint16_t timeout = 8192;
  while (timeout-- > 0)
  {
    if (0 == i2cbus_->readByteData(address, command, &data))
      return SUCCESS;
  }
  return FAILURE;
}


uint8_t *LT_SMBusBase::probe(uint8_t command)
{
  uint8_t   address;
  uint8_t   found = 0;

  for (address = 0x10; address < 0x7F; address++)
  {
    if (address == 0x0C)
      continue;

    if (0==i2cbus_->writeByte(address, command))
    {

      if (found < FOUND_SIZE)
        found_address_[found++] = address;

    }


  }

  found_address_[found] = 0;

  return found_address_;
}

uint8_t *LT_SMBusBase::probeUnique(uint8_t command)
{
  uint8_t   address;
  uint8_t   found = 0;

  for (address = 0x10; address < 0x7F; address++)
  {
    if (address == 0x0C)
      continue;
    if (address == 0x5B)
      continue;
    if (address == 0x5A)
      continue;
    if (address == 0x7C)
      continue;

    if (0==i2cbus_->writeByte(address, command))
    {

      if (found < FOUND_SIZE)
        found_address_[found++] = address;

    }


  }

  found_address_[found] = 0;

  return found_address_;
}

void LT_SMBusBase::writeByte(uint8_t address, uint8_t command, uint8_t data)
{
  if (pec_enabled_)
  {
    uint8_t buffer[2];
    buffer[0] = data;

    pecClear();
    pecAdd(address << 1);
    pecAdd(command);
    pecAdd(data);
    buffer[1] = pecGet();
    if (i2cbus_->writeBlockData(address, command, 2, buffer))
      Serial.print(F("Write Byte With Pec: fail.\n"));
  }
  else
  {
    if (i2cbus_->writeByteData(address, command, data))
      Serial.print(F("Write Byte: fail.\n"));
  }
}

void LT_SMBusBase::writeBytes(uint8_t *addresses, uint8_t *commands,
                              uint8_t *data, uint8_t no_addresses)
{
  if (pec_enabled_)
  {
    uint8_t buffer[2];
    uint16_t index = 0;

    while (index < no_addresses)
    {
      buffer[0] = data[index];
      pecClear();
      pecAdd(addresses[index] << 1);
      pecAdd(commands[index]);
      pecAdd(data[index]);
      buffer[1] = pecGet();

      if (i2cbus_->writeBlockData(addresses[index], commands[index], 2, buffer))
        Serial.print(F("Write Bytes With Pec: fail.\n"));
      index++;
    }
  }
  else
  {
    uint16_t index = 0;

    while (index < no_addresses)
    {
      if (i2cbus_->writeBlockData(addresses[index], commands[index], 1, &data[index]))
        Serial.print(F("Write Bytes: fail.\n"));
      index++;
    }
  }
}

uint8_t LT_SMBusBase::readByte(uint8_t address, uint8_t command)
{
  if (pec_enabled_)
  {
    uint8_t input[2];
    input[0] = 0x00;
    input[1] = 0x00;

    pecClear();
    pecAdd(address << 1);
    pecAdd(command);
    pecAdd((address << 1) | 0x01);
    if (i2cbus_->readBlockData(address, command, 2, input))
      Serial.print(F("Read Byte With Pec: fail.\n"));

    pecAdd(input[0]);
    if (pecGet() != input[1])
      Serial.print(F("Read Byte With Pec: fail pec\n"));

    return input[0];
  }
  else
  {
    uint8_t result;

    if (i2cbus_->readByteData(address, command, &result))
      Serial.print(F("Read Byte: fail.\n"));
    return result;
  }
}

void LT_SMBusBase::writeWord(uint8_t address, uint8_t command, uint16_t data)
{
  if (pec_enabled_)
  {
    uint8_t buffer[3];
    buffer[0] = (uint8_t) (data & 0xff);
    buffer[1] = (uint8_t) (data >> 8);

    pecClear();
    pecAdd(address << 1);
    pecAdd(command);
    pecAdd(data & 0xff);
    pecAdd(data >> 8);
    buffer[2] = pecGet();
    if (i2cbus_->writeBlockData(address, command, 3, buffer))
      Serial.print(F("Write Word With Pec: fail.\n"));
  }
  else
  {

#if USE_BLOCK_TRANSACTION
    uint8_t buffer[2];
    buffer[0] = (uint8_t) (data & 0xff);
    buffer[1] = (uint8_t) (data >> 8);

    if (i2cbus_->writeBlockData(address, command, 2, buffer))
      Serial.print(F("Write Word: fail.\n"));
#else
    uint16_t rdata;
    rdata = (data << 8) | (data >> 8);
    if (i2cbus_->writeWordData(address, command, rdata))
      Serial.print(F("Write Word: fail.\n"));
#endif
  }
}

uint16_t LT_SMBusBase::readWord(uint8_t address, uint8_t command)
{
  if (pec_enabled_)
  {
    uint8_t input[3];
    input[0] = 0x00;
    input[1] = 0x00;
    input[2] = 0x00;

    pecClear();
    pecAdd(address << 1);
    pecAdd(command);
    pecAdd((address << 1) | 0x01);

    if (i2cbus_->readBlockData(address, command, 3, input))
      Serial.print(F("Read Word With Pec: fail.\n"));

    pecAdd(input[0]);
    pecAdd(input[1]);
    if (pecGet() != input[2])
      Serial.print(F("Read Word With Pec: fail pec\n"));

    return input[1] << 8 | input[0];
  }
  else
  {

#if USE_BLOCK_TRANSACTION
    uint8_t input[2];
    input[0] = 0x00;
    input[1] = 0x00;

    if (i2cbus_->readBlockData(address, command, 2, input))
      Serial.print(F("Read Word: fail.\n"));
    return input[1] << 8 | input[0];
#else
    uint16_t rdata;
    if (i2cbus_->readWordData(address, command, &rdata))
      Serial.print(F("Read Word: fail.\n"));
    return (rdata << 8) | (rdata >> 8);
#endif
  }
}

void LT_SMBusBase::writeBlock(uint8_t address, uint8_t command,
                              uint8_t *block, uint16_t block_size)
{
  if (pec_enabled_)
  {
    uint16_t pos = 0;

    pecClear();
    pecAdd(address << 1);
    pecAdd(command);
    pecAdd(block_size);

    while (pos < block_size)
      pecAdd(block[pos++]);
    uint8_t pec = pecGet();

    uint8_t *data_with_pec = (uint8_t *) malloc(block_size + 2);
    data_with_pec[0] = block_size;
    memcpy(data_with_pec + 1, block, block_size);
    data_with_pec[block_size + 1] = pec;

    if (i2cbus_->writeBlockData(address, command, block_size + 2, data_with_pec))
      Serial.print(F("Write Block With Pec: fail.\n"));
    free(data_with_pec);
  }
  else
  {
    uint8_t *buffer = (uint8_t *)malloc(block_size + 1);
    buffer[0] = block_size;
    memcpy(buffer + 1, block, block_size);
    if (i2cbus_->writeBlockData(address, command, block_size + 1, buffer))
      Serial.print(F("Write Block: fail.\n"));
    free(buffer);
  }
}

uint8_t LT_SMBusBase::writeReadBlock(uint8_t address, uint8_t command,
                                     uint8_t *block_out, uint16_t block_out_size, uint8_t *block_in, uint16_t block_in_size)
{
  if (pec_enabled_)
  {
    uint16_t pos = 0;
    uint8_t actual_block_size;

    pecClear();
    pecAdd(address << 1);
    pecAdd(command);
    pecAdd(block_out_size);
    while (pos < block_out_size)
      pecAdd(block_out[pos++]);


    uint8_t *buffer = (uint8_t *)malloc(block_out_size + 1);
    buffer[0] = block_out_size;
    memcpy(buffer + 1, block_out, block_out_size);

    i2cbus_->startGroupProtocol();
    if (i2cbus_->writeBlockData(address, command, block_out_size + 1, buffer))
      Serial.print(F("Write/Read Block w/PEC: write fail\n"));
    free(buffer);



    pecAdd((address << 1) | 0x01);

    i2cbus_->endGroupProtocol();
    buffer = (uint8_t *)malloc(block_in_size + 2);
    if (i2cbus_->readBlockData(address, block_in_size + 2, buffer))
      Serial.print(F("Write/Read Block w/PEC: read fail.\n"));
    if (buffer[0] > block_in_size)
    {
      Serial.print(F("Write/Read Block w/PEC: fail read size too big.\n"));
    }
    memcpy(block_in, buffer + 1, block_in_size);

    for (pos = 0; pos<buffer[0] + 1u; pos++)
      pecAdd(buffer[pos]);
    if (pecGet() != buffer[buffer[0]+1])
      Serial.print(F("Write/Read Block w/Pec: fail pec\n"));

    actual_block_size = buffer[0];
    free(buffer);
    return actual_block_size;
  }
  else
  {
    uint8_t *buffer = (uint8_t *)malloc(block_out_size + 1);
    uint8_t actual_block_size;

    buffer[0] = block_out_size;
    memcpy(buffer + 1, block_out, block_out_size);

    i2cbus_->startGroupProtocol();
    if (i2cbus_->writeBlockData(address, command, block_out_size + 1, buffer))
      Serial.print(F("Write/Read Block write fail\n"));
    free(buffer);

    i2cbus_->endGroupProtocol();
    buffer = (uint8_t *)malloc(block_in_size + 1);
    if (i2cbus_->readBlockData(address, block_in_size + 1, buffer))
      Serial.print(F("Write/Read Block: read fail.\n"));
    if (buffer[0] > block_in_size)
    {
      Serial.print(F("Write/Read Block: fail size too big.\n"));
    }
    memcpy(block_in, buffer + 1, block_in_size);

    actual_block_size = buffer[0];
    free(buffer);
    return actual_block_size;
  }

}

uint8_t LT_SMBusBase::readBlock(uint8_t address, uint8_t command,
                                uint8_t *block, uint16_t block_size)
{
  if (pec_enabled_)
  {
    uint16_t pos;
    uint8_t *buffer = (uint8_t *)malloc(block_size + 2);
    uint8_t actual_block_size;

    pecClear();
    pecAdd(address << 1);
    pecAdd(command);
    pecAdd((address << 1) | 0x01);

    if (i2cbus_->readBlockData(address, command, block_size + 2, buffer))

      if (buffer[0] > block_size)
        Serial.print(F("Read Block with PEC: fail size too big.\n"));

    memcpy(block, buffer + 1, block_size);

    for (pos = 0; pos<buffer[0] + 1u; pos++)
      pecAdd(buffer[pos]);
    if (pecGet() != buffer[buffer[0]+1])
      Serial.print(F("Read Block With Pec: fail pec\n"));

    actual_block_size = buffer[0];
    free(buffer);
    return actual_block_size;
  }
  else
  {
    uint8_t *buffer = (uint8_t *)malloc(block_size + 1);
    uint8_t actual_block_size;

    if (i2cbus_->readBlockData(address, command, block_size + 1, buffer))
      Serial.print(F("Read Block: fail.\n"));
    if (buffer[0] > block_size)
    {
      Serial.print(F("Read Block: fail size too big.\n"));
    }
    memcpy(block, buffer + 1, block_size);

    actual_block_size = buffer[0];
    free(buffer);
    return actual_block_size;
  }
}

void LT_SMBusBase::sendByte(uint8_t address, uint8_t command)
{
  if (pec_enabled_)
  {
    uint8_t pec;

    pecClear();
    pecAdd(address << 1);
    pecAdd(command);
    pec = pecGet();

    if (i2cbus_->writeBlockData(address, command, 1, &pec))
      Serial.print(F("Send Byte With Pec: fail.\n"));
  }
  else
  {
    if (i2cbus_->writeByte(address, command))
      Serial.print(F("Send Byte: fail.\n"));
  }
}
