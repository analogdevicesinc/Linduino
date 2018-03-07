/*!
LTC6946: Ultralow Noise and Spurious 0.37GHz to 6.39GHz Integer-N Synthesizer with Integrated VCO

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

http://www.linear.com/product/LTC6946

http://www.linear.com/product/LTC6946#demoboards


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
    @ingroup LTC6946
    Header for LTC6946: Ultralow Noise and Spurious 0.37GHz to 6.39GHz IntegerN Synthesizer with Integrated VCO
*/

#ifndef LTC6946_H

#define LTC6946_H


//! Define the SPI CS pin
#ifndef LTC6946_CS
#define LTC6946_CS QUIKEVAL_CS  //! SPI Chip Select Pin
#endif


/*! @name LTC6946 Registers Fields in Alphabetical Order */
#define LTC6946_ALCCAL 1   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_ALCEN 2   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_ALCHI 3   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_ALCLO 4   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_ALCMON 5   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_ALCULOK 6   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_BD 7   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_BST 8   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_CAL 9   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_CP 10   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_CPCHI 11   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_CPCLO 12   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_CPDN 13   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_CPINV 14   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_CPMID 15   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_CPRST 16   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_CPUP 17   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_CPWIDE 18   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_FILT 19   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_LKCT 20   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_LKEN 21   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_LKWIN 22   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_LOCK 23   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_MTCAL 24   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_ND 25   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_OD 26   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_OMUTE 27   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_PART 28   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_PDALL 29   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_PDOUT 30   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_PDPLL 31   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_PDREFO 32   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_PDVCO 33   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_POR 34   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_RD 35   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_REV 36   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_RFO 37   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_THI 38   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_TLO 39   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_UNLOK 40   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6946_x 41   //!<  for spi_map array, defines location for field specific information used to create the spi map


#define LTC6946_NUM_REGADDR 12  //!< Defines number of LTC6946 SPI registers, used in spi_map array
#define LTC6946_NUM_REGFIELD 41 //!< Defines number of LTC6946 SPI fields, used in spi_map array

#define ADDRx 0                 //!< used for 2nd dim of 2d spi_map array
#define DxMSB 1                 //!< used for 2nd dim of 2d spi_map array
#define NUMBITS 2               //!< used for 2nd dim of 2d spi_map array
#define R_ONLY 3                //!< used for 2nd dim of 2d spi_map array

#define OneMHz 1000000L         //!< 1MHz in long format, used in 64 bit math

#define LTC6946_1_MAXFREQ 3740  //!< LTC6946-1 upper freq limit
#define LTC6946_2_MAXFREQ 4910  //!< LTC6946-2 upper freq limit
#define LTC6946_3_MAXFREQ 5790  //!< LTC6946-3 upper freq limit
#define LTC6946_4_MAXFREQ 6390  //!< LTC6946-4 upper freq limit

#define LTC6946_1_MINFREQ 2240  //!< LTC6946-1 lower freq limit
#define LTC6946_2_MINFREQ 3080  //!< LTC6946-2 lower freq limit
#define LTC6946_3_MINFREQ 3840  //!< LTC6946-3 lower freq limit
#define LTC6946_4_MINFREQ 4200  //!< LTC6946-4 lower freq limit

#define LTC6946_MIN_REF_FREQ 10  //!< LTC6946 lower reference frequency limit
#define LTC6946_MAX_REF_FREQ 250 //!< LTC6946 upper reference frequency limit

//! @} */

/* ------------------------------------------------------------------------- */
//! LTC6946 Read Single Address
//!  reads 8 bit Data field to LTC6946.
//!  has to shift data by one bit to account for RW bit
//! @return data that was read from address
uint8_t LTC6946_read(uint8_t cs,    //!< Chip Select Pin
                     int8_t address //!< Register address for the LTC6946.
                    );


/* ------------------------------------------------------------------------- */
//! LTC6946 Read Single Field
//! For SPI FIELDS located in 1 or multiple address locations
//!  reads specific address locations
//!  identifies and returns specific field in question
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return data that was read from field
long LTC6946_read_field(uint8_t cs,       //!< Chip Select Pin
                        uint8_t address,  //!< Register address for the LTC6946.
                        uint8_t MSB_loc,  //!< MSB bit location of field
                        uint8_t numbits   //!< length of field (i.e. number of bits in field)
                       );


/* ------------------------------------------------------------------------- */
//! Gets the LTC6946 SPI field value
//! calls function LTC6946_read_field, which
//!  reads specific address locations
//!  identifies and returns specific field in question
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return data that was read from field
long get_LTC6946_SPI_FIELD(uint8_t cs,          //!< Chip Select Pin
                           uint8_t f            //!< SPI field number
                          );


/* ------------------------------------------------------------------------- */
//! LTC6946 Write Single Address
//!  writes 8 bit Data field to LTC6946.
//!  has to shift data by one bit to account for RW bit
//! @return void
void LTC6946_write(uint8_t cs,                  //!< Chip Select Pin
                   uint8_t address,             //!< Register address for the LTC6946.
                   uint8_t Data                 //!< 8-bit data to be written to register
                  );


/* ------------------------------------------------------------------------- */
//! LTC6946 Write Single Field
//!  For SPI FIELDS in 1 or multiple address locations
//!  reads specific address/field location then writes to specific field
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return void
uint8_t LTC6946_write_field(uint8_t cs,        //!< Chip Select Pin
                            long field_data,   //!< Value field value to be set to
                            uint8_t address,   //!< Register address for the LTC6946.
                            uint8_t MSB_loc,   //!< MSB bit location of field
                            uint8_t numbits   //!< length of field (i.e. number of bits in field)
                           );


/* ------------------------------------------------------------------------- */
//! Sets the LTC6946 SPI field value
//! calls function LTC6946_read_field, which
//!  reads specific address/field location then writes to specific field
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return void
void set_LTC6946_SPI_FIELD(uint8_t cs,          //!< Chip Select Pin
                           uint8_t f,           //!< SPI field number
                           long field_data      //!< Value field value to be set to
                          );


/* ------------------------------------------------------------------------- */
//! Writes values to ALL LTC6946 RW address
//! @return void
void set_LTC6946_ALLREGS(uint8_t cs,            //!< Chip Select Pin
                         uint8_t reg01,         //!< LTC6946 register 1
                         uint8_t reg02,         //!< LTC6946 register 2
                         uint8_t reg03,         //!< LTC6946 register 3
                         uint8_t reg04,         //!< LTC6946 register 4
                         uint8_t reg05,         //!< LTC6946 register 5
                         uint8_t reg06,         //!< LTC6946 register 6
                         uint8_t reg07,         //!< LTC6946 register 7
                         uint8_t reg08,         //!< LTC6946 register 8
                         uint8_t reg09,         //!< LTC6946 register 9
                         uint8_t reg0A          //!< LTC6946 register 10
                        );

/* ------------------------------------------------------------------------- */
//! Initializes the SPI MAP arrays
//! The values set in initialization are used all the LTC6946 SPI/WRITE and
//! read functions (set_LTC6946_SPI_FIELD, get_LTC6946_SPI_FIELD,
//! LTC6946_read, LTC6946_write, etc, etc)
//! @return void
void LTC6946_init();


/* ------------------------------------------------------------------------- */
//! returns # of addresses in parts register map (array size)
//! @return # of addresses in parts register map
uint8_t get_LTC6946_REGSIZE();


/* ------------------------------------------------------------------------- */
//! returns the number of bits for a given field name in the SPI map
//! @return the number of bits for a given field name in the SPI map
uint8_t get_LTC6946_SPI_FIELD_NUMBITS(uint8_t f //!< SPI field number
                                     );


/* ------------------------------------------------------------------------- */
//! returns if the given field name is (0)read/write or (1)read_only field
//! @return if the given field is a (0)read/write or (1)read_only field
uint8_t get_LTC6946_SPI_FIELD_RW(uint8_t f   //!< SPI field number
                                ) ;


/* ------------------------------------------------------------------------- */
//! calculates the output divider setting based on the frf and version of LTC6946
//! @return odiv = 1-6 divider setting for valid frequency, or 999 for invalid frequency
unsigned long LTC6946_calc_odiv(char part_version[],    //!< LTC6946 part number LTC6946-x
                                unsigned long frf[2]    //!< output frequency
                               );

/* ------------------------------------------------------------------------- */
//! FUNCTION: LTC6946_set_frf
//!  Calculates the integer (N), fractional (NUM) and output divider (OD) SPI values
//!  using self created 64bit math functions.
//!
//! Datasheet equations:
//! - fvco = fpfd*(N + F)
//! - frf  = fvco/O
//! - fpfd = fref/R
//!
//!    can be modified to the following equations:
//!    - N   = (int) (fvco/fpfd)  = (int) frf*O*R/fref
//!
//!    where:
//!    - N = ndiv, O= odiv  in the code below
//!
//!    Linduino One (Arduino Uno) is limited to 32 bit floats/double/long.
//!    32 bit math functions will create rounding errors with the above equations,
//!    that can result in frequency errors.
//!    Therefore, the following code uses self created 64bit functions for 64bit integer math.
//!
//!    - frf (33 bits) LTC6946-4 max frf/fvco = 6.4GHZ, which is 33 bit number (2^33 = 8.59G)
//!    - fref (23 bits) LTC6946 min fref = 10MHz, which is a 23 bit number (2^23 = 8.3M)
//!    - O   (3 bits)
//!    - R   (10 bits)
//!
//!    step 1: create 64 bit frf and fref numbers
//!
//!    step 2: calculate O (output divider)
//!
//!    step 3: get current R-divider setting
//!
//!    step 4: calculate frf*O*R
//!               max bit count/resolution: 33b+3b+10b= 46b
//!
//!    step 5: calculate N(16b), using value from step 1
//!    - N = (int) frf*O*R/fref
//!    - max bit count/resolution: 46b-23b = 23b
//! @return void
void LTC6946_set_frf(char part_version[]  //!< LTC6946 part number LTC6946-x
                    );


/* ------------------------------------------------------------------------- */
//! sets globals LTC6946_Fref_MHz and LTC6946_Fref_Hz
//! @return void
void set_LTC6946_global_fref(unsigned long fref_MHz, unsigned long fref_Hz);


/* ------------------------------------------------------------------------- */
//! sets globals LTC6946_Frf_MHz and LTC6946_Frf_Hz
//! @return void
void set_LTC6946_global_frf(unsigned long frf_MHz, unsigned long frf_Hz);


/* ------------------------------------------------------------------------- */
//! returns global LTC6946_Fref_MHz
//! @return LTC6946_Fref_MHz
unsigned long get_LTC6946_global_fref_MHz();


/* ------------------------------------------------------------------------- */
//! returns global LTC6946_Fref_Hz
//! @return LTC6946_Fref_Hz
unsigned long get_LTC6946_global_fref_Hz();


/* ------------------------------------------------------------------------- */
//! returns global LTC6946_Frf_MHz
//! @return LTC6946_Frf_MHz
unsigned long get_LTC6946_global_frf_MHz();


/* ------------------------------------------------------------------------- */
//! returns global LTC6946_Frf_Hz
//! @return LTC6946_Frf_Hz
unsigned long get_LTC6946_global_frf_Hz();


/* ------------------------------------------------------------------------- */
//!   create a 64 bit Hz number from
//!   32 bit xxxx MHz number and 32 bit yyy yyy Hz number.
//!   A) if an < 2^32 bits:
//!      - an(upper 32b) = 0;
//!      - an(lower 32b) = MHzPart(32b)*1MHz + HzPart (32b)
//!
//!   B) if an > 2^32 bits (4,294,967,296):
//!     - an(upper 32b) = 1
//!     - an(lower 32b) = ((MHzPart-4294)*1MHz+HzPart)-967296
//! @return void
void HZto64(unsigned long  an[],            //!< 64 bit number, 1x2 32 bit array
            unsigned long MHzPart,          //!< integer in MHZ
            unsigned long HzPart            //!< integer in Hz
           );


/* ------------------------------------------------------------------------- */
//! Creates a equivalent 64 bit number from 2 32 bit numbers
//! - an[0]=bigPart    //upper 32 bits
//! - an[1]=littlePart //lower 32 bits
//! @return void
void init64(unsigned long an[],        //!< 64 bit number, 1x2 32 bit array
            unsigned long bigPart,     //!< upper 32 bits
            unsigned long littlePart   //!< lower 32 bits
           );

/* ------------------------------------------------------------------------- */
//! Single Bit shift left of equivalent 64 bit number (an[] = an[]<<1)
//! @return void
void shl64(unsigned long  an[]         //!<  an[] = an[]<<1
          );

/* ------------------------------------------------------------------------- */
//! Multi Bit shift left of equivalent 64 bit number (an[] = an[]<<shiftnum)
//! @return void
void shl64by(unsigned long  an[],     //!<  an[] = an[]<<shiftnum
             uint8_t shiftnum         //!<  number of bits to shift left
            );

/* ------------------------------------------------------------------------- */
//! Single Bit shift right of equivalent 64 bit number (an[] = an[]<<1)
//! @return void
void shr64(unsigned long  an[]        //!<  an[] = an[]>>shiftnum
          );

/* ------------------------------------------------------------------------- */
//! Multi Bit shift right of equivalent 64 bit number (an[] = an[]>>shiftnum)
//! @return void
void shr64by(unsigned long  an[],     //!<  an[] = an[]>>shiftnum
             uint8_t shiftnum         //!<  number of bits to shift right
            );

/* ------------------------------------------------------------------------- */
//! 64 bit Add ann to an (an[] = an[] + ann[])
//! @return void
void add64(unsigned long  an[],      //!<  64 bit number, in 1x2 32bit array
           unsigned long  ann[]      //!<  64 bit number, in 1x2 32bit array
          );

/* ------------------------------------------------------------------------- */
//! 64 bit Subtract ann from an (an[] = an[] - ann[])
//! @return void
void sub64(unsigned long  an[],      //!<  64 bit number, in 1x2 32bit array
           unsigned long  ann[]      //!<  64 bit number, in 1x2 32bit array
          );

/* ------------------------------------------------------------------------- */
//! 64 bit, if an == ann, then true
//! @return true, if an==ann; false, if an<>ann
boolean eq64(unsigned long  an[],    //!<  64 bit number, in 1x2 32bit array
             unsigned long  ann[]    //!<  64 bit number, in 1x2 32bit array
            );

/* ------------------------------------------------------------------------- */
//! 64 bit, if an < ann, then true
//! @return true, if an<ann; false, if an=>ann
boolean lt64(unsigned long  an[],     //!<  64 bit number, in 1x2 32bit array
             unsigned long  ann[]     //!<  64 bit number, in 1x2 32bit array
            );

/* ------------------------------------------------------------------------- */
//! 64 bit Divide,   num=num/div
//! @return void
void div64(unsigned long num[],    //!<  numerator: 64 bit number, in 1x2 32bit array
           unsigned long den[]     //!<  denominator: 64 bit number, in 1x2 32bit array
          );


/* ------------------------------------------------------------------------- */
//! 64 bit multiply,   an=an*ann
//! @return void
void mul64(unsigned long an[],     //!<  64 bit number, in 1x2 32bit array
           unsigned long ann[]     //!<  64 bit number, in 1x2 32bit array
          );

/* ------------------------------------------------------------------------- */
//! Prints HEX representation of 64 bit an
//! @return void
void prt(unsigned long  an[]       //!<  64 bit number, in 1x2 32bit array
        );

#endif  // LTC6946_H
