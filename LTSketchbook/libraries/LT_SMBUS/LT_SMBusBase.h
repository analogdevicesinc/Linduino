/*!
LTC SMBus Support: Implementation for a shared SMBus layer

@verbatim

This API is shared with Linduino and RTOS code. End users should code to this
API to enable use of the PMBus code without modifications.

@endverbatim

REVISION HISTORY
$Revision: 4631 $
$Date: 2016-01-27 16:53:13 -0700 (Wed, 27 Jan 2016) $

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
    @ingroup LT_SMBusBase
    Library Header File for LT_SMBusBase
*/

#ifndef LT_SMBusBase_H_
#define LT_SMBusBase_H_

#include <Arduino.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_I2CBus.h"
#include "LT_SMBus.h"

class LT_SMBusBase : public LT_SMBus
{
  protected:
    static bool         open_;          //!< Used to ensure initialisation of i2c once
    static uint8_t found_address_[];
    LT_I2CBus *i2cbus_;

    LT_SMBusBase();
    LT_SMBusBase(uint32_t speed);
    virtual ~LT_SMBusBase();

  public:

    LT_I2CBus *i2cbus (void)
    {
      return i2cbus_;
    }

    void i2cbus (LT_I2CBus *i2cbus)
    {
      i2cbus_ = i2cbus;
    }

    //! SMBus write byte command
    //! @return void
    void writeByte(uint8_t address,     //!< Slave address
                   uint8_t command,     //!< Command byte
                   uint8_t data         //!< Data to send
                  );

    //! SMBus write byte command for a list of addresses
    //! @return void
    void writeBytes(uint8_t *addresses,         //!< Slave Addresses
                    uint8_t *commands,          //!< Command bytes
                    uint8_t *data,              //!< Data to send
                    uint8_t no_addresses
                   );

    //! SMBus read byte command
    //! @return byte
    uint8_t readByte(uint8_t address,        //!< Slave Address
                     uint8_t command         //!< Command byte
                    );

    //! SMBus write word command
    //! @return void
    void writeWord(uint8_t address,     //!< Slave Address
                   uint8_t command,     //!< Command byte
                   uint16_t data        //!< Data to send
                  );

    //! SMBus read word command
    //! @return word
    uint16_t readWord(uint8_t address,      //!< Slave Address
                      uint8_t command       //!< Command byte
                     );

    //! SMBus write block command
    //! @return void
    void writeBlock(uint8_t address,        //!< Slave Address
                    uint8_t command,        //!< Command byte
                    uint8_t *block,         //!< Data to send
                    uint16_t block_size
                   );

    //! SMBus write then read block command
    //! @return actual size
    uint8_t writeReadBlock(uint8_t address,         //!< Slave Address
                           uint8_t command,         //!< Command byte
                           uint8_t *block_out,      //!< Data to send
                           uint16_t block_out_size, //!< Size of data to send
                           uint8_t *block_in,       //!< Memory to receive data
                           uint16_t block_in_size   //!< Size of receive data memory
                          );

    //! SMBus read block command
    //! @return actual size
    uint8_t readBlock(uint8_t address,         //!< Slave Address
                      uint8_t command,         //!< Command byte
                      uint8_t *block,          //!< Memory to receive data
                      uint16_t block_size      //!< Size of receive data memory
                     );

    //! SMBus send byte command
    //! @return void
    void sendByte(uint8_t address,      //!< Slave Address
                  uint8_t command       //!< Command byte
                 );

    //! Perform ARA
    //! @return address
    uint8_t readAlert(void);

    //! Read with the address and command in loop until ack, then issue stop
    //! @return void
    uint8_t waitForAck(uint8_t address,        //!< Slave Address
                       uint8_t command         //!< Command byte
                      );

    //! SMBus bus probe
    //! @return array of addresses
    uint8_t *probe(uint8_t command      //!< Command byte
                  );

    //! SMBus bus probe
    //! @return array of unique addresses (no global addresses)
    uint8_t *probeUnique(uint8_t command      //!< Command byte
                        );

};

#endif /* LT_SMBusBase_H_ */
