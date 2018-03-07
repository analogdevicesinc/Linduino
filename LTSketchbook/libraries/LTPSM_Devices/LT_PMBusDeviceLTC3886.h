/*!
LTC PSM Device

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
    Library Header File for LT_PMBusDeviceLTC3886
*/

#ifndef LT_PMBusDeviceLTC3886_H_
#define LT_PMBusDeviceLTC3886_H_

#include "LT_PMBusDeviceController.h"

class LT_PMBusDeviceLTC3886 : public LT_PMBusDeviceController
{
  public:

    static uint32_t cap_;

    LT_PMBusDeviceLTC3886(LT_PMBus *pmbus, uint8_t address) : LT_PMBusDeviceController(pmbus, address, 2)
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
      LT_PMBusDeviceLTC3886 *device;

      id = pmbus->readMfrSpecialId(address);
      if ((id & 0xFFF0) == 0x4600)
      {
        if (pmbus->getRailAddress(address) != address)
        {
          device = new LT_PMBusDeviceLTC3886(pmbus, address);
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


};

#endif /* LT_PMBusDeviceLTC3886_H_ */
