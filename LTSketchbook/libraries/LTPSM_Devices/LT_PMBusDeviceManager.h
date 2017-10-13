/*!
LTC PSM Manager Device

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
    Library Header File for LT_PMBusDeviceController
*/

#ifndef LT_PMBusDeviceManager_H_
#define LT_PMBusDeviceManager_H_

#include "LT_PMBusDevice.h"
#include "LT_PMBusRail.h"

class LT_PMBusDeviceManager : public LT_PMBusDevice
{
  private:
    uint8_t no_pages_;

  public:

    LT_PMBusDeviceManager(LT_PMBus *pmbus, uint8_t address, uint8_t no_pages) : LT_PMBusDevice(pmbus, address), no_pages_(no_pages) {}

    void setSpeed(uint32_t speed)
    {
      pmbus_->smbus()->i2cbus()->changeSpeed(speed);
    }

    LT_PMBusRail **getRails()
    {
      LT_PMBusRail **rails = NULL;
      tRailDef **railDef = NULL;
      uint8_t i;
      rails = (LT_PMBusRail **) malloc((no_pages_ + 1) * sizeof(LT_PMBusRail *));

      for (i = 0; i < no_pages_; i++)
      {
        railDef = (tRailDef **)malloc((no_pages_ + 1) * sizeof(tRailDef *));
        railDef[0] = new tRailDef;
        railDef[0]->address = address_;
        railDef[0]->pages = (uint8_t *) malloc(sizeof(uint8_t));
        railDef[0]->pages[0] = i;
        railDef[0]->noOfPages = 1;
        railDef[0]->controller = false;
        railDef[0]->multiphase = false;
        railDef[0]->capabilities = getCapabilities();
        railDef[1] = NULL;
        rails[i] = new LT_PMBusRail(pmbus_, railDef[0]->address, railDef);
      }

      rails[no_pages_] = NULL;
      return rails;
    }

};

#endif /* LT_PMBusDeviceManager_H_ */
