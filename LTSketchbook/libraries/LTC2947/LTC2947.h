/*!
LTC2947: LTC2947 a high-precision power and energy monitor with an internal sense resistor supporting up to 30A.

@verbatim

The LTC2947 is a high-precision power and energy
monitor with an internal sense resistor supporting up
to 30A. Three internal No Latency delta sigma ADCs ensure
accurate measurement of voltage and current, while high-
bandwidth analog multiplication of voltage and current
provides accurate power measurement in a wide range of
applications. Internal or external clocking options enable
precise charge and energy measurements.
An internal 300 micro ohms, temperature-compensated sense
resistor minimizes efficiency loss and external compo-
nents, simplifying energy measurement applications while
enabling high accuracy current measurement over the full
temperature range. For more details see following URLs:

@endverbatim


http://www.linear.com/product/LTC2947

http://www.linear.com/product/LTC2947#demoboards


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


/*! @file
  @ingroup LTC2947
  Header for LTC2947 a high-precision power and energy monitor with an internal sense resistor supporting up to 30A.
*/

#ifndef LTC2947_H
#define LTC2947_H

#undef LTC2947_DEBUG

#include "arduino.h"

#define bitMaskSetChk(value, bitMask) (((value) & (bitMask)) == (bitMask))
#define bitMaskClrChk(value, bitMask) (((value) & (bitMask)) == 0)

#define bitMaskSet(value, bitMask) (value |=  (bitMask))
#define bitMaskClr(value, bitMask) (value &= ~(bitMask))

#define bitMaskSetClr(value, bitMask, setNotClr) ((setNotClr) ? bitMaskSet(value,bitMask) : bitMaskClr(value,bitMask))

#define LOG_1(n) (((n) >= 2) ? 1 : 0)
#define LOG_2(n) (((n) >= 1<<2) ? (2 + LOG_1((n)>>2)) : LOG_1(n))
#define LOG_4(n) (((n) >= 1<<4) ? (4 + LOG_2((n)>>4)) : LOG_2(n))
#define LOG_8(n) (((n) >= 1<<8) ? (8 + LOG_4((n)>>8)) : LOG_4(n))
#define LOG(n)   (((n) >= 1<<16) ? (16 + LOG_8((n)>>16)) : LOG_8(n))


// used to convert any bitmask like LTC2947_BM_TBCTL_DIV_0
// to its bit position index to be used for shift operations
// BM2BITPOS(LTC2947_BM_TBCTL_DIV_0) = BM2BITPOS(0x8) = BM2BITPOS(1<<3) = 3
#define BM2BITPOS(bm) ((bm)&1<<7 ? 7 : \
             (bm)&1<<6 ? 6 : \
             (bm)&1<<5 ? 5 : \
             (bm)&1<<4 ? 4 : \
             (bm)&1<<3 ? 3 : \
             (bm)&1<<2 ? 2 : \
             (bm)&1<<1 ? 1 : 0)


#define LTC2947_I2C_ADDR_LL  0x5C  // AD0=L, AD1=L (default)
#define LTC2947_I2C_ADDR_LH  0x5D  // AD0=L, AD1=H
#define LTC2947_I2C_ADDR_LR  0x5E  // AD0=L, AD1=R
#define LTC2947_I2C_ADDR_RL  0x64  // AD0=R, AD1=L
#define LTC2947_I2C_ADDR_RH  0x65  // AD0=R, AD1=H
#define LTC2947_I2C_ADDR_RR  0x66  // AD0=R, AD1=R


#define LTC2947_SPI_READ_CMD  0x01
#define LTC2947_SPI_WRITE_CMD 0x00

#define LTC2947_LSB_FACTOR_MILLI_HOURS (1.0 / 60.0 / 60.0 / 1.0e-3)
#define LTC2947_LSB_FACTOR_HOURS       (1.0 / 60.0 / 60.0)

#ifndef LTC2947_CS
#define LTC2947_CS QUIKEVAL_CS
#endif

/**
\brief  defines ltc 2947 extclk.
PRE, DIV, ext. oscillator setting:
Before include of LTC2947.h you may
1. define LTC2947_EXTCLK only to specify your external oscillator / crystal
frequency

2. not define anything. In this case the internal oscillator will be used (PRE = 7, DIV = 0)
#undef  LTC2947_EXTCLK // internal oscillator, so no external clock frequency
#undef  LTC2947_PRE    // will be defined for internal oscillator by LTC2947.h
#undef  LTC2947_DIV    // will be defined for internal oscillator by LTC2947.h

3. define full set of PRE, DIV and EXTCLK
#define LTC2947_PRE 2     // LTC2947.h will use those definitions to calc LSB calues
#define LTC2947_DIV 30       // LTC2947.h will use those definitions to calc LSB calues
#define LTC2947_EXTCLK 4e6  // LTC2947.h will use those definitions to calc LSB calues

In all cases after loading the header file you can
1. Use LTC2947_PRE, LTC2947_DIV to write to LTC2947_REG_TBCTL see below.
2. Use LSB definitions like LTC2947_LSB_TB1 to calculate real values out of integers

Note: All LSBs are calculated as a function of PRE, DIV and EXTCLK by
pre-processor macros within LTC2947.h

\param  must be 4MHz in case of crystal  operation,
or 0.2 to 25 MHz in case of external oscillator
*/
#define LTC2947_EXTCLK 4e6 // external frequency on CLKI pin (must be 4MHz in case of crystal)
#undef  LTC2947_PRE   // will be calculated and defined by LTC2947.h
#undef  LTC2947_DIV   // will be calculated and defined by LTC2947.h


/** @name LTC2947 auto generated code
*  This code is automatically generated by the LTC2947's GUI, see Tools menu of the GUI
*  @{
*/

/////////////////////////////////////////////////////////
// Definitions of available registers without those    //
// that are part of multiple byte values               //
// (e.g. accumulators).                                //
//                                                     //
// Note: The address constants are 9 bit values where  //
// the lower 8 bit define the address and the 9th bit  //
// defines the page!                                   //
/////////////////////////////////////////////////////////

// GPIO status and control (register on page 0)
#define LTC2947_REG_GPIOSTATCL 0x067

// Status register (active high) (register on page 0)
#define LTC2947_REG_STATUS 0x080

// Voltage, Temperature Threshold Alerts (register on page 0)
#define LTC2947_REG_STATVT 0x081

// Current Power Threshold Alerts (register on page 0)
#define LTC2947_REG_STATIP 0x082

// Charge Threshold Alerts (register on page 0)
#define LTC2947_REG_STATC 0x083

// Energy Threshold Alerts (register on page 0)
#define LTC2947_REG_STATE 0x084

// Charge, Energy Overflow Alerts (register on page 0)
#define LTC2947_REG_STATCEOF 0x085

// Time Base Alerts (register on page 0)
#define LTC2947_REG_STATTB 0x086

// DVCC threshold alerts (register on page 0)
#define LTC2947_REG_STATVDVCC 0x087

// STATUS mask register (1: alert from STATUS is disabled) (register on page 0)
#define LTC2947_REG_STATUSM 0x088

// STATVT mask register (1: alert from STATVT is disabled) (register on page 0)
#define LTC2947_REG_STATVTM 0x089

// STATIP mask register (1: alert from STATIP is disabled) (register on page 0)
#define LTC2947_REG_STATIPM 0x08A

// STATC mask register (1: alert from STATC is disabled) (register on page 0)
#define LTC2947_REG_STATCM 0x08B

// STATE mask register (1: alert from STATE is disabled) (register on page 0)
#define LTC2947_REG_STATEM 0x08C

// STATCEOF mask register (1: alert from STATCEOF is disabled) (register on page 0)
#define LTC2947_REG_STATCEOFM 0x08D

// STATTB mask register (1: alert from STATTB is disabled) (register on page 0)
#define LTC2947_REG_STATTBM 0x08E

// STATVDVCC mask register (1: alert from STATVDVCC is disabled) (register on page 0)
#define LTC2947_REG_STATVDVCCM 0x08F

// Accumulation control of Charge1/2 and Energy1/2 by current polarity.
// 00: Accumulation takes place always,
// 01: only if the current is positive,
// 10: only if the current is negative,
// 11: No accumulation takes place. (register on page 0)
#define LTC2947_REG_ACCICTL 0x0E1

// Accumulation control of Charge1/2, Energy1/2 and TB1/2 by pin GPIO.
// 00: Accumulation takes place always,
// 01: only if pin GPIO is 1,
// 10: only if pin GPIO is 0,
// 11: RESERVED (register on page 0)
#define LTC2947_REG_ACCGPCTL 0x0E3

// Deadband current for accumulation
// If the absolute current value is higher than or equal this value, accumulation of Charge1/Charge2 and Energy1/Energy2 and comparison to their respective threshold takes place. If lower, Charge1/Charge2 and Energy1/Energy2 values are not accumulated and there is no comparison against thresholds. (register on page 0)
#define LTC2947_REG_ACCIDB 0x0E4

// Alert Master Control Enable (register on page 0)
#define LTC2947_REG_ALERTBCTL 0x0E8

// Time Base Control (register on page 0)
#define LTC2947_REG_TBCTL 0x0E9

// Operation Control (register on page 0)
#define LTC2947_REG_OPCTL 0x0F0

// Page Control Register (register on page 0)
#define LTC2947_REG_PGCTL 0x0FF

/////////////////////////////////////////////////////////
// Definitions of all byte addresses of all multiple   //
// byte values (e.g. accumulators).                    //
//                                                     //
// Note: The address constants are 9 bit values where  //
// the lower 8 bit define the address and the 9th bit  //
// defines the page!                                   //
/////////////////////////////////////////////////////////

// Charge1 (register on page 0)
#define LTC2947_REG_C1_47_40 0x000

// (register on page 0)
#define LTC2947_REG_C1_39_32 0x001

// (register on page 0)
#define LTC2947_REG_C1_31_24 0x002

// (register on page 0)
#define LTC2947_REG_C1_23_16 0x003

// (register on page 0)
#define LTC2947_REG_C1_15_8 0x004

// (register on page 0)
#define LTC2947_REG_C1_7_0 0x005

// Energy1 (register on page 0)
#define LTC2947_REG_E1_47_40 0x006

// (register on page 0)
#define LTC2947_REG_E1_39_32 0x007

// (register on page 0)
#define LTC2947_REG_E1_31_24 0x008

// (register on page 0)
#define LTC2947_REG_E1_23_16 0x009

// (register on page 0)
#define LTC2947_REG_E1_15_8 0x00A

// (register on page 0)
#define LTC2947_REG_E1_7_0 0x00B

// Time1 (register on page 0)
#define LTC2947_REG_TB1_31_24 0x00C

// (register on page 0)
#define LTC2947_REG_TB1_23_16 0x00D

// (register on page 0)
#define LTC2947_REG_TB1_15_8 0x00E

// (register on page 0)
#define LTC2947_REG_TB1_7_0 0x00F

// Charge2 (register on page 0)
#define LTC2947_REG_C2_47_40 0x010

// (register on page 0)
#define LTC2947_REG_C2_39_32 0x011

// (register on page 0)
#define LTC2947_REG_C2_31_24 0x012

// (register on page 0)
#define LTC2947_REG_C2_23_16 0x013

// (register on page 0)
#define LTC2947_REG_C2_15_8 0x014

// (register on page 0)
#define LTC2947_REG_C2_7_0 0x015

// Energy2 (register on page 0)
#define LTC2947_REG_E2_47_40 0x016

// (register on page 0)
#define LTC2947_REG_E2_39_32 0x017

// (register on page 0)
#define LTC2947_REG_E2_31_24 0x018

// (register on page 0)
#define LTC2947_REG_E2_23_16 0x019

// (register on page 0)
#define LTC2947_REG_E2_15_8 0x01A

// (register on page 0)
#define LTC2947_REG_E2_7_0 0x01B

// Time2 (register on page 0)
#define LTC2947_REG_TB2_31_24 0x01C

// (register on page 0)
#define LTC2947_REG_TB2_23_16 0x01D

// (register on page 0)
#define LTC2947_REG_TB2_15_8 0x01E

// (register on page 0)
#define LTC2947_REG_TB2_7_0 0x01F

// Maximum current tracking (register on page 0)
#define LTC2947_REG_IMAX_15_8 0x040

// (register on page 0)
#define LTC2947_REG_IMAX_7_0 0x041

// Minimum Current tracking (register on page 0)
#define LTC2947_REG_IMIN_15_8 0x042

// (register on page 0)
#define LTC2947_REG_IMIN_7_0 0x043

// Maximum Power tracking (register on page 0)
#define LTC2947_REG_PMAX_15_8 0x044

// (register on page 0)
#define LTC2947_REG_PMAX_7_0 0x045

// Minimum Power tracking (register on page 0)
#define LTC2947_REG_PMIN_15_8 0x046

// (register on page 0)
#define LTC2947_REG_PMIN_7_0 0x047

// Maximum Voltage (VD = VP - VM) tracking (register on page 0)
#define LTC2947_REG_VMAX_15_8 0x050

// (register on page 0)
#define LTC2947_REG_VMAX_7_0 0x051

// Minimum Voltage (VD = VP - VM) tracking (register on page 0)
#define LTC2947_REG_VMIN_15_8 0x052

// (register on page 0)
#define LTC2947_REG_VMIN_7_0 0x053

// Maximum Temperature tracking (register on page 0)
#define LTC2947_REG_TEMPMAX_15_8 0x054

// (register on page 0)
#define LTC2947_REG_TEMPMAX_7_0 0x055

// Minimum Temperature tracking (register on page 0)
#define LTC2947_REG_TEMPMIN_15_8 0x056

// (register on page 0)
#define LTC2947_REG_TEMPMIN_7_0 0x057

// Maximum Voltage at DVCC tracking (register on page 0)
#define LTC2947_REG_VDVCCMAX_15_8 0x058

// (register on page 0)
#define LTC2947_REG_VDVCCMAX_7_0 0x059

// Minimum Voltage at DVCC tracking (register on page 0)
#define LTC2947_REG_VDVCCMIN_15_8 0x05A

// (register on page 0)
#define LTC2947_REG_VDVCCMIN_7_0 0x05B

// Current (register on page 0)
#define LTC2947_REG_I_23_16 0x090

// (register on page 0)
#define LTC2947_REG_I_15_8 0x091

// (register on page 0)
#define LTC2947_REG_I_7_0 0x092

// Power (register on page 0)
#define LTC2947_REG_P_23_16 0x093

// (register on page 0)
#define LTC2947_REG_P_15_8 0x094

// (register on page 0)
#define LTC2947_REG_P_7_0 0x095

// Voltage (VD = VP - VM) (register on page 0)
#define LTC2947_REG_V_15_8 0x0A0

// (register on page 0)
#define LTC2947_REG_V_7_0 0x0A1

// Temperature (register on page 0)
#define LTC2947_REG_TEMP_15_8 0x0A2

// (register on page 0)
#define LTC2947_REG_TEMP_7_0 0x0A3

// Voltage at DVCC (register on page 0)
#define LTC2947_REG_VDVCC_15_8 0x0A4

// (register on page 0)
#define LTC2947_REG_VDVCC_7_0 0x0A5

// Current History 1 (prev. result) (register on page 0)
#define LTC2947_REG_IH1_23_16 0x0B0

// (register on page 0)
#define LTC2947_REG_IH1_15_8 0x0B1

// (register on page 0)
#define LTC2947_REG_IH1_7_0 0x0B2

// Current History 2 (prev. result '-1') (register on page 0)
#define LTC2947_REG_IH2_23_16 0x0B3

// (register on page 0)
#define LTC2947_REG_IH2_15_8 0x0B4

// (register on page 0)
#define LTC2947_REG_IH2_7_0 0x0B5

// Current History 3 (prev. result '-2') (register on page 0)
#define LTC2947_REG_IH3_23_16 0x0B6

// (register on page 0)
#define LTC2947_REG_IH3_15_8 0x0B7

// (register on page 0)
#define LTC2947_REG_IH3_7_0 0x0B8

// Current History 4 (prev. result '-3') (register on page 0)
#define LTC2947_REG_IH4_23_16 0x0B9

// (register on page 0)
#define LTC2947_REG_IH4_15_8 0x0BA

// (register on page 0)
#define LTC2947_REG_IH4_7_0 0x0BB

// Current History 5 (prev. result '-4') (register on page 0)
#define LTC2947_REG_IH5_23_16 0x0BC

// (register on page 0)
#define LTC2947_REG_IH5_15_8 0x0BD

// (register on page 0)
#define LTC2947_REG_IH5_7_0 0x0BE

// Charge 1 threshold high (register on page 1)
#define LTC2947_REG_C1TH_47_40 0x100

// (register on page 1)
#define LTC2947_REG_C1TH_39_32 0x101

// (register on page 1)
#define LTC2947_REG_C1TH_31_24 0x102

// (register on page 1)
#define LTC2947_REG_C1TH_23_16 0x103

// (register on page 1)
#define LTC2947_REG_C1TH_15_8 0x104

// (register on page 1)
#define LTC2947_REG_C1TH_7_0 0x105

// Charge 1 threshold low (register on page 1)
#define LTC2947_REG_C1TL_47_40 0x106

// (register on page 1)
#define LTC2947_REG_C1TL_39_32 0x107

// (register on page 1)
#define LTC2947_REG_C1TL_31_24 0x108

// (register on page 1)
#define LTC2947_REG_C1TL_23_16 0x109

// (register on page 1)
#define LTC2947_REG_C1TL_15_8 0x10A

// (register on page 1)
#define LTC2947_REG_C1TL_7_0 0x10B

// Time Base 1 threshold high (register on page 1)
#define LTC2947_REG_TB1TH_31_24 0x10C

// (register on page 1)
#define LTC2947_REG_TB1TH_23_16 0x10D

// (register on page 1)
#define LTC2947_REG_TB1TH_15_8 0x10E

// (register on page 1)
#define LTC2947_REG_TB1TH_7_0 0x10F

// Energy 1 threshold high (register on page 1)
#define LTC2947_REG_E1TH_47_40 0x110

// (register on page 1)
#define LTC2947_REG_E1TH_39_32 0x111

// (register on page 1)
#define LTC2947_REG_E1TH_31_24 0x112

// (register on page 1)
#define LTC2947_REG_E1TH_23_16 0x113

// (register on page 1)
#define LTC2947_REG_E1TH_15_8 0x114

// (register on page 1)
#define LTC2947_REG_E1TH_7_0 0x115

// Energy 1 threshold low (register on page 1)
#define LTC2947_REG_E1TL_47_40 0x116

// (register on page 1)
#define LTC2947_REG_E1TL_39_32 0x117

// (register on page 1)
#define LTC2947_REG_E1TL_31_24 0x118

// (register on page 1)
#define LTC2947_REG_E1TL_23_16 0x119

// (register on page 1)
#define LTC2947_REG_E1TL_15_8 0x11A

// (register on page 1)
#define LTC2947_REG_E1TL_7_0 0x11B

// Charge 2 threshold high (register on page 1)
#define LTC2947_REG_C2TH_47_40 0x120

// (register on page 1)
#define LTC2947_REG_C2TH_39_32 0x121

// (register on page 1)
#define LTC2947_REG_C2TH_31_24 0x122

// (register on page 1)
#define LTC2947_REG_C2TH_23_16 0x123

// (register on page 1)
#define LTC2947_REG_C2TH_15_8 0x124

// (register on page 1)
#define LTC2947_REG_C2TH_7_0 0x125

// Charge 2 threshold low (register on page 1)
#define LTC2947_REG_C2TL_47_40 0x126

// (register on page 1)
#define LTC2947_REG_C2TL_39_32 0x127

// (register on page 1)
#define LTC2947_REG_C2TL_31_24 0x128

// (register on page 1)
#define LTC2947_REG_C2TL_23_16 0x129

// (register on page 1)
#define LTC2947_REG_C2TL_15_8 0x12A

// (register on page 1)
#define LTC2947_REG_C2TL_7_0 0x12B

// Time Base 2 threshold high (register on page 1)
#define LTC2947_REG_TB2TH_31_24 0x12C

// (register on page 1)
#define LTC2947_REG_TB2TH_23_16 0x12D

// (register on page 1)
#define LTC2947_REG_TB2TH_15_8 0x12E

// (register on page 1)
#define LTC2947_REG_TB2TH_7_0 0x12F

// Energy 2 threshold high (register on page 1)
#define LTC2947_REG_E2TH_47_40 0x130

// (register on page 1)
#define LTC2947_REG_E2TH_39_32 0x131

// (register on page 1)
#define LTC2947_REG_E2TH_31_24 0x132

// (register on page 1)
#define LTC2947_REG_E2TH_23_16 0x133

// (register on page 1)
#define LTC2947_REG_E2TH_15_8 0x134

// (register on page 1)
#define LTC2947_REG_E2TH_7_0 0x135

// Energy 2 threshold low (register on page 1)
#define LTC2947_REG_E2TL_47_40 0x136

// (register on page 1)
#define LTC2947_REG_E2TL_39_32 0x137

// (register on page 1)
#define LTC2947_REG_E2TL_31_24 0x138

// (register on page 1)
#define LTC2947_REG_E2TL_23_16 0x139

// (register on page 1)
#define LTC2947_REG_E2TL_15_8 0x13A

// (register on page 1)
#define LTC2947_REG_E2TL_7_0 0x13B

// Current threshold high (register on page 1)
#define LTC2947_REG_ITH_15_8 0x180

// (register on page 1)
#define LTC2947_REG_ITH_7_0 0x181

// Current threshold low (register on page 1)
#define LTC2947_REG_ITL_15_8 0x182

// (register on page 1)
#define LTC2947_REG_ITL_7_0 0x183

// Power threshold high (register on page 1)
#define LTC2947_REG_PTH_15_8 0x184

// (register on page 1)
#define LTC2947_REG_PTH_7_0 0x185

// Power threshold low (register on page 1)
#define LTC2947_REG_PTL_15_8 0x186

// (register on page 1)
#define LTC2947_REG_PTL_7_0 0x187

// Voltage (VD = VP - VM) threshold high (register on page 1)
#define LTC2947_REG_VTH_15_8 0x190

// (register on page 1)
#define LTC2947_REG_VTH_7_0 0x191

// Voltage (VD = VP - VM) threshold low (register on page 1)
#define LTC2947_REG_VTL_15_8 0x192

// (register on page 1)
#define LTC2947_REG_VTL_7_0 0x193

// Temperature threshold high (register on page 1)
#define LTC2947_REG_TEMPTH_15_8 0x194

// (register on page 1)
#define LTC2947_REG_TEMPTH_7_0 0x195

// Temperature threshold low (register on page 1)
#define LTC2947_REG_TEMPTL_15_8 0x196

// (register on page 1)
#define LTC2947_REG_TEMPTL_7_0 0x197

// DVCC Voltage threshold high (register on page 1)
#define LTC2947_REG_VDVCCTH_15_8 0x198

// (register on page 1)
#define LTC2947_REG_VDVCCTH_7_0 0x199

// DVCC Voltage threshold low (register on page 1)
#define LTC2947_REG_VDVCCTL_15_8 0x19A

// (register on page 1)
#define LTC2947_REG_VDVCCTL_7_0 0x19B

// Fan temperature threshold high (if this temperature is exceeded the fan is enabled, see GPIOSTATCL) (register on page 1)
#define LTC2947_REG_TEMPTFANH_15_8 0x19C

// (register on page 1)
#define LTC2947_REG_TEMPTFANH_7_0 0x19D

// Fan temperature threshold low (if temperature falls below this value the fan is disabled, see GPIOSTATCL) (register on page 1)
#define LTC2947_REG_TEMPTFANL_15_8 0x19E

// (register on page 1)
#define LTC2947_REG_TEMPTFANL_7_0 0x19F

/////////////////////////////////////////////////////////
// Definitions of all available multiple byte values   //
// (e.g. accumulators).                                //
//                                                     //
// Note: The address constants are 9 bit values where  //
// the lower 8 bit define the address and the 9th bit  //
// defines the page!                                   //
/////////////////////////////////////////////////////////

// Charge1 (register on page 0)
#define LTC2947_VAL_C1 0x000

// Energy1 (register on page 0)
#define LTC2947_VAL_E1 0x006

// Time1 (register on page 0)
#define LTC2947_VAL_TB1 0x00C

// Charge2 (register on page 0)
#define LTC2947_VAL_C2 0x010

// Energy2 (register on page 0)
#define LTC2947_VAL_E2 0x016

// Time2 (register on page 0)
#define LTC2947_VAL_TB2 0x01C

// Maximum current tracking (register on page 0)
#define LTC2947_VAL_IMAX 0x040

// Minimum Current tracking (register on page 0)
#define LTC2947_VAL_IMIN 0x042

// Maximum Power tracking (register on page 0)
#define LTC2947_VAL_PMAX 0x044

// Minimum Power tracking (register on page 0)
#define LTC2947_VAL_PMIN 0x046

// Maximum Voltage (VD = VP - VM) tracking (register on page 0)
#define LTC2947_VAL_VMAX 0x050

// Minimum Voltage (VD = VP - VM) tracking (register on page 0)
#define LTC2947_VAL_VMIN 0x052

// Maximum Temperature tracking (register on page 0)
#define LTC2947_VAL_TEMPMAX 0x054

// Minimum Temperature tracking (register on page 0)
#define LTC2947_VAL_TEMPMIN 0x056

// Maximum Voltage at DVCC tracking (register on page 0)
#define LTC2947_VAL_VDVCCMAX 0x058

// Minimum Voltage at DVCC tracking (register on page 0)
#define LTC2947_VAL_VDVCCMIN 0x05A

// Current (register on page 0)
#define LTC2947_VAL_I 0x090

// Power (register on page 0)
#define LTC2947_VAL_P 0x093

// Voltage (VD = VP - VM) (register on page 0)
#define LTC2947_VAL_V 0x0A0

// Temperature (register on page 0)
#define LTC2947_VAL_TEMP 0x0A2

// Voltage at DVCC (register on page 0)
#define LTC2947_VAL_VDVCC 0x0A4

// Current History 1 (prev. result) (register on page 0)
#define LTC2947_VAL_IH1 0x0B0

// Current History 2 (prev. result '-1') (register on page 0)
#define LTC2947_VAL_IH2 0x0B3

// Current History 3 (prev. result '-2') (register on page 0)
#define LTC2947_VAL_IH3 0x0B6

// Current History 4 (prev. result '-3') (register on page 0)
#define LTC2947_VAL_IH4 0x0B9

// Current History 5 (prev. result '-4') (register on page 0)
#define LTC2947_VAL_IH5 0x0BC

// Charge 1 threshold high (register on page 1)
#define LTC2947_VAL_C1TH 0x100

// Charge 1 threshold low (register on page 1)
#define LTC2947_VAL_C1TL 0x106

// Time Base 1 threshold high (register on page 1)
#define LTC2947_VAL_TB1TH 0x10C

// Energy 1 threshold high (register on page 1)
#define LTC2947_VAL_E1TH 0x110

// Energy 1 threshold low (register on page 1)
#define LTC2947_VAL_E1TL 0x116

// Charge 2 threshold high (register on page 1)
#define LTC2947_VAL_C2TH 0x120

// Charge 2 threshold low (register on page 1)
#define LTC2947_VAL_C2TL 0x126

// Time Base 2 threshold high (register on page 1)
#define LTC2947_VAL_TB2TH 0x12C

// Energy 2 threshold high (register on page 1)
#define LTC2947_VAL_E2TH 0x130

// Energy 2 threshold low (register on page 1)
#define LTC2947_VAL_E2TL 0x136

// Current threshold high (register on page 1)
#define LTC2947_VAL_ITH 0x180

// Current threshold low (register on page 1)
#define LTC2947_VAL_ITL 0x182

// Power threshold high (register on page 1)
#define LTC2947_VAL_PTH 0x184

// Power threshold low (register on page 1)
#define LTC2947_VAL_PTL 0x186

// Voltage (VD = VP - VM) threshold high (register on page 1)
#define LTC2947_VAL_VTH 0x190

// Voltage (VD = VP - VM) threshold low (register on page 1)
#define LTC2947_VAL_VTL 0x192

// Temperature threshold high (register on page 1)
#define LTC2947_VAL_TEMPTH 0x194

// Temperature threshold low (register on page 1)
#define LTC2947_VAL_TEMPTL 0x196

// DVCC Voltage threshold high (register on page 1)
#define LTC2947_VAL_VDVCCTH 0x198

// DVCC Voltage threshold low (register on page 1)
#define LTC2947_VAL_VDVCCTL 0x19A

// Fan temperature threshold high (if this temperature is exceeded the fan is enabled, see GPIOSTATCL) (register on page 1)
#define LTC2947_VAL_TEMPTFANH 0x19C

// Fan temperature threshold low (if temperature falls below this value the fan is disabled, see GPIOSTATCL) (register on page 1)
#define LTC2947_VAL_TEMPTFANL 0x19E

/////////////////////////////////////////////////////////
// Definition of LSB constants.                        //
/////////////////////////////////////////////////////////

#ifndef LTC2947_PRE
#ifndef LTC2947_EXTCLK
// internal oscillator
#define LTC2947_PRE 7
#define LTC2947_DIV 0
#define LTC2947_EXTCLK 4e6
#else
#define LTC2947_EXTCLK_UNSUPPORTED 5
// user provided external oscillator frequency
#define LTC2947_PRE ( \
      LTC2947_EXTCLK <=  1e6 ? 0 : \
      LTC2947_EXTCLK <=  2e6 ? 1 : \
      LTC2947_EXTCLK <=  4e6 ? 2 : \
      LTC2947_EXTCLK <=  8e6 ? 3 : \
      LTC2947_EXTCLK <= 16e6 ? 4 : \
      LTC2947_EXTCLK <= 25e6 ? 5 : LTC2947_EXTCLK_UNSUPPORTED)
#define LTC2947_DIV ((uint8_t)(LTC2947_EXTCLK/32768.0/(1 << LTC2947_PRE)))
#endif
#endif

#define LTC2947_EXTPER (1.0/LTC2947_EXTCLK)

#define LTC2947_INTC 31e-6

// LSB of C1 in As
#define LTC2947_LSB_C1           (0.0384946 * (LTC2947_PRE == 7 ? LTC2947_INTC : LTC2947_EXTPER * (1 << LTC2947_PRE) * (LTC2947_DIV + 1)))
// LSB of E1 in Ws
#define LTC2947_LSB_E1           (0.641576 * (LTC2947_PRE == 7 ? LTC2947_INTC : LTC2947_EXTPER * (1 << LTC2947_PRE) * (LTC2947_DIV + 1)))
// LSB of TB1 in s
#define LTC2947_LSB_TB1          (12.8315 * (LTC2947_PRE == 7 ? LTC2947_INTC : LTC2947_EXTPER * (1 << LTC2947_PRE) * (LTC2947_DIV + 1)))
// LSB of C2 in As
#define LTC2947_LSB_C2           (0.0384946 * (LTC2947_PRE == 7 ? LTC2947_INTC : LTC2947_EXTPER * (1 << LTC2947_PRE) * (LTC2947_DIV + 1)))
// LSB of E2 in Ws
#define LTC2947_LSB_E2           (0.641576 * (LTC2947_PRE == 7 ? LTC2947_INTC : LTC2947_EXTPER * (1 << LTC2947_PRE) * (LTC2947_DIV + 1)))
// LSB of TB2 in s
#define LTC2947_LSB_TB2          (12.8315 * (LTC2947_PRE == 7 ? LTC2947_INTC : LTC2947_EXTPER * (1 << LTC2947_PRE) * (LTC2947_DIV + 1)))
// LSB of IMAX in mA
#define LTC2947_LSB_IMAX         12.0
// LSB of IMIN in mA
#define LTC2947_LSB_IMIN         12.0
// LSB of PMAX in mW
#define LTC2947_LSB_PMAX         200.0
// LSB of PMIN in mW
#define LTC2947_LSB_PMIN         200.0
// LSB of VMAX in mV
#define LTC2947_LSB_VMAX         2.0
// LSB of VMIN in mV
#define LTC2947_LSB_VMIN         2.0
// LSB of TEMPMAX in °C (Note: Temperature has additionally an offset of 5.5 °C)
#define LTC2947_LSB_TEMPMAX      0.204
// Offset of TEMPMAX in °C
#define LTC2947_OFFS_TEMPMAX     5.5
// LSB of TEMPMIN in °C (Note: Temperature has additionally an offset of 5.5 °C)
#define LTC2947_LSB_TEMPMIN      0.204
// Offset of TEMPMIN in °C
#define LTC2947_OFFS_TEMPMIN     5.5
// LSB of VDVCCMAX in mV
#define LTC2947_LSB_VDVCCMAX     145.0
// LSB of VDVCCMIN in mV
#define LTC2947_LSB_VDVCCMIN     145.0
// LSB of I in mA
#define LTC2947_LSB_I            3.0
// LSB of P in mW
#define LTC2947_LSB_P            50.0
// LSB of V in mV
#define LTC2947_LSB_V            2.0
// LSB of TEMP in °C (Note: Temperature has additionally an offset of 5.5 °C)
#define LTC2947_LSB_TEMP         0.204
// Offset of TEMP in °C
#define LTC2947_OFFS_TEMP        5.5
// LSB of VDVCC in mV
#define LTC2947_LSB_VDVCC        145.0
// LSB of IH1 in mA
#define LTC2947_LSB_IH1          3.0
// LSB of IH2 in mA
#define LTC2947_LSB_IH2          3.0
// LSB of IH3 in mA
#define LTC2947_LSB_IH3          3.0
// LSB of IH4 in mA
#define LTC2947_LSB_IH4          3.0
// LSB of IH5 in mA
#define LTC2947_LSB_IH5          3.0
// LSB of C1TH in As
#define LTC2947_LSB_C1TH         (0.0384946 * (LTC2947_PRE == 7 ? LTC2947_INTC : LTC2947_EXTPER * (1 << LTC2947_PRE) * (LTC2947_DIV + 1)))
// LSB of C1TL in As
#define LTC2947_LSB_C1TL         (0.0384946 * (LTC2947_PRE == 7 ? LTC2947_INTC : LTC2947_EXTPER * (1 << LTC2947_PRE) * (LTC2947_DIV + 1)))
// LSB of TB1TH in s
#define LTC2947_LSB_TB1TH        (12.8315 * (LTC2947_PRE == 7 ? LTC2947_INTC : LTC2947_EXTPER * (1 << LTC2947_PRE) * (LTC2947_DIV + 1)))
// LSB of E1TH in Ws
#define LTC2947_LSB_E1TH         (0.641576 * (LTC2947_PRE == 7 ? LTC2947_INTC : LTC2947_EXTPER * (1 << LTC2947_PRE) * (LTC2947_DIV + 1)))
// LSB of E1TL in Ws
#define LTC2947_LSB_E1TL         (0.641576 * (LTC2947_PRE == 7 ? LTC2947_INTC : LTC2947_EXTPER * (1 << LTC2947_PRE) * (LTC2947_DIV + 1)))
// LSB of C2TH in As
#define LTC2947_LSB_C2TH         (0.0384946 * (LTC2947_PRE == 7 ? LTC2947_INTC : LTC2947_EXTPER * (1 << LTC2947_PRE) * (LTC2947_DIV + 1)))
// LSB of C2TL in As
#define LTC2947_LSB_C2TL         (0.0384946 * (LTC2947_PRE == 7 ? LTC2947_INTC : LTC2947_EXTPER * (1 << LTC2947_PRE) * (LTC2947_DIV + 1)))
// LSB of TB2TH in s
#define LTC2947_LSB_TB2TH        (12.8315 * (LTC2947_PRE == 7 ? LTC2947_INTC : LTC2947_EXTPER * (1 << LTC2947_PRE) * (LTC2947_DIV + 1)))
// LSB of E2TH in Ws
#define LTC2947_LSB_E2TH         (0.641576 * (LTC2947_PRE == 7 ? LTC2947_INTC : LTC2947_EXTPER * (1 << LTC2947_PRE) * (LTC2947_DIV + 1)))
// LSB of E2TL in Ws
#define LTC2947_LSB_E2TL         (0.641576 * (LTC2947_PRE == 7 ? LTC2947_INTC : LTC2947_EXTPER * (1 << LTC2947_PRE) * (LTC2947_DIV + 1)))
// LSB of ITH in mA
#define LTC2947_LSB_ITH          12.0
// LSB of ITL in mA
#define LTC2947_LSB_ITL          12.0
// LSB of PTH in mW
#define LTC2947_LSB_PTH          200.0
// LSB of PTL in mW
#define LTC2947_LSB_PTL          200.0
// LSB of VTH in mV
#define LTC2947_LSB_VTH          2.0
// LSB of VTL in mV
#define LTC2947_LSB_VTL          2.0
// LSB of TEMPTH in °C (Note: Temperature has additionally an offset of 5.5 °C)
#define LTC2947_LSB_TEMPTH       0.204
// Offset of TEMPTH in °C
#define LTC2947_OFFS_TEMPTH      5.5
// LSB of TEMPTL in °C (Note: Temperature has additionally an offset of 5.5 °C)
#define LTC2947_LSB_TEMPTL       0.204
// Offset of TEMPTL in °C
#define LTC2947_OFFS_TEMPTL      5.5
// LSB of VDVCCTH in mV
#define LTC2947_LSB_VDVCCTH      145.0
// LSB of VDVCCTL in mV
#define LTC2947_LSB_VDVCCTL      145.0
// LSB of TEMPTFANH in °C (Note: Temperature has additionally an offset of 5.5 °C)
#define LTC2947_LSB_TEMPTFANH    0.204
// Offset of TEMPTFANH in °C
#define LTC2947_OFFS_TEMPTFANH   5.5
// LSB of TEMPTFANL in °C (Note: Temperature has additionally an offset of 5.5 °C)
#define LTC2947_LSB_TEMPTFANL    0.204
// Offset of TEMPTFANL in °C
#define LTC2947_OFFS_TEMPTFANL   5.5

/////////////////////////////////////////////////////////
// Definition of all bit masks.                        //
/////////////////////////////////////////////////////////

// GPIOSTATCL bit masks
// GPIO output enable (0: input, 1: output)
#define LTC2947_BM_GPIOSTATCL_GPOEN 0x1
// This register shows the applied level at pin GPIO
// 0: Logical level 0 at pin GPIO
// 1: Logical level 1 at pin GPIO
#define LTC2947_BM_GPIOSTATCL_GPI 0x10
// This register sets the level at GPIO if set as output provided there is a pull-up resistor at GPIO
// 0: Pin GPIO is set to 0 if set as output
// 1: Pin GPIO is set to 1 if set as output
#define LTC2947_BM_GPIOSTATCL_GPO 0x20
// GPIO fan control enable
// 0:GPIO controlled by GPO
// 1: GPIO controlled by temperature measurement against thresholds
#define LTC2947_BM_GPIOSTATCL_FANEN 0x40
// Polarity of GPIO if FANEN = 1
// 0: low active
// 1: high active
#define LTC2947_BM_GPIOSTATCL_FANPOL 0x80

// STATUS bit masks
// 1: Undervoltage in the analog domain including ADCs during a conversion
#define LTC2947_BM_STATUS_UVLOA 0x1
// 1: Power On Reset has occurred due to undervoltage in the analog domain.
#define LTC2947_BM_STATUS_PORA 0x2
// 1: Undervoltage in the standby domain
#define LTC2947_BM_STATUS_UVLOSTBY 0x4
// 1: Undervoltage in the digital domain
#define LTC2947_BM_STATUS_UVLOD 0x8
// 1: Result registers have been updated
#define LTC2947_BM_STATUS_UPDATE 0x10
// 1: The ADC conversion is not valid due to undervoltage during a conversion
#define LTC2947_BM_STATUS_ADCERR 0x20
// 1: External clock provided does not fit to TBC settings. Also LSB values of accumulated registers are wrong.
#define LTC2947_BM_STATUS_TBERR 0x40

// STATVT bit masks
// 1: Voltage (VD = VP - VM) high threshold exceeded
#define LTC2947_BM_STATVT_VH 0x1
// 1: Voltage (VD = VP - VM) low threshold exceeded
#define LTC2947_BM_STATVT_VL 0x2
// 1: Temperature high threshold exceeded
#define LTC2947_BM_STATVT_TEMPH 0x4
// 1: Temperature low threshold exceeded
#define LTC2947_BM_STATVT_TEMPL 0x8
// 1: Fan high temperature threshold exceeded
#define LTC2947_BM_STATVT_FANH 0x10
// 1: Fan low temperature threshold exceeded
#define LTC2947_BM_STATVT_FANL 0x20

// STATIP bit masks
// 1: Current high threshold exceeded
#define LTC2947_BM_STATIP_IH 0x1
// 1: Current low threshold exceeded
#define LTC2947_BM_STATIP_IL 0x2
// 1: Power high threshold exceeded
#define LTC2947_BM_STATIP_PH 0x4
// 1: Power low threshold exceeded
#define LTC2947_BM_STATIP_PL 0x8

// STATC bit masks
// 1: Charge1 high threshold exceeded
#define LTC2947_BM_STATC_C1H 0x1
// 1: Charge1 low threshold exceeded
#define LTC2947_BM_STATC_C1L 0x2
// 1: Charge2 high threshold exceeded
#define LTC2947_BM_STATC_C2H 0x4
// 1: Charge2 low threshold exceeded
#define LTC2947_BM_STATC_C2L 0x8

// STATE bit masks
// 1: Energy1 high threshold exceeded
#define LTC2947_BM_STATE_E1H 0x1
// 1: Energy1 low threshold exceeded
#define LTC2947_BM_STATE_E1L 0x2
// 1: Energy2 high threshold exceeded
#define LTC2947_BM_STATE_E2H 0x4
// 1: Energy2 low threshold exceeded
#define LTC2947_BM_STATE_E2L 0x8

// STATCEOF bit masks
// 1: Charge1 overflow alert
#define LTC2947_BM_STATCEOF_C1OF 0x1
// 1: Charge2 overflow alert
#define LTC2947_BM_STATCEOF_C2OF 0x2
// 1: Energy1 overflow alert
#define LTC2947_BM_STATCEOF_E1OF 0x10
// 1: Energy2 overflow alert
#define LTC2947_BM_STATCEOF_E2OF 0x20

// STATTB bit masks
// 1: Time1 threshold exceeded
#define LTC2947_BM_STATTB_TB1TH 0x1
// 1: Time2 threshold exceeded
#define LTC2947_BM_STATTB_TB2TH 0x2
// 1: Time1 overflow
#define LTC2947_BM_STATTB_TB1OF 0x10
// 1: Time 2 overflow
#define LTC2947_BM_STATTB_TB2OF 0x20

// STATVDVCC bit masks
// 1: DVCC high threshold exceeded
#define LTC2947_BM_STATVDVCC_VDVCCH 0x1
// 1: DVCC low threshold exceeded
#define LTC2947_BM_STATVDVCC_VDVCCL 0x2

// STATUSM bit masks
// UVLOA mask
#define LTC2947_BM_STATUSM_UVLOAM 0x1
// UVLOD mask
#define LTC2947_BM_STATUSM_UVLODM 0x8
// UPDATE mask
#define LTC2947_BM_STATUSM_UPDATEM 0x10
// ADCERR mask
#define LTC2947_BM_STATUSM_ADCERRM 0x20
// TBERR mask
#define LTC2947_BM_STATUSM_TBERRM 0x40

// STATVTM bit masks
// VH mask
#define LTC2947_BM_STATVTM_VHM 0x1
// VL mask
#define LTC2947_BM_STATVTM_VLM 0x2
// TEMPH mask
#define LTC2947_BM_STATVTM_TEMPHM 0x4
// TEMPL mask
#define LTC2947_BM_STATVTM_TEMPLM 0x8
// FANH mask
#define LTC2947_BM_STATVTM_FANHM 0x10
// FANL mask
#define LTC2947_BM_STATVTM_FANLM 0x20

// STATIPM bit masks
// IH mask
#define LTC2947_BM_STATIPM_IHM 0x1
// IL mask
#define LTC2947_BM_STATIPM_ILM 0x2
// PH mask
#define LTC2947_BM_STATIPM_PHM 0x4
// PL mask
#define LTC2947_BM_STATIPM_PLM 0x8

// STATCM bit masks
// C1H mask
#define LTC2947_BM_STATCM_C1HM 0x1
// C1L mask
#define LTC2947_BM_STATCM_C1LM 0x2
// C2H mask
#define LTC2947_BM_STATCM_C2HM 0x4
// C2L mask
#define LTC2947_BM_STATCM_C2LM 0x8

// STATEM bit masks
// E1H mask
#define LTC2947_BM_STATEM_E1HM 0x1
// E1L mask
#define LTC2947_BM_STATEM_E1LM 0x2
// E2H mask
#define LTC2947_BM_STATEM_E2HM 0x4
// E2L mask
#define LTC2947_BM_STATEM_E2LM 0x8

// STATCEOFM bit masks
// C1OF mask
#define LTC2947_BM_STATCEOFM_C1OFM 0x1
// C2OF mask
#define LTC2947_BM_STATCEOFM_C2OFM 0x2
// E1OF mask
#define LTC2947_BM_STATCEOFM_E1OFM 0x10
// E2OF mask
#define LTC2947_BM_STATCEOFM_E2OFM 0x20

// STATTBM bit masks
// TB1TH mask
#define LTC2947_BM_STATTBM_TB1THM 0x1
// TB2TH mask
#define LTC2947_BM_STATTBM_TB2THM 0x2
// TB1OF mask
#define LTC2947_BM_STATTBM_TB1OFM 0x10
// TB2OF mask
#define LTC2947_BM_STATTBM_TB2OFM 0x20

// STATVDVCCM bit masks
// VDVCCH mask
#define LTC2947_BM_STATVDVCCM_VDVCCHM 0x1
// VDVCCL mask
#define LTC2947_BM_STATVDVCCM_VDVCCLM 0x2

// ACCICTL bit masks
// accumulator 1 current control (bit 0)
#define LTC2947_BM_ACCICTL_ACC1I0 0x1
// accumulator 1 current control (bit 1)
#define LTC2947_BM_ACCICTL_ACC1I1 0x2
// accumulator 2 current control (bit 0)
#define LTC2947_BM_ACCICTL_ACC2I0 0x4
// accumulator 2 current control (bit 1)
#define LTC2947_BM_ACCICTL_ACC2I1 0x8

// ACCGPCTL bit masks
// accumulator 1 GPIO control (bit 0)
#define LTC2947_BM_ACCGPCTL_ACC1GP0 0x1
// accumulator 1 GPIO control (bit 1)
#define LTC2947_BM_ACCGPCTL_ACC1GP1 0x2
// accumulator 2 GPIO control (bit 0)
#define LTC2947_BM_ACCGPCTL_ACC2GP0 0x4
// accumulator 2 GPIO control (bit 1)
#define LTC2947_BM_ACCGPCTL_ACC2GP1 0x8

// ALERTBCTL bit masks
// 0: Unmasked alerts (see MASK registers) are not forwarded to ALERT pin
// 1: Unmasked alerts (see MASK registers) are forwarded to ALERT pin
#define LTC2947_BM_ALERTBCTL_ALERTBEN 0x1

// TBCTL bit masks
// Prescaler value bit 0, binary coded.
#define LTC2947_BM_TBCTL_PRE_0 0x1
// Prescaler value bit 1, binary coded.
#define LTC2947_BM_TBCTL_PRE_1 0x2
// Prescaler value bit 2, binary coded.
#define LTC2947_BM_TBCTL_PRE_2 0x4
// Divider value bit 0, binary coded.
#define LTC2947_BM_TBCTL_DIV_0 0x8
// Divider value bit 1, binary coded.
#define LTC2947_BM_TBCTL_DIV_1 0x10
// Divider value bit 2, binary coded.
#define LTC2947_BM_TBCTL_DIV_2 0x20
// Divider value bit 3, binary coded.
#define LTC2947_BM_TBCTL_DIV_3 0x40
// Divider value bit 4, binary coded.
#define LTC2947_BM_TBCTL_DIV_4 0x80

// OPCTL bit masks
// 0: Normal operation
// 1: Shutdown. The LTC2947 will exit shutdown in SPI mode if the pin AD1/CS is pulled lowand in I2C mode if it receives the correct I2C address (programmed at the ADx pins).
#define LTC2947_BM_OPCTL_SHDN 0x1
// 1: Clear. The accumulation and tracking (max/min) registers are cleared: C1, E1, TB1, C2, E2, TB2, IMAX, IMIN, PMAX, PMIN, VMAX, VMIN, TEMPMAX, TEMPMIN, VDVCCMAX,VDVCCMIN.
#define LTC2947_BM_OPCTL_CLR 0x2
// 1: Single Shot Measurement. A single set of measurements of current, voltage, power, temperature and VDVCC are performed and the result registers updated. If CONT is set, it is cleared after completion of any conversion cycle in progress and the single shot measurement is executed. SSHOT is cleared after the single measurement cycle is complete.
#define LTC2947_BM_OPCTL_SSHOT 0x4
// 0: Idle mode (no measurement)
// 1: Continuous measurement is enabled. Measurement cycles run continuously. Charge and energy measurements are only active in continuous mode.
#define LTC2947_BM_OPCTL_CONT 0x8
// Global Reset. When set, the 2947 is reset and all registers are set to their default values.
#define LTC2947_BM_OPCTL_RST 0x80

// PGCTL bit masks
// Memory Map Page Select.
// 0: PAGE 0 of memory map is selected.
// 1: PAGE 1 of memory map is selected.
#define LTC2947_BM_PGCTL_PAGE 0x1

/** @} */ // end of LTC2947 auto generated code

/** @name function prototypes
*  @{
*/

//! Initializes the LTC2947 library for I2C mode operation and configures the slave address
//! see defines LTC2947_I2C_ADDR_LL to LTC2947_I2C_ADDR_RR for possible slave addresses
void LTC2947_InitI2C(
  uint8_t slvAddr //!< 7-bit I2C slave address of the LTC2947 (e.g. LTC2947_I2C_ADDR_LL)
);

//! Initializes the LTC2947 library for SPI mode operation
void LTC2947_InitSPI();

//! Converts an array of 2 bytes to 16-bit unsigned integer
//! @return 16-bit unsigned integer
uint16_t LTC2947_2BytesToUInt16(
  byte *bytes //!< 2 byte array (MSB first)
);

//! converts an array of 3 bytes to 32-bit unsigned integer
//! @return 32-bit unsigned integer
uint32_t LTC2947_3BytesToUInt32(
  byte *bytes //!< 3 byte array (MSB first)
);

//! Converts an array of 4 bytes to 32-bit unsigned integer
//! @return 32-bit unsigned integer
uint32_t LTC2947_4BytesToUInt32(
  byte *bytes //!< 4 byte array (MSB first)
);

//! converts an array of 2 bytes to 16-bit signed integer
//! @return 16-bit signed integer
int16_t LTC2947_2BytesToInt16(
  byte *bytes //!< 2 byte array (MSB first)
);

//! converts an array of 3 bytes to 32-bit signed integer
//! @return 32-bit signed integer
int32_t LTC2947_3BytesToInt32(
  byte *bytes //!< 3 byte array (MSB first)
);

//! Converts an array of 4 bytes to 32-bit signed integer
//! @return 32-bit signed integer
int32_t LTC2947_4BytesToInt32(
  byte *bytes //!< 4 byte array (MSB first)
);

//! Converts an unsigned value of arbitrary number of bytes to a floating point value with the scaling factor lsb
//! The input value must be usigned, use LTC2947_Abs to convert the bytes to an absolute (positive) value or use
//! LTC2947_SignedBytesToDouble instead.
//! For input values with up to 4 bytes use LTC2947_<X>BytesToUInt<N> (e.g. LTC2947_4BytesToUInt32, LTC2947_2BytesToUInt16)
//! and multiply the result by lsb for reduced calculation time, e.g. "LTC2947_4BytesToUInt32(bytes)*lsb".
//! @return result in floating point number format (absolute value, so always positive!)
double LTC2947_UnsignedBytesToDouble(
  uint8_t *unsignedBytes, //!< unsigned input value as byte array (MSB first)
  uint8_t length,         //!< number of bytes of the unsigned value
  double lsb              //!< lsb value (scaling factor used to scale the unsigned value)
);

//! Converts a signed value of arbitrary number of bytes to a floating point value with the scaling factor lsb
//! The input value must be a signed format. For unsigned values use LTC2947_UnsignedBytesToDouble.
//! For input values with up to 4 bytes use LTC2947_<X>BytesToInt<N> (e.g. LTC2947_4BytesToInt32, LTC2947_2BytesToInt16)
//! and multiply the result by lsb for reduced calculation time, e.g. "LTC2947_4BytesToInt32(bytes)*lsb".
//! @return result in floating point number format.
double LTC2947_SignedBytesToDouble(
  uint8_t *signedBytes, //!< signed input value as byte array (MSB first)
  uint8_t length,     //!< number of bytes of the signed value
  double lsb        //!< lsb value (scaling factor used to scale the signed value)
);

//! Converts a signed or unsigned value of arbitrary number of bytes to a floating point number
//! @return result in floating point number format.
double LTC2947_BytesToDouble(
  uint8_t *bytes, //!< input value as byte array (MSB first)
  uint8_t length, //!< number of bytes of the input value
  boolean sig,    //!< true for signed value, false for unsigned
  double lsb      //!< lsb value (scaling factor used to scale the input value)
);

//! Converts a floating point number that was scaled with a given LSB to an integer representation
//! that will be stored in the given byte array.
void LTC2947_DoubleToBytes(
  double value, //!< floating point value
  double lsb,   //!< lsb of the floating point value (integer = value / lsb)
  uint8_t *bytes, //!< byte array of the integer representation
  uint8_t length  //!< length (<=8) of the byte array. Note: The function does not check for overflow of the integer representation
);

//! Prints a 8-bit value in 2-character hexadecimal format with left padded zeros
void LTC2947_SerialPrint8hex(
  uint8_t val //!< 8-bit input value
);

//! Prints a 16-bit value in 4-character hexadecimal format with left padded zeros
void LTC2947_SerialPrint16hex(
  uint16_t val  //!< 16-bit input value
);

//! Prints a 32-bit value in 8-character hexadecimal format with left padded zeros
void LTC2947_SerialPrint32hex(
  uint32_t val  //!< 32-bit input value
);

//! Prints a 64-bit value in 16-character hexadecimal format with left padded zeros
void LTC2947_SerialPrint64hex(
  uint64_t uint64Val  //!< 64-bit input value
);

#ifdef LTC2947_DEBUG
//! conversion function test
void LTC2947_DoubleToBytes_Test();
#endif

//! Calculates the absolute value of a signed value with arbitrary number of bytes
//! @return true: value was inverted, false: value was already positive
boolean LTC2947_Abs(
  uint8_t *bytes,  //!< bytes of the signed value (MSB first)
  uint8_t length   //!< number of bytes
);

//! Reads current (I), power (P), voltage (V), temperature (TEMP)
//! and supply voltage (VCC) from the device
//! Make sure LTC2947's page 0 is selected before calling this function.
//! Use LTC2947_SetPageSelect to change page if necessary
void LTC2947_Read_I_P_V_TEMP_VCC(
  float *I,  //!< Current im amps
  float *P,  //!< Power in watts
  float *V,  //!< Voltage in volts
  float *TEMP, //!< Temperature in degree celcius
  float *VCC   //!< Supply voltage in volts
);

//! Reads charge (C), energy (E) and time (TB) from the device.
//! Charge and Energy are converted to absulte
//! values (always positive!) and a separate sign bit.
//! If the separation in absolute value and sign is not desired use
//! LTC2947_Read_C_E_TB instead.
//! Make sure LTC2947's page 0 is selected before calling this function.
//! Use LTC2947_SetPageSelect to change page if necessary
void LTC2947_Read_Abs_C_E_TB(
  boolean accuSet1, //!< True: Read C1, E1, TB1. False: Read C2, E2, TB2.
  double *C,      //!< Absolute value of charge in As
  boolean *signC,   //!< Sign of charge (True: negative, False: positive)
  double *E,      //!< Absolute value of energy in Ws
  boolean *signE,   //!< Sign of energy (True: negative, False: positive)
  double *TB      //!< Time in s
);

//! Reads charge (C), energy (E) and time (TB) from the device.
//! Make sure LTC2947's page 0 is selected before calling this function.
//! Use LTC2947_SetPageSelect to change page if necessary
void LTC2947_Read_C_E_TB(
  boolean accuSet1, //!< True: Read C1, E1, TB1. False: Read C2, E2, TB2.
  double *C,      //!< Signed charge in As
  double *E,      //!< Signed energy in Ws
  double *TB      //!< Time in s
);

//! read single byte from SPI interface
//! @return always 0
int8_t LTC2947_SpiRdByte(
  uint8_t address,   //!< register address
  uint8_t *value     //!< Byte pointer to store read byte
);

//! write single byte to SPI interface
//! @return always 0
int8_t LTC2947_SpiWrByte(
  uint8_t address,   //!< register address
  uint8_t value      //!< Byte to be written
);

//! read array of bytes from the SPI interface
//! @return always 0
int8_t LTC2947_SpiRdBlock(
  uint8_t address,  //!< register address
  uint8_t length,   //!< Length of array
  uint8_t *values   //!< Byte array to store read bytes
);

//! writes block (array) of bytes to the SPI interface
//! @return always 0
int8_t LTC2947_SpiWrBlock(
  uint8_t address,  //!< register address
  uint8_t length,   //!< Length of array
  uint8_t *values   //!< Byte array to be written
);

//! read single byte via I2C
//! @return 0 if successful, 1 if not successful
int8_t LTC2947_I2CRdByte(
  uint8_t slvAddr, //!< The slv address.
  uint8_t regAddr, //!< The register address.
  uint8_t *value   //!< byte that was read
);

//! read multiple bytes via I2C
//! @return 0 if successful, 1 if not successful
int8_t LTC2947_I2CRdBlock(
  uint8_t slvAddr, //!< The slv address.
  uint8_t regAddr, //!< The register address.
  uint8_t length,  //!< number of bytes.
  uint8_t *values  //!< read bytes
);

//! write single byte via I2C
//! @return 0 if successful, 1 if not successful
int8_t LTC2947_I2CWrByte(
  uint8_t slvAddr, //!< The slv address.
  uint8_t regAddr, //!< The register address.
  uint8_t value    //!< byte to be written
);

//! write byte array via I2C interface
//! @return 0 if successful, 1 if not successful
int8_t LTC2947_I2CWrBlock(
  uint8_t slvAddr, //!< The slv address.
  uint8_t regAddr, //!< The register address.
  uint8_t length,  //!< number of bytes.
  uint8_t *values  //!< bytes to be written
);

//! general I2C communication error
#define LTC2947_ARA_ERROR             0xFF
//! got ARA response from LTC2947
#define LTC2947_ARA_LTC2947_RESPONSE  1
//! got ARA response from some other I2C slave
#define LTC2947_ARA_OTHER_RESPONSE    2
//! got NO ARA response
#define LTC2947_ARA_NO_RESPONSE       3
//! got ARA response from any slave but wihtout the expected WR bit
#define LTC2947_ARA_RESPONSE_WO_WR      4
//! The general alert response address
#define LTC2947_ALERT_RESP_ADDR 0x0C


//! Sends the Alert Response address to the I2C bus and reads the response
//! If two or more devices on the same bus are generating
//! alerts when the ARA is broadcasted, standard I2C arbitra-
//! tion causes the device with the highest priority (lowest
//! address) to reply first and the device with the lowest pri-
//! ority (highest address) to reply last.The bus master will
//! repeat the alert response protocol until the ALERT line is
//! released.
//! @return LTC2947_ARA_ERROR            : general I2C communication error
//!         LTC2947_ARA_LTC2947_RESPONSE : got ARA response from LTC2947
//!         LTC2947_ARA_OTHER_RESPONSE   : got ARA response from some other I2C slave
//!         LTC2947_ARA_NO_RESPONSE      : got NO ARA response
//!         LTC2947_ARA_RESPONSE_WO_WR   : got ARA response from any slave but wihtout the expected WR bit
uint8_t LTC2947_Ara(
  uint8_t *svlAddr //!< 7-bit address of the responding slave
);

//! write LTC2947's page control register to selected one of two memory pages
void LTC2947_SetPageSelect(
  boolean page //!< false: select page 0, true: select page 1
);

//! Wake up LTC2947 from shutdown mode and measure the wakeup time
//! @return -1 in case of timeout or milliseconds it took to wakeup LTC2947
int16_t LTC2947_wake_up();

//! reads LTC2947's page control register to determine the currently selected memory page
//! @return true: page 1 is selected, false: page 0
boolean LTC2947_GetCurrentPageSelect();

//! reads the current GPIO pin state
//! Make sure LTC2947's page 0 is selected before calling this function.
//! Use LTC2947_SetPageSelect to change page if necessary
//! @return true: pin high, false: pin low
boolean LTC2947_GPIO_Read();

//! Enables/disables the output driver on the GPIO pin
//! Make sure LTC2947's page 0 is selected before calling this function.
//! Use LTC2947_SetPageSelect to change page if necessary
void LTC2947_GPIO_PinMode(
  uint8_t mode //!< OUTPUT (1): output driver enabled, INPUT (0): output driver disabled
);

//! Sets the level of the output driver on the GPIO pin
//! This has only an effect if the output driver is enabled, see LTC2947_GPIO_PinMode
//! Make sure LTC2947's page 0 is selected before calling this function.
//! Use LTC2947_SetPageSelect to change page if necessary
void LTC2947_GPIO_SetPinState(
  uint8_t val //!< LOW (0): pin low, HIGH (1): pin high
);

/** @} */ // end of function prototypes

//! controlled by LTC2947_InitI2C / LTC2947_InitSPI to switch between I2C / SPI mode
extern boolean LTC2947_SPI_Mode_Enabled;
//! set by LTC2947_InitI2C to set slave address for I2C operation
extern uint8_t LTC2947_I2C_Slave_Addr;

/** @name serial communication wrapper macros
*  LTC2947's I2C / Spi functions are wrapped to common serial communication functions
*  The user sets the active communication interface by calling LTC2947_InitI2C or LTC2947_InitSPI
*  once at boot-up.
*  @{
*/

//! read multiple bytes via I2C/SPI
//! @return 0 if successful, 1 if not successful
static inline int8_t LTC2947_RD_BYTES(
  uint8_t REG_ADDR, //!< The register address.
  uint8_t LENGTH,   //!< number of bytes.
  uint8_t *BYTES    //!< read bytes
)
{
  return LTC2947_SPI_Mode_Enabled
         ? LTC2947_SpiRdBlock(REG_ADDR, LENGTH, BYTES)
         : LTC2947_I2CRdBlock(LTC2947_I2C_Slave_Addr, REG_ADDR, LENGTH, BYTES);
}

//! write byte array via I2C/SPI interface
//! @return 0 if successful, 1 if not successful
static inline int8_t LTC2947_WR_BYTES(
  uint8_t REG_ADDR, //!< The register address.
  uint8_t LENGTH,   //!< number of bytes.
  uint8_t *BYTES    //!< bytes to be written
)
{
  return LTC2947_SPI_Mode_Enabled
         ? LTC2947_SpiWrBlock(REG_ADDR, LENGTH, BYTES)
         : LTC2947_I2CWrBlock(LTC2947_I2C_Slave_Addr, REG_ADDR, LENGTH, BYTES);
}

//! read single byte via I2C/SPI
//! @return 0 if successful, 1 if not successful
static inline int8_t LTC2947_RD_BYTE(
  uint8_t REG_ADDR, //!< The register address.
  uint8_t *RESULT   //!< byte that was read
)
{
  return LTC2947_SPI_Mode_Enabled
         ? LTC2947_SpiRdByte(REG_ADDR, RESULT)
         : LTC2947_I2CRdByte(LTC2947_I2C_Slave_Addr, REG_ADDR, RESULT);
}

//! write single byte via I2C
//! @return 0 if successful, 1 if not successful
static inline int8_t LTC2947_WR_BYTE(
  uint8_t REG_ADDR, //!< The register address.
  uint8_t VALUE   //!< byte to be written
)
{
  return LTC2947_SPI_Mode_Enabled
         ? LTC2947_SpiWrByte(REG_ADDR, VALUE)
         : LTC2947_I2CWrByte(LTC2947_I2C_Slave_Addr, REG_ADDR, VALUE);
}
/** @} */ // end of serial communication wrapper macros

#endif  // LTC2947_H

