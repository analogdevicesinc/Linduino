/*
Copyright 2011(c) Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
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
   
THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED.

IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, INTELLECTUAL PROPERTY
RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


//#include <16F876A.h>                         // Device
//#use delay(clock=6000000)                    // 6MHz clock
//#fuses NOWDT,HS, PUT, NOPROTECT, NOBROWNOUT  // Configuration fuses
//#rom 0x2007 = {0x3f3a}

//#include "PCM73A.h"                          // Various defines
//#include "lcd420.c"                             // LCD driver functions


//#byte SSPCON   = 0x14
//#byte SSPSTAT  = 0x94
//#bit CKP             = SSPCON.4
//#bit CKE             = SSPSTAT.6

// Useful defines for the LTC2492 - OR them together to make the
// 8 bit channel address and 8 bit config word.

// Select ADC channel
#define CH0_1  0b10101000  // CH0 is negative
#define CH2_3  0b10101001  // CH2 is negative
#define CH0SE  0b10110000
#define CH1SE  0b10111001
#define CH2SE  0b10110000
#define CH3SE  0b10111000
#define NO_CH  0b10100000

// Select ADC source - differential input or PTAT circuit
#define VIN    0b10000000
#define PTAT   0b11000000

// Select rejection frequency - 50, 55, or 60Hz
#define R50    0b10010000
#define R55    0b10000000
#define R60    0b10100000

// Speed settings is bit 7 in the 2nd byte
#define SLOW   0b10000000 // slow output rate with autozero
#define FAST   0b10001000 // fast output rate with no autozero

// Define connections to LTC2480 and buttons
//#define CS PIN_C2
//#define INC_BUTTON   PIN_A0
//#define DEC_BUTTON   PIN_A1
//#define CAL_JUMPER   PIN_A4



// Function Prototypes
int32_t read_LTC2492(char channel, char config);
void initialize(void);
void display(void);
void calibrate(void);
void read_calibration(void);
float type_K_V2C(float tc_voltage, float cj_temp);
