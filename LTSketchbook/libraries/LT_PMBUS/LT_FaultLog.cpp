/*!

LTC PMBus Support: API for a shared LTC Fault Log


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
//! @defgroup LT_FaultLog LT_FaultLog: PLTC PSM Fault Log support
//! @}

/*! @file
    @ingroup LT_FaultLog
    Library Header File for LT_FaultLog
*/

#include <Arduino.h>
#include "LT_FaultLog.h"

LT_FaultLog::LT_FaultLog(LT_PMBus *pmbus)
{
  pmbus_ = pmbus;
}

/*
 * Read the status byte
 *
 * address: PMBUS address
 */
uint8_t
LT_FaultLog::readMfrStatusByte(uint8_t address)
{
  uint8_t status_byte;

  status_byte = pmbus_->smbus()->readByte(address, STATUS_MFR_SPECIFIC);
  return status_byte;
}

/*
 * Read the mfr status byte
 *
 * address: PMBUS address
 */
uint8_t
LT_FaultLog::readMfrFaultLogStatusByte(uint8_t address)
{
  uint8_t status_byte;

  status_byte = pmbus_->smbus()->readByte(address, MFR_FAULT_LOG_STATUS);

  return status_byte;
}

/*
 * Check if there is a fault log
 *
 * address: PMBUS address
 */
bool
LT_FaultLog::hasFaultLog(uint8_t address)
{
  uint8_t   status;
  PsmDeviceType t = pmbus_->deviceType(address);

  if (t == LTC3880 || t == LTC3886 || t == LTC3887 || t == LTM4675 || t == LTM4676|| t == LTM4676A || t == LTM4677)
  {
    status = readMfrStatusByte(address);
    return (status & LTC3880_SMFR_FAULT_LOG) > 0;
  }
  else if (t == LTC3882 || t == LTC3882_1)
  {
    status = readMfrStatusByte(address);
    return (status & LTC3882_SMFR_FAULT_LOG) > 0;
  }
  else if (t == LTC3883)
  {
    status = readMfrStatusByte(address);
    return (status & LTC3883_SMFR_FAULT_LOG) > 0;
  }
  else if (t == LTC2974 || t == LTC2975)
  {
    status = readMfrFaultLogStatusByte(address);
    return (status & LTC2974_SFL_EEPROM) > 0;
  }
  else if (t == LTC2977 || t == LTC2978 || t == LTC2980 || t == LTM2987)
  {
    status = readMfrFaultLogStatusByte(address);
    return (status & LTC2978_SFL_EEPROM) > 0;
  }
  else
    return false;
}

/*
 * Enable fault log
 *
 * address: PMBUS address
 */
void
LT_FaultLog::enableFaultLog(uint8_t address)
{
  uint8_t   config8;
  uint16_t  config16;
  PsmDeviceType t = pmbus_->deviceType(address);

  if (
    (t == LTC3880)
    || (t == LTC3882)
    || (t == LTC3882_1)
    || (t == LTC3883)
    || (t == LTC3886)
    || (t == LTM4675)
    || (t == LTM4676)
    || (t == LTM4676A)
    || (t == LTM4677)
    || (t == LTC2978)
  )
  {
    config8 = pmbus_->smbus()->readByte(address, MFR_CONFIG_ALL);
    pmbus_->smbus()->writeByte(address, MFR_CONFIG_ALL, config8 | CFGALL_EFL);
  }
  else if (
    (t == LTC2974)
    || (t == LTC2975)
    || (t == LTC2977)
    || (t == LTC2980)
    || (t == LTM2987)
  )
  {
    config16 = pmbus_->smbus()->readWord(address, MFR_CONFIG_ALL);
    pmbus_->smbus()->writeWord(address, MFR_CONFIG_ALL, config16 | LTC2974_CFGALL_EFL);
  }
}

/*
 * Disable fault log
 *
 * address: PMBUS address
 */
void
LT_FaultLog::disableFaultLog(uint8_t address)
{
  uint8_t   config8;
  uint16_t  config16;
  PsmDeviceType t = pmbus_->deviceType(address);

  if (
    (t == LTC3880)
    || (t == LTC3882)
    || (t == LTC3882_1)
    || (t == LTC3883)
    || (t == LTC3886)
    || (t == LTM4675)
    || (t == LTM4676)
    || (t == LTM4676A)
    || (t == LTM4677)
    || (t == LTC2978)
  )
  {
    config8 = pmbus_->smbus()->readByte(address, MFR_CONFIG_ALL);
    pmbus_->smbus()->writeByte(address, MFR_CONFIG_ALL, config8 & ~CFGALL_EFL);
  }
  else if (
    (t == LTC2974)
    || (t == LTC2975)
    || (t == LTC2977)
    || (t == LTC2980)
    || (t == LTM2987)
  )
  {
    config16 = pmbus_->smbus()->readWord(address, MFR_CONFIG_ALL);
    pmbus_->smbus()->writeWord(address, MFR_CONFIG_ALL, config16 & ~CFGALL_EFL);
  }
}

void LT_FaultLog::dumpBin(Print *printer, uint8_t *log, uint8_t size)
{
  if (printer == 0)
    printer = &Serial;
  uint8_t *temp = log;
  for (uint8_t i = 0; i < size; i++)
  {
    if (!(i % 16))
      printer->println();
    if (temp[i] < 0x10)
      printer->write('0');
    printer->print(temp[i], HEX);
  }
  printer->println();
}


/*
 * Clear fault log
 *
 * address: PMBUS address
 */
void
LT_FaultLog::clearFaultLog(uint8_t address)
{
  pmbus_->smbus()->sendByte(address, MFR_FAULT_LOG_CLEAR);
}


uint64_t
LT_FaultLog::getSharedTime200us(FaultLogTimeStamp time_stamp)
{
  uint64_t num200us = ((uint64_t) time_stamp.shared_time_byte5 << 40);
  num200us = num200us | ((uint64_t) time_stamp.shared_time_byte4 << 32);
  num200us = num200us | ((uint32_t) time_stamp.shared_time_byte3 << 24);
  num200us = num200us | ((uint32_t) time_stamp.shared_time_byte2 << 16);
  num200us = num200us | ((uint32_t) time_stamp.shared_time_byte1 << 8);
  num200us = num200us | (time_stamp.shared_time_byte0);
  return num200us;

}

float
LT_FaultLog::getTimeInMs(FaultLogTimeStamp time_stamp)
{
  double ms = getSharedTime200us(time_stamp)/5.0;
  return ms;
}

uint8_t
LT_FaultLog::getRawByteVal(RawByte value)
{
  return (uint8_t) value.uint8_tValue;
}

uint16_t
LT_FaultLog::getRawWordVal(RawWord value)
{
  return (uint16_t) (value.lo_byte | (value.hi_byte << 8));
}

uint16_t
LT_FaultLog::getRawWordReverseVal(RawWordReverse value)
{
  return (uint16_t) (value.lo_byte | (value.hi_byte << 8));
}

uint16_t
LT_FaultLog::getLin5_11WordVal(Lin5_11Word value)
{
  return (uint16_t) (value.lo_byte | (value.hi_byte << 8));
}

uint16_t
LT_FaultLog::getLin5_11WordReverseVal(Lin5_11WordReverse value)
{
  return (uint16_t) (value.lo_byte | (value.hi_byte << 8));
}

uint16_t
LT_FaultLog::getLin16WordVal(Lin16Word value)
{
  return (uint16_t) (value.lo_byte | (value.hi_byte << 8));
}

uint16_t
LT_FaultLog::getLin16WordReverseVal(Lin16WordReverse value)
{
  return (uint16_t) (value.lo_byte | (value.hi_byte << 8));
}