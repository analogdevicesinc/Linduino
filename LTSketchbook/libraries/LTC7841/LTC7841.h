/*!
LTC7841: PolyPhase Synchronous Boost Controller with PMBus Interface

@verbatim

The LTC7841 is a high performance PolyPhase® single output
synchronous boost converter controller that drives two
N-channel power MOSFET stages out-of-phase.

@endverbatim

http://www.analog.com/products/LTC7841

Copyright 2020(c) Analog Devices, Inc.

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

//! @ingroup Power_System_Management
//! @{
//! @defgroup LTC7841: PolyPhase Synchronous Boost Controller with PMBus Interface
//! @}

/*! @file
    @ingroup LTC7841
  Header for LTC7841: PolyPhase Synchronous Boost Controller with PMBus Interface

*/

#ifndef LTC7841_h
#define LTC7841_h

//! @name LTC7841 DEVICE I2C ADDRESSES (7-bit)
// GLOBAL ADDRESS (7-BIT)
#define LTC7841_I2C_GLOBAL_ADDRESS          0x5B
#define LTC7841_I2C_PAGED_GLOBAL_ADDRESS    0x5A
#define LTC7841_I2C_ARA_ADDRESS             0x0C

//DEVICE ADDRESSES (ASEL)
//#define LTC7841_I2C_ADDRESS       0x20
//#define LTC7841_I2C_ADDRESS       0x21
//#define LTC7841_I2C_ADDRESS       0x22
//#define LTC7841_I2C_ADDRESS       0x23
//#define LTC7841_I2C_ADDRESS       0x24
//#define LTC7841_I2C_ADDRESS       0x25
//#define LTC7841_I2C_ADDRESS       0x26
//#define LTC7841_I2C_ADDRESS       0x27

//! @}

/********************************************************************************/
//! @name LTC7841 command definitions

#define LTC7841_OPERATION               0x01
#define LTC7841_VOUT_MODE               0x20
#define LTC7841_STATUS_WORD             0x79
#define LTC7841_READ_VIN                0x88
#define LTC7841_READ_IIN                0x89
#define LTC7841_READ_VOUT               0x8B
#define LTC7841_READ_IOUT               0x8C
#define LTC7841_READ_TEMPERATURE_1      0x8D
#define LTC7841_PMBUS_REVISION          0x98
#define LTC7841_MFR_IOUT_PEAK           0xD7
#define LTC7841_MFR_VOUT_PEAK           0xDD
#define LTC7841_MFR_VIN_PEAK            0xDE
#define LTC7841_MFR_TEMPERATURE1_PEAK   0xDF
#define LTC7841_MFR_IIN_PEAK            0xE1
#define LTC7841_MFR_CLEAR_PEAKS         0xE3
#define LTC7841_MFR_VOUT_MARGIN_HIGH    0xE5
#define LTC7841_MFR_SPECIAL_ID          0xE7
#define LTC7841_MFR_VOUT_COMMAND        0xE8
#define LTC7841_MFR_CONFIG              0xE9
#define LTC7841_MFR_VOUT_MARGIN_LOW     0xED
#define LTC7841_MFR_RAIL_ADDRESS        0xFA
#define LTC7841_MFR_RESET               0xFD

//! @}

/********************************************************************************/
//! @name LTC7841 bit definitions

#define LTC7841_OPERATION_OFF           0x00
#define LTC7841_OPERATION_ON            0x80
#define LTC7841_OPERATION_MARGIN_LOW    0x98
#define LTC7841_OPERATION_MARGIN_HIGH   0xA8

#define LTC7841_CONFIG_NORMAL_SLEW      0x00
#define LTC7841_CONFIG_SLOW_SLEW        0x01
#define LTC7841_CONFIG_FAST_SLEW        0x02

#define LTC7841_CONFIG_TEMP_INTERNAL    0x00
#define LTC7841_CONFIG_TEMP_EXTERNAL    0x04

#define LTC7841_STATUS_NONE             0x0001
#define LTC7841_STATUS_COM              0x0002
#define LTC7841_STATUS_TEMP             0x0004
#define LTC7841_STATUS_VOUT_OV          0x0020
#define LTC7841_STATUS_OFF              0x0040
#define LTC7841_STATUS_PGOODB           0x0800
#define LTC7841_STATUS_VOUT             0x8000

//! @}

#endif
