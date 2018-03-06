/*!
 LTC6951: Ultra-Low Jitter 2MHz to 2.7GHz Multi-Output Clock Synthesizer with Integrated VCO

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

http://www.linear.com/product/LTC6951

http://www.linear.com/product/LTC6951#demoboards


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
    @ingroup LTC6951
    Header for LTC6951: Ultra-Low Jitter 2MHz to 2.5GHz Multi-Output Clock Synthesizer with Integrated VCO
*/

#ifndef LTC6951_H

#define LTC6951_H


//! Define the SPI CS pin
#ifndef LTC6951_CS
#define LTC6951_CS QUIKEVAL_CS  //! SPI Chip Select Pin
#endif


/*! @name LTC6951 Registers Fields in Alphabetical Order */
#define LTC6951_ALCCAL 1     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_ALCEN 2      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_ALCHI 3      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_ALCLO 4      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_ALCMON 5     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_ALCULOK 6    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_AUTOCAL 7    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_BD 8         //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_BST 9        //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_CAL 10       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_CP 11        //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_CPDN 12      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_CPMID 13     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_CPRST 14     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_CPUP 15      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_CPWIDE 16    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_DLY0 17      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_DLY1 18      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_DLY2 19      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_DLY3 20      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_DLY4 21      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_FILT 22      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_INVSTAT 23   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_LKCT 24      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_LKWIN 25     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_LOCK 26      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_NLOCK 27     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_MCO 28       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_MC1 29       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_MC2 30       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_MC3 31       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_MC4 32       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_MD0 33       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_MD1 34       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_MD2 35       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_MD3 36       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_MD4 37       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_MUTE0 38     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_MUTE1 39     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_MUTE2 40     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_MUTE3 41     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_MUTE4 42     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_ND 43        //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_OINV0 44     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_OINV1 45     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_OINV2 46     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_OINV3 47     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_OINV4 48     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_PART 49      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_PD 50        //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_PDALL 51     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_PDOUT 52     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_PDPLL 53     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_PDREFPK 54   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_PDVCO 55     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_POR 56       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_RAO 57       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_RD 58        //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_REFOK 59     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_NREFOK 60    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_REV 61       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_SN 62        //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_SR 63        //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_SSYNC 64     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_SYNCEN0 65   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_SYNCEN1 66   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_SYNCEN2 67   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_SYNCEN3 68   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_SYNCEN4 69   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_UNLOCK 70     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6951_x 71         //!<  for spi_map array, defines location for field specific information used to create the spi map



#define LTC6951_NUM_REGADDR 20  //!< Defines number of LTC6951 SPI registers, used in spi_map array
#define LTC6951_NUM_REGFIELD 71 //!< Defines number of LTC6951 SPI fields, used in spi_map array

#define ADDRx 0                 //!< used for 2nd dim of 2d spi_map array
#define DxMSB 1                 //!< used for 2nd dim of 2d spi_map array
#define NUMBITS 2               //!< used for 2nd dim of 2d spi_map array
#define R_ONLY 3                //!< used for 2nd dim of 2d spi_map array

//! @} */

/* ------------------------------------------------------------------------- */
//! LTC6951 Read Single Address
//!  reads 8 bit Data field to LTC6951.
//!  has to shift data by one bit to account for RW bit
//! @return data that was read from address
uint8_t LTC6951_read(uint8_t cs,    //!< Chip Select Pin
                     int8_t address //!< Register address for the LTC6951.
                    );


/* ------------------------------------------------------------------------- */
//! LTC6951 Read Single Field
//! For SPI FIELDS located in 1 or multiple address location
//!  reads specific address locations
//!  identifies and returns specific field in question
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return data that was read from field
long LTC6951_read_field(uint8_t cs,       //!< Chip Select Pin
                        uint8_t address,  //!< Register address for the LTC6951.
                        uint8_t MSB_loc,  //!< MSB bit location of field
                        uint8_t numbits   //!< length of field (i.e. number of bits in field)
                       );


/* ------------------------------------------------------------------------- */
//! Gets the LTC6951 SPI field value
//! calls function LTC6951_read_field, which
//!  reads specific address locations
//!  identifies and returns specific field in question
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return data that was read from field
long get_LTC6951_SPI_FIELD(uint8_t cs,          //!< Chip Select Pin
                           uint8_t f            //!< SPI field number
                          );


/* ------------------------------------------------------------------------- */
//! LTC6951 Write Single Address
//!  writes 8 bit Data field to LTC6951.
//!  has to shift data by one bit to account for RW bit
//! @return void
void LTC6951_write(uint8_t cs,                  //!< Chip Select Pin
                   uint8_t address,             //!< Register address for the LTC6951.
                   uint8_t Data                 //!< 8-bit data to be written to register
                  );
/* ------------------------------------------------------------------------- */
//! LTC6951 Write Single Field
//!  For SPI FIELDS in 1 or multiple address locations
//!  reads specific address/field location then writes to specific field
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return void
uint8_t LTC6951_write_field(uint8_t cs,        //!< Chip Select Pin
                            long field_data,   //!< Value field value to be set to
                            uint8_t address,   //!< Register address for the LTC6951.
                            uint8_t MSB_loc,   //!< MSB bit location of field
                            uint8_t numbits   //!< length of field (i.e. number of bits in field)
                           );


/* ------------------------------------------------------------------------- */
//! Sets the LTC6951 SPI field value
//! calls function LTC6951_read_field, which
//!  reads specific address/field location then writes to specific field
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return void
void set_LTC6951_SPI_FIELD(uint8_t cs,          //!< Chip Select Pin
                           uint8_t f,           //!< SPI field number
                           long field_data      //!< Value field value to be set to
                          );


/* ------------------------------------------------------------------------- */
//! Writes values to ALL LTC6951 RW addresses
//! @return void
void set_LTC6951_ALLREGS(uint8_t cs,            //!< Chip Select Pin
                         uint8_t reg01,         //!< LTC6951 register 1
                         uint8_t reg02,         //!< LTC6951 register 2
                         uint8_t reg03,         //!< LTC6951 register 3
                         uint8_t reg04,         //!< LTC6951 register 4
                         uint8_t reg05,         //!< LTC6951 register 5
                         uint8_t reg06,         //!< LTC6951 register 6
                         uint8_t reg07,         //!< LTC6951 register 7
                         uint8_t reg08,         //!< LTC6951 register 8
                         uint8_t reg09,         //!< LTC6951 register 9
                         uint8_t reg0A,         //!< LTC6951 register 10
                         uint8_t reg0B,         //!< LTC6951 register 11
                         uint8_t reg0C,         //!< LTC6951 register 12
                         uint8_t reg0D,         //!< LTC6951 register 13
                         uint8_t reg0E,         //!< LTC6951 register 14
                         uint8_t reg0F,         //!< LTC6951 register 15
                         uint8_t reg10,         //!< LTC6951 register 16
                         uint8_t reg11,         //!< LTC6951 register 17
                         uint8_t reg12          //!< LTC6951 register 18
                        );

/* ------------------------------------------------------------------------- */
//! Writes values to ALL LTC6951 RW addresses from a look-up table
//! - uses the Multi-byte write option for more efficient data transfer
//! - Calibrates VCO based on RAO and Autocal bit vales
//! - Either SPI Syncs or recommends the use to SYNC pin based on RAO, SN and SR values
//! @return void
void set_LTC6951_REGS_lkup_tbl(uint8_t lkup_tbl_row  //!< Any number 0-9, lookup table has 10 options
                              );

/* ------------------------------------------------------------------------- */
//! This function toggles between two frequencies
//! - if measuring calibration or settling times set SPI clock to max SPI rate (8MHz)
//! - writes LTC6951 registers from a local look up table
//! - uses the Multi-byte write option for more efficient data transfer
//! - Calibrates VCO based on RAO and Autocal bit vales
//! - DOES NOT SYNC OUTPUTS
//! - RECOMMENDATION: If measuring settling times with E5052, NOT MUTING OUTPUT DURING CAL can provide more meaningful results
//! @return void
void set_LTC6951_REGS_freq_jump(uint8_t lkup_tbl_row,  //!< Can be 0 or 1
                                uint8_t init_setup     //!<  0 = not first setup, 1= first setup
                               );


/* ------------------------------------------------------------------------- */
//! Initializes the SPI MAP arrays
//! The values set in initialization are used for all the LTC6951 SPI/WRITE and
//! read functions (set_LTC6951_SPI_FIELD, get_LTC6951_SPI_FIELD,
//! LTC6951_read, LTC6951_write, etc, etc)
//! @return void
void LTC6951_init();

/* ------------------------------------------------------------------------- */
//! returns # of addresses in parts register map (array size)

//! @return # of addresses in parts register map


uint8_t get_LTC6951_REGSIZE();








/* ------------------------------------------------------------------------- */


//! returns the number of bits for a given field name in the SPI map


//! @return the number of bits for a given field name in the SPI map


uint8_t get_LTC6951_SPI_FIELD_NUMBITS(uint8_t f //!< SPI field number


                                     );








/* ------------------------------------------------------------------------- */


//! returns if the given field name is (0)read/write or (1)read_only field


//! @return if the given field is a (0)read/write or (1)read_only field


uint8_t get_LTC6951_SPI_FIELD_RW(uint8_t f   //!< SPI field number


                                ) ;





/* ------------------------------------------------------------------------- */


//! Read the ID string from the EEPROM and determine if any demo board is connected.


//! Returns 1 if successful, 0 if not successful


int8_t discover_demo_board_local(char *demo_name


                                );





#endif  // LTC6951_H



