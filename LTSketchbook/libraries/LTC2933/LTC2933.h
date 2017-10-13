/*!
LTC2933: Programable Hex Voltage Supervisor with EEPROM

@verbatim

@endverbatim

http://www.linear.com/product/LTC2933

http://www.linear.com/demo/DC1633B

REVISION HISTORY
$Revision: 6237 $
$Date: 2016-12-20 15:09:16 -0800 (Tue, 20 Dec 2016) $

Copyright (c) 2014, Linear Technology Corp.(LTC)
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
*/

//! @ingroup Power_System_Management
//! @{
//! @defgroup LTC2933 LTC2933: Programable Hex Voltage Supervisor with EEPROM
//! @}

/*! @file
    @ingroup LTC2933
  Header for LTC2933: Programable Hex Voltage Supervisor with EEPROM
*/

#ifndef LTC2933_h
#define LTC2933_h

//! @name LTC2933 DEVICE I2C ADDRESSES (7-bit)
// GLOBAL ADDRESS
#define LTC2933_I2C_GLOBAL_ADDRESS    0x1B
#define LTC2933_I2C_ARA_ADDRESS     0x0C
//DEVICE ADDRESSES
//#define LTC2933_I2C_ADDRESS 0x1C
//#define LTC2933_I2C_ADDRESS 0x1D
//#define LTC2933_I2C_ADDRESS 0x1E
//! @}

//! @name LTC2933 REGISTER DEFINITIONS
#define LTC2933_WRITE_PROTECT       0x00
#define LTC2933_GPI_CONFIG        0x01
#define LTC2933_GPIO1_CONFIG        0x02
#define LTC2933_GPIO2_3_CONFIG      0x03
#define LTC2933_V1_THR            0x04
#define LTC2933_V2_THR            0x05
#define LTC2933_V3_THR            0x06
#define LTC2933_V4_THR            0x07
#define LTC2933_V5_THR            0x08
#define LTC2933_V6_THR            0x09
#define LTC2933_V1_CONFIG         0x0A
#define LTC2933_V2_CONFIG         0x0B
#define LTC2933_V3_CONFIG         0x0C
#define LTC2933_V4_CONFIG         0x0D
#define LTC2933_V5_CONFIG         0x0E
#define LTC2933_V6_CONFIG         0x0F
//        0x10
#define LTC2933_HISTORY_WORD        0x11
//        0x12
//        ...
//        0x1A
#define LTC2933_CLEAR_HISTORY       0x1B
#define LTC2933_STORE_USER        0x1C
#define LTC2933_RESTORE_USER        0x1D
#define LTC2933_BACKUP_WORD       0x1E
#define LTC2933_STATUS_WORD       0x1F
//! @}

#endif
