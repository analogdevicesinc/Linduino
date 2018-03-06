/*!
LTC LT_PMBusSpeedTest

@verbatim

Check bus speed capability.

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

/*! @file
    @ingroup LT_PMBusDevice
    Library Header File for LT_PMBusSpeedTest
*/

#include "LT_PMBusSpeedTest.h"

LT_PMBusSpeedTest::LT_PMBusSpeedTest(LT_PMBus *pmbus):pmbus_(pmbus)
{
}

uint32_t LT_PMBusSpeedTest::test(uint8_t address, uint8_t tries)
{
  bool nok;

  LT_I2CBus *i2cbus = pmbus_->smbus()->i2cbus();
  i2cbus->changeSpeed(400000);
  nok = false;
  for (int i = 0; i < tries; i++)
  {
    pmbus_->setPage(address, 0x00);
    nok |= pmbus_->getPage(address) != 0x00;
    if (nok) break;
    pmbus_->setPage(address, 0xFF);
    nok |= pmbus_->getPage(address) != 0xFF;
    if (nok) break;
  }
  if (!nok) return 400000;

  i2cbus->changeSpeed(100000);
  nok = false;
  for (int i = 0; i < tries; i++)
  {
    pmbus_->setPage(address, 0x00);
    nok |= pmbus_->getPage(address) != 0x00;
    if (nok) break;
    pmbus_->setPage(address, 0xFF);
    nok |= pmbus_->getPage(address) != 0xFF;
    if (nok) break;
  }
  if (!nok) return 100000;

  // Arduino Mega did not run at 10kHz.
  i2cbus->changeSpeed(20000);
  nok = false;
  for (int i = 0; i < tries; i++)
  {
    pmbus_->setPage(address, 0x00);
    nok |= pmbus_->getPage(address) != 0x00;
    if (nok) break;
    pmbus_->setPage(address, 0xFF);
    nok |= pmbus_->getPage(address) != 0xFF;
    if (nok) break;
  }
  if (!nok) return 10000;
  return 0;
}