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

//! @ingroup PMBus_SMBus
//! @{
//! @defgroup LT_PMBusDevice LT_PMBusDevice: Implementation of Device Detection and Capability
//! @}

/*! @file
    @ingroup LT_PMBusDevice
    Library Header File for LT_PMBusDevice
*/

#ifndef LT_PMBusDevice_H_
#define LT_PMBusDevice_H_

#define HAS_VOUT          1 << 0
#define HAS_VIN           1 << 1
#define HAS_IOUT          1 << 2
#define HAS_IIN           1 << 3
#define HAS_POUT          1 << 4
#define HAS_PIN           1 << 5
#define HAS_TEMP          1 << 6
#define HAS_DC            1 << 7
#define HAS_STATUS_WORD   1 << 14
#define HAS_STATUS_EXT    1 << 15


#include <stdint.h>
#include "LT_PMBus.h"
#include "LT_PMBusRail.h"
#include "LT_PMBusSpeedTest.h"

class LT_PMBusDevice
{
  protected:
    LT_PMBus *pmbus_;
    uint8_t address_;
    uint32_t maxSpeed_;
    uint8_t model_[9];

    LT_PMBusDevice(LT_PMBus *pmbus, uint8_t address):pmbus_(pmbus), address_(address)
    {
    }

  protected:

    void probeSpeed();


  public:
    virtual ~LT_PMBusDevice() {}

    LT_PMBus *pmbus();

    LT_SMBus *smbus();

    //! Change the pmbus
    void changePMBus(LT_PMBus *pmbus);

    //! Get the address
    //! @return address
    uint8_t getAddress ();

    virtual char *getType(void);

    virtual uint8_t getNumPages(void) = 0;

    void setPage(uint8_t page);

    //! Get the maximum speed the device can communicate with
    //! @return speed
    uint32_t getMaxSpeed(void);

    //! Set the speed. If > 100000, enable clock stretching
    virtual void setSpeed(uint32_t speed);        //!< Speed

    //! Get the supported capabilities
    //! @return or'd list of capabilities
    virtual uint32_t getCapabilities () = 0;

    //! Is/are these capability(s) supported?
    //! @return true if yes
    virtual bool hasCapability(
      uint32_t capability          //!< List of capabilities
    ) = 0;


    //! Return a list of rails if any page is part of a rail.
    //! @return A rail, but partially built, and may need merging with other devices. Caller must free memory.
    virtual LT_PMBusRail **getRails() = 0;

    //! Enable the Fault Log
    virtual void enableFaultLog() {}

    //! Disable the Fault Log
    virtual void disableFaultLog() {}

    //! Is there a fault log?
    //! @return true/false
    virtual bool hasFaultLog();

    //! Get the fault log text (call must free)
    //! @return text
    virtual char *getFaultLog();

    //! Clear the Fault Log
    virtual void clearFaultLog() { }

    /*
     * Set the output voltage of a polyphase rail
     *
     * voltage: the target voltage
     */
    void setVout(float voltage);

    /*
     * Read the input voltage of a polyphase rail
     *
     * polling: poll if true
     * return: the input voltage
     */
    float readVin(bool polling);

    /*
     * Read the output voltage of a polyphase rail
     *
     * polling: poll if true
     * return: the output voltage
     */
    float readVout(bool polling);

    /*
     * Read the input current of a polyphase rail
     *
     * polling: poll if true
     * return: the input current
     */
    float readIin(bool polling);

    /*
     * Read the output current of a polyphase rail
     *
     * polling: poll if true
     * return: the output current
     */
    float readIout(bool polling);

    /*
     * Read the input power of a polyphase rail
     *
     * polling: poll if true
     * return: the output current
     */
    float readPin(bool polling);

    /*
     * Read the output power of a polyphase rail
     *
     * polling: poll if true
     * return: the output power
     */
    float readPout(bool polling);

    /*
     * Read the external temperature of a polyphase rail
     *
     * polling: poll if true
     * return: the temperature
     */
    float readExternalTemperature(bool polling);

    /*
     * Read the internal temperature of a polyphase rail
     *
     * polling: poll if true
     * return: the temperature
     */
    float readInternalTemperature(bool polling);

    /*
     * Read the average duty cycle of a polyphase rail
     *
     * polling: poll if true
     * return: the duty cycle
     */
    float readDutyCycle(bool polling);

    /*
     * Read the status word of a polyphase rail
     *
     * return: status word
     */
    uint16_t readStatusWord();

    /*
     * Read the special id of a polyphase rail
     *
     * return: special id
     */
    uint16_t readMfrSpecialId();

    /*
     * Clear faults of polyphase rail
     */
    void clearFaults();

    /*
     * Margin high polyphase rail
     */
    void marginHigh();

    /*
     * Margin low polyphase rail
     *
     */
    void marginLow();

    /*
     * Margin off polyphase rail
     *
     */
    void marginOff();
};

#endif /* LT_PMBusDevice_H_ */
