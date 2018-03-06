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
