/*!
LTC PMBusRail Support

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
//! @defgroup LT_PMBusRail LT_PMBusRail: PMBus Rail commands
//! @}

/*! @file
    @ingroup LT_PMBusRail
    Library File for LT_PMBusRail
*/

#include <Arduino.h>
#include <stdint.h>
#include <math.h>
#include "Linduino.h"
#include "LT_PMBusRail.h"
#include "LT_PMBusDevice.h"

LT_PMBusRail::LT_PMBusRail (LT_PMBus *pmbus, uint8_t railAddress, tRailDef **railDef)
{
  pmbus_ = pmbus;
  railAddress_ = railAddress;
  railDef_ = railDef;
}

void LT_PMBusRail::merge(LT_PMBusRail *rail)
{
  uint8_t i;

  (*railDef_)->pages = (uint8_t *)realloc((*railDef_)->pages, (*railDef_)->noOfPages + (*(rail->railDef_))->noOfPages);
  for (i = (*railDef_)->noOfPages; i < (*railDef_)->noOfPages + (*(rail->railDef_))->noOfPages; i++)
    (*railDef_)->pages[i] = (*(rail->railDef_))->pages[i - (*railDef_)->noOfPages];
  (*railDef_)->noOfPages += (*(rail->railDef_))->noOfPages;
}

LT_PMBusRail::~LT_PMBusRail()
{
  tRailDef **rail = railDef_;
  while (*rail != NULL)
  {
    free ((*rail)->pages);
    delete *rail;
    rail++;
  }
  free (railDef_);
}

void LT_PMBusRail::changePMBus(LT_PMBus *pmbus)
{
  pmbus_ = pmbus;
}

uint8_t LT_PMBusRail::getAddress()
{
  return railAddress_;
}

bool LT_PMBusRail::isController ()
{
  tRailDef **rail = railDef_;
  return (*rail)->controller;
}

uint8_t LT_PMBusRail::getNoPages()
{
  tRailDef **rail = railDef_;
  uint8_t noPages = 0;

  while (*rail != NULL)
  {
    noPages += (*rail)->noOfPages;
    rail++;
  }
  return noPages;
}

bool LT_PMBusRail::isMultiphase()
{
  return (*railDef_)->multiphase;
}

uint32_t LT_PMBusRail::getCapabilities()
{
  return (*railDef_)->capabilities;
}

uint32_t LT_PMBusRail::hasCapability(uint32_t capability)
{
  return ((*railDef_)->capabilities & capability) == capability;
}

/*
 * Set the output voltage of a polyphase rail
 *
 * voltage: the target voltage
 */
void LT_PMBusRail::setVout(float voltage)
{
  // Add phases will move via the rail address.
  pmbus_->setVout(railAddress_, voltage);
}

/*
 * Read the input voltage of a polyphase rail
 *
 * polling: poll if true
 * return: the input voltage
 */
float LT_PMBusRail::readVin(bool polling)
{
  // This assumes that the VIN of all physical devices share
  // the same VIN.
  return pmbus_->readVin((*railDef_)->address, polling);
}

/*
 * Read the output voltage of a polyphase rail
 *
 * polling: poll if true
 * return: the output voltage
 */
float LT_PMBusRail::readVout(bool polling)
{
  // All VOUTs are connected, so any physical address and
  // page will do.
  pmbus_->setPage((*railDef_)->address, (*railDef_)->pages[0]);
  return pmbus_->readVout((*railDef_)->address, polling);
}

/*
 * Read the input current of a polyphase rail
 *
 * polling: poll if true
 * return: the input current
 */
float LT_PMBusRail::readIin(bool polling)
{
  float current = 0.0;
  tRailDef **rail = railDef_;

  // Add up inputs from all physical devices. This
  // may include rail/phases that are not part of the rail.
  while (*rail != NULL)
  {
    for (int j = 0; j < (*rail)->noOfPages; j++)
    {
      pmbus_->setPage((*rail)->address, (*rail)->pages[j]);
      current += pmbus_->readIin((*rail)->address, polling);
    }
    rail++;
  }

  return current;
}

/*
 * Read the output current of a polyphase rail
 *
 * polling: poll if true
 * return: the output current
 */
float LT_PMBusRail::readIout(bool polling)
{
  float current = 0.0;
  tRailDef **rail = railDef_;

  // Add up all phases. There will not be any unwanted phases.
  while (*rail != NULL)
  {
    for (int j = 0; j < (*rail)->noOfPages; j++)
    {
      pmbus_->setPage((*rail)->address, (*rail)->pages[j]);
      current += pmbus_->readIout((*rail)->address, polling);
    }
    rail++;
  }

  return current;
}

/*
 * Read the input power of a polyphase rail
 *
 * polling: poll if true
 * return: the output current
 */
float LT_PMBusRail::readPin(bool polling)
{
  float power = 0.0;
  tRailDef **rail = railDef_;

  // Add up inputs from all physical devices. This
  // may include rail/phases that are not part of the rail.
  while (*rail != NULL)
  {
    power += pmbus_->readPin((*rail)->address, polling);
    rail++;
  }

  return power;
}

/*
 * Read the output power of a polyphase rail
 *
 * polling: poll if true
 * return: the output power
 */
float LT_PMBusRail::readPout(bool polling)
{
  float power = 0.0;
  tRailDef **rail = railDef_;

  // Add up all phases. There will not be any unwanted phases.
  while (*rail != NULL)
  {
    for (int j = 0; j < (*rail)->noOfPages; j++)
    {
      pmbus_->setPage((*rail)->address, (*rail)->pages[j]);
      power += pmbus_->readPout((*rail)->address, polling);
    }
    rail++;
  }

  return power;
}

/*
 * Read the external temperature of a polyphase rail
 *
 * polling: poll if true
 * return: the temperature
 */
float LT_PMBusRail::readExternalTemperature(bool polling)
{
  float temp = 0.0;
  float pages = 0;
  tRailDef **rail = railDef_;

  // Add up all phases. There will not be any unwanted phases.
  while (*rail != NULL)
  {
    pages += (float) ((*rail)->noOfPages);
    for (int j = 0; j < (*rail)->noOfPages; j++)
    {
      pmbus_->setPage((*rail)->address, (*rail)->pages[j]);
      temp += pmbus_->readExternalTemperature((*rail)->address, polling);
    }
    rail++;
  }

  return temp/pages;
}

/*
 * Read the internal temperature of a polyphase rail
 *
 * polling: poll if true
 * return: the temperature
 */
float LT_PMBusRail::readInternalTemperature(bool polling)
{
  float temp = 0.0;
  float pages = 0;
  tRailDef **rail = railDef_;

  // Add up all phases. There will not be any unwanted phases.
  while (*rail != NULL)
  {
    pages += (float) ((*rail)->noOfPages);
    for (int j = 0; j < (*rail)->noOfPages; j++)
    {
      pmbus_->setPage((*rail)->address, (*rail)->pages[j]);
      PsmDeviceType t = pmbus_->deviceType((*rail)->address);
      if (t == LTC2977 || t == LTC2978)
        temp += pmbus_->readExternalTemperature((*rail)->address, polling); // Really internal.
      else
        temp += pmbus_->readInternalTemperature((*rail)->address, polling);
    }
    rail++;
  }

  return temp/pages; // Account for multiple devices
}


/*
 * Read the efficiency of a polyphase rail
 *
 * polling: poll if true
 * return: the efficiency
 */
float LT_PMBusRail::readEfficiency(bool polling)
{
  float val1, val2 = 0.0;
  float efficiency = 0.0;

//  Serial.print("eff "); Serial.println(getCapabilities(), HEX);
  if (hasCapability(HAS_POUT | HAS_PIN))
  {
//    Serial.println("1");
    val1 = readPin(polling);
    val2 += readPout(polling);
    efficiency = 100.0 * val2/val1;
  }
  else if (hasCapability(HAS_POUT | HAS_IIN | HAS_VIN))
  {
//    Serial.println("2");
    val1 = readVin(polling);
    val1 *= readIin(polling);
    val2 = readPout(polling);
    efficiency = 100.0 * val2/val1;
  }
  else if (hasCapability(HAS_VOUT | HAS_IOUT | HAS_PIN))
  {
//    Serial.println("3");
    val1 = readPin(polling);
    val2 = readVout(polling);
    val2 *= readIout(polling);
    efficiency = 100.0 * val2/val1;
  }
  else if (hasCapability(HAS_VOUT | HAS_IOUT | HAS_VIN | HAS_IIN))
  {
//    Serial.println("4");
    val1 = readVin(polling);
    val1 *= readIin(polling);
    val2 = readVout(polling);
    val2 *= readIout(polling);
    efficiency = 100.0 * val2/val1;
  }
  return efficiency;
}

/*
 * Read the average duty cycle of a polyphase rail
 *
 * polling: poll if true
 * return: the duty cycle
 */
float LT_PMBusRail::readDutyCycle(bool polling)
{
  float total = 0.0;
  tRailDef **rail = railDef_;

  if (hasCapability(HAS_DC))
  {

    while (*rail != NULL)
    {
      for (int j = 0; j < (*rail)->noOfPages; j++)
      {
        pmbus_->setPage((*rail)->address, (*rail)->pages[j]);
        total += pmbus_->readDutyCycle((*rail)->address, polling);
//        Serial.println(dc,DEC);
      }
      rail++;
    }

    return total/(*rail)->noOfPages;
  }
  else
    return 0.0;
}

float LT_PMBusRail::readPhaseBalance(bool polling)
{
  float current = 0.0;
  float min = 10000.0;
  float max = -10000.0;
  float total = 0.0;
  tRailDef **rail = railDef_;

  if (hasCapability(HAS_IOUT))
  {

    while (*rail != NULL)
    {
      for (int j = 0; j < (*rail)->noOfPages; j++)
      {
        pmbus_->setPage((*rail)->address, (*rail)->pages[j]);
        total += (current = pmbus_->readIout((*rail)->address, polling));
//        Serial.println(current,DEC);
        if (current > max) max = current;
        if (current < min) min = current;
      }
      rail++;
    }

    return 100.0 * (max - min)/total;
  }
  else
    return 0.0;
}

float LT_PMBusRail::readTransient(bool polling)
{
  float max = 0.0;
  float vout;
  float vout_uv;
  uint8_t vout_response;
  uint8_t status;
  tRailDef **rail = railDef_;
  float v;
  bool is_controller;
  uint16_t pads;
  PsmDeviceType t;

  while (*rail != NULL)
  {
    is_controller = isController();
    max = 0.0;
    for (int j = 0; j < (*rail)->noOfPages; j++)
    {
      // Skip LTC3882 Slave phases
      t = pmbus_->deviceType((*rail)->address);
      if (is_controller && (t == LTC3882 || t == LTC3882_1))
      {
//        Serial.println(j, DEC);
        pads = pmbus_->smbus()->readWord((*rail)->address, MFR_PADS);
//        Serial.println(pads, HEX);
        if (pads & (1 << (14 + j)))
          continue;
      }

      if (polling) pmbus_->smbus()->waitForAck((*rail)->address, 0x00);
      if (polling) pmbus_->waitForNotBusy((*rail)->address);
      pmbus_->setPage((*rail)->address, (*rail)->pages[j]);
      if (polling) pmbus_->smbus()->waitForAck((*rail)->address, 0x00);
      if (polling) pmbus_->waitForNotBusy((*rail)->address);
      vout = pmbus_->getVout((*rail)->address, polling);
      if (polling) pmbus_->smbus()->waitForAck((*rail)->address, 0x00);
      if (polling) pmbus_->waitForNotBusy((*rail)->address);
      vout_uv = pmbus_->getVoutUv((*rail)->address, polling);
      if (polling) pmbus_->smbus()->waitForAck((*rail)->address, 0x00);
      if (polling) pmbus_->waitForNotBusy((*rail)->address);
      vout_response = pmbus_->smbus()->readByte((*rail)->address, VOUT_UV_FAULT_RESPONSE);
      if (polling) pmbus_->smbus()->waitForAck((*rail)->address, 0x00);
      if (polling) pmbus_->waitForNotBusy((*rail)->address);
      pmbus_->smbus()->writeByte((*rail)->address, VOUT_UV_FAULT_RESPONSE, 0);
      if (polling) pmbus_->smbus()->waitForAck((*rail)->address, 0x00);
      if (polling) pmbus_->waitForNotBusy((*rail)->address);

      status = pmbus_->readVoutStatusByte((*rail)->address);
      if (polling) pmbus_->smbus()->waitForAck((*rail)->address, 0x00);
      if (polling) pmbus_->waitForNotBusy((*rail)->address);
      if (is_controller)
        pmbus_->smbus()->writeByte((*rail)->address, STATUS_VOUT, status | (1 << 4));
      else
        pmbus_->clearFaults((*rail)->address);
      if (polling) pmbus_->smbus()->waitForAck((*rail)->address, 0x00);
      if (polling) pmbus_->waitForNotBusy((*rail)->address);

//      Serial.print("N "); Serial.println(vout, DEC);

      // Generating a fault, even if ignored, can make things busy, so poll.
      for (v = 0.95 * vout; v < 1.05 * vout; v = v + 0.001)
      {
        pmbus_->setVoutUvFaultLimit((*rail)->address, v);
        if (polling) pmbus_->smbus()->waitForAck((*rail)->address, 0x00);
        if (polling) pmbus_->waitForNotBusy((*rail)->address);
        status = pmbus_->readVoutStatusByte((*rail)->address);
        if (polling) pmbus_->smbus()->waitForAck((*rail)->address, 0x00);
        if (polling) pmbus_->waitForNotBusy((*rail)->address);
        if (status & (1 << 4))
        {
//          Serial.print("V "); Serial.print(v,DEC); Serial.print(" S "); Serial.println(status, HEX);
          break;
        }
        delay(50);
      }

      pmbus_->setVoutUvFaultLimit((*rail)->address, vout_uv);
      if (polling) pmbus_->smbus()->waitForAck((*rail)->address, 0x00);
      if (polling) pmbus_->waitForNotBusy((*rail)->address);
      pmbus_->smbus()->writeByte((*rail)->address, VOUT_UV_FAULT_RESPONSE, vout_response);
      if (polling) pmbus_->smbus()->waitForAck((*rail)->address, 0x00);
      if (polling) pmbus_->waitForNotBusy((*rail)->address);

      if (is_controller)
        pmbus_->smbus()->writeByte((*rail)->address, STATUS_VOUT, status | (1 << 4));
      else
        pmbus_->clearFaults((*rail)->address);

//      Serial.println(vout, DEC);
//      Serial.println(v, DEC);
//      Serial.println();

      max = max(max, vout-v);
//      Serial.print("transient "); Serial.println(vout-v, DEC);
//      Serial.print("uv "); Serial.println(vout_uv, DEC);
//      Serial.print("resp "); Serial.println(vout_response, HEX);

    }
    rail++;
  }

  return max;
}

/*
 * Read the status word of a polyphase rail
 *
 * return: status word
 */
uint16_t LT_PMBusRail::readStatusWord()
{
  uint16_t sw = 0;
  tRailDef **rail = railDef_;

  // Combine all words. Assumes 1 = notification, so that anything
  // with a 1 is interesting.
  while (*rail != NULL)
  {
    for (int j = 0; j < (*rail)->noOfPages; j++)
    {
      pmbus_->setPage((*rail)->address, (*rail)->pages[j]);
      sw |= pmbus_->readStatusWord((*rail)->address);
    }
    rail++;
  }

  return sw;
}

/*
 * Read the special id of a polyphase rail
 *
 * return: special id
 */
uint16_t LT_PMBusRail::readMfrSpecialId()
{
  uint16_t id = 0;
  tRailDef **rail = railDef_;

  while (*rail != NULL)
  {
    for (int j = 0; j < (*rail)->noOfPages; j++)
    {
      pmbus_->setPage((*rail)->address, (*rail)->pages[j]);
      id = pmbus_->readMfrSpecialId((*rail)->address);
      // Exist at first opportunity.
      break;
    }
    rail++;
  }

  return id;
}

/*
 * Clear faults of polyphase rail
 */
void LT_PMBusRail::clearFaults()
{
  tRailDef **rail = railDef_;

  pmbus_->startGroupProtocol();

  while (*rail != NULL)
  {
    pmbus_->clearAllFaults((*rail)->address);
    rail++;
  }

  pmbus_->executeGroupProtocol();
}

/*
 * Sequence on a polyphase rail
 */
void LT_PMBusRail::sequenceOn()
{
  pmbus_->sequenceOn(&railAddress_, 1);
}

/*
 * Turn off a polyphase rail
 */
void LT_PMBusRail::immediateOff()
{
  pmbus_->immediateOff(&railAddress_, 1);
}

/*
 * Sequence off a polyphase rail
 *
 */
void LT_PMBusRail::sequenceOff()
{
  pmbus_->sequenceOff(&railAddress_, 1);
}

/*
 * Margin high polyphase rail
 */
void LT_PMBusRail::marginHigh()
{
  pmbus_->marginHigh(railAddress_);
}

/*
 * Margin low polyphase rail
 *
 */
void LT_PMBusRail::marginLow()
{
  pmbus_->marginLow(railAddress_);
}

/*
 * Margin off polyphase rail
 *
 */
void LT_PMBusRail::marginOff()
{
  pmbus_->marginOff(railAddress_);
}
