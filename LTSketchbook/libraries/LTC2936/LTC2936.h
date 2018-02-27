/*!
LTC2936: Programable Hex Voltage Supervisor with Comparator Outputs and EEPROM

@verbatim

@endverbatim

http://www.linear.com/product/LTC2936

http://www.linear.com/demo/DC1633B


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

//! @ingroup Power_System_Management
//! @{
//! @defgroup LTC2936 LTC2936: Programable Hex Voltage Supervisor with Comparator Outputs and EEPROM
//! @}

/*! @file
    @ingroup LTC2936
  Header for LTC2936: Programable Hex Voltage Supervisor with Comparator Outputs and EEPROM
*/

#ifndef LTC2936_h
#define LTC2936_h

//! @name LTC2933 DEVICE I2C ADDRESSES (7-bit)
// GLOBAL ADDRESS
#define LTC2936_I2C_GLOBAL_ADDRESS    0x73
#define LTC2936_I2C_ARA_ADDRESS     0x0C

//DEVICE ADDRESSES (ASEL1,ASEL0)
//#define LTC2936_I2C_ADDRESS       0x50 // 00 -- WARNING: SAME ADDRESS AS DC1605B EEPROM
//#define LTC2936_I2C_ADDRESS       0x51 // 0Z -- WARNING: SAME ADDRESS AS DC1605B EEPROM
//#define LTC2936_I2C_ADDRESS       0x52 // 01 -- WARNING: SAME ADDRESS AS DC1605B EEPROM
//#define LTC2936_I2C_ADDRESS       0x53 // Z0 -- WARNING: SAME ADDRESS AS DC1605B EEPROM
//#define LTC2936_I2C_ADDRESS       0x54 // ZZ -- WARNING: SAME ADDRESS AS DC1605B EEPROM
//#define LTC2936_I2C_ADDRESS       0x55 // Z1 -- WARNING: SAME ADDRESS AS DC1605B EEPROM
//#define LTC2936_I2C_ADDRESS       0x58 // 10
//#define LTC2936_I2C_ADDRESS       0x59 // 1Z
//#define LTC2936_I2C_ADDRESS       0x5A // 11
//! @}

// WARNING: The DC1605B contains a indentity EEPROM that answers to addresses 0x5n
// WARNING:  where n = 0,1,2,3,4,5,6,7
// WARNING: Unfortunately, the Linduino ties the EEPROM's I2C bus into the main I2C bus
// WARNING:  and essentially reserves these addresses so they cannot be used by the DUT
// WARNING: Consequently, when using Linduino, only LTC2936 addresses 0x58, 0x59, and
// WARNING:  0x5A are usable.
// WARNING: YOU MUST SET JUMPER ASEL1 = 1 !
// NOTE: The above situation only applies to the DC1605B board with Linduino
// NOTE: When the LTC2936 is used in a customer system, different addresses will apply

//! @name LTC2936 REGISTER DEFINITIONS
#define LTC2936_WRITE_PROTECT       0x00
#define LTC2936_GPI_CONFIG        0x01
#define LTC2936_GPIO1_CONFIG        0x02
#define LTC2936_GPIO2_3_CONFIG      0x03
#define LTC2936_V1_THR            0x04
#define LTC2936_V2_THR            0x05
#define LTC2936_V3_THR            0x06
#define LTC2936_V4_THR            0x07
#define LTC2936_V5_THR            0x08
#define LTC2936_V6_THR            0x09
#define LTC2936_V1_CONFIG         0x0A
#define LTC2936_V2_CONFIG         0x0B
#define LTC2936_V3_CONFIG         0x0C
#define LTC2936_V4_CONFIG         0x0D
#define LTC2936_V5_CONFIG         0x0E
#define LTC2936_V6_CONFIG         0x0F
//        0x10
#define LTC2936_HISTORY_WORD        0x11
//        0x12
//        ...
//        0x19
#define LTC2936_PADS            0x1A
#define LTC2936_CLEAR_HISTORY     0x1B
#define LTC2936_STORE_USER        0x1C
#define LTC2936_RESTORE_USER        0x1D
#define LTC2936_BACKUP_WORD       0x1E
#define LTC2936_STATUS_WORD       0x1F
//! @}

#endif
