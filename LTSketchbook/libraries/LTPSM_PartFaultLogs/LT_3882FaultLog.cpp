/*!
LTC SMBus Support: API for a LTC3882 Fault Log

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
//! @defgroup LT_3882FaultLog LT_3882FaultLog
//! @}

/*! @file
    @ingroup LT_3882FaultLog
    Library Header File for LT_3882FaultLog
*/

#include <Arduino.h>
#include "LT_3882FaultLog.h"

#define RAW_EEPROM

LT_3882FaultLog::LT_3882FaultLog(LT_PMBus *pmbus):LT_EEDataFaultLog(pmbus)
{
  faultLog3882 = NULL;

  buffer = NULL;
}


/*
 * Read LTC3882 fault log
 *
 * address: PMBUS address
 */
void
LT_3882FaultLog::read(uint8_t address)
{
#ifdef RAW_EEPROM
  uint8_t *data = (uint8_t *) malloc(sizeof(uint8_t) * 80 * 2); // Becuse CRC is stripped, the acutal data size is smaller

  getNvmBlock(address, 176, 80, 0x00, data);
#else
  uint8_t *data = (uint8_t *) malloc(147);
  data[0] = 0x00;

  pmbus_->smbus()->readBlock(address, MFR_FAULT_LOG, data, 147);
#endif

  faultLog3882 = (struct LT_3882FaultLog::FaultLogLtc3882 *) (data);
}

void LT_3882FaultLog::release()
{
  free(faultLog3882);
  faultLog3882 = 0;
}

uint8_t *LT_3882FaultLog::getBinary()
{
  return (uint8_t *)faultLog3882;
}

uint16_t LT_3882FaultLog::getBinarySize()
{
  return 147;
}

void LT_3882FaultLog::dumpBinary(Print *printer)
{
  dumpBin(printer, (uint8_t *)faultLog3882, 147);
}

void LT_3882FaultLog::print(Print *printer)
{
  if (printer == 0)
    printer = &Serial;
  buffer = new char[FILE_TEXT_LINE_MAX];

  printTitle(printer);

  printTime(printer);

  printPeaks(printer);

  printAllLoops(printer);


  delete [] buffer;
}


void LT_3882FaultLog::printTitle(Print *printer)
{
  printer->print(F("LTC3882 Log Data\n"));

  uint8_t position = faultLog3882->preamble.position_fault;
  switch (position)
  {
    case 0xFF :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position MFR_FAULT_LOG_STORE\n"));
      break;
    case 0x00 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position TON_MAX_FAULT Channel 0\n"));
      break;
    case 0x01 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position VOUT_OV_FAULT Channel 0\n"));
      break;
    case 0x02 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position VOUT_UV_FAULT Channel 0\n"));
      break;
    case 0x03 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position IOUT_OC_FAULT Channel 0\n"));
      break;
    case 0x05 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position OT_FAULT Channel 0\n"));
      break;
    case 0x06 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position UT_FAULT Channel 0\n"));
      break;
    case 0x07 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position VIN_OV_FAULT Channel 0\n"));
      break;
    case 0x0A :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position MFR_OT_FAULT Channel 0\n"));
      break;
    case 0x10 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position TON_MAX_FAULT Channel 1\n"));
      break;
    case 0x11 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position VOUT_OV_FAULT Channel 1\n"));
      break;
    case 0x12 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position VOUT_UV_FAULT Channel 1\n"));
      break;
    case 0x13 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position IOUT_OC_FAULT Channel 1\n"));
      break;
    case 0x15 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position OT_FAULT Channel 1\n"));
      break;
    case 0x16 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position UT_FAULT Channel 1\n"));
      break;
    case 0x17 :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position VIN_OV_FAULT Channel 1\n"));
      break;
    case 0x1A :
      snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Position MFR_OT_FAULT Channel 1\n"));
      break;
  }

  printer->print(buffer);
}

void LT_3882FaultLog::printTime(Print *printer)
{
  uint8_t *time = (uint8_t *)&faultLog3882->preamble.shared_time;
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Time 0x%02x%02x%02x%02x%02x%02x\n"), time[5], time[4], time[3], time[2], time[1], time[0]);
  printer->print(buffer);
  printer->print((long) getSharedTime200us(faultLog3882->preamble.shared_time));
  printer->println(F(" Ticks (200us each)"));
}

void LT_3882FaultLog::printPeaks(Print *printer)
{
  printer->println(F("\nHeader Information:\n--------"));
  printer->print(F("VOUT Peak 0 "));
  printer->println(math_.lin16_to_float(getLin16WordReverseVal(faultLog3882->preamble.peaks.mfr_vout_peak_p0), 0x14), 6);
  printer->print(F("VOUT Peak 1 "));
  printer->println(math_.lin16_to_float(getLin16WordReverseVal(faultLog3882->preamble.peaks.mfr_vout_peak_p1), 0x14), 6);
  printer->print(F("IOUT Peak 0 "));
  printer->println(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3882->preamble.peaks.mfr_iout_peak_p0)), 6);
  printer->print(F("IOUT Peak 1 "));
  printer->println(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3882->preamble.peaks.mfr_iout_peak_p1)), 6);
  printer->print(F("VIN Peak "));
  printer->println(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3882->preamble.peaks.mfr_vin_peak)), 6);
  printer->print(F("Temp External Last Event Page 0 "));
  printer->println(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3882->preamble.peaks.read_temperature_1_p0)), 6);
  printer->print(F("Temp External Last Event Page 1 "));
  printer->println(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3882->preamble.peaks.read_temperature_1_p1)), 6);
  printer->print(F("Temp Internal Last Event "));
  printer->println(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3882->preamble.peaks.read_temperature_2)), 6);
}

void LT_3882FaultLog::printAllLoops(Print *printer)
{
  printer->println(F("\nFault Log Loops Follow:"));
  printer->println(F("(most recent data first)"));

  for (int index = 0; index < 6; index++)
  {
    printLoop(index, printer);
  }
}

void LT_3882FaultLog::printLoop(uint8_t index, Print *printer)
{
  printer->println(F("-------"));
  printer->print(F("Loop: "));
  printer->println(index);
  printer->println(F("-------"));

  printer->print(F("Input: "));
  printer->print(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3882->fault_log_loop[index].read_vin)), 6);
  printer->println(F(" V"));
  printer->print(F("Chan0: "));
  printer->print(math_.lin16_to_float(getLin16WordReverseVal(faultLog3882->fault_log_loop[index].read_vout_p0), 0x14), 6);
  printer->print(F(" V, "));
  printer->print(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3882->fault_log_loop[index].read_iout_p0)), 6);
  printer->println(F(" A"));
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_VOUT: 0x%02x\n"), getRawByteVal(faultLog3882->fault_log_loop[index].status_vout_p0));
  printer->print(buffer);
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_MFR_SPECIFIC: 0x%02x\n"), getRawByteVal(faultLog3882->fault_log_loop[index].status_mfr_specificP0));
  printer->print(buffer);
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_WORD: 0x%04x\n"), getRawWordReverseVal(faultLog3882->fault_log_loop[index].status_word_p0));
  printer->print(buffer);
  printer->print(F("Chan1: "));
  printer->print(math_.lin16_to_float(getLin16WordReverseVal(faultLog3882->fault_log_loop[index].read_vout_p1), 0x14), 6);
  printer->print(F(" V, "));
  printer->print(math_.lin11_to_float(getLin5_11WordReverseVal(faultLog3882->fault_log_loop[index].read_iout_p1)), 6);
  printer->println(F(" A"));
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_VOUT: 0x%02x\n"), getRawByteVal(faultLog3882->fault_log_loop[index].status_vout_p1));
  printer->print(buffer);
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_MFR_SPECIFIC: 0x%02x\n"), getRawByteVal(faultLog3882->fault_log_loop[index].status_mfr_specificP1));
  printer->print(buffer);
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_WORD: 0x%04x\n"), getRawWordReverseVal(faultLog3882->fault_log_loop[index].status_word_p1));
  printer->print(buffer);
}
