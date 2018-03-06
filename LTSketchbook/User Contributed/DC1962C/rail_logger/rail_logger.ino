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
#include <Linduino.h>
#include <UserInterface.h>
#include <stdint.h>

#include <LT_SMBusPec.h>
#include <LT_PMBusMath.h>
#include <LT_SMBus.h>
#include <LT_PMBusRail.h>
#include <LT_I2CBus.h>
#include <LT_SMBusGroup.h>
#include <LT_PMBusDetect.h>
#include <LT_PMBus.h>
#include <LT_FaultLog.h>
#include <LT_PMBusDevice.h>
#include <LT_SMBusNoPec.h>
#include <LT_SMBusBase.h>
#include <LT_PMBusDetect.h>
#include <LT_PMBusSpeedTest.h>
#include <LT_PMBusDeviceLTC3880.h>
#include <LT_3880FaultLog.h> // For compilation of library

static bool doprint = true;
static bool dodisplay = true;

#define POLL_DELAY 2000

static LT_SMBusNoPec *smbusNoPec = new LT_SMBusNoPec(100000); // Start at slow speed
static LT_SMBusPec *smbusPec = new LT_SMBusPec(100000); // Start at slow speed
static LT_PMBus *pmbusNoPec = new LT_PMBus(smbusNoPec);
static LT_PMBus *pmbusPec = new LT_PMBus(smbusPec);
static LT_PMBusDetect *detector = new LT_PMBusDetect(pmbusNoPec);
static LT_PMBusDevice **devices;
static LT_PMBusDevice **device;
static LT_PMBusRail **rails;
static LT_PMBusRail **rail;
static int id = 0;

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
    if (doprint) Serial.print(F("  Device: "));
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

  rail = (rails = detector->getRails());
  while (*rail != NULL)
  {
    if (doprint) Serial.print(F("  Rail: "));
    if (doprint) Serial.print(F(" Address: "));
    if (doprint) Serial.print((*rail)->getAddress(), HEX);
    if (doprint) Serial.print(F(" Pages: "));
    if (doprint) Serial.print((*rail)->getNoPages(), DEC);
    if (doprint) Serial.println();
    (*rail)->changePMBus(pmbusPec);
    rail++;
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

  Serial.println(F("Id,Time(ms),Address,NoPages,Multiphase,Type,Value"));

}

//! Print basic info
//! @return void
void print_info(LT_PMBusRail *rail)
{
  unsigned long time;

  time = millis();

  Serial.print(id++);
  Serial.print(F(","));
  Serial.print(time);
  Serial.print(F(",0x"));
  Serial.print(rail->getAddress(), HEX);
  Serial.print(F(",0x"));
  Serial.print(rail->readMfrSpecialId(), HEX);
  Serial.print(F(","));
  Serial.print(rail->getNoPages());
  Serial.print(F(","));
  if (rail->isMultiphase()) Serial.print(F("1"));
  else Serial.print("0");
  Serial.print(F(","));
}

//! Repeats Linduino loop
//! @return void
void loop()
{
  float f;
  uint16_t status;

  // Telemetry
  rail = rails;
  while ((*rail) != NULL)
  {
    if ((*rail)->hasCapability(HAS_STATUS_WORD))
    {
      status = (*rail)->readStatusWord();
      print_info(*rail);
      Serial.print(F("SWORD,0x"));
      Serial.println(status, HEX);
    }

    if ((*rail)->hasCapability(HAS_VIN))
    {
      f = (*rail)->readVin(true);
      print_info(*rail);
      Serial.print(F("VIN,"));
      Serial.println(f, DEC);
    }

    if ((*rail)->hasCapability(HAS_VOUT))
    {
      f = (*rail)->readVout(true);
      print_info(*rail);
      Serial.print(F("VOUT,"));
      Serial.println(f, DEC);
    }

    if ((*rail)->hasCapability(HAS_IIN))
    {
      f = (*rail)->readIin(true);
      print_info(*rail);
      Serial.print(F("IIN,"));
      Serial.println(f, DEC);
    }

    if ((*rail)->hasCapability(HAS_IOUT))
    {
      f = (*rail)->readIout(true);
      print_info(*rail);
      Serial.print(F("Iout,"));
      Serial.println(f, DEC);
    }

    if ((*rail)->hasCapability(HAS_PIN))
    {
      f = (*rail)->readPin(true);
      print_info(*rail);
      Serial.print(F("PIN,"));
      Serial.println(f, DEC);
    }

    if ((*rail)->hasCapability(HAS_POUT))
    {
      f = (*rail)->readPout(true);
      print_info(*rail);
      Serial.print(F("POUT,"));
      Serial.println(f, DEC);
    }

    if ((*rail)->hasCapability(HAS_TEMP))
    {
      f = (*rail)->readInternalTemperature(true);
      print_info(*rail);
      Serial.print(F("TEMP,"));
      Serial.println(f, DEC);
    }

    f = (*rail)->readEfficiency(true);
    if (f > 20.0) // Weeds out no load rails or unmeasuremable rails.
    {
      print_info(*rail);
      Serial.print(F("EFF,"));
      Serial.println(f, DEC);
    }

    if ((*rail)->isMultiphase())
    {
      f = (*rail)->readPhaseBalance(true);
      print_info(*rail);
      Serial.print(F("IBAL,"));
      Serial.println(f, DEC);
    }

    rail++;
  }

  delay(POLL_DELAY);



}
