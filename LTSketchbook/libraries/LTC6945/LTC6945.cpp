/*!
    LTC6945: Ultralow Noise and Spurious 0.35GHz to 6GHz Integer-N Synthesizer

@verbatim

The LTC®6945 is a high performance, low noise, 6GHz phase-locked
loop (PLL), including a reference divider, phase-frequency
detector (PFD) with phase-lock indicator, charge
pump, integer feedback divider and VCO output divider.

The part features a buffered, programmable VCO output
divider with a range of 1 through 6. The differential, low
noise output buffer has user-programmable output power
ranging from –6dBm to 3dBm, and may be muted through
either a digital input pin or software.

The low noise reference buffer outputs a typical 0dBm
square wave directly into a 50Ω impedance from 10MHz
to 250MHz, or may be disabled through software.

The ultralow noise charge pump contains selectable high
and low voltage clamps useful for VCO monitoring, and
also may be set to provide a V+/2 bias.

All device settings are controlled through a SPI-compatible
serial port.

@endverbatim


http://www.linear.com/product/LTC6945

http://www.linear.com/product/LTC6945#demoboards


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

//! @ingroup RF_Timing
//! @{
//! @defgroup LTC6945 LTC6945: Ultralow Noise and Spurious 0.35GHz to 6GHz Integer-N Synthesizer
//! @}

/*! @file
    @ingroup LTC6945
    Library for LTC6945: Ultralow Noise and Spurious 0.35GHz to 6GHz Integer-N Synthesizer
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_SPI.h"
#include "LTC6945.h"
#include <SPI.h>

uint8_t LTC6945_reg[LTC6945_NUM_REGADDR];               //!< number of LTC6945 spi addresses
uint8_t LTC6945_spi_map[(LTC6945_NUM_REGFIELD+1)][4];   //!< LTC6945 spi map, stores MSB address location, MSB bit location, field length in bits, and R or RW capability

unsigned long LTC6945_Fref_MHz = 100;                   //!< Default Fref frequency - MHz portion (xxx);  Fref = xxx, yyy,yyy
unsigned long LTC6945_Fref_Hz = 0;                      //!< Default Fref frequency - Hz portion (yyy,yyy);  Fref = x,xxx, yyy,yyy
unsigned long LTC6945_Frf_MHz = 914;                    //!< Default Frf frequency - MHz portion (xxxx);  Frf = x,xxx, yyy,yyy
unsigned long LTC6945_Frf_Hz  = 0;                      //!< Default Frf frequency - Hz portion (yyy,yyy);  Frf = x,xxx, yyy,yyy
unsigned long LTC6945_VCO_Max_Freq_MHz = 928;           //!< Max Vco frequency for default on board VCO  - MHz portion (xxxx);  Fvco max = xxx, yyy,yyy
unsigned long LTC6945_VCO_Min_Freq_MHz = 902;           //!< Min Vco frequency for default on board VCO - MHz portion (xxxx);  Fvco min = x,xxx, yyy,yyy
unsigned long LTC6945_VCO_Max_Freq_Hz = 0;              //!< Max Vco frequency for default on board VCO  - Hz portion (yyy,yyy);  Fvco max = x,xxx, yyy,yyy
unsigned long LTC6945_VCO_Min_Freq_Hz = 0;              //!< Min Vco frequency for default on board VCO  - Hz portion (yyy,yyy);  Fvco min= x,xxx, yyy,yyy

unsigned long zero64[]= {0,0}; //!< for 64bit math functions

/* -------------------------------------------------------------------------
  FUNCTION: LTC6945_read
  - reads 8 bit Data field to LTC6945.
  - has to shift data by one bit to account for RW bit
 -------------------------------------------------------------------------- */
uint8_t LTC6945_read(uint8_t cs, int8_t address)
{
  int8_t address_shift;
  LT_union_int16_2bytes rx;

  address_shift =(address << 1) | 0x01; // shift to left to account for R/W bit, set bit high for read
  spi_transfer_word(cs, address_shift<<8 , &rx.LT_uint16);

  LTC6945_reg[address]=rx.LT_byte[0];
  return(rx.LT_byte[0]);
}


/* -------------------------------------------------------------------------
  FUNCTION: LTC6945_read_field
  For SPI FIELDS located in 1 or multiple address location
  - reads specific address locations
  - identifies and returns specific field in question
    - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
--------------------------------------------------------------------------- */
long LTC6945_read_field(uint8_t cs, uint8_t address, uint8_t MSB_loc, uint8_t numbits)
{
  int bit_shift, i, num_reg;
  long field_val, maskbits, pow2;

  num_reg=0;
  field_val=0;
// determines how many register are used
  do
  {
    bit_shift = (MSB_loc+1)- (numbits-num_reg*8);   // determines bit_shift for last register location
    field_val=LTC6945_read(cs, (address+num_reg))+(field_val<<8);  // reads current address locations, shifts previous address location 8 bits
    num_reg++;
  }
  while ((bit_shift<0) && (num_reg<4));

// creates a bit mask for complete word,
  maskbits = 1;
  pow2=1;
  for (i=1, maskbits=1; i<numbits; i++)
  {
    pow2=pow2*2;
    maskbits = maskbits+pow2;
  }

  field_val=(field_val >>bit_shift) &maskbits;
  return field_val;
}

/* -------------------------------------------------------------------------
  FUNCTION: get_LTC6945_SPI_FIELD
  For SPI FIELDS
  - reads specific address locations
  - identifies and returns specific field in question
    - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
--------------------------------------------------------------------------- */
long get_LTC6945_SPI_FIELD(uint8_t cs, uint8_t f)
{

  return LTC6945_read_field(cs, LTC6945_spi_map[f][ADDRx], LTC6945_spi_map[f][DxMSB], LTC6945_spi_map[f][NUMBITS]);
}

/* -------------------------------------------------------------------------
  FUNCTION: LTC6945_write
  - writes 8 bit Data field to LTC6945.
  - has to shift data by one bit to account for RW bit
--------------------------------------------------------------------------- */
void LTC6945_write(uint8_t cs, uint8_t address, uint8_t Data)
{
  LT_union_int16_2bytes rx;

  address=address << 1; // shift to left to account for R/W bit
  spi_transfer_word(cs, (address<<8) | Data, &rx.LT_uint16);
}


/* -------------------------------------------------------------------------
 FUNCTION: LTC6945_write_field
 For SPI FIELDS
 - reads specific address location
 - identifies and returns specific field in question
   - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
---------------------------------------------------------------------------- */
uint8_t LTC6945_write_field(uint8_t cs, long field_data, uint8_t address, uint8_t MSB_loc, uint8_t numbits)
{
  long current_content, desired_content, reg_val;
  int LSB_loc, i, j, num_reg, bit_shift;
  long temp_arr[32];

  for (i=0; i<32 ; i++) temp_arr[i]=0;  // init temp_arr

// read data in current address location and put in a bit array
  num_reg=0;
  current_content=0;
  do
  {
    bit_shift=(MSB_loc+1)-(numbits-num_reg*8);
    current_content=LTC6945_read(cs, (address+num_reg)) + (current_content<<8);

    num_reg++;
  }
  while ((bit_shift<0) && (num_reg<4));
  for (i=0; i<(8*num_reg); i++)
  {
    temp_arr[i]=(current_content>>i) & 1;
  }

// exchange current bits with desired bits
  LSB_loc = 8*(num_reg-1)+MSB_loc-numbits+1;
  for (i=LSB_loc, j=0; i<=(MSB_loc+(num_reg-1)*8); i++, j++)
  {
    temp_arr[i] = (field_data>>j) &1;
  } // end of for loop

// reconstruct bits into an integer
  desired_content = 0;
  for (i=0; i<(8*num_reg); i++)
  {
    desired_content = desired_content | (temp_arr[i]<<i);
  } // end of for loop

// write new field value to part
  for (i=0; i<num_reg; i++)
  {
    reg_val = (desired_content >> 8*(num_reg-1-i)) & 0xff;
    LTC6945_write(cs, (address+i), reg_val);
  } // end of for loop
} // end of LTC6945_write_field


/* -------------------------------------------------------------------------
   FUNCTION: get_LTC6945_REGSIZE
   - returns # of addresses in parts register map (array size)
---------------------------------------------------------------------------- */
uint8_t get_LTC6945_REGSIZE()
{
  return sizeof(LTC6945_reg);
}


/* -------------------------------------------------------------------------
   FUNCTION: get_LTC6945_SPI_FIELD_NUMBITS
   - returns the number of bits for a given field name in the SPI map
---------------------------------------------------------------------------- */
uint8_t get_LTC6945_SPI_FIELD_NUMBITS(uint8_t f)
{
  return LTC6945_spi_map[f][NUMBITS];
}


/* -------------------------------------------------------------------------
   FUNCTION: get_LTC6945_SPI_FIELD_RW
   - returns if the given field name is (0)read/write or (1)read_only field
---------------------------------------------------------------------------- */
uint8_t get_LTC6945_SPI_FIELD_RW(uint8_t f)
{
  return LTC6945_spi_map[f][R_ONLY];
}


/* -------------------------------------------------------------------------
   FUNCTION: set_LTC6945_SPI_FIELD
   For SPI FIELDS
   - reads specific address location
   - identifies and returns specific field in question
   - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
---------------------------------------------------------------------------- */
void set_LTC6945_SPI_FIELD(uint8_t cs, uint8_t f, long field_data)
{
  LTC6945_write_field(cs, field_data, LTC6945_spi_map[f][ADDRx], LTC6945_spi_map[f][DxMSB], LTC6945_spi_map[f][NUMBITS]);
}


/* -------------------------------------------------------------------------
   FUNCTION: set_LTC6945_ALLREGS
   - writes data to all registers at once
--------------------------------------------------------------------------- */
void set_LTC6945_ALLREGS(uint8_t cs, uint8_t reg01, uint8_t reg02, uint8_t reg03, uint8_t reg04, uint8_t reg05, uint8_t reg06, uint8_t reg07, uint8_t reg08, uint8_t reg09, uint8_t reg0A)
{
  uint8_t i;

  LTC6945_reg[1] = reg01;
  LTC6945_reg[2] = reg02;
  LTC6945_reg[3] = reg03;
  LTC6945_reg[4] = reg04;
  LTC6945_reg[5] = reg05;
  LTC6945_reg[6] = reg06;
  LTC6945_reg[7] = reg07;
  LTC6945_reg[8] = reg08;
  LTC6945_reg[9] = reg09;
  LTC6945_reg[10] = reg0A;

  for (i=1; i<11; i++)  LTC6945_write(cs, i, LTC6945_reg[i]);
} // end of set_LTC6945_ALLREGS


/* -------------------------------------------------------------------------
   FUNCTION: LTC6945_init
   - initializes the SPI MAP
   - for ease of programming there is spreadsheet that automates this some.
----------------------------------------------------------------------------*/
void LTC6945_init()
{

// spi map
  LTC6945_spi_map[LTC6945_BST][ADDRx]=0x08;
  LTC6945_spi_map[LTC6945_BST][DxMSB]= 7;
  LTC6945_spi_map[LTC6945_BST][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_CP][ADDRx]=0x09;
  LTC6945_spi_map[LTC6945_CP][DxMSB]= 3;
  LTC6945_spi_map[LTC6945_CP][NUMBITS]= 4;
  LTC6945_spi_map[LTC6945_CPCHI][ADDRx]=0x0a;
  LTC6945_spi_map[LTC6945_CPCHI][DxMSB]= 7;
  LTC6945_spi_map[LTC6945_CPCHI][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_CPCLO][ADDRx]=0x0a;
  LTC6945_spi_map[LTC6945_CPCLO][DxMSB]= 6;
  LTC6945_spi_map[LTC6945_CPCLO][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_CPDN][ADDRx]=0x0a;
  LTC6945_spi_map[LTC6945_CPDN][DxMSB]= 0;
  LTC6945_spi_map[LTC6945_CPDN][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_CPINV][ADDRx]=0x0a;
  LTC6945_spi_map[LTC6945_CPINV][DxMSB]= 4;
  LTC6945_spi_map[LTC6945_CPINV][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_CPMID][ADDRx]=0x0a;
  LTC6945_spi_map[LTC6945_CPMID][DxMSB]= 5;
  LTC6945_spi_map[LTC6945_CPMID][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_CPRST][ADDRx]=0x0a;
  LTC6945_spi_map[LTC6945_CPRST][DxMSB]= 2;
  LTC6945_spi_map[LTC6945_CPRST][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_CPUP][ADDRx]=0x0a;
  LTC6945_spi_map[LTC6945_CPUP][DxMSB]= 1;
  LTC6945_spi_map[LTC6945_CPUP][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_CPWIDE][ADDRx]=0x0a;
  LTC6945_spi_map[LTC6945_CPWIDE][DxMSB]= 3;
  LTC6945_spi_map[LTC6945_CPWIDE][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_FILT][ADDRx]=0x08;
  LTC6945_spi_map[LTC6945_FILT][DxMSB]= 6;
  LTC6945_spi_map[LTC6945_FILT][NUMBITS]= 2;
  LTC6945_spi_map[LTC6945_LKCT][ADDRx]=0x09;
  LTC6945_spi_map[LTC6945_LKCT][DxMSB]= 5;
  LTC6945_spi_map[LTC6945_LKCT][NUMBITS]= 2;
  LTC6945_spi_map[LTC6945_LKEN][ADDRx]=0x07;
  LTC6945_spi_map[LTC6945_LKEN][DxMSB]= 0;
  LTC6945_spi_map[LTC6945_LKEN][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_LKWIN][ADDRx]=0x09;
  LTC6945_spi_map[LTC6945_LKWIN][DxMSB]= 7;
  LTC6945_spi_map[LTC6945_LKWIN][NUMBITS]= 2;
  LTC6945_spi_map[LTC6945_LOCK][ADDRx]=0x00;
  LTC6945_spi_map[LTC6945_LOCK][DxMSB]= 2;
  LTC6945_spi_map[LTC6945_LOCK][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_ND][ADDRx]=0x05;
  LTC6945_spi_map[LTC6945_ND][DxMSB]= 7;
  LTC6945_spi_map[LTC6945_ND][NUMBITS]= 16;
  LTC6945_spi_map[LTC6945_OD][ADDRx]=0x08;
  LTC6945_spi_map[LTC6945_OD][DxMSB]= 2;
  LTC6945_spi_map[LTC6945_OD][NUMBITS]= 3;
  LTC6945_spi_map[LTC6945_OMUTE][ADDRx]=0x02;
  LTC6945_spi_map[LTC6945_OMUTE][DxMSB]= 1;
  LTC6945_spi_map[LTC6945_OMUTE][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_PART][ADDRx]=0x0b;
  LTC6945_spi_map[LTC6945_PART][DxMSB]= 4;
  LTC6945_spi_map[LTC6945_PART][NUMBITS]= 5;
  LTC6945_spi_map[LTC6945_PDALL][ADDRx]=0x02;
  LTC6945_spi_map[LTC6945_PDALL][DxMSB]= 7;
  LTC6945_spi_map[LTC6945_PDALL][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_PDOUT][ADDRx]=0x02;
  LTC6945_spi_map[LTC6945_PDOUT][DxMSB]= 4;
  LTC6945_spi_map[LTC6945_PDOUT][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_PDPLL][ADDRx]=0x02;
  LTC6945_spi_map[LTC6945_PDPLL][DxMSB]= 6;
  LTC6945_spi_map[LTC6945_PDPLL][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_PDREFO][ADDRx]=0x02;
  LTC6945_spi_map[LTC6945_PDREFO][DxMSB]= 3;
  LTC6945_spi_map[LTC6945_PDREFO][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_POR][ADDRx]=0x02;
  LTC6945_spi_map[LTC6945_POR][DxMSB]= 0;
  LTC6945_spi_map[LTC6945_POR][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_RD][ADDRx]=0x03;
  LTC6945_spi_map[LTC6945_RD][DxMSB]= 1;
  LTC6945_spi_map[LTC6945_RD][NUMBITS]= 10;
  LTC6945_spi_map[LTC6945_REV][ADDRx]=0x0b;
  LTC6945_spi_map[LTC6945_REV][DxMSB]= 7;
  LTC6945_spi_map[LTC6945_REV][NUMBITS]= 3;
  LTC6945_spi_map[LTC6945_RFO][ADDRx]=0x08;
  LTC6945_spi_map[LTC6945_RFO][DxMSB]= 4;
  LTC6945_spi_map[LTC6945_RFO][NUMBITS]= 2;
  LTC6945_spi_map[LTC6945_THI][ADDRx]=0x00;
  LTC6945_spi_map[LTC6945_THI][DxMSB]= 1;
  LTC6945_spi_map[LTC6945_THI][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_TLO][ADDRx]=0x00;
  LTC6945_spi_map[LTC6945_TLO][DxMSB]= 0;
  LTC6945_spi_map[LTC6945_TLO][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_UNLOCK][ADDRx]=0x00;
  LTC6945_spi_map[LTC6945_UNLOCK][DxMSB]= 5;
  LTC6945_spi_map[LTC6945_UNLOCK][NUMBITS]= 1;
  LTC6945_spi_map[LTC6945_x][ADDRx]=0x01;
  LTC6945_spi_map[LTC6945_x][DxMSB]= 5;
  LTC6945_spi_map[LTC6945_x][NUMBITS]= 6;

  LTC6945_spi_map[LTC6945_BST][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_CP][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_CPCHI][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_CPCLO][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_CPDN][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_CPINV][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_CPMID][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_CPRST][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_CPUP][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_CPWIDE][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_FILT][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_LKCT][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_LKEN][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_LKWIN][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_LOCK][R_ONLY]= 1;
  LTC6945_spi_map[LTC6945_ND][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_OD][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_OMUTE][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_PART][R_ONLY]= 1;
  LTC6945_spi_map[LTC6945_PDALL][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_PDOUT][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_PDPLL][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_PDREFO][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_POR][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_RD][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_REV][R_ONLY]= 1;
  LTC6945_spi_map[LTC6945_RFO][R_ONLY]= 0;
  LTC6945_spi_map[LTC6945_THI][R_ONLY]= 1;
  LTC6945_spi_map[LTC6945_TLO][R_ONLY]= 1;
  LTC6945_spi_map[LTC6945_UNLOCK][R_ONLY]= 1;
  LTC6945_spi_map[LTC6945_x][R_ONLY]= 0;

} // end of LTC6945_init

void set_LTC6945_global_fref(unsigned long fref_MHz, unsigned long fref_Hz)
{
  LTC6945_Fref_MHz=fref_MHz;
  LTC6945_Fref_Hz=fref_Hz;
}

void set_LTC6945_global_frf(unsigned long frf_MHz, unsigned long frf_Hz)
{
  LTC6945_Frf_MHz=frf_MHz;
  LTC6945_Frf_Hz=frf_Hz;
}

void set_LTC6945_global_vcolim(unsigned long fvco_max_MHz, unsigned long fvco_max_Hz, unsigned long fvco_min_MHz, unsigned long fvco_min_Hz)
{
  LTC6945_VCO_Max_Freq_MHz=fvco_max_MHz;
  LTC6945_VCO_Max_Freq_Hz=fvco_max_Hz;
  LTC6945_VCO_Min_Freq_MHz=fvco_min_MHz;
  LTC6945_VCO_Min_Freq_Hz=fvco_min_Hz;
}



unsigned long get_LTC6945_global_fref_MHz()
{
  return LTC6945_Fref_MHz;
}

unsigned long get_LTC6945_global_fref_Hz()
{
  return LTC6945_Fref_Hz;
}

unsigned long get_LTC6945_global_frf_MHz()
{
  return LTC6945_Frf_MHz;
}

unsigned long get_LTC6945_global_frf_Hz()
{
  return LTC6945_Frf_Hz;
}

unsigned long get_LTC6945_global_VCO_MAX_MHz()
{
  return LTC6945_VCO_Max_Freq_MHz;
}

unsigned long get_LTC6945_global_VCO_MIN_MHz()
{
  return LTC6945_VCO_Min_Freq_MHz;
}

unsigned long get_LTC6945_global_VCO_MAX_Hz()
{
  return LTC6945_VCO_Max_Freq_Hz;
}

unsigned long get_LTC6945_global_VCO_MIN_Hz()
{
  return LTC6945_VCO_Min_Freq_Hz;
}

/* -------------------------------------------------------------------------
   FUNCTION: calc_odiv
   - calculates the output divider setting based on the frf and on board
     VCO frequencies of LTC6945
   - @return odiv = 1-6 for valid setting, 999 as invalid frequency
---------------------------------------------------------------------------- */
unsigned long LTC6945_calc_odiv(unsigned long frf[2])
{
  unsigned long odiv, i;
  unsigned long max_fout64[2];
  unsigned long min_fout64[2];
  unsigned long temp_fout[2];
  unsigned long temp_i[2];
  boolean valid_input=false;


  HZto64(max_fout64,LTC6945_VCO_Max_Freq_MHz,LTC6945_VCO_Max_Freq_Hz);
  HZto64(min_fout64,LTC6945_VCO_Min_Freq_MHz,LTC6945_VCO_Min_Freq_Hz);

// verify desired frequency falls within a divider range (1-6)
  valid_input=false;
  for (i=1; i<=6; i++)
  {
    init64(temp_i,0L,i);
    temp_fout[0] = frf[0];
    temp_fout[1] = frf[1];
    mul64(temp_fout,temp_i);
    if (lt64(temp_fout,max_fout64) || eq64(temp_fout, max_fout64))    // same as frf*i <= max_fout
    {
      if (lt64(min_fout64,temp_fout) || eq64(temp_fout, min_fout64))  // same as frf*i >= min_fout
      {
        valid_input=true;
        odiv=i;
      }
    }
  } // end of for loop

  if (valid_input == false) odiv= 999L;
  return odiv;

} // end of LTC6945_calc_odiv

/* -------------------------------------------------------------------------
   FUNCTION: LTC6945_set_frf
   Calculates the integer (N) and output divider (OD) SPI values
   using self created 64bit math functions.

  Datasheet equations
    fvco = fpfd*N
    frf  = fvco/O
    fpfd = fref/R

    can be modified to the following equations
    N   = (int) (fvco/fpfd)  = (int) frf*O*R/fref

    where
    N = ndiv, O= odiv,  in the code below

    Linduino One (Arduino Uno) is limited to 32 bit floats/double/long.
    32 bit math functions will create rounding errors with the above equations,
    tha can result in frequency errors.
    Therefore, the following code uses self created 64bit functions for 64bit integer math.

    - frf (33 bits) LTC6945 max frf/fvco = 6.0GHZ, which is 23 bit number (2^33 = 8.59G)
    - fref (23 bits) LTC6945 min fref = 10MHz, which is a 23 bit number (2^23 = 8.3M)
    - O   (3 bits)
    - R   (10 bits)

    step 1: create 64 bit frf and fref numbers

    step 2: calculate O (output divider)

    step 3: get current R-divider setting

    step 4: calculate frf*O*R
    - max bit count/resolution: 33b+3b+10b= 46b

    step 5: calculate N(16b), using value from step 1
    - N = (int) frf*O*R/fref
    - max bit count/resolution: 46b-23b = 13b
---------------------------------------------------------------------------- */
void LTC6945_set_frf()
{
  unsigned long frf_MHz, frf_Hz, fref_MHz, fref_Hz, odiv, rdiv, ndiv, N_remainder;
  unsigned long N64[2], R64[2], O64[2], temp_long[2];
  char buffer[100];
  unsigned long frf[2], frf_act[2];
  unsigned long fref[2];
  unsigned long temp_math[2];
  unsigned long frf_rdiv_odiv[2];
  unsigned long frf_rdiv_odiv_int[2];
  unsigned long roundup[2];

  /* step 1: create 64 bit frf and fref numbers
     32 bit xxxx MHz number and 32 bit yyy yyy Hz number. */
  frf_MHz=LTC6945_Frf_MHz;
  frf_Hz=LTC6945_Frf_Hz;
  HZto64(frf, frf_MHz, frf_Hz);

  fref_MHz=LTC6945_Fref_MHz;
  fref_Hz=LTC6945_Fref_Hz;
  HZto64(fref,fref_MHz,fref_Hz);

// step 2: calculate O (output divider)
  odiv=LTC6945_calc_odiv(frf);

// step 3: get current R-divider setting
  rdiv=get_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_RD);    // reads selected field

// step 4: calculate frf*O*R
  frf_rdiv_odiv[0]=0;
  frf_rdiv_odiv[1]=odiv*rdiv;
  mul64(frf_rdiv_odiv, frf);  // odiv*rdiv*frf
  frf_rdiv_odiv_int[0]=frf_rdiv_odiv[0];  // copy odiv*rdiv*frf to another variable
  frf_rdiv_odiv_int[1]=frf_rdiv_odiv[1];

// step 5: calculate N(16b), using value from step 3; N = (int) frf*O*R/fref
  temp_math[0]=fref[0];   // copy fref to temp variable for math operation
  temp_math[1]=fref[1];
  div64(frf_rdiv_odiv_int, temp_math);   // frf_rdiv_odiv_int= [(frf*odiv*rdiv)]/fref  -->  int(fvco/fpfd)
  ndiv=frf_rdiv_odiv_int[1];

// step 6: find N for closest frequency - accounts for rounding with integer math
  temp_math[0]=fref[0];   // copy fref to temp variable for math operation
  temp_math[1]=fref[1];
  shl64by(frf_rdiv_odiv,13);            // frf_rdiv_odiv     -->  [(frf*odiv*rdiv)<<13]/fref = N(double) <<13
  div64(frf_rdiv_odiv,temp_math);
  shl64by(frf_rdiv_odiv_int,13); // frf_rdiv_odiv_int  -->  [(int)((frf*odiv*rdiv)/fref)] <<13 = N(int) <<13
  sub64(frf_rdiv_odiv,frf_rdiv_odiv_int);  // N(double) <<13 - N(int)<<13

// at this point frf_rdiv_odiv is the delta between N(double) and N(int) shifted by 13 bits.
// if this remainder is < 4096, N(int) will give the closest frequency
// if this remainder is >=4096, N(int)+1 will give the closest frequency
// 4096 is mid point of the 13 bit number range
  N_remainder = (frf_rdiv_odiv[1] & 8191L);  // 13 bits max code = 8191
  if (N_remainder >= 4096)
  {
    ndiv=ndiv+1;
  }

// program part and print out results to screen
  set_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_OD,odiv);    // programs output divider
  Serial.print("OD = ");
  Serial.println(odiv);

  Serial.print("RD = ");
  Serial.println(rdiv);

  set_LTC6945_SPI_FIELD(LTC6945_CS,LTC6945_ND,ndiv);    // programs N-divider
  Serial.print(F("ND = "));
  Serial.println(ndiv);


/////  calculate actual fout here and print out
  frf_act[0]=fref[0];
  frf_act[1]=fref[1];
  N64[0]=0;
  R64[0]=0;
  O64[0]=0;
  N64[1]=ndiv;
  R64[1]=rdiv;
  O64[1]=odiv;

  mul64(frf_act,N64);  // N*fref
  div64(frf_act,R64);  // N*fref/rdiv (or N*fpfd)
  div64(frf_act,O64);  // N*fpdf/odiv
  if (frf_act[0]==0)   // no 32 bit overflow case
  {
    Serial.print(F("Actual Output Frequency (closest available) is "));
    Serial.print(frf_act[1]);
    Serial.println("Hz");
  }
  else    // overflow case
  {
    temp_math[0]=frf_act[0];
    temp_math[1]=frf_act[1];
    temp_long[0]=0;
    temp_long[1]=OneMHz;
    div64(temp_math,temp_long);  // frf/1e6
    Serial.print(F("Actual Output Frequency (closest available) is "));
    Serial.print(temp_math[1]);
    Serial.print("MHz + ");
    temp_long[0]=0;
    temp_long[1]=OneMHz;
    mul64(temp_math,temp_long);  // int(frf/1e6)*1e6
    sub64(frf_act,temp_math);    //  frf-int(frf/1e6)*1e6
    Serial.print(frf_act[1]);
    Serial.println("Hz");
  }
}


/* -------------------------------------------------------------------------
   FUNCTION: prt
   Prints HEX representation of 64 bit an
---------------------------------------------------------------------------- */
void prt(unsigned long  an[])
{
  Serial.print(an[0],HEX);
  Serial.print(" ");
  Serial.println(an[1],HEX);
}


/* -------------------------------------------------------------------------
   FUNCTION: init64
   Creates a equivalent 64 bit number from 2 32 bit numbers
    an[0]=bigPart    //upper 32 bits
    an[1]=littlePart //lower 32 bits
---------------------------------------------------------------------------- */
void init64(unsigned long  an[], unsigned long bigPart, unsigned long littlePart )
{
  an[0]=bigPart;
  an[1]=littlePart;
}

/* -------------------------------------------------------------------------
   FUNCTION: HZto64
   create a 64 bit Hz number from
   32 bit xxxx MHz number and 32 bit yyy yyy Hz number.
   A) if an < 2^32 bits
      an(upper 32b) = 0
      an(lower 32b) = MHzPart(32b)*1MHz + HzPart (32b)
   B) if an > 2^32 bits (4,294,967,296)
     an(upper 32b) = 1
     an(lower 32b) = ((MHzPart-4294)*1MHz+HzPart)-967296
---------------------------------------------------------------------------- */
void HZto64(unsigned long  an[], unsigned long MHzPart, unsigned long HzPart )
{

  if ((MHzPart>4295) || ((MHzPart==4294) && (HzPart>=967296)))
  {
    an[0]=1L;                                        // upper 32 bits
    an[1] =(MHzPart-4294L)*OneMHz + HzPart-967296L;  // lower 32 bits
  }
  else
  {
    an[0] = 0;                        // upper 32 bits
    an[1] = MHzPart*OneMHz+HzPart;    // lower 32 bits
  }
}

/* -------------------------------------------------------------------------
   FUNCTION: shl64
   Single Bit shift left of equivalent 64 bit number (an[] = an[]<<1)
---------------------------------------------------------------------------- */
void shl64(unsigned long  an[])
{
  an[0] <<= 1;
  if (an[1] & 0x80000000)
    an[0]++;
  an[1] <<= 1;
}


/* -------------------------------------------------------------------------
   FUNCTION: shr64
   Single Bit shift right of equivalent 64 bit number (an[] = an[]>>1)
---------------------------------------------------------------------------- */
void shr64(unsigned long  an[])
{
  an[1] >>= 1;
  if (an[0] & 0x1)
    an[1]+=0x80000000;
  an[0] >>= 1;
}


/* -------------------------------------------------------------------------
   FUNCTION: shl64by
   Multi Bit shift left of equivalent 64 bit number (an[] = an[]<<shiftnum)
---------------------------------------------------------------------------- */
void shl64by(unsigned long  an[], uint8_t shiftnum)
{
  uint8_t i;

  for (i=0; i<shiftnum; i++)
  {
    an[0] <<= 1;
    if (an[1] & 0x80000000)
      an[0]++;
    an[1] <<= 1;
  }
}


/* -------------------------------------------------------------------------
   FUNCTION: shr64by
   Multi Bit shift right of equivalent 64 bit number (an[] = an[]>>shiftnum)
---------------------------------------------------------------------------- */
void shr64by(unsigned long  an[], uint8_t shiftnum)
{
  uint8_t i;

  for (i=0; i<shiftnum; i++)
  {
    an[1] >>= 1;
    if (an[0] & 0x1)
      an[1]+=0x80000000;
    an[0] >>= 1;
  }
}


/* -------------------------------------------------------------------------
   FUNCTION: add64
   64 bit Add ann to an (an[] = an[] + ann[])
---------------------------------------------------------------------------- */
void add64(unsigned long  an[], unsigned long  ann[])
{
  an[0]+=ann[0];
  if (an[1] + ann[1] < ann[1])
    an[0]++;
  an[1]+=ann[1];
}


/* -------------------------------------------------------------------------
   FUNCTION: sub64
   64 bit Subtract ann from an (an[] = an[] - ann[])
---------------------------------------------------------------------------- */
void sub64(unsigned long  an[], unsigned long  ann[])
{
  an[0]-=ann[0];
  if (an[1] < ann[1])
  {
    an[0]--;
  }
  an[1]-= ann[1];
}


/* -------------------------------------------------------------------------
   FUNCTION: eq64
   64 bit, if an == ann, then true
---------------------------------------------------------------------------- */
boolean eq64(unsigned long  an[], unsigned long  ann[])
{
  return (an[0]==ann[0]) && (an[1]==ann[1]);
}


/* -------------------------------------------------------------------------
   FUNCTION: lt64
   64 bit, if an < ann, then true
---------------------------------------------------------------------------- */
boolean lt64(unsigned long  an[], unsigned long  ann[])
{
  if (an[0]>ann[0]) return false;
  return (an[0]<ann[0]) || (an[1]<ann[1]);
}


/* -------------------------------------------------------------------------
   FUNCTION: div64
   64 bit Divide,   num=num/div
---------------------------------------------------------------------------- */
void div64(unsigned long num[], unsigned long den[])
{
  unsigned long quot[2];
  unsigned long qbit[2];
  unsigned long tmp[2];
  init64(quot,0,0);
  init64(qbit,0,1);

  if (eq64(num, zero64))    //numerator 0, call it 0
  {
    init64(num,0,0);
    return;
  }

  if (eq64(den, zero64))   //numerator not zero, denominator 0, infinity in my book.
  {
    init64(num,0xffffffff,0xffffffff);
    return;
  }

  init64(tmp,0x80000000,0);
  while (lt64(den,tmp))
  {
    shl64(den);
    shl64(qbit);
  }

  while (!eq64(qbit,zero64))
  {
    if (lt64(den,num) || eq64(den,num))
    {
      sub64(num,den);
      add64(quot,qbit);
    }
    shr64(den);
    shr64(qbit);
  }

  //remainder now in num, but using it to return quotient for now
  init64(num,quot[0],quot[1]);
}


/* -------------------------------------------------------------------------
   FUNCTION: mul64
   64 bit multiply,   an=an*ann
---------------------------------------------------------------------------- */
void mul64(unsigned long an[], unsigned long ann[])
{
  unsigned long p[2] = {0,0};
  unsigned long y[2] = {ann[0], ann[1]};
  while (!eq64(y,zero64))
  {
    if (y[1] & 1)
      add64(p,an);
    shl64(an);
    shr64(y);
  }
  init64(an,p[0],p[1]);
}
