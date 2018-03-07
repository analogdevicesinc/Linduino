/*!
LTC PSM Controller Device

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

#ifndef LT_PMBusDeviceController_H_
#define LT_PMBusDeviceController_H_

#include "LT_PMBusDevice.h"
#include "LT_PMBusRail.h"

class LT_PMBusDeviceController : public LT_PMBusDevice
{
  private:
    uint8_t no_pages_;

  public:

    LT_PMBusDeviceController(LT_PMBus *pmbus, uint8_t address, uint8_t no_pages) : LT_PMBusDevice(pmbus, address), no_pages_(no_pages) {}

    LT_PMBusRail **getRails()
    {
      LT_PMBusRail **rails = NULL;
      tRailDef **railDef = NULL;
      void *m;
      uint8_t rail_address, last_rail_address;
      uint8_t no_rails = 0;
//    Serial.println("controller get rails ");

      no_rails = 0;
      pmbus_->setPage(address_, 0);
      last_rail_address = (rail_address = pmbus_->getRailAddress(address_));
      if (rail_address == 0x80)
      {
        railDef = (tRailDef **)malloc(2*sizeof(tRailDef *));
        railDef[1] = NULL;
        railDef[0] = new tRailDef;
        railDef[0]->address = address_;
        railDef[0]->pages = (uint8_t *) malloc(1);
        railDef[0]->pages[0] = 0;
        railDef[0]->noOfPages = 1;
        railDef[0]->controller = true;
        railDef[0]->multiphase = false;
        railDef[0]->capabilities = getCapabilities();
        last_rail_address = address_;
        no_rails++;
      }
      else
      {
        m = malloc(2*sizeof(tRailDef *));
        if (m == NULL)
          free(m);
        else
        {
          railDef = (tRailDef **)m;
          railDef[1] = NULL;
          railDef[0] = new tRailDef;
          railDef[0]->address = address_;
          railDef[0]->pages = (uint8_t *) malloc(1);
          railDef[0]->pages[0] = 0;
          railDef[0]->noOfPages = 1;
          railDef[0]->controller = true;
          railDef[0]->multiphase = true;
          railDef[0]->capabilities = getCapabilities();
        }
        no_rails++;
      }

      if (no_pages_ > 0) // Only handes 1/2 channel controllers
      {
        pmbus_->setPage(address_, 1);
        rail_address = pmbus_->getRailAddress(address_);
        if (rail_address == 0x80)
        {
          m = realloc(railDef, 4*sizeof(tRailDef *));
          if (m == NULL)
            free(m);
          else
          {
            railDef = (tRailDef **)m;
            railDef[3] = NULL;
            railDef[2] = new tRailDef;
            railDef[2]->address = address_;
            railDef[2]->pages = (uint8_t *) malloc(1);
            railDef[2]->pages[0] = 1;
            railDef[2]->noOfPages = 1;
            railDef[2]->controller = true;
            railDef[2]->multiphase = false;
            railDef[2]->capabilities = getCapabilities();
            rail_address = address_;
          }
          no_rails++;
        }
        else
        {
          if (last_rail_address == rail_address) // Both pages in same rail.
          {
            railDef[0]->pages = (uint8_t *) realloc(railDef[0]->pages, 2);
            railDef[0]->pages[1] = 1;
            railDef[0]->noOfPages = 2;
          }
          else
          {
            m = realloc(railDef,4 *sizeof(tRailDef *));
            if (m == NULL)
              free(m);
            else
            {
              railDef = (tRailDef **)m;
              railDef[3] = NULL;
              railDef[2] = new tRailDef;
              railDef[2]->address = address_;
              railDef[2]->pages = (uint8_t *) malloc(1);
              railDef[2]->pages[0] = 1;
              railDef[2]->noOfPages = 1;
              railDef[2]->controller = true;
              railDef[2]->multiphase = true;
              railDef[2]->capabilities = getCapabilities();
            }
            no_rails++;
          }
        }
      }

      if (no_rails > 0)
      {
        rails = (LT_PMBusRail **) malloc((no_rails + 1) * sizeof(LT_PMBusRail *));
        rails[0] = new LT_PMBusRail(pmbus_, last_rail_address, railDef);
        if (no_rails > 1)
          rails[1] = new LT_PMBusRail(pmbus_, rail_address, railDef + 2);
      }

      rails[no_rails] = NULL;
      return rails;
    }

};

#endif /* LT_PMBusDeviceController_H_ */
