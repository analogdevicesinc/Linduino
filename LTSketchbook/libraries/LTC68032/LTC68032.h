/*!
LTC6803-2 Multicell Battery Monitor

http://www.linear.com/product/LTC6803-2

http://www.linear.com/product/LTC6803-2#demoboards


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

Copyright 2015 Linear Technology Corp. (LTC)
*/

/*! @file
    @ingroup LTC68032
    Library Header File for LTC6803-2 Multicell Battery Monitor
*/

#ifndef LTC68032_H
#define LTC68032_H


#ifndef LTC6803_CS
#define LTC6803_CS QUIKEVAL_CS
#endif

#define PEC_POLY 7

//!Initializes the SPI port
//! @return void
void LTC6803_initialize();

//! Function to start Cell Voltage measurement
//! @return void
void LTC6803_stcvad();

//! Function to start Temp channel voltage measurement
//! @return void
void LTC6803_sttmpad();

//! Function that reads Cell Voltage registers
//! @returns  This function will return a 0 if there is no PEC error and will return -1 if there is a PEC error
uint8_t LTC6803_rdcv(uint8_t total_ic,      //!< total_ic number of LTC6803 ICs in stack
                     uint16_t cell_codes[][12]  //!< The Function will put the parsed measured cell voltages into this array
                    );

//! Function that reads Temp Voltage registers
//! @returns  This function will return a 0 if there is no PEC error and will return -1 if there is a PEC error
int8_t LTC6803_rdtmp(uint8_t total_ic,      //!< total_ic number of LTC6803 ICs in stack
                     uint16_t temp_codes[][3] //!< The Function will put the parsed measured Temp voltages into this array
                    );

//! Function that writes configuration of LTC6803-2/-4
//! @return void
void LTC6803_wrcfg(uint8_t total_ic,      //!< total_ic number of LTC6803 ICs in stack
                   uint8_t config[][6]      //!< The function will write the 6803 CFGR register with data in the config array
                  );

//! Function that reads configuration of LTC6803-2/-4
//! @returns  This function will return a 0 if there is no PEC error and will return -1 if there is a PEC error
int8_t LTC6803_rdcfg(uint8_t total_ic,      //!< total_ic number of LTC6803 ICs in stack
                     uint8_t r_config[][7]    //!< The Function will put the read config register data into this array
                    );
//! Function that calculates PEC byte
//! @returns The calculated CRC8
uint8_t pec8_calc(uint8_t len,          //!< the length of the data array
                  uint8_t *data         //!< data array
                 );

//! Writes an array of bytes out of the SPI port
//! @return void
void spi_write_array(uint8_t length,      //!< length of the data array being written on the SPI port
                     uint8_t *data        //!< the data array to be written on the SPI port
                    );

//! Writes and read a set number of bytes using the SPI port.
//! @return void
void spi_write_read(uint8_t *TxData,      //!< array of data to be written on the SPI port
                    uint8_t TXlen,        //!< length of the tx_data array
                    uint8_t *rx_data,     //!< array that read data will be written too.
                    uint8_t RXlen       //!< number of bytes to be read from the SPI port.
                   );

#endif