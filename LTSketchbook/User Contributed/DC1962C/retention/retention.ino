/*
Linear Technology DC1962A/C Demonstration Board
LTC3880, LTC2974, LTC2977/8: Power Management Solution for Application Processors
Example application of NVM Retention

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim

http://www.linear.com/product/LTC3880
http://www.linear.com/product/LTC2974
http://www.linear.com/product/LTC2977

http://www.linear.com/demo/DC1962C


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

#include <Arduino.h>
#include <Linduino.h>
#include <UserInterface.h>
#include <avr/boot.h>

#include <LT_SMBus.h>
#include <LT_SMBusPec.h>
#include <LT_SMBusNoPec.h>
#include <LT_SMBusGroup.h>
#include <LT_PMBusMath.h>

// Some bit definitions for MFR_COMMON
#define CML      1
#define BUSY     7
#define RESERVED2  2

// Control over polling with PEND
// Set USE_PEND to 0 for LTC3883 which does not support PEND
#define USE_PEND        1

#define NOT_BUSY        1 << 6
#define NOT_TRANS       1 << 4
#ifdef USE_PEND
#define NOT_PENDING     1 << 5
#else
#define NOT_PENDING     0
#endif

// PMBus Commands
#define PAGE                    0x00
#define CLEAR_FAULTS            0x03
#define STORE_USER_ALL          0x15
#define RESTORE_USER_ALL        0x16
#define STATUS_BYTE             0x78
#define STATUS_WORD             0x79
#define STATUS_CML              0x7E
#define STATUS_MFR_SPECIFIC     0x80
#define READ_ITEMP_2977_8       0x8D
#define READ_ITEMP              0x8E
#define USER_DATA_04            0xB4
#define MFR_EE_UNLOCK           0xBD
#define MFR_EE_ERASE            0xBE
#define MFR_EE_DATA             0xBF
#define MFR_PAGE_FF_MASK        0xE4
#define MFR_FAULT_LOG_STORE     0xEA
#define MFR_FAULT_LOG_RESTORE   0xEB
#define MFR_FAULT_LOG_CLEAR     0xEC
#define MFR_SPECIAL_ID          0xE7
#define MFR_COMMON              0xEF
#define MFR_EEPROM_STATUS       0xF1
#define MFR_SPARE_0             0xF7
#define MFR_RESET               0xFD

// Bit definitions for STATUS_CML
#define STATUS_CML_CMD_FAULT            0x80
#define STATUS_CML_DATA_FAULT           0x40
#define STATUS_CML_PEC_FAULT            0x20
#define STATUS_CML_MEM_FAULT            0x10
#define STATUS_CML_PROC_FAULT           0x04
#define STATUS_CML_PMBUS_FAULT          0x02
#define STATUS_CML_UNKNOWN_FAULT        0x01

// Bit definitions for STATUS_MFR_SPECIFIC
#define STATUS_MFR_SPECIFIC_NVM_CRC     0x20

// Bit definitions for STATUS_WORD
#define STATUS_WORD_POWER_GOOD_B        0x0800

#define LTC3880_I2C_ADDRESS 0x30
#define LTC2974_I2C_ADDRESS 0x32
// (Covers DC1962A/C. A has LTC2978, C has LTC2977)
#define LTC2977_8_I2C_ADDRESS 0x33

// Used for reading and writing NVM.
// Read will allocate or reallocate data.
// Write will free it and set the pointer to NULL.
static uint16_t *ltc3880data;
static uint16_t ltc3880dataCount;
static uint16_t *ltc2974data;
static uint16_t ltc2974dataCount;
static uint16_t *ltc2977data;
static uint16_t ltc2977dataCount;

// Used to talk to SMBus
LT_SMBusGroup *smbus = (LT_SMBusGroup *) new LT_SMBusNoPec();

bool is_ltc297x(uint8_t address)
{
  uint16_t chipId;

  chipId = smbus->readWord(address, MFR_SPECIAL_ID) >> 8;
  if (
    (chipId == 0x021) // LTC2974
    || (chipId == 0x022) // LTC2975
    || (chipId == 0x013) // LTC2977
    || (chipId == 0x803) // LTC2980
    || (chipId == 0x804) // LTC2980
    || (chipId == 0x801) // LTM2987
    || (chipId == 0x802) // LTM2987
    || (chipId == 0x011) // LTC2978
    || (chipId == 0x012) // LTC2978
  )
    return true;
  else
    return false;
}

bool is_ltc2977(uint8_t address)
{
  uint16_t chipId;

  chipId = smbus->readWord(address, MFR_SPECIAL_ID) >> 8;
  if (
    (chipId == 0x013) // LTC2977
    || (chipId == 0x803) // LTC2980
    || (chipId == 0x804) // LTC2980
    || (chipId == 0x801) // LTM2987
    || (chipId == 0x802) // LTM2987
  )
    return true;
  else
    return false;
}

bool is_ltc2978(uint8_t address)
{
  bool is_ltc2978;
  uint8_t mfr_common;
  mfr_common = smbus->readByte(address, MFR_COMMON);
  is_ltc2978 = ((mfr_common & (1 << RESERVED2)) == 0);
  return is_ltc2978;
}

uint8_t wait_for_ack(uint8_t address, uint8_t command)
{
  uint8_t data;
  // A real application should timeout at 4.1 seconds.
  uint16_t timeout = 8192;
  while (timeout-- > 0)
  {
    if (0 == smbus->i2cbus()->readByteData(address, command, &data))
      return 1;
  }
  return 0;
}

void wait_for_ready(uint8_t address)
{
  uint8_t status;
  uint8_t mask;
  uint8_t page;

  // All devices poll on Ack, but the LTC2978/A will assert ALERTB if busy.
  // By waiting 1ms, the only ALERTBs will be during a store or restore user all.
  // This value can be tuned lower based on the clock rate, but should be well
  // tested if the expectation is no faults except on store/restore.
  delay(1);
  wait_for_ack(address, MFR_COMMON);
  if (is_ltc2978(address))
  {
    // Check if there is a CML or BUSY fault, and clear faults,
    // but only if there are no other faults, because another
    // thread or function may be interested in them.

    // Make sure we can use PAGE 0xFF to clear faults, but save
    // off the mask and page so it can be restored.
    mask = smbus->readByte(address, MFR_PAGE_FF_MASK);
    smbus->writeByte(address, MFR_PAGE_FF_MASK, 0xFF);
    page = smbus->readByte(address, PAGE);
    smbus->writeByte(address, PAGE, 0xFF);

    // Clear if CML/BUSY but not other faults.
    status = smbus->readByte(address, STATUS_BYTE);
    if ((status &  ((1 << CML) | (1 << BUSY))) &&
        !(status & ~((1 << CML) | (1 << BUSY))))
      smbus->sendByte(address, CLEAR_FAULTS);

    // Restore page and mask.
    smbus->writeByte(address, PAGE, page);
    smbus->writeByte(address, MFR_PAGE_FF_MASK, mask);
  }
  else
    // All other devices have a busy bit to poll.
    // Controllers also have a EEPROM register that can be
    // polled to wait for internal operations to complete.
    // This is not in general polling, because controllers
    // still operate during this time.
    wait_for_not_busy(address);
}

uint8_t wait_for_not_busy(uint8_t address)
{
  uint8_t mfr_common;
  // A real application should timeout at 4.1 seconds.
  uint16_t timeout = 8192;

  while (timeout-- > 0)
  {
    mfr_common = smbus->readByte(address, MFR_COMMON);
    // If too busy to answer, poll again.
    if (mfr_common == 0xFF)
      continue;
    if ((mfr_common  & (NOT_BUSY | NOT_PENDING | NOT_TRANS)) == (NOT_BUSY | NOT_PENDING | NOT_TRANS))
      return 1;
  }
  return 0;
}

uint8_t wait_for_nvm_done(uint8_t address)
{
  uint8_t mfr_eeprom_status;
  // A real application should timeout at 4.1 seconds.
  uint16_t timeout = 8192;

  while (timeout-- > 0)
  {
    wait_for_ack(address, 0x00);
    wait_for_not_busy(address);
    mfr_eeprom_status = smbus->readByte(address, MFR_EEPROM_STATUS);
    if (mfr_eeprom_status == 0xFF)
      continue;
    if ((mfr_eeprom_status & 0xC0) == 0)
      return 1;
  }
  return 0;
}

void store_fault_logs()
{
  smbus->sendByte(0x5B, MFR_FAULT_LOG_STORE);
}

void clear_fault_logs()
{
  smbus->sendByte(0x5B, MFR_FAULT_LOG_CLEAR);
}

void clear_faults_global(void)
{
  // This clear assumes the MASK regsiter of managers is set to 0xFF.
  // If not, there is no general way to globally clear faults, as the code
  // would have to know all the device addresses, types, etc. It also assumes that
  // any other code that depends on PAGE will reset the page. In the
  // context of refresh, it means that clear faults global disturbs
  // the state and other code may have side effects if not dealt with.
  // This reset does not work for LTC2978/A which does not work with page 0xFF.

  // This clear is good for controllers/managers which respond to page 0xFF
  // during the clear. This is all PSM devices except LTC2978/A
  smbus->writeByte(0x5B, PAGE, 0xFF);
  smbus->sendByte(0x5B, CLEAR_FAULTS);

  // This clear will clear page 0x00 of a LTC2978/A, however to
  // really clear an LTC2978/A, requres a loop with pages, as
  // shown in the commented code. There is no general way to clear
  // faults on a LTC2978/A without a restore or power cycle.

//  for (int page; page < 8; page++) {
//    smbus->writeByte(0x5B, PAGE, page);
  smbus->writeByte(0x5B, PAGE, 0x00);
  smbus->sendByte(0x5B, CLEAR_FAULTS);
//}

}

void lock_nvm(uint8_t address)
{
  smbus->writeByte(address, MFR_EE_UNLOCK, 0x00);
  wait_for_ready(address);
}

void unlock_nvm(uint8_t address)
{
  // Unlock device for write normal data, not all data
  smbus->writeByte(address, MFR_EE_UNLOCK, 0x2B);
  wait_for_ready(address);
  if (smbus->pecEnabled())
    smbus->writeByte(address, MFR_EE_UNLOCK, 0xD5);
  else
    smbus->writeByte(address, MFR_EE_UNLOCK, 0xD4);
  wait_for_ready(address);
}

void nvm_lock_reset(uint8_t address)
{
  lock_nvm(address);
  unlock_nvm(address);
  lock_nvm(address);
}

bool check_nvm_data_checksum(uint8_t address)
{
  uint16_t w;
  uint8_t bs[32];
  uint16_t count;
  uint8_t bpos = 0, wpos;
  bool nok = false;

  // Unlock
  unlock_nvm(address);

  // Read preamble and count
  smbus->readWord(address, MFR_EE_DATA);
  wait_for_ready(address);
  count = smbus->readWord(address, MFR_EE_DATA);
  wait_for_ready(address);

  for (wpos = 0; wpos < count/16; wpos++)
  {
    while (bpos < 32)
    {
      w = smbus->readWord(address, MFR_EE_DATA);
      wait_for_ready(address);
      bs[bpos++] = w & 0xFF;
      bs[bpos++] = (w >> 8) & 0xFF;
    }
    // Compute CRC on 32 byte boundary
    nok |= smbus->checkCRC(bs);
    bpos = 0;
  }

  lock_nvm(address);
  return nok;
}

void erase(uint8_t address, uint16_t **data)
{
  if (is_ltc2978(address))
    Serial.println(F("  Erase not supported on LTC2978"));
  if (*data == NULL)
  {
    Serial.println(F("  No Data, please read data before erasing"));
  }
  else
  {
    unlock_nvm(address);
    smbus->writeByte(address, MFR_EE_ERASE, 0x2B); // Erase
    wait_for_ready(address);
    lock_nvm(address);
  }
}

// If a device has a memory fault, and has been through a power cycle or reset, communcation
// will probably fail during the check, as the address of the device may not be correct.
bool check_health(uint8_t address)
{
  bool res = true;

  if (is_ltc2978(address))
  {
    if (smbus->readByte(address, STATUS_CML) & STATUS_CML_MEM_FAULT)
    {
      res = false;
      Serial.println(F("  Device has a Memory Fault"));
    }
    else if (smbus->readWord(address, STATUS_WORD) & ~STATUS_WORD_POWER_GOOD_B) // Assumes chann
    {
      res = false;
      Serial.println(F("  Device has a Fault"));
    }
    else if (readItemp2977_8(address) > 85.0) // Really ITemp on LTC297X
    {
      res = false;
      Serial.println(F("  Device is too hot"));
    }
    else
    {
      Serial.println(F("  Checksum not verified (LTC2978/A does not support direct read of NVM)"));
      Serial.println(F("  Device is Ok (No Memory Fault, Not Hot)"));
    }
  }
  else
  {
    if (smbus->readByte(address, STATUS_CML) & STATUS_CML_MEM_FAULT)
    {
      res = false;
      Serial.println(F("  Device has a Memory Fault"));
    }
    else if (smbus->readWord(address, STATUS_WORD) & ~STATUS_WORD_POWER_GOOD_B)
    {
      res = false;
      Serial.println(F("  Device has a Fault"));
    }
    // This could be converted to a positive check rather than a negative check.
    // This depends on left to right execution.
    else if ((!is_ltc297x(address)) && ((smbus->readByte(address, STATUS_MFR_SPECIFIC) & STATUS_MFR_SPECIFIC_NVM_CRC)))
    {
      res = false;
      Serial.println(F("  Device has a Memory Fault"));
    }
    // This depends on left to right execution.
    else if ((is_ltc2977(address) && (readItemp2977_8(address) > 85.0)) || (!is_ltc2977(address) && (readItemp(address) > 85.0)))
    {
      res = false;
      Serial.println(F("  Device is hot"));
    }
    else if (check_nvm_data_checksum(address))
    {
      res = false;
      Serial.println(F("  Device has an invalid CRC"));
    }
    else
      Serial.println(F("  Device is Ok (No Memory Fault, Not Hot, CRC is Good)"));
  }
  return res;
}


void increment_counter(uint8_t address)
{
  // For LTC388X and LTC2974/5/7 use USER_DATA_4
  // For LTC2978/A use MFR_SPARE_0

  uint16_t user;

  if (is_ltc2978(address))
  {
    user = smbus->readWord(address, MFR_SPARE_0);
    user++;
    smbus->writeWord(address, MFR_SPARE_0, user);
    // No need to read again in typical application
    user = smbus->readWord(address, MFR_SPARE_0);
  }
  else
  {
    user = smbus->readWord(address, USER_DATA_04);
    user++;
    smbus->writeWord(address, USER_DATA_04, user);
    // No need to read again in typical application
    user = smbus->readWord(address, USER_DATA_04);
  }
}

void print_counter(uint8_t address)
{
  // For LTC388X and LTC2974/5/7 use USER_DATA_4
  // For LTC2978/A use MFR_SPARE_0

  uint16_t user;

  if (is_ltc2978(address))
  {
    user = smbus->readWord(address, MFR_SPARE_0);
    Serial.println(user, DEC);
  }
  else
  {
    user = smbus->readWord(address, USER_DATA_04);
    Serial.println(user, DEC);
  }
}

void store ()
{
  smbus->sendByte(0x5B, STORE_USER_ALL);
}

void restore ()
{
  smbus->sendByte(0x5B, RESTORE_USER_ALL);
}

void read_bytes(uint8_t address, uint16_t **data, uint16_t *count)
{
  int i;

  // Ensure we are not busy
  wait_for_ready(address);

  nvm_lock_reset(address);
  unlock_nvm(address);

  // Read preamble
  smbus->readWord(address, MFR_EE_DATA); // Read ID
  wait_for_ready(address);
  *count = smbus->readWord(address, MFR_EE_DATA);
  wait_for_ready(address);

  // If we don't have memory yet, get some.
  if (*data == NULL)
    *data = (uint16_t *) malloc((*count) * sizeof(uint16_t));
  else
  {
    free(*data);
    *data = (uint16_t *) malloc((*count) * sizeof(uint16_t));
  }

  // Read the data word at a time.
  for (i = 0; i < *count; i++)
  {
    (*data)[i] = smbus->readWord(address, MFR_EE_DATA);
    wait_for_ready(address);
  }

  lock_nvm(address);
}

void write_bytes(uint8_t address, uint16_t **data, uint16_t *count)
{
  int i;
  uint16_t d;

  if (*data == NULL)
  {
    Serial.println(F("  No Data, please read data before writing"));
  }
  else
  {
    // Ensure we are not busy
    wait_for_ready(address);

    // To improve reliability
    nvm_lock_reset(address);

    // Erase the data to enable write
    unlock_nvm(address);
    smbus->writeByte(address, MFR_EE_ERASE, 0x2B); // Erase
    // Very important to wait here
    wait_for_ready(address);
    lock_nvm(address);

    // Write the data
    unlock_nvm(address);
    for (i = 0; i < *count; i++)
    {
      smbus->writeWord(address, MFR_EE_DATA, (*data)[i]);
      // Very important to wait here
      wait_for_ready(address);
    }
    lock_nvm(address);

    // Verify the data
    unlock_nvm(address);    // Read preamble
    smbus->readWord(address, MFR_EE_DATA); // Read ID
    wait_for_ready(address);
    *count = smbus->readWord(address, MFR_EE_DATA);
    wait_for_ready(address);
    // Read the data word at a time.
    for (i = 0; i < *count; i++)
    {
      if (d = (smbus->readWord(address, MFR_EE_DATA) != (*data)[i]))
      {
        Serial.print(F("  Data Mismatch ("));
        Serial.print(i, DEC);
        Serial.print(F(","));
        Serial.print(d, HEX);
        Serial.print(F(","));
        Serial.print((*data)[i], HEX);
        Serial.println(F(")"));
      }
    }
    lock_nvm(address);
  }
}

float readItemp2977_8(uint8_t address)
{
  uint16_t temp_L11;

  // read the output current as an L11
  temp_L11 = smbus->readWord(address, READ_ITEMP_2977_8);

  // convert L11 value to floating point value
  return math_.lin11_to_float(temp_L11);      //! 2) Convert from Lin11
}

float readItemp(uint8_t address)
{
  uint16_t temp_L11;

  // read the output current as an L11
  temp_L11 = smbus->readWord(address, READ_ITEMP);

  // convert L11 value to floating point value
  return math_.lin11_to_float(temp_L11);      //! 2) Convert from Lin11
}

//! Print all status bytes and words
//! @return void
void print_all_status()
{
  uint8_t b;
  uint16_t w;
  uint8_t page;

  for (page = 0; page < 2; page++)
  {
    Serial.print(F("PAGE "));
    Serial.println(page, DEC);
    smbus->writeByte(LTC3880_I2C_ADDRESS, PAGE, page);
    w = smbus->readWord(LTC3880_I2C_ADDRESS, STATUS_WORD);
    Serial.print(F("LTC3880 STATUS WORD 0x"));
    Serial.println(w, HEX);
    b = smbus->readWord(LTC3880_I2C_ADDRESS, STATUS_MFR_SPECIFIC);
    Serial.print(F("LTC3880 MFR STATUS BYTE 0x"));
    Serial.println(b, HEX);
  }

  for (page = 0; page < 4; page++)
  {
    Serial.print(F("PAGE "));
    Serial.println(page, DEC);
    smbus->writeByte(LTC2974_I2C_ADDRESS, PAGE, page);
    w = smbus->readWord(LTC2974_I2C_ADDRESS, STATUS_WORD);
    Serial.print(F("LTC2974 STATUS WORD 0x"));
    Serial.println(w, HEX);
    b = smbus->readWord(LTC2974_I2C_ADDRESS, STATUS_MFR_SPECIFIC);
    Serial.print(F("LTC2974 MFR STATUS BYTE 0x"));
    Serial.println(b, HEX);
  }

  for (page = 0; page < 8; page++)
  {
    Serial.print(F("PAGE "));
    Serial.println(page, DEC);
    smbus->writeByte(LTC2977_8_I2C_ADDRESS, PAGE, page);
    w = smbus->readWord(LTC2977_8_I2C_ADDRESS, STATUS_WORD);
    Serial.print(F("LTC2977 STATUS WORD 0x"));
    Serial.println(w, HEX);
    b = smbus->readWord(LTC2977_8_I2C_ADDRESS, STATUS_MFR_SPECIFIC);
    Serial.print(F("LTC2977MFR STATUS BYTE 0x"));
    Serial.println(b, HEX);
  }
}

void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC

  // There is not enough memory in an Uno, so make noise if necessary.
  byte sig0;
  byte sig2;
  byte sig4;
  sig0 = boot_signature_byte_get (0);
  sig2 = boot_signature_byte_get (2);
  sig4 = boot_signature_byte_get (4);
  if (sig0 != 0x1E || sig2 != 0x98 | sig4 != 0x01)
  {
    Serial.println("Sketch only runs on Mega 2560");
    return;
  }

  print_title();
  print_prompt();
}

void loop()
{
  uint8_t user_command;
  uint8_t res;
  uint8_t model[7];
  uint8_t revision[10];
  uint8_t *addresses = NULL;

  if (Serial.available())                          //! Checks for user input
  {
    user_command = read_int();                     //! Reads the user command
    if (user_command != 'm')
      Serial.println(user_command);

    switch (user_command)                          //! Prints the appropriate submenu
    {
      case 1:
        Serial.println(F("Check LTC3880 NVM"));
        check_health(LTC3880_I2C_ADDRESS);
        Serial.println(F("Check LTC2974 NVM"));
        check_health(LTC2974_I2C_ADDRESS);
        Serial.println(F("Check LTC2977/8 NVM"));
        check_health(LTC2977_8_I2C_ADDRESS);
        break;
      case 2:
        print_all_status();
        break;
      case 3:
        Serial.println(F("Store All Fault Logs"));
        store_fault_logs();
        wait_for_ready(LTC3880_I2C_ADDRESS);
        wait_for_ready(LTC2974_I2C_ADDRESS);
        wait_for_ready(LTC2977_8_I2C_ADDRESS);
        break;
      case 4:
        Serial.print(F("LTC3880 Counter "));
        print_counter(LTC3880_I2C_ADDRESS);
        Serial.print(F("LTC2974 Counter "));
        print_counter(LTC2974_I2C_ADDRESS);
        Serial.print(F("LTC2977/8 Counter "));
        print_counter(LTC2977_8_I2C_ADDRESS);
        break;
      case 5:
        erase(LTC3880_I2C_ADDRESS, &ltc3880data);
        Serial.println(F("Erased LTC3880 Data"));
        erase(LTC2974_I2C_ADDRESS, &ltc2974data);
        Serial.println(F("Erased LTC2974 Data"));
        erase(LTC2977_8_I2C_ADDRESS, &ltc2977data);
        Serial.println(F("Erased LTC2977 Data"));
        wait_for_ready(LTC3880_I2C_ADDRESS);
        wait_for_ready(LTC2974_I2C_ADDRESS);
        wait_for_ready(LTC2977_8_I2C_ADDRESS);
        break;
      case 6: // (technique does not work with LTC2978/A)
        read_bytes(LTC3880_I2C_ADDRESS, &ltc3880data, &ltc3880dataCount);
        Serial.println(F("Read LTC3880 Data"));
        read_bytes(LTC2974_I2C_ADDRESS, &ltc2974data, &ltc2974dataCount);
        Serial.println(F("Read LTC2974 Data"));
        if (is_ltc2978(LTC2977_8_I2C_ADDRESS))
          Serial.println("LTC2978/A data not read (LTC2978/A does not support direct read of NVM)");
        else
        {
          read_bytes(LTC2977_8_I2C_ADDRESS, &ltc2977data, &ltc2977dataCount);
          Serial.println(F("Read LTC2977 Data"));
        }
        break;
      case 7: // (technique does not work with LTC2978/A)
        write_bytes(LTC3880_I2C_ADDRESS, &ltc3880data, &ltc3880dataCount);
        Serial.println(F("LTC3880 Data written to NVM"));
        write_bytes(LTC2974_I2C_ADDRESS, &ltc2974data, &ltc2974dataCount);
        Serial.println(F("LTC2974 Data written to NVM"));
        if (is_ltc2978(LTC2977_8_I2C_ADDRESS))
          Serial.println("LTC2978/A data not written (LTC2978/A does not support direct read of NVM)");
        else
        {
          write_bytes(LTC2977_8_I2C_ADDRESS, &ltc2977data, &ltc2977dataCount);
          Serial.println(F("LTC2977 Data written to NVM"));
        }
        // Uncomment to free memory
        //free(ltc3880data);
        //ltc3880data = NULL;
        //free(ltc2974data);
        //ltc2974data = NULL;
        //free(ltc2977data);
        //ltc2977data = NULL;
        break;
      case 8: // (technique works on all PSM devices)
        // This clears the status register so that tools observing on the
        // bus can observe restore behavior. It is not neccessary
        // for correct operation.
        smbus->writeByte(LTC3880_I2C_ADDRESS, MFR_EEPROM_STATUS, 0x00);

        // Example of something like a real application without retry,
        // but no policy, such as protection of over programming or
        // scheduling randomness.

        // Make sure the world is sane. This does not allow
        // storing with NVM/EEPROM CRC errors, but you can
        // change the code to do that.
        Serial.println(F("Check EEPROM"));
        // This check requires no faults.
        if (check_health(LTC3880_I2C_ADDRESS) && check_health(LTC2974_I2C_ADDRESS) && check_health(LTC2977_8_I2C_ADDRESS))
        {
          // Store fault logs so the devices cannot be busy with a fault log.
          store_fault_logs();
          wait_for_ready(LTC3880_I2C_ADDRESS);
          wait_for_ready(LTC2974_I2C_ADDRESS);
          wait_for_ready(LTC2977_8_I2C_ADDRESS);

          // Bump the counters.
          increment_counter(LTC3880_I2C_ADDRESS);
          increment_counter(LTC2974_I2C_ADDRESS);
          increment_counter(LTC2977_8_I2C_ADDRESS);
          Serial.println(F("  Storing with STORE_USER_ALL...\n"));
          // Store all data in parallel.
          smbus->sendByte(0x5B, STORE_USER_ALL);
          wait_for_ready(LTC3880_I2C_ADDRESS);
          wait_for_nvm_done(LTC3880_I2C_ADDRESS);
          wait_for_ready(LTC2974_I2C_ADDRESS);
          wait_for_ready(LTC2977_8_I2C_ADDRESS);

          // Clear any faults so that health check wont fail from some spurious fault.
          clear_faults_global();
          wait_for_ready(LTC3880_I2C_ADDRESS);
          wait_for_ready(LTC2974_I2C_ADDRESS);
          wait_for_ready(LTC2977_8_I2C_ADDRESS);

          // All is ok, so clear the fault logs so they can be used if enabled.
          clear_fault_logs();
          wait_for_ready(LTC3880_I2C_ADDRESS);
          wait_for_ready(LTC2974_I2C_ADDRESS);
          wait_for_ready(LTC2977_8_I2C_ADDRESS);

          // Do not restore or reset here or there will be
          // a power cycle and rails with turn off and on.
          Serial.println(F("Check NVM"));
          if (!check_health(LTC3880_I2C_ADDRESS))
          {
            Serial.println(F("LTC3880 failed health check"));
            // Implement code to try again if CRC fails, etc.
          }
          if (!check_health(LTC2974_I2C_ADDRESS))
          {
            Serial.println(F("LTC2974 failed health check"));
            // Implement code to try again if CRC fails, etc.
          }
          if (!check_health(LTC2977_8_I2C_ADDRESS))
          {
            Serial.println(F("LTC2977/8 failed health check"));
            // Implement code to try again if CRC fails, etc.
          }

        }
        Serial.println(F("NVM (EEPROM) is Updated"));
        break;
      case 9:
        Serial.println(F("Clear All Fault Logs"));
        clear_fault_logs();
        wait_for_ready(LTC3880_I2C_ADDRESS);
        wait_for_ready(LTC2974_I2C_ADDRESS);
        wait_for_ready(LTC2977_8_I2C_ADDRESS);
        break;
      case 10:
        clear_faults_global();
        break;
      case 11:
        addresses = smbus->probe(0);
        while (*addresses != 0)
        {
          Serial.print(F("ADDR 0x"));
          Serial.println(*addresses++, HEX);
        }
        break;
      case 12:
        // Group protocol is a way to send multiple devices in one command as an atomic transaction.
        // It begins with a start, follows with address/command transactions, and ends in a stop.
        // At the stop, all devices respond to their command at the same time. It is not legal
        // or defined how things behave if it sends more than one command to a device withing one
        // group protocol transaction..

        // The reason for using this is that the managers do not have a reset command like the controllers,
        // so the code next to mix restore commands to managers with reset commands to controllers.

        // Tell the PMBus object to start saving commands, but don't do anything on PMBus.
        smbus->beginStoring();
        // Add commands to the list
        smbus->sendByte(LTC3880_I2C_ADDRESS, MFR_RESET);
        smbus->sendByte(LTC2974_I2C_ADDRESS, RESTORE_USER_ALL);
        smbus->sendByte(LTC2977_8_I2C_ADDRESS, RESTORE_USER_ALL);
        // Tell the PMBus object to send all the comands atomically as a Group Command Protocol transaction.
        // At the stop the controller will reset, and the managers will do a restore. This effectivly restarts
        // all devices at the same time so that proper sequencing takes place.
        smbus->execute();

        // Wait for each device to finish.
        wait_for_ready(LTC3880_I2C_ADDRESS);
        wait_for_ready(LTC2974_I2C_ADDRESS);
        wait_for_ready(LTC2977_8_I2C_ADDRESS);

        break;
      default:
        Serial.println(F("Incorrect Option"));
        break;
    }
    print_prompt();
  }
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n********************************************************************\n"));
  Serial.print(F("* DC1962C Store/Restore User All                                   *\n"));
  Serial.print(F("*                                                                  *\n"));
  Serial.print(F("* This program demonstrates how to store and restore EEPROM.       *\n"));
  Serial.print(F("*                                                                  *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.   *\n"));
  Serial.print(F("*                                                                  *\n"));
  Serial.print(F("********************************************************************\n"));
}

//! Prints main menu.
void print_prompt()
{
  Serial.print(F("\n  1 -Check NVM (EEPROM) is Ok to Program\n"));
  Serial.print(F("  2 -Read All Status\n"));
  Serial.print(F("  3 -Force Fault Logs (EEPROM)\n"));
  Serial.print(F("  4 -Print Counter (RAM)\n"));
  Serial.print(F("  5 -Erase NVM (EEPROM)\n"));
  Serial.print(F("  6 -Bulk Read NVM (EEPROM)\n"));
  Serial.print(F("  7 -Bulk Write NVM (EEPROM)\n"));
  Serial.print(F("  8 -Store (STORE_USER_ALL)\n"));
  Serial.print(F("  9 -Clear/Erase Fault Logs (EEPROM)\n"));
  Serial.print(F(" 10 -Clear Faults (RAM)\n"));
  Serial.print(F(" 11 -Bus Probe\n"));
  Serial.print(F(" 12 -Reset (Will Power Cycle)\n"));
  Serial.print(F("\nEnter a command:"));
}
