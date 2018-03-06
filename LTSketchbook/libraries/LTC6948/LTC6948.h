/*!
 LTC6948: Ultralow Noise and Spurious 0.37GHz to 6.39GHz FracN Synthesizer with Integrated VCO

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

http://www.linear.com/product/LTC6948

http://www.linear.com/product/LTC6948#demoboards


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
    @ingroup LTC6948
    Header for LTC6948: Ultralow Noise and Spurious 0.37GHz to 6.39GHz FracN Synthesizer with Integrated VCO
*/

#ifndef LTC6948_H

#define LTC6948_H


//! Define the SPI CS pin
#ifndef LTC6948_CS
#define LTC6948_CS QUIKEVAL_CS  //! SPI Chip Select Pin
#endif


/*! @name LTC6948 Registers Fields in Alphabetical Order */
#define LTC6948_ALCCAL 1   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_ALCEN 2    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_ALCHI 3    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_ALCLO 4    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_ALCMON 5   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_ALCULOK 6  //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_AUTOCAL 7  //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_AUTORST 8  //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_BD 9       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_BST 10     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_CAL 11     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_CP 12      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_CPCHI 13   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_CPCLO 14   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_CPDN 15    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_CPINV 16   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_CPLE 17    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_CPMID 18   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_CPRST 19   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_CPUP 20    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_CPWIDE 21  //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_DITHEN 22  //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_FILT 23    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_INTN 24    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_LDOEN 25   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_LDOV 26    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_LKCT 27    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_LKWIN 28   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_LOCK 29    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_MTCAL 30   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_ND 31      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_NUM 32     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_OD 33      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_OMUTE 34   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_PART 35    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_PDALL 36   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_PDFN 37    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_PDOUT 38   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_PDPLL 39   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_PDVCO 40   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_POR 41     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_RD 42      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_REV 43     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_RFO 44     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_RSTFN 45   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_SEED 46    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_THI 47     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_TLO 48     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_UNLOK 49   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6948_x 50       //!<  for spi_map array, defines location for field specific information used to create the spi map


#define LTC6948_NUM_REGADDR 15  //!< Defines number of LTC6948 SPI registers, used in spi_map array
#define LTC6948_NUM_REGFIELD 50 //!< Defines number of LTC6948 SPI fields, used in spi_map array

#define ADDRx 0                 //!< used for 2nd dim of 2d spi_map array
#define DxMSB 1                 //!< used for 2nd dim of 2d spi_map array
#define NUMBITS 2               //!< used for 2nd dim of 2d spi_map array
#define R_ONLY 3                //!< used for 2nd dim of 2d spi_map array

#define OneMHz 1000000L         //!< 1MHz in long format, used in 64 bit math

#define LTC6948_1_MAXFREQ 3740  //!< LTC6948-1 upper freq limit
#define LTC6948_2_MAXFREQ 4910  //!< LTC6948-2 upper freq limit
#define LTC6948_3_MAXFREQ 5790  //!< LTC6948-3 upper freq limit
#define LTC6948_4_MAXFREQ 6390  //!< LTC6948-4 upper freq limit

#define LTC6948_1_MINFREQ 2240  //!< LTC6948-1 lower freq limit
#define LTC6948_2_MINFREQ 3080  //!< LTC6948-2 lower freq limit
#define LTC6948_3_MINFREQ 3840  //!< LTC6948-3 lower freq limit
#define LTC6948_4_MINFREQ 4200  //!< LTC6948-4 lower freq limit

#define LTC6948_MIN_REF_FREQ 10  //!< LTC6948 lower reference frequency limit
#define LTC6948_MAX_REF_FREQ 425 //!< LTC6948 upper reference frequency limit

//! @} */

/* ------------------------------------------------------------------------- */
//! LTC6948 Read Single Address
//!  reads 8 bit Data field to LTC6948.
//!  has to shift data by one bit to account for RW bit
//! @return data that was read from address
uint8_t LTC6948_read(uint8_t cs,    //!< Chip Select Pin
                     int8_t address //!< Register address for the LTC6948.
                    );


/* ------------------------------------------------------------------------- */
//! LTC6948 Read Single Field
//! For SPI FIELDS located in 1 or multiple address locations
//!  reads specific address locations
//!  identifies and returns specific field in question
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return data that was read from field
long LTC6948_read_field(uint8_t cs,       //!< Chip Select Pin
                        uint8_t address,  //!< Register address for the LTC6948.
                        uint8_t MSB_loc,  //!< MSB bit location of field
                        uint8_t numbits   //!< length of field (i.e. number of bits in field)
                       );


/* ------------------------------------------------------------------------- */
//! Gets the LTC6948 SPI field value
//! calls function LTC6948_read_field, which
//!  reads specific address locations
//!  identifies and returns specific field in question
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return data that was read from field
long get_LTC6948_SPI_FIELD(uint8_t cs,          //!< Chip Select Pin
                           uint8_t f            //!< SPI field number
                          );


/* ------------------------------------------------------------------------- */
//! LTC6948 Write Single Address
//!  writes 8 bit Data field to LTC6948.
//!  has to shift data by one bit to account for RW bit
//! @return void
void LTC6948_write(uint8_t cs,                  //!< Chip Select Pin
                   uint8_t address,             //!< Register address for the LTC6948.
                   uint8_t Data                 //!< 8-bit data to be written to register
                  );


/* ------------------------------------------------------------------------- */
//! LTC6948 Write Single Field
//!  For SPI FIELDS in 1 or multiple address locations
//!  reads specific address/field location then writes to specific field
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return void
uint8_t LTC6948_write_field(uint8_t cs,        //!< Chip Select Pin
                            long field_data,   //!< Value field value to be set to
                            uint8_t address,   //!< Register address for the LTC6948.
                            uint8_t MSB_loc,   //!< MSB bit location of field
                            uint8_t numbits   //!< length of field (i.e. number of bits in field)
                           );


/* ------------------------------------------------------------------------- */
//! Sets the LTC6948 SPI field value
//! calls function LTC6948_read_field, which
//!  reads specific address/field location then writes to specific field
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return void
void set_LTC6948_SPI_FIELD(uint8_t cs,          //!< Chip Select Pin
                           uint8_t f,           //!< SPI field number
                           long field_data      //!< Value field value to be set to
                          );


/* ------------------------------------------------------------------------- */
//! Writes values to ALL LTC6948 RW address
//! @return void
void set_LTC6948_ALLREGS(uint8_t cs,            //!< Chip Select Pin
                         uint8_t reg01,         //!< LTC6948 register 1
                         uint8_t reg02,         //!< LTC6948 register 2
                         uint8_t reg03,         //!< LTC6948 register 3
                         uint8_t reg04,         //!< LTC6948 register 4
                         uint8_t reg05,         //!< LTC6948 register 5
                         uint8_t reg06,         //!< LTC6948 register 6
                         uint8_t reg07,         //!< LTC6948 register 7
                         uint8_t reg08,         //!< LTC6948 register 8
                         uint8_t reg09,         //!< LTC6948 register 9
                         uint8_t reg0A,         //!< LTC6948 register 10
                         uint8_t reg0B,         //!< LTC6948 register 11
                         uint8_t reg0C,         //!< LTC6948 register 12
                         uint8_t reg0D          //!< LTC6948 register 13
                        );

/* ------------------------------------------------------------------------- */
//! Initializes the SPI MAP arrays
//! The values set in initialization are used all the LTC6948 SPI/WRITE and
//! read functions (set_LTC6948_SPI_FIELD, get_LTC6948_SPI_FIELD,
//! LTC6948_read, LTC6948_write, etc, etc)
//! @return void
void LTC6948_init();


/* ------------------------------------------------------------------------- */
//! returns # of addresses in parts register map (array size)
//! @return # of addresses in parts register map
uint8_t get_LTC6948_REGSIZE();


/* ------------------------------------------------------------------------- */
//! returns the number of bits for a given field name in the SPI map
//! @return the number of bits for a given field name in the SPI map
uint8_t get_LTC6948_SPI_FIELD_NUMBITS(uint8_t f //!< SPI field number
                                     );


/* ------------------------------------------------------------------------- */
//! returns if the given field name is (0)read/write or (1)read_only field
//! @return if the given field is a (0)read/write or (1)read_only field
uint8_t get_LTC6948_SPI_FIELD_RW(uint8_t f   //!< SPI field number
                                ) ;


/* ------------------------------------------------------------------------- */
//! calculates the output divider setting based on the frf and version of LTC6948
//! @return odiv = 1-6 divider setting for valid frequency, or 999 for invalid frequency
unsigned long LTC6948_calc_odiv(char part_version[],    //!< LTC6948 part number LTC6948-x
                                unsigned long frf[2]    //!< output frequency
                               );

/* ------------------------------------------------------------------------- */
//! FUNCTION: LTC6948_set_frf
//!  Calculates the integer (N), fractional (NUM) and output divider (OD) SPI values
//!  using self created 64bit math functions.
//!
//! Datasheet equations:
//! - fvco = fpfd*(N + F)
//! - frf  = fvco/O
//! - fpfd = fref/R
//!    - F=NUM/2^18 = NUM/262144
//!
//!    can be modified to the following equations:
//!    - N   = (int) (fvco/fpfd)  = (int) frf*O*R/fref
//!    - NUM = (int) [262144 * (frf*O*R/fref -N)]
//!
//!    where:
//!    - N = ndiv, O= odiv, NUM=fracN_num  in the code below
//!
//!    Linduino One (Arduino Uno) is limited to 32 bit floats/double/long.
//!    32 bit math functions will create rounding errors with the above equations,
//!    that can result in frequency errors.
//!    Therefore, the following code uses self created 64bit functions for 64bit integer math.
//!
//!    - frf (33 bits) LTC6948-4 max frf/fvco = 6.4GHZ, which is 33 bit number (2^33 = 8.59G)
//!    - fref (23 bits) LTC6948 min fref = 10MHz, which is a 23 bit number (2^23 = 8.3M)
//!    - O   (3 bits)
//! - R   (5 bits)
//!
//!    step 1: create 64 bit frf and fref numbers
//!
//!    step 2: calculate O (output divider)
//!
//!    step 3: get current R-divider setting
//!
//!    step 4: calculate frf*O*R
//!               max bit count/resolution: 33b+3b+5b= 41b
//!
//!    step 5: calculate N(10b), using value from step 1
//!               N = (int) frf*O*R/fref
//!               max bit count/resolution: 41b-23b = 18b
//!
//!    step 6: calculate NUM (18b)
//! - NUM = (int) [262144 * (frf*O*R/fref -N)]
//! - NUM = (int) [2^18   * (frf*O*R/fref -N)]
//!
//!    to get the correct value the above calculation are performed like this
//! - NUM =((int)((frf*O*R/fref)<<23) - N<<23 + rounding) >> 5,  where << & >> are  bit shift left, right
//! - note 1: << 23b was chosen, because step one use 41b,  41b +23b = 64b (max number for 64 bit math)
//!    - note 2: >> 5 was chosen because 23-5 = 2^18, which is the same as multiplying by 262144
//!    - note 3: the rounding variable is equivalent to adding 0.5 to the NUM number.  the integer operation always
//!    rounds down, so adding a 0.5 to the number before rounding down ensures the closest value is selected.
//!    In this function the lower 5 bits of  (frf*O*R/fref)<<23) - N<<23 (possible variable 0-31)
//!    are check to see if they are >=16.
//!    If they are then 16 is added to this number before performing the int and >>5 functions.
//! @return void
void LTC6948_set_frf(char part_version[]  //!< LTC6948 part number LTC6948-x
                    );


/* ------------------------------------------------------------------------- */
//! sets globals LTC6948_Fref_MHz and LTC6948_Fref_Hz
//! @return void
void set_LTC6948_global_fref(unsigned long fref_MHz, unsigned long fref_Hz);


/* ------------------------------------------------------------------------- */
//! sets globals LTC6948_Frf_MHz and LTC6948_Frf_Hz
//! @return void
void set_LTC6948_global_frf(unsigned long frf_MHz, unsigned long frf_Hz);


/* ------------------------------------------------------------------------- */
//! returns global LTC6948_Fref_MHz
//! @return LTC6948_Fref_MHz
unsigned long get_LTC6948_global_fref_MHz();


/* ------------------------------------------------------------------------- */
//! returns global LTC6948_Fref_Hz
//! @return LTC6948_Fref_Hz
unsigned long get_LTC6948_global_fref_Hz();


/* ------------------------------------------------------------------------- */
//! returns global LTC6948_Frf_MHz
//! @return LTC6948_Frf_MHz
unsigned long get_LTC6948_global_frf_MHz();


/* ------------------------------------------------------------------------- */
//! returns global LTC6948_Frf_Hz
//! @return LTC6948_Frf_Hz
unsigned long get_LTC6948_global_frf_Hz();


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

#endif  // LTC6948_H
