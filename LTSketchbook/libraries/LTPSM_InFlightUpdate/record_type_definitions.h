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
    Library Header File
*/

#ifndef RECORD_TYPE_DEFINITIONS_
#define RECORD_TYPE_DEFINITIONS_

#include <stdint.h>
#include "record_type_basic_definitions.h"  /* Basic Record Type Definitions that the following specific record types are built upon */

/*******************************************************************/
/******** Record Type Definitions at a Glance **********************/
/*******************************************************************/
/*
  0x01 - RECORD_TYPE_PMBUS_WRITE_BYTE
  0x02 - RECORD_TYPE_PMBUS_WRITE_WORD
  0x03 - RECORD_TYPE_PMBUS_WRITE_BLOCK
  0x04 - RECORD_TYPE_PMBUS_READ_BYTE_EXPECT
  0x05 - RECORD_TYPE_PMBUS_READ_WORD_EXPECT
  0x06 - RECORD_TYPE_PMBUS_READ_BLOCK_EXPECT
  0x07 -    RECORD_TYPE_DEVICE_ADDRESS           -- OBSOLETED
  0x08 -    RECORD_TYPE_PACKING_CODE           -- OBSOLETED
  0x09 - RECORD_TYPE_NVM_DATA                -- FUNCTIONALITY CHANGED 25/01/2011
  0x0A - RECORD_TYPE_PMBUS_READ_BYTE_LOOP_MASK
  0x0B - RECORD_TYPE_PMBUS_READ_WORD_LOOP_MASK
  0x0C - RECORD_TYPE_PMBUS_POLL_UNTIL_ACK_NOPEC
  0x0D - RECORD_TYPE_DELAY_MS
  0x0E - RECORD_TYPE_PMBUS_SEND_BYTE
  0x0F - RECORD_TYPE_PMBUS_WRITE_BYTE_NOPEC
  0x10 - RECORD_TYPE_PMBUS_WRITE_WORD_NOPEC
  0x11 - RECORD_TYPE_PMBUS_WRITE_BLOCK_NOPEC
  0x12 - RECORD_TYPE_PMBUS_READ_BYTE_EXPECT_NOPEC
  0x13 - RECORD_TYPE_PMBUS_READ_WORD_EXPECT_NOPEC
  0x14 - RECORD_TYPE_PMBUS_READ_BLOCK_EXPECT_NOPEC
  0x15 - RECORD_TYPE_PMBUS_READ_BYTE_LOOP_MASK_NOPEC
  0x16 - RECORD_TYPE_PMBUS_READ_WORD_LOOP_MASK_NOPEC
  0x17 - RECORD_TYPE_PMBUS_SEND_BYTE_NOPEC
  0x18 - RECORD_TYPE_EVENT
  0x19 - RECORD_TYPE_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC
  0x1A - RECORD_TYPE_PMBUS_READ_WORD_EXPECT_MASK_NOPEC
  0x1B - RECORD_TYPE_VARIABLE_META_DATA
  0x1C - RECORD_TYPE_MODIFY_WORD_NOPEC
  0x1D - RECORD_TYPE_MODIFY_BYTE_NOPEC
  0x1E - RECORD_TYPE_PMBUS_WRITE_EE_DATA
  0x1F - RECORD_TYPE_PMBUS_READ_AND_VERIFY_EE_DATA
  0x20 - RECORD_TYPE_PMBUS_MODIFY_BYTE
  0x21 - RECORD_TYPE_PMBUS_MODIFY_WORD
  0x22 - RECORD_TYPE_END_OF_RECORDS
*/


#pragma pack(push, 1)


/*******************************************************************/
/******** Record Type Definitions in Detail            *************/
/********        Please refer to the PDF for more info *************/
/*******************************************************************/

/********************************************************************
 * Struct:          t_RECORD_PMBUS_WRITE_BYTE
 *
 * Overview:        PMBus write byte with optional PEC
 * Note:            RECORD_TYPE_PMBUS_WRITE_BYTE with value of 0x01 uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_WRITE_BYTE  (1)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithOptionalPEC detailedRecordHeader;
  uint8_t dataByte;
} t_RECORD_PMBUS_WRITE_BYTE;





/********************************************************************
 * Struct:          t_RECORD_PMBUS_WRITE_WORD
 *
 * Overview:        PMBus write word with optional PEC
 * Note:            RECORD_TYPE_PMBUS_WRITE_WORD with value of 0x02 uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_WRITE_WORD (2)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithOptionalPEC detailedRecordHeader;
  uint16_t dataWord;
} t_RECORD_PMBUS_WRITE_WORD;





/********************************************************************
 * Struct:          NOT DEFINED
 *
 * Overview:        PMBus Write Block with optional PEC
 * Note:            RECORD_TYPE_PMBUS_WRITE_BLOCK with value of 0x03 would use this struct
 *          RECORD TYPE NOT USED AND NOT DEFINED
 *******************************************************************/
#define RECORD_TYPE_PMBUS_WRITE_BLOCK (3)





/********************************************************************
 * Struct:          t_RECORD_PMBUS_READ_BYTE_EXPECT
 *
 * Overview:        PMBus Read Byte with optional PEC, and expect a specified value (fail if different)
 * Note:            RECORD_TYPE_PMBUS_READ_BYTE_EXPECT with value of 0x04 uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_READ_BYTE_EXPECT (4)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithOptionalPEC detailedRecordHeader;
  uint8_t expectedDataByte;
} t_RECORD_PMBUS_READ_BYTE_EXPECT;





/********************************************************************
 * Struct:          t_RECORD_PMBUS_READ_WORD_EXPECT
 *
 * Overview:        PMBus Read Word with optional PEC, and expect a specified value (fail if different)
 * Note:            t_RECORD_PMBUS_READ_WORD_EXPECT with value of 0x05 uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_READ_WORD_EXPECT (5)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithOptionalPEC detailedRecordHeader;
  uint16_t expectedDataWord;
} t_RECORD_PMBUS_READ_WORD_EXPECT;





/********************************************************************
 * Struct:          NOT DEFINED
 *
 * Overview:        PMBus Read Block with optional PEC, and expect a specified value (fail if different)
 * Note:            RECORD_TYPE_PMBUS_READ_BLOCK_EXPECT with value of 0x06 would use this struct
 *          RECORD TYPE NOT USED AND NOT DEFINED
 *******************************************************************/
#define RECORD_TYPE_PMBUS_READ_BLOCK_EXPECT (6)





/********************************************************************
 * Struct:          NOT DEFINED
 *
 * Overview:        Not Available
 * Note:            RECORD_TYPE_DEVICE_ADDRESS with value of 0x07 would use this struct
 *          RECORD TYPE NOT USED AND NOT DEFINED
 *******************************************************************/
#define RECORD_TYPE_DEVICE_ADDRESS (7)





/********************************************************************
 * Struct:          t_RECORD_PACKING_CODE
 *
 * Overview:        Not Available
 * Note:            RECORD_TYPE_PACKING_CODE with value of 0x08 would use this struct
 *          RECORD TYPE NOT USED BUT IS DEFINED
 *******************************************************************/
#define RECORD_TYPE_PACKING_CODE (8)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  uint16_t packingCode;
} t_RECORD_PACKING_CODE;





/********************************************************************
 * Struct:          t_RECORD_NVM_DATA
 *
 * Overview:        A block of NVM Data to be written as a block of bytes
 * Note:            t_RECORD_NVM_DATA with value of 0x09 uses this struct
 *          The actual NVM Data to be written is not absolutely defined in this
 *          struct because the length of the NVM data is variable
 *******************************************************************/
#define RECORD_TYPE_NVM_DATA (9)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithOptionalPEC detailedRecordHeader;
} t_RECORD_NVM_DATA;


/********************************************************************
 * Struct:          t_RECORD_PMBUS_READ_BYTE_LOOP_MASK
 *
 * Overview:        PMBus Read Byte with optional PEC in a loop until the device returns a specified value
 * Note:            RECORD_TYPE_PMBUS_READ_BYTE_LOOP_MASK with value of 0x0A uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_READ_BYTE_LOOP_MASK (0xA)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithOptionalPEC detailedRecordHeader;
  uint8_t byteMask;
  uint8_t expectedDataByte;
} t_RECORD_PMBUS_READ_BYTE_LOOP_MASK;





/********************************************************************
 * Struct:          t_RECORD_PMBUS_READ_WORD_LOOP_MASK
 *
 * Overview:        PMBus Read Word with optional PEC in a loop until the device returns a specified value
 * Note:            RECORD_TYPE_PMBUS_READ_WORD_LOOP_MASK with value of 0x0B uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_READ_WORD_LOOP_MASK (0xB)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithOptionalPEC detailedRecordHeader;
  uint16_t wordMask;
  uint16_t expectedDataWord;
} t_RECORD_PMBUS_READ_WORD_LOOP_MASK;





/********************************************************************
 * Struct:          t_RECORD_PMBUS_POLL_READ_BYTE_UNTIL_ACK
 *
 * Overview:        Poll a device at an address until it Acknowledges (ACKs)
 * Note:            t_RECORD_PMBUS_POLL_READ_BYTE_UNTIL_ACK with value
 *          of 0x0C uses this struct. This record type indicates
 *          that the software should continually attempt to
 *          execute a PMBus read byte operation until the
 *          slave devices acknowledges the command code or until
 *          the timeout value in milliseconds is reached. This
 *          is typically used to wait until the device is
 *          non-busy. This never uses PEC
 *******************************************************************/
#define RECORD_TYPE_PMBUS_POLL_UNTIL_ACK_NOPEC (0xC)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithoutPEC detailedRecordHeader;
  uint16_t timeout_in_ms;
} t_RECORD_PMBUS_POLL_READ_BYTE_UNTIL_ACK;





/********************************************************************
 * Struct:          t_RECORD_DELAY_MS
 *
 * Overview:        Simple delay function. Idle the bus for a specified
 *          amount of time
 * Note:            RECORD_TYPE_DELAY_MS with value of 0x0D uses this
 *          struct. This record type indicates that the software
 *          should delay execution for a specified number of
 *          milliseconds before proceeding.
 *******************************************************************/
#define RECORD_TYPE_DELAY_MS (0xD)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  uint16_t numMs;
} t_RECORD_DELAY_MS;





/********************************************************************
 * Struct:          t_RECORD_PMBUS_SEND_BYTE
 *
 * Overview:        This record type indicates that the software
 *          should execute a PMBus send byte operation
 * Note:            RECORD_TYPE_PMBUS_SEND_BYTE with value of 0x0E
 *          uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_SEND_BYTE (0xE)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithOptionalPEC detailedRecordHeader;
} t_RECORD_PMBUS_SEND_BYTE;





/********************************************************************
 * Struct:          t_RECORD_PMBUS_WRITE_BYTE_NOPEC
 *
 * Overview:        PMBus write byte WITHOUT optional PEC. PEC is *never* used
 * Note:            RECORD_TYPE_PMBUS_WRITE_BYTE_NOPEC with value of 0x0F uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_WRITE_BYTE_NOPEC (0xF)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithoutPEC detailedRecordHeader;
  uint8_t dataByte;
} t_RECORD_PMBUS_WRITE_BYTE_NOPEC;





/********************************************************************
 * Struct:          t_RECORD_PMBUS_WRITE_WORD_NOPEC
 *
 * Overview:        PMBus write word WITHOUT optional PEC. PEC is *never* used
 * Note:            RECORD_TYPE_PMBUS_WRITE_WORD_NOPEC with value of 0x10 uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_WRITE_WORD_NOPEC (0x10)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithoutPEC detailedRecordHeader;
  uint16_t dataWord;
} t_RECORD_PMBUS_WRITE_WORD_NOPEC;





/********************************************************************
 * Struct:          NOT DEFINED
 *
 * Overview:        Same as RECORD_TYPE_PMBUS_WRITE_BLOCK except PEC is never used
 * Note:            RECORD_TYPE_PMBUS_WRITE_BLOCK_NOPEC with value of 0x11 would use this struct
 *          RECORD TYPE NOT USED AND NOT DEFINED
 *******************************************************************/
/* RECORD TYPE NOT USED AND NOT DEFINED */
#define RECORD_TYPE_PMBUS_WRITE_BLOCK_NOPEC (0x11)





/********************************************************************
 * Struct:          t_RECORD_PMBUS_READ_BYTE_EXPECT_NOPEC
 *
 * Overview:        PMBus Read Byte without PEC, and expect a specified value (fail if different)
 * Note:            RECORD_TYPE_PMBUS_READ_BYTE_EXPECT_NOPEC with value of 0x12 uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_READ_BYTE_EXPECT_NOPEC (0x12)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithoutPEC detailedRecordHeader;
  uint8_t expectedDataByte;
} t_RECORD_PMBUS_READ_BYTE_EXPECT_NOPEC;





/********************************************************************
 * Struct:          t_RECORD_PMBUS_READ_WORD_EXPECT_NOPEC
 *
 * Overview:        PMBus Read Word without PEC, and expect a specified value (fail if different)
 * Note:            RECORD_TYPE_PMBUS_READ_WORD_EXPECT_NOPEC with value of 0x13 uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_READ_WORD_EXPECT_NOPEC (0x13)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithoutPEC detailedRecordHeader;
  uint16_t expectedDataWord;
} t_RECORD_PMBUS_READ_WORD_EXPECT_NOPEC;





/********************************************************************
 * Struct:          NOT DEFINED
 *
 * Overview:        Same as RECORD_TYPE_PMBUS_READ_BLOCK_EXPECT except PEC is never used
 * Note:            RECORD_TYPE_PMBUS_READ_BLOCK_EXPECT_NOPEC with value of 0x14 would use this struct
 *          RECORD TYPE NOT USED AND NOT DEFINED
 *******************************************************************/
#define RECORD_TYPE_PMBUS_READ_BLOCK_EXPECT_NOPEC (0x14)





/********************************************************************
 * Struct:          t_RECORD_PMBUS_READ_BYTE_LOOP_MASK_NOPEC
 *
 * Overview:        PMBus Read Byte without PEC in a loop until the
 *          device returns a specified value
 * Note:            RECORD_TYPE_PMBUS_READ_BYTE_LOOP_MASK_NOPEC with
 *          value of 0x15 uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_READ_BYTE_LOOP_MASK_NOPEC (0x15)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithoutPEC detailedRecordHeader;
  uint8_t byteMask;
  uint8_t expectedDataByte;
} t_RECORD_PMBUS_READ_BYTE_LOOP_MASK_NOPEC;





/********************************************************************
 * Struct:          t_RECORD_PMBUS_READ_WORD_LOOP_MASK_NOPEC
 *
 * Overview:        PMBus Read Word without PEC in a loop until the
 *          device returns a specified value
 * Note:            RECORD_TYPE_PMBUS_READ_WORD_LOOP_MASK_NOPEC with
 *          value of 0x16 uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_READ_WORD_LOOP_MASK_NOPEC (0x16)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithoutPEC detailedRecordHeader;
  uint16_t wordMask;
  uint16_t expectedDataWord;
} t_RECORD_PMBUS_READ_WORD_LOOP_MASK_NOPEC;





/********************************************************************
 * Struct:          t_RECORD_PMBUS_SEND_BYTE_NOPEC
 *
 * Overview:        This record type indicates that the software should execute a PMBus send byte operation and never using PEC
 * Note:            RECORD_TYPE_PMBUS_SEND_BYTE_NOPEC with value of 0x17 uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_SEND_BYTE_NOPEC (0x17)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithoutPEC detailedRecordHeader;
} t_RECORD_PMBUS_SEND_BYTE_NOPEC;





/********************************************************************
 * Struct:          t_RECORD_EVENT
 *
 * Overview:        Indicates that we are at a particular place in
 *          the In-Circuit Programming file [recipe file]
 * Note:            RECORD_TYPE_EVENT with value of 0x18 uses this
 *          struct. This can be used for custom user
 *          interactions or at a basic level debug prompts/actions
 *******************************************************************/
#define RECORD_TYPE_EVENT (0x18)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  uint16_t eventId;
} t_RECORD_EVENT;

/********************************************************************
 * Definitions: NAME (VALUE) NOTE
 *    BEFORE_BEGIN      (0) This event is fired before any
 *                  commands are issued to communicate
 *                  with the system
 *    SYSTEM_BEFORE_PROGRAM   (1) This event is fired before any
 *                  commands are issued to program
 *                  the NVM
 *    SYSTEM_BEFORE_VERIFY  (2) This event is fired after programming
 *                  the system before any commands are
 *                  issued to verify the NVM
 *    AFTER_DONE        (3) This event is fired after the end
 *                  of the entire programming and
 *                  verification sequence is complete
 *******************************************************************/
#define BEFORE_BEGIN (0x00)
#define BEFORE_INSYSTEM_PROGRAMMING_BEGIN (0x10)
#define SYSTEM_BEFORE_PROGRAM (0x01)
#define INSYSTEM_CHIP_BEFORE_PROGRAM (0x11)
#define SYSTEM_BEFORE_VERIFY (0x02)
#define INSYSTEM_CHIP_BEFORE_VERIFY (0x12)
#define INSYSTEM_CHIP_AFTER_VERIFY (0x13)
#define SYSTEM_AFTER_VERIFY (0x04)
#define AFTER_DONE (0x03)


/********************************************************************
 * Struct:          t_RECORD_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC
 *
 * Overview:        PMBus Read Byte without PEC, and expect a specified
 *          value bitwise ANDed with the Mask (fail if different)
 * Note:            RECORD_TYPE_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC with
 *          value of 0x19 uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC (0x19)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithoutPEC detailedRecordHeader;
  uint8_t expectedDataByte;
  uint8_t byteMask;
} t_RECORD_PMBUS_READ_BYTE_EXPECT_MASK_NOPEC;






/********************************************************************
 * Struct:          t_RECORD_PMBUS_READ_WORD_EXPECT_MASK_NOPEC
 *
 * Overview:        PMBus Read Word without PEC, and expect a specified
 *          value bitwise ANDed with the Mask (fail if different)
 * Note:            RECORD_TYPE_PMBUS_READ_WORD_EXPECT_MASK_NOPEC with
 *          value of 0x1A uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_READ_WORD_EXPECT_MASK_NOPEC (0x1A)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithoutPEC detailedRecordHeader;
  uint16_t expectedDataWord;
  uint16_t wordMask;
} t_RECORD_PMBUS_READ_WORD_EXPECT_MASK_NOPEC;





/********************************************************************
 * Struct:          t_RECORD_VARIABLE_META_DATA
 *
 * Overview:        Generic instruction used to communicate meta data
 *          to the host processor
 * Note:            RECORD_TYPE_VARIABLE_META_DATA with value of 0x1B
 *          uses this struct. Currently setting the global
 *          base address and serialization are supported
 *******************************************************************/
#define RECORD_TYPE_VARIABLE_META_DATA (0x1B)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  uint16_t metaDataType;
} t_RECORD_VARIABLE_META_DATA;

/********************************************************************
 * Struct:          t_RECORD_META_SET_GLOBAL_BASE_ADDRESS
 * Overview:        A META DATA struct containing the new global base address
 *******************************************************************/
#define META_SET_GLOBAL_BASE_ADDRESS (0)
typedef struct
{
  t_RECORD_VARIABLE_META_DATA metaHeader;
  uint16_t globalBaseAddressInWordFormat;
} t_RECORD_META_SET_GLOBAL_BASE_ADDRESS;


/********************************************************************
 * Struct:          t_RECORD_META_OEM_SERIAL_NUMBER
 * Overview:        A META DATA struct containing the serial number of the device to program
 *******************************************************************/
#define META_OEM_SERIAL_NUMBER (1)
typedef struct
{
  t_RECORD_VARIABLE_META_DATA metaHeader;
  uint16_t serialNumber;
} t_RECORD_META_OEM_SERIAL_NUMBER;





/********************************************************************
 * Struct:          t_RECORD_TYPE__MODIFY_WORD_NOPEC
 *
 * Overview:        This record type indicates that the software should
 *          read a word, apply a mask with new data to it, and
 *          then write it back to the device never using PEC
 * Note:            RECORD_TYPE_MODIFY_WORD_NOPEC with value of 0x1C
 *          uses this struct. Modify only the specified bits
 *          (set to 1 in wordMask) to be desiredDataWord,
 *          never use PEC
 *******************************************************************/
#define RECORD_TYPE_MODIFY_WORD_NOPEC (0x1C)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithOptionalPEC detailedRecordHeader;
  uint16_t wordMask;
  uint16_t desiredDataWord;
} t_RECORD_PMBUS_MODIFY_WORD_NO_PEC;





/********************************************************************
 * Struct:          t_RECORD_TYPE__MODIFY_BYTE_NOPEC
 *
 * Overview:        This record type indicates that the software should
 *          read a byte, apply a mask with new data to it, and
 *          then write it back to the device never using PEC
 * Note:            RECORD_TYPE_MODIFY_BYTE_NOPEC with value of 0x1D
 *          uses this struct. Modify only the specified bits
 *          (set to 1 in byteMask) to be desiredDataByte,
 *          never use PEC
 *******************************************************************/
#define RECORD_TYPE_MODIFY_BYTE_NOPEC (0x1D)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithOptionalPEC detailedRecordHeader;
  uint8_t byteMask;
  uint8_t desiredDataByte;
} t_RECORD_PMBUS_MODIFY_BYTE_NO_PEC;





/********************************************************************
 * Struct:          t_RECORD_TYPE_PMBUS_WRITE_EE_DATA
 *
 * Overview:        Write data that was previously specified in the
 *          NVM_DATA record and stored in the global linked
 *          list to the slave
 * Note:            RECORD_TYPE_PMBUS_WRITE_EE_DATA with value of 0x1E
 *          uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_WRITE_EE_DATA (0x1E)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithOptionalPEC detailedRecordHeader;

} t_RECORD_TYPE_PMBUS_WRITE_EE_DATA;





/********************************************************************
 * Struct:          t_RECORD_TYPE_PMBUS_VERIFY_EE_DATA
 *
 * Overview:        Read back and verify NVM data that was previously specified in the NVM_DATA record and stored in the global linked list from the slave
 * Note:            RECORD_TYPE_PMBUS_READ_AND_VERIFY_EE_DATA with value of 0x1F uses this struct
 *******************************************************************/
#define RECORD_TYPE_PMBUS_READ_AND_VERIFY_EE_DATA (0x1F)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithOptionalPEC detailedRecordHeader;

} t_RECORD_TYPE_PMBUS_VERIFY_EE_DATA;





/********************************************************************
 * Struct:          t_RECORD_PMBUS_MODIFY_BYTE
 *
 * Overview:        This record type indicates that the software should
 *          read a byte, apply a mask with new data to it, and
 *          then write it back to the device optionally using PEC
 * Note:            RECORD_TYPE_PMBUS_MODIFY_BYTE with value of 0x20
 *          uses this struct. Modify only the specified bits
 *          (set to 1 in byteMask) to be desiredDataByte,
 *          optionally using PEC
 *******************************************************************/
// Modify only the specified word bits (set to 1 in MaskData) to be DesiredData, optional PEC
#define RECORD_TYPE_PMBUS_MODIFY_BYTE (0x20)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithOptionalPEC detailedRecordHeader;
  uint8_t byteMask;
  uint8_t desiredDataByte;
} t_RECORD_PMBUS_MODIFY_BYTE;





/********************************************************************
 * Struct:          t_RECORD_PMBUS_MODIFY_WORD
 *
 * Overview:        This record type indicates that the software should
 *          read a word, apply a mask with new data to it, and
 *          then write it back to the device optionally using PEC
 * Note:            RECORD_TYPE_PMBUS_MODIFY_WORD with value of 0x21
 *          uses this struct. Modify only the specified bits
 *          (set to 1 in wordMask) to be desiredDataWord,
 *          optionally using PEC
 *******************************************************************/
#define RECORD_TYPE_PMBUS_MODIFY_WORD (0x21)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithOptionalPEC detailedRecordHeader;
  uint16_t wordMask;
  uint16_t desiredDataWord;
} t_RECORD_PMBUS_MODIFY_WORD;

/********************************************************************
 * Struct:          t_RECORD_TYPE_END_OF_RECORDS
 *
 * Overview:        This record type indicates there are no more records.
 * Note:
 *******************************************************************/
#define RECORD_TYPE_END_OF_RECORDS (0x22)
typedef struct
{
  tRecordHeaderLengthAndType baseRecordHeader;
  tRecordHeaderAddressAndCommandWithOptionalPEC detailedRecordHeader;
} t_RECORD_TYPE_END_OF_RECORDS;

#pragma pack(pop)
/////////////////////////////////////////////////////
// End of Record Type Definitions
/////////////////////////////////////////////////////
#endif
