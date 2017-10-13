/*!
LTC6803-1 Multicell Battery Monitor

http://www.linear.com/product/LTC6803-1

http://www.linear.com/product/LTC6803-1#demoboards

REVISION HISTORY
$Revision: 4432 $
$Date: 2015-11-30 14:03:02 -0800 (Mon, 30 Nov 2015) $

Copyright (c) 2015, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.

Copyright 2015 Linear Technology Corp. (LTC)
*/

/*! @file
    @ingroup LTC68031
    Library Header File for LTC6803-1 Multicell Battery Monitor
*/

#ifndef LTC68031_H
#define LTC68031_H


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

//! Function that writes configuration of LTC6803-1/-3
//! @return void
void LTC6803_wrcfg(uint8_t total_ic,      //!< total_ic number of LTC6803 ICs in stack
                   uint8_t config[][6]      //!< The function will write the 6803 CFGR register with data in the config array
                  );

//! Function that reads configuration of LTC6803-1/-3
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