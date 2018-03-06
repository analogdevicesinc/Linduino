/*!
    LTC6953: Ultra-Low Jitter, JESD204B Clock Distributor with Eleven Programmable Outputs

@verbatim

The LTC®6953 is a high performance, ultralow jitter,
JESD204B clock distribution IC. The LTC6953’s eleven
outputs can be configured as up to five JESD204B subclass
1 device clock/SYSREF pairs plus one general
purpose output or simply eleven general purpose clock
outputs for non-JESD204B applications. Each output has
its own individually programmable frequency divider and
output driver. All outputs can also be synchronized and
set to precise phase alignment using individual coarse half
cycle digital delays and fine analog time delays.

For applications requiring more than eleven total outputs,
multiple LTC6953s can be connected together with
LTC6952s and LTC6955s using the EZSync or ParallelSync
synchronization protocols.

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

//! @ingroup RF_Timing
//! @{
//! @defgroup LTC6953 LTC6953: Ultra-Low Jitter, JESD204B Clock Distributor with Eleven Programmable Outputs
//! @}

/*! @file
    @ingroup LTC6953
    Library for LTC6953: Ultra-Low Jitter, JESD204B Clock Distributor with Eleven Programmable Outputs
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_SPI.h"
#include "LTC6953.h"
#include "QuikEval_EEPROM.h"
#include <SPI.h>

uint8_t LTC6953_reg[LTC6953_NUM_REGADDR];              //!< number of LTC6953 spi addresses
uint16_t LTC6953_spi_map[(LTC6953_NUM_REGFIELD+1)];  //!< LTC6953 spi map, AAAA AAAA RMMM NNNN;  A= ADDR LOC, R=R or RW, M = MSB bit location, N = field length

uint8_t LTC6953_lkup_tbl[2][LTC6953_NUM_REGADDR] =   //!< created with the LTC6952Wizard tool
{
  {0x25, 0x04, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x8c, 0xe0, 0x1b, 0x00, 0x40, 0x80, 0x24, 0x00, 0x8c, 0xe0, 0x1b, 0x00, 0x40, 0x80, 0x24, 0x00, 0x8c, 0xe0, 0x00, 0x00, 0x89, 0x80, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x8c, 0xe0, 0x23, 0x00, 0x00, 0x80, 0x24, 0x00, 0x8c, 0xe0, 0x23, 0x00, 0x00, 0x80, 0x24, 0x00, 0x13},  //!< xxxx
  {0x25, 0x04, 0x00, 0xf0, 0xf0, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x13},  //!< xxxx
};  //!< LTC6953 Configuration look-up table


/* -------------------------------------------------------------------------
  FUNCTION: LTC6953_read
  - reads 8 bit Data field to LTC6953.
  - has to shift data by one bit to account for RW bit
 -------------------------------------------------------------------------- */
uint8_t LTC6953_read(uint8_t cs, int8_t address)
{
  int8_t address_shift;
  LT_union_int16_2bytes rx;

  address_shift =(address << 1) | 0x01; // shift to left to account for R/W bit, set bit high for read
  spi_transfer_word(cs, address_shift<<8 , &rx.LT_uint16);

  LTC6953_reg[address]=rx.LT_byte[0];
  return(rx.LT_byte[0]);
}


/* -------------------------------------------------------------------------
  FUNCTION: LTC6953_read_field
  For SPI FIELDS located in 1 or multiple address location
  - reads specific address locations
  - identifies and returns specific field in question
  - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
--------------------------------------------------------------------------- */
long LTC6953_read_field(uint8_t cs, uint8_t address, uint8_t MSB_loc, uint8_t numbits)
{
  int bit_shift, i, num_reg;
  long field_val, maskbits, pow2;

  num_reg=0;
  field_val=0;
// determines how many register are used
  do
  {
    bit_shift = (MSB_loc+1)- (numbits-num_reg*8);   // determines bit_shift for last register location
    field_val=LTC6953_read(cs, (address+num_reg))+(field_val<<8);  // reads current address locations, shifts previous address location 8 bits
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
  FUNCTION: get_LTC6953_SPI_FIELD
  For SPI FIELDS
  - reads specific address locations
  - identifies and returns specific field in question
    - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
--------------------------------------------------------------------------- */
long get_LTC6953_SPI_FIELD(uint8_t cs, uint8_t f)
{
  uint8_t addrx;
  uint8_t dxmsb;
  uint8_t numbits;

  addrx = (LTC6953_spi_map[f] & 0xff00) >> 8;
  dxmsb = (LTC6953_spi_map[f] & 0x0070) >> 4;
  numbits = (LTC6953_spi_map[f] & 0x000f) + 1;
  return LTC6953_read_field(cs, addrx, dxmsb, numbits);

#if INCLUDE_REGMAP
  return LTC6953_read_field(cs, LTC6953_spi_map[f][ADDRx], LTC6953_spi_map[f][DxMSB], LTC6953_spi_map[f][NUMBITS]);
#endif
}

/* -------------------------------------------------------------------------
  FUNCTION: LTC6953_write
  - writes 8 bit Data field to LTC6953.
  - has to shift data by one bit to account for RW bit
--------------------------------------------------------------------------- */
void LTC6953_write(uint8_t cs, uint8_t address, uint8_t Data)
{
  LT_union_int16_2bytes rx;

  address=address << 1; // shift to left to account for R/W bit
  spi_transfer_word(cs, (address<<8) | Data, &rx.LT_uint16);
}


/* -------------------------------------------------------------------------
 FUNCTION: LTC6953_write_field
 For SPI FIELDS
 - reads specific address location
 - identifies and returns specific field in question
   - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
---------------------------------------------------------------------------- */
uint8_t LTC6953_write_field(uint8_t cs, long field_data, uint8_t address, uint8_t MSB_loc, uint8_t numbits)
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
    current_content=LTC6953_read(cs, (address+num_reg)) + (current_content<<8);

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
    LTC6953_write(cs, (address+i), reg_val);
  } // end of for loop
} // end of LTC6953_write_field


/* -------------------------------------------------------------------------
   FUNCTION: get_LTC6953_REGSIZE
   - returns # of addresses in parts register map (array size)
---------------------------------------------------------------------------- */
uint8_t get_LTC6953_REGSIZE()
{
  return sizeof(LTC6953_reg);
}


/* -------------------------------------------------------------------------
   FUNCTION: get_LTC6953_SPI_FIELD_NUMBITS
   - returns the number of bits for a given field name in the SPI map
---------------------------------------------------------------------------- */
uint8_t get_LTC6953_SPI_FIELD_NUMBITS(uint8_t f)
{
  uint8_t numbits;

  numbits = (LTC6953_spi_map[f] & 0x000f) + 1;
  return numbits;
}


/* -------------------------------------------------------------------------
   FUNCTION: get_LTC6953_SPI_FIELD_RW
   - returns if the given field name is (0)read/write or (1)read_only field
---------------------------------------------------------------------------- */
uint8_t get_LTC6953_SPI_FIELD_RW(uint8_t f)
{
  uint8_t rw_stat;

  rw_stat = (LTC6953_spi_map[f] & 0x0080) >> 7;
  return rw_stat;
}


/* -------------------------------------------------------------------------
   FUNCTION: set_LTC6953_SPI_FIELD
   For SPI FIELDS
   - reads specific address location
   - identifies and returns specific field in question
   - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
---------------------------------------------------------------------------- */
void set_LTC6953_SPI_FIELD(uint8_t cs, uint8_t f, long field_data)
{
  uint8_t addrx;
  uint8_t dxmsb;
  uint8_t numbits;

  addrx = (LTC6953_spi_map[f] & 0xff00) >> 8;
  dxmsb = (LTC6953_spi_map[f] & 0x0070) >> 4;
  numbits = (LTC6953_spi_map[f] & 0x000f) + 1;
  LTC6953_write_field(cs, field_data, addrx, dxmsb, numbits);

#if INCLUDE_REGMAP
  LTC6953_write_field(cs, field_data, LTC6953_spi_map[f][ADDRx], LTC6953_spi_map[f][DxMSB], LTC6953_spi_map[f][NUMBITS]);
#endif
}

/* -------------------------------------------------------------------------
   FUNCTION: set_LTC6953_REGS_lkup_tbl
   - writes LTC6953 registers from a look up table
   - uses the Multi-byte write option for more efficient data transfer
   - Either SPI Syncs or recommends the use to SYNC pin based on RAO value
--------------------------------------------------------------------------- */
void set_LTC6953_REGS_lkup_tbl(uint8_t lkup_tbl_row)
{
  uint8_t val_temp, i;
  uint8_t val_reg2, val_reg3, val_regA;
  uint8_t *rx;

//! ************** SPI REGISTER WRITE SECTION ************
  output_low(QUIKEVAL_CS);               //! 1) CS low, enable SPI
  val_temp = 1<<1;                       //! 2) addr 1, shift 1 bit to account for RW bit (W=0)
  *rx = SPI.transfer(val_temp);          //! 3) send ADDR1 byte
  for (i=1; i<LTC6953_NUM_REGADDR; i++)
  {
    val_temp=LTC6953_lkup_tbl[lkup_tbl_row][i];
    *rx = SPI.transfer(val_temp);       //! 4) send REG byte
  }
  output_high(QUIKEVAL_CS);              //! 5) CS high, disable SPI


//! **************** OUTPUT SYNC SECTION ******************
//! toggle SSRQ to SYNC outputs
  set_LTC6953_SPI_FIELD(LTC6953_CS, LTC6953_SSRQ, 1L);
  delay(1);  // min 1ms delay for SSRQ HIGH pulse
  set_LTC6953_SPI_FIELD(LTC6953_CS, LTC6953_SSRQ, 0L);
  delay(1);  // min 1ms delay for SSRQ LOW pulse

//! FOR JESD204B subclass 1: SET SRQMD=1, then toggle SSRQ to start SYSREF
  set_LTC6953_SPI_FIELD(LTC6953_CS, LTC6953_SRQMD, 1L);
  set_LTC6953_SPI_FIELD(LTC6953_CS, LTC6953_SSRQ, 1L);
  delay(1);  // min 1ms delay for SSRQ HIGH pulse
  set_LTC6953_SPI_FIELD(LTC6953_CS, LTC6953_SSRQ, 0L);
  delay(1);  // min 1ms delay for SSRQ LOW pulse

//! FOR JESD204B subclass 1: SET SRQMD=0, reduce power
  set_LTC6953_SPI_FIELD(LTC6953_CS, LTC6953_SRQMD, 0L);

//! OPTIONAL FOR JESD204B subclass 1: power down SYSREF outputs when complete(PDx=2), reduces power.
//! keep SYSREF dividers running to maintain synchronization.

//! **************** INFO SECTION ******************
//! INFO ONLY: Print Register Map written to part


} // end of set_LTC6953_REGS_lkup_tbl


/* -------------------------------------------------------------------------
   FUNCTION: LTC6953_init
   - initializes the SPI MAP
   - for ease of programming there is spreadsheet that automates this some.
----------------------------------------------------------------------------*/
void LTC6953_init()
{
// look up table


// spi map
  LTC6953_spi_map[LTC6953_ADEL0]=3925;
  LTC6953_spi_map[LTC6953_ADEL1]=4949;
  LTC6953_spi_map[LTC6953_ADEL2]=5973;
  LTC6953_spi_map[LTC6953_ADEL3]=6997;
  LTC6953_spi_map[LTC6953_ADEL4]=8021;
  LTC6953_spi_map[LTC6953_ADEL5]=9045;
  LTC6953_spi_map[LTC6953_ADEL6]=10069;
  LTC6953_spi_map[LTC6953_ADEL7]=11093;
  LTC6953_spi_map[LTC6953_ADEL8]=12117;
  LTC6953_spi_map[LTC6953_ADEL9]=13141;
  LTC6953_spi_map[LTC6953_ADEL10]=14165;
  LTC6953_spi_map[LTC6953_DDEL0]=3387;
  LTC6953_spi_map[LTC6953_DDEL1]=4411;
  LTC6953_spi_map[LTC6953_DDEL2]=5435;
  LTC6953_spi_map[LTC6953_DDEL3]=6459;
  LTC6953_spi_map[LTC6953_DDEL4]=7483;
  LTC6953_spi_map[LTC6953_DDEL5]=8507;
  LTC6953_spi_map[LTC6953_DDEL6]=9531;
  LTC6953_spi_map[LTC6953_DDEL7]=10555;
  LTC6953_spi_map[LTC6953_DDEL8]=11579;
  LTC6953_spi_map[LTC6953_DDEL9]=12603;
  LTC6953_spi_map[LTC6953_DDEL10]=13627;
  LTC6953_spi_map[LTC6953_EZMD]=2880;
  LTC6953_spi_map[LTC6953_FILTV]=528;
  LTC6953_spi_map[LTC6953_INVSTAT]=368;
  LTC6953_spi_map[LTC6953_MD0]=3106;
  LTC6953_spi_map[LTC6953_MD1]=4130;
  LTC6953_spi_map[LTC6953_MD2]=5154;
  LTC6953_spi_map[LTC6953_MD3]=6178;
  LTC6953_spi_map[LTC6953_MD4]=7202;
  LTC6953_spi_map[LTC6953_MD5]=8226;
  LTC6953_spi_map[LTC6953_MD6]=9250;
  LTC6953_spi_map[LTC6953_MD7]=10274;
  LTC6953_spi_map[LTC6953_MD8]=11298;
  LTC6953_spi_map[LTC6953_MD9]=12322;
  LTC6953_spi_map[LTC6953_MD10]=13346;
  LTC6953_spi_map[LTC6953_MODE0]=3425;
  LTC6953_spi_map[LTC6953_MODE1]=4449;
  LTC6953_spi_map[LTC6953_MODE2]=5473;
  LTC6953_spi_map[LTC6953_MODE3]=6497;
  LTC6953_spi_map[LTC6953_MODE4]=7521;
  LTC6953_spi_map[LTC6953_MODE5]=8545;
  LTC6953_spi_map[LTC6953_MODE6]=9569;
  LTC6953_spi_map[LTC6953_MODE7]=10593;
  LTC6953_spi_map[LTC6953_MODE8]=11617;
  LTC6953_spi_map[LTC6953_MODE9]=12641;
  LTC6953_spi_map[LTC6953_MODE10]=13665;
  LTC6953_spi_map[LTC6953_MP0]=3188;
  LTC6953_spi_map[LTC6953_MP1]=4212;
  LTC6953_spi_map[LTC6953_MP2]=5236;
  LTC6953_spi_map[LTC6953_MP3]=6260;
  LTC6953_spi_map[LTC6953_MP4]=7284;
  LTC6953_spi_map[LTC6953_MP5]=8308;
  LTC6953_spi_map[LTC6953_MP6]=9332;
  LTC6953_spi_map[LTC6953_MP7]=10356;
  LTC6953_spi_map[LTC6953_MP8]=11380;
  LTC6953_spi_map[LTC6953_MP9]=12404;
  LTC6953_spi_map[LTC6953_MP10]=13428;
  LTC6953_spi_map[LTC6953_OINV0]=3392;
  LTC6953_spi_map[LTC6953_OINV1]=4416;
  LTC6953_spi_map[LTC6953_OINV2]=5440;
  LTC6953_spi_map[LTC6953_OINV3]=6464;
  LTC6953_spi_map[LTC6953_OINV4]=7488;
  LTC6953_spi_map[LTC6953_OINV5]=8512;
  LTC6953_spi_map[LTC6953_OINV6]=9536;
  LTC6953_spi_map[LTC6953_OINV7]=10560;
  LTC6953_spi_map[LTC6953_OINV8]=11584;
  LTC6953_spi_map[LTC6953_OINV9]=12608;
  LTC6953_spi_map[LTC6953_OINV10]=13632;
  LTC6953_spi_map[LTC6953_PART]=14515;
  LTC6953_spi_map[LTC6953_PDALL]=624;
  LTC6953_spi_map[LTC6953_PDVCOPK]=592;
  LTC6953_spi_map[LTC6953_PD10]=1361;
  LTC6953_spi_map[LTC6953_PD9]=1329;
  LTC6953_spi_map[LTC6953_PD8]=1297;
  LTC6953_spi_map[LTC6953_PD7]=1137;
  LTC6953_spi_map[LTC6953_PD6]=1105;
  LTC6953_spi_map[LTC6953_PD5]=1073;
  LTC6953_spi_map[LTC6953_PD4]=1041;
  LTC6953_spi_map[LTC6953_PD3]=881;
  LTC6953_spi_map[LTC6953_PD2]=849;
  LTC6953_spi_map[LTC6953_PD1]=817;
  LTC6953_spi_map[LTC6953_PD0]=785;
  LTC6953_spi_map[LTC6953_POR]=512;
  LTC6953_spi_map[LTC6953_REV]=14579;
  LTC6953_spi_map[LTC6953_SYSCT]=2849;
  LTC6953_spi_map[LTC6953_SRQMD]=2864;
  LTC6953_spi_map[LTC6953_SSRQ]=2816;
  LTC6953_spi_map[LTC6953_SRQEN0]=3440;
  LTC6953_spi_map[LTC6953_SRQEN1]=4464;
  LTC6953_spi_map[LTC6953_SRQEN2]=5488;
  LTC6953_spi_map[LTC6953_SRQEN3]=6512;
  LTC6953_spi_map[LTC6953_SRQEN4]=7536;
  LTC6953_spi_map[LTC6953_SRQEN5]=8560;
  LTC6953_spi_map[LTC6953_SRQEN6]=9584;
  LTC6953_spi_map[LTC6953_SRQEN7]=10608;
  LTC6953_spi_map[LTC6953_SRQEN8]=11632;
  LTC6953_spi_map[LTC6953_SRQEN9]=12656;
  LTC6953_spi_map[LTC6953_SRQEN10]=13680;
  LTC6953_spi_map[LTC6953_TEMPO]=1392;
  LTC6953_spi_map[LTC6953_nVCOOK]=176;
  LTC6953_spi_map[LTC6953_VCOOK]=160;
  LTC6953_spi_map[LTC6953_x]=358;


} // end of LTC6953_init



// Read the ID string from the EEPROM and determine if the correct board is connected.
// Returns 1 if successful, 0 if not successful
int8_t discover_demo_board_local(char *demo_name)
{
  int8_t connected;
  connected = 1;
  // read the ID from the serial EEPROM on the board
  // reuse the buffer declared in UserInterface
  if (read_quikeval_id_string(&ui_buffer[0]) == 0) connected = 0;

  return(connected);
}



