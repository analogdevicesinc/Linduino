/*!
Analog Devices EVAL-ADM1293EBZ Demonstration Board
ADM1293: Digital Power Minitor with PMBus Interface

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim


REVISION HISTORY
$Revision: 4080 $
$Date: 2018-03-19 16:36:34 -0600 (Mon, 19 Mar 2018) $

Copyright (c) 2018, Analog Devices
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
either expressed or implied, of Analog Devices.

The Analog Devices Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.
*/

/*! @file
    @ingroup ADM1293
*/

#define ADM1293_CLEAR_FAULTS        0x03

#define ADM1293_CAPABILITY          0x19

#define ADM1293_IOUT_OC_WARN_LIMIT  0x4A

#define ADM1293_VIN_OV_WARN_LIMIT   0x57
#define ADM1293_VIN_UV_WARN_LIMIT   0x58

#define ADM1293_PIN_OP_WARN_LIMIT   0x6B

#define ADM1293_STATUS_BYTE         0x78
#define ADM1293_STATUS_WORD         0x79

#define ADM1293_STATUS_IOUT         0x7B
#define ADM1293_STATUS_INPUT        0x7C

#define ADM1293_STATUS_MFR_SPECIFIC 0x80

#define ADM1293_READ_EIN            0x86
#define ADM1293_READ_EOUT           0x87
#define ADM1293_READ_VIN            0x88

#define ADM1293_READ_IOUT           0x8C

#define ADM1293_READ_PIN            0x97
#define ADM1293_PMBUS_REVISION      0x98
#define ADM1293_MFR_ID              0x99
#define ADM1293_MFR_MODEL           0x9A
#define ADM1293_MFR_REVISION        0x9B

#define ADM1293_MAX_IOUT            0xD0
#define ADM1293_PEAK_VIN            0xD1
#define ADM1293_PEAK_VAUX           0xD2
#define ADM1293_PMON_CONTROL        0xD3
#define ADM1293_PMON_CONFIG         0xD4
#define ADM1293_ALERT1_CONFIG       0xD5
#define ADM1293_ALERT2_CONFIG       0xD6

#define ADM1293_DEVICE_CONFIG       0xD8

#define ADM1293_MAX_PIN             0xDA
#define ADM1293_READ_PIN_EXT        0xDB
#define ADM1293_READ_EIN_EXT        0xDC
#define ADM1293_READ_VAUX           0xDD
#define ADM1293_VAUX_OV_WARN_LIMIT  0xDE
#define ADM1293_VAUX_UV_WARN_LIMIT  0xDF

#define ADM1293_MIN_IOUT            0xE3
#define ADM1293_MIN_PIN             0xE4
#define ADM1293_READ_EOUT_EXT       0xE5

#define ADM1293_HYSTERESIS_LOW      0xF2
#define ADM1293_HYSTERESIS_HIGH     0xF3
#define ADM1293_STATUS_HYSTERESIS   0xF4

