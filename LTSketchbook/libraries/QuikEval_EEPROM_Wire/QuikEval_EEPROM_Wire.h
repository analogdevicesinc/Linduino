/*!
QuikEval EEPROM Library


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
    @ingroup QuikEval
    Header file for QuikEval EEPROM library
*/

#ifndef QUIKEVAL_H
#define QUIKEVAL_H

#include <stdint.h>

//! Historical length of the ID string. There are 256 bytes
//! in the 24LC024 EEPROM, the rest is free for user data.
#define QUIKEVAL_ID_SIZE 50

//! 0x0A terminates the ID String
#define QUIKEVAL_ID_TERMINATOR 0x0A

//!@name EEPROM constants
//!@{
#define EEPROM_CAL_STATUS_ADDRESS 0x40    //! QuikEval EEPROM calibration start address
#define EEPROM_CAL_KEY            0x1234  //! Value to indicate calibration has occurred
#define EEPROM_I2C_ADDRESS        0xA0    //! EEPROM I2C address
#define EEPROM_PAGE_SIZE          16      //! EEPROM Page size in bytes
#define EEPROM_DATA_SIZE          256     //! EEPROM size in bytes
#define EEPROM_TIMEOUT            10      //! EEPROM timeout in ms
//!@}

//! Structure to hold parsed information from
//! ID string - example: LTC2654-L16,Cls,D2636,01,01,DC,DC1678A-A,-------
struct demo_board_type
{
  char product_name[15];       //!< LTC Product  (LTC2654-L16)
  char name[15];               //!< Demo Circuit number (DC1678)
  char option;                 //!< Demo Circuit option (A)
};

//! Instantiate demo board structure
extern demo_board_type demo_board;

//! Read the id string from the EEPROM, then parse the
//! product name, demo board name, and demo board option
//! from the id string into the global demo_board variable.
//! Returns the number of characters read from the information string.
uint8_t read_quikeval_id_string(char *buffer);

//! Read the ID string from the EEPROM and determine if the correct board is connected.
//! Returns 1 if successful, 0 if not successful
int8_t discover_demo_board(char *demo_name);

//! Determine if the EEPROM is ready for communication by writing
//! the address+!write byte and looking for an acknowledge. This is
//! repeated every 1ms until an acknowledge occurs, or a timeout occurs.
//! If a timeout occurs, an I2C stop is generated.
//! Returns 0 if an acknowledge is generated and 1 if not.
int8_t eeprom_poll(uint8_t i2c_address);

//! Wait for the eeprom write cycle to complete by executing
//! the acknowledge polling loop.
//! Returns 0 if an acknowledge is generated and 1 if not.
int8_t eeprom_write_poll(uint8_t i2c_address);

//! Write the data byte to the EEPROM with i2c_address starting at EEPROM address.
//! Returns the total number of bytes written
uint8_t eeprom_write_byte(uint8_t i2c_address, char data, uint16_t address);

//! Write the data byte array to the EEPROM with i2c_address starting at EEPROM address.
//!! Returns the total number of bytes written
uint8_t eeprom_write_byte_array(uint8_t i2c_address, char data[], uint16_t address, uint8_t num_bytes);

//! Write the buffer to the EEPROM with i2c_address starting at EEPROM address in blocks of EEPROM_PAGE_SIZE bytes.
//! Returns the total number of byte written
uint8_t eeprom_write_buffer(uint8_t i2c_address, char *buffer, uint16_t address);

//! Read a data byte at address from the EEPROM with i2c_address.
//! Returns the number of bytes read.
uint8_t eeprom_read_byte(uint8_t i2c_address, char *data, uint16_t address);

//! Read a data byte at address from the EEPROM with i2c_address.
//! Returns the number of bytes read.
uint8_t eeprom_read_byte_array(uint8_t i2c_address, char *data, uint16_t address, uint8_t num_bytes);

//! Read data bytes from the EEPROM starting at address until number bytes read equals count. A null terminator is
//! added to the end of the buffer.
//! Returns the number of bytes read.
uint8_t eeprom_read_buffer(uint8_t i2c_address, char *buffer, uint16_t address, uint8_t count);

//! Read data bytes from the EEPROM starting at address until the terminator is read
//! or the number bytes read equals count. A null terminator is placed at the end of the buffer.
//! Returns the number of bytes read.
uint8_t eeprom_read_buffer_with_terminator(uint8_t i2c_address, char *buffer, uint16_t address, char terminator, uint8_t count);

//! Write the 2 byte integer data to the EEPROM starting at address. Use the eeprom_write_byte
//! routine to avoid keeping track of page boundaries with the eeprom_write_buffer routine.
//! Returns the total number of bytes written.
uint8_t eeprom_write_int16(uint8_t i2c_address, int16_t write_data, uint16_t address);

//! Read the two byte integer data from the EEPROM starting at address.
//! Returns the total number of bytes read.
uint8_t eeprom_read_int16(uint8_t i2c_address, int16_t *read_data, uint16_t address);

//! Write the 4 byte float data to the EEPROM starting at address. Use the eeprom_write_byte
//! routine to avoid keeping track of page boundaries with the eeprom_write_buffer routine.
//! Returns the total number of bytes written.
uint8_t eeprom_write_float(uint8_t i2c_address, float write_data, uint16_t address);

//! Read the four byte float data from the EEPROM starting at address.
//! Returns the total number of bytes written
uint8_t eeprom_read_float(uint8_t i2c_address, float *read_data, uint16_t address);

//! Write the 4 byte long data to the EEPROM starting at address. Use the eeprom_write_byte
//! routine to avoid keeping track of page boundaries with the eeprom_write_buffer routine.
//! Returns the total number of bytes written.
uint8_t eeprom_write_int32(uint8_t i2c_address, int32_t write_data, uint16_t address);

//! Read the four byte long data from the EEPROM starting at address.
//! Returns the total number of bytes written
uint8_t eeprom_read_int32(uint8_t i2c_address, int32_t *read_data, uint16_t address);

//! Functions to set and clear the calibration key. Useful for swtiching between
//! calibrated operation and default operation, without actually clearing the stored
//! calibration numbers.

//! Enable calibration key
uint8_t enable_calibration();
//! Disable calibration key
uint8_t disable_calibration();

#endif

