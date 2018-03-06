/*!
    LTC6954: Low Phase Noise, Triple Output Clock Distribution Divider/Driver

@verbatim

The LTC®6954 is a family of very low phase noise clock
distribution parts. Each part has three outputs and each
output has an individually programmable frequency
divider and delay.

There are four members of the family, differing in their output logic signal type:
LTC6954-1: Three LVPECL outputs
LTC6954-2: Two LVPECL and one LVDS/CMOS outputs
LTC6954-3: One LVPECL and two LVDS/CMOS outputs
LTC6954-4: Three LVDS/CMOS outputs

Each output is individually programmable to divide the
input frequency by any integer from 1 to 63, and to delay
each output by 0 to 63 input clock cycles. The output duty
cycle is always 50%, regardless of the divide number.

The LVDS/CMOS outputs are jumper selectable via the
OUTSEL pins to provide either an LVDS logic output or a
CMOS logic output.

The LTC6954 also features Linear Technology’s EZSync
system for perfect clock synchronization and alignment
every time.

The LTC6954 is available in a 36-lead, 4mm × 7mm QFN
package.

All device settings are controlled through an SPI-compatible
serial port.

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

//! @ingroup RF_Timing
//! @{
//! @defgroup LTC6954 LTC6954: Low Phase Noise, Triple Output Clock Distribution Divider/Driver
//! @}

/*! @file
    @ingroup LTC6954
    Library for LTC6954: Low Phase Noise, Triple Output Clock Distribution Divider/Driver
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_SPI.h"
#include "LTC6954.h"
#include <SPI.h>

uint8_t LTC6954_reg[LTC6954_NUM_REGADDR];             //!< number of LTC6954 spi addresses
uint8_t LTC6954_spi_map[(LTC6954_NUM_REGFIELD+1)][4]; //!< LTC6954 spi map, stores MSB address location, MSB bit location, field length in bits, and R or RW capability


/* -------------------------------------------------------------------------
  FUNCTION: LTC6954_read
  - reads 8 bit Data field to LTC6954.
  - has to shift data by one bit to account for RW bit
 -------------------------------------------------------------------------- */
uint8_t LTC6954_read(uint8_t cs, int8_t address)
{
  int8_t address_shift;
  LT_union_int16_2bytes rx;

  address_shift =(address << 1) | 0x01; // shift to left to account for R/W bit, set bit high for read
  spi_transfer_word(cs, address_shift<<8 , &rx.LT_uint16);

  LTC6954_reg[address]=rx.LT_byte[0];
  return(rx.LT_byte[0]);
}


/* -------------------------------------------------------------------------
  FUNCTION: LTC6954_read_field
  For SPI FIELDS located in 1 or multiple address location
  - reads specific address locations
  - identifies and returns specific field in question
    - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
--------------------------------------------------------------------------- */
long LTC6954_read_field(uint8_t cs, uint8_t address, uint8_t MSB_loc, uint8_t numbits)
{
  int bit_shift, i, num_reg;
  long field_val, maskbits, pow2;

  num_reg=0;
  field_val=0;
// determines how many register are used
  do
  {
    bit_shift = (MSB_loc+1)- (numbits-num_reg*8);   // determines bit_shift for last register location
    field_val=LTC6954_read(cs, (address+num_reg))+(field_val<<8);  // reads current address locations, shifts previous address location 8 bits
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
  FUNCTION: get_LTC6954_SPI_FIELD
  For SPI FIELDS
  - reads specific address locations
  - identifies and returns specific field in question
    - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
--------------------------------------------------------------------------- */
long get_LTC6954_SPI_FIELD(uint8_t cs, uint8_t f)
{

  return LTC6954_read_field(cs, LTC6954_spi_map[f][ADDRx], LTC6954_spi_map[f][DxMSB], LTC6954_spi_map[f][NUMBITS]);
}

/* -------------------------------------------------------------------------
  FUNCTION: LTC6954_write
  - writes 8 bit Data field to LTC6954.
  - has to shift data by one bit to account for RW bit
--------------------------------------------------------------------------- */
void LTC6954_write(uint8_t cs, uint8_t address, uint8_t Data)
{
  LT_union_int16_2bytes rx;

  address=address << 1; // shift to left to account for R/W bit
  spi_transfer_word(cs, (address<<8) | Data, &rx.LT_uint16);
}


/* -------------------------------------------------------------------------
 FUNCTION: LTC6954_write_field
 For SPI FIELDS
 - reads specific address location
 - identifies and returns specific field in question
   - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
---------------------------------------------------------------------------- */
uint8_t LTC6954_write_field(uint8_t cs, long field_data, uint8_t address, uint8_t MSB_loc, uint8_t numbits)
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
    current_content=LTC6954_read(cs, (address+num_reg)) + (current_content<<8);

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
    LTC6954_write(cs, (address+i), reg_val);
  } // end of for loop
} // end of LTC6954_write_field


/* -------------------------------------------------------------------------
   FUNCTION: get_LTC6954_REGSIZE
   - returns # of addresses in parts register map (array size)
---------------------------------------------------------------------------- */
uint8_t get_LTC6954_REGSIZE()
{
  return sizeof(LTC6954_reg);
}


/* -------------------------------------------------------------------------
   FUNCTION: get_LTC6954_SPI_FIELD_NUMBITS
   - returns the number of bits for a given field name in the SPI map
---------------------------------------------------------------------------- */
uint8_t get_LTC6954_SPI_FIELD_NUMBITS(uint8_t f)
{
  return LTC6954_spi_map[f][NUMBITS];
}


/* -------------------------------------------------------------------------
   FUNCTION: get_LTC6954_SPI_FIELD_RW
   - returns if the given field name is (0)read/write or (1)read_only field
---------------------------------------------------------------------------- */
uint8_t get_LTC6954_SPI_FIELD_RW(uint8_t f)
{
  return LTC6954_spi_map[f][R_ONLY];
}


/* -------------------------------------------------------------------------
   FUNCTION: set_LTC6954_SPI_FIELD
   For SPI FIELDS
   - reads specific address location
   - identifies and returns specific field in question
   - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
---------------------------------------------------------------------------- */
void set_LTC6954_SPI_FIELD(uint8_t cs, uint8_t f, long field_data)
{
  LTC6954_write_field(cs, field_data, LTC6954_spi_map[f][ADDRx], LTC6954_spi_map[f][DxMSB], LTC6954_spi_map[f][NUMBITS]);
}


/* -------------------------------------------------------------------------
   FUNCTION: set_LTC6954_ALLREGS
   - writes data to all registers at once
--------------------------------------------------------------------------- */
void set_LTC6954_ALLREGS(uint8_t cs, uint8_t reg00, uint8_t reg01, uint8_t reg02, uint8_t reg03, uint8_t reg04, uint8_t reg05, uint8_t reg06)
{
  uint8_t i;

  LTC6954_reg[0] = reg00;
  LTC6954_reg[1] = reg01;
  LTC6954_reg[2] = reg02;
  LTC6954_reg[3] = reg03;
  LTC6954_reg[4] = reg04;
  LTC6954_reg[5] = reg05;
  LTC6954_reg[6] = reg06;


  for (i=0; i<get_LTC6954_REGSIZE(); i++)  LTC6954_write(cs, i, LTC6954_reg[i]);
} // end of set_LTC6954_ALLREGS


/* -------------------------------------------------------------------------
   FUNCTION: LTC6954_init
   - initializes the SPI MAP
   - for ease of programming there is spreadsheet that automates this some.
----------------------------------------------------------------------------*/
void LTC6954_init()
{

// spi map
  LTC6954_spi_map[LTC6954_CMSINV0][ADDRx]=0x01;
  LTC6954_spi_map[LTC6954_CMSINV0][DxMSB]= 6;
  LTC6954_spi_map[LTC6954_CMSINV0][NUMBITS]= 1;
  LTC6954_spi_map[LTC6954_CMSINV1][ADDRx]=0x03;
  LTC6954_spi_map[LTC6954_CMSINV1][DxMSB]= 6;
  LTC6954_spi_map[LTC6954_CMSINV1][NUMBITS]= 1;
  LTC6954_spi_map[LTC6954_CMSINV2][ADDRx]=0x05;
  LTC6954_spi_map[LTC6954_CMSINV2][DxMSB]= 6;
  LTC6954_spi_map[LTC6954_CMSINV2][NUMBITS]= 1;
  LTC6954_spi_map[LTC6954_DEL0][ADDRx]=0x01;
  LTC6954_spi_map[LTC6954_DEL0][DxMSB]= 5;
  LTC6954_spi_map[LTC6954_DEL0][NUMBITS]= 6;
  LTC6954_spi_map[LTC6954_DEL1][ADDRx]=0x03;
  LTC6954_spi_map[LTC6954_DEL1][DxMSB]= 5;
  LTC6954_spi_map[LTC6954_DEL1][NUMBITS]= 6;
  LTC6954_spi_map[LTC6954_DEL2][ADDRx]=0x05;
  LTC6954_spi_map[LTC6954_DEL2][DxMSB]= 5;
  LTC6954_spi_map[LTC6954_DEL2][NUMBITS]= 6;
  LTC6954_spi_map[LTC6954_LVCS0][ADDRx]=0x02;
  LTC6954_spi_map[LTC6954_LVCS0][DxMSB]= 6;
  LTC6954_spi_map[LTC6954_LVCS0][NUMBITS]= 1;
  LTC6954_spi_map[LTC6954_LVCS1][ADDRx]=0x04;
  LTC6954_spi_map[LTC6954_LVCS1][DxMSB]= 6;
  LTC6954_spi_map[LTC6954_LVCS1][NUMBITS]= 1;
  LTC6954_spi_map[LTC6954_LVCS2][ADDRx]=0x06;
  LTC6954_spi_map[LTC6954_LVCS2][DxMSB]= 6;
  LTC6954_spi_map[LTC6954_LVCS2][NUMBITS]= 1;
  LTC6954_spi_map[LTC6954_M0][ADDRx]=0x02;
  LTC6954_spi_map[LTC6954_M0][DxMSB]= 5;
  LTC6954_spi_map[LTC6954_M0][NUMBITS]= 6;
  LTC6954_spi_map[LTC6954_M1][ADDRx]=0x04;
  LTC6954_spi_map[LTC6954_M1][DxMSB]= 5;
  LTC6954_spi_map[LTC6954_M1][NUMBITS]= 6;
  LTC6954_spi_map[LTC6954_M2][ADDRx]=0x06;
  LTC6954_spi_map[LTC6954_M2][DxMSB]= 5;
  LTC6954_spi_map[LTC6954_M2][NUMBITS]= 6;
  LTC6954_spi_map[LTC6954_PART][ADDRx]=0x07;
  LTC6954_spi_map[LTC6954_PART][DxMSB]= 4;
  LTC6954_spi_map[LTC6954_PART][NUMBITS]= 5;
  LTC6954_spi_map[LTC6954_PDALL][ADDRx]=0x00;
  LTC6954_spi_map[LTC6954_PDALL][DxMSB]= 6;
  LTC6954_spi_map[LTC6954_PDALL][NUMBITS]= 1;
  LTC6954_spi_map[LTC6954_PD_DIV0][ADDRx]=0x00;
  LTC6954_spi_map[LTC6954_PD_DIV0][DxMSB]= 0;
  LTC6954_spi_map[LTC6954_PD_DIV0][NUMBITS]= 1;
  LTC6954_spi_map[LTC6954_PD_DIV1][ADDRx]=0x00;
  LTC6954_spi_map[LTC6954_PD_DIV1][DxMSB]= 2;
  LTC6954_spi_map[LTC6954_PD_DIV1][NUMBITS]= 1;
  LTC6954_spi_map[LTC6954_PD_DIV2][ADDRx]=0x00;
  LTC6954_spi_map[LTC6954_PD_DIV2][DxMSB]= 4;
  LTC6954_spi_map[LTC6954_PD_DIV2][NUMBITS]= 1;
  LTC6954_spi_map[LTC6954_PD_OUT0][ADDRx]=0x00;
  LTC6954_spi_map[LTC6954_PD_OUT0][DxMSB]= 1;
  LTC6954_spi_map[LTC6954_PD_OUT0][NUMBITS]= 1;
  LTC6954_spi_map[LTC6954_PD_OUT1][ADDRx]=0x00;
  LTC6954_spi_map[LTC6954_PD_OUT1][DxMSB]= 3;
  LTC6954_spi_map[LTC6954_PD_OUT1][NUMBITS]= 1;
  LTC6954_spi_map[LTC6954_PD_OUT2][ADDRx]=0x00;
  LTC6954_spi_map[LTC6954_PD_OUT2][DxMSB]= 5;
  LTC6954_spi_map[LTC6954_PD_OUT2][NUMBITS]= 1;
  LTC6954_spi_map[LTC6954_REV][ADDRx]=0x07;
  LTC6954_spi_map[LTC6954_REV][DxMSB]= 7;
  LTC6954_spi_map[LTC6954_REV][NUMBITS]= 3;
  LTC6954_spi_map[LTC6954_SYNC_EN0][ADDRx]=0x01;
  LTC6954_spi_map[LTC6954_SYNC_EN0][DxMSB]= 7;
  LTC6954_spi_map[LTC6954_SYNC_EN0][NUMBITS]= 1;
  LTC6954_spi_map[LTC6954_SYNC_EN1][ADDRx]=0x03;
  LTC6954_spi_map[LTC6954_SYNC_EN1][DxMSB]= 7;
  LTC6954_spi_map[LTC6954_SYNC_EN1][NUMBITS]= 1;
  LTC6954_spi_map[LTC6954_SYNC_EN2][ADDRx]=0x05;
  LTC6954_spi_map[LTC6954_SYNC_EN2][DxMSB]= 7;
  LTC6954_spi_map[LTC6954_SYNC_EN2][NUMBITS]= 1;


  LTC6954_spi_map[LTC6954_CMSINV0][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_CMSINV1][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_CMSINV2][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_DEL0][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_DEL1][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_DEL2][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_LVCS0][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_LVCS1][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_LVCS2][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_M0][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_M1][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_M2][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_PART][R_ONLY]= 1;
  LTC6954_spi_map[LTC6954_PDALL][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_PD_DIV0][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_PD_DIV1][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_PD_DIV2][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_PD_OUT0][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_PD_OUT1][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_PD_OUT2][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_REV][R_ONLY]= 1;
  LTC6954_spi_map[LTC6954_SYNC_EN0][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_SYNC_EN1][R_ONLY]= 0;
  LTC6954_spi_map[LTC6954_SYNC_EN2][R_ONLY]= 0;


} // end of LTC6954_init
