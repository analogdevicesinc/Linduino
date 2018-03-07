/*!
LTC SMBus Support: API for a LTC2975 Fault Log

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
//! @defgroup LT_2975FaultLog LT_2975FaultLog
//! @}

/*! @file
    @ingroup LT_2975FaultLog
    Library Header File for LT_2975FaultLog
*/

#include <Arduino.h>
#include "LT_2975FaultLog.h"

#define RAW_EEPROM

LT_2975FaultLog::LT_2975FaultLog(LT_PMBus *pmbus):LT_CommandPlusFaultLog(pmbus)
{
  faultLog2975 = NULL;

  buffer = NULL;
  voutPeaks = NULL;
  ioutPeaks = NULL;
  tempPeaks = NULL;
  chanStatuses = NULL;
  voutDatas = NULL;
  ioutDatas = NULL;
  poutDatas = NULL;
  tempDatas = NULL;
}


bool LT_2975FaultLog::FaultLogLtc2975::isValidData(void *pos, uint8_t size)
{
  return (uint8_t *)pos >= (uint8_t *)this + firstValidByte && (uint8_t *)pos + (size - 1) <= (uint8_t *)this + lastValidByte;
}

/*
 * Read LTC2975 fault log
 *
 * address: PMBUS address
 */
void
LT_2975FaultLog::read(uint8_t address)
{
  // Copy to RAM
  pmbus_->smbus()->sendByte(address, MFR_FAULT_LOG_RESTORE);
  // Monitor BUSY bit
  while ((pmbus_->smbus()->readByte(address, MFR_COMMON) & (1 << 6)) == 0);

  uint16_t size = sizeof(struct LT_2975FaultLog::FaultLogLtc2975);
  uint8_t *data = (uint8_t *) malloc(size);
  if (data == 0)
    Serial.print(F("bad malloc."));
#ifdef RAW_EEPROM
  getNvmBlock(address, 0, 128, 0xC8, data);
#else
  // Read block data with log
  pmbus_->smbus()->readBlock(address, MFR_FAULT_LOG, data, 255);
#endif

  struct LT_2975FaultLog::FaultLogLtc2975 *log = (struct LT_2975FaultLog::FaultLogLtc2975 *)data;

  log->firstValidByte = 79; // first byte after preamble

  log->lastValidByte = 238; // last byte of loop data

  uint8_t cycle_start = log->preamble.position_last;

  //57 corresponds to datasheet p77, last line 2nd column
  log->loops = (LT_2975FaultLog::FaultLogReadLoopLtc2975 *) (log->telemetryData - 57 + cycle_start);

  faultLog2975 = log;
}


void LT_2975FaultLog::release()
{
  free(faultLog2975);
  faultLog2975 = 0;
}

uint8_t *LT_2975FaultLog::getBinary()
{
  return (uint8_t *)faultLog2975;
}

uint16_t LT_2975FaultLog::getBinarySize()
{
  return 255;
}

void LT_2975FaultLog::dumpBinary(Print *printer)
{
  dumpBin(printer, (uint8_t *)faultLog2975, 255);
}

void LT_2975FaultLog::print(Print *printer)
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


void LT_2975FaultLog::printTitle(Print *printer)
{
  printer->print(F("LTC2975 Log Data\n"));
}

void LT_2975FaultLog::printTime(Print *printer)
{
  uint8_t *time = (uint8_t *)&faultLog2975->preamble.shared_time;
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("Fault Time 0x%02x%02x%02x%02x%02x%02x\n"), time[5], time[4], time[3], time[2], time[1], time[0]);
  printer->print(buffer);
  printer->print((long) getSharedTime200us(faultLog2975->preamble.shared_time));
  printer->println(F(" Ticks (200us each)"));
}

void LT_2975FaultLog::printPeaks(Print *printer)
{
  voutPeaks = new Peak16Words*[4];
  ioutPeaks = new Peak5_11Words*[4];
  tempPeaks = new Peak5_11Words*[4];
  chanStatuses = new ChanStatus*[4];

  voutPeaks[0] = &faultLog2975->preamble.peaks.vout0_peaks;
  voutPeaks[1] = &faultLog2975->preamble.peaks.vout1_peaks;
  voutPeaks[2] = &faultLog2975->preamble.peaks.vout2_peaks;
  voutPeaks[3] = &faultLog2975->preamble.peaks.vout3_peaks;

  ioutPeaks[0] = &faultLog2975->preamble.peaks.iout0_peaks;
  ioutPeaks[1] = &faultLog2975->preamble.peaks.iout1_peaks;
  ioutPeaks[2] = &faultLog2975->preamble.peaks.iout2_peaks;
  ioutPeaks[3] = &faultLog2975->preamble.peaks.iout3_peaks;

  tempPeaks[0] = &faultLog2975->preamble.peaks.temp0_peaks;
  tempPeaks[1] = &faultLog2975->preamble.peaks.temp1_peaks;
  tempPeaks[2] = &faultLog2975->preamble.peaks.temp2_peaks;
  tempPeaks[3] = &faultLog2975->preamble.peaks.temp3_peaks;

  chanStatuses[0] = &faultLog2975->preamble.fault_log_status.chan_status0;
  chanStatuses[1] = &faultLog2975->preamble.fault_log_status.chan_status1;
  chanStatuses[2] = &faultLog2975->preamble.fault_log_status.chan_status2;
  chanStatuses[3] = &faultLog2975->preamble.fault_log_status.chan_status3;


  printer->println(F("\nPeak Values and Fast Status:"));
  printer->println(F("--------"));

  printFastChannel(0, printer);

  float vin_max, vin_min;
  vin_max = math_.lin11_to_float(getLin5_11WordVal(faultLog2975->preamble.peaks.vin_peaks.peak));
  vin_min = math_.lin11_to_float(getLin5_11WordVal(faultLog2975->preamble.peaks.vin_peaks.min));
  printer->print(F("Vin: Min: "));
  printer->print(vin_min, 6);
  printer->print(F(", Peak: "));
  printer->println(vin_max, 6);

  float iin_max, iin_min;
  iin_max = math_.lin11_to_float(getLin5_11WordVal(faultLog2975->preamble.peaks.iin_peaks.peak));
  iin_min = math_.lin11_to_float(getLin5_11WordVal(faultLog2975->preamble.peaks.iin_peaks.min));
  printer->print(F("Iin: Min: "));
  printer->print(iin_min, 6);
  printer->print(F(", Peak: "));
  printer->println(iin_max, 6);
  printer->println();

  float pin_max, pin_min;
  pin_max = math_.lin11_to_float(getLin5_11WordVal(faultLog2975->preamble.peaks.pin_peaks.peak));
  pin_min = math_.lin11_to_float(getLin5_11WordVal(faultLog2975->preamble.peaks.pin_peaks.min));
  printer->print(F("Pin: Min: "));
  printer->print(pin_min, 6);
  printer->print(F(", Peak: "));
  printer->println(pin_max, 6);
  printer->println();

  printFastChannel(1, printer);
  printFastChannel(2, printer);
  printFastChannel(3, printer);

  delete [] voutPeaks;
  delete [] ioutPeaks;
  delete [] tempPeaks;
  delete [] chanStatuses;
}

void LT_2975FaultLog::printFastChannel(uint8_t index, Print *printer)
{
  float vout_peak, vout_min, iout_peak, iout_min, temp_peak, temp_min;
  uint8_t status;
  vout_peak = math_.lin16_to_float(getLin16WordVal(voutPeaks[index]->peak), 0x13);
  vout_min = math_.lin16_to_float(getLin16WordVal(voutPeaks[index]->min), 0x13);
  printer->print(F("Vout"));
  printer->print(index);
  printer->print(F(": Min: "));
  printer->print(vout_min, 6);
  printer->print(F(", Peak: "));
  printer->println(vout_peak, 6);
  temp_peak = math_.lin11_to_float(getLin5_11WordVal(tempPeaks[index]->peak));
  temp_min = math_.lin11_to_float(getLin5_11WordVal(tempPeaks[index]->min));
  printer->print(F("Temp"));
  printer->print(index);
  printer->print(F(": Min: "));
  printer->print(temp_min, 6);
  printer->print(F(", Peak: "));
  printer->println(temp_peak, 6);
  iout_peak = math_.lin11_to_float(getLin5_11WordVal(ioutPeaks[index]->peak));
  iout_min = math_.lin11_to_float(getLin5_11WordVal(ioutPeaks[index]->min));
  printer->print(F("Iout"));
  printer->print(index);
  printer->print(F(": Min: "));
  printer->print(iout_min, 6);
  printer->print(F(", Peak: "));
  printer->println(iout_peak, 6);

  printer->print(F("Fast Status"));
  printer->println(index);
  status = getRawByteVal(chanStatuses[index]->status_vout);
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_VOUT%u: 0x%02x\n"), index, status);
  printer->print(buffer);
  status = getRawByteVal(chanStatuses[index]->status_iout);
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_IOUT%u: 0x%02x\n"), index, status);
  printer->print(buffer);
  status = getRawByteVal(chanStatuses[index]->status_mfr_specific);
  snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_MFR%u: 0x%02x\n"), index, status);
  printer->println(buffer);
}

void LT_2975FaultLog::printAllLoops(Print *printer)
{
  voutDatas = new VoutData*[4];
  ioutDatas = new IoutData*[4];
  poutDatas = new PoutData*[4];
  tempDatas = new TempData*[4];

  printer->println(F("Fault Log Loops Follow:"));
  printer->println(F("(most recent data first)"));

  for (int index = 0; index <= 4 && (index < 4 || faultLog2975->isValidData(&faultLog2975->loops[index])); index++)
  {
    printLoop(index, printer);
  }

  delete [] voutDatas;
  delete [] ioutDatas;
  delete [] poutDatas;
  delete [] tempDatas;
}

void LT_2975FaultLog::printLoop(uint8_t index, Print *printer)
{
  printer->println(F("-------"));
  printer->print(F("Loop: "));
  printer->println(index);
  printer->println(F("-------"));

  voutDatas[0] = &faultLog2975->loops[index].vout_data0;
  voutDatas[1] = &faultLog2975->loops[index].vout_data1;
  voutDatas[2] = &faultLog2975->loops[index].vout_data2;
  voutDatas[3] = &faultLog2975->loops[index].vout_data3;

  ioutDatas[0] = &faultLog2975->loops[index].iout_data0;
  ioutDatas[1] = &faultLog2975->loops[index].iout_data1;
  ioutDatas[2] = &faultLog2975->loops[index].iout_data2;
  ioutDatas[3] = &faultLog2975->loops[index].iout_data3;

  poutDatas[0] = &faultLog2975->loops[index].pout_data0;
  poutDatas[1] = &faultLog2975->loops[index].pout_data1;
  poutDatas[2] = &faultLog2975->loops[index].pout_data2;
  poutDatas[3] = &faultLog2975->loops[index].pout_data3;

  tempDatas[0] = &faultLog2975->loops[index].temp_data0;
  tempDatas[1] = &faultLog2975->loops[index].temp_data1;
  tempDatas[2] = &faultLog2975->loops[index].temp_data2;
  tempDatas[3] = &faultLog2975->loops[index].temp_data3;

  uint8_t stat;
  float val;

  printLoopChannel(3, printer);
  printLoopChannel(2, printer);
  printLoopChannel(1, printer);

  if (faultLog2975->isValidData(&faultLog2975->loops[index].vin_data.status_vin, 1))
  {
    printer->println(F("VIN:"));
    stat = getRawByteVal(faultLog2975->loops[index].vin_data.status_vin);
    snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_INPUT: 0x%02x\n"), stat);
    printer->print(buffer);
  }
  if (faultLog2975->isValidData(&faultLog2975->loops[index].vin_data.vin))
  {
    val = math_.lin11_to_float(getLin5_11WordReverseVal(faultLog2975->loops[index].vin_data.vin));
    printer->print(F("  READ_VIN: "));
    printer->print(val, 6);
    printer->println(F(" V"));
  }
  if (faultLog2975->isValidData(&faultLog2975->loops[index].iin_data.read_iin))
  {
    val = math_.lin11_to_float(getLin5_11WordReverseVal(faultLog2975->loops[index].iin_data.read_iin));
    printer->print(F("  READ_IIN: "));
    printer->print(val, 6);
    printer->println(F(" A"));
  }
  if (faultLog2975->isValidData(&faultLog2975->loops[index].pin_data.read_pin))
  {
    val = math_.lin11_to_float(getLin5_11WordReverseVal(faultLog2975->loops[index].pin_data.read_pin));
    printer->print(F("  READ_PIN: "));
    printer->print(val, 6);
    printer->println(F(" W"));
  }

  printLoopChannel(0, printer);

  if (faultLog2975->isValidData(&faultLog2975->loops[index].read_temp2))
  {
    printer->println(F("Chip Temp:"));
    val = math_.lin11_to_float(getLin5_11WordReverseVal(faultLog2975->loops[index].read_temp2));
    printer->print(F("  CHIP TEMP: "));
    printer->print(val, 6);
    printer->println(F(" C"));
  }


}

void LT_2975FaultLog::printLoopChannel(uint8_t index, Print *printer)
{
  uint8_t stat;
  float val;

  if (faultLog2975->isValidData(&poutDatas[index]->read_pout, 2))
  {
    printer->print(F("CHAN"));
    printer->print(index);
    printer->println(F(":"));
    val = math_.lin11_to_float(getLin5_11WordReverseVal(poutDatas[index]->read_pout));
    printer->print(F("  READ POUT: "));
    printer->print(val, 6);
    printer->println(F(" W "));
  }
  if (faultLog2975->isValidData(&ioutDatas[index]->read_iout, 2))
  {
    val = math_.lin11_to_float(getLin5_11WordReverseVal(ioutDatas[index]->read_iout));
    printer->print(F("  READ IOUT: "));
    printer->print(val, 6);
    printer->println(F(" A "));
  }
  if (faultLog2975->isValidData(&ioutDatas[index]->status_iout, 1))
  {
    stat = getRawByteVal(ioutDatas[index]->status_iout);
    snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS IOUT: 0x%02x\n"), stat);
    printer->print(buffer);
  }
  if (faultLog2975->isValidData(&tempDatas[index]->status_temp, 1))
  {
    stat = getRawByteVal(tempDatas[index]->status_temp);
    snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS TEMP: 0x%02x\n"), stat);
    printer->print(buffer);
  }
  if (faultLog2975->isValidData(&tempDatas[index]->read_temp1, 2))
  {
    val = math_.lin11_to_float(getLin5_11WordReverseVal(tempDatas[index]->read_temp1));
    printer->print(F("  READ TEMP: "));
    printer->print(val, 6);
    printer->println(F(" C"));
  }
  if (faultLog2975->isValidData(&voutDatas[index]->status_mfr, 1))
  {
    stat = getRawByteVal(voutDatas[index]->status_mfr);
    snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_MFR: 0x%02x\n"), stat);
    printer->print(buffer);
  }
  if (faultLog2975->isValidData(&voutDatas[index]->status_vout, 1))
  {
    stat = getRawByteVal(voutDatas[index]->status_vout);
    snprintf_P(buffer, FILE_TEXT_LINE_MAX, PSTR("  STATUS_VOUT: 0x%02x\n"), stat);
    printer->print(buffer);
  }
  if (faultLog2975->isValidData(&voutDatas[index]->read_vout))
  {
    val = math_.lin16_to_float(getLin16WordReverseVal(voutDatas[index]->read_vout), 0x13);
    printer->print(F("  READ_VOUT: "));
    printer->print(val, 6);
    printer->println(F(" V"));
  }
}



