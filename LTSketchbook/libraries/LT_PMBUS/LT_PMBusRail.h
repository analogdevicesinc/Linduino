/*!
LTC PMBus Support

@verbatim

This PMBus code does not cover the entire PMBus command set. The intention
is to cover the common commands. It was written for LTC PMBus devices,
and may not perform properly with other PMBus devices.

@endverbatim

REVISION HISTORY
$Revision: 4080 $
$Date: 2015-11-30 12:35:30 -0600 (Mon, 30 Nov 2015) $

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
    @ingroup LT_PMBusRail
    Library Header File for LT_PMBusRail
*/

#ifndef PMBUSRAIL_H_
#define PMBUSRAIL_H_

#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "LT_PMBus.h"

typedef struct
{
  uint8_t address; // or char strVal[20];
  uint8_t *pages;
  uint8_t noOfPages;
  bool controller;
  bool multiphase;
  uint32_t capabilities;
} tRailDef;

//! PMBusRail communication. For Multiphase Rails.
class LT_PMBusRail
{
  private:

    LT_PMBus *pmbus_;
    uint8_t railAddress_;
    uint8_t model_[9];

  protected:
    tRailDef **railDef_;

  public:

    //! Construct a LT_PMBus.
    LT_PMBusRail(LT_PMBus *pmbus,     //!< SMBus for communication. Use the PEC or non-PEC version.
                 uint8_t railAddress, //!< The rail address.
                 tRailDef **railDef);  //!< A list of addresses with pages that make up the rail. NULL terminated.

    ~LT_PMBusRail();

    //! Change the pmbus
    void changePMBus(LT_PMBus *pmbus);

    //! Get ther rail address
    uint8_t getAddress();

    //! Ask if devices is a PSM controller
    bool isController ();

    //! Get the number of pages in the rail
    uint8_t getNoPages();

    //! Ask if the rail is multiphase
    bool isMultiphase();

    //! Get a list of capabilities
    uint32_t getCapabilities();

    //! Ask if the rail has the given capability
    uint32_t hasCapability(uint32_t capability);

    //! Merge a rail into this one. No delete of incomming rail.
    void merge(LT_PMBusRail *rail //!< Rail to merge
              );

    //! Set the output voltage of a polyphase rail
    //! @return void
    void setVout(float voltage //!< Rail voltage
                );

    //! Read the input voltage of a polyphase rail
    //! @return voltage
    float readVin(bool polling  //!< true for polling
                 );

    //! Read the output voltage of a polyphase rail
    //! @return voltage
    float readVout(bool polling //!< true for polling
                  );

    //! Read the input current of a polyphase rail
    //! @return current
    float readIin(bool polling  //!< true for polling
                 );

    //! Read the output current of a polyphase rail
    //! @return current
    float readIout(bool polling //!< true for polling
                  );

    //! Read the input power of a polyphase rail
    //! @return current
    float readPin(bool polling //!< true for polling
                 );

    //! Read the output power of a polyphase rail
    //! @return current
    float readPout(bool polling //!< true for polling
                  );

    //! Read the external temperature of a polyphase rail
    //! @return current
    float readExternalTemperature(bool polling //!< true for polling
                                 );

    //! Read the internal temperature of a polyphase rail
    //! @return current
    float readInternalTemperature(bool polling //!< true for polling
                                 );

    //! Read the efficiency (calculated)
    //! @return efficiency or zero if not supported
    float readEfficiency(bool polling //!< true for polling
                        );

    //! Read the average duty cycle
    //! @return duty cycle or zero if not supported
    float readDutyCycle(bool polling //!< true for polling
                       );

    //! Read the phase balance (calculated)
    //! @return phase balance as 100 * (max-min)/total current
    float readPhaseBalance(bool polling //!< true for polling
                          );

    //! Read the transient
    //! @return a value in ms.
    float readTransient(bool polling //!< true for polling
                       );

    //! Read the status word of a polyphase rail
    //! @return word
    uint16_t readStatusWord();

    //! Read the special of a polyphase rail
    //! @return word
    uint16_t readMfrSpecialId();

    //! Clear faults of polyphase rail
    //! @return void
    void clearFaults();

    //! Sequence on a polyphase rail
    //! @return void
    void sequenceOn();

    //! Turn off a polyphase rail
    //! @return void
    void immediateOff();

    //! Sequence off a polyphase rail
    //! @return void
    void sequenceOff();

    //! Margin high polyphase rail
    //! @return void
    void marginHigh();

    //! Margin low polyphase rail
    //! @return void
    void marginLow();

    //! Margin off polyphase rail
    //! @return void
    void marginOff();

};

#endif /* PMBUS_H_ */

