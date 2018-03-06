/*!
    LTC6951: Ultra-Low Jitter 2MHz to 2.7GHz Multi-Output Clock Synthesizer with Integrated VCO

@verbatim

The LTC®6951 is a high performance, low noise, Phase
Locked Loop (PLL) with a fully integrated VCO. The low
noise VCO uses no external components and is internally
calibrated to the correct output frequency with no external
system support.

The clock generation section provides five outputs based
on the VCO prescaler signal with individual dividers for
each output. Four outputs feature very low noise, low skew
CML logic. The fifth output is low noise LVDS. All outputs
can be synchronized and set to precise phase alignment
using the programmable delays.

Choose the LTC6951-1 if any desired output frequency
falls in the ranges 2.5GHz to 2.7GHz, 1.66GHz to 1.8GHz,
or 1.25GHz to 1.35GHz. Choose the LTC6951 for all other
frequencies.

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

//! @ingroup RF_Timing
//! @{
//! @defgroup LTC6951 LTC6951: Ultra-Low Jitter 2MHz to 2.7GHz Multi-Output Clock Synthesizer with Integrated VCO
//! @}

/*! @file
    @ingroup LTC6951
    Library for LTC6951: Ultra-Low Jitter 2MHz to 2.7GHz Multi-Output Clock Synthesizer with Integrated VCO
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_SPI.h"
#include "LTC6951.h"
#include "QuikEval_EEPROM.h"
#include <SPI.h>

uint8_t LTC6951_reg[LTC6951_NUM_REGADDR];              //!< number of LTC6951 spi addresses
uint8_t LTC6951_spi_map[(LTC6951_NUM_REGFIELD+1)][4];  //!< LTC6951 spi map, stores MSB address location, MSB bit location, field length in bits, and R or RW capability

uint8_t LTC6951_lkup_tbl[10][LTC6951_NUM_REGADDR] =   //!< the following settings assume a 100MHz reference input
{
  {0x05, 0xba, 0x00, 0x78, 0xb3, 0x04, 0x30, 0x07, 0x00, 0x92, 0x00, 0x92, 0x00, 0x92, 0x00, 0x92, 0x00, 0x92, 0x00, 0x11},  //!< ALL CHAN 600MHZ, EZSync
  {0x05, 0xba, 0x00, 0x74, 0xa3, 0x04, 0x06, 0x07, 0x00, 0x12, 0x00, 0x92, 0x06, 0x92, 0x06, 0x92, 0x06, 0x92, 0x06, 0x11},  //!< ALL CHAN 600MHZ, EZSync Ref Aligned
  {0x05, 0xba, 0x00, 0x74, 0xa3, 0x04, 0x06, 0x07, 0x00, 0x12, 0x00, 0x92, 0x06, 0x92, 0x06, 0x92, 0x06, 0x92, 0x06, 0x11},  //!< ALL CHAN 600MHZ, EZParallelSync
  {0x05, 0xba, 0x00, 0x74, 0xa3, 0x04, 0x06, 0x07, 0x00, 0x12, 0xc0, 0x92, 0x06, 0x92, 0x06, 0x92, 0x06, 0x92, 0x06, 0x11},  //!< ALL CHAN 600MHZ, ParallelSync
  {0x05, 0xba, 0x00, 0x78, 0xb3, 0x04, 0x32, 0x07, 0x00, 0x90, 0x00, 0x90, 0x00, 0x90, 0x00, 0x90, 0x00, 0x92, 0x00, 0x11},  //!< OUT[0:3] 2500MHZ, OUT4:625MHz, EZSync
  {0x05, 0xba, 0x00, 0x74, 0x83, 0x04, 0x19, 0x07, 0x00, 0x10, 0x00, 0x90, 0x07, 0x90, 0x07, 0x90, 0x07, 0x92, 0x07, 0x11},  //!< OUT[0:3] 2500MHZ, OUT4:625MHz, EZSync Ref Aligned
  {0x05, 0xba, 0x00, 0x74, 0xa3, 0x04, 0x06, 0x07, 0x00, 0x12, 0x00, 0x90, 0x06, 0x90, 0x06, 0x90, 0x06, 0x92, 0x06, 0x11},  //!< OUT[1:3] 2400MHZ, OUT0 & OUT4:600MHz, EZParallelSync
  {0x05, 0xba, 0x00, 0x74, 0x83, 0x04, 0x19, 0x07, 0x00, 0x10, 0xc0, 0x90, 0x07, 0x90, 0x07, 0x90, 0x07, 0x92, 0x07, 0x11},  //!< OUT[0:3] 2500MHZ, OUT4:625MHz, ParallelSync
  {0x00, 0xba, 0x00, 0x74, 0x93, 0x04, 0x14, 0x07, 0x00, 0x20, 0xc0, 0xd3, 0x05, 0x30, 0x00, 0xdb, 0x09, 0x95, 0x05, 0x00},  //!< Same frequency as DC2226 U10 device
  {0x00, 0xba, 0x00, 0x74, 0x93, 0x04, 0x14, 0x07, 0x00, 0x20, 0xc0, 0xd3, 0x05, 0x30, 0x00, 0xdb, 0x09, 0x9b, 0x09, 0x00}   //!< Same frequency as DC2226 U13 device
};  //!< LTC6951 Configuration look-up table


/* -------------------------------------------------------------------------
  FUNCTION: LTC6951_read
  - reads 8 bit Data field to LTC6951.
  - has to shift data by one bit to account for RW bit
 -------------------------------------------------------------------------- */
uint8_t LTC6951_read(uint8_t cs, int8_t address)
{
  int8_t address_shift;
  LT_union_int16_2bytes rx;

  address_shift =(address << 1) | 0x01; // shift to left to account for R/W bit, set bit high for read
  spi_transfer_word(cs, address_shift<<8 , &rx.LT_uint16);

  LTC6951_reg[address]=rx.LT_byte[0];
  return(rx.LT_byte[0]);
}


/* -------------------------------------------------------------------------
  FUNCTION: LTC6951_read_field
  For SPI FIELDS located in 1 or multiple address location
  - reads specific address locations
  - identifies and returns specific field in question
    - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
--------------------------------------------------------------------------- */
long LTC6951_read_field(uint8_t cs, uint8_t address, uint8_t MSB_loc, uint8_t numbits)
{
  int bit_shift, i, num_reg;
  long field_val, maskbits, pow2;

  num_reg=0;
  field_val=0;
// determines how many register are used
  do
  {
    bit_shift = (MSB_loc+1)- (numbits-num_reg*8);   // determines bit_shift for last register location
    field_val=LTC6951_read(cs, (address+num_reg))+(field_val<<8);  // reads current address locations, shifts previous address location 8 bits
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
  FUNCTION: get_LTC6951_SPI_FIELD
  For SPI FIELDS
  - reads specific address locations
  - identifies and returns specific field in question
    - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
--------------------------------------------------------------------------- */
long get_LTC6951_SPI_FIELD(uint8_t cs, uint8_t f)
{

  return LTC6951_read_field(cs, LTC6951_spi_map[f][ADDRx], LTC6951_spi_map[f][DxMSB], LTC6951_spi_map[f][NUMBITS]);
}

/* -------------------------------------------------------------------------
  FUNCTION: LTC6951_write
  - writes 8 bit Data field to LTC6951.
  - has to shift data by one bit to account for RW bit
--------------------------------------------------------------------------- */
void LTC6951_write(uint8_t cs, uint8_t address, uint8_t Data)
{
  LT_union_int16_2bytes rx;

  address=address << 1; // shift to left to account for R/W bit
  spi_transfer_word(cs, (address<<8) | Data, &rx.LT_uint16);
}


/* -------------------------------------------------------------------------
 FUNCTION: LTC6951_write_field
 For SPI FIELDS
 - reads specific address location
 - identifies and returns specific field in question
   - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
---------------------------------------------------------------------------- */
uint8_t LTC6951_write_field(uint8_t cs, long field_data, uint8_t address, uint8_t MSB_loc, uint8_t numbits)
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
    current_content=LTC6951_read(cs, (address+num_reg)) + (current_content<<8);

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
    LTC6951_write(cs, (address+i), reg_val);
  } // end of for loop
} // end of LTC6951_write_field


/* -------------------------------------------------------------------------
   FUNCTION: get_LTC6951_REGSIZE
   - returns # of addresses in parts register map (array size)
---------------------------------------------------------------------------- */
uint8_t get_LTC6951_REGSIZE()
{
  return sizeof(LTC6951_reg);
}


/* -------------------------------------------------------------------------
   FUNCTION: get_LTC6951_SPI_FIELD_NUMBITS
   - returns the number of bits for a given field name in the SPI map
---------------------------------------------------------------------------- */
uint8_t get_LTC6951_SPI_FIELD_NUMBITS(uint8_t f)
{
  return LTC6951_spi_map[f][NUMBITS];
}


/* -------------------------------------------------------------------------
   FUNCTION: get_LTC6951_SPI_FIELD_RW
   - returns if the given field name is (0)read/write or (1)read_only field
---------------------------------------------------------------------------- */
uint8_t get_LTC6951_SPI_FIELD_RW(uint8_t f)
{
  return LTC6951_spi_map[f][R_ONLY];
}


/* -------------------------------------------------------------------------
   FUNCTION: set_LTC6951_SPI_FIELD
   For SPI FIELDS
   - reads specific address location
   - identifies and returns specific field in question
   - can handle SPI fields in multiple addresses, if MSB bit is in the lower number address
---------------------------------------------------------------------------- */
void set_LTC6951_SPI_FIELD(uint8_t cs, uint8_t f, long field_data)
{
  LTC6951_write_field(cs, field_data, LTC6951_spi_map[f][ADDRx], LTC6951_spi_map[f][DxMSB], LTC6951_spi_map[f][NUMBITS]);
}


/* -------------------------------------------------------------------------
   FUNCTION: set_LTC6951_ALLREGS
   - writes data to all registers at once
--------------------------------------------------------------------------- */
void set_LTC6951_ALLREGS(uint8_t cs, uint8_t reg01, uint8_t reg02, uint8_t reg03, uint8_t reg04, uint8_t reg05, uint8_t reg06, uint8_t reg07,
                         uint8_t reg08, uint8_t reg09, uint8_t reg0A, uint8_t reg0B, uint8_t reg0C, uint8_t reg0D, uint8_t reg0E, uint8_t reg0F,
                         uint8_t reg10, uint8_t reg11, uint8_t reg12)
{
  uint8_t i;

  LTC6951_reg[1] = reg01;
  LTC6951_reg[2] = reg02;
  LTC6951_reg[3] = reg03;
  LTC6951_reg[4] = reg04;
  LTC6951_reg[5] = reg05;
  LTC6951_reg[6] = reg06;
  LTC6951_reg[7] = reg07;
  LTC6951_reg[8] = reg08;
  LTC6951_reg[9] = reg09;
  LTC6951_reg[10] = reg0A;
  LTC6951_reg[11] = reg0B;
  LTC6951_reg[12] = reg0C;
  LTC6951_reg[13] = reg0D;
  LTC6951_reg[14] = reg0E;
  LTC6951_reg[15] = reg0F;
  LTC6951_reg[16] = reg10;
  LTC6951_reg[17] = reg11;
  LTC6951_reg[18] = reg12;


  for (i=1; i<19; i++)  LTC6951_write(cs, i, LTC6951_reg[i]);
} // end of set_LTC6951_ALLREGS


/* -------------------------------------------------------------------------
   FUNCTION: set_LTC6951_REGS_lkup_tbl
   - writes LTC6951 registers from a look up table
   - uses the Multi-byte write option for more efficient data transfer
   - Calibrates VCO based on RAO and Autocal bit vales
   - Either SPI Syncs or recommends the use to SYNC pin based on RAO, SN and SR values
--------------------------------------------------------------------------- */
void set_LTC6951_REGS_lkup_tbl(uint8_t lkup_tbl_row)
{
  uint8_t val_temp, i, RAO_bit, AUTOCAL_bit, SN_bit, SR_bit;
  uint8_t val_reg2, val_reg3, val_regA;
  uint8_t *rx;

//! ************** SPI REGISTER WRITE SECTION ************
  output_low(QUIKEVAL_CS);               //! 1) CS low, enable SPI
  val_temp = 1<<1;                       //! 2) addr 1, shift 1 bit to account for RW bit (W=0)
  *rx = SPI.transfer(val_temp);          //! 3) send ADDR1 byte
  for (i=1; i<19; i++)
  {
    val_temp=LTC6951_lkup_tbl[lkup_tbl_row][i];
    *rx = SPI.transfer(val_temp);       //! 4) send REG byte
  }
  output_high(QUIKEVAL_CS);              //! 5) CS high, disable SPI

//! **************** CALIBRATION  SECTION ******************
//! Calibration required after Register writes if RAO=1 or if RAO=AUTOCAL=0.
  val_reg2 = LTC6951_lkup_tbl[lkup_tbl_row][2];  //! CAL bit is located @ reg 2, bit 0, AUTOCAL bit reg 2, bit 3
  val_reg3 = LTC6951_lkup_tbl[lkup_tbl_row][3];  //! RAO bit is located @ reg 3, bit 2;  SSYNC bit @ reg 3, bit 4
  RAO_bit = (val_reg3 & 0x04)>>2;
  AUTOCAL_bit = (val_reg3 & 0x08)>>3;

  val_regA = LTC6951_lkup_tbl[lkup_tbl_row][10];  //! SN, SR bit are located @ reg A, bits 7.6
  SN_bit = (val_regA & 0x80)>>7;
  SR_bit = (val_regA & 0x40)>>6;

  if ((RAO_bit ==1) || (AUTOCAL_bit ==0))  //! checks if RAO=1 or AUTOCAL bit = 0
  {
    val_reg2 = val_reg2 | 0x01;           //! sets CAL bit = 1
    output_low(QUIKEVAL_CS);              //! CS low, enable SPI
    val_temp = 2<<1;                      //! addr 2, shift 1 bit to account for RW bit (W=0)
    *rx = SPI.transfer(val_temp);         //! send ADDR2 byte + RW byte
    *rx = SPI.transfer(val_reg2);         //! send REG2 byte w/ CAL=1
    output_high(QUIKEVAL_CS);             //! CS high, disable SPI
    val_reg2 = LTC6951_lkup_tbl[lkup_tbl_row][2];  //! CAL bit auto-clears after Calibration, set val_reg2 back to look-up table value
  }
  delay(1);                             //! wait 1ms for Calibration to complete, calibration usually takes <100us

//! **************** OUTPUT SYNC SECTION ******************
//! SSYNC can be used for EZSYNC StandAlone, EZSync Ref aligned standalone, EZParallelSYnc, EZ204Sync
//!  - EZSync Standalone, RAO=0
//!  - EZParallelSync, EZ204Sync, EZSync ref aligned: RA0=1, SN=SR=0
//! For ParallelSync or EZSync Controller to work the the SYNC pin should be used
//!  - EZSync Controller, RAO=0 (User will have to remember if its a Controller as there is not a bit to distinguish this)
//!  - ParallelSync: RA0=1, SN or SR= 1
//! See datasheet for timing requirements
  if ((RAO_bit==0) ||  ((RAO_bit == 1) && ((SN_bit+SR_bit)==0)))
  {
    val_reg2 = val_reg2 | 0x04;            //! Sets SSYNC bit =1
    output_low(QUIKEVAL_CS);               //! CS low, enable SPI
    val_temp = 2<<1;                       //! addr 2, shift 1 bit to account for RW bit (W=0)
    *rx = SPI.transfer(val_temp);          //! send ADDR2 byte + RW byte
    *rx = SPI.transfer(val_reg2);          //! send REG2 byte w/ CAL=1
    output_high(QUIKEVAL_CS);              //! CS high, disable SPI
    delay(1);                              //! SSYNC bit remains high for a minimum of 1ms

    val_reg2 = val_reg2 - 0x04;            //! Sets SSYNC bit  back to 0
    output_low(QUIKEVAL_CS);               //! CS low, enable SPI
    val_temp = 2<<1;                       //! addr 2, shift 1 bit to account for RW bit (W=0)
    *rx = SPI.transfer(val_temp);          //! send ADDR2 byte + RW byte
    *rx = SPI.transfer(val_reg2);          //! send REG2 byte w/ CAL=1
    output_high(QUIKEVAL_CS);              //! CS high, disable SPI
  }

//! **************** INFO SECTION ******************
//! INFO ONLY: Print Register Map written to part
  for (i=1; i<19; i++)
  {
    val_temp=LTC6951_lkup_tbl[lkup_tbl_row][i];
    Serial.print(F("ADDR"));
    if (i<16) Serial.print("0");
    Serial.print(i, HEX);
    Serial.print(F(" = 0x"));
    Serial.println(val_temp, HEX);
  }

  if (RAO_bit == 0)
  {
    Serial.println(F(" ***************************************************************************** "));
    Serial.println(F("If this is a EZSync Standalone Device ignore this message - LTC6951 has been SYNC'd via SPI "));
    Serial.println(F("If this is a EZSync Controller Device toggle all EZSync Device's Sync pins "));
    Serial.println(F("EZSync TIMING: Sync Pins must remain high for 1ms, and with <10us skew between all EZSync Sync pins "));
    Serial.println(F(" ***************************************************************************** "));
  }
  if ( (RAO_bit == 1) && ( (SN_bit+SR_bit)>0 ) )
  {
    Serial.println(F(" ***************************************************************************** "));
    Serial.println(F("ParallelSync Timing: See Datasheet for setup and hold times "));
    Serial.println(F("USER MUST TOGGLE SYNC PINS ACROSS ALL PARALLLESYNC DEVICES "));
    Serial.println(F("If a DC2430 is available:"));
    Serial.println(F(" - DC2430's J20-J22 circuitry can be used to create correct SYNC to REF setup and hold times"));
    Serial.println(F(" ***************************************************************************** "));
  }

} // end of set_LTC6951_REGS_lkup_tbl

/* -------------------------------------------------------------------------
   FUNCTION: set_LTC6951_REGS_freq_jump
   - used to toggle between two frequencies
   - if measuring calibration or settling times set SPI clock to max SPI rate (8MHz)
   = writes LTC6951 registers from a local look up table
   - uses the Multi-byte write option for more efficient data transfer
   - Calibrates VCO based on RAO and Autocal bit vales
   - DOES NOTE SYNC OUTPUTS
   - RECOMMENDATION: If measuring settling times with E5052, NOT MUTING OUTPUT DURING CAL can provide more meaningful results
--------------------------------------------------------------------------- */
void set_LTC6951_REGS_freq_jump(uint8_t lkup_tbl_row, uint8_t init_setup)
{
  boolean REGx3toREGx5_change=false;
  boolean REGx9toREGx12_change=false;
  boolean REGx7toREGx8_change=false;
  uint8_t start_addr, end_addr, val_temp, i, RAO_bit, AUTOCAL_bit, SN_bit, SR_bit;
  uint8_t val_reg2, val_reg3, val_regA;
  uint8_t *rx;
// Modifiy the 2 registers to try different settings.  LTC6951Wizard has an export register option that supplies the below format.
  uint8_t local6951_lkup_tbl[2][LTC6951_NUM_REGADDR] =   //!< the following settings assume a 100MHz reference input
  {
//{0x05, 0xba, 0x00, 0x74, 0xa3, 0x04, 0x06, 0x07, 0x00, 0x02, 0xc0, 0x82, 0x06, 0x82, 0x06, 0x82, 0x06, 0x82, 0x06, 0x11}, //! ALL CHAN 600MHz, ParallelSync
//{0x05, 0xba, 0x00, 0x78, 0xb3, 0x04, 0x31, 0x07, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x81, 0x00, 0x11}, //! ALL CHAN 1225MHz, EZSync
    {0x05, 0xba, 0x00, 0x78, 0xb3, 0x04, 0x30, 0x07, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x11},  //!< ALL CHAN 600MHZ, EZSync
    {0x05, 0xba, 0x00, 0x78, 0xb3, 0x04, 0x2c, 0x07, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x11}  //!< ALL CHAN 550MHZ, EZSync
  };//

// inital setup program all registers
  if (init_setup)
  {
    //! ************** INIT: SPI REGISTERS (ALL) WRITE SECTION ************
    output_low(QUIKEVAL_CS);               //! 1) CS low, enable SPI
    start_addr = 1;
    val_temp = start_addr<<1;              //! 2) addr 1, shift 1 bit to account for RW bit (W=0)
    *rx = SPI.transfer(val_temp);          //! 3) send ADDR1 byte
    for (i=start_addr; i<19; i++)
    {
      val_temp=local6951_lkup_tbl[lkup_tbl_row][i];
      *rx = SPI.transfer(val_temp);       //! 4) send REG byte
    }
    output_high(QUIKEVAL_CS);              //! 5) CS high, disable SPI
  } // end if(init_setup)

// checks if b or r-divider change?
  REGx3toREGx5_change=false;
  for (i=3; i<=5; i++)
  {
    if (local6951_lkup_tbl[0][i] != local6951_lkup_tbl[1][i]) REGx3toREGx5_change=true;
  }

// checks if p-divider change?
  REGx7toREGx8_change=false;
  for (i=7; i<=8; i++)
  {
    if (local6951_lkup_tbl[0][i] != local6951_lkup_tbl[1][i]) REGx7toREGx8_change=true;
  }

// checks if output divider change?
  REGx9toREGx12_change=false;
  for (i=9; i<=0x12; i++)
  {
    if (local6951_lkup_tbl[0][i] != local6951_lkup_tbl[1][i]) REGx9toREGx12_change=true;
  }

//! **************** SELECTION SECTION ******************
//! Calibration required after Register writes if RAO=1 or if RAO=AUTOCAL=0.
  val_reg2 = local6951_lkup_tbl[lkup_tbl_row][2];  //! CAL bit is located @ reg 2, bit 0, AUTOCAL bit reg 2, bit 3
  val_reg3 = local6951_lkup_tbl[lkup_tbl_row][3];  //! RAO bit is located @ reg 3, bit 2;  SSYNC bit @ reg 3, bit 4
  RAO_bit = (val_reg3 & 0x04)>>2;
  AUTOCAL_bit = (val_reg3 & 0x08)>>3;

//! All Settings of RAO and AUTOCAL Run this section
//! ************** SPI REGISTERS 4-19 WRITE SECTION ************
//! it possible the that register 6 is the only register that needs to change (N-divider),
//! but this codes was written for any and all frequency changes.
  start_addr = 6;                             //! ADDR6 includes include ND
  if (REGx3toREGx5_change==true) start_addr=3; //! ADDR4 includes include BD (cal-divider), ADDR 5 includes RD
  end_addr=6;                                 //! ADDR6 includes include ND
  if (REGx7toREGx8_change==true) end_addr=8;  //! ADDR8 includes include PD
  if (REGx9toREGx12_change==true) end_addr=12; //! ADDR9-12 include output dividers

  output_low(QUIKEVAL_CS);               //! CS low, enable SPI
  val_temp = start_addr<<1;              //! addr 4, shift 1 bit to account for RW bit (W=0)
  *rx = SPI.transfer(val_temp);          //! send ADDR byte
  for (i=start_addr; i<=end_addr; i++)
  {
    val_temp=local6951_lkup_tbl[lkup_tbl_row][i];
    *rx = SPI.transfer(val_temp);       //! send REG bytes
  }
  output_high(QUIKEVAL_CS);              //! CS high, disable SPI


//! If Autocal=0 or RAO_bit=1 (ParallelSync, EZParallelSync, EZ204Sync, EZSync Ref Aligned
//! Then the cal bit needs to be toggle, with Linduino it maybe fastest to continue
//! The multi-byte write operation.  (reason: The CS L-H transistion has a delay)
  if (RAO_bit  || !AUTOCAL_bit)
  {
    start_addr = 2;
    val_temp = start_addr<<1;             //! ADDR, shift 1 bit to account for RW bit (W=0)
    val_reg2 = val_reg2 | 0x01;           //! sets CAL bit loc = 1

    output_low(QUIKEVAL_CS);              //! CS low, enable SPI
    *rx = SPI.transfer(val_temp);         //! send ADDR2 byte
    *rx = SPI.transfer(val_reg2);         //! send REG2 byte
    output_high(QUIKEVAL_CS);             //! CS high, disable SPI
  }

}  // end of set_LTC6951_REGS_freq_jump


/* -------------------------------------------------------------------------
   FUNCTION: LTC6951_init
   - initializes the SPI MAP
   - for ease of programming there is spreadsheet that automates this some.
----------------------------------------------------------------------------*/
void LTC6951_init()
{
// look up table

// spi map
  LTC6951_spi_map[LTC6951_ALCCAL][ADDRx]=0x03;
  LTC6951_spi_map[LTC6951_ALCCAL][DxMSB]= 5;
  LTC6951_spi_map[LTC6951_ALCCAL][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_ALCEN][ADDRx]=0x03;
  LTC6951_spi_map[LTC6951_ALCEN][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_ALCEN][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_ALCHI][ADDRx]=0x00;
  LTC6951_spi_map[LTC6951_ALCHI][DxMSB]= 5;
  LTC6951_spi_map[LTC6951_ALCHI][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_ALCLO][ADDRx]=0x00;
  LTC6951_spi_map[LTC6951_ALCLO][DxMSB]= 4;
  LTC6951_spi_map[LTC6951_ALCLO][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_ALCMON][ADDRx]=0x03;
  LTC6951_spi_map[LTC6951_ALCMON][DxMSB]= 6;
  LTC6951_spi_map[LTC6951_ALCMON][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_ALCULOK][ADDRx]=0x03;
  LTC6951_spi_map[LTC6951_ALCULOK][DxMSB]= 4;
  LTC6951_spi_map[LTC6951_ALCULOK][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_AUTOCAL][ADDRx]=0x03;
  LTC6951_spi_map[LTC6951_AUTOCAL][DxMSB]= 3;
  LTC6951_spi_map[LTC6951_AUTOCAL][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_BD][ADDRx]=0x04;
  LTC6951_spi_map[LTC6951_BD][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_BD][NUMBITS]= 4;
  LTC6951_spi_map[LTC6951_BST][ADDRx]=0x03;
  LTC6951_spi_map[LTC6951_BST][DxMSB]= 1;
  LTC6951_spi_map[LTC6951_BST][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_CAL][ADDRx]=0x02;
  LTC6951_spi_map[LTC6951_CAL][DxMSB]= 0;
  LTC6951_spi_map[LTC6951_CAL][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_CP][ADDRx]=0x07;
  LTC6951_spi_map[LTC6951_CP][DxMSB]= 2;
  LTC6951_spi_map[LTC6951_CP][NUMBITS]= 3;
  LTC6951_spi_map[LTC6951_CPDN][ADDRx]=0x07;
  LTC6951_spi_map[LTC6951_CPDN][DxMSB]= 3;
  LTC6951_spi_map[LTC6951_CPDN][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_CPMID][ADDRx]=0x07;
  LTC6951_spi_map[LTC6951_CPMID][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_CPMID][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_CPRST][ADDRx]=0x07;
  LTC6951_spi_map[LTC6951_CPRST][DxMSB]= 5;
  LTC6951_spi_map[LTC6951_CPRST][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_CPUP][ADDRx]=0x07;
  LTC6951_spi_map[LTC6951_CPUP][DxMSB]= 4;
  LTC6951_spi_map[LTC6951_CPUP][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_CPWIDE][ADDRx]=0x07;
  LTC6951_spi_map[LTC6951_CPWIDE][DxMSB]= 6;
  LTC6951_spi_map[LTC6951_CPWIDE][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_DLY0][ADDRx]=0x0a;
  LTC6951_spi_map[LTC6951_DLY0][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_DLY0][NUMBITS]= 8;
  LTC6951_spi_map[LTC6951_DLY1][ADDRx]=0x0c;
  LTC6951_spi_map[LTC6951_DLY1][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_DLY1][NUMBITS]= 8;
  LTC6951_spi_map[LTC6951_DLY2][ADDRx]=0x0e;
  LTC6951_spi_map[LTC6951_DLY2][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_DLY2][NUMBITS]= 8;
  LTC6951_spi_map[LTC6951_DLY3][ADDRx]=0x10;
  LTC6951_spi_map[LTC6951_DLY3][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_DLY3][NUMBITS]= 8;
  LTC6951_spi_map[LTC6951_DLY4][ADDRx]=0x12;
  LTC6951_spi_map[LTC6951_DLY4][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_DLY4][NUMBITS]= 8;
  LTC6951_spi_map[LTC6951_FILT][ADDRx]=0x03;
  LTC6951_spi_map[LTC6951_FILT][DxMSB]= 0;
  LTC6951_spi_map[LTC6951_FILT][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_INVSTAT][ADDRx]=0x01;
  LTC6951_spi_map[LTC6951_INVSTAT][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_INVSTAT][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_LKCT][ADDRx]=0x04;
  LTC6951_spi_map[LTC6951_LKCT][DxMSB]= 1;
  LTC6951_spi_map[LTC6951_LKCT][NUMBITS]= 2;
  LTC6951_spi_map[LTC6951_LKWIN][ADDRx]=0x04;
  LTC6951_spi_map[LTC6951_LKWIN][DxMSB]= 2;
  LTC6951_spi_map[LTC6951_LKWIN][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_LOCK][ADDRx]=0x00;
  LTC6951_spi_map[LTC6951_LOCK][DxMSB]= 2;
  LTC6951_spi_map[LTC6951_LOCK][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_NLOCK][ADDRx]=0x00;
  LTC6951_spi_map[LTC6951_NLOCK][DxMSB]= 3;
  LTC6951_spi_map[LTC6951_NLOCK][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_MCO][ADDRx]=0x09;
  LTC6951_spi_map[LTC6951_MCO][DxMSB]= 5;
  LTC6951_spi_map[LTC6951_MCO][NUMBITS]= 2;
  LTC6951_spi_map[LTC6951_MC1][ADDRx]=0x0b;
  LTC6951_spi_map[LTC6951_MC1][DxMSB]= 5;
  LTC6951_spi_map[LTC6951_MC1][NUMBITS]= 2;
  LTC6951_spi_map[LTC6951_MC2][ADDRx]=0x0d;
  LTC6951_spi_map[LTC6951_MC2][DxMSB]= 5;
  LTC6951_spi_map[LTC6951_MC2][NUMBITS]= 2;
  LTC6951_spi_map[LTC6951_MC3][ADDRx]=0x0f;
  LTC6951_spi_map[LTC6951_MC3][DxMSB]= 5;
  LTC6951_spi_map[LTC6951_MC3][NUMBITS]= 2;
  LTC6951_spi_map[LTC6951_MC4][ADDRx]=0x11;
  LTC6951_spi_map[LTC6951_MC4][DxMSB]= 5;
  LTC6951_spi_map[LTC6951_MC4][NUMBITS]= 2;
  LTC6951_spi_map[LTC6951_MD0][ADDRx]=0x09;
  LTC6951_spi_map[LTC6951_MD0][DxMSB]= 3;
  LTC6951_spi_map[LTC6951_MD0][NUMBITS]= 4;
  LTC6951_spi_map[LTC6951_MD1][ADDRx]=0x0b;
  LTC6951_spi_map[LTC6951_MD1][DxMSB]= 3;
  LTC6951_spi_map[LTC6951_MD1][NUMBITS]= 4;
  LTC6951_spi_map[LTC6951_MD2][ADDRx]=0x0d;
  LTC6951_spi_map[LTC6951_MD2][DxMSB]= 3;
  LTC6951_spi_map[LTC6951_MD2][NUMBITS]= 4;
  LTC6951_spi_map[LTC6951_MD3][ADDRx]=0x0f;
  LTC6951_spi_map[LTC6951_MD3][DxMSB]= 3;
  LTC6951_spi_map[LTC6951_MD3][NUMBITS]= 4;
  LTC6951_spi_map[LTC6951_MD4][ADDRx]=0x11;
  LTC6951_spi_map[LTC6951_MD4][DxMSB]= 3;
  LTC6951_spi_map[LTC6951_MD4][NUMBITS]= 4;
  LTC6951_spi_map[LTC6951_MUTE0][ADDRx]=0x08;
  LTC6951_spi_map[LTC6951_MUTE0][DxMSB]= 0;
  LTC6951_spi_map[LTC6951_MUTE0][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_MUTE1][ADDRx]=0x08;
  LTC6951_spi_map[LTC6951_MUTE1][DxMSB]= 1;
  LTC6951_spi_map[LTC6951_MUTE1][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_MUTE2][ADDRx]=0x08;
  LTC6951_spi_map[LTC6951_MUTE2][DxMSB]= 2;
  LTC6951_spi_map[LTC6951_MUTE2][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_MUTE3][ADDRx]=0x08;
  LTC6951_spi_map[LTC6951_MUTE3][DxMSB]= 3;
  LTC6951_spi_map[LTC6951_MUTE3][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_MUTE4][ADDRx]=0x08;
  LTC6951_spi_map[LTC6951_MUTE4][DxMSB]= 4;
  LTC6951_spi_map[LTC6951_MUTE4][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_ND][ADDRx]=0x05;
  LTC6951_spi_map[LTC6951_ND][DxMSB]= 1;
  LTC6951_spi_map[LTC6951_ND][NUMBITS]= 10;
  LTC6951_spi_map[LTC6951_OINV0][ADDRx]=0x09;
  LTC6951_spi_map[LTC6951_OINV0][DxMSB]= 6;
  LTC6951_spi_map[LTC6951_OINV0][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_OINV1][ADDRx]=0x0b;
  LTC6951_spi_map[LTC6951_OINV1][DxMSB]= 6;
  LTC6951_spi_map[LTC6951_OINV1][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_OINV2][ADDRx]=0x0d;
  LTC6951_spi_map[LTC6951_OINV2][DxMSB]= 6;
  LTC6951_spi_map[LTC6951_OINV2][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_OINV3][ADDRx]=0x0f;
  LTC6951_spi_map[LTC6951_OINV3][DxMSB]= 6;
  LTC6951_spi_map[LTC6951_OINV3][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_OINV4][ADDRx]=0x11;
  LTC6951_spi_map[LTC6951_OINV4][DxMSB]= 6;
  LTC6951_spi_map[LTC6951_OINV4][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_PART][ADDRx]=0x13;
  LTC6951_spi_map[LTC6951_PART][DxMSB]= 3;
  LTC6951_spi_map[LTC6951_PART][NUMBITS]= 4;
  LTC6951_spi_map[LTC6951_PD][ADDRx]=0x08;
  LTC6951_spi_map[LTC6951_PD][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_PD][NUMBITS]= 3;
  LTC6951_spi_map[LTC6951_PDALL][ADDRx]=0x02;
  LTC6951_spi_map[LTC6951_PDALL][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_PDALL][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_PDOUT][ADDRx]=0x02;
  LTC6951_spi_map[LTC6951_PDOUT][DxMSB]= 4;
  LTC6951_spi_map[LTC6951_PDOUT][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_PDPLL][ADDRx]=0x02;
  LTC6951_spi_map[LTC6951_PDPLL][DxMSB]= 6;
  LTC6951_spi_map[LTC6951_PDPLL][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_PDREFPK][ADDRx]=0x02;
  LTC6951_spi_map[LTC6951_PDREFPK][DxMSB]= 3;
  LTC6951_spi_map[LTC6951_PDREFPK][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_PDVCO][ADDRx]=0x02;
  LTC6951_spi_map[LTC6951_PDVCO][DxMSB]= 5;
  LTC6951_spi_map[LTC6951_PDVCO][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_POR][ADDRx]=0x02;
  LTC6951_spi_map[LTC6951_POR][DxMSB]= 1;
  LTC6951_spi_map[LTC6951_POR][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_RAO][ADDRx]=0x03;
  LTC6951_spi_map[LTC6951_RAO][DxMSB]= 2;
  LTC6951_spi_map[LTC6951_RAO][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_RD][ADDRx]=0x05;
  LTC6951_spi_map[LTC6951_RD][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_RD][NUMBITS]= 6;
  LTC6951_spi_map[LTC6951_REFOK][ADDRx]=0x00;
  LTC6951_spi_map[LTC6951_REFOK][DxMSB]= 0;
  LTC6951_spi_map[LTC6951_REFOK][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_NREFOK][ADDRx]=0x00;
  LTC6951_spi_map[LTC6951_NREFOK][DxMSB]= 1;
  LTC6951_spi_map[LTC6951_NREFOK][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_REV][ADDRx]=0x13;
  LTC6951_spi_map[LTC6951_REV][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_REV][NUMBITS]= 4;
  LTC6951_spi_map[LTC6951_SN][ADDRx]=0x0a;
  LTC6951_spi_map[LTC6951_SN][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_SN][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_SR][ADDRx]=0x0a;
  LTC6951_spi_map[LTC6951_SR][DxMSB]= 6;
  LTC6951_spi_map[LTC6951_SR][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_SSYNC][ADDRx]=0x02;
  LTC6951_spi_map[LTC6951_SSYNC][DxMSB]= 2;
  LTC6951_spi_map[LTC6951_SSYNC][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_SYNCEN0][ADDRx]=0x09;
  LTC6951_spi_map[LTC6951_SYNCEN0][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_SYNCEN0][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_SYNCEN1][ADDRx]=0x0b;
  LTC6951_spi_map[LTC6951_SYNCEN1][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_SYNCEN1][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_SYNCEN2][ADDRx]=0x0d;
  LTC6951_spi_map[LTC6951_SYNCEN2][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_SYNCEN2][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_SYNCEN3][ADDRx]=0x0f;
  LTC6951_spi_map[LTC6951_SYNCEN3][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_SYNCEN3][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_SYNCEN4][ADDRx]=0x11;
  LTC6951_spi_map[LTC6951_SYNCEN4][DxMSB]= 7;
  LTC6951_spi_map[LTC6951_SYNCEN4][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_UNLOCK][ADDRx]=0x00;
  LTC6951_spi_map[LTC6951_UNLOCK][DxMSB]= 6;
  LTC6951_spi_map[LTC6951_UNLOCK][NUMBITS]= 1;
  LTC6951_spi_map[LTC6951_x][ADDRx]=0x01;
  LTC6951_spi_map[LTC6951_x][DxMSB]= 6;
  LTC6951_spi_map[LTC6951_x][NUMBITS]= 7;

  LTC6951_spi_map[LTC6951_ALCCAL][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_ALCEN][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_ALCHI][R_ONLY]= 1;
  LTC6951_spi_map[LTC6951_ALCLO][R_ONLY]= 1;
  LTC6951_spi_map[LTC6951_ALCMON][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_ALCULOK][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_AUTOCAL][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_BD][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_BST][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_CAL][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_CP][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_CPDN][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_CPMID][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_CPRST][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_CPUP][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_CPWIDE][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_DLY0][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_DLY1][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_DLY2][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_DLY3][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_DLY4][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_FILT][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_INVSTAT][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_LKCT][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_LKWIN][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_LOCK][R_ONLY]= 1;
  LTC6951_spi_map[LTC6951_NLOCK][R_ONLY]= 1;
  LTC6951_spi_map[LTC6951_MCO][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_MC1][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_MC2][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_MC3][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_MC4][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_MD0][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_MD1][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_MD2][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_MD3][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_MD4][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_MUTE0][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_MUTE1][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_MUTE2][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_MUTE3][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_MUTE4][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_ND][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_OINV0][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_OINV1][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_OINV2][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_OINV3][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_OINV4][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_PART][R_ONLY]= 1;
  LTC6951_spi_map[LTC6951_PD][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_PDALL][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_PDOUT][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_PDPLL][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_PDREFPK][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_PDVCO][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_POR][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_RAO][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_RD][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_REFOK][R_ONLY]= 1;
  LTC6951_spi_map[LTC6951_NREFOK][R_ONLY]= 1;
  LTC6951_spi_map[LTC6951_REV][R_ONLY]= 1;
  LTC6951_spi_map[LTC6951_SN][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_SR][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_SSYNC][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_SYNCEN0][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_SYNCEN1][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_SYNCEN2][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_SYNCEN3][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_SYNCEN4][R_ONLY]= 0;
  LTC6951_spi_map[LTC6951_UNLOCK][R_ONLY]= 1;
  LTC6951_spi_map[LTC6951_x][R_ONLY]= 0;
} // end of LTC6951_init



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

