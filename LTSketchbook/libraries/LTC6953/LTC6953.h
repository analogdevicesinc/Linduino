/*!
 LTC6953: Ultra-Low Jitter, JESD204B Clock Distributor with Eleven Programmable Outputs

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

http://www.linear.com/product/LTC6953

http://www.linear.com/product/LTC6953#demoboards


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
    @ingroup LTC6953
    Header for LTC6953: Ultra-Low Jitter, JESD204B Clock Distributor with Eleven Programmable Outputs
*/

#ifndef LTC6953_H

#define LTC6953_H


//! Define the SPI CS pin
#ifndef LTC6953_CS
#define LTC6953_CS QUIKEVAL_CS  //! SPI Chip Select Pin
#endif

#define LTC6953_ADEL0 1   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_ADEL1 2   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_ADEL2 3   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_ADEL3 4   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_ADEL4 5   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_ADEL5 6   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_ADEL6 7   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_ADEL7 8   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_ADEL8 9   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_ADEL9 10   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_ADEL10 11   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_DDEL0 12   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_DDEL1 13   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_DDEL2 14   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_DDEL3 15   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_DDEL4 16   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_DDEL5 17   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_DDEL6 18   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_DDEL7 19   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_DDEL8 20   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_DDEL9 21   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_DDEL10 22   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_EZMD 23   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_FILTV 24   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_INVSTAT 25   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MD0 26   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MD1 27   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MD2 28   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MD3 29   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MD4 30   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MD5 31   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MD6 32   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MD7 33   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MD8 34   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MD9 35   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MD10 36   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MODE0 37   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MODE1 38   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MODE2 39   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MODE3 40   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MODE4 41   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MODE5 42   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MODE6 43   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MODE7 44   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MODE8 45   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MODE9 46   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MODE10 47   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MP0 48   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MP1 49   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MP2 50   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MP3 51   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MP4 52   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MP5 53   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MP6 54   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MP7 55   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MP8 56   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MP9 57   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_MP10 58   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_OINV0 59   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_OINV1 60   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_OINV2 61   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_OINV3 62   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_OINV4 63   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_OINV5 64   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_OINV6 65   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_OINV7 66   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_OINV8 67   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_OINV9 68   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_OINV10 69   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_PART 70   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_PDALL 71   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_PDVCOPK 72   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_PD10 73   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_PD9 74   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_PD8 75   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_PD7 76   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_PD6 77   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_PD5 78   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_PD4 79   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_PD3 80   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_PD2 81   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_PD1 82   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_PD0 83   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_POR 84   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_REV 85   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_SYSCT 86   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_SRQMD 87   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_SSRQ 88   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_SRQEN0 89   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_SRQEN1 90   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_SRQEN2 91   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_SRQEN3 92   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_SRQEN4 93   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_SRQEN5 94   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_SRQEN6 95   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_SRQEN7 96   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_SRQEN8 97   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_SRQEN9 98   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_SRQEN10 99   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_TEMPO 100   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_nVCOOK 101   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_VCOOK 102   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6953_x 103   //!<  for spi_map array, defines location for field specific information used to create the spi map

#define LTC6953_NUM_REGADDR 57  //!< Defines number of LTC6953 SPI registers, used in spi_map array
#define LTC6953_NUM_REGFIELD 103 //!< Defines number of LTC6953 SPI fields, used in spi_map array

#define ADDRx 0                 //!< used for 2nd dim of 2d spi_map array
#define DxMSB 1                 //!< used for 2nd dim of 2d spi_map array
#define NUMBITS 2               //!< used for 2nd dim of 2d spi_map array
#define R_ONLY 3                //!< used for 2nd dim of 2d spi_map array

//! @} */

/* ------------------------------------------------------------------------- */
//! LTC6953 Read Single Address
//!  reads 8 bit Data field to LTC6953.
//!  has to shift data by one bit to account for RW bit
//! @return data that was read from address
uint8_t LTC6953_read(uint8_t cs,    //!< Chip Select Pin
                     int8_t address //!< Register address for the LTC6953.
                    );


/* ------------------------------------------------------------------------- */
//! LTC6953 Read Single Field
//! For SPI FIELDS located in 1 or multiple address location
//!  reads specific address locations
//!  identifies and returns specific field in question
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return data that was read from field
long LTC6953_read_field(uint8_t cs,       //!< Chip Select Pin
                        uint8_t address,  //!< Register address for the LTC6953.
                        uint8_t MSB_loc,  //!< MSB bit location of field
                        uint8_t numbits   //!< length of field (i.e. number of bits in field)
                       );


/* ------------------------------------------------------------------------- */
//! Gets the LTC6953 SPI field value
//! calls function LTC6953_read_field, which
//!  reads specific address locations
//!  identifies and returns specific field in question
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return data that was read from field
long get_LTC6953_SPI_FIELD(uint8_t cs,          //!< Chip Select Pin
                           uint8_t f            //!< SPI field number
                          );


/* ------------------------------------------------------------------------- */
//! LTC6953 Write Single Address
//!  writes 8 bit Data field to LTC6953.
//!  has to shift data by one bit to account for RW bit
//! @return void
void LTC6953_write(uint8_t cs,                  //!< Chip Select Pin
                   uint8_t address,             //!< Register address for the LTC6953.
                   uint8_t Data                 //!< 8-bit data to be written to register
                  );


/* ------------------------------------------------------------------------- */
//! LTC6953 Write Single Field
//!  For SPI FIELDS in 1 or multiple address locations
//!  reads specific address/field location then writes to specific field
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return void
uint8_t LTC6953_write_field(uint8_t cs,        //!< Chip Select Pin
                            long field_data,   //!< Value field value to be set to
                            uint8_t address,   //!< Register address for the LTC6953.
                            uint8_t MSB_loc,   //!< MSB bit location of field
                            uint8_t numbits   //!< length of field (i.e. number of bits in field)
                           );


/* ------------------------------------------------------------------------- */
//! Sets the LTC6953 SPI field value
//! calls function LTC6953_read_field, which
//!  reads specific address/field location then writes to specific field
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return void
void set_LTC6953_SPI_FIELD(uint8_t cs,          //!< Chip Select Pin
                           uint8_t f,           //!< SPI field number
                           long field_data      //!< Value field value to be set to
                          );


/* ------------------------------------------------------------------------- */
//! Writes values to ALL LTC6953 RW addresses from a look-up table
//! - uses the Multi-byte write option for more efficient data transfer
//! - Either SPI Syncs or recommends the use to SYNC pin based on RAO value
//! @return void
void set_LTC6953_REGS_lkup_tbl(uint8_t lkup_tbl_row  //!< Any number 0-2, lookup table has 2 options
                              );

/* ------------------------------------------------------------------------- */
//! Initializes the SPI MAP arrays
//! The values set in initialization are used for all the LTC6953 SPI/WRITE and
//! read functions (set_LTC6953_SPI_FIELD, get_LTC6953_SPI_FIELD,
//! LTC6953_read, LTC6953_write, etc, etc)
//! @return void
void LTC6953_init();


/* ------------------------------------------------------------------------- */
//! returns # of addresses in parts register map (array size)
//! @return # of addresses in parts register map
uint8_t get_LTC6953_REGSIZE();


/* ------------------------------------------------------------------------- */
//! returns the number of bits for a given field name in the SPI map
//! @return the number of bits for a given field name in the SPI map
uint8_t get_LTC6953_SPI_FIELD_NUMBITS(uint8_t f //!< SPI field number
                                     );


/* ------------------------------------------------------------------------- */
//! returns if the given field name is (0)read/write or (1)read_only field
//! @return if the given field is a (0)read/write or (1)read_only field
uint8_t get_LTC6953_SPI_FIELD_RW(uint8_t f   //!< SPI field number
                                ) ;

/* ------------------------------------------------------------------------- */
//! Read the ID string from the EEPROM and determine if any demo board is connected.
//! Returns 1 if successful, 0 if not successful
int8_t discover_demo_board_local(char *demo_name
                                );

#endif  // LTC6953_H
