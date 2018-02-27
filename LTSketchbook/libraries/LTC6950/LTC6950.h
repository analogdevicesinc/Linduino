/*!
 LTC6950: 1.4GHz Low Phase Noise, Low Jitter PLL with Clock Distribution

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

http://www.linear.com/product/LTC6950

http://www.linear.com/product/LTC6950#demoboards


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
    @ingroup LTC6950
    Header for LTC6950: 1.4GHz Low Phase Noise, Low Jitter PLL with Clock Distribution
*/

#ifndef LTC6950_H

#define LTC6950_H


//! Define the SPI CS pin
#ifndef LTC6950_CS
#define LTC6950_CS QUIKEVAL_CS  //! SPI Chip Select Pin
#endif


/*! @name LTC6950 Registers Fields in Alphabetical Order */
#define LTC6950_CMSINV 1      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_CP 2          //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_CPCHI 3       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_CPCLO 4       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_CPDN 5        //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_CPINV 6       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_CPMID 7       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_CPRST 8       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_CPUP 9        //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_CPWIDE 10     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_DEL0 11       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_DEL1 12       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_DEL2 13       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_DEL3 14       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_DEL4 15       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_FILTR 16      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_FILTV 17      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_FLDRV0 18     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_FLDRV1 19     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_FLDRV2 20     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_FLDRV3 21     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_FLDRV4 22     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_IBIAS0 23     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_IBIAS1 24     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_IBIAS2 25     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_IBIAS3 26     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_INV_ST1 27    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_INV_ST2 28    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_LKCT 29       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_LKEN 30       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_LKWIN 31      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_LOCK 32       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_LVCMS 33      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_M0 34         //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_M1 35         //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_M2 36         //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_M3 37         //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_M4 38         //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_N 39          //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_NO_REF 40     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_NO_VCO 41     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_PART 42       //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_PDALL 43      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_PD_DIV0 44    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_PD_DIV1 45    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_PD_DIV2 46    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_PD_DIV3 47    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_PD_DIV4 48    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_PD_OUT0 49    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_PD_OUT1 50    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_PD_OUT2 51    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_PD_OUT3 52    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_PD_OUT4 53    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_PDPLL 54      //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_PDREFAC 55    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_PDVCOAC 56    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_R 57          //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_RDIVOUT 58    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_RES6950 59    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_RESET_R 60    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_RESET_N 61    //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_REV 62        //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_SM1 63        //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_SM2 64        //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_SYNCMD 65     //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_SYNC_EN0 66   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_SYNC_EN1 67   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_SYNC_EN2 68   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_SYNC_EN3 69   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_SYNC_EN4 70   //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_THI 71        //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_TLO 72        //!<  for spi_map array, defines location for field specific information used to create the spi map
#define LTC6950_UNLOCK 73     //!<  for spi_map array, defines location for field specific information used to create the spi map


#define LTC6950_NUM_REGADDR 23  //!< Defines number of LTC6950 SPI registers, used in spi_map array
#define LTC6950_NUM_REGFIELD 73 //!< Defines number of LTC6950 SPI fields, used in spi_map array

#define ADDRx 0                 //!< used for 2nd dim of 2d spi_map array
#define DxMSB 1                 //!< used for 2nd dim of 2d spi_map array
#define NUMBITS 2               //!< used for 2nd dim of 2d spi_map array
#define R_ONLY 3                //!< used for 2nd dim of 2d spi_map array

#define OneMHz 1000000L         //!< 1MHz in long format, used in 64 bit math

#define LTC6950_MINFREQ 0     //!< LTC6950 lower freq limit
#define LTC6950_MAXFREQ 1400  //!< LTC6950 upper freq limit

#define LTC6950_MIN_REF_FREQ 2   //!< LTC6950 lower reference frequency limit
#define LTC6950_MAX_REF_FREQ 250 //!< LTC6950 upper reference frequency limit

//! @} */

/* ------------------------------------------------------------------------- */
//! LTC6950 Read Single Address
//!  reads 8 bit Data field to LTC6950.
//!  has to shift data by one bit to account for RW bit
//! @return data that was read from address
uint8_t LTC6950_read(uint8_t cs,    //!< Chip Select Pin
                     int8_t address //!< Register address for the LTC6950.
                    );


/* ------------------------------------------------------------------------- */
//! LTC6950 Read Single Field
//! For SPI FIELDS located in 1 or multiple address location
//!  reads specific address locations
//!  identifies and returns specific field in question
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return data that was read from field
long LTC6950_read_field(uint8_t cs,       //!< Chip Select Pin
                        uint8_t address,  //!< Register address for the LTC6950.
                        uint8_t MSB_loc,  //!< MSB bit location of field
                        uint8_t numbits   //!< length of field (i.e. number of bits in field)
                       );


/* ------------------------------------------------------------------------- */
//! Gets the LTC6950 SPI field value
//! calls function LTC6950_read_field, which
//!  reads specific address locations
//!  identifies and returns specific field in question
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return data that was read from field
long get_LTC6950_SPI_FIELD(uint8_t cs,          //!< Chip Select Pin
                           uint8_t f            //!< SPI field number
                          );


/* ------------------------------------------------------------------------- */
//! LTC6950 Write Single Address
//!  writes 8 bit Data field to LTC6950.
//!  has to shift data by one bit to account for RW bit
//! @return void
void LTC6950_write(uint8_t cs,                  //!< Chip Select Pin
                   uint8_t address,             //!< Register address for the LTC6950.
                   uint8_t Data                 //!< 8-bit data to be written to register
                  );


/* ------------------------------------------------------------------------- */
//! LTC6950 Write Single Field
//!  For SPI FIELDS in 1 or multiple address locations
//!  reads specific address/field location then writes to specific field
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return void
uint8_t LTC6950_write_field(uint8_t cs,        //!< Chip Select Pin
                            long field_data,   //!< Value field value to be set to
                            uint8_t address,   //!< Register address for the LTC6950.
                            uint8_t MSB_loc,   //!< MSB bit location of field
                            uint8_t numbits   //!< length of field (i.e. number of bits in field)
                           );


/* ------------------------------------------------------------------------- */
//! Sets the LTC6950 SPI field value
//! calls function LTC6950_read_field, which
//!  reads specific address/field location then writes to specific field
//!  can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
//! @return void
void set_LTC6950_SPI_FIELD(uint8_t cs,          //!< Chip Select Pin
                           uint8_t f,           //!< SPI field number
                           long field_data      //!< Value field value to be set to
                          );


/* ------------------------------------------------------------------------- */
//! Writes values to ALL LTC6950 RW addresses
//! @return void
void set_LTC6950_ALLREGS(uint8_t cs,            //!< Chip Select Pin
                         uint8_t reg01,         //!< LTC6950 register 1
                         uint8_t reg02,         //!< LTC6950 register 2
                         uint8_t reg03,         //!< LTC6950 register 3
                         uint8_t reg04,         //!< LTC6950 register 4
                         uint8_t reg05,         //!< LTC6950 register 5
                         uint8_t reg06,         //!< LTC6950 register 6
                         uint8_t reg07,         //!< LTC6950 register 7
                         uint8_t reg08,         //!< LTC6950 register 8
                         uint8_t reg09,         //!< LTC6950 register 9
                         uint8_t reg0A,         //!< LTC6950 register 10
                         uint8_t reg0B,         //!< LTC6950 register 11
                         uint8_t reg0C,         //!< LTC6950 register 12
                         uint8_t reg0D,         //!< LTC6950 register 13
                         uint8_t reg0E,         //!< LTC6950 register 14
                         uint8_t reg0F,         //!< LTC6950 register 15
                         uint8_t reg10,         //!< LTC6950 register 16
                         uint8_t reg11,         //!< LTC6950 register 17
                         uint8_t reg12,         //!< LTC6950 register 18
                         uint8_t reg13,         //!< LTC6950 register 19
                         uint8_t reg14,         //!< LTC6950 register 20
                         uint8_t reg15          //!< LTC6950 register 21
                        );

/* ------------------------------------------------------------------------- */
//! Initializes the SPI MAP arrays
//! The values set in initialization are used for all the LTC6950 SPI/WRITE and
//! read functions (set_LTC6950_SPI_FIELD, get_LTC6950_SPI_FIELD,
//! LTC6950_read, LTC6950_write, etc, etc)
//! @return void
void LTC6950_init();


/* ------------------------------------------------------------------------- */
//! returns # of addresses in parts register map (array size)
//! @return # of addresses in parts register map
uint8_t get_LTC6950_REGSIZE();


/* ------------------------------------------------------------------------- */
//! returns the number of bits for a given field name in the SPI map
//! @return the number of bits for a given field name in the SPI map
uint8_t get_LTC6950_SPI_FIELD_NUMBITS(uint8_t f //!< SPI field number
                                     );


/* ------------------------------------------------------------------------- */
//! returns if the given field name is (0)read/write or (1)read_only field
//! @return if the given field is a (0)read/write or (1)read_only field
uint8_t get_LTC6950_SPI_FIELD_RW(uint8_t f   //!< SPI field number
                                ) ;


/* ------------------------------------------------------------------------- */
//! calculates the output divider setting based on the frf and on board
//! VCO frequencies of LTC6950
//! @return odiv = 1-6 divider setting for valid frequency, or 999 for invalid odiv
unsigned long LTC6950_calc_odiv(unsigned long frf[2]    //!< output frequency
                               );

/* ------------------------------------------------------------------------- */
//!   FUNCTION: LTC6950_set_frf
//!   Calculates the integer (N) and output divider (OD) SPI values
//!   using self created 64bit math functions. For Out0 only.  Use menu 3 to set dividers to
//!   achieve desired frequencies on other outputs.
//!
//!  Datasheet equations
//!   - fvco = fpfd*(N + F)
//!   - frf  = fvco/O
//!   - fpfd = fref/R
//!
//!    can be modified to the following equations
//!    - N   = (int) (fvco/fpfd)  = (int) frf*O*R/fref
//!
//!    where
//!    - N = ndiv, O= M0div  in the code below
//!
//!    Linduino One (Arduino Uno) is limited to 32 bit floats/double/long.
//!    32 bit math functions will create rounding errors with the above equations,
//!    tha can result in frequency errors.
//!    Therefore, the following code uses self created 64bit functions for 64bit integer math.
//!
//!    - frf (33 bits) LTC6950 max frf/fvco = 1.4GHZ, which is 23 bit number (2^31 = 2.147G)
//!    - fref (23 bits) LTC6950 min fref = 2MHz, which is a 20 bit number (2^20 = 1.05M)
//!    - O   (6 bits)
//!    - R   (10 bits)
//!
//!    step 1: create 64 bit frf and fref numbers
//!
//!    step 2: calculate O (output divider)
//!
//!    step 3: get current R-divider setting
//!
//!    step 4: calculate frf*O*R
//!    - max bit count/resolution: 31b+6b+10b= 47b
//!
//!    step 5: calculate N(11b), using value from step 1
//!    - N = (int) frf*O*R/fref
//!    - max bit count/resolution: 47b-20b = 27b
//!
//!    step 6: find N for closest frequency - accounts for rounding with integer math
//!    - 64b-47b=17b (using 16b instead of 17b)
//!
//! @return void
void LTC6950_set_frf();


/* ------------------------------------------------------------------------- */
//! sets globals LTC6950_Fref_MHz and LTC6950_Fref_Hz
//! @return void
void set_LTC6950_global_fref(unsigned long fref_MHz, unsigned long fref_Hz);


/* ------------------------------------------------------------------------- */
//! sets globals LTC6950_Frf_MHz and LTC6950_Frf_Hz
//! @return void
void set_LTC6950_global_frf(unsigned long frf_MHz, unsigned long frf_Hz);

/* ------------------------------------------------------------------------- */
//! sets globals LTC6950_VCO_Max_Freq_MHz, LTC6950_VCO_Max_Freq_Hz, LTC6950_VCO_Min_Freq_MHz and LTC6950_VCO_Min_Freq_Hz
//! @return void
void set_LTC6950_global_vcolim(unsigned long fvco_max_MHz, unsigned long fvco_max_Hz, unsigned long fvco_min_MHz, unsigned long fvco_min_Hz);


/* ------------------------------------------------------------------------- */
//! returns global LTC6950_Fref_MHz
//! @return LTC6950_Fref_MHz
unsigned long get_LTC6950_global_fref_MHz();


/* ------------------------------------------------------------------------- */
//! returns global LTC6950_Fref_Hz
//! @return LTC6950_Fref_Hz
unsigned long get_LTC6950_global_fref_Hz();


/* ------------------------------------------------------------------------- */
//! returns global LTC6950_Frf_MHz
//! @return LTC6950_Frf_MHz
unsigned long get_LTC6950_global_frf_MHz();


/* ------------------------------------------------------------------------- */
//! returns global LTC6950_Frf_Hz
//! @return LTC6950_Frf_Hz
unsigned long get_LTC6950_global_frf_Hz();


/* ------------------------------------------------------------------------- */
//! returns global LTC6950_VCO_Max_Freq_MHz
//! @return LTC6950_VCO_Max_Freq_MHz
unsigned long get_LTC6950_global_VCO_MAX_MHz();


/* ------------------------------------------------------------------------- */
//! returns global LTC6950_VCO_Min_Freq_MHz
//! @return LTC6950_VCO_Min_Freq_MHz
unsigned long get_LTC6950_global_VCO_MIN_MHz();


/* ------------------------------------------------------------------------- */
//! returns global LTC6950_VCO_Max_Freq_Hz
//! @return LTC6950_VCO_Max_Freq_Hz
unsigned long get_LTC6950_global_VCO_MAX_Hz();


/* ------------------------------------------------------------------------- */
//! returns global LTC6950_VCO_Min_Freq_Hz
//! @return LTC6950_VCO_Min_Freq_Hz
unsigned long get_LTC6950_global_VCO_MIN_Hz();


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
//! - an[0]=bigPart    upper 32 bits
//! - an[1]=littlePart lower 32 bits
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

#endif  // LTC6950_H
