/*!
LTC SMBus Support: API for a shared SMBus layer

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
    @ingroup LT_SMBus
    Library Header File for LT_SMBus
*/

#ifndef LT_SMBus_H_
#define LT_SMBus_H_

#include <Arduino.h>
#include <stdint.h>
#include <LT_I2CBus.h>

#define SUCCESS                 1
#define FAILURE                 0

class LT_SMBus
{
  protected:
    bool pec_enabled_;

    uint8_t               running_pec_;     //!< Temporary pec calc value
    unsigned char         poly_;            //!< The poly used in the calc
    uint16_t              crc_polynomial_;  //!< The crc poly used in the calc


    //! Initialize the table used to speed up pec calculations
    //! @return void
    void constructTable(uint16_t user_polynomial  //!< The controlling polynomial
                       );

    //! Helper function for pec
    //! @return value
    uint8_t doCalculate(uint8_t data,
                        uint8_t begining_value  //!< The initial value
                       );

    LT_SMBus ();

  public:
    virtual ~LT_SMBus() {}

    virtual LT_I2CBus *i2cbus(void) = 0;
    virtual void i2cbus(LT_I2CBus *i2cbus) = 0;

    //! Check if PEC is enabled
    //! @return true if enabled
    bool pecEnabled(void)
    {
      return pec_enabled_;
    }

    //! Clear the pec value so it can start a new calculation
    //! @return void
    void pecClear(void);

    //! Add a byte to the pec calculation
    //! @return void
    void pecAdd(uint8_t byte_value);

    //! Get the current pec result
    //! @return the pec
    uint8_t pecGet(void);

    //! Check CRC of block data organized as 31 data bytes plus CRC.
    //! Do not mix with PEC calculations.
    //! Return true if CRC does not match.
    bool checkCRC (uint8_t *data);

    //! Get CRC of block data organized as 31 bytes pluse CRC.
    //! Do not mix with PEC calculations.
    //! Return PEC
    uint8_t getCRC (uint8_t *data);

    //! Helper function for pec
    //! @return value
    uint8_t calculate(uint8_t *data,          //!< Data to be pec'ed
                      uint8_t begining_value, //!< Starting value for pec
                      uint8_t start_index,    //!< Starting index
                      uint8_t length          //!< Length of data
                     );

    //! Perform ARA
    //! @return address
    virtual uint8_t readAlert(void) = 0;

    //! SMBus write byte command
    //! @return void
    virtual void writeByte(uint8_t address,   //!< Slave address
                           uint8_t command,   //!< Command byte
                           uint8_t data       //!< Data to send
                          ) = 0;

    //! SMBus write byte command for a list of addresses
    //! @return void
    virtual void writeBytes(uint8_t *addresses,     //!< Slave Addresses
                            uint8_t *commands,      //!< Command bytes
                            uint8_t *data,          //!< Data to send
                            uint8_t no_addresses
                           ) = 0;

    //! SMBus read byte command
    //! @return byte
    virtual uint8_t readByte(uint8_t address,     //!< Slave Address
                             uint8_t command      //!< Command byte
                            ) = 0;

    //! SMBus write word command
    //! @return void
    virtual void writeWord(uint8_t address,   //!< Slave Address
                           uint8_t command,   //!< Command byte
                           uint16_t data      //!< Data to send
                          ) = 0;

    //! SMBus read word command
    //! @return word
    virtual uint16_t readWord(uint8_t address,    //!< Slave Address
                              uint8_t command     //!< Command byte
                             ) = 0;

    //! SMBus write block command
    //! @return void
    virtual void writeBlock(uint8_t address,    //!< Slave Address
                            uint8_t command,    //!< Command byte
                            uint8_t *block,     //!< Data to send
                            uint16_t block_size
                           ) = 0;

    //! SMBus write then read block command
    //! @return actual value
    virtual uint8_t writeReadBlock(uint8_t address,         //!< Slave Address
                                   uint8_t command,         //!< Command byte
                                   uint8_t *block_out,      //!< Data to send
                                   uint16_t block_out_size, //!< Size of data to send
                                   uint8_t *block_in,       //!< Memory to receive data
                                   uint16_t block_in_size   //!< Size of receive data memory
                                  ) = 0;

    //! SMBus read block command
    //! @return actual size
    virtual uint8_t readBlock(uint8_t address,     //!< Slave Address
                              uint8_t command,     //!< Command byte
                              uint8_t *block,      //!< Memory to receive data
                              uint16_t block_size  //!< Size of receive data memory
                             ) = 0;

    //! SMBus send byte command
    //! @return void
    virtual void sendByte(uint8_t address,    //!< Slave Address
                          uint8_t command     //!< Command byte
                         ) = 0;

    //! Read with the address and command in loop until ack, then issue stop
    //! @return void
    virtual uint8_t waitForAck(uint8_t address,     //!< Slave Address
                               uint8_t command      //!< Command byte
                              ) = 0;

    //! SMBus bus probe
    //! @return array of addresses (caller must not delete return memory)
    virtual uint8_t *probe(uint8_t command    //!< Command byte
                          ) = 0;

    //! SMBus bus probe
    //! @return array of unique addresses (no global addresses)
    virtual uint8_t *probeUnique(uint8_t command    //!< Command byte
                                ) = 0;

};

#endif /* LT_SMBus_H_ */
