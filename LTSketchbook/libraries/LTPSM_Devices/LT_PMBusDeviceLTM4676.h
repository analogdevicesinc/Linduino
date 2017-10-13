/*!
LTC PSM Device

@verbatim

Representation of a device and its capabilities.

@endverbatim

REVISION HISTORY
$Revision: 3845 $
$Date: 2015-08-24 14:11:21 -0600 (Mon, 24 Aug 2015) $

Copyright (c) 2014, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/

/*! @file
    @ingroup LT_PMBusDevice
    Library Header File for LT_PMBusDeviceLTM4676
*/

#ifndef LT_PMBusDeviceLTM4676_H_
#define LT_PMBusDeviceLTM4676_H_

#include "LT_PMBusDeviceController.h"
#include "../LTPSM_PartFaultLogs/LT_3880FaultLog.h"

class LT_PMBusDeviceLTM4676 : public LT_PMBusDeviceController
{
  public:

    static uint32_t cap_;

    LT_PMBusDeviceLTM4676(LT_PMBus *pmbus, uint8_t address) : LT_PMBusDeviceController(pmbus, address, 2)
    {
    }

    uint32_t getCapabilities (
    )
    {
      return cap_;
    }

    //! Is/are these capability(s) supported?
    //! @return true if yes
    bool hasCapability(
      uint32_t capability          //!< List of capabilities
    )
    {
      return (cap_ & capability) == capability;
    }

    static LT_PMBusDevice *detect(LT_PMBus *pmbus, uint8_t address)
    {
      uint16_t id;
      LT_PMBusDeviceLTM4676 *device;

      id = pmbus->readMfrSpecialId(address);
      if (  (id & 0xFFF0) == 0x4400
            || (id & 0xFFF0) == 0x4480
            || (id & 0xFFF0) == 0x47E0)
      {
        if (pmbus->getRailAddress(address) != address)
        {
          device = new LT_PMBusDeviceLTM4676(pmbus, address);
          device->probeSpeed();
          return device;
        }
        else
          return NULL;
      }
      else
        return NULL;
    }

    uint8_t getNumPages(void)
    {
      return 2;
    }

    void enableFaultLog()
    {
      LT_3880FaultLog *faultLog = new LT_3880FaultLog(pmbus_);
      faultLog->enableFaultLog(address_);
      delete faultLog;
    }

    void disableFaultLog()
    {
      LT_3880FaultLog *faultLog = new LT_3880FaultLog(pmbus_);
      faultLog->disableFaultLog(address_);
      delete faultLog;
    }

    bool hasFaultLog()
    {
      LT_3880FaultLog *faultLog = new LT_3880FaultLog(pmbus_);
      if (faultLog->hasFaultLog(address_))
      {
        delete faultLog;
        return true;
      }
      else
      {
        delete faultLog;
        return false;
      }
    }

    char *getFaultLog()
    {
      LT_3880FaultLog *faultLog = new LT_3880FaultLog(pmbus_);
      if (faultLog->hasFaultLog(address_))
      {
        faultLog->read(address_);
//      faultLog->print(&Serial);
        faultLog->dumpBinary(&Serial);
        faultLog->release();
        delete faultLog;
        return NULL;
      }
      else
      {
        delete faultLog;
        return NULL;
      }
    }

    void clearFaultLog()
    {
      LT_3880FaultLog *faultLog = new LT_3880FaultLog(pmbus_);
      if (faultLog->hasFaultLog(address_))
      {
        faultLog->clearFaultLog(address_);
        pmbus_->smbus()->waitForAck(address_, 0x00);
        pmbus_->waitForNotBusy(address_);
        delete faultLog;
      }
      else
      {
        delete faultLog;
      }
    }
};

#endif /* LT_PMBusDeviceLTM4676_H_ */
