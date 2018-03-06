/*!
 LTC6952: Ultralow Jitter, 4.5GHz PLL with 11 Outputs and JESD204B Support

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

http://www.linear.com/product/LTC6952

http://www.linear.com/product/LTC6952#demoboards


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
    @ingroup LTC6952
    Header for LTC6952: Ultralow Jitter, 4.5GHz PLL with 11 Outputs and JESD204B Support
*/

#ifndef LTC6952_H

#define LTC6952_H


//! Define the SPI CS pin
#ifndef LTC6952_CS
#define LTC6952_CS QUIKEVAL_CS  //! SPI Chip Select Pin
#endif


#define LTC6952_ADEL0 1   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_ADEL1 2   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_ADEL2 3   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_ADEL3 4   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_ADEL4 5   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_ADEL5 6   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_ADEL6 7   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_ADEL7 8   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_ADEL8 9   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_ADEL9 10   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_ADEL10 11   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_BST 12   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_CP 13   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_CPDN 14   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_CPINV 15   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_CPMID 16   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_CPRST 17   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_CPUP 18   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_CPWIDE 19   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_DDEL0 20   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_DDEL1 21   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_DDEL2 22   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_DDEL3 23   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_DDEL4 24   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_DDEL5 25   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_DDEL6 26   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_DDEL7 27   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_DDEL8 28   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_DDEL9 29   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_DDEL10 30   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_EZMD 31   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_FILTR 32   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_FILTV 33   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_INVSTAT 34   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_LKCT 35   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_LKWIN 36   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_nLOCK 37   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_LOCK 38   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MD0 39   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MD1 40   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MD2 41   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MD3 42   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MD4 43   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MD5 44   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MD6 45   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MD7 46   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MD8 47   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MD9 48   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MD10 49   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MODE0 50   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MODE1 51   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MODE2 52   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MODE3 53   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MODE4 54   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MODE5 55   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MODE6 56   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MODE7 57   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MODE8 58   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MODE9 59   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MODE10 60   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MP0 61   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MP1 62   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MP2 63   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MP3 64   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MP4 65   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MP5 66   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MP6 67   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MP7 68   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MP8 69   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MP9 70   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_MP10 71   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_ND 72   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_OINV0 73   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_OINV1 74   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_OINV2 75   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_OINV3 76   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_OINV4 77   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_OINV5 78   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_OINV6 79   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_OINV7 80   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_OINV8 81   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_OINV9 82   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_OINV10 83   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PARSYNC 84   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PART 85   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PDALL 86   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PDPLL 87   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PDREFPK 88   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PDVCOPK 89   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PD10 90   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PD9 91   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PD8 92   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PD7 93   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PD6 94   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PD5 95   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PD4 96   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PD3 97   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PD2 98   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PD1 99   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_PD0 100   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_POR 101   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_RAO 102   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_RD 103   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_nREFOK 104   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_REFOK 105   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_REV 106   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_SYSCT 107   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_SRQMD 108   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_SSRQ 109   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_SRQEN0 110   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_SRQEN1 111   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_SRQEN2 112   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_SRQEN3 113   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_SRQEN4 114   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_SRQEN5 115   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_SRQEN6 116   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_SRQEN7 117   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_SRQEN8 118   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_SRQEN9 119   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_SRQEN10 120   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_TEMPO 121   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_UNLOCK 122   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_nVCOOK 123   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_VCOOK 124   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6952_x 125   //!<  for spi_map array, defines location for field specific information used to create the spi map

#define LTC6952_NUM_REGADDR 57  //!< Defines number of LTC6952 SPI registers, used in spi_map array
#define LTC6952_NUM_REGFIELD 125 //!< Defines number of LTC6952 SPI fields, used in spi_map array

#define ADDRx 0                 //!< used for 2nd dim of 2d spi_map array
#define DxMSB 1                 //!< used for 2nd dim of 2d spi_map array
#define NUMBITS 2               //!< used for 2nd dim of 2d spi_map array
#define R_ONLY 3                //!< used for 2nd dim of 2d spi_map array

//! @} */

/* ------------------------------------------------------------------------- */
//! LTC6952 Read Single Address
//!  reads 8 bit Data field to LTC6952.
//!  has to shift data by one bit to account for RW bit
//! @return data that was read from address
uint8_t LTC6952_read(uint8_t cs,    //!< Chip Select Pin
                     int8_t address //!< Register address for the LTC6952.
                    );


/* ------------------------------------------------------------------------- */
//! LTC6952 Read Single Field
//! For SPI FIELDS located in 1 or multiple address location
//!  reads specific address locations
//!  identifies and returns specific field in question
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return data that was read from field
long LTC6952_read_field(uint8_t cs,       //!< Chip Select Pin
                        uint8_t address,  //!< Register address for the LTC6952.
                        uint8_t MSB_loc,  //!< MSB bit location of field
                        uint8_t numbits   //!< length of field (i.e. number of bits in field)
                       );


/* ------------------------------------------------------------------------- */
//! Gets the LTC6952 SPI field value
//! calls function LTC6952_read_field, which
//!  reads specific address locations
//!  identifies and returns specific field in question
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return data that was read from field
long get_LTC6952_SPI_FIELD(uint8_t cs,          //!< Chip Select Pin
                           uint8_t f            //!< SPI field number
                          );


/* ------------------------------------------------------------------------- */
//! LTC6952 Write Single Address
//!  writes 8 bit Data field to LTC6952.
//!  has to shift data by one bit to account for RW bit
//! @return void
void LTC6952_write(uint8_t cs,                  //!< Chip Select Pin
                   uint8_t address,             //!< Register address for the LTC6952.
                   uint8_t Data                 //!< 8-bit data to be written to register
                  );


/* ------------------------------------------------------------------------- */
//! LTC6952 Write Single Field
//!  For SPI FIELDS in 1 or multiple address locations
//!  reads specific address/field location then writes to specific field
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return void
uint8_t LTC6952_write_field(uint8_t cs,        //!< Chip Select Pin
                            long field_data,   //!< Value field value to be set to
                            uint8_t address,   //!< Register address for the LTC6952.
                            uint8_t MSB_loc,   //!< MSB bit location of field
                            uint8_t numbits   //!< length of field (i.e. number of bits in field)
                           );


/* ------------------------------------------------------------------------- */
//! Sets the LTC6952 SPI field value
//! calls function LTC6952_read_field, which
//!  reads specific address/field location then writes to specific field
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return void
void set_LTC6952_SPI_FIELD(uint8_t cs,          //!< Chip Select Pin
                           uint8_t f,           //!< SPI field number
                           long field_data      //!< Value field value to be set to
                          );


/* ------------------------------------------------------------------------- */
//! Writes values to ALL LTC6952 RW addresses from a look-up table
//! - uses the Multi-byte write option for more efficient data transfer
//! - Either SPI Syncs or recommends the use to SYNC pin based on RAO value
//! @return void
void set_LTC6952_REGS_lkup_tbl(uint8_t lkup_tbl_row  //!< Any number 0-2, lookup table has 2 options
                              );

/* ------------------------------------------------------------------------- */
//! Initializes the SPI MAP arrays
//! The values set in initialization are used for all the LTC6952 SPI/WRITE and
//! read functions (set_LTC6952_SPI_FIELD, get_LTC6952_SPI_FIELD,
//! LTC6952_read, LTC6952_write, etc, etc)
//! @return void
void LTC6952_init();


/* ------------------------------------------------------------------------- */
//! returns # of addresses in parts register map (array size)
//! @return # of addresses in parts register map
uint8_t get_LTC6952_REGSIZE();


/* ------------------------------------------------------------------------- */
//! returns the number of bits for a given field name in the SPI map
//! @return the number of bits for a given field name in the SPI map
uint8_t get_LTC6952_SPI_FIELD_NUMBITS(uint8_t f //!< SPI field number
                                     );


/* ------------------------------------------------------------------------- */
//! returns if the given field name is (0)read/write or (1)read_only field
//! @return if the given field is a (0)read/write or (1)read_only field
uint8_t get_LTC6952_SPI_FIELD_RW(uint8_t f   //!< SPI field number
                                ) ;

/* ------------------------------------------------------------------------- */
//! Read the ID string from the EEPROM and determine if any demo board is connected.
//! Returns 1 if successful, 0 if not successful
int8_t discover_demo_board_local(char *demo_name
                                );

#endif  // LTC6952_H
