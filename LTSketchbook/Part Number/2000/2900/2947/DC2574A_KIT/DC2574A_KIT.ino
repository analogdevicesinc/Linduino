/**
\date 6/17/2016

\brief
Linduino.INO File for DC2574A_KIT: Standalone power/energy monitor for evaluation of the LTC2947 monitor without a PC
Linear Technology DC2574A_KIT Demonstration Board: Standalone power/energy monitor for evaluation of the LTC2947 monitor without a PC.
LTC2947: high-precision power and energy monitor with an internal sense resistor supporting up to 30A.

NOTE:
The DC2574A-KIT includes the following items:
- DC2334A: LTC2947 Demo Circuit
- DC2026: Linduino with Pre-Programmed "LTC2947 - DC2574A_KIT Linduino.INO File" (THIS FILE!)
- LCD Keypad Shield
- USB Cable: Power Supply And Optional Communication Interface for Operating with the GUI
- 14-pin Ribbon Cable.

The DC2574A_KIT Linduino sketch contains a lot of code that is only necessary to operate with the
LCD, the serial ports and to allow compatibility with DC590 commands (see usage of GUI together with
the DC2574A-KIT). This code might not be of interest by the customer that wants to focuse on functionality
of the LTC2947. The focuse on LTC2947 relevant code only, just search for LTC2947_ within this source file.

Setup:
Set the terminal baud rate to 115200 and de-select the newline terminator. (That means
e.g. within the serial monitor of the Arduino IDE select "No line ending")
Refer to Demo Manual DC2574A_KIT.
Ensure all jumpers are installed in the factory default positions.
Especially make sure the board is configured to I2C mode with slave address set
to 0x5C (LL), see also below the define LTC2947_I2C_ADDRESS

Command Description:
After start-up the DC2574A_KIT Linduino sketch will send the following lines
on the connected serial com port:

Hello LTC2947
30A PowerMonitor
LCD found! Enter l or m for options.

- or in case no LCD Keypad Shield is plugged on top of the DC2334A demoboard -

No LCD found! Enter l or m for options.

In any case commands l or m can be send:
l-command: Status/Alert regsierts, the following submenu will be shown:
0:STATUS
1:STATVT
2:STATIP
3:STATC
4:STATE
5:STATCEOF
6:STATTB
7:STATVDVCC
8:CYCLE ALL
enter 0-8, any other to cancel

Select one out of 0-7 to report a single status/alert register or 8 to cycle
through all status/alert registers. In both cases new values will be reported
every second. E.g. for option 0 (STATUS) you will get the following output:
STAT:UPD,

STAT:UPD,
...

The bits of the status/alert registers that are set to 1 will be reported, e.g. UPD
is the UPDATE bit of the STATUS register that tells a new conversion cycle
was performed.

Option 8 (CYCLE ALL) will show the following output:

STAT:UPD,

STATVT:

STATIP:

STATC:

STATE:

STATCEOF:

STATTB:

STATVDVCC:

In the example output above only the UPDATE bit was set

m-command: Measurement values, the following submenu will be shown:
0: I V
P TEMP
1:C1 VDVCC
E1 TB1
2:C2 VDVCC
E2 TB2
3:IMIN IMAX
PMIN PMAX
4:VMIN VMAX
TMIN TMAX
5:TMIN TMAX
VDMIN VDMAX
6:CYCLE ALL
measurements
enter 0-6, any other to cancel

Select one out of 0-5 to report a single set of measurement values or 8 to cycle
through all measurements. In both cases new values will be reported
every second. E.g. for option 0 (I V P TEMP) you will get the following output:

I V
P TEMP
0.012A -0.342V
0.000W 30.2°C
0.006A -0.342V
0.000W 30.2°C
0.012A -0.342V
0.000W 30.2°C
I V
P TEMP
0.012A -0.342V
0.000W 30.2°C
....

Current, Voltage, Power and die temperature are reported every second.
Option 6 (CYCLE ALL) will show the following output:

I V
P TEMP
0.006A -0.342V
0.000W 30.2°C
0.012A -0.342V
0.000W 30.2°C
0.012A -0.342V
0.000W 30.2°C
C1 VDVCC
E1 TB1
4.83mAh 4.50V
0.00mWh 34m35s
4.83mAh 4.50V
0.00mWh 34m36s
4.83mAh 4.50V
0.00mWh 34m37s
C2 VDVCC
E2 TB2
4.84mAh 4.64V
0.00mWh 34m40s
4.84mAh 4.50V
0.00mWh 34m41s
4.85mAh 4.50V
0.00mWh 34m42s
....

In case a LCD Keypad Shield is plugged on top of the DC2334A demoboard, all
output will also be shown on the LCD.

DC590B compatibility:
Appart from commands m and l the DC2574A_KIT sketch also supports a subset of the DC590B commands.
This way the LTC2947 GUI is also able to communicate with the DC2574A_KIT sketch which enables the
user to use the GUI to change LTC2947 configuration e.g. to set alarm thresholds etc.
The main limitations of this compatibility mode are:
- EEPROM read from the DC2334A demo board is faked (it will always report the right EEPROM string)
- SPI operation is not supported, due to incompatibility with the optional LCD Keypad Shield

See tinyDC590.cpp for details

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
Linduino .INO file for LTC2947 a high-precision power and energy monitor with an internal sense resistor supporting up to 30A.
*/

#include <SPI.h>
#include <LT_SPI.h>
#include <LT_I2C.h>
#include <Linduino.h>
#include <LiquidCrystal.h>
#include "LCDKeypad.h"
#include "DC590_subset.ino"

//! DC590 busy check. Declared and controlled by DC590_subset.ino
//! true if DC590 is busy processing commands, false if not busy
extern boolean dc590Busy;

/** \brief Edit LTC2947.h to adjust the following settings:
  Internal / External clock operation and frequency setting
  I2C / SPI mode operation
  In case of I2C: Slave address configuration
*/
#include "LTC2947.h"

/** \brief
Define either LTC2947_I2C_MODE_ENABLE or LTC2947_SPI_MODE_ENABLE to set the communication interface to I2C or SPI
NOTE: It is not possible to operate in SPI mode together with the LCD due to pin conflicts. If you want to
operate in SPI mode you have to remove the LCD and connect DC2334A via the 14-pin ribbon cable to J1 of Linduino
instead of connecting DC2334A as a shield on top of Linduino.
*/
#define LTC2947_I2C_MODE_ENABLE
//#define LTC2947_SPI_MODE_ENABLE

/** \brief  Only one mode is possible at a time. If I2C and SPI are both enabled, I2C mode will be used! */
#ifdef LTC2947_I2C_MODE_ENABLE
#undef LTC2947_SPI_MODE_ENABLE
#endif

/** \brief  update interval time in milli seconds */
#define SCREEN_UPDATE_TIME    1000UL
#define LABEL_DELAY           2000UL
#define SPLASH_SCREEN_DELAY   3000UL
/** \brief Menu Group 1 count: STATUS STATVT STATIP STATC STATE STATCEOF STATTB STATVDVCC "CYCLE stats" */
#define USER_SEL_GRP1_COUNT 9
/** \brief  Menu Group 1 cycle count: STATUS STATVT STATIP STATC STATE STATCEOF STATTB STATVDVCC */
#define USER_SEL_GRP1_CYCLE_COUNT 8
/** \brief  Menu group 2 count: RAW, ACCU1, ACCU2, Track1, Track2, Track3, "Cycle meas." */
#define USER_SEL_GRP2_COUNT 7
/** \brief  Menu Group 2 cycle count:  RAW, ACCU1, ACCU2, Track1, Track2, Track3 */
#define USER_SEL_GRP2_CYCLE_COUNT 6
/** \brief  defines how many cycles a single measurement group will be displayed on the LCD in case of "cycle all" (2 means each measurement is shown 3 times) */
#define USERGROUPCYCLETIMERRELOAD 3

#define MEAS_LABEL00 F("I V")
#define MEAS_LABEL01 F("P TEMP")
#define MEAS_LABEL10 F("C1 VDVCC")
#define MEAS_LABEL11 F("E1 TB1")
#define MEAS_LABEL20 F("C2 VDVCC")
#define MEAS_LABEL21 F("E2 TB2")
#define MEAS_LABEL30 F("IMIN IMAX")
#define MEAS_LABEL31 F("PMIN PMAX")
#define MEAS_LABEL40 F("VMIN VMAX")
#define MEAS_LABEL41 F("TMIN TMAX")
#define MEAS_LABEL50 F("TMIN TMAX")
#define MEAS_LABEL51 F("VDMIN VDMAX")
#define MEAS_LABEL60 F("CYCLE ALL")
#define MEAS_LABEL61 F("measurements")

/** \brief  The next update time. Stores the next time stamp (used for timing of update task) */
unsigned long nextUpdateTime = 0;

/** \brief  pointer to the LCD keypad object */
LCDKeypad *lcd;
/** \brief  The previous key. Stores previously pressed key*/
int8_t previousKey = KEYPAD_NONE;
/** \brief  flag to store actions from user input. */
boolean flagClear = false;
/** \brief  User selection of the first group. */
uint8_t userSelGroup1 = 0;
/** \brief  group 1 cycle enable */
boolean userGroup1Cycle = false;
/** \brief  User selection of the second group. */
uint8_t userSelGroup2 = 0;
/** \brief  group 2 cycle enable */
boolean userGroup2Cycle = false;
/** \brief selected group 1: false, 2: true */
boolean userGroupSelect = true;
/** \brief  flag is asserted once after all status registers were read. */
boolean statusRegsRead = false;
/** \brief  array to store all status / alert registers */
byte statusRegs[8];
/** \brief  The user group cycle timer. */
uint8_t userGroupCycleTimer = 0;

/**  \fn  void setup()
 \brief general initialization */
void setup()
{
  // init serial port
  Serial.begin(115200);
  // connect to LCD
  lcd = new LCDKeypad(10);
#ifdef LTC2947_SPI_MODE_ENABLE
  lcd->Disable4BitLCD();
  LTC2947_InitSPI();
  spi_enable(SPI_CLOCK_DIV16);  // Configure the spi port for 1MHz SCK (16MHz / 16)
  quikeval_SPI_connect();
#else
  lcd->searchForLCD();
  // set LTC2947 library to I2C mode
  LTC2947_InitI2C(LTC2947_I2C_ADDR_LL);
  // Initializes Linduino I2C port.
  quikeval_I2C_init();
  // Connects I2C to QuikEval port
  quikeval_I2C_connect();
#endif
  // make a reset of LTC2947
  LTC2947_WR_BYTE(LTC2947_REG_OPCTL, LTC2947_BM_OPCTL_RST);
  // give device some time to restart
  DelayAndCheckSerialData(120);
  // write PRE and DIV setting
  LTC2947_WR_BYTE(LTC2947_REG_TBCTL,
                  LTC2947_PRE << BM2BITPOS(LTC2947_BM_TBCTL_PRE_0) |
                  LTC2947_DIV << BM2BITPOS(LTC2947_BM_TBCTL_DIV_0));
  // enable continuous measurement on LTC2947
  LTC2947_WR_BYTE(LTC2947_REG_OPCTL, LTC2947_BM_OPCTL_CONT);
  CLEAR_SCREEN();
  lcd->EnableSerialOutput = true;
  PRINTSCREEN(F("Hello LTC2947"));
  NEWLINE();
  PRINTSCREEN(F("30A PowerMonitor"));
  lcd->EnableSerialOutput = false;
  Serial.println();
  if (!lcd->foundLCD)
    Serial.print(F("No "));
  Serial.println(F("LCD found! Enter l or m for options."));
  Serial.flush();
  DelayAndProcessInputs(SPLASH_SCREEN_DELAY); //DelayAndCheckSerialData(SPLASH_SCREEN_DELAY);
}

/**  \fn  void update()
\brief update LTC2947 measurement results */
void update()
{
  // general buffer to hold data from 2947
  byte data[8];
  // stores the last value or PGCTL
  boolean lastPageSelect;
  // 1st check if we will actually read values
  // from the device or just show labels of
  // measured values on the LCD
  if (!flagClear && userGroupSelect)
  {
    // labels might only be shown if no clear is requested and measurement values will be shown
    boolean showHeader = false;
    if (userGroupCycleTimer == 0)
    {
      if (userGroup2Cycle)
        userSelGroup2 = (userSelGroup2 + 1) % USER_SEL_GRP2_CYCLE_COUNT;
      userGroupCycleTimer = USERGROUPCYCLETIMERRELOAD;
      showHeader = true;
    }
    else
      userGroupCycleTimer--;
    if (showHeader)
    {
      CLEAR_SCREEN();
      switch (userSelGroup2)
      {
        case 0:
          PRINTSCREEN(MEAS_LABEL00);
          NEWLINE();
          PRINTSCREEN(MEAS_LABEL01);
          DelayAndProcessInputs(LABEL_DELAY);
          return;
        case 1:
          PRINTSCREEN(MEAS_LABEL10);
          NEWLINE();
          PRINTSCREEN(MEAS_LABEL11);
          DelayAndProcessInputs(LABEL_DELAY);
          return;
        case 2:
          PRINTSCREEN(MEAS_LABEL20);
          NEWLINE();
          PRINTSCREEN(MEAS_LABEL21);
          DelayAndProcessInputs(LABEL_DELAY);
          return;
        case 3:
          PRINTSCREEN(MEAS_LABEL30);
          NEWLINE();
          PRINTSCREEN(MEAS_LABEL31);
          DelayAndProcessInputs(LABEL_DELAY);
          return;
        case 4:
          PRINTSCREEN(MEAS_LABEL40);
          NEWLINE();
          PRINTSCREEN(MEAS_LABEL41);
          DelayAndProcessInputs(LABEL_DELAY);
          return;
        default:
          PRINTSCREEN(MEAS_LABEL50);
          NEWLINE();
          PRINTSCREEN(MEAS_LABEL51);
          DelayAndProcessInputs(LABEL_DELAY);
          return;
      }
    }
  }
  // check if we are in cont mode
  LTC2947_RD_BYTE(LTC2947_REG_OPCTL, data);
  if (bitMaskClrChk(data[0], LTC2947_BM_OPCTL_CONT))
  {
    // enable continuous measurement on LTC2947
    LTC2947_WR_BYTE(LTC2947_REG_OPCTL, LTC2947_BM_OPCTL_CONT);
    return; // we were in IDLE, so nothing todo here
  }
  // store page select
  // this is needed to restore last selected page to avoid page missmatch
  // if the LTC2947 GUI is also communicating with Linduino
  lastPageSelect = LTC2947_GetCurrentPageSelect();
  if (lastPageSelect)
    LTC2947_SetPageSelect(false); // go to page 0 where the measuremnt results are stored
#ifdef LTC2947_AUTO_READ_STATUS
  // reading the STATUS register would also affect the GUI as status register bits are CoR!!!
  // for this reason the following code is not enabled!
  // still KEYPAD_RIGHT allows read of STATUS on demand and print result on LCD
  // read all status and alert (if alerts are used it is important to read all in a single burst of 8 bytes!)
  LTC2947_RD_BYTES(LTC2947_REG_STATUS, 8, data);
  // if UPDATE bit is set we have a new conversion result
  if (bitMaskSetChk(data[0], LTC2947_BM_STATUS_UPDATE))
    return; // UPDATE bit not set, so no new data
#endif
  if (flagClear)
  {
    // clear everything
    flagClear = false;
    NEWLINE();
    PRINTSCREEN(F("****CLEARED*****"));
    LTC2947_WR_BYTE(LTC2947_REG_OPCTL, 0); // leave continuous mode and go to IDLE
    DelayAndCheckSerialData(100);
    LTC2947_WR_BYTE(LTC2947_REG_OPCTL, LTC2947_BM_OPCTL_CLR); // clear tracking registers and accumulators
    DelayAndCheckSerialData(50);
    LTC2947_WR_BYTE(LTC2947_REG_OPCTL, LTC2947_BM_OPCTL_CONT); // enable continuous
  }
  else
  {
    if (userGroupSelect)
    {
      switch (userSelGroup2)
      {
        case 0://PRINTSCREEN(F("I V P TEMP"));
          {
            // show RAW quantities like I,P,V,TEMP
            float I, P, V, TEMP, VCC;
            LTC2947_Read_I_P_V_TEMP_VCC(&I, &P, &V, &TEMP, &VCC);
            CheckSerialData();// we execute CheckSerialData() from time to time here to make sure the uart rx buffer never overflows
            // print current and limit decimal points for values -30.00 ... -10.00 (but -9.999 ... 30.000)
            CLEAR_SCREEN();
            PRINTSCREEN2(I, I > -10.0 ? 3 : 2);
            PRINTSCREEN('A');
            SCREENSETCURSOR(8, 0);
            PRINTSCREEN2(V, 3);
            PRINTSCREEN('V');
            CheckSerialData();
            NEWLINE();
            PRINTSCREEN2(P, 3); // max characters: -450.000W = 9: 0..8
            PRINTSCREEN('W');
            boolean limitTempCharacters = P <= -100.0;
            SCREENSETCURSOR(10, 1);
            PRINTSCREEN2(TEMP, limitTempCharacters ? 0 : 1);
            WRITE_DEGREE_C();
          }
          break;
        case 1://PRINTSCREEN(F("C1 VDVCC E1 TB1"));
        case 2://PRINTSCREEN(F("C2 VDVCC E2 TB2"));
          {
            double C, E, TB;
            boolean signC = true, signE = true;
#ifndef LTC2947_READ_SIGNED_ACCUS
            LTC2947_Read_Abs_C_E_TB(userSelGroup2 == 1, &C, &signC, &E, &signE, &TB); //Read either C1/2,E1/2,TB1/2
#else
            // instead of using LTC2947_Read_Abs_C_E_TB we could also call LTC2947_Read_C_E_TB to
            // get signed charge and energy values. But as the following code makes usage of
            // LTC2947_Read_Abs_C_E_TB's separation in absolute values and sign, we won't use
            // LTC2947_Read_C_E_TB here!
            LTC2947_Read_C_E_TB(userSelGroup2 == 1, &C, &E, &TB); //Read either C1/2,E1/2,TB1/2
            if (C < 0) C = -C;
            else signC = false;
            if (E < 0) E = -E;
            else signE = false;
#endif
            C = C * LTC2947_LSB_FACTOR_MILLI_HOURS; //! convert As to mAh
            E = E * LTC2947_LSB_FACTOR_MILLI_HOURS; //! convert Ws to mWh
            CLEAR_SCREEN();
            // convert to reasonable units
            if (C < 1e3)
            {
              // restore sign and print value in mAh
              PRINTSCREEN2(signC ? -C : C, 2); // e.g. -999.99mAh: 10 chars
              PRINTSCREEN(F("mAh"));
            }
            else if (C < 1e6)
            {
              // restore sign and print value in Ah
              C = C * 1e-3;
              PRINTSCREEN2(signC ? -C : C, 3); // e.g. -999.999Ah: 10 chars
              PRINTSCREEN(F("Ah"));
            }
            else if (C < 1e9)
            {
              // restore sign and print value in kAh
              C = C * 1e-6;
              PRINTSCREEN2(signC ? -C : C, 2); // e.g. -999.99kAh: 10 chars
              PRINTSCREEN(F("kAh"));
            }
            CheckSerialData();
            SCREENSETCURSOR(11, 0);
            LTC2947_RD_BYTES(LTC2947_VAL_VDVCC, 2, data); // read VDVCC[15:0]
            double vcc = LTC2947_BytesToDouble(data, 2, true, LTC2947_LSB_VDVCC * 1e-3); // convert to real volts
            PRINTSCREEN2(vcc, 2);
            PRINTSCREEN('V'); // e.g. 4.65V: 5 chars
            CheckSerialData();
            NEWLINE();
            // convert to reasonable units
            if (E < 1e3)
            {
              // restore sign and print value in mWh
              PRINTSCREEN2(signE ? -E : E, signE && (E >= 1e2) ? 1 : 2);
              PRINTSCREEN(F("mWh"));
            }
            else if (E < 1e6)
            {
              // restore sign and print value in Wh
              E = E * 1e-3;
              PRINTSCREEN2(signE ? -E : E, signE && (E >= 1e5) ? 2 : 3);
              PRINTSCREEN(F("Wh"));
            }
            else if (E < 1e9)
            {
              // restore sign and print value in kWh
              E = E * 1e-6;
              PRINTSCREEN2(signE ? -E : E, signE && (E >= 1e8) ? 1 : 2);
              PRINTSCREEN(F("kWh"));
            }
            CheckSerialData();
            SCREENSETCURSOR(10, 1);
            // convert to reasonable units
            if (TB < 60.0) // less than one minute
            {
              //print time in seconds
              PRINTSCREEN2(TB, 2);
              PRINTSCREEN('s');//e.g. 59.9s
            }
            else if (TB < 60.0 * 60.0) // less than one hour
            {
              //print time in minutes and seconds
              uint16_t intSec = TB;
              PRINTSCREEN(intSec / 60);
              PRINTSCREEN('m');
              PRINTSCREEN(intSec % 60);
              PRINTSCREEN('s');//e.g. 59m59s
            }
            else if (TB < 60.0 * 60.0 * 24.0)
            {
              //print time in hours and minutes
              uint16_t intMin = TB / 60.0;
              PRINTSCREEN(intMin / 60);
              PRINTSCREEN('h');
              PRINTSCREEN(intMin % 60);
              PRINTSCREEN('m');//e.g. 23h59m
            }
            else if (TB < 60.0 * 60.0 * 24.0 * 99.0)
            {
              //print time in days and hours
              uint16_t inthour = TB / (60.0 * 60.0);
              PRINTSCREEN(inthour / 24);
              PRINTSCREEN('d');
              PRINTSCREEN(inthour % 24);
              PRINTSCREEN('h');//e.g. 99d23h
            }
          }
          break;
        case 3://IMIN IMAX / PMIN PMAX
          {
            float floatHelp;
            LTC2947_RD_BYTES(LTC2947_VAL_IMAX, 8, data); // IMAX[15:0] IMIN[15:0] PMAX[15:0] PMIN[15:0] starting at data[0]
            CheckSerialData();
            floatHelp = LTC2947_2BytesToInt16(data + 2) * LTC2947_LSB_IMIN * 1e-3;// calc min current in amps
            CLEAR_SCREEN();
            PRINTSCREEN2(floatHelp, 2); //e.g. -30.00...30.00A..-30.00A
            PRINTSCREEN(F("..."));
            floatHelp = LTC2947_2BytesToInt16(data) * LTC2947_LSB_IMAX * 1e-3;
            PRINTSCREEN2(floatHelp, 2);
            PRINTSCREEN('A');
            CheckSerialData();
            floatHelp = LTC2947_2BytesToInt16(data + 6) * LTC2947_LSB_PMIN * 1e-3;// calc power in watts
            NEWLINE();
            PRINTSCREEN2(floatHelp, 1); // e.g. -450.0W
            PRINTSCREEN(F("..."));
            floatHelp = LTC2947_2BytesToInt16(data + 4) * LTC2947_LSB_PMAX * 1e-3;
            PRINTSCREEN2(floatHelp, 1);
            PRINTSCREEN('W');
          }
          break;
        case 4://VMIN VMAX / TMIN TMAX
          {
            float floatHelp;
            LTC2947_RD_BYTES(LTC2947_VAL_VMAX, 8, data); // VMAX[15:0] VMIN[15:0] TEMPMAX[15:0] TEMPMIN[15:0] starting at data[0]
            CheckSerialData();
            floatHelp = LTC2947_2BytesToInt16(data + 2) * LTC2947_LSB_VMIN * 1e-3;
            CLEAR_SCREEN();
            PRINTSCREEN2(floatHelp, 2);
            PRINTSCREEN(F("..."));
            floatHelp = LTC2947_2BytesToInt16(data) * LTC2947_LSB_VMAX * 1e-3;
            PRINTSCREEN2(floatHelp, 2);
            PRINTSCREEN('V');
            CheckSerialData();
            floatHelp = LTC2947_2BytesToInt16(data + 6) * LTC2947_LSB_TEMPMIN + LTC2947_OFFS_TEMPMIN;
            boolean limitTempCharacters = floatHelp <= -100.0;
            NEWLINE();
            PRINTSCREEN2(floatHelp, limitTempCharacters ? 0 : 1);
            PRINTSCREEN(F("..."));
            floatHelp = LTC2947_2BytesToInt16(data + 4) * LTC2947_LSB_TEMPMAX + LTC2947_OFFS_TEMPMAX;
            limitTempCharacters = floatHelp <= -100.0;
            PRINTSCREEN2(floatHelp, limitTempCharacters ? 0 : 1);
            WRITE_DEGREE_C();
          }
          break;
        default://TMIN TMAX / VDMIN VDMAX
          {
            float floatHelp;
            LTC2947_RD_BYTES(LTC2947_VAL_TEMPMAX, 8, data); // TEMPMAX[15:0] TEMPMIN[15:0] VDVCCMAX[15:0] VDVCCMIN[15:0] starting at data[0]
            CheckSerialData();
            floatHelp = LTC2947_2BytesToInt16(data + 2) * LTC2947_LSB_TEMPMIN + LTC2947_OFFS_TEMPMIN;
            boolean limitTempCharacters = floatHelp <= -100.0;
            CLEAR_SCREEN();
            PRINTSCREEN2(floatHelp, limitTempCharacters ? 0 : 1);
            PRINTSCREEN(F("..."));
            floatHelp = LTC2947_2BytesToInt16(data) * LTC2947_LSB_TEMPMAX + LTC2947_OFFS_TEMPMAX;
            limitTempCharacters = floatHelp <= -100.0;
            PRINTSCREEN2(floatHelp, limitTempCharacters ? 0 : 1);
            WRITE_DEGREE_C();
            CheckSerialData();
            floatHelp = LTC2947_2BytesToInt16(data + 6) * LTC2947_LSB_VDVCCMIN * 1e-3;
            NEWLINE();
            PRINTSCREEN2(floatHelp, 0);
            PRINTSCREEN(F("..."));
            floatHelp = LTC2947_2BytesToInt16(data + 4) * LTC2947_LSB_VDVCCMAX * 1e-3;
            PRINTSCREEN2(floatHelp, 0);
            PRINTSCREEN('V');
          }
          break;
      }
    }
    else
    {
      if (userGroup1Cycle)
        userSelGroup1 = (userSelGroup1 + 1) % USER_SEL_GRP1_CYCLE_COUNT;
      else
        statusRegsRead = false;// in case we don't cycle the status regs are read always
      if (!statusRegsRead)
      {
        // read all status and alert (if alerts are used it is important to read all in a single burst of 8 bytes!)
        LTC2947_RD_BYTES(LTC2947_REG_STATUS, 8, statusRegs);
        // all status & alert regs are cleared-on-read. To make sure we report all bits correctly we
        // only re-read them once we cycled through all of them (see last case STATVDVCC in which we clear
        // statusRegsRead again.
        statusRegsRead = true;
      }
      CLEAR_SCREEN();
      switch (userSelGroup1)
      {
        case 0:
          PRINTSCREEN(F("STAT:"));
          if (bitMaskSetChk(statusRegs[0], LTC2947_BM_STATUS_UPDATE))
            PRINTSCREEN(F("UPD,"));
          if (bitMaskSetChk(statusRegs[0], LTC2947_BM_STATUS_ADCERR))
            PRINTSCREEN(F("AER,"));
          if (bitMaskSetChk(statusRegs[0], LTC2947_BM_STATUS_TBERR))
            PRINTSCREEN(F("TER"));
          NEWLINE();
          if (bitMaskSetChk(statusRegs[0], LTC2947_BM_STATUS_UVLOA))
            PRINTSCREEN(F("UVA,"));
          if (bitMaskSetChk(statusRegs[0], LTC2947_BM_STATUS_PORA))
            PRINTSCREEN(F("POR,"));
          if (bitMaskSetChk(statusRegs[0], LTC2947_BM_STATUS_UVLOSTBY))
            PRINTSCREEN(F("UVS,"));
          if (bitMaskSetChk(statusRegs[0], LTC2947_BM_STATUS_UVLOD))
            PRINTSCREEN(F("UVD"));
          break;
        case 1:
          PRINTSCREEN(F("STATVT:"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATVT - LTC2947_REG_STATUS], LTC2947_BM_STATVT_VH))
            PRINTSCREEN(F("VH,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATVT - LTC2947_REG_STATUS], LTC2947_BM_STATVT_VL))
            PRINTSCREEN(F("VL"));
          NEWLINE();
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATVT - LTC2947_REG_STATUS], LTC2947_BM_STATVT_TEMPH))
            PRINTSCREEN(F("TEH,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATVT - LTC2947_REG_STATUS], LTC2947_BM_STATVT_TEMPL))
            PRINTSCREEN(F("TEL,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATVT - LTC2947_REG_STATUS], LTC2947_BM_STATVT_FANH))
            PRINTSCREEN(F("FH,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATVT - LTC2947_REG_STATUS], LTC2947_BM_STATVT_FANL))
            PRINTSCREEN(F("FL"));
          break;
        case 2:
          PRINTSCREEN(F("STATIP:"));
          NEWLINE();

          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATIP - LTC2947_REG_STATUS], LTC2947_BM_STATIP_IH))
            PRINTSCREEN(F("IH,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATIP - LTC2947_REG_STATUS], LTC2947_BM_STATIP_IL))
            PRINTSCREEN(F("IL,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATIP - LTC2947_REG_STATUS], LTC2947_BM_STATIP_PH))
            PRINTSCREEN(F("PH,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATIP - LTC2947_REG_STATUS], LTC2947_BM_STATIP_PL))
            PRINTSCREEN(F("PL"));
          break;
        case 3:
          PRINTSCREEN(F("STATC:"));
          NEWLINE();
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATC - LTC2947_REG_STATUS], LTC2947_BM_STATC_C1H))
            PRINTSCREEN(F("C1H,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATC - LTC2947_REG_STATUS], LTC2947_BM_STATC_C1L))
            PRINTSCREEN(F("C1L,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATC - LTC2947_REG_STATUS], LTC2947_BM_STATC_C2H))
            PRINTSCREEN(F("C2H,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATC - LTC2947_REG_STATUS], LTC2947_BM_STATC_C2L))
            PRINTSCREEN(F("C2L"));
          break;
        case 4:
          PRINTSCREEN(F("STATE:"));
          NEWLINE();
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATE - LTC2947_REG_STATUS], LTC2947_BM_STATE_E1H))
            PRINTSCREEN(F("E1H,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATE - LTC2947_REG_STATUS], LTC2947_BM_STATE_E1L))
            PRINTSCREEN(F("E1L,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATE - LTC2947_REG_STATUS], LTC2947_BM_STATE_E2H))
            PRINTSCREEN(F("E2H,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATE - LTC2947_REG_STATUS], LTC2947_BM_STATE_E2L))
            PRINTSCREEN(F("E2L"));
          break;
        case 5:
          PRINTSCREEN(F("STATCEOF:"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATCEOF - LTC2947_REG_STATUS], LTC2947_BM_STATCEOF_C1OF))
            PRINTSCREEN(F("C1OF"));
          NEWLINE();
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATCEOF - LTC2947_REG_STATUS], LTC2947_BM_STATCEOF_C2OF))
            PRINTSCREEN(F("C2OF,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATCEOF - LTC2947_REG_STATUS], LTC2947_BM_STATCEOF_E1OF))
            PRINTSCREEN(F("E1OF,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATCEOF - LTC2947_REG_STATUS], LTC2947_BM_STATCEOF_E2OF))
            PRINTSCREEN(F("E2OF"));
          break;
        case 6:
          PRINTSCREEN(F("STATTB:"));
          NEWLINE();
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATTB - LTC2947_REG_STATUS], LTC2947_BM_STATTB_TB1OF))
            PRINTSCREEN(F("1OF,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATTB - LTC2947_REG_STATUS], LTC2947_BM_STATTB_TB2OF))
            PRINTSCREEN(F("2OF,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATTB - LTC2947_REG_STATUS], LTC2947_BM_STATTB_TB1TH))
            PRINTSCREEN(F("1TH,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATTB - LTC2947_REG_STATUS], LTC2947_BM_STATTB_TB2TH))
            PRINTSCREEN(F("2TH"));
          break;
        default:
          PRINTSCREEN(F("STATVDVCC:"));
          NEWLINE();
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATVDVCC - LTC2947_REG_STATUS], LTC2947_BM_STATVDVCC_VDVCCH))
            PRINTSCREEN(F("VDVCCH,"));
          if (bitMaskSetChk(statusRegs[LTC2947_REG_STATVDVCC - LTC2947_REG_STATUS], LTC2947_BM_STATVDVCC_VDVCCL))
            PRINTSCREEN(F("VDVCCL"));
          statusRegsRead = false;// this will trigger a re-read of all status register in the next cycle
          break;
      }
    }
  }
  // go back to page 1 if we initially had to switch to page 0
  // this is only neccessary for compatibility with the GUI
  if (lastPageSelect)
    LTC2947_SetPageSelect(true); // go back to page 1
}

/**  \fn  void loop()
 \brief main loop */
void loop()
{
  DelayAndProcessInputs(0);
}

/**  \fn  void DelayAndProcessInputs(unsigned long ms)
\brief Delay and process input commands either from serial port or from keypad
\param  ms  delay in milliseconds
*/
void DelayAndProcessInputs(unsigned long ms)
{
  unsigned long currentMillis = millis();
  ms += currentMillis; // calculate delay end time
  do
  {
    if (dc590Busy || CheckSerialData())
    {
      // DC590 is still busy or new data available from serial port
      char cmd = get_char();
      if (!tinyDC590B(cmd))
        processCommands(cmd); // no DC590B command so check for other commands
      else if (cmd == 'i' || cmd == 'I') // check if one of the id commands was received (quikeval and GUI will send this command)
        lcd->EnableSerialOutput = false; // disable any serial output
      return;
    }
    if (currentMillis < ms)
    {
      currentMillis = millis();
      continue;
    }
    if (currentMillis >= nextUpdateTime)
    {
      // make the periodic update of LTC2947 measurement results
      nextUpdateTime = currentMillis + SCREEN_UPDATE_TIME;// calc next update time
      if (!userInput())
        update(); // update data from LTC2947
    }
    break;
  }
  while (true);
}

/**  \fn  boolean userInput()
 \brief  processes the user input from LCD keypad shield
 \return  true if any key was pressed on the LDC keypad shield */
boolean userInput()
{
  if (!lcd->foundLCD)
    return false;
  int8_t key = lcd->button();// read pressed key
  if (key != KEYPAD_NONE)
  {
    CLEAR_SCREEN();
    // desable cyclic operation
    userGroup1Cycle = false;
    userGroup2Cycle = false;
    userGroupCycleTimer = 0;
  }
  switch (key)
  {
    case KEYPAD_NONE:
      break;
    case KEYPAD_LEFT:
    case KEYPAD_RIGHT:
      if (previousKey == KEYPAD_LEFT)
      {
        if (userSelGroup2 == 0)
          userSelGroup2 = USER_SEL_GRP2_COUNT - 1;
        else
          userSelGroup2--;
      }
      else if (previousKey == KEYPAD_RIGHT)
      {
        if (userSelGroup2 == USER_SEL_GRP2_COUNT - 1)
          userSelGroup2 = 0;
        else
          userSelGroup2++;
      }
      userGroupSelect = true;
      userGroupCycleTimer = 0;
      switch (userSelGroup2)
      {
        case 0:
          PRINTSCREEN(F("I V"));
          NEWLINE();
          PRINTSCREEN(F("P TEMP"));
          break;
        case 1:
          PRINTSCREEN(F("C1 VDVCC"));
          NEWLINE();
          PRINTSCREEN(F("E1 TB1"));
          break;
        case 2:
          PRINTSCREEN(F("C2 VDVCC"));
          NEWLINE();
          PRINTSCREEN(F("E2 TB2"));
          break;
        case 3:
          PRINTSCREEN(F("IMIN IMAX"));
          NEWLINE();
          PRINTSCREEN(F("PMIN PMAX"));
          break;
        case 4:
          PRINTSCREEN(F("VMIN VMAX"));
          NEWLINE();
          PRINTSCREEN(F("TMIN TMAX"));
          break;
        case 5:
          PRINTSCREEN(F("TMIN TMAX"));
          NEWLINE();
          PRINTSCREEN(F("VDMIN VDMAX"));
          break;
        default:
          PRINTSCREEN(F("CYCLE ALL"));
          NEWLINE();
          PRINTSCREEN(F("measurements"));
          userGroup2Cycle = true;
          break;
      }
      break;
    case KEYPAD_UP:
    case KEYPAD_DOWN:
      if (previousKey == KEYPAD_DOWN)
      {
        if (userSelGroup1 == 0)
          userSelGroup1 = USER_SEL_GRP1_COUNT - 1;
        else
          userSelGroup1--;
      }
      else if (previousKey == KEYPAD_UP)
      {
        if (userSelGroup1 == USER_SEL_GRP1_COUNT - 1)
          userSelGroup1 = 0;
        else
          userSelGroup1++;
      }
      userGroupSelect = false;
      userGroupCycleTimer = 0;
      PRINTSCREEN(F("STATUS,ALERTS:"));
      NEWLINE();
      switch (userSelGroup1)
      {
        case 0:
          PRINTSCREEN(F("STATUS"));
          break;
        case 1:
          PRINTSCREEN(F("STATVT"));
          break;
        case 2:
          PRINTSCREEN(F("STATIP"));
          break;
        case 3:
          PRINTSCREEN(F("STATC"));
          break;
        case 4:
          PRINTSCREEN(F("STATE"));
          break;
        case 5:
          PRINTSCREEN(F("STATCEOF"));
          break;
        case 6:
          PRINTSCREEN(F("STATTB"));
          break;
        case 7:
          PRINTSCREEN(F("STATVDVCC"));
          break;
        default:
          PRINTSCREEN(F("CYCLE ALL"));
          userGroup1Cycle = true;
          statusRegsRead = false; // this will make sure we read all STATUS regs once we enter the cycle the first time
          userSelGroup1 = USER_SEL_GRP1_CYCLE_COUNT - 1; // this will make sure we start with STATUS
          break;
      }
      break;
    case KEYPAD_SELECT:
      PRINTSCREEN(F("CLEAR"));
      flagClear = true;
      break;
  }
  previousKey = key;
  return (key != KEYPAD_NONE);
}

/** \fn  void processCommands()
\brief  process commands received via serial interface. Those are the m,l commands and the basic DC590B commands
used for DC590B compatibility mode that allows the GUI to communicate with this sketch.
(basic set of DC590 commands to allow I2C communication to the LTC2947)
\param  command  the command to be decoded */
void processCommands(char command)
{
  switch (command)
  {
    case 'm':
      {
        // show measurement menu
        Serial.println();
        Serial.print(F("0:"));
        Serial.println(MEAS_LABEL00);
        Serial.print(F("  "));
        Serial.println(MEAS_LABEL01);
        Serial.print(F("1:"));
        Serial.println(MEAS_LABEL10);
        Serial.print(F("  "));
        Serial.println(MEAS_LABEL11);
        Serial.print(F("2:"));
        Serial.println(MEAS_LABEL20);
        Serial.print(F("  "));
        Serial.println(MEAS_LABEL21);
        Serial.print(F("3:"));
        Serial.println(MEAS_LABEL30);
        Serial.print(F("  "));
        Serial.println(MEAS_LABEL31);
        Serial.print(F("4:"));
        Serial.println(MEAS_LABEL40);
        Serial.print(F("  "));
        Serial.println(MEAS_LABEL41);
        Serial.print(F("5:"));
        Serial.println(MEAS_LABEL50);
        Serial.print(F("  "));
        Serial.println(MEAS_LABEL51);
        Serial.print(F("6:"));
        Serial.println(MEAS_LABEL60);
        Serial.print(F("  "));
        Serial.println(MEAS_LABEL61);
        Serial.println(F("enter 0-6, any other to cancel"));
        char sel = get_char() - 0x30;
        userGroupCycleTimer = 0;
        switch (sel)
        {
          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
            lcd->EnableSerialOutput = true;
            userGroup1Cycle = false;
            userGroup2Cycle = false;
            userSelGroup2 = sel;
            userGroupSelect = true;
            break;
          case 6:
            lcd->EnableSerialOutput = true;
            userGroup1Cycle = false;
            userGroup2Cycle = true;
            userGroupSelect = true;
            break;
          default:
            lcd->EnableSerialOutput = false;
            Serial.println(F("canceled"));
            break;
        }
      }
      break;
    case 'l':
      {
        // show status / alerts menu
        Serial.println();
        Serial.println(F("0:STATUS"));
        Serial.println(F("1:STATVT"));
        Serial.println(F("2:STATIP"));
        Serial.println(F("3:STATC"));
        Serial.println(F("4:STATE"));
        Serial.println(F("5:STATCEOF"));
        Serial.println(F("6:STATTB"));
        Serial.println(F("7:STATVDVCC"));
        Serial.println(F("8:CYCLE ALL"));
        Serial.println(F("enter 0-8, any other to cancel"));
        char sel = get_char() - 0x30;
        userGroupCycleTimer = 0;
        switch (sel)
        {
          case 0:
          case 1:
          case 2:
          case 3:
          case 4:
          case 5:
          case 6:
          case 7:
            lcd->EnableSerialOutput = true;
            userGroup1Cycle = false;
            userGroup2Cycle = false;
            userSelGroup1 = sel;
            userGroupSelect = false;
            break;
          case 8:
            lcd->EnableSerialOutput = true;
            userGroup2Cycle = false;
            userGroupSelect = false;
            userGroup1Cycle = true;
            statusRegsRead = false; // this will make sure we read all STATUS regs once we enter the cycle the first time
            userSelGroup1 = USER_SEL_GRP1_CYCLE_COUNT - 1; // this will make sure we start with STATUS
            break;
          default:
            lcd->EnableSerialOutput = false;
            Serial.println(F("canceled"));
            break;
        }
      }
      break;
  }
}


