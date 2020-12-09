/*!

Copyright 2020(c) Analog Devices, Inc.

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
    @ingroup LTPSM_InFlightUpdate
    Library File
*/

#include "nvm_create.h"
#include <Arduino.h>
#include <stdlib.h>
#include <record_type_definitions.h>

uint8_t *create_event(uint16_t id)
{
  t_RECORD_EVENT *record = malloc(sizeof(t_RECORD_EVENT));
  record->baseRecordHeader.RecordType = RECORD_TYPE_EVENT;
  record->eventId = id;

  record->baseRecordHeader.Length = sizeof(*record);
  
  return (uint8_t *) record;
}

uint8_t *create_meta_data(uint16_t data)
{
  t_RECORD_VARIABLE_META_DATA *record = malloc(sizeof(t_RECORD_VARIABLE_META_DATA));
  record->baseRecordHeader.RecordType = RECORD_TYPE_VARIABLE_META_DATA;
  record->metaDataType = data;
  
  record->baseRecordHeader.Length = sizeof(*record);

  return (uint8_t *) record;
}

uint8_t *create_delay_ms(uint16_t delay)
{
  t_RECORD_DELAY_MS *record = malloc(sizeof(t_RECORD_DELAY_MS));
  record->baseRecordHeader.RecordType = RECORD_TYPE_DELAY_MS;
  record->numMs = delay;

  record->baseRecordHeader.Length = sizeof(*record);

  return (uint8_t *) record;
}

uint8_t *create_send_byte(uint16_t address, uint8_t command, bool pec = false)
{ 
  t_RECORD_PMBUS_SEND_BYTE *record = malloc(sizeof(t_RECORD_PMBUS_SEND_BYTE));
  record->baseRecordHeader.RecordType = RECORD_TYPE_PMBUS_SEND_BYTE;
  record->detailedRecordHeader.DeviceAddress = address;
  record->detailedRecordHeader.CommandCode = command;
  record->detailedRecordHeader.UsePec = pec ? 1 : 0;

  record->baseRecordHeader.Length = sizeof(*record);

  return (uint8_t *) record;
}

uint8_t *create_write_byte(uint16_t address, uint8_t command, uint8_t data, bool pec = false)
{ 
  t_RECORD_PMBUS_WRITE_BYTE *record = malloc(sizeof(t_RECORD_PMBUS_WRITE_BYTE));
  record->baseRecordHeader.RecordType = RECORD_TYPE_PMBUS_WRITE_BYTE;
  record->detailedRecordHeader.DeviceAddress = address;
  record->detailedRecordHeader.CommandCode = command;
  record->detailedRecordHeader.UsePec = pec ? 1 : 0;
  record->dataByte = data;

  record->baseRecordHeader.Length = sizeof(*record);

  return (uint8_t *) record;
}

uint8_t *create_extended_write_byte(uint16_t address, uint16_t command, uint8_t data, bool pec = false)
{ 
  t_RECORD_PMBUS_EXTENDED_WRITE_BYTE *record = malloc(sizeof(t_RECORD_PMBUS_EXTENDED_WRITE_BYTE));
  record->baseRecordHeader.RecordType = RECORD_TYPE_PMBUS_EXTENDED_WRITE_BYTE;
  record->detailedRecordHeader.DeviceAddress = address;
  record->detailedRecordHeader.CommandCode = command;
  record->detailedRecordHeader.UsePec = pec ? 1 : 0;
  record->dataByte = data;

  record->baseRecordHeader.Length = sizeof(*record);

  return (uint8_t *) record;
}

uint8_t *create_write_word(uint16_t address, uint8_t command, uint16_t data, bool pec = false)
{ 
  t_RECORD_PMBUS_WRITE_WORD *record = malloc(sizeof(t_RECORD_PMBUS_WRITE_WORD));
  record->baseRecordHeader.RecordType = RECORD_TYPE_PMBUS_WRITE_WORD;
  record->detailedRecordHeader.DeviceAddress = address;
  record->detailedRecordHeader.CommandCode = command;
  record->detailedRecordHeader.UsePec = pec ? 1 : 0;
  record->dataWord = data;

  record->baseRecordHeader.Length = sizeof(*record);

  return (uint8_t *) record;
}

uint8_t *create_extended_write_word(uint16_t address, uint16_t command, uint16_t data, bool pec = false)
{ 
  t_RECORD_PMBUS_EXTENDED_WRITE_WORD *record = malloc(sizeof(t_RECORD_PMBUS_EXTENDED_WRITE_WORD));
  record->baseRecordHeader.RecordType = RECORD_TYPE_PMBUS_EXTENDED_WRITE_WORD;
  record->detailedRecordHeader.DeviceAddress = address;
  record->detailedRecordHeader.CommandCode = command;
  record->detailedRecordHeader.UsePec = pec ? 1 : 0;
  record->dataWord = data;

  record->baseRecordHeader.Length = sizeof(*record);

  return (uint8_t *) record;
}

uint8_t *create_write_block(uint16_t address, uint8_t command, uint8_t *data, int size, bool pec = false)
{ 
  t_RECORD_PMBUS_WRITE_BLOCK *record = malloc(sizeof(t_RECORD_PMBUS_WRITE_BLOCK));
  record->baseRecordHeader.RecordType = RECORD_TYPE_PMBUS_WRITE_BLOCK;
  record->detailedRecordHeader.DeviceAddress = address;
  record->detailedRecordHeader.CommandCode = command;
  record->detailedRecordHeader.UsePec = pec ? 1 : 0;
  record->baseRecordHeader.Length = sizeof(*record) + size;

  uint8_t *block = malloc(sizeof(*record) + size);
  memcpy (block, record, sizeof(*record));
  uint8_t *blockData = block + sizeof(*record);
  memcpy (blockData, data, size); 

  delete record;
  return block;
}

uint8_t *create_read_byte_expect(uint16_t address, uint8_t command, uint8_t data, bool pec = false)
{
  t_RECORD_PMBUS_READ_BYTE_EXPECT *record = malloc(sizeof(t_RECORD_PMBUS_READ_BYTE_EXPECT));
  record->baseRecordHeader.RecordType = RECORD_TYPE_PMBUS_READ_BYTE_EXPECT;
  record->detailedRecordHeader.DeviceAddress = address;
  record->detailedRecordHeader.CommandCode = command;
  record->detailedRecordHeader.UsePec = pec ? 1 : 0;
  record->expectedDataByte = data;

  record->baseRecordHeader.Length = sizeof(*record);

  return (uint8_t *) record;
}

uint8_t *create_extended_read_byte_expect(uint16_t address, uint16_t command, uint8_t data, bool pec = false)
{
  t_RECORD_PMBUS_EXTENDED_READ_BYTE_EXPECT *record = malloc(sizeof(t_RECORD_PMBUS_EXTENDED_READ_BYTE_EXPECT));
  record->baseRecordHeader.RecordType = RECORD_TYPE_PMBUS_EXTENDED_READ_BYTE_EXPECT;
  record->detailedRecordHeader.DeviceAddress = address;
  record->detailedRecordHeader.CommandCode = command;
  record->detailedRecordHeader.UsePec = pec ? 1 : 0;
  record->expectedDataByte = data;

  record->baseRecordHeader.Length = sizeof(*record);

  return (uint8_t *) record;
}


uint8_t *create_read_word_expect(uint16_t address, uint8_t command, uint16_t data, bool pec = false)
{
  t_RECORD_PMBUS_READ_WORD_EXPECT *record = malloc(sizeof(t_RECORD_PMBUS_READ_WORD_EXPECT));
  record->baseRecordHeader.RecordType = RECORD_TYPE_PMBUS_READ_WORD_EXPECT;
  record->detailedRecordHeader.DeviceAddress = address;
  record->detailedRecordHeader.CommandCode = command;
  record->detailedRecordHeader.UsePec = pec ? 1 : 0;
  record->expectedDataWord = data;

  record->baseRecordHeader.Length = sizeof(*record);

  return (uint8_t *) record;
}

uint8_t *create_extended_read_word_expect(uint16_t address, uint16_t command, uint16_t data, bool pec = false)
{
  t_RECORD_PMBUS_EXTENDED_READ_WORD_EXPECT *record = malloc(sizeof(t_RECORD_PMBUS_EXTENDED_READ_WORD_EXPECT));
  record->baseRecordHeader.RecordType = RECORD_TYPE_PMBUS_EXTENDED_READ_WORD_EXPECT;
  record->detailedRecordHeader.DeviceAddress = address;
  record->detailedRecordHeader.CommandCode = command;
  record->detailedRecordHeader.UsePec = pec ? 1 : 0;
  record->expectedDataWord = data;

  record->baseRecordHeader.Length = sizeof(*record);

  return (uint8_t *) record;
}

uint8_t *create_termination()
{
  tRecordHeaderLengthAndType *record = malloc(sizeof(tRecordHeaderLengthAndType));
  record->Length = 4;
  record->RecordType = 0x22;
  return (uint8_t *) record;
}

uint8_t *pack_records(uint8_t **records)
{
  uint8_t **working_records;
  uint8_t *working_record;
  uint16_t block_size = 0;
  tRecordHeaderLengthAndType *header;
  uint8_t *data;
  uint8_t *pos;

//  Serial.println("pack_records");
  working_records = records;

  while (*working_records != 0)
  {
    working_record = *working_records;
    working_records += 1;

    header = (pRecordHeaderLengthAndType) working_record;
    block_size += header->Length;

//    Serial.print("block_size "); Serial.println(block_size, DEC);

  }

  working_records = records;
  data = (uint8_t *) malloc(block_size);
  pos = data;

  while (*working_records != 0)
  {
    working_record = *working_records;
    working_records += 1;
        
    header = (pRecordHeaderLengthAndType) working_record;
//    Serial.print("header->Length "); Serial.println(header->Length, DEC);

    memcpy(pos, working_record, header->Length);
    pos += header->Length;

//    Serial.print("pos "); Serial.println(header->Length, DEC);


  }

  working_records = records;
  while (*working_records != 0)
  {
    working_record = *working_records;
    free(working_record);
    working_records += 1;

  }
  
  Serial.print("Block size: ");
  Serial.println(block_size, DEC);
//  Serial.println("exit pack_records");
  return data;
}