/*!
LT_I2CBus: Routines to communicate to I2C by Wire Library.


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
    @ingroup LT_I2CBus
    Library Header File for LT_I2CBus
*/

#ifndef LT_I2CBus_H
#define LT_I2CBus_H

#include <stdint.h>
#include <LT_Wire.h>

class LT_I2CBus
{
  private:
    bool inGroupProtocol_;
    uint32_t speed_;

  public:
    LT_I2CBus();
    LT_I2CBus(uint32_t speed);

    //! Change the speed of the bus.
    void changeSpeed(uint32_t speed  //!< the speed
                    );

    //! Get the speed of the bus.
    uint32_t getSpeed();

    //! Read a byte, store in "value".
    //! @return 0 on success, 1 on failure
    int8_t readByte(uint8_t address,  //!< 7-bit I2C address
                    uint8_t *value    //!< Byte to be read
                   );

    //! Write "value" byte to device at "address"
    //! @return 0 on success, 1 on failure
    int8_t writeByte(uint8_t address, //!< 7-bit I2C address
                     uint8_t value    //!< Byte to be written
                    );

    //! Read a byte of data at register specified by "command", store in "value"
    //! @return 0 on success, 1 on failure
    int8_t readByteData(uint8_t address,     //!< 7-bit I2C address
                        uint8_t command,     //!< Command byte
                        uint8_t *value       //!< Byte to be read
                       );

    //! Write a byte of data to register specified by "command"
    //! @return 0 on success, 1 on failure
    int8_t writeByteData(uint8_t address,    //!< 7-bit I2C address
                         uint8_t command,    //!< Command byte
                         uint8_t value       //!< Byte to be written
                        );

    //! Read a 16-bit word of data from register specified by "command"
    //! @return 0 on success, 1 on failure
    int8_t readWordData(uint8_t address,     //!< 7-bit I2C address
                        uint8_t command,     //!< Command byte
                        uint16_t *value      //!< Word to be read
                       );

    //! Write a 16-bit word of data to register specified by "command"
    //! @return 0 on success, 1 on failure
    int8_t writeWordData(uint8_t address,    //!< 7-bit I2C address
                         uint8_t command,    //!< Command byte
                         uint16_t value      //!< Word to be written
                        );

    //! Read a block of data, starting at register specified by "command" and ending at (command + length - 1)
    //! @return 0 on success, 1 on failure
    int8_t readBlockData(uint8_t address,     //!< 7-bit I2C address
                         uint8_t command,     //!< Command byte
                         uint16_t length,     //!< Length of array
                         uint8_t *values      //!< Byte array to be read
                        );

    //! Read a block of data, starting at register specified by "command" and ending at (command + length - 1)
    //! @return 0 on success, 1 on failure
    int8_t readBlockDataPec(uint8_t address,     //!< 7-bit I2C address
                            uint8_t command,     //!< Command byte
                            uint16_t length,     //!< Length of array
                            uint8_t *values      //!< Byte array to be read
                           );

    //! Read a block of data, no command byte, reads length number of bytes and stores it in values.
    //! @return 0 on success, 1 on failure
    int8_t readBlockData(uint8_t address,     //!< 7-bit I2C address
                         uint16_t length,     //!< Length of array
                         uint8_t *values      //!< Byte array to be read
                        );

    //! Read a block of data, no command byte, reads length number of bytes and stores it in values.
    //! @return 0 on success, 1 on failure
    int8_t readBlockDataPec(uint8_t address,     //!< 7-bit I2C address
                            uint16_t length,     //!< Length of array
                            uint8_t *values      //!< Byte array to be read
                           );



    //! Write a block of data, starting at register specified by "command" and ending at (command + length - 1)
    //! @return 0 on success, 1 on failure
    int8_t writeBlockData(uint8_t address,       //!< 7-bit I2C address
                          uint8_t command,       //!< Command byte
                          uint16_t length,       //!< Length of array
                          uint8_t *values        //!< Byte array to be written
                         );

    //! Write a two command bytes, then receive a block of data
    //! @return 0 on success, 1 on failure
    int8_t twoByteCommandReadBlock(uint8_t address,     //!< 7-bit I2C address
                                   uint16_t command,    //!< Command word
                                   uint16_t length,     //!< Length of array
                                   uint8_t *values      //!< Byte array to be read
                                  );

    //! Initializes Linduino I2C port.
    //! Before communicating to the I2C port through the QuikEval connector, you must also run
    //! quikeval_I2C_connect to connect the I2C port to the QuikEval connector through the
    //! QuikEval MUX (and disconnect SPI).
    void quikevalI2CInit(void);

    //! Switch MUX to connect I2C pins to QuikEval connector.
    //! This will disconnect SPI pins.
    void quikevalI2CConnect(void);

    //! starts group protocol so I2CBus knows to repeat START instead of STOP.
    void startGroupProtocol(void);

    //! ends group protocol so I2CBus knows to send STOPs again.
    void endGroupProtocol(void);
};

#endif  // LT_I2CBus_H
