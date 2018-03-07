/*!
LTC SMBus Support: Implementation for a shared SMBus layer

@verbatim

This API is shared with Linduino and RTOS code. End users should code to this
API to enable use of the PMBus code without modifications.

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
