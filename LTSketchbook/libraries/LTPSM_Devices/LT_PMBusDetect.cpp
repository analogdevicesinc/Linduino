/*!
LTC LT_PMBusDetect

@verbatim

Support for Detection

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
    Library Header File for LT_PMBusDetect
*/

#include <string.h>

#include "../LT_PMBUS/LT_PMBusDetect.h"
#include "LT_PMBusDeviceLTC3880.h"
#include "LT_PMBusDeviceLTC3882.h"
#include "LT_PMBusDeviceLTC3883.h"
#include "LT_PMBusDeviceLTC3886.h"
#include "LT_PMBusDeviceLTC3887.h"
#include "LT_PMBusDeviceLTM4675.h"
#include "LT_PMBusDeviceLTM4676.h"
#include "LT_PMBusDeviceLTM4677.h"
#include "LT_PMBusDeviceLTC2974.h"
#include "LT_PMBusDeviceLTC2975.h"
#include "LT_PMBusDeviceLTC2977.h"
#include "LT_PMBusDeviceLTC2978.h"
#include "LT_PMBusDeviceLTC2980.h"
#include "LT_PMBusDeviceLTM2987.h"

LT_PMBusDetect::LT_PMBusDetect(LT_PMBus *pmbus):pmbus_(pmbus)
{
}

LT_PMBusDevice **LT_PMBusDetect::getDevices(
)
{
  return devices_;
}

LT_PMBusRail **LT_PMBusDetect::getRails(
)
{
  return rails_;
}


void LT_PMBusDetect::detect ()
{
  devices_ = NULL;
  rails_ = NULL;
  deviceCnt_ = 0;
  railCnt_ = 0;

  uint8_t *addresses;
  LT_PMBusDevice *device;
  unsigned int i, j;

  if (deviceCnt_ > 0)
  {
    while (deviceCnt_ > 0)
      delete (*(devices_ + (--deviceCnt_)));
    free(devices_);
  }

  if (railCnt_ > 0)
  {
    while (railCnt_ > 0)
      delete (*(rails_ + (--railCnt_)));
    free(rails_);
  }

  addresses = pmbus_->smbus()->probeUnique(0x00);

  // May be more than required. Can add code to trim based on deviceCnt_
  // +1 and calloc so there is a terminating NULL
  devices_ = (LT_PMBusDevice **) calloc(strlen((char *)addresses) + 1, sizeof(LT_PMBusDevice *));

  // Check each device type one by one. Trading generality and composability
  // for performance. For better performance, cache IDs or write decoder here
  // and then call detect on the correct device first time.
  for (i = 0; i < strlen((char *)addresses); i++)
  {
    if ((device = LT_PMBusDeviceLTC3880::detect(pmbus_, addresses[i])) != NULL)
      devices_[deviceCnt_++] = device;
    else if ((device = LT_PMBusDeviceLTC3882::detect(pmbus_, addresses[i])) != NULL)
      devices_[deviceCnt_++] = device;
    else if ((device = LT_PMBusDeviceLTC3883::detect(pmbus_, addresses[i])) != NULL)
      devices_[deviceCnt_++] = device;
    else if ((device = LT_PMBusDeviceLTC3886::detect(pmbus_, addresses[i])) != NULL)
      devices_[deviceCnt_++] = device;
    else if ((device = LT_PMBusDeviceLTC3887::detect(pmbus_, addresses[i])) != NULL)
      devices_[deviceCnt_++] = device;
    else if ((device = LT_PMBusDeviceLTM4675::detect(pmbus_, addresses[i])) != NULL)
      devices_[deviceCnt_++] = device;
    else if ((device = LT_PMBusDeviceLTM4676::detect(pmbus_, addresses[i])) != NULL)
      devices_[deviceCnt_++] = device;
    else if ((device = LT_PMBusDeviceLTM4677::detect(pmbus_, addresses[i])) != NULL)
      devices_[deviceCnt_++] = device;
    else if ((device = LT_PMBusDeviceLTC2974::detect(pmbus_, addresses[i])) != NULL)
      devices_[deviceCnt_++] = device;
    else if ((device = LT_PMBusDeviceLTC2975::detect(pmbus_, addresses[i])) != NULL)
      devices_[deviceCnt_++] = device;
    else if ((device = LT_PMBusDeviceLTC2977::detect(pmbus_, addresses[i])) != NULL)
      devices_[deviceCnt_++] = device;
    else if ((device = LT_PMBusDeviceLTC2978::detect(pmbus_, addresses[i])) != NULL)
      devices_[deviceCnt_++] = device;
    else if ((device = LT_PMBusDeviceLTC2980::detect(pmbus_, addresses[i])) != NULL)
      devices_[deviceCnt_++] = device;
    else if ((device = LT_PMBusDeviceLTM2987::detect(pmbus_, addresses[i])) != NULL)
      devices_[deviceCnt_++] = device;

  }

  // Get all the rails, while merging duplicates.
  for (i = 0; i < deviceCnt_; i++)
  {
    LT_PMBusRail **new_rail;
    void *m;
    bool merged;

    new_rail = devices_[i]->getRails();

    // Loop if device has rail support, and if there are rails to process.
    while (new_rail != NULL && *new_rail != NULL)
    {
      // See if we are already in the list.
      merged = false;
      for (j = 0; j < railCnt_; j++)
      {
        if (rails_[j]->isMultiphase() && rails_[j]->getAddress() == (*new_rail)->getAddress())
        {
          rails_[j]->merge(*new_rail);
          delete (*new_rail);
          merged = true;
          break;
        }
      }
      if (!merged)
      {
        if (rails_ == NULL)
          rails_ = (LT_PMBusRail **) malloc(sizeof(LT_PMBusRail *));
        else
        {
          m = realloc(rails_, (railCnt_ + 1) * sizeof(LT_PMBusRail *));
          if (m == NULL)
            free(m);
          else
            rails_ = (LT_PMBusRail **) m;
        }
        if (rails_ != NULL)
          rails_[railCnt_++] = *new_rail;
      }

      new_rail++;
    }
  }

  rails_ = (LT_PMBusRail **) realloc(rails_, (railCnt_ + 1) * sizeof(LT_PMBusRail *));
  rails_[railCnt_] = NULL;
}