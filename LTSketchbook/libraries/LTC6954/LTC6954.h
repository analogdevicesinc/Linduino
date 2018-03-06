/*!
 LTC6954: Low Phase Noise, Triple Output Clock Distribution Divider/Driver

@verbatim
 SPI DATA FORMAT (MSB First):

 Write Sequence:
       Byte #1                    Byte #2
 MOSI: A6 A5 A4 A3 A2 A1 A0 W   D7 D6 D5 D4 D3 D2 D1 D0
 MISO: X  X  X  X  X  X  X  X   X  X  X  X  X  X  X  X

 Read Sequence:
       Byte #1                    Byte #2
 MOSI: A6 A5 A4 A3 A2 A1 A0 R   X  X  X  X  X  X  X  X
 MISO: X  X  X  X  X  X  X  X   D7 D6 D5 D4 D3 D2 D1 D0

 W    : SPI Write (0)
 R    : SPI Read  (1)
 Ax   : Address
 Dx   : Data Bits
 X    : Don't care

@endverbatim

http://www.linear.com/product/LTC6954

http://www.linear.com/product/LTC6954#demoboards


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
    @ingroup LTC6954
    Header for LTC6954: Low Phase Noise, Triple Output Clock Distribution Divider/Driver
*/

#ifndef LTC6954_H

#define LTC6954_H


//! Define the SPI CS pin
#ifndef LTC6954_CS
#define LTC6954_CS QUIKEVAL_CS  //! SPI Chip Select Pin
#endif


/*! @name LTC6954 Registers Fields in Alphabetical Order */
#define LTC6954_CMSINV0 1   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_CMSINV1 2   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_CMSINV2 3   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_DEL0 4      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_DEL1 5      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_DEL2 6      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_LVCS0 7     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_LVCS1 8     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_LVCS2 9     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_M0 10       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_M1 11       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_M2 12       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_PART 13     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_PDALL 14    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_PD_DIV0 15  //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_PD_DIV1 16  //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_PD_DIV2 17  //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_PD_OUT0 18  //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_PD_OUT1 19  //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_PD_OUT2 20  //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_REV 21      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_SYNC_EN0 22 //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_SYNC_EN1 23 //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6954_SYNC_EN2 24 //!<  for spi_map array, defines location for field specific information used to create the spi map

#define LTC6954_NUM_REGADDR 8  //!< Defines number of LTC6954 SPI registers, used in spi_map array
#define LTC6954_NUM_REGFIELD 24 //!< Defines number of LTC6954 SPI fields, used in spi_map array

#define ADDRx 0                 //!< used for 2nd dim of 2d spi_map array
#define DxMSB 1                 //!< used for 2nd dim of 2d spi_map array
#define NUMBITS 2               //!< used for 2nd dim of 2d spi_map array
#define R_ONLY 3                //!< used for 2nd dim of 2d spi_map array


//! @} */

/* ------------------------------------------------------------------------- */
//! LTC6954 Read Single Address
//!  reads 8 bit Data field to LTC6954.
//!  has to shift data by one bit to account for RW bit
//! @return data that was read from address
uint8_t LTC6954_read(uint8_t cs,    //!< Chip Select Pin
                     int8_t address //!< Register address for the LTC6954.
                    );


/* ------------------------------------------------------------------------- */
//! LTC6954 Read Single Field
//! For SPI FIELDS located in 1 or multiple address locations
//!  reads specific address locations
//!  identifies and returns specific field in question
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return data that was read from field
long LTC6954_read_field(uint8_t cs,       //!< Chip Select Pin
                        uint8_t address,  //!< Register address for the LTC6954.
                        uint8_t MSB_loc,  //!< MSB bit location of field
                        uint8_t numbits   //!< length of field (i.e. number of bits in field)
                       );


/* ------------------------------------------------------------------------- */
//! Gets the LTC6954 SPI field value
//! calls function LTC6954_read_field, which
//!  reads specific address locations
//!  identifies and returns specific field in question
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return data that was read from field
long get_LTC6954_SPI_FIELD(uint8_t cs,          //!< Chip Select Pin
                           uint8_t f            //!< SPI field number
                          );


/* ------------------------------------------------------------------------- */
//! LTC6954 Write Single Address
//!  writes 8 bit Data field to LTC6954.
//!  has to shift data by one bit to account for RW bit
//! @return void
void LTC6954_write(uint8_t cs,                  //!< Chip Select Pin
                   uint8_t address,             //!< Register address for the LTC6954.
                   uint8_t Data                 //!< 8-bit data to be written to register
                  );


/* ------------------------------------------------------------------------- */
//! LTC6954 Write Single Field
//!  For SPI FIELDS in 1 or multiple address locations
//!  reads specific address/field location then writes to specific field
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return void
uint8_t LTC6954_write_field(uint8_t cs,        //!< Chip Select Pin
                            long field_data,   //!< Value field value to be set to
                            uint8_t address,   //!< Register address for the LTC6954.
                            uint8_t MSB_loc,   //!< MSB bit location of field
                            uint8_t numbits   //!< length of field (i.e. number of bits in field)
                           );


/* ------------------------------------------------------------------------- */
//! Sets the LTC6954 SPI field value
//! calls function LTC6954_read_field, which
//!  reads specific address/field location then writes to specific field
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return void
void set_LTC6954_SPI_FIELD(uint8_t cs,          //!< Chip Select Pin
                           uint8_t f,           //!< SPI field number
                           long field_data      //!< Value field value to be set to
                          );


/* ------------------------------------------------------------------------- */
//! Writes values to ALL LTC6954 RW address
//! @return void
void set_LTC6954_ALLREGS(uint8_t cs,            //!< Chip Select Pin
                         uint8_t reg00,         //!< LTC6954 register 0
                         uint8_t reg01,         //!< LTC6954 register 1
                         uint8_t reg02,         //!< LTC6954 register 2
                         uint8_t reg03,         //!< LTC6954 register 3
                         uint8_t reg04,         //!< LTC6954 register 4
                         uint8_t reg05,         //!< LTC6954 register 5
                         uint8_t reg06         //!< LTC6954 register 6
                        );

/* ------------------------------------------------------------------------- */
//! Initializes the SPI MAP arrays
//! The values set in initialization are used all the LTC6954 SPI/WRITE and
//! read functions (set_LTC6954_SPI_FIELD, get_LTC6954_SPI_FIELD,
//! LTC6954_read, LTC6954_write, etc, etc)
//! @return void
void LTC6954_init();


/* ------------------------------------------------------------------------- */
//! returns # of addresses in parts register map (array size)
//! @return # of addresses in parts register map
uint8_t get_LTC6954_REGSIZE();


/* ------------------------------------------------------------------------- */
//! returns the number of bits for a given field name in the SPI map
//! @return the number of bits for a given field name in the SPI map
uint8_t get_LTC6954_SPI_FIELD_NUMBITS(uint8_t f //!< SPI field number
                                     );


/* ------------------------------------------------------------------------- */
//! returns if the given field name is (0)read/write or (1)read_only field
//! @return if the given field is a (0)read/write or (1)read_only field
uint8_t get_LTC6954_SPI_FIELD_RW(uint8_t f   //!< SPI field number
                                ) ;



#endif  // LTC6954_H
