/*!
Linear Technology DC1962C Demonstration Board
LTC3880, LTC2974, LTC2977: Power Management Solution for Application Processors

This sketch has only been tested on a Mega 2560. It is known to fail
on Aarduino Uno and Linduino due to size of RAM.

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

/*! @file
    @ingroup DC1962
*/

#include <Arduino.h>
#include <stdint.h>

#include <LT_SMBusPec.h>
#include <LT_SMBusNoPec.h>
#include <LT_SMBusGroup.h>
#include <LT_PMBus.h>
#include <LT_PMBusMath.h>
#include <LT_PMBusDevice.h>
#include <LT_PMBusRail.h>
#include <LT_PMBusDetect.h>
#include <LT_PMBusDetect.h>
#include <LT_PMBusSpeedTest.h>
#include <LT_PMBusDeviceLTC3880.h>
#include <LT_FaultLog.h>
#include <LT_3880FaultLog.h> // For compilation of library

static bool doprint = false;

#define POLL_DELAY 200

static LT_SMBusNoPec *smbusNoPec = new LT_SMBusNoPec(100000); // Start at slow speed
static LT_SMBusPec *smbusPec = new LT_SMBusPec(100000); // Start at slow speed
static LT_PMBus *pmbusNoPec = new LT_PMBus(smbusNoPec);
static LT_PMBus *pmbusPec = new LT_PMBus(smbusPec);
static LT_PMBusDetect *detector = new LT_PMBusDetect(pmbusNoPec);
static LT_PMBusDevice **devices;
static LT_PMBusDevice **device;
static int index = 0;
static int channel = 0;

//! Initialize Linduino
//! @return void
void setup()
{
  uint32_t speed = 500000;

  Serial.begin(115200);         //! Initialize the serial port to the PC

  if (doprint) Serial.println(F("Detecting Addresses..."));
  detector->detect();
  device = (devices = detector->getDevices());
  while (*device != NULL)
  {
    if (doprint) Serial.print(F("  Type: "));
    if (doprint) Serial.print((*device)->getType());
    if (doprint) Serial.print(F(" Address: "));
    if (doprint) Serial.print((*device)->getAddress(), HEX);

    if (doprint) Serial.print(F(", Speed: "));
    if (doprint) Serial.print((*device)->getMaxSpeed(), DEC);
    if (doprint) Serial.println();

    // Switch the device to Pec
    pmbusNoPec->enablePec((*device)->getAddress());
    (*device)->changePMBus(pmbusPec);

    // Speed test to find fastest speed the bus can operate at.
    if (speed > (*device)->getMaxSpeed())
      speed = (*device)->getMaxSpeed();

    device++;
  }

  if (doprint) Serial.print("Changing bus speed to ");
  if (doprint) Serial.println(speed, DEC);
  // This will change the speed of the I2C bus and enable clock stretching if > 100kHz
  // Only affects Pec becuase the device was switched to Pec.
  device = devices;
  while ((*device) != 0)
  {
    (*device)->setSpeed(speed);
    device++;
  }
  // Change the NoPec version to keep out world consistent.
  pmbusNoPec->smbus()->i2cbus()->changeSpeed(speed);

  Serial.println(F("Index,DeviceId,Channel,MfrSpecialId,Time,DeviceAddress,DevicePage,TelemetryCommand,TelemetryType,ValueLength,Value,FloatValue"));

}

void print_info(LT_PMBusDevice *device, int id)
{
  unsigned long time;

  time = micros();

  Serial.print(index);
  Serial.print(F(","));
  Serial.print(id);
  Serial.print(F(","));
  Serial.print(channel++);
  Serial.print(F(","));

  id = device->readMfrSpecialId();
  Serial.print(F("0x"));
  Serial.print(id, HEX);
  Serial.print(F(","));

  Serial.print(time);
  Serial.print(F(",0x"));
  Serial.print(device->getAddress(), HEX);
  Serial.print(F(",0x"));

}

//! Repeats Linduino loop
//! @return void
void loop()
{
  float f;
  uint16_t val16;
  uint8_t val8;
  uint8_t page;
  int id = 0 ;

  if (index > 100)
    return;

  // Telemetry
  device = devices;
  channel = 0;
  while ((*device) != NULL)
  {
    if ((*device)->hasCapability(HAS_STATUS_WORD))
    {
      for (page = 0; page < (*device)->getNumPages(); page++)
      {
        (*device)->setPage(page);
        val16 = (*device)->smbus()->readWord((*device)->getAddress(), STATUS_WORD);
        print_info(*device, id);
        Serial.print(page, HEX);
        Serial.print(F(",0x"));
        Serial.print(STATUS_WORD, HEX);
        Serial.print(F(",STATUS_WORD,2,0x"));
        Serial.print(val16, HEX);
        Serial.print(F(","));
        Serial.println(val16, DEC);
      }
    }

    if ((*device)->hasCapability(HAS_VIN))
    {
      val16 = (*device)->smbus()->readWord((*device)->getAddress(), READ_VIN);
      print_info(*device, id);
      Serial.print(0x00, HEX);
      Serial.print(F(",0x"));
      Serial.print(READ_VIN, HEX);
      Serial.print(F(",READ_VIN,2,0x"));
      Serial.print(val16, HEX);
      Serial.print(F(","));
      Serial.println(math_.lin11_to_float(val16), DEC);
    }

    if ((*device)->hasCapability(HAS_VOUT))
    {
      for (page = 0; page < (*device)->getNumPages(); page++)
      {
        (*device)->setPage(page);
        val16 = (*device)->smbus()->readWord((*device)->getAddress(), READ_VOUT);
        val8 = (*device)->smbus()->readByte((*device)->getAddress(), VOUT_MODE);
        print_info(*device, id);
        Serial.print(page, HEX);
        Serial.print(F(",0x"));
        Serial.print(READ_VOUT, HEX);
        Serial.print(F(",READ_VOUT,2,0x"));
        Serial.print(val16, HEX);
        Serial.print(F(","));
        Serial.println(math_.lin16_to_float(val16, val8 & 0x1F), DEC);
      }
    }

    if ((*device)->hasCapability(HAS_IIN))
    {
      for (page = 0; page < (*device)->getNumPages(); page++)
      {
        (*device)->setPage(page);
        val16 = (*device)->smbus()->readWord((*device)->getAddress(), READ_IIN);
        print_info(*device, id);
        Serial.print(page, HEX);
        Serial.print(F(",0x"));
        Serial.print(READ_IIN, HEX);
        Serial.print(F(",READ_IIN,2,0x"));
        Serial.print(val16, HEX);
        Serial.print(F(","));
        Serial.println(math_.lin11_to_float(val16), DEC);
      }
    }

    if ((*device)->hasCapability(HAS_IOUT))
    {
      for (page = 0; page < (*device)->getNumPages(); page++)
      {
        (*device)->setPage(page);
        val16 = (*device)->smbus()->readWord((*device)->getAddress(), READ_IOUT);
        print_info(*device, id);
        Serial.print(page, HEX);
        Serial.print(F(",0x"));
        Serial.print(READ_IOUT, HEX);
        Serial.print(F(",READ_IOUT,2,0x"));
        Serial.print(val16, HEX);
        Serial.print(F(","));
        Serial.println(math_.lin11_to_float(val16), DEC);
      }
    }

    if ((*device)->hasCapability(HAS_PIN))
    {
      for (page = 0; page < (*device)->getNumPages(); page++)
      {
        (*device)->setPage(page);
        val16 = (*device)->smbus()->readWord((*device)->getAddress(), READ_PIN);
        print_info(*device, id);
        Serial.print(page, HEX);
        Serial.print(F(",0x"));
        Serial.print(READ_PIN, HEX);
        Serial.print(F(",READ_PIN,2,0x"));
        Serial.print(val16, HEX);
        Serial.print(F(","));
        Serial.println(math_.lin11_to_float(val16), DEC);
      }
    }

    if ((*device)->hasCapability(HAS_POUT))
    {
      for (page = 0; page < (*device)->getNumPages(); page++)
      {
        (*device)->setPage(page);
        val16 = (*device)->smbus()->readWord((*device)->getAddress(), READ_POUT);
        print_info(*device, id);
        Serial.print(page, HEX);
        Serial.print(F(",0x"));
        Serial.print(READ_POUT, HEX);
        Serial.print(F(",READ_POUT,2,0x"));
        Serial.print(val16, HEX);
        Serial.print(F(","));
        Serial.println(math_.lin11_to_float(val16), DEC);
      }
    }
    /*
        if ((*device)->hasCapability(HAS_TEMP))
        {
          for (page = 0; page < (*device)->getNumPages(); page++)
          {
            (*device)->setPage(page);
            val16 = (*device)->smbus()->readWord((*device)->getAddress(), READ_ITEMP);
            Serial.print(page, HEX); Serial.print(F(",0x"));
            Serial.print(READ_ITEMP, HEX);Serial.print(F(",READ_ITEMP,2,0x"));Serial.print(val16, HEX);Serial.print(F(","));Serial.println(math_.lin11_to_float(val16), DEC);
          }
        }
    */
    id++;
    device++;
  }
  index++;
  delay(POLL_DELAY);



}
