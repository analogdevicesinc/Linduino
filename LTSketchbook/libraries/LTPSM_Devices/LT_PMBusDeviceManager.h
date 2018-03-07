/*!
LTC PSM Manager Device

@verbatim

Representation of a device and its capabilities.

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
