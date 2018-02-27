/*!
LTC PMBus Support

@verbatim

This PMBus code does not cover the entire PMBus command set. The intention
is to cover the common commands. It was written for LTC PMBus devices,
and may not perform properly with other PMBus devices.

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
//! @defgroup LT_PMBus LT_PMBus: PMBus commands
//! @}

/*! @file
    @ingroup LT_PMBus
    Library File for LT_PMBus
*/
#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_PMBus.h"

/*
 * Be aware that many of these calls require setting the page before calling.
 *
 * malloc() is not checked for errors.
 * SMBus errors are printed and swallowed.
 * The assumption is this code will be in an embedded environment and tested,
 * without a need to respond to errors. If that assumption does not hold,
 * you must implement error handling.
 */

// Fast math is the in the LT_PMBusMath.cpp file and is optimized for performance.
// Setting USE_FAST_MATH to 0 uses simpler routines that are conceptually better for learning.
#define USE_FAST_MATH   1

// Set to 0 for LTC3883 which does not support PEND
#define USE_PEND        1

#define NOT_BUSY        1 << 6
#define NOT_TRANS       1 << 4
#if (USE_PEND)
#define NOT_PENDING     1 << 5
#else
#define NOT_PENDING     0
#endif

LT_PMBus::LT_PMBus (LT_SMBus *smbus)
{
  smbus_ = new LT_SMBusGroup(smbus, smbus->i2cbus()->getSpeed());
}

LT_PMBus::~LT_PMBus ()
{
  delete smbus_;
}

PsmDeviceType LT_PMBus::deviceType(uint8_t address)
{
  uint16_t id;

  id = 0xFFF0 & smbus_->readWord(address, MFR_SPECIAL_ID);
  switch (id)
  {
    case 0x4020:
      return LTC3880;
    case 0x4200:
      return LTC3882;
    case 0x4240:
      return LTC3882_1;
    case 0x4300:
      return LTC3883;
    case 0x4600:
      return LTC3886;
    case 0x47A0:
      return LTM4675;
    case 0x4400:
      return LTM4676;
    case 0x4480:
      return LTM4676;
    case 0x47E0:
      return LTM4676A;
    case 0x47B0:
      return LTM4677;
    case 0x0210:
      return LTC2974;
    case 0x0220:
      return LTC2975;
    case 0x0130:
      return LTC2977;
    case 0x8030:
      return LTC2980;
    case 0x8040:
      return LTC2980;
    case 0x8010:
      return LTM2987;
    case 0x8020:
      return LTM2987;
    case 0x0110:
      return LTC2978;
    case 0x0120:
      return LTC2978;
    default:
      return LTCUnknown;
  }
}

bool LT_PMBus::isLtc297x(uint8_t address)
{
  PsmDeviceType t = deviceType(address);
  return (t == LTC2974 || t == LTC2975 || t == LTC2977 || t == LTC2978 || t == LTC2980 || t == LTM2987);
}

bool LT_PMBus::isLtc2977(uint8_t address)
{
  PsmDeviceType t = deviceType(address);
  return (t == LTC2977 || t == LTC2980 || t == LTM2987);
}

bool LT_PMBus::isLtc2974(uint8_t address)
{
  PsmDeviceType t = deviceType(address);
  return (t == LTC2974 || t == LTC2975);
}

bool LT_PMBus::isLtc2978(uint8_t address)
{
  bool is_ltc2978;
  uint8_t mfr_common;
  mfr_common = smbus_->readByte(address, MFR_COMMON);
  is_ltc2978 = ((mfr_common & (1 << 2)) == 0);
  return is_ltc2978;
}

/*
 * Write a byte with polling
 *
 * address: SMBUS address
 * command: SMBUS command
 * data: data to send
 */
void LT_PMBus::pmbusWriteByteWithPolling(uint8_t address, uint8_t command, uint8_t data)
{
  smbus_->waitForAck(address, 0x00);
  if (waitForNotBusy(address) == 1)   //! 1) Wait for not busy
  {
    // Device cannot be made not busy...
  }

  smbus_->writeByte(address, command, data);   //! 2) Write Byte
}

/*
 * Read one byte with polling
 *
 * address: SMBUS address
 * command: SMB command
 */
uint8_t LT_PMBus::pmbusReadByteWithPolling(uint8_t address, uint8_t command)
{
  uint8_t return_value;

  smbus_->waitForAck(address, 0x00);
  if (waitForNotBusy(address) == 1)   //! 1) Wait for not busy
  {
    // Device cannot be made not busy...
  }

  return_value = smbus_->readByte(address, command);   //! 2) Read Byte

  return return_value;
}

/*
 * Read word with polling
 *
 * address: SMBUS address
 * command: SMBUS command
 */
uint16_t LT_PMBus::pmbusReadWordWithPolling(uint8_t address, uint8_t command)
{
  uint16_t return_value;

  smbus_->waitForAck(address, 0x00);
  if (waitForNotBusy(address) == 1)   //! 1) Wait for not busy
  {
    // Device cannot be made not busy...
  }

  return_value = smbus_->readWord(address, command);//! 2) Read Word

  return return_value;
}

/*
 * Set the output voltage
 *
 * address: PMBUS address
 * voltage: the target voltage
 */
void LT_PMBus::setVout(uint8_t address, float voltage)
{
  uint16_t vout;

#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);    //! 1) Read VOUT_MODE & 0x1F
  vout = math_.float_to_lin16(voltage, vout_mode);        //! 2) Convert voltage to Lin16
#else
  vout = Float_to_L16(address, voltage);
#endif
  smbus_->writeWord(address, VOUT_COMMAND, vout);  //! 3) Write VOUT_COMMAND
}

/*
 * Set the output voltage
 *
 * address: PMBUS address
 * voltage: the target voltage
 * page: page
 */
void LT_PMBus::setVoutWithPage(uint8_t address, float voltage, uint8_t page)
{
  setPage(address, page);
  setVout(address, voltage);
}

/*
 * Set the output voltage
 *
 * address: PMBUS address
 * voltage: the target voltage
 * page: page
 */
void LT_PMBus::setVoutWithPagePlus(uint8_t address, float voltage, uint8_t page)
{
  uint16_t vout;
  uint8_t data[4];

#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);    //! 1) Read VOUT_MODE & 0x1F
  vout = math_.float_to_lin16(voltage, vout_mode);    //! 2) Convert voltage to Lin16
#else
  vout = Float_to_L16(address, voltage);
#endif

  data[0] = page;
  data[1] = VOUT_COMMAND;
  data[2] = 0xFF & vout;
  data[3] = vout >> 8;
  smbus_->writeBlock(address, PAGE_PLUS_WRITE, data, 4);   //! 3) Write VOUT_COMMAND
}

/*
 * Set the output voltage, margin voltage, and warning voltage
 *
 * address: PMBUS address
 * voltage: output voltage
 * margin_percent: percent to margin high/low
 * warn_percent: percent to set warning voltage high/low
 * fault_percent: percent to set fault voltage high/low
 *
 * This will move values in order that prevents fault. However, there are no delays between changes.
 */
void LT_PMBus::setVoutWithSupervision(uint8_t address, float voltage, float margin_percent, float warn_percent, float fault_percent)
{
  float old_vout;

  old_vout = readVout(address, false);        //! 1) Read VOUT

  if (voltage >= old_vout)
  {
    setVoutOvFaultLimit(address, voltage * (1.0 + fault_percent));  //! 2a) Set VOUT_OV_FAULT
    setVoutOvWarnLimit(address, voltage * (1.0 + warn_percent));    //! 3a) Set VOUT_OV_WARN
    setVoutMarginHigh(address, voltage * (1.0 + margin_percent));   //! 4a) Set VOUT_MARGIN_HIGH
    setVout(address, voltage);      //! 5a) Set VOUT
    setVoutMarginLow(address, voltage * (1.0 - margin_percent));    //! 6a) Set VOUT_MARGIN_LOW
    setVoutUvWarnLimit(address, voltage * (1.0 - warn_percent));    //! 7a) Set VOUT_UV_WARN_LIMIT
    setVoutUvFaultLimit(address, voltage * (1.0 - fault_percent));  //! 8a) Set VOUT_UV_FAULT_LIMIT
  }
  else
  {
    setVoutUvFaultLimit(address, voltage * (1.0 - fault_percent));  //! 2b) Set VOUT_UV_FAULT_LIMIT
    setVoutUvWarnLimit(address, voltage * (1.0 - warn_percent));//! 3b) Set VOUT_UV_WARN_LIMIT
    setVoutMarginLow(address, voltage * (1.0 - margin_percent));//! 4b) Set VOUT_MARGIN_LOW
    setVout(address, voltage);  //! 5b) Set VOUT
    setVoutMarginHigh(address, voltage * (1.0 + margin_percent));   //! 6b) Set VOUT_MARGIN_HIGH
    setVoutOvWarnLimit(address, voltage * (1.0 + warn_percent));    //! 7b) Set VOUT_OV_WARN
    setVoutOvFaultLimit(address, voltage * (1.0 + fault_percent));  //! 8b) Set VOUT_OV_FAULT
  }
}

/*
 * Set the output voltage, margin voltage, and warning voltage
 *
 * address: PMBUS address
 * voltage: output voltage
 * margin_percent: percent to margin high/low
 * warn_percent: percent to set warning voltage high/low
 * fault_percent: percent to set fault voltage high/low
 * page: page
 *
 * This will move values in order that prevents fault. However, there are no delays between changes.
 */
void LT_PMBus::setVoutWithSupervisionWithPage(uint8_t address, float voltage,
    float margin_percent, float warn_percent, float fault_percent, uint8_t page)
{
  setPage(address, page);
  setVoutWithSupervision(address, voltage, margin_percent, warn_percent, fault_percent);
}

/*
 * Set the output voltage, margin voltage, and warning voltage
 *
 * address: PMBUS address
 * voltage: output voltage
 * margin_percent: percent to margin high/low
 * warn_percent: percent to set warning voltage high/low
 * fault_percent: percent to set fault voltage high/low
 * page: page
 *
 * This will move values in order that prevents fault. However, there are no delays between changes.
 */
void LT_PMBus::setVoutWithSupervisionWithPagePlus(uint8_t address, float voltage,
    float margin_percent, float warn_percent, float fault_percent, uint8_t page)
{
  float old_vout;

  old_vout = readVoutWithPage(address, page);     //! 1) Read VOUT

  if (voltage >= old_vout)
  {
    setVoutOvFaultLimitWithPagePlus(address, voltage * (1.0 + fault_percent), page);    //! 2a) Set VOUT_OV_FAULT
    setVoutOvWarnLimitWithPagePlus(address, voltage * (1.0 + warn_percent), page);  //! 3a) Set VOUT_OV_WARN
    setVoutMarginHighWithPagePlus(address, voltage * (1.0 + margin_percent), page); //! 4a) Set VOUT_MARGIN_HIGH
    setVoutWithPagePlus(address, voltage, page);        //! 5a) Set VOUT
    setVoutMarginLowWithPagePlus(address, voltage * (1.0 - margin_percent), page);  //! 6a) Set VOUT_MARGIN_LOW
    setVoutUvWarnLimitWithPagePlus(address, voltage * (1.0 - warn_percent), page);  //! 7a) Set VOUT_UV_WARN_LIMIT
    setVoutUvFaultLimitWithPagePlus(address, voltage * (1.0 - fault_percent), page);    //! 8a) Set VOUT_UV_FAULT_LIMIT
  }
  else
  {
    setVoutUvFaultLimitWithPagePlus(address, voltage * (1.0 - fault_percent), page);    //! 2b) Set VOUT_UV_FAULT_LIMIT
    setVoutUvWarnLimitWithPagePlus(address, voltage * (1.0 - warn_percent), page);//! 3b) Set VOUT_UV_WARN_LIMIT
    setVoutMarginLowWithPagePlus(address, voltage * (1.0 - margin_percent), page);//! 4b) Set VOUT_MARGIN_LOW
    setVoutWithPagePlus(address, voltage, page);    //! 5b) Set VOUT
    setVoutMarginHighWithPagePlus(address, voltage * (1.0 + margin_percent), page); //! 6b) Set VOUT_MARGIN_HIGH
    setVoutOvWarnLimitWithPagePlus(address, voltage * (1.0 + warn_percent), page);  //! 7b) Set VOUT_OV_WARN
    setVoutOvFaultLimitWithPagePlus(address, voltage * (1.0 + fault_percent), page);    //! 8b) Set VOUT_OV_FAULT
  }
}

/*
 * Set the maximum voltage
 *
 * address: PMBUS address
 * voltage: maximum voltage
 */
void LT_PMBus::setVoutMax(uint8_t address, float voltage)
{
  uint16_t vout;

#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! 1) Read VOUT_MODE & 0x1F
  vout = math_.float_to_lin16(voltage, vout_mode);        //! 2) Convert to Lin16
#else

  vout = Float_to_L16(address, voltage);
#endif
  smbus_->writeWord(address, VOUT_MAX, vout);      //! 3) Write VOUT_MAX
}

/*
 * Set the maximum voltage
 *
 * address: PMBUS address
 * voltage: maximum voltage
 * page: page
 */
void LT_PMBus::setVoutMaxWithPage(uint8_t address, float voltage, uint8_t page)
{
  setPage(address, page);
  setVoutMax(address, voltage);
}

/*
 * Set the maximum voltage
 *
 * address: PMBUS address
 * voltage: maximum voltage
 * page: page
 */
void LT_PMBus::setVoutMaxWithPagePlus(uint8_t address, float voltage, uint8_t page)
{
  uint16_t vout;
  uint8_t data[4];

#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! 1) Read VOUT_MODE & 0x1F
  vout = math_.float_to_lin16(voltage, vout_mode);        //! 2) Convert to Lin16
#else
  vout = Float_to_L16(address, voltage);
#endif

  data[0] = page;
  data[1] = VOUT_MAX;
  data[2] = 0xFF & vout;
  data[3] = vout >> 8;
  smbus_->writeBlock(address, PAGE_PLUS_WRITE, data, 4);       //! 3) Write VOUT_MAX
}


/*
 * Set the TON_DELAY
 *
 * address: PMBUS address
 * delay: delay
 */
void LT_PMBus::setTonDelay(uint8_t address, float delay)
{
  uint16_t time;

#if USE_FAST_MATH
  time = math_.float_to_lin11(delay);     //! 1) Convert to Lin11
#else
  time = Float_to_L11(delay);
#endif

  smbus_->writeWord(address, TON_DELAY, time);     //! 2) Write TON_DELAY
}

/*
 * Set the TON_RISE
 *
 * address: PMBUS address
 * rise: rise time
 */
void LT_PMBus::setTonRise(uint8_t address, float rise)
{
  uint16_t time;

#if USE_FAST_MATH
  time = math_.float_to_lin11(rise);      //! 1) Convert to Lin11
#else
  time = Float_to_L11(rise);
#endif

  smbus_->writeWord(address, TON_RISE, time);      //! 2) Write TON_RISE
}

/*
 * Set the TON_MAX
 *
 * address: PMBUS address
 * max: max time
 */
void LT_PMBus::setTonMaxFaultLimit(uint8_t address, float max)
{
  uint16_t time;

#if USE_FAST_MATH
  time = math_.float_to_lin11(max);       //! 1) Convert to Lin11
#else
  time = Float_to_L11(max);
#endif

  smbus_->writeWord(address, TON_MAX_FAULT_LIMIT, time);       //! 2) Write TON_MAX_FAULT_LIMIT
}

/*
 * Set the TOFF_DELAY
 *
 * address: PMBUS address
 * delay: delay
 */
void LT_PMBus::setToffDelay(uint8_t address, float delay)
{
  uint16_t time;

#if USE_FAST_MATH
  time = math_.float_to_lin11(delay);     //! 1) Convert to Lin11
#else
  time = Float_to_L11(delay);
#endif

  smbus_->writeWord(address, TOFF_DELAY, time);        //! 2) Write TOFF_DELAY
}

/*
 * Set the TOFF_FALL
 *
 * address: PMBUS address
 * fall: fall time
 */
void LT_PMBus::setToffFall(uint8_t address, float fall)
{
  uint16_t time;

#if USE_FAST_MATH
  time = math_.float_to_lin11(fall);      //! 1) Convert to L11
#else
  time = Float_to_L11(fall);
#endif

  smbus_->writeWord(address, TOFF_FALL, time);     //! 2) Write TOFF_FALL
}

/*
 * Set the TOFF_MAX_WARN
 *
 * address: PMBUS address
 * max: max time
 */
void LT_PMBus::setToffMaxWarnLimit(uint8_t address, float max)
{
  uint16_t time;

#if USE_FAST_MATH
  time = math_.float_to_lin11(max);       //! 1) Convert to Lin11
#else
  time = Float_to_L11(max);
#endif

  smbus_->writeWord(address, TOFF_MAX_WARN_LIMIT, time);       //! 2) Write TOFF_MAX_WARN_LIMIT
}

/*
 * Set the over voltage fault
 *
 * address: PMBUS address
 * voltage: fault voltage
 */
void LT_PMBus::setVoutOvFaultLimit(uint8_t address, float voltage)
{
  uint16_t vout;

#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! 1) Read VOUT_MODE & 0x1F
  vout = math_.float_to_lin16(voltage, vout_mode);        //! 2) Convert to Lin16
#else
  vout = Float_to_L16(address, voltage);
#endif
  smbus_->writeWord(address, VOUT_OV_FAULT_LIMIT, vout);       //! 3) Write VOUT_OV_FAULT_LIMIT
}

/*
 * Set the over voltage fault
 *
 * address: PMBUS address
 * voltage: fault voltage
 * page: page
 */
void LT_PMBus::setVoutOvFaultLimitWithPage(uint8_t address, float voltage, uint8_t page)
{
  setPage(address, page);
  setVoutOvFaultLimit(address, voltage);
}

/*
 * Set the over voltage fault
 *
 * address: PMBUS address
 * voltage: fault voltage
 * page: page
 */
void LT_PMBus::setVoutOvFaultLimitWithPagePlus(uint8_t address, float voltage, uint8_t page)
{
  uint16_t vout;
  uint8_t data[4];

#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! 1) REad VOUT_MODE & 0x1F
  vout = math_.float_to_lin16(voltage, vout_mode);        //! 2) Convert to Lin16
#else
  vout = Float_to_L16(address, voltage);
#endif

  data[0] = page;
  data[1] = VOUT_OV_FAULT_LIMIT;
  data[2] = 0xFF & vout;
  data[3] = vout >> 8;
  smbus_->writeBlock(address, PAGE_PLUS_WRITE, data, 4);       //! 3) Write VOUT_OV_FAULT_LIMIT
}

/*
 * Set the over voltage warning
 *
 * address: PMBUS address
 * voltage: warning voltage
 */
void LT_PMBus::setVoutOvWarnLimit(uint8_t address, float voltage)
{
  uint16_t vout;

#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! 1) Read VOUT_MODE & 0x1F
  vout = math_.float_to_lin16(voltage, vout_mode);        //! 2) Convert to Lin16
#else
  vout = Float_to_L16(address, voltage);
#endif
  smbus_->writeWord(address, VOUT_OV_WARN_LIMIT, vout);        //! 3) Write VOUT_OV_WARN_LIMIT
}

/*
 * Set the over voltage warning
 *
 * address: PMBUS address
 * voltage: warning voltage
 * page: page
 */
void LT_PMBus::setVoutOvWarnLimitWithPage(uint8_t address, float voltage, uint8_t page)
{
  setPage(address, page);
  setVoutOvWarnLimit(address, voltage);
}

/*
 * Set the over voltage warning
 *
 * address: PMBUS address
 * voltage: warning voltage
 * page: page
 */
void LT_PMBus::setVoutOvWarnLimitWithPagePlus(uint8_t address, float voltage, uint8_t page)
{
  uint16_t vout;
  uint8_t data[4];

#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! 1) Read VOUT_MODE & 0x1F
  vout = math_.float_to_lin16(voltage, vout_mode);        //! 2) Convert to Lin16
#else
  vout = Float_to_L16(address, voltage);
#endif

  data[0] = page;
  data[1] = VOUT_OV_WARN_LIMIT;
  data[2] = 0xFF & vout;
  data[3] = vout >> 8;
  smbus_->writeBlock(address, PAGE_PLUS_WRITE, data, 4);       //! 3) Write VOUT_OV_WARN_LIMIT
}

/*
 * Set the high margin voltage
 *
 * address: PMBUS address
 * voltage: margin voltage
 */
void LT_PMBus::setVoutMarginHigh(uint8_t address, float voltage)
{
  uint16_t vout;

#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! 1) REad VOUT_MODE & 0x1F
  vout = math_.float_to_lin16(voltage, vout_mode);        //! 2) Convert to Lin16
#else
  vout = Float_to_L16(address, voltage);
#endif
  smbus_->writeWord(address, VOUT_MARGIN_HIGH, vout);      //! 3) Write VOUT_MARGIN_HIGH
}

/*
 * Set the high margin voltage
 *
 * address: PMBUS address
 * voltage: margin voltage
 * page: page
 */
void LT_PMBus::setVoutMarginHighWithPage(uint8_t address, float voltage, uint8_t page)
{
  setPage(address, page);
  setVoutMarginHigh(address, voltage);
}

/*
 * Set the high margin voltage
 *
 * address: PMBUS address
 * voltage: margin voltage
 * page: page
 */
void LT_PMBus::setVoutMarginHighWithPagePlus(uint8_t address, float voltage, uint8_t page)
{
  uint16_t vout;
  uint8_t data[4];

#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! 1) Read VOUT_MODE & 0x1F
  vout = math_.float_to_lin16(voltage, vout_mode);        //! 2) Convert to Lin16
#else
  vout = Float_to_L16(address, voltage);
#endif

  data[0] = page;
  data[1] = VOUT_MARGIN_HIGH;
  data[2] = 0xFF & vout;
  data[3] = vout >> 8;
  smbus_->writeBlock(address, PAGE_PLUS_WRITE, data, 4);       //! 3) Write VOUT_MARGIN_HIGH
}

/*
 * Set the low margin voltage
 *
 * address: PMBUS address
 * voltage: margin voltage
 */
void LT_PMBus::setVoutMarginLow(uint8_t address, float voltage)
{
  uint16_t vout;

#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! Read VOUT_MODE & 0x1F
  vout = math_.float_to_lin16(voltage, vout_mode);        //! 2) Convert to Lin16
#else
  vout = Float_to_L16(address, voltage);
#endif
  smbus_->writeWord(address, VOUT_MARGIN_LOW, vout);       //! 3) Write VOUT_MARGIN_HIGH
}

/*
 * Set the low margin voltage
 *
 * address: PMBUS address
 * voltage: margin voltage
 * page: page
 */
void LT_PMBus::setVoutMarginLowWithPage(uint8_t address, float voltage, uint8_t page)
{
  setPage(address, page);
  setVoutMarginLow(address, voltage);
}

/*
 * Set the low margin voltage
 *
 * address: PMBUS address
 * voltage: margin voltage
 * page: page
 */
void LT_PMBus::setVoutMarginLowWithPagePlus(uint8_t address, float voltage, uint8_t page)
{
  uint16_t vout;
  uint8_t data[4];

#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! 1) Read VOUT_MODE & 0x1F
  vout = math_.float_to_lin16(voltage, vout_mode);        //! 2) Convert to L16
#else
  vout = Float_to_L16(address, voltage);
#endif

  data[0] = page;
  data[1] = VOUT_MARGIN_LOW;
  data[2] = 0xFF & vout;
  data[3] = vout >> 8;
  smbus_->writeBlock(address, PAGE_PLUS_WRITE, data, 4);       //! 3) Write VOUT_MARGIN_LOW
}

/*
 * Set the under voltage warning
 *
 * address: PMBUS address
 * voltage: warning voltage
 */
void LT_PMBus::setVoutUvWarnLimit(uint8_t address, float voltage)
{
  uint16_t vout;

#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! 1) Read VOUT_MODE & 0x1F
  vout = math_.float_to_lin16(voltage, vout_mode);        //! 2) Convert to Lin16
#else
  vout = Float_to_L16(address, voltage);
#endif
  smbus_->writeWord(address, VOUT_UV_WARN_LIMIT, vout);        //! 3) Write VOUT_UV_WARN_LIMIT
}

/*
 * Set the under voltage warning
 *
 * address: PMBUS address
 * voltage: warning voltage
 * page: page
 */
void LT_PMBus::setVoutUvWarnLimitWithPage(uint8_t address, float voltage, uint8_t page)
{
  setPage(address, page);
  setVoutUvWarnLimit(address, voltage);
}

/*
 * Set the under voltage warning
 *
 * address: PMBUS address
 * voltage: warning voltage
 * page: page
 */
void LT_PMBus::setVoutUvWarnLimitWithPagePlus(uint8_t address, float voltage, uint8_t page)
{
  uint16_t vout;
  uint8_t data[4];

#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! 1) Rread VOUT_MODE & 0x1F
  vout = math_.float_to_lin16(voltage, vout_mode);        //! 2) Convert to Lin16
#else
  vout = Float_to_L16(address, voltage);
#endif

  data[0] = page;
  data[1] = VOUT_UV_WARN_LIMIT;
  data[2] = 0xFF & vout;
  data[3] = vout >> 8;
  smbus_->writeBlock(address, PAGE_PLUS_WRITE, data, 4);       //! 3) Write VOUT_UV_WARN_LIMIT
}

/*
 * Set the under voltage limit
 *
 * address: PMBUS address
 * voltage: limit voltage
 */
void LT_PMBus::setVoutUvFaultLimit(uint8_t address, float voltage)
{
  uint16_t vout;

#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! 1) Read VOUT_MODE & 0x1F
  vout = math_.float_to_lin16(voltage, vout_mode);        //! 2) Convert to Lin16
#else
  vout = Float_to_L16(address, voltage);
#endif
  smbus_->writeWord(address, VOUT_UV_FAULT_LIMIT, vout);       //! 3) Write VOUT_UV_FAULT_LIMIT
}

/*
 * Set the under voltage limit
 *
 * address: PMBUS address
 * voltage: limit voltage
 * page: page
 */
void LT_PMBus::setVoutUvFaultLimitWithPage(uint8_t address, float voltage, uint8_t page)
{
  setPage(address, page);
  setVoutUvFaultLimit(address, voltage);
}

/*
 * Set the under voltage limit
 *
 * address: PMBUS address
 * voltage: limit voltage
 * page: page
 */
void LT_PMBus::setVoutUvFaultLimitWithPagePlus(uint8_t address, float voltage, uint8_t page)
{
  uint16_t vout;
  uint8_t data[4];

#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! 1) Read VOUT_MODE & 0x1F
  vout = math_.float_to_lin16(voltage, vout_mode);        //! 2) Convert to L16
#else
  vout = Float_to_L16(address, voltage);
#endif

  data[0] = page;
  data[1] = VOUT_UV_FAULT_LIMIT;
  data[2] = 0xFF & vout;
  data[3] = vout >> 8;
  smbus_->writeBlock(address, PAGE_PLUS_WRITE, data, 4);       //! 3) Write VOUT_UV_FAULT_LIMIT
}

/*
 * Set the over current fault
 *
 * address: PMBUS address
 * current: fault current
 */
void LT_PMBus::setIoutOcFaultLimit(uint8_t address, float current)
{
  uint16_t iout;

#if USE_FAST_MATH
  iout = math_.float_to_lin11(current);       //! 1) Convert to Lin11
#else
  iout = Float_to_L11(current);
#endif

  smbus_->writeWord(address, IOUT_OC_FAULT_LIMIT, iout);       //! 2) Write IOUT_OC_FAULT_LIMIT
}

/*
 * Set the over current fault
 *
 * address: PMBUS address
 * current: fault current
 * page: page
 */
void LT_PMBus::setIoutOcFaultLimitWithPage(uint8_t address, float current, uint8_t page)
{
  setPage(address, page);
  setIoutOcFaultLimit(address, current);
}

/*
 * Set the over current fault
 *
 * address: PMBUS address
 * current: fault current
 * page: page
 */
void LT_PMBus::setIoutOcFaultLimitWithPagePlus(uint8_t address, float current, uint8_t page)
{
  uint16_t iout;
  uint8_t data[4];

#if USE_FAST_MATH
  iout = math_.float_to_lin11(current);       //! 1) Convert to Lin11
#else
  iout = Float_to_L11(current);
#endif

  data[0] = page;
  data[1] = IOUT_OC_FAULT_LIMIT;
  data[2] = 0xFF & iout;
  data[3] = iout >> 8;
  smbus_->writeBlock(address, PAGE_PLUS_WRITE, data, 4);       //! 2) Write IOUT_OC_FAULT_LIMIT
}

/*
 * Set the over current warning
 *
 * address: PMBUS address
 * current: warning current
 */
void LT_PMBus::setIoutOcWarnLimit(uint8_t address, float current)
{
  uint16_t iout;

#if USE_FAST_MATH
  iout = math_.float_to_lin11(current);       //! 1) Convert to Lin11
#else
  iout = Float_to_L11(current);
#endif

  smbus_->writeWord(address, IOUT_OC_WARN_LIMIT, iout);        //! 2) Write IOUT_OC_WARN_LIMIT
}

/*
 * Set the over current warning
 *
 * address: PMBUS address
 * current: warning current
 * page: page
 */
void LT_PMBus::setIoutOcWarnLimitWithPage(uint8_t address, float current, uint8_t page)
{
  setPage(address, page);
  setIoutOcWarnLimit(address, current);
}

/*
 * Set the over current warning
 *
 * address: PMBUS address
 * current: warning current
 * page: page
 */
void LT_PMBus::setIoutOcWarnLimitWithPagePlus(uint8_t address, float current, uint8_t page)
{
  uint16_t iout;
  uint8_t data[4];

#if USE_FAST_MATH
  iout = math_.float_to_lin11(current);       //! 1) Convert to Lin11
#else
  iout = Float_to_L11(current);
#endif

  data[0] = page;
  data[1] = IOUT_OC_WARN_LIMIT;
  data[2] = 0xFF & iout;
  data[3] = iout >> 8;
  smbus_->writeBlock(address, PAGE_PLUS_WRITE, data, 4);       //! 2) Write IOUT_OC_WARN_LIMIT
}

/*
 * Set the input voltage fault
 *
 * address: PMBUS address
 * current: fault voltage
 */
void LT_PMBus::setVinOvFaultLimit(uint8_t address, float voltage)
{
  uint16_t vin;

#if USE_FAST_MATH
  vin = math_.float_to_lin11(voltage);        //! 1) Convert to Lin11
#else
  vin = Float_to_L11(voltage);
#endif

  smbus_->writeWord(address, VIN_OV_FAULT_LIMIT, vin);     //! 2) Write VIN_OV_FAULT_LIMIT
}

/*
 * Set the input voltage warning
 *
 * address: PMBUS address
 * current: fault voltage
 */
void LT_PMBus::setVinOvWarningLimit(uint8_t address, float voltage)
{
  uint16_t vin;

#if USE_FAST_MATH
  vin = math_.float_to_lin11(voltage);        //! 1) Convert to Lin11
#else
  vin = Float_to_L11(voltage);
#endif

  smbus_->writeWord(address, VIN_OV_WARN_LIMIT, vin);      //! 2) Convert to VIN_OV_WARN_LIMIT
}

/*
 * Set the input voltage fault
 *
 * address: PMBUS address
 * current: warning voltage
 */
void LT_PMBus::setVinUvFaultLimit(uint8_t address, float voltage)
{
  uint16_t vin;

#if USE_FAST_MATH
  vin = math_.float_to_lin11(voltage);        //! 1) Convert to Lin11
#else
  vin = Float_to_L11(voltage);
#endif

  smbus_->writeWord(address, VIN_UV_FAULT_LIMIT, vin);     //! 2) Write VIN_UV_FAULT_LIMIT
}

/*
 * Set the input voltage warning
 *
 * address: PMBUS address
 * current: warning voltage
 */
void LT_PMBus::setVinUvWarnLimit(uint8_t address, float voltage)
{
  uint16_t vin;

#if USE_FAST_MATH
  vin = math_.float_to_lin11(voltage);        //! 1) Convert to Lin11
#else
  vin = Float_to_L11(voltage);
#endif

  smbus_->writeWord(address, VIN_UV_WARN_LIMIT, vin);      //! 2) Write VIN_UV_WARN_LIMIT
}

/*
 * Set the input current warning
 *
 * address: PMBUS address
 * current: warning current
 */
void LT_PMBus::setIinOcWarnLimit(uint8_t address, float current)
{
  uint16_t iin;

#if USE_FAST_MATH
  iin = math_.float_to_lin11(current);        //! 1) Convert to Lin11
#else
  iin = Float_to_L11(current);
#endif

  smbus_->writeWord(address, IIN_OC_WARN_LIMIT, iin);      //! 2) Write IIN_OC_WARN_LIMIT
}

/*
 * Set the over temperature fault
 *
 * address: PMBUS address
 * current: warning temperature
 */
void LT_PMBus::setOtFaultLimit(uint8_t address, float temperature)
{
  uint16_t temp;

#if USE_FAST_MATH
  temp = math_.float_to_lin11(temperature);       //! 1) Convert to Lin11
#else
  temp = Float_to_L11(temperature);
#endif

  smbus_->writeWord(address, OT_FAULT_LIMIT, temp);        //! 2) Write OT_FAULT_LIMIT
}


/*
 * Set the over temperature warning
 *
 * address: PMBUS address
 * current: warning temperature
 */
void LT_PMBus::setOtWarnLimit(uint8_t address, float temperature)
{
  uint16_t temp;

#if USE_FAST_MATH
  temp = math_.float_to_lin11(temperature);       //! 1) Convert to Lin11
#else
  temp = Float_to_L11(temperature);
#endif
  smbus_->writeWord(address, OT_WARN_LIMIT, temp);     //! 2) Write OT_WARN_LIMIT
}

/*
 * Set the over temperature fault
 *
 * address: PMBUS address
 * current: warning temperature
 */
void LT_PMBus::setOtFaultLimitWithPage(uint8_t address, float temperature, uint8_t page)
{
  setPage(address, page);
  setOtFaultLimit(address, temperature);
}

/*
 * Set the over temperature fault
 *
 * address: PMBUS address
 * current: warning temperature
 */
void LT_PMBus::setOtFaultLimitWithPagePlus(uint8_t address, float temperature, uint8_t page)
{
  uint16_t temp;
  uint8_t data[4];

#if USE_FAST_MATH
  temp = math_.float_to_lin11(temperature);       //! 1) Convert to Lin11
#else
  temp = Float_to_L11(temperature);
#endif


  data[0] = page;
  data[1] = OT_FAULT_LIMIT;
  data[2] = 0xFF & temp;
  data[3] = temp >> 8;

  smbus_->writeBlock(address, PAGE_PLUS_WRITE, data, 4);       //! 2) Write OT_FAULT_LIMIT
}


/*
 * Set the under temperature fault
 *
 * address: PMBUS address
 * current: warning temperature
 */
void LT_PMBus::setUtFaultLimit(uint8_t address, float temperature)
{
  uint16_t temp;

#if USE_FAST_MATH
  temp = math_.float_to_lin11(temperature);       //! 1) Convert to Lin11
#else
  temp = Float_to_L11(temperature);
#endif

  smbus_->writeWord(address, UT_FAULT_LIMIT, temp);        //! 2) Write UT_FAULT_LIMIT
}

/*
 * Set the over temperature warning
 *
 * address: PMBUS address
 * current: warning temperature
 */
void LT_PMBus::setOtWarnLimitWithPage(uint8_t address, float temperature, uint8_t page)
{
  setPage(address, page);
  setOtWarnLimit(address, temperature);
}

/*
 * Set the over temperature warning
 *
 * address: PMBUS address
 * current: warning temperature
 */
void LT_PMBus::setOtWarnLimitWithPagePlus(uint8_t address, float temperature, uint8_t page)
{
  uint16_t temp;
  uint8_t data[4];

#if USE_FAST_MATH
  temp = math_.float_to_lin11(temperature);       //! 1) Convert to Lin11
#else
  temp = Float_to_L11(temperature);
#endif

  data[0] = page;
  data[1] = OT_WARN_LIMIT;
  data[2] = 0xFF & temp;
  data[3] = temp >> 8;

  smbus_->writeBlock(address, PAGE_PLUS_WRITE, data, 4);       //! 2) Write OT_WARN_LIMIT
}

/*
 * Set the under temperature warning
 *
 * address: PMBUS address
 * current: warning temperature
 */
void LT_PMBus::setUtWarnLimit(uint8_t address, float temperature)
{
  uint16_t temp;

#if USE_FAST_MATH
  temp = math_.float_to_lin11(temperature);       //! 1) Confert to Lin11
#else
  temp = Float_to_L11(temperature);
#endif

  smbus_->writeWord(address, UT_WARN_LIMIT, temp);     //! Write UT_WARN_LIMIT
}

/*
 * Set the under temperature fault
 *
 * address: PMBUS address
 * current: warning temperature
 */
void LT_PMBus::setUtFaultLimitWithPage(uint8_t address, float temperature, uint8_t page)
{
  setPage(address, page);
  setUtFaultLimit(address, temperature);
}

/*
 * Set the under temperature fault
 *
 * address: PMBUS address
 * current: warning temperature
 */
void LT_PMBus::setUtFaultLimitWithPagePlus(uint8_t address, float temperature, uint8_t page)
{
  uint16_t temp;
  uint8_t data[4];

#if USE_FAST_MATH
  temp = math_.float_to_lin11(temperature);
#else
  temp = Float_to_L11(temperature);       //! 1) Convert to Lin11
#endif

  data[0] = page;
  data[1] = UT_FAULT_LIMIT;
  data[2] = 0xFF & temp;
  data[3] = temp >> 8;

  smbus_->writeBlock(address, PAGE_PLUS_WRITE, data, 4);       //! 2) Write UT_FAULT_LIMIT
}

/*
 * Set the under temperature warning
 *
 * address: PMBUS address
 * current: warning temperature
 */
void LT_PMBus::setUtWarnLimitWithPage(uint8_t address, float temperature, uint8_t page)
{
  setPage(address, page);
  setUtWarnLimit(address, temperature);
}

/*
 * Set the under temperature warning
 *
 * address: PMBUS address
 * current: warning temperature
 */
void LT_PMBus::setUtWarnLimitWithPagePlus(uint8_t address, float temperature, uint8_t page)
{
  uint16_t temp;
  uint8_t data[4];

#if USE_FAST_MATH
  temp = math_.float_to_lin11(temperature);       //! 1) Convert to Lin11
#else
  temp = Float_to_L11(temperature);
#endif

  data[0] = page;
  data[1] = UT_WARN_LIMIT;
  data[2] = 0xFF & temp;
  data[3] = temp >> 8;

  smbus_->writeBlock(address, PAGE_PLUS_WRITE, data, 4);       //! 2) Write UT_WARN_LIMIT
}

/*
 * Set the temperature gain
 *
 * address: PMBUS address
 * gain: gain
 */
void LT_PMBus::setTemp1Gain(uint8_t address, uint16_t gain)
{
  smbus_->writeWord(address, MFR_TEMP_1_GAIN, gain);        //! 1) Write MFR_TEMP_1_GAIN with no conversion
}

/*
 * Get the temperature gain
 *
 * address: PMBUS address
 * return: gain
 */
uint16_t LT_PMBus::getTemp1Gain(uint8_t address)
{
  uint16_t temp;

  temp = smbus_->readWord(address, MFR_TEMP_1_GAIN);       //! 1) Read MFR_TEMP_1_GAIN
  return temp;
}

/*
 * Set the temperature offset
 *
 * address: PMBUS address
 * offset: offset
 */
void LT_PMBus::setTemp1Offset(uint8_t address, float offset)
{
  uint16_t temp;

#if USE_FAST_MATH
  temp = math_.float_to_lin11(offset);       //! 1) Confert to Lin11
#else
  temp = Float_to_L11(offset);
#endif

  smbus_->writeWord(address, MFR_TEMP_1_OFFSET, temp);     //! Write MFR_TEMP_1_OFFSET
}

/*
 * Get the temperature offset
 *
 * address: PMBUS address
 * return: offset
 */
float LT_PMBus::getTemp1Offset(uint8_t address)
{
  uint16_t temp;

  temp = smbus_->readWord(address, MFR_TEMP_1_OFFSET);     //! 1) Read MFR_TEMP1_OFFSET

#if USE_FAST_MATH
  return math_.lin11_to_float(temp);      //! 2) Convert from Lin11
#else
  return L11_to_Float(temp);
#endif
}

/*
 * get the over temperature warning
 *
 * address: PMBUS address
 * current: warning temperature
 */
float LT_PMBus::getOtWarnLimit(uint8_t address)
{
  uint16_t temp;

  temp = smbus_->readWord(address, OT_WARN_LIMIT);     //! 1) Read OT_WARN_LIMIT

#if USE_FAST_MATH
  return math_.lin11_to_float(temp);      //! 2) Convert from Lin11
#else
  return L11_to_Float(temp);
#endif
}

/*
 * get the over temperature warning
 *
 * address: PMBUS address
 * current: warning temperature
 */
float LT_PMBus::getOtWarnLimitWithPage(uint8_t address, uint8_t page)
{
  setPage(address, page);
  return getOtWarnLimit(address);
}

/*
 * get the over temperature warning
 *
 * address: PMBUS address
 * current: warning temperature
 */
float LT_PMBus::getOtWarnLimitWithPagePlus(uint8_t address, uint8_t page)
{
  uint16_t temp_L11;
  uint8_t data_in[2];
  uint8_t data_out[2];

  data_out[0] = page;
  data_out[1] = OT_WARN_LIMIT;
  smbus_->writeReadBlock(address, PAGE_PLUS_READ, data_out, 2, data_in, 2);        //! 1) Read OT_WARN_LIMIT
  temp_L11 = (data_in[1] << 8) | data_in[0];
#if USE_FAST_MATH
  return  math_.lin11_to_float(temp_L11);     //! 2) Convert from Lin11
#else
  return L11_to_Float(temp_L11);
#endif
}

/*
 * Read the input voltage
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the input voltage
 */
float LT_PMBus::readVin(uint8_t address, bool polling)
{
  uint16_t vin_L11;

  // read the input voltage as an L11
  if (polling)        //! 1) Read READ_VIN
    vin_L11 = pmbusReadWordWithPolling(address, READ_VIN);
  else
    vin_L11 = smbus_->readWord(address, READ_VIN);

  // convert L11 value to floating point value
#if USE_FAST_MATH
  return math_.lin11_to_float(vin_L11);       //! 2) Convert from L11
#else
  return L11_to_Float(vin_L11);
#endif
}

/*
 * Read the output high fault voltage
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the output fault voltage
 */
float LT_PMBus::getVoutOv(uint8_t address, bool polling)
{
#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  uint16_t vout_L16;

  // read the output voltage as an L16
  if (polling)
  {
    vout_L16 = pmbusReadWordWithPolling(address, VOUT_OV_FAULT_LIMIT);      //! 1) Read VOUT_OV_FAULT_LIMIT
    vout_mode = (LT_PMBusMath::lin16_t)(pmbusReadByteWithPolling(address, VOUT_MODE) & 0x1F);       //! 2) Read VOUT_MODE & 0x1F
    return math_.lin16_to_float(vout_L16, vout_mode);       //! 3) Convert from L16
  }
  else
  {
    vout_L16 = smbus_->readWord(address, VOUT_OV_FAULT_LIMIT);
    vout_mode = (LT_PMBusMath::lin16_t)(pmbusReadByteWithPolling(address, VOUT_MODE) & 0x1F);
    return math_.lin16_to_float(vout_L16, vout_mode);
  }
#else
  uint16_t vout_L16;

  // read the output voltage as an L16
  if (polling)
  {
    vout_L16 = pmbusReadWordWithPolling(address, VOUT_OV_FAULT_LIMIT);
    return L16_to_Float_with_polling(address, vout_L16);
  }
  else
  {
    vout_L16 = smbus_->readWord(address, VOUT_OV_FAULT_LIMIT);
    return L16_to_Float(address, vout_L16);
  }
#endif
}

/*
 * Read the output high fault voltage
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the output fault voltage
 * page: page
 */
float LT_PMBus::getVoutOvWithPage(uint8_t address, uint8_t page)
{
  setPage(address, page);
  return getVoutOv(address, false);
}

/*
 * Read the output high fault voltage
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the output fault voltage
 * page: page
 */
float LT_PMBus::getVoutOvWithPagePlus(uint8_t address, uint8_t page)
{
//CHECK THIS
  uint16_t vout_L16;
  uint8_t data_in[2];
  uint8_t data_out[2];
#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
#endif
  data_out[0] = page;
  data_out[1] = READ_VOUT;
  smbus_->writeReadBlock(address, PAGE_PLUS_READ, data_out, 2, data_in, 2);        //! 1) Read READ_VOUT
  vout_L16 = (data_in[1] << 8) | data_in[0];
#if USE_FAST_MATH
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! 1) Read VOUT_MODE & 0x1F
  return math_.lin16_to_float(vout_L16, vout_mode);       //! 3) Convert from L16
#else
  return L16_to_Float(address, vout_L16);
#endif
}

/*
 * Read the output  voltage
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the output voltage
 */
float LT_PMBus::readVout(uint8_t address, bool polling)
{
  int8_t vout_mode;
  uint16_t vout_L16;

#if USE_FAST_MATH
  // Read the output voltage as an L16
  if (polling)
  {
    vout_L16 = pmbusReadWordWithPolling(address, READ_VOUT);        //! 1) Read READ_VOUT
    vout_mode = (LT_PMBusMath::lin16_t)(pmbusReadByteWithPolling(address, VOUT_MODE) & 0x1F);       //! 2) Read VOUT_MODE & 0x1F
    return math_.lin16_to_float(vout_L16, vout_mode);           //! 3) Convert from Lin16
  }
  else
  {
    vout_L16 = smbus_->readWord(address, READ_VOUT);
    vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);
    return math_.lin16_to_float(vout_L16, vout_mode);
  }
#else
  // Read the output voltage as an L16
  if (polling)
  {
    vout_L16 = pmbusReadWordWithPolling(address, READ_VOUT);        //! 1) Read READ_VOUT
    vout_mode = (LT_PMBusMath::lin16_t)(pmbusReadByteWithPolling(address, VOUT_MODE) & 0x1F);       //! 2) Read VOUT_MODE & 0x1F
    return L16_to_Float_mode(address, vout_L16);            //! 3) Convert from Lin16
  }
  else
  {
    vout_L16 = smbus_->readWord(address, READ_VOUT);
    vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);
    return L16_to_Float_mode(vout_mode, vout_L16);
  }
#endif
}

float LT_PMBus::getVout(uint8_t address, bool polling)
{
  int8_t vout_mode;
  uint16_t vout_L16;

#if USE_FAST_MATH
  // Read the output voltage as an L16
  if (polling)
  {
    vout_L16 = pmbusReadWordWithPolling(address, VOUT_COMMAND);        //! 1) Read READ_VOUT
    vout_mode = (LT_PMBusMath::lin16_t)(pmbusReadByteWithPolling(address, VOUT_MODE) & 0x1F);       //! 2) Read VOUT_MODE & 0x1F
    return math_.lin16_to_float(vout_L16, vout_mode);           //! 3) Convert from Lin16
  }
  else
  {
    vout_L16 = smbus_->readWord(address, VOUT_COMMAND);
    vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);
    return math_.lin16_to_float(vout_L16, vout_mode);
  }
#else
  // Read the output voltage as an L16
  if (polling)
  {
    vout_L16 = pmbusReadWordWithPolling(address, VOUT_COMMAND);        //! 1) Read READ_VOUT
    vout_mode = (LT_PMBusMath::lin16_t)(pmbusReadByteWithPolling(address, VOUT_MODE) & 0x1F);       //! 2) Read VOUT_MODE & 0x1F
    return L16_to_Float_mode(address, vout_L16);            //! 3) Convert from Lin16
  }
  else
  {
    vout_L16 = smbus_->readWord(address, VOUT_COMMAND);
    vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);
    return L16_to_Float_mode(vout_mode, vout_L16);
  }
#endif
}

/*
 * Read the output  voltage
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the output voltage
 * page: page
 */
float LT_PMBus::readVoutWithPage(uint8_t address, uint8_t page)
{
  setPage(address, page);
  return readVout(address, false);
}

/*
 * Read the output  voltage
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the output voltage
 * page: page
 */
float LT_PMBus::readVoutWithPagePlus(uint8_t address, uint8_t page)
{
  uint16_t vout_L16;
  uint8_t data_in[2];
  uint8_t data_out[2];
#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
#endif
  data_out[0] = page;
  data_out[1] = READ_VOUT;
  smbus_->writeReadBlock(address, PAGE_PLUS_READ, data_out, 2, data_in, 2);        //! 1) Read READ_VOUT
  vout_L16 = (data_in[1] << 8) | data_in[0];
#if USE_FAST_MATH
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! 2) Read VOUT_MODE & 0x1F
  return math_.lin16_to_float(vout_L16, vout_mode);       //! 3) Convert from Lin16
#else
  return L16_to_Float(address, vout_L16);
#endif
}

/*
 * Read the output low fault voltage
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the output voltage
 */
float LT_PMBus::getVoutUv(uint8_t address, bool polling)
{
#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
  uint16_t vout_L16;

  // read the output voltage as an L16
  if (polling)
  {
    vout_L16 = pmbusReadWordWithPolling(address, VOUT_UV_FAULT_LIMIT);      //! 1) Read VOUT_UV_FAULT_LIMIT
    vout_mode = (LT_PMBusMath::lin16_t)(pmbusReadByteWithPolling(address, VOUT_MODE) & 0x1F);       //! 2) Read VOUT_MODE & 0x1F
    return math_.lin16_to_float(vout_L16, vout_mode);       //! 3) Convert frmo Lin16
  }
  else
  {
    vout_L16 = smbus_->readWord(address, VOUT_UV_FAULT_LIMIT);
    vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);
    return math_.lin16_to_float(vout_L16, vout_mode);
  }
#else
  uint16_t vout_L16;

  // read the output voltage as an L16
  if (polling)
  {
    vout_L16 = pmbusReadWordWithPolling(address, VOUT_UV_FAULT_LIMIT);
    return L16_to_Float_with_polling(address, vout_L16);
  }
  else
  {
    vout_L16 = smbus_->readWord(address, VOUT_UV_FAULT_LIMIT);
    return L16_to_Float(address, vout_L16);
  }
#endif
}

/*
 * Read the output low fault voltage
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the output voltage
 * page: page
 */
float LT_PMBus::getVoutUvWithPage(uint8_t address, uint8_t page)
{
  setPage(address, page);
  return getVoutUv(address, false);
}

/*
 * Read the output low fault voltage
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the output voltage
 * page: page
 */
float LT_PMBus::getVoutUvWithPagePlus(uint8_t address, uint8_t page)
{
  uint16_t vout_L16;
  uint8_t data_in[2];
  uint8_t data_out[2];
#if USE_FAST_MATH
  LT_PMBusMath::lin16_t vout_mode;
#endif
  data_out[0] = page;
  data_out[1] = VOUT_UV_FAULT_LIMIT;
  smbus_->writeReadBlock(address, PAGE_PLUS_READ, data_out, 2, data_in, 2);        //! 1) Read VOUT_UV_FAULT_LIMIT
  vout_L16 = (data_in[1] << 8) | data_in[0];
#if USE_FAST_MATH
  vout_mode = (LT_PMBusMath::lin16_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);        //! 2) Read VOUT_MODE & 0x1F
  return math_.lin16_to_float(vout_L16, vout_mode);       //! 3) Convert from Lin16
#else
  return L16_to_Float(address, vout_L16);
#endif
}


/*
 * Read the input current
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the input current
 */
float LT_PMBus::readIin(uint8_t address, bool polling)
{
  uint16_t iin_L11;
  uint8_t command = READ_IIN;
  PsmDeviceType t = deviceType(address);
  if (t == LTC3880)
    command = MFR_READ_IIN;

  // read the input current as an L11
  if (polling)
    iin_L11 = pmbusReadWordWithPolling(address, command);       //! 1) Read READ_IIN
  else
    iin_L11 = smbus_->readWord(address, command);

  // convert L11 value to floating point value
#if USE_FAST_MATH
  return math_.lin11_to_float(iin_L11);       //! 2) Convert from Lin11
#else
  return L11_to_Float(iin_L11);
#endif
}

/*
 * Read the output fault current
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the fault current
 */
float LT_PMBus::getIoutOc(uint8_t address, bool polling)
{
  uint16_t iout_L11;

  // read the output current as an L11
  if (polling)
    iout_L11 = pmbusReadWordWithPolling(address, IOUT_OC_FAULT_LIMIT);      //! 1) Read IOUT_OC_FAULT_LIMIT
  else
    iout_L11 = smbus_->readWord(address, IOUT_OC_FAULT_LIMIT);

  // convert L11 value to floating point value
#if USE_FAST_MATH
  return math_.lin11_to_float(iout_L11);      //! 2) Convert from Lin11
#else
  return L11_to_Float(iout_L11);
#endif
}

/*
 * Read the output fault current
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the fault current
 * page: page
 */
float LT_PMBus::getIoutOcWithPage(uint8_t address, uint8_t page)
{
  setPage(address, page);
  return getIoutOc(address, false);
}

/*
 * Read the output fault current
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the fault current
 * page: page
 */
float LT_PMBus::getIoutOcWithPagePlus(uint8_t address, uint8_t page)
{
  uint16_t iout_L11;
  uint8_t data_in[2];
  uint8_t data_out[2];

  data_out[0] = page;
  data_out[1] = IOUT_OC_FAULT_LIMIT;
  smbus_->writeReadBlock(address, PAGE_PLUS_READ, data_out, 2, data_in, 2);        //! 1) Read IOUT_OC_FAULT_LIMIT
  iout_L11 = (data_in[1] << 8) | data_in[0];
#if USE_FAST_MATH
  return math_.lin11_to_float(iout_L11);      //! 2) Convert from Lin11
#else
  return L11_to_Float(iout_L11);
#endif
}

/*
 * Read the output current
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the output current
 */
float LT_PMBus::readIout(uint8_t address, bool polling)
{
  uint16_t iout_L11;

  // read the output current as an L11
  if (polling)
    iout_L11 = pmbusReadWordWithPolling(address, READ_IOUT);        //! 1) REad READ_IOUT
  else
    iout_L11 = smbus_->readWord(address, READ_IOUT);

  // convert L11 value to floating point value
#if USE_FAST_MATH
  return math_.lin11_to_float(iout_L11);      //! 2) Convert from Lin11
#else
  return L11_to_Float(iout_L11);
#endif
}

/*
 * Read the output current
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the output current
 * page: page
 */
float LT_PMBus::readIoutWithPage(uint8_t address, uint8_t page)
{
  setPage(address, page);
  return readIout(address, page);
}

/*
 * Read the output current
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the output current
 * page: page
 */
float LT_PMBus::readIoutWithPagePlus(uint8_t address, uint8_t page)
{
  //CHECK THIS
  uint16_t iout_L11;
  uint8_t data_in[2];
  uint8_t data_out[2];

  data_out[0] = page;
  data_out[1] = READ_IOUT;
  smbus_->writeReadBlock(address, PAGE_PLUS_READ, data_out, 2, data_in, 2);        //! Read READ_IOUT
  iout_L11 = (data_in[1] << 8) | data_in[0];
#if USE_FAST_MATH
  return math_.lin11_to_float(iout_L11);      //! 2) Convert from Lin11
#else
  return L11_to_Float(iout_L11);
#endif
}

/*
 * Read the output power
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the output power
 */
float LT_PMBus::readPout(uint8_t address, bool polling)
{
  uint16_t pout_L11;

  // read the output current as an L11
  if (polling)
    pout_L11 = pmbusReadWordWithPolling(address, READ_POUT);        //! 1) REad READ_IOUT
  else
    pout_L11 = smbus_->readWord(address, READ_POUT);

  // convert L11 value to floating point value
#if USE_FAST_MATH
  return math_.lin11_to_float(pout_L11);      //! 2) Convert from Lin11
#else
  return L11_to_Float(pout_L11);
#endif
}

/*
 * Read the output power
 *
 * address: PMBUS address
 * page: page
 * return: the output power
 */
float LT_PMBus::readPoutWithPage(uint8_t address, uint8_t page)
{
  setPage(address, page);
  return readPout(address, false);
}

/*
 * Read the output power
 *
 * address: PMBUS address
 * page: page
 * return: the output power
 */
float LT_PMBus::readPoutWithPagePlus(uint8_t address, uint8_t page)
{
  uint16_t pout_L11;
  uint8_t data_in[2];
  uint8_t data_out[2];

  data_out[0] = page;
  data_out[1] = READ_POUT;

  smbus_->writeReadBlock(address, PAGE_PLUS_READ, data_out, 2, data_in, 2);        //! Read READ_POUT
  pout_L11 = (data_in[1] << 8) | data_in[0];
#if USE_FAST_MATH
  return math_.lin11_to_float(pout_L11);      //! 2) Convert from Lin11
#else
  return L11_to_Float(pout_L11);
#endif
}


/*
 * Read the input power
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the output current
 */
float LT_PMBus::readPin(uint8_t address, bool polling)
{
  uint16_t iout_L11;

  // read the output current as an L11
  if (polling)
    iout_L11 = pmbusReadWordWithPolling(address, READ_PIN);     //! 1) REad READ_IOUT
  else
    iout_L11 = smbus_->readWord(address, READ_PIN);

  // convert L11 value to floating point value
#if USE_FAST_MATH
  return math_.lin11_to_float(iout_L11);      //! 2) Convert from Lin11
#else
  return L11_to_Float(iout_L11);
#endif
}

/*
 * Read the external temperature
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the temperature
 */
float LT_PMBus::readExternalTemperature(uint8_t address, bool polling)
{
  uint16_t temp_L11;

  // read the output current as an L11
  if (polling)
    temp_L11 = pmbusReadWordWithPolling(address, READ_OTEMP);     //! 1) Read READ_OTEMP
  else
    temp_L11 = smbus_->readWord(address, READ_OTEMP);

  // convert L11 value to floating point value
#if USE_FAST_MATH
  return math_.lin11_to_float(temp_L11);      //! 2) Convert from Lin11
#else
  return L11_to_Float(temp_L11);
#endif
}

/*
 * Read the internal temperature
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the temperature
 */
float LT_PMBus::readInternalTemperature(uint8_t address, bool polling)
{
  uint16_t temp_L11;

  // read the output current as an L11
  if (polling)
    temp_L11 = pmbusReadWordWithPolling(address, READ_ITEMP);     //! 1) Read READ_OTEMP
  else
    temp_L11 = smbus_->readWord(address, READ_ITEMP);

  // convert L11 value to floating point value
#if USE_FAST_MATH
  return math_.lin11_to_float(temp_L11);      //! 2) Convert from Lin11
#else
  return L11_to_Float(temp_L11);
#endif
}

/*
 * Read the duty cycle
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the duty cycle
 */
float LT_PMBus::readDutyCycle(uint8_t address, bool polling)
{
  uint16_t dc_L11;

  // read the output current as an L11
  if (polling)
    dc_L11 = pmbusReadWordWithPolling(address, READ_DUTY_CYCLE);     //! 1) Read READ_DUTY_CYCLE
  else
    dc_L11 = smbus_->readWord(address, READ_DUTY_CYCLE);

  // convert L11 value to floating point value
#if USE_FAST_MATH
  return math_.lin11_to_float(dc_L11);      //! 2) Convert from Lin11
#else
  return L11_to_Float(dc_L11);
#endif
}

/*
 * Read the internal temperature
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the output current
 */
float LT_PMBus::readItemp(uint8_t address)
{
  uint16_t temp_L11;

  // read the output current as an L11
  temp_L11 = smbus_->readWord(address, READ_ITEMP);

  // convert L11 value to floating point value
#if USE_FAST_MATH
  return math_.lin11_to_float(temp_L11);      //! 2) Convert from Lin11
#else
  return L11_to_Float(temp_L11);
#endif
}

/*
 * Read the output temperature
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the temperature
 * page: page
 */
float LT_PMBus::readOtempWithPage(uint8_t address, uint8_t page)
{
  //CHECK THIS
  uint16_t temp_L11;
  uint8_t data_in[2];
  uint8_t data_out[2];

  data_out[0] = page;
  data_out[1] = READ_OTEMP;
  smbus_->writeReadBlock(address, PAGE_PLUS_READ, data_out, 2, data_in, 2);        //! 1)Read READ_OTEMP
  temp_L11 = (data_in[1] << 8) | data_in[0];
#if USE_FAST_MATH
  return math_.lin11_to_float(temp_L11);      //! 2) Convert frmo Lin11
#else
  return L11_to_Float(temp_L11);
#endif
}

/*
 * Read the output temperature
 *
 * address: PMBUS address
 * polling: poll if true
 * return: the output current
 */
float LT_PMBus::readOtemp(uint8_t address)
{
  uint16_t temp_L11;

  // read the output current as an L11
  temp_L11 = smbus_->readWord(address, READ_OTEMP);

  // convert L11 value to floating point value
#if USE_FAST_MATH
  return math_.lin11_to_float(temp_L11);      //! 2) Convert from Lin11
#else
  return L11_to_Float(temp_L11);
#endif
}

void LT_PMBus::setVoutStatusMask(uint8_t address, uint8_t mask)
{
  uint16_t data;

  data = (mask << 8) | STATUS_VOUT;
  smbus_->writeWord(address, SMBALERT_MASK, data);
}

void LT_PMBus::setIoutStatusMask(uint8_t address, uint8_t mask)
{
  uint16_t data;

  data = (mask << 8) | STATUS_IOUT;
  smbus_->writeWord(address, SMBALERT_MASK, data);
}

void LT_PMBus::setInputStatusMask(uint8_t address, uint8_t mask)
{
  uint16_t data;

  data = (mask << 8) | STATUS_INPUT;
  smbus_->writeWord(address, SMBALERT_MASK, data);
}

void LT_PMBus::setTemperatureStatusMask(uint8_t address, uint8_t mask)
{
  uint16_t data;

  data = (mask << 8) | STATUS_TEMP;
  smbus_->writeWord(address, SMBALERT_MASK, data);
}

void LT_PMBus::setCmlStatusMask(uint8_t address, uint8_t mask)
{
  uint16_t data;

  data = (mask << 8) | STATUS_CML;
  smbus_->writeWord(address, SMBALERT_MASK, data);
}

void LT_PMBus::setMfrStatusMask(uint8_t address, uint8_t mask)
{
  uint16_t data;

  data = (mask << 8) | STATUS_MFR_SPECIFIC;
  smbus_->writeWord(address, SMBALERT_MASK, data);
}

uint8_t LT_PMBus::getVoutStatusMask(uint8_t address)
{
  uint8_t data_out[1];
  uint8_t data_in[1];

  data_out[0] = STATUS_VOUT;

  smbus_->writeReadBlock(address, SMBALERT_MASK, data_out, 1, data_in, 1);

  return data_in[0];
}

uint8_t LT_PMBus::getIoutStatusMask(uint8_t address)
{
  uint8_t data_out[1];
  uint8_t data_in[1];

  data_out[0] = STATUS_IOUT;

  smbus_->writeReadBlock(address, SMBALERT_MASK, data_out, 1, data_in, 1);

  return data_in[0];
}

uint8_t LT_PMBus::getInputStatusMask(uint8_t address)
{
  uint8_t data_out[1];
  uint8_t data_in[1];

  data_out[0] = STATUS_INPUT;

  smbus_->writeReadBlock(address, SMBALERT_MASK, data_out, 1, data_in, 0);

  return data_in[0];
}

uint8_t LT_PMBus::getTemperatureStatusMask(uint8_t address)
{
  uint8_t data_out[1];
  uint8_t data_in[1];

  data_out[0] = STATUS_TEMP;

  smbus_->writeReadBlock(address, SMBALERT_MASK, data_out, 1, data_in, 1);

  return data_in[0];
}

uint8_t LT_PMBus::getCmlStatusMask(uint8_t address)
{
  uint8_t data_out[1];
  uint8_t data_in[1];

  data_out[0] = STATUS_CML;

  smbus_->writeReadBlock(address, SMBALERT_MASK, data_out, 1, data_in, 1);

  return data_in[0];
}

uint8_t LT_PMBus::getMfrStatusMask(uint8_t address)
{
  uint8_t data_out[1];
  uint8_t data_in[1];

  data_out[0] = STATUS_MFR_SPECIFIC;

  smbus_->writeReadBlock(address, SMBALERT_MASK, data_out, 1, data_in, 1);

  return data_in[0];
}

uint8_t LT_PMBus::readVoutStatusByte(uint8_t address)
{
  uint8_t status_byte;

  status_byte = smbus_->readByte(address, STATUS_VOUT);
  return status_byte;
}

uint8_t LT_PMBus::readIoutStatusByte(uint8_t address)
{
  uint8_t status_byte;

  status_byte = smbus_->readByte(address, STATUS_IOUT);
  return status_byte;
}

uint8_t LT_PMBus::readInputStatusByte(uint8_t address)
{
  uint8_t status_byte;

  status_byte = smbus_->readByte(address, STATUS_INPUT);
  return status_byte;
}

uint8_t LT_PMBus::readTemperatureStatusByte(uint8_t address)
{
  uint8_t status_byte;

  status_byte = smbus_->readByte(address, STATUS_INPUT);
  return status_byte;
}

uint8_t LT_PMBus::readCmlStatusByte(uint8_t address)
{
  uint8_t status_byte;

  status_byte = smbus_->readByte(address, STATUS_CML);
  return status_byte;
}

uint8_t LT_PMBus::readMfrStatusByte(uint8_t address)
{
  uint8_t status_byte;

  status_byte = smbus_->readByte(address, STATUS_MFR_SPECIFIC);
  return status_byte;
}

uint8_t LT_PMBus::readMfrFaultLogStatusByte(uint8_t address)
{
  uint8_t status_byte;

  status_byte = smbus_->readByte(address, MFR_FAULT_LOG_STATUS);
  return status_byte;
}

uint16_t LT_PMBus::readMfrPads(uint8_t address)
{
  uint16_t status_word;

  status_word = smbus_->readWord(address, MFR_PADS);
  return status_word;
}

/*
 * Read the pmbus revision
 *
 * address: PMBUS address
 * return: revision
 */
uint8_t LT_PMBus::readPmbusRevision(uint8_t address)
{
  uint8_t revision;

  return revision;
}

/*
 * Read the slave revision
 *
 * address: PMBUS address
 * return: revision
 */
void LT_PMBus::readRevision(uint8_t address, uint8_t *revision)
{
  uint8_t buffer[8];

  // This keeps memory problems out of the driver code where it is
  // easier to deal with.
  memcpy(revision, buffer, 7);
  revision[7] = '\0';
}

/*
 * Read the pmbus model
 *
 * address: PMBUS address
 * return: revision
 */
void LT_PMBus::readModel(uint8_t address, uint8_t *model)
{
  uint8_t block_size;

  block_size = smbus_->readBlock(address, MFR_MODEL, model, 8);
  model[block_size] = '\0';
}

/*
 * Read the status byte
 *
 * address: PMBUS address
 * return: status byte
 */
uint8_t LT_PMBus::readStatusByte(uint8_t address)
{
  uint8_t status_byte;

  status_byte = smbus_->readByte(address, STATUS_BYTE);
  return status_byte;
}

/*
 * Read the status byte
 *
 * address: PMBUS address
 * page: PMBUS page
 * return: status byte
 */
uint8_t LT_PMBus::readStatusByteWithPage(uint8_t address, uint8_t page)
{
  setPage(address, page);
  return readStatusByte(address);
}

/*
 * Read the status byte
 *
 * address: PMBUS address
 * page: PMBUS page
 * return: status byte
 */
uint8_t LT_PMBus::readStatusByteWithPagePlus(uint8_t address, uint8_t page)
{
  uint8_t data_in[1];
  uint8_t data_out[2];
  uint8_t status_byte;

  data_out[0] = page;
  data_out[1] = STATUS_BYTE;
  smbus_->writeReadBlock(address, PAGE_PLUS_READ, data_out, 2, data_in, 1);
  status_byte = data_in[0];
  return status_byte;
}

/*
 * Read the status word
 *
 * address: PMBUS address
 * return: status word
 */
uint16_t LT_PMBus::readStatusWord(uint8_t address)
{
  uint16_t status_word;

  status_word = smbus_->readWord(address, STATUS_WORD);
  return status_word;
}

/*
 * Read the status word
 *
 * address: PMBUS address
 * page: PMBUS page
 * return: status word
 */
uint16_t LT_PMBus::readStatusWordWithPage(uint8_t address, uint8_t page)
{
  setPage(address, page);
  return readStatusWord(address);
}

/*
 * Read the status word
 *
 * address: PMBUS address
 * page: PMBUS page
 * return: status word
 */
uint16_t LT_PMBus::readStatusWordWithPagePlus(uint8_t address, uint8_t page)
{
  uint8_t data_in[2];
  uint8_t data_out[2];
  uint8_t status_word;

  data_out[0] = page;
  data_out[1] = STATUS_WORD;
  smbus_->writeReadBlock(address, PAGE_PLUS_READ, data_out, 2, data_in, 2);
  status_word = (data_in[1] << 8) | data_in[0];
  return status_word;
}

/*
 * Read the status CML byte
 *
 * address: PMBUS address
 * return: status byte
 */
uint8_t LT_PMBus::readStatusCml(uint8_t address)
{
  uint8_t status_byte;

  status_byte = smbus_->readByte(address, STATUS_CML);
  return status_byte;
}

/*
 * Enable write protect
 *
 * address: PMBUS address
 */
void LT_PMBus::enableWriteProtect(uint8_t address)
{
  smbus_->writeByte(address, WRITE_PROTECT, 0x80);
}

/*
 * Enable write protect
 */
void LT_PMBus::enableWriteProtectGlobal(void)
{
  smbus_->writeByte(0x5B, WRITE_PROTECT, 0x80);
}

/*
 * Disable write protect
 *
 * address: PMBUS address
 */
void LT_PMBus::disableWriteProtect(uint8_t address)
{
  smbus_->writeByte(address, WRITE_PROTECT, 0x00);
}

/*
 * Disable write protect
 */
void LT_PMBus::disableWriteProtectGlobal(void)
{
  smbus_->writeByte(0x5B, WRITE_PROTECT, 0x00);
}

/*
 * Clear faults
 *
 * address: PMBUS address
 */
void LT_PMBus::clearFaults(uint8_t address)
{
  smbus_->sendByte(address, CLEAR_FAULTS);
}

/*
 * Clear all faults
 *
 * address: PMBUS address
 */
void LT_PMBus::clearAllFaults(uint8_t address)
{
  uint8_t page;

  if (isLtc2974(address))
  {
    for (page = 0x00; page <= 0x03; page++)
    {
      setPage(address, page);
      smbus_->sendByte(address, CLEAR_FAULTS);
    }
  }
  else if (isLtc2977(address))
  {
    for (page = 0x00; page <= 0x07; page++)
    {
      setPage(address, page);
      smbus_->sendByte(address, CLEAR_FAULTS);
    }
  }
  else
  {
    setPage(address, 0xFF);
    smbus_->sendByte(address, CLEAR_FAULTS);
  }
}

/*
 * Clear all faults on all rails
 */
void LT_PMBus::clearFaultsGlobal(void)
{
  // This is the best possible without knowledge of what is on the bus.

  // Clear all pages for devices that can do so.
  setPage(0x5B, 0xFF);
  smbus_->sendByte(0x5B, CLEAR_FAULTS);


  // Use 0x00 for any devices that don't do 0xFF. Will not clear other pages.
  setPage(0x5B, 0x00);
  smbus_->sendByte(0x5B, CLEAR_FAULTS);
}

/*
 * Set the first watch dog value
 *
 * address: PMBUS address
 * delay: time to wait
 */
void LT_PMBus::setMfrWatchdogFirst(uint8_t address, float delay)
{
  uint16_t time;

#if USE_FAST_MATH
  time = math_.float_to_lin11(delay);
#else
  time = Float_to_L11(delay);
#endif

  smbus_->writeWord(address, MFR_WATCHDOG_T_FIRST, time);
}

/*
 * Set the first watch dog value
 *
 * address: PMBUS address
 * delay: time to wait
 */
void LT_PMBus::setMfrWatchdog(uint8_t address, float delay)
{
  uint16_t time;

#if USE_FAST_MATH
  time = math_.float_to_lin11(delay);     //! 1) Convert to Lin11
#else
  time = Float_to_L11(delay);
#endif

  smbus_->writeWord(address, MFR_WATCHDOG_T, time);        //! 2) Write MFR_WATCHDOG_T
}

/*
 * Restore all values from non-volatile memory
 *
 * address: PMBUS address
 */
void LT_PMBus::restoreFromNvm(uint8_t address)
{
  smbus_->sendByte(address, RESTORE_USER_ALL);
}

/*
 * Restore all values from non-volatile memory
 *
 * address: PMBUS address
 */
void LT_PMBus::restoreFromNvmAll(uint8_t *addresses, uint8_t no_addresses)
{
  uint8_t index;
  for (index = 0; index < no_addresses; index++)
    smbus_->sendByte(addresses[index], RESTORE_USER_ALL);
}

/*
 * Restore all values from non-volatile memory
 */
void LT_PMBus::restoreFromNvmGlobal()
{
  smbus_->sendByte(0x5B, RESTORE_USER_ALL);
}

void LT_PMBus::storeToNvm(uint8_t address)
{
  smbus_->sendByte(address, STORE_USER_ALL);
}

void LT_PMBus::storeToNvmAll(uint8_t *addresses, uint8_t no_addresses)
{
  uint8_t index;
  for (index = 0; index < no_addresses; index++)
    smbus_->sendByte(addresses[index], STORE_USER_ALL);
}

void LT_PMBus::storeToNvmGlobal()
{
  smbus_->sendByte(0x5B, STORE_USER_ALL);
}

bool LT_PMBus::unlockNVM(uint8_t address)
{
  uint8_t d;

  if (lockNVM(address))
  {
    Serial.print(F("Unlock failed during lock on address "));
    Serial.println(address, HEX);
    return true;
  }

  waitForNotBusy(address);
  smbus_->writeByte(address, MFR_EE_UNLOCK, 0x2B);
  waitForNotBusy(address);
  smbus_->writeByte(address, MFR_EE_UNLOCK, 0xD4);
  waitForNotBusy(address);

  d = smbus_->readByte(address, MFR_EE_UNLOCK);
  if (d == 0xD4)
    return false;
  else
    return true;
}

bool LT_PMBus::lockNVM(uint8_t address)
{
  uint8_t d;

  waitForNotBusy(address);
  smbus_->writeByte(address, MFR_EE_UNLOCK, 0x00);
  waitForNotBusy(address);
  d = smbus_->readByte(address, MFR_EE_UNLOCK);
  if (d == 0x00)
    return false;
  else
    return true;
}

void LT_PMBus::eraseNVM(uint8_t address)
{
  waitForNotBusy(address);
  smbus_->writeByte(address, MFR_EE_ERASE, 0x2B);
  waitForNotBusy(address);
}

bool LT_PMBus::compareRamWithNvm(uint8_t address)
{
  uint8_t status;

  waitForNotBusy(address);
  smbus_->sendByte(address, MFR_COMPARE_USER_ALL);
  waitForNotBusy(address);
  waitForNvmDone(address);
  status = readStatusByte(address);
  return ((status & 0x02) != 0);
}

/*
 * Set the page register for a list of addresses
 *
 * addresses: list of addresses
 * no_addresses: the length of the list
 */
void LT_PMBus::page(uint8_t *addresses, uint8_t no_addresses)
{
  uint8_t *commands;
  uint8_t *data_bytes;
  uint8_t index;

  commands  = new uint8_t [no_addresses];
  for (index = 0; index < no_addresses; index++)
    commands[index] = PAGE;

  data_bytes  = new uint8_t [no_addresses];
  for (index = 0; index < no_addresses; index++)
    data_bytes[index] = 0xFF;

  smbus_->writeBytes(addresses, commands, data_bytes, no_addresses);

  delete [] commands;
  delete [] data_bytes;
}

/*
 * Turn on the rails for a list of addresses
 *
 * addresses: list of addresses
 * no_addresses: the length of the list
 */
void LT_PMBus::sequenceOn(uint8_t *addresses, uint8_t no_addresses)
{
  uint8_t *commands;
  uint8_t *data_bytes;
  uint8_t index;

  // Not using group protocol because there may be more than one
  // page per device. Timing of the operation is not perfectly aligned.
  commands  = new uint8_t [no_addresses];
  for (index = 0; index < no_addresses; index++)
    commands[index] = OPERATION;

  data_bytes  = new uint8_t [no_addresses];
  for (index = 0; index < no_addresses; index++)
    data_bytes[index] = 0x80;

  smbus_->writeBytes(addresses, commands, data_bytes, no_addresses);

  delete [] commands;
  delete [] data_bytes;
}

/*
 * Turn off the rails for a list of addresses
 *
 * addresses: list of addresses
 * no_addresses: the length of the list
 */
void LT_PMBus::immediateOff(uint8_t *addresses, uint8_t no_addresses)
{
  uint8_t *commands;
  uint8_t *data_bytes;
  uint8_t index;

  // Not using group protocol because there may be more than one
  // page per device. Timing of the operation is not perfectly aligned.
  commands  = new uint8_t [no_addresses];
  for (index = 0; index < no_addresses; index++)
    commands[index] = OPERATION;

  data_bytes  = new uint8_t [no_addresses];
  for (index = 0; index < no_addresses; index++)
    data_bytes[index] = 0x00;

  smbus_->writeBytes(addresses, commands, data_bytes, no_addresses);

  delete [] commands;
  delete [] data_bytes;
}

/*
 * Sequence off the rails for a list of addresses
 *
 * addresses: list of addresses
 * no_addresses: the length of the list
 */
void LT_PMBus::sequenceOff(uint8_t *addresses, uint8_t no_addresses)
{
  uint8_t *commands;
  uint8_t *data_bytes;
  uint8_t index;

  // Not using group protocol because there may be more than one
  // page per device. Timing of the operation is not perfectly aligned.
  commands  = new uint8_t [no_addresses];
  for (index = 0; index < no_addresses; index++)
    commands[index] = OPERATION;

  data_bytes  = new uint8_t [no_addresses];
  for (index = 0; index < no_addresses; index++)
    data_bytes[index] = 0x40;

  smbus_->writeBytes(addresses, commands, data_bytes, no_addresses);

  delete [] commands;
  delete [] data_bytes;
}

/*
 * Sequence off the rails
 *
 */
void LT_PMBus::sequenceOffGlobal(void)
{
  setPage(0x5B, 0xFF);
  smbus_->writeByte(0x5B, OPERATION, 0x40);
}

/*
 * Sequence off the rails
 *
 */
void LT_PMBus::immediateOffGlobal(void)
{
  setPage(0x5B, 0xFF);
  smbus_->writeByte(0x5B, OPERATION, 0x00);
}

/*
 * Sequence on the rails
 *
 */
void LT_PMBus::sequenceOnGlobal(void)
{
  setPage(0x5B, 0xFF);
  smbus_->writeByte(0x5B, OPERATION, 0x80);
}

/*
 * Sequence off the rails
 *
 */
void LT_PMBus::resetGlobal(void)
{
  setPage(0x5B, 0xFF);
  smbus_->sendByte(0x5B, MFR_RESET);
}

void LT_PMBus::reset(uint8_t address)
{
  smbus_->sendByte(address, MFR_RESET);
}

/*
 * Margin high rails for a list of addresses
 *
 * addresses: list of addresses
 * no_addresses: the length of the list
 */
void LT_PMBus::marginHighAll(uint8_t *addresses, uint8_t *pages, uint8_t no_addresses)
{
  uint8_t *addrs;
  uint8_t *commands;
  uint8_t *data_bytes;
  uint8_t index;
  uint8_t pos;

  addrs  = new uint8_t [2*no_addresses];
  commands  = new uint8_t [2*no_addresses];
  pos = 0;
  for (index = 0; index < 2*no_addresses; index+=2)
  {
    addrs[index] = addresses[pos];
    addrs[index+1] = addresses[pos];
    commands[index] = PAGE;
    commands[index+1] = OPERATION;
    pos++;
  }

  data_bytes  = new uint8_t [2*no_addresses];
  pos = 0;
  for (index = 0; index < 2*no_addresses; index+=2)
  {
    data_bytes[index] = pages[pos];
    data_bytes[index+1] = 0xA8;
    pos++;
  }

  smbus_->writeBytes(addrs, commands, data_bytes, 2*no_addresses);

  delete [] addrs;
  delete [] commands;
  delete [] data_bytes;
}

/*
 * Margin low rails for a list of addresses
 *
 * addresses: list of addresses
 * no_addresses: the length of the list
 */
void LT_PMBus::marginLowAll(uint8_t *addresses, uint8_t *pages, uint8_t no_addresses)
{
  uint8_t *addrs;
  uint8_t *commands;
  uint8_t *data_bytes;
  uint8_t index;
  uint8_t pos;

  addrs  = new uint8_t [2*no_addresses];
  commands  = new uint8_t [2*no_addresses];
  pos = 0;
  for (index = 0; index < 2*no_addresses; index+=2)
  {
    addrs[index] = addresses[pos];
    addrs[index+1] = addresses[pos];
    commands[index] = PAGE;
    commands[index+1] = OPERATION;
    pos++;
  }

  data_bytes  = new uint8_t [2*no_addresses];
  pos = 0;
  for (index = 0; index < 2*no_addresses; index+=2)
  {
    data_bytes[index] = pages[pos];
    data_bytes[index+1] = 0x98;
    pos++;
  }

  smbus_->writeBytes(addrs, commands, data_bytes, 2*no_addresses);

  delete [] addrs;
  delete [] commands;
  delete [] data_bytes;
}

void LT_PMBus::marginOffAll(uint8_t *addresses, uint8_t *pages, uint8_t no_addresses)
{
  uint8_t *addrs;
  uint8_t *commands;
  uint8_t *data_bytes;
  uint8_t index;
  uint8_t pos;

  addrs  = new uint8_t [2*no_addresses];
  commands  = new uint8_t [2*no_addresses];
  pos = 0;
  for (index = 0; index < 2*no_addresses; index+=2)
  {
    addrs[index] = addresses[pos];
    addrs[index+1] = addresses[pos];
    commands[index] = PAGE;
    commands[index+1] = OPERATION;
    pos++;
  }

  data_bytes  = new uint8_t [2*no_addresses];
  pos = 0;
  for (index = 0; index < 2*no_addresses; index+=2)
  {
    data_bytes[index] = pages[pos];
    data_bytes[index+1] = 0x80;
    pos++;
  }

  smbus_->writeBytes(addrs, commands, data_bytes, 2*no_addresses);

  delete [] addrs;
  delete [] commands;
  delete [] data_bytes;
}

/*
 * Margin rail high
 *
 */
void LT_PMBus::marginHigh(uint8_t address)
{
  smbus_->writeByte(address, OPERATION, 0xA8);
}

/*
 * Margin rail low
 *
 */
void LT_PMBus::marginLow(uint8_t address)
{
  smbus_->writeByte(address, OPERATION, 0x98);
}

/*
 * Margin rail off
 *
 */
void LT_PMBus::marginOff(uint8_t address)
{
  smbus_->writeByte(address, OPERATION, 0x80);
}

/*
 * Margin all rails high
 *
 */
void LT_PMBus::marginHighGlobal(void)
{
  setPage(0x5B, 0xFF);
  smbus_->writeByte(0x5B, OPERATION, 0xA8);
}

/*
 * Margin all rails low
 *
 */
void LT_PMBus::marginLowGlobal(void)
{
  setPage(0x5B, 0xFF);
  smbus_->writeByte(0x5B, OPERATION, 0x98);
}

/*
 * Margin all rails off
 *
 */
void LT_PMBus::marginOffGlobal(void)
{
  setPage(0x5B, 0xFF);
  smbus_->writeByte(0x5B, OPERATION, 0x80);
}

void LT_PMBus::setUserData03(uint8_t address, uint16_t data)
{
  smbus_->writeWord(address, USER_DATA_03, data);
}

uint16_t LT_PMBus::getUserData03(uint8_t address)
{
  return smbus_->readWord(address, USER_DATA_03);
}

void LT_PMBus::setUserData04(uint8_t address, uint16_t data)
{
  smbus_->writeWord(address, USER_DATA_04, data);
}

uint16_t LT_PMBus::getUserData04(uint8_t address)
{
  return smbus_->readWord(address, USER_DATA_04);
}

void LT_PMBus::setSpareData0(uint8_t address, uint16_t data)
{
  smbus_->writeWord(address, MFR_SPARE_0, data);

}

uint16_t LT_PMBus::getSpareData0(uint8_t address)
{
  return smbus_->readWord(address, MFR_SPARE_0);
}


void LT_PMBus::setRailAddress(uint8_t address, uint8_t rail_address)
{
  smbus_->writeByte(address, MFR_RAIL_ADDRESS, rail_address);
}

uint8_t LT_PMBus::getRailAddress(uint8_t address)
{
  uint8_t rail_address;
  rail_address = smbus_->readByte(address, MFR_RAIL_ADDRESS);
  return rail_address;
}

/*
 *  Set the page of a device using polling
 *
 * address: PMBUS address
 * page: the page
 */
void LT_PMBus::setPageWithPolling(uint8_t address, uint8_t page)
{
  // Set the page of the device to desired_page
  pmbusWriteByteWithPolling(address, PAGE, page);
}

/*
 *  Set the page of a device
 *
 * address: PMBUS address
 * page: the page
 */
void LT_PMBus::setPage(uint8_t address, uint8_t page)
{
  // Set the page of the device to desired_page
  smbus_->writeByte(address, PAGE, page);
}

/*
 *  Get the page of a device
 *
 * address: PMBUS address
 */
uint8_t LT_PMBus::getPage(uint8_t address)
{
  // Set the page of the device to desired_page
  return smbus_->readByte(address, PAGE);
}

void LT_PMBus::enablePec(uint8_t address)
{
  uint8_t config;
  uint16_t config2;

  PsmDeviceType t = deviceType(address);

  if (
    (t == LTC3880)
    || (t == LTC3882)
    || (t == LTC3882_1)
    || (t == LTC3883)
    || (t == LTM4675)
    || (t == LTM4676)
    || (t == LTM4676A)
    || (t == LTM4677)
    || (t == LTC2978)
  )
  {
    config = smbus_->readByte(address, MFR_CONFIG_ALL);
    config |= 0x04;
    smbus_->writeByte(address, MFR_CONFIG_ALL, config);
  }
  else if (
    (t == LTC2974)
    || (t == LTC2975)
    || (t == LTC2977)
    || (t == LTC2980)
    || (t == LTM2987)
  )
  {
    config2 = smbus_->readWord(address, MFR_CONFIG_ALL );
    config2 |= 0x04;
    smbus_->writeWord(address, MFR_CONFIG_ALL, config2);
  }
}

void LT_PMBus::disablePec(uint8_t address)
{
  uint8_t config;
  uint16_t config2;

  PsmDeviceType t = deviceType(address);

  if (
    (t == LTC3880)
    || (t == LTC3882)
    || (t == LTC3882_1)
    || (t == LTC3883)
    || (t == LTM4675)
    || (t == LTM4676)
    || (t == LTM4676A)
    || (t == LTM4677)
    || (t == LTC2978)
  )
  {
    config = smbus_->readByte(address, MFR_CONFIG_ALL);
    config &= ~(0x04);
    smbus_->writeByte(address, MFR_CONFIG_ALL, config);
  }
  else if (
    (t == LTC2974)
    || (t == LTC2975)
    || (t == LTC2977)
    || (t == LTC2980)
    || (t == LTM2987)
  )
  {
    config2 = smbus_->readWord(address, MFR_CONFIG_ALL );
    config2 &= ~(0x04);
    smbus_->writeWord(address, MFR_CONFIG_ALL, config2);
  }
}

/*
 * Wait for the busy bit to clear.
 *
 * This uses the MFR_COMMON BUSY/PEND bits. LTC2978 does not support this bit and
 * will cause a timeout. LTC3883 does not support PEND, which will cause a timeout
 * if not disabled by setting USE_PEND to 0.
 */
uint8_t LT_PMBus::waitForNotBusy(uint8_t address)
{
  uint16_t timeout = 4096;
  uint8_t mfr_common;

  while (timeout-- > 0)
  {
    mfr_common = smbus_->readByte(address, MFR_COMMON);
    // If too busy to answer, poll again.
    if (mfr_common == 0xFF)
      continue;
    // Can add not transition bit for controllers. Managers do not support it.
//    if ((mfr_common  & (NOT_BUSY | NOT_PENDING | NOT_TRANS)) == (NOT_BUSY | NOT_PENDING | NOT_TRANS))
    if ((mfr_common  & (NOT_BUSY | NOT_PENDING)) == (NOT_BUSY | NOT_PENDING))
      return SUCCESS;
  }
  return FAILURE;
}

/*
 * Wait for the nvm done bit to clear.
 *
 * This uses the MFR_EEPROM_STATUS BUSY bit. LTC297X does not support this register.
 */
uint8_t LT_PMBus::waitForNvmDone(uint8_t address)
{
  uint16_t timeout = 8192;
  uint8_t mfr_eeprom_status;

  while (timeout-- > 0)
  {
    smbus_->waitForAck(address, 0x00);
    mfr_eeprom_status = smbus_->readByte(address, MFR_EEPROM_STATUS);
    if (mfr_eeprom_status == 0xFF)
      continue;
    if ((mfr_eeprom_status & 0xC0) == 0)
      return SUCCESS;
  }
  return FAILURE;
}

uint8_t *LT_PMBus::bricks(uint8_t *addresses, uint8_t no_addresses)
{
  uint8_t i,j;
  bool found;
  uint8_t len;

  // Get a list of addresses on the bus
  uint8_t *addresses_on_bus = smbus_->probe(0x00);
  for (len=0; addresses_on_bus[len]!='\0'; ++len);

  // Iterate through given addresses
  for (i = 0; i < no_addresses; i++)
  {
    // Interate on the probed addresses
    found = false;
    for (j = 0; j < len; j++)
    {
      // Check if we match; break if we match
      if (addresses[i] == addresses_on_bus[j])
      {
        found = true;
        break;
      }
    }
    // Found, so we check CML for a MEM fault, just in case the device lands on the correct address.
    // This will not find a bricked device if non-bricked device is at the same effective address
    // and answers to this command with a bit value of zero. The non bricked device will coverup
    // the bricked device. The solution is to define base addresses and ASEL values that guearentee
    // this can never happen.
    if (found && (1 << 4) != (smbus_->readByte(addresses[i], STATUS_CML)  & (1 << 4)))
      addresses[i] = 0x00; // Good device, so remove.
  }


  // Pack the bricked addresses to the left.
  i = 0;
  while (i < no_addresses)
  {
    if (addresses[i] == 0x00)
    {
      j = i + 1;
      while (addresses[i] == 0x00 && j < no_addresses)
      {
        addresses[i] = addresses[j];
        addresses[j] = 0x00;
        j++;
      }
    }
    i++;
  }


  return addresses; // Returning in the origonal array.
}

void LT_PMBus::startGroupProtocol(void)
{
  smbus_->beginStoring();
}


void LT_PMBus::executeGroupProtocol(void)
{
  smbus_->execute();
}

uint16_t LT_PMBus::readMfrSpecialId(uint8_t address)
{
  return smbus_->readWord(address, MFR_SPECIAL_ID);
}

/*
 * Convert L16 value to float with polling
 *
 * address: PMBUS address
 * input_value: the value to convert
 * return: converted value
 *
 * The address is used to get the exponent to use
 */
float LT_PMBus::L16_to_Float_with_polling(uint8_t address, uint16_t input_val)
{
  // Read mode from the VOUT_MODE register of the device
  uint8_t vout_mode = (uint8_t)(pmbusReadByteWithPolling(address, VOUT_MODE) & 0x1F);

  return  L16_to_Float_mode(vout_mode, input_val);
}

/*
 * Convert L16 value to float
 *
 * address: PMBUS address
 * input_value: the value to convert
 * return: converted value
 *
 * The address is used to get the exponent to use
 */
float LT_PMBus::L16_to_Float(uint8_t address, uint16_t input_val)
{
  // Read mode from the VOUT_MODE register of the device
  uint8_t vout_mode = (uint8_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);

  return  L16_to_Float_mode(vout_mode, input_val);
}

/*
 * Convert Float value to L16
 *
 * address: PMBUS address
 * input_value: the value to convert
 * return: converted value
 *
 * The address is used to get the exponent to use
 */
uint16_t LT_PMBus::Float_to_L16(uint8_t address, float input_val)
{
  // Get the mode from the device.
  uint8_t vout_mode = (uint8_t)(smbus_->readByte(address, VOUT_MODE) & 0x1F);

  return Float_to_L16_mode(vout_mode, input_val);
}

/*
 * Convert L16 value to float
 *
 * exponent: vout_mode
 * input_value: the value to convert
 * return: converted value
 *
 * The address is used to get the exponent to use
 */
float LT_PMBus::L16_to_Float_mode(uint8_t vout_mode, uint16_t input_val)
{
  // Assume Linear 16, pull out 5 bits of exponent, and use signed value.
  int8_t exponent = (int8_t) vout_mode & 0x1F;

  // Sign extend exponent from 5 to 8 bits
  if (exponent > 0x0F) exponent |= 0xE0;

  // Convert mantissa to a float so we can do math.
  float mantissa = (float)input_val;

  float value = mantissa * pow(2.0, exponent);

  return  value;
}

/*
 * Convert Float value to L16
 *
 * vout_mode: vout_mode
 * input_value: the value to convert
 * return: converted value
 *
 * The address is used to get the exponent to use
 */
uint16_t LT_PMBus::Float_to_L16_mode(uint8_t vout_mode, float input_val)
{
  // Assume Linear 16, pull out 5 bits of exponent, and use signed value.
  int8_t exponent = vout_mode & 0x1F;

  // Sign extend exponent from 5 to 8 bits
  if (exponent > 0x0F) exponent |= 0xE0;

  // Scale the value to a mantissa based on the exponent
  uint16_t mantissa = (uint16_t)(input_val / pow(2.0, exponent));

  return mantissa;
}


/*
 * Convert L11 value to float
 *
 * input_value: value to convert
 * return: converted value
 */
float LT_PMBus::L11_to_Float(uint16_t input_val)
{
  // Extract exponent as MS 5 bits
  int8_t exponent = (int8_t) (input_val >> 11);

  // Extract mantissa as LS 11 bits
  int16_t mantissa = input_val & 0x7ff;

  // Sign extend exponent from 5 to 8 bits
  if (exponent > 0x0F) exponent |= 0xE0;

  // Sign extend mantissa from 11 to 16 bits
  if (mantissa > 0x03FF) mantissa |= 0xF800;

  // Compute value as mantissa * 2^(exponent)
  return  mantissa * pow(2.0,(float)exponent);
}
/*
 * Convert float to L11
 *
 * input_val: the value to convert
 * return: converted value
 */
uint16_t LT_PMBus::Float_to_L11(float input_val)
{
  uint16_t uExponent;
  uint16_t uMantissa;

  // Set exponent to -16
  int exponent = -16;

  // Extract mantissa from input value
  int mantissa = (int)(input_val / pow(2.0, exponent));
  // Search for an exponent that produces
  // a valid 11-bit mantissa
  do
  {
    if ((mantissa >= -1024) && (mantissa <= +1023))
    {
      break; // stop if mantissa valid
    }
    exponent++;
    mantissa = (int)(input_val / pow(2.0, exponent));
  }
  while (exponent < +15);

  // Format the exponent of the L11
  uExponent = exponent << 11; // Format the mantissa of the L11
  uMantissa = mantissa & 0x07FF;

  // Compute value as exponent | mantissa
  return uExponent | uMantissa;
}
