/*!

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
    @ingroup LTPSM_InFlightUpdate
    Library File
*/

#include "hex_file_parser.h"
#include "nvm_data_helpers.h"

#define DEBUG_PARSE_DATA 0
#define PARSE_DATA_LEN 64

uint8_t ascii_to_hex(uint8_t ascii)
{
  return 0;
}

uint8_t filter_terminations(uint8_t (*get_data)(void))
{
  uint8_t c = '\0';

  c = get_data();
  while (c == '\n' || c == '\r')
    c = get_data();
  return c;
}

uint8_t detect_colons(uint8_t (*get_data)(void))
{
  uint8_t c;

  c = get_data();
  if (c == ':')
  {
    Serial.print(".");
  }
  return c;
}

static uint8_t parse_data[PARSE_DATA_LEN];
static uint16_t parse_data_length = 0;
static uint16_t parse_data_position = 0;

void reset_parse_hex()
{
  parse_data_length = 0;
  parse_data_position = 0;
}

/*
 * Parse a list of hex file bytes returning a list of ltc record bytes
 *
 * in_data:   Function to get one line of data to parse.
 * return:    One byte of data
 *
 * Notes:     CRC is ignored. No error processing; original data must be correct.
 *        Copies char input data so that it can be const, and frees when done.
 *        It is best to use const input so that memory usage is only temporary.
 */
uint8_t parse_hex(uint8_t (*get_data)(void))
{
  uint8_t     start_code;
  uint16_t    byte_count;
  uint16_t    record_type;
//  uint32_t    address;
//  uint16_t    crc;
  uint16_t    i;
  char      data[5];

  if (parse_data_position == parse_data_length)
  {
    // Get colon
    start_code = 0x00;
    while (start_code != ':')
      start_code = get_data();

    data[0] = get_data();
    data[1] = get_data();
    data[2] = '\0';
    byte_count = httoi(data);
#if (DEBUG_PARSE_DATA)
    Serial.print(F("Cnt: "));
    Serial.println(byte_count, HEX);
#endif
    if (byte_count > PARSE_DATA_LEN)
      Serial.println(F("Parse_data size too samll"));

//    Serial.print(F("data size ")); Serial.println(byte_count, DEC);

    data[0] = get_data();
    data[1] = get_data();
    data[2] = get_data();
    data[3] = get_data();
    data[4] = '\0';
#if (DEBUG_PARSE_DATA)
    int address = httoi(data);
    Serial.print(F("Addr: "));
    Serial.println(address, HEX);
#endif

    data[0] = get_data();
    data[1] = get_data();
    data[2] = '\0';
    record_type = httoi(data);
#if (DEBUG_PARSE_DATA)
    Serial.print(F("Rec: "));
    Serial.println(record_type, HEX);
#endif
    if (record_type == 0)
    {
      for (i = 0; i < byte_count; i++)
      {
        data[0] = get_data();
        data[1] = get_data();
        data[2] = '\0';
        parse_data[i] = httoi(data);
#if (DEBUG_PARSE_DATA)
        Serial.print(F("Parse: "));
        Serial.print(data[0], HEX);
        Serial.print(F(" "));
        Serial.print(data[1], HEX);
        Serial.print(F(" "));
        Serial.println(parse_data[i], HEX);
#endif
      }

      data[0] = get_data();
      data[1] = get_data();
      data[2] = '\0';
#if (DEBUG_PARSE_DATA)
      int crc = httoi(data);
      Serial.print(F("Crc: "));
      Serial.println(crc, HEX);
#endif
      parse_data_position = 0;
      parse_data_length = byte_count;
    }
    else if (record_type == 1) // Make termination record
    {
      parse_data[0] = 4;
      parse_data[1] = 0;
      parse_data[2] = 0x22;
      parse_data[3] = 0;
      parse_data_position = 0;
      parse_data_length = 4;
    }
  }
#if (DEBUG_PARSE_DATA)
  Serial.print(F("Byte "));
  Serial.println(parse_data[parse_data_position], HEX);
#endif
  return parse_data[parse_data_position++];
}

uint16_t parse_records(uint8_t *in_data, uint16_t in_length, tRecordHeaderLengthAndType **out_records)
{
  uint16_t    in_position;
  uint16_t  out_position;
  tRecordHeaderLengthAndType *record;

  in_position = 0;
  out_position = 0;

  while (1)
  {
    record = (tRecordHeaderLengthAndType *) &in_data[in_position];
    *(out_records+out_position) = record;
    in_position += (*out_records[out_position]).Length;
    out_position += 1;

    if (in_position >= in_length)
      break;
  }

  return out_position;
}

pRecordHeaderLengthAndType parse_record(uint8_t (*get_data)(void))
{
  uint32_t  header;
  uint16_t  size;
  uint16_t  pos;
  pRecordHeaderLengthAndType record;

  header = (uint32_t)get_data() << 0 | (uint32_t)get_data() << 8 | (uint32_t)get_data() << 16 | (uint32_t)get_data() << 24;

  record = (pRecordHeaderLengthAndType) &header;
  size = record->Length;

  if (size > getMaxRecordSize())
  {
    Serial.println(F("record_data size too samll"));
    Serial.print(F("requires "));
    Serial.print(size, DEC);
    Serial.print(F(" has "));
    Serial.println(getMaxRecordSize());
    Serial.print(F("Record Type 0x"));
    Serial.println(record->RecordType, HEX);
  }

//  Serial.print(F("Record size ")); Serial.println(size, DEC);

  uint8_t *record_data = getRecordData();

  record_data[0] = (header >> 0) & 0xFF;
  record_data[1] = (header >> 8) & 0xFF;
  record_data[2] = (header >> 16) & 0xFF;
  record_data[3] = (header >> 24) & 0xFF;

  if (size <= getMaxRecordSize())
  {
    for (pos = 0; pos < size - 4; pos++)
      record_data[pos + 4] = get_data();
  }
  record = (pRecordHeaderLengthAndType) record_data;

  return record;
}

pRecordHeaderLengthAndType print_record(pRecordHeaderLengthAndType (*get_record)(void))
{
  pRecordHeaderLengthAndType record = get_record();

  switch (record->RecordType)
  {
    case 0x01:
      Serial.println(F("processWriteByteOptionalPEC(t_RECORD_PMBUS_WRITE_BYTE*);"));
      break;
    case 0x02:
      Serial.println(F("processWriteWordOptionalPEC(t_RECORD_PMBUS_WRITE_WORD*);"));
      break;
    case 0x04:
      Serial.println(F("processReadByteExpectOptionalPEC(t_RECORD_PMBUS_READ_BYTE_EXPECT*);"));
      break;
    case 0x05:
      Serial.println(F("processReadWordExpectOptionalPEC(t_RECORD_PMBUS_READ_WORD_EXPECT*);"));
      break;
    case 0x09:
      Serial.println(F("bufferNVMData(t_RECORD_NVM_DATA*);"));
      break;
    case 0x0A:
      Serial.println(F("processReadByteLoopMaskOptionalPEC(t_RECORD_PMBUS_READ_BYTE_LOOP_MASK*);"));
      break;
    case 0x0B:
      Serial.println(F("processReadWordLoopMaskOptionalPEC(t_RECORD_PMBUS_READ_WORD_LOOP_MASK*);"));
      break;
    case 0x0C:
      Serial.println(F("processPollReadByteUntilAckNoPEC(t_RECORD_PMBUS_POLL_READ_BYTE_UNTIL_ACK*);"));
      break;
    case 0x0D:
      Serial.println(F("processDelayMs(t_RECORD_DELAY_MS*);"));
      break;
    case 0x0E:
      Serial.println(F("processSendByteOptionalPEC(t_RECORD_PMBUS_SEND_BYTE*);"));
      break;
    case 0x0F:
      Serial.println(F("processWriteByteNoPEC(t_RECORD_PMBUS_WRITE_BYTE_NOPEC*);"));
      break;
    case 0x10:
      Serial.println(F("processWriteWordNoPEC(t_RECORD_PMBUS_WRITE_WORD_NOPEC*);"));
      break;
    case 0x12:
      Serial.println(F("processReadByteExpectNoPEC(t_RECORD_PMBUS_READ_BYTE_EXPECT_NOPEC*);"));
      break;
    case 0x13:
      Serial.println(F("processReadWordExpectNoPEC(t_RECORD_PMBUS_READ_WORD_EXPECT_NOPEC*);"));
      break;
    case 0x15:
      Serial.println(F("processReadByteLoopMaskNoPEC(t_RECORD_PMBUS_READ_BYTE_LOOP_MASK_NOPEC*);"));
      break;
    case 0x16:
      Serial.println(F("processReadWordLoopMaskNoPEC(t_RECORD_PMBUS_READ_WORD_LOOP_MASK_NOPEC*);"));
      break;
    case 0x17:
      Serial.println(F("processSendByteNoPEC(t_RECORD_PMBUS_SEND_BYTE_NOPEC*);"));
      break;
    case 0x18:
      Serial.println(F("processEvent(t_RECORD_EVENT*);"));
      break;
    case 0x19:
      Serial.println(F("processReadByteExpectMaskNoPEC(t_RECORD_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC*);"));
      break;
    case 0x1A:
      Serial.println(F("processReadWordExpectMaskNoPEC(t_RECORD_PMBUS_READ_WORD_EXPECT_MASK_NOPEC*);"));
      break;
    case 0x1B:
      Serial.println(F("processVariableMetaData(t_RECORD_VARIABLE_META_DATA*);"));
      break;
    case 0x1C:
      Serial.println(F("modifyWordNoPEC(t_RECORD_PMBUS_MODIFY_WORD_NO_PEC*);"));
      break;
    case 0x1D:
      Serial.println(F("modifyByteNoPEC(t_RECORD_PMBUS_MODIFY_BYTE_NO_PEC*);"));
      break;
    case 0x1E:
      Serial.println(F("writeNvmData(t_RECORD_NVM_DATA*);"));
      break;
    case 0x1F:
      Serial.println(F("read_then_verifyNvmData(t_RECORD_NVM_DATA*);"));
      break;
    case 0x20:
      Serial.println(F("modifyByteOptionalPEC(t_RECORD_PMBUS_MODIFY_BYTE*);"));
      break;
    case 0x21:
      Serial.println(F("modifyWordOptionalPEC(t_RECORD_PMBUS_MODIFY_WORD*);"));
      break;
    default :
      Serial.print(F("Unknown Record Type "));
      Serial.println(record->RecordType, HEX);
      break;
  }

  return record;
}
