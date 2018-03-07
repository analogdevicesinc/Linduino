/*!
LTC2937: Six Channel Sequencer and Voltage Supervisor with EEPROM

@verbatim

The LTC2937 is a six-channel voltage sequencer and supervisor with EEPROM.
It autonomously sequences and supervises six power supplies using configuration stored in ther EEPROM.
It maintains status and fault information in on-board I2C-addressable registers.
It cooperates with up to 50 other LTC2937s to sequence and supervise up to 300 power rails.
It uses the LTpowerPlay as a GUI interface when connected to the I2C bus.

@endverbatim

http://www.linear.com/product/LTC2937

http://www.linear.com/demo/DC2313A


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
//! @defgroup LTC2937 LTC2937: Six Channel Sequencer and Voltage Supervisor with EEPROM
//! @}

/*! @file
    @ingroup LTC2937
  Header for LTC2937: Six Channel Sequencer and Voltage Supervisor with EEPROM

*/

#ifndef LTC2937_h
#define LTC2937_h

//! @name LTC2937 DEVICE I2C ADDRESSES (7-bit)
// GLOBAL ADDRESS (7-BIT)
#define LTC2937_I2C_GLOBAL_ADDRESS    0x36
#define LTC2937_I2C_ARA_ADDRESS     0x0C

//DEVICE ADDRESSES (ASEL3,ASEL2,ASEL1)
//#define LTC2937_I2C_ADDRESS       0x37 // LLL
//#define LTC2937_I2C_ADDRESS       0x38 // LLZ
//#define LTC2937_I2C_ADDRESS       0x39 // LLH
//#define LTC2937_I2C_ADDRESS       0x3A // LZL
//#define LTC2937_I2C_ADDRESS       0x3B // LZZ
//#define LTC2937_I2C_ADDRESS       0x3C // LZH
//#define LTC2937_I2C_ADDRESS       0x3D // LHL
//#define LTC2937_I2C_ADDRESS       0x3E // LHZ
//#define LTC2937_I2C_ADDRESS       0x3F // LHH
//#define LTC2937_I2C_ADDRESS       0x40 // ZLL
//#define LTC2937_I2C_ADDRESS       0x41 // ZLZ
//#define LTC2937_I2C_ADDRESS       0x42 // ZLH
//#define LTC2937_I2C_ADDRESS       0x43 // ZZL
//#define LTC2937_I2C_ADDRESS       0x44 // ZZZ
//#define LTC2937_I2C_ADDRESS       0x45 // ZZH
//#define LTC2937_I2C_ADDRESS       0x46 // ZHL
//#define LTC2937_I2C_ADDRESS       0x47 // ZHZ
//#define LTC2937_I2C_ADDRESS       0x48 // ZHH
//#define LTC2937_I2C_ADDRESS       0x49 // HLL
//#define LTC2937_I2C_ADDRESS       0x4A // HLZ
//#define LTC2937_I2C_ADDRESS       0x4B // HLH
//#define LTC2937_I2C_ADDRESS       0x4C // HZL
//#define LTC2937_I2C_ADDRESS       0x4D // HZZ
//#define LTC2937_I2C_ADDRESS       0x4E // HZH
//#define LTC2937_I2C_ADDRESS       0x4F // HHL
//#define LTC2937_I2C_ADDRESS       0x50 // HHZ
//#define LTC2937_I2C_ADDRESS       0x51 // HHH

//! @}

/********************************************************************************/
//! @name LTC2937 command address definitions

#define LTC2937_WRITE_PROTECTION  0x00
#define LTC2937_SPECIAL_LOT   0x01
#define LTC2937_ON_OFF_CONTROL    0x02
#define LTC2937_V_RANGE     0x03
#define LTC2937_V_THRESHOLD_1   0x04
#define LTC2937_V_THRESHOLD_2   0x05
#define LTC2937_V_THRESHOLD_3   0x06
#define LTC2937_V_THRESHOLD_4   0x07
#define LTC2937_V_THRESHOLD_5   0x08
#define LTC2937_V_THRESHOLD_6   0x09
#define LTC2937_TON_TIMERS_1    0x0A
#define LTC2937_TON_TIMERS_2    0x0B
#define LTC2937_TON_TIMERS_3    0x0C
#define LTC2937_TON_TIMERS_4    0x0D
#define LTC2937_TON_TIMERS_5    0x0E
#define LTC2937_TON_TIMERS_6    0x0F
#define LTC2937_TOFF_TIMERS_1   0x10
#define LTC2937_TOFF_TIMERS_2   0x11
#define LTC2937_TOFF_TIMERS_3   0x12
#define LTC2937_TOFF_TIMERS_4   0x13
#define LTC2937_TOFF_TIMERS_5   0x14
#define LTC2937_TOFF_TIMERS_6   0x15
#define LTC2937_SEQ_UP_POSITION_1 0x16
#define LTC2937_SEQ_UP_POSITION_2 0x17
#define LTC2937_SEQ_UP_POSITION_3 0x18
#define LTC2937_SEQ_UP_POSITION_4 0x19
#define LTC2937_SEQ_UP_POSITION_5 0x1A
#define LTC2937_SEQ_UP_POSITION_6 0x1B
#define LTC2937_SEQ_DOWN_POSITION_1 0x1C
#define LTC2937_SEQ_DOWN_POSITION_2 0x1D
#define LTC2937_SEQ_DOWN_POSITION_3 0x1E
#define LTC2937_SEQ_DOWN_POSITION_4 0x1F
#define LTC2937_SEQ_DOWN_POSITION_5 0x20
#define LTC2937_SEQ_DOWN_POSITION_6 0x21
#define LTC2937_RSTB_CONFIG   0x22
#define LTC2937_FAULT_RESPONSE    0x23
//          0x24
//          0x25
#define LTC2937_MONITOR_STATUS_HISTORY  0x26
//          0x27
#define LTC2937_CLEAR_ALERTB    0x28
#define LTC2937_STATUS_INFORMATION  0x29
#define LTC2937_BREAK_POINT         0x2A
#define LTC2937_SEQ_POSITION_COUNT      0x2B
#define LTC2937_STORE                   0x2C
#define LTC2937_RESTORE                 0x2D
#define LTC2937_CLEAR                   0x2E
#define LTC2937_MONITOR_BACKUP          0x2F
#define LTC2937_MONITOR_STATUS          0x30
#define LTC2937_DEVICE_ID         0x31

//! @}

#endif
