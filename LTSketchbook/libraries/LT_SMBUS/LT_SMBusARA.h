/*!
LTC PSM ARA Handling

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

//! @ingroup PMBus_SMBus
//! @{
//! @defgroup LT_SMBusAra LT_SMBusAra: Implementation of Device Alert
//! @}

/*! @file
    @ingroup LT_SMBusAra
    Library Header File for LT_SMBusAlert
*/

#ifndef LT_SMBusARA_H_
#define LT_SMBusARA_H_


#include <stdint.h>
#include <LT_PMBus.h>
#include <LT_SMBusARA.h>
#include <LT_PMBusDevice.h>

class LT_SMBusARA
{
  protected:
    LT_SMBus *smbus_;


  public:
    LT_SMBusARA(LT_SMBus *smbus):smbus_(smbus)
    {
    }
    virtual ~LT_SMBusARA() {}

    //! Get the ARA addresses (user must free)
    //! @return addresses
    uint8_t *getAddresses (
    )
    {
      uint8_t *addresses;
      uint16_t count = 0;
      uint8_t address = 0xFF;

      addresses = (uint8_t *) malloc (sizeof(uint8_t));
      do
      {
        address = smbus_->readAlert();
        addresses[count++] = address;
        delay(1);
        addresses = (uint8_t *) realloc(addresses, (count + 1) * sizeof(uint8_t));
      }
      while (address != 0);

      addresses[count] = 0;

      return addresses;
    }

    //! Get all the ARA devices.
    //! @return a list of devices (call must free list, but not devices in list)
    LT_PMBusDevice **getDevices(LT_PMBusDevice **devices //!< A list of known devices                                   //!< The number of devices in the list
                               )
    {
      uint8_t *addresses;
      uint8_t *address;
      LT_PMBusDevice **device;
      LT_PMBusDevice **matchingDevices;
      LT_PMBusDevice **matchingDevice;
      int devCount;

      devCount = 0;
      device = devices;
      while ((*device) != 0)
      {
        device++;
        devCount++;
      }

      addresses = getAddresses();
      matchingDevice = (matchingDevices = (LT_PMBusDevice **) calloc(devCount + 1, sizeof(LT_PMBusDevice *)));

      // Bad algorithm lies here and needs replacing
      address = addresses;
      while (*address != 0)
      {
        device = devices;
        while ((*device) != 0)
        {
          if ((*address) == (*device)->getAddress())
          {
            *matchingDevice = *device;
            matchingDevice++;
          }
          device++;
        }
        address++;
      }
      free (addresses);
      return matchingDevices;
    }

};

#endif /* LT_SMBusDevice_H_ */
