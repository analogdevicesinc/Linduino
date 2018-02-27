/*!
@verbatim

Linear Technology DC2334A Demonstration Board.
LTC2947: LTC2947 a high-precision power and energy monitor
with an internal sense resistor supporting up to 30A.

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

http://www.linear.com/product/LTC2947
http://www.linear.com/product/LTC2947#demoboards

NOTES
Setup:
Set the terminal baud rate to 115200 and select the newline terminator.

USER INPUT DATA FORMAT:
decimal : 1024
hex     : 0x400
octal   : 02000  (leading 0 "zero")
binary  : B10000000000
float   : 1024.0

See the DC2334 demo manual and LTC2947 datasheet  for  more  details on
operation.

@endverbatim



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
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LT_I2C.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC2947.h"
#include <SPI.h>
#include <LiquidCrystal.h>

//! Global variables
static uint8_t demo_board_connected;                //!< Set to 1 if the board is connected

//! Initialize Linduino
void setup()
{
  char demo_name[] = "DC2334";  //! Demo Board Name stored in QuikEval EEPROM
  spi_enable(SPI_CLOCK_DIV16);  //! Configure the spi port for 1MHz SCK
  quikeval_SPI_connect();       //! Connect SPI to main data port (in case the demo board is
  //! configured to SPI mode this avoids pin conflicts when
  //! reading the EEPROM with discover_demo_board)
  quikeval_I2C_init();          //! Configure the EEPROM I2C port for 100kHz
  Serial.begin(115200);         //! Initialize the serial port to the PC
  print_title();          //! print title
  demo_board_connected = discover_demo_board(demo_name); //! read demo board's EEPROM
  if (demo_board_connected)
  {
    Serial.print(F("Found correct demo board"));
  }
  else
  {
    Serial.println(F("Will attempt to proceed"));
    demo_board_connected = 1;
  }
  setup_ser_mode();   //! setup LTC2947's serial interface mode
  go_idle_cfg_tbctl();  //! go to idle and configure TBCTL
  print_main_menu();    //! show main menu
}

//! Repeats Linduino loop
void loop()
{
  if (Serial.available()) //! Do nothing if serial is not available
  {
    int32_t command = read_int(); //! get user input
    Serial.println(command);      //! echo selection
    Serial.println();
    switch (command)
    {
      case 1:
        mm1_operation_control();
        break;
      case 2:
        mm2_read_status_alert_registers();
        break;
      case 3:
        mm3_measurements();
        break;
      case 4:
        mm4_accu_measurements();
        break;
      case 5:
        mm5_accu_init();
        break;
      case 6:
        mm6_min_max();
        break;
      case 7:
        mm7_gpio_ctrl();
        break;
      case 8:
        mm8_ara();
        break;
#ifdef LTC2947_DEBUG
      case 99:
        LTC2947_DoubleToBytes_Test();
        break;
#endif
      default:
        Serial.println(F("Unknown command!"));
        break;
    }
    print_main_menu();
  }
}

//! Print the title block
void print_title()
{
  Serial.println();
  Serial.println(F("*****************************************************************"));
  Serial.println(F("* DC2334A Demonstration Program                                 *"));
  Serial.println(F("*                                                               *"));
  Serial.println(F("* This program demonstrates the basic features of the LTC2947   *"));
  Serial.println(F("* a high-precision power and energy monitor with an internal    *"));
  Serial.println(F("* sense resistor supporting up to 30A.                          *"));
  Serial.println(F("* Set the baud rate to 115200 and select the newline terminator.*"));
  Serial.println(F("*****************************************************************"));
}

//! Print the main menu
void print_main_menu()
{
  Serial.println(F("\n\nMain Menu"));
  Serial.println(F("  1. Operation control menu"));
  Serial.println(F("  2. Read Status and Alert Registers"));
  Serial.println(F("  3. Read measurements (I,P,V,TEMP,VCC)"));
  Serial.println(F("  4. Read accumulated measurements (C,E,TB)"));
  Serial.println(F("  5. Init accumulated measurements (C,E,TB)"));
  Serial.println(F("  6. Read min/max values of I,V,P,TEMP"));
  Serial.println(F("  7. GPIO control"));
  if (!LTC2947_SPI_Mode_Enabled)
    Serial.println(F("  8. Send ARA"));

#ifdef LTC2947_DEBUG
  Serial.println(F("  99. LTC2947_DoubleToBytes_Test"));
#endif
  Serial.print(F("Enter: "));
}

//! LTC2947's operation control menu
void mm1_operation_control()
{
  Serial.println(F("\n\nOperation control"));
  Serial.println(F(" 1. Continuous measurement"));
  Serial.println(F(" 2. Single measurement"));
  Serial.println(F(" 3. Clear measurement results"));
  Serial.println(F(" 4. Stop measurement"));
  Serial.println(F(" 5. Shutdown"));
  Serial.println(F(" 6. Wakeup"));
  Serial.println(F(" 7. Read current operation mode"));
  Serial.println(F(" 8. RESET"));
  Serial.println(F(" Any other number to cancel"));
  Serial.print(F("Enter: "));

  while (!Serial.available()); //! wait for user input
  int32_t opt = read_int();    //! Read user input command
  Serial.println(opt);         //! echo user input
  switch (opt)
  {
    case 1: //! Continuous measurement
      LTC2947_WR_BYTE(LTC2947_REG_OPCTL, LTC2947_BM_OPCTL_CONT);
      break;
    case 2: //! Single measurement
      LTC2947_WR_BYTE(LTC2947_REG_OPCTL, LTC2947_BM_OPCTL_SSHOT);
      break;
    case 3: //! Clear measurement results
      {
        byte data[1];
        LTC2947_RD_BYTE(LTC2947_REG_OPCTL, data); //! store current operation mode
        LTC2947_WR_BYTE(LTC2947_REG_OPCTL,
                        LTC2947_BM_OPCTL_CLR |        //! set clear bit
                        (data[0] & LTC2947_BM_OPCTL_CONT) //! restore last operation mode (idle or cont. meas.)
                       );
      }
      break;
    case 4: //! Stop measurement
      LTC2947_WR_BYTE(LTC2947_REG_OPCTL, 0);
      break;
    case 5: //! Shutdown
      LTC2947_WR_BYTE(LTC2947_REG_OPCTL, LTC2947_BM_OPCTL_SHDN);
      break;
    case 6: //! Wakeup
      {
        int16_t wakeupTime = LTC2947_wake_up();
        if (wakeupTime < 0)
          Serial.println(F("Failed to wakeup LTC2947 due to timeout"));
        else
        {
          Serial.print(F("LTC2947 woke up after "));
          Serial.print(wakeupTime);
          Serial.println(F(" milliseconds"));
        }
      }
      break;
    case 7: //! Read current operation mode
      {
        byte data[1];
        LTC2947_RD_BYTE(LTC2947_REG_OPCTL, data);
        if (bitMaskSetChk(data[0], LTC2947_BM_OPCTL_CONT))
          Serial.println(F("Continuous measurement enabled"));
        else if (bitMaskSetChk(data[0], LTC2947_BM_OPCTL_SHDN))
          Serial.println(F("LTC2947 was in shutdown mode and should\nhave been woken up by this read operation"));
        else
          Serial.println(F("Idle mode"));
      }
      break;
    case 8: //! make a reset of LTC2947
      LTC2947_WR_BYTE(LTC2947_REG_OPCTL, LTC2947_BM_OPCTL_RST);
      delay(120); //! give device some time to restart
      go_idle_cfg_tbctl(); //! re-configure TBCTL
      break;
    default:
      Serial.println(F("Unsupported command, canceled!"));
      break;
  }
}

//! Read and print all status / alert registers
void mm2_read_status_alert_registers()
{
  Serial.println(F("\nSTATUS / ALERT registers"));
  uint8_t values[8];
  LTC2947_RD_BYTES(LTC2947_REG_STATUS, 8, values);
  for (uint8_t i = 0; i < 8; i++)
    parse_status_alert_regs(values, i);
}

//! Read measurement results
void mm3_measurements()
{
  Serial.println(F("\nMeasurements"));
  float current, power, voltage, temp, vcc;

  LTC2947_SetPageSelect(false); //! make sure we are on page 0
  LTC2947_Read_I_P_V_TEMP_VCC(&current, &power, &voltage, &temp, &vcc);

  Serial.print(F("Current(A): "));
  Serial.println(current, 3);

  Serial.print(F("Power(W)  : "));
  Serial.println(power, 2);

  Serial.print(F("Voltage(V): "));
  Serial.println(voltage, 3);

  Serial.print(F("Temp.(C)  : "));
  Serial.println(temp, 1);

  Serial.print(F("Supply(V) : "));
  Serial.println(vcc, 2);

  Serial.println();
}

//! Read accumulated measurement results
void mm4_accu_measurements()
{
  Serial.println(F("\nAccumulated measurements"));
  double C, E, TB;
  boolean accuSet1 = true; //! start with accu set 1
  LTC2947_SetPageSelect(false); //! make sure we are on page 0
  do
  {
    LTC2947_Read_C_E_TB(accuSet1, &C, &E, &TB); //! read values from device
    Serial.print(F("\nAccu set "));
    Serial.println(accuSet1 ? 1 : 2);

    Serial.print(F("Charge(As): "));
    Serial.println(C, 4); // smallest increment is roughly 3 mA * 100 ms = 0.0003 As

    Serial.print(F("Energy(Ws): "));
    Serial.println(E, 3); // smallest increment is roughly 50 mW * 100 ms = 0.005 Ws

    Serial.print(F("Time(s)   : "));
    Serial.println(TB, 1); // smallest increment is roughly 100 ms = 0.1 s

    if (!accuSet1)
      break; //! DONE!
    accuSet1 = false;
  }
  while (true);
}

//! Initialize accumulators with user defined values
void mm5_accu_init()
{
  boolean accuSet1;
  byte bytes[16];   //! 16 bytes to store charge, energy and time
  uint8_t sel;
  boolean lsbs;
  double cetb;
  uint8_t accu_base_addr;

  //! Which ACCUs? (C1,E1,TB1 or C2,E2,TB2)
  Serial.println(F("\nInit Accumulator set 1 or 2?"));
  Serial.print(F("Enter: "));
  while (!Serial.available());
  sel = read_int();
  accuSet1 = (sel == 1);
  if (accuSet1)
    Serial.println(F("1 (C1,E1,TB1)"));
  else if (sel == 2)
    Serial.println(F("2 (C2,E2,TB2)"));
  else
    Serial.println(F("Unknown selection, will use C2,E2,TB2"));

  //! assign base register address of the accu set
  accu_base_addr = accuSet1 ? LTC2947_VAL_C1 : LTC2947_VAL_C2;

  //! Set accus in LSBs or in real values?
  Serial.println(F("\nInput LSBs or real values in As/Ws/s?"));
  Serial.println(F(" 1. LSBs"));
  Serial.println(F(" 2. Real Values in As/Ws/s"));
  Serial.print(F("Enter: "));
  while (!Serial.available());
  sel = read_int();
  lsbs = (sel == 1);
  if (lsbs)
    Serial.println(F("1 (LSBs)"));
  else if (sel == 2)
    Serial.println(F("2 (As/Ws/s)"));
  else
    Serial.println(F("Unknown selection, will use 1 (LSBs)"));

  //! Input new charge value
  Serial.println();
  Serial.print(accuSet1 ? F("C1: ") : F("C2: "));
  while (!Serial.available());
  if (lsbs)
    //! convert integer input to floating point value
    //! Note: actually this is not necessary as we have to
    //!       write integer values to the device anyway. So
    //!       we could convert this int value directly to
    //!       a bytes array and write it to the device instead of first going to float
    //!       and then going back to an integer representation
    //!       in form of a byte array. Still the focus of the code below
    //!       is to show the usage of the function LTC2947_DoubleToBytes
    cetb = read_int() * LTC2947_LSB_C1;
  else
    cetb = read_float();
  Serial.print(cetb, 8);
  Serial.println(F(" As"));
  LTC2947_DoubleToBytes(cetb, LTC2947_LSB_C1, bytes, 6); //! convert to bytes

  //! Input new energy value
  Serial.println();
  Serial.print(accuSet1 ? F("E1: ") : F("E2: "));
  while (!Serial.available());
  if (lsbs)
    cetb = read_int() * LTC2947_LSB_E1;
  else
    cetb = read_float();
  Serial.print(cetb, 8);
  Serial.println(F(" Ws"));
  LTC2947_DoubleToBytes(cetb, LTC2947_LSB_E1, bytes + 6, 6); //! convert to bytes

  //! Input new time value
  Serial.println();
  Serial.print(accuSet1 ? F("TB1: ") : F("TB2: "));
  while (!Serial.available());
  if (lsbs)
    cetb = read_int() * LTC2947_LSB_TB1;
  else
    cetb = read_float();
  Serial.print(cetb, 8);
  Serial.println(F(" s"));
  LTC2947_DoubleToBytes(cetb, LTC2947_LSB_TB1, bytes + 6 + 6, 4); //! convert to bytes

  Serial.print(F("\nC,E,TB will be written with following values starting at address 0x"));
  LTC2947_SerialPrint8hex(accu_base_addr);
  Serial.println();

  //! report the whole byte array
  for (int8_t i = 0; i < 16; i++)
  {
    Serial.print(i);
    Serial.print(F(":0x"));
    LTC2947_SerialPrint8hex(bytes[i]);
    if (i == 0)
      Serial.print(F(" (MSB)"));
    else if (i == 15)
      Serial.print(F(" (LSB)"));
    Serial.println();
  }

  LTC2947_SetPageSelect(false); //! make sure we are on page 0
  LTC2947_WR_BYTES(accu_base_addr, 16, bytes); //! write new values to the device

  //! readback values from the device
  Serial.println(F("\nRead back of both accu sets from device:"));
  mm4_accu_measurements();
}

//! Read tracking (min/max) measurement values
void mm6_min_max()
{
  float min, max;
  byte bytes[8];
  Serial.println(F("\nMIN/MAX measurements"));

  //! read current/power max/min from device
  LTC2947_RD_BYTES(LTC2947_VAL_IMAX, 8, bytes);

  //! current max/min
  //! LSB is given in mA, *1e-3 gives A. Note also the different LSB size of
  //! LTC2947_LSB_IMAX (or LTC2947_LSB_IMIN which is the same) compared to LTC2947_LSB_I!
  Serial.print(F("Current(A) "));
  max = LTC2947_2BytesToInt16(bytes + 0) * LTC2947_LSB_IMAX * 1e-3;
  min = LTC2947_2BytesToInt16(bytes + 2) * LTC2947_LSB_IMIN * 1e-3;
  print_min_max(min, max, 3, bytes + 2, bytes + 0);

  Serial.print(F("Power(W)   "));
  max = LTC2947_2BytesToInt16(bytes + 4) * LTC2947_LSB_PMAX * 1e-3;
  min = LTC2947_2BytesToInt16(bytes + 6) * LTC2947_LSB_PMIN * 1e-3;
  print_min_max(min, max, 3, bytes + 6, bytes + 4);

  //! read voltage/temperature max/min from device
  LTC2947_RD_BYTES(LTC2947_VAL_VMAX, 8, bytes);

  Serial.print(F("Voltage(V) "));
  max = LTC2947_2BytesToInt16(bytes + 0) * LTC2947_LSB_VMAX * 1e-3;
  min = LTC2947_2BytesToInt16(bytes + 2) * LTC2947_LSB_VMIN * 1e-3;
  print_min_max(min, max, 3, bytes + 2, bytes + 0);

  Serial.print(F("Temp.(C)   "));
  max = LTC2947_2BytesToInt16(bytes + 4) * LTC2947_LSB_TEMPMAX + LTC2947_OFFS_TEMPMAX;
  min = LTC2947_2BytesToInt16(bytes + 6) * LTC2947_LSB_TEMPMIN + LTC2947_OFFS_TEMPMIN;
  print_min_max(min, max, 2, bytes + 6, bytes + 4);
}

//! LTC2947's GPIO control menu
void mm7_gpio_ctrl()
{
  Serial.println(F("\n\nGPIO control"));
  Serial.println(F(" 1. Disable output driver"));
  Serial.println(F(" 2. Enable output driver"));
  Serial.println(F(" 3. Set GPO"));
  Serial.println(F(" 4. Clr GPO"));
  Serial.println(F(" 5. Read GPIO pin state"));
  Serial.print(F("Enter: "));

  while (!Serial.available()); //! wait for user input
  int32_t opt = read_int();    //! Read user input command
  Serial.println(opt);         //! echo user input
  switch (opt)
  {
    case 1:
    case 2:
      Serial.print(F("GPIO output driver "));
      LTC2947_GPIO_PinMode((opt == 1)
                           ? INPUT
                           : OUTPUT);
      Serial.println((opt == 1)
                     ? F("disabled")
                     : F("enabled")
                    );
      break;
    case 3:
    case 4:
      Serial.print(F("GPIO pin state set "));
      LTC2947_GPIO_SetPinState((opt == 3)
                               ? HIGH
                               : LOW);
      Serial.println((opt == 3)
                     ? F("high")
                     : F("low")
                    );
      break;
    case 5:
      Serial.print(F("GPIO pin "));
      Serial.println(LTC2947_GPIO_Read()
                     ? F("high")
                     : F("low")
                    );
      break;
    default:
      Serial.println(F("Unsupported command, canceled!"));
      break;
  }
}

//! SMBus Alert Response Protocol
void mm8_ara()
{
  uint8_t slvAddr;
  uint8_t result;
  Serial.print(F("\nSend ARA, slave response:0x"));
  result = LTC2947_Ara(&slvAddr);
  LTC2947_SerialPrint8hex(slvAddr);
  switch (result)
  {
    case LTC2947_ARA_ERROR:
      Serial.println(F(" (ERROR: I2C communication failed)"));
      break;
    case LTC2947_ARA_LTC2947_RESPONSE:
      Serial.println(F(" (LTC2947 response)"));
      break;
    case LTC2947_ARA_OTHER_RESPONSE:
      Serial.println(F(" (other slave response)"));
      break;
    case LTC2947_ARA_NO_RESPONSE:
      Serial.println(F(" (no slave response)"));
      break;
    case LTC2947_ARA_RESPONSE_WO_WR:
      if (slvAddr == LTC2947_I2C_Slave_Addr)
        Serial.println(F(" (ERROR: LTC2947 response without WR bit)"));
      else
        Serial.println(F(" (ERROR: other slave response without WR bit)"));
      break;
  }
}

//! prints two (min/max) values with label including the 16 bit raw values
void print_min_max(float min, float max, uint8_t digits, byte *minBytes, byte *maxBytes)
{
  Serial.print(F("MIN: "));
  if (min >= 0)
    Serial.print('+');
  Serial.print(min, digits); //! print float value
  Serial.print(F(" (0x"));
  //! print integer value
  LTC2947_SerialPrint16hex(LTC2947_2BytesToInt16(minBytes));
  Serial.print(F(") / MAX: "));
  if (max >= 0)
    Serial.print('+');
  Serial.print(max, digits); //! print float value
  Serial.print(F(" (0x"));
  //! print integer value
  LTC2947_SerialPrint16hex(LTC2947_2BytesToInt16(maxBytes));
  Serial.println(')');
}

//! put LTC2947 into idle mode and configure TBCTL
void go_idle_cfg_tbctl()
{
  byte data[1];
  byte tbctl;
  LTC2947_RD_BYTE(LTC2947_REG_OPCTL, data); //! check if we are in idle mode
  if (data[0] != 0) //! not in idle mode
  {
    Serial.println(F("Will set LTC2947 to IDLE mode now!"));
    LTC2947_WR_BYTE(LTC2947_REG_OPCTL, 0);//! go to idle mode
    delay(200); //! after 2 system tickes (2x100ms) LTC2947 must be in idle mode
    LTC2947_RD_BYTE(LTC2947_REG_OPCTL, data);
  }
  if (data[0] == 0)
  {
    Serial.println(F("LTC2947 in IDLE mode now!"));
  }
  else
  {
    Serial.println(F("Unable to set LTC2947 to IDLE mode!"));
  }

  LTC2947_SetPageSelect(false); //! make sure we are on page 0

  tbctl = //! generate TBCTL configuration value
    LTC2947_PRE << BM2BITPOS(LTC2947_BM_TBCTL_PRE_0) |
    LTC2947_DIV << BM2BITPOS(LTC2947_BM_TBCTL_DIV_0);

  LTC2947_WR_BYTE(LTC2947_REG_TBCTL, tbctl); //! write PRE and DIV setting

  LTC2947_RD_BYTE(LTC2947_REG_TBCTL, data); //! read back configuration

  if (tbctl == data[0]) //! check if everything was written correctly
    Serial.println(F("Successfully configured LTC2947's TBCTL"));
  else
    Serial.println(F("Failed to configured LTC2947's TBCTL"));
}

//! parse status and alert registers. register_num (0..7) selects one of the 8 status/alert registers
//! statusRegs is a 8-byte array that holds the content of the status/alert registers read from the device
void parse_status_alert_regs(uint8_t statusRegs[8], uint8_t register_num)
{
  switch (register_num)
  {
    case 0:
      Serial.println(F("STATUS:"));
      Serial.print(F(" UPDATE:  "));
      Serial.println(bitMaskSetChk(statusRegs[0], LTC2947_BM_STATUS_UPDATE/*  */) ? 1 : 0);
      Serial.print(F(" ADCERR:  "));
      Serial.println(bitMaskSetChk(statusRegs[0], LTC2947_BM_STATUS_ADCERR/*  */) ? 1 : 0);
      Serial.print(F(" TBERR:   "));
      Serial.println(bitMaskSetChk(statusRegs[0], LTC2947_BM_STATUS_TBERR/*   */) ? 1 : 0);
      Serial.print(F(" PORA:    "));
      Serial.println(bitMaskSetChk(statusRegs[0], LTC2947_BM_STATUS_PORA/*    */) ? 1 : 0);
      Serial.print(F(" UVLOA:   "));
      Serial.println(bitMaskSetChk(statusRegs[0], LTC2947_BM_STATUS_UVLOA/*   */) ? 1 : 0);
      Serial.print(F(" UVLOD:   "));
      Serial.println(bitMaskSetChk(statusRegs[0], LTC2947_BM_STATUS_UVLOD/*   */) ? 1 : 0);
      Serial.print(F(" UVLOSTBY:"));
      Serial.println(bitMaskSetChk(statusRegs[0], LTC2947_BM_STATUS_UVLOSTBY/**/) ? 1 : 0);
      break;
    case 1:
      Serial.println(F("STATVT:"));
      Serial.print(F(" FANH:    "));
      Serial.println(bitMaskSetChk(statusRegs[1], LTC2947_BM_STATVT_FANH/*   */) ? 1 : 0);
      Serial.print(F(" FANL:    "));
      Serial.println(bitMaskSetChk(statusRegs[1], LTC2947_BM_STATVT_FANL/*   */) ? 1 : 0);
      Serial.print(F(" TEMPH:   "));
      Serial.println(bitMaskSetChk(statusRegs[1], LTC2947_BM_STATVT_TEMPH/*  */) ? 1 : 0);
      Serial.print(F(" TEMPL:   "));
      Serial.println(bitMaskSetChk(statusRegs[1], LTC2947_BM_STATVT_TEMPL/*  */) ? 1 : 0);
      Serial.print(F(" VH:      "));
      Serial.println(bitMaskSetChk(statusRegs[1], LTC2947_BM_STATVT_VH/*     */) ? 1 : 0);
      Serial.print(F(" VL:      "));
      Serial.println(bitMaskSetChk(statusRegs[1], LTC2947_BM_STATVT_VL/*     */) ? 1 : 0);
      break;
    case 2:
      Serial.println(F("STATIP:"));
      Serial.print(F(" IH:      "));
      Serial.println(bitMaskSetChk(statusRegs[2], LTC2947_BM_STATIP_IH) ? 1 : 0);
      Serial.print(F(" IL:      "));
      Serial.println(bitMaskSetChk(statusRegs[2], LTC2947_BM_STATIP_IL) ? 1 : 0);
      Serial.print(F(" PH:      "));
      Serial.println(bitMaskSetChk(statusRegs[2], LTC2947_BM_STATIP_PH) ? 1 : 0);
      Serial.print(F(" PL:      "));
      Serial.println(bitMaskSetChk(statusRegs[2], LTC2947_BM_STATIP_PL) ? 1 : 0);
      break;
    case 3:
      Serial.println(F("STATC:"));
      Serial.print(F(" C1H      "));
      Serial.println(bitMaskSetChk(statusRegs[3], LTC2947_BM_STATC_C1H) ? 1 : 0);
      Serial.print(F(" C1L      "));
      Serial.println(bitMaskSetChk(statusRegs[3], LTC2947_BM_STATC_C1L) ? 1 : 0);
      Serial.print(F(" C2H      "));
      Serial.println(bitMaskSetChk(statusRegs[3], LTC2947_BM_STATC_C2H) ? 1 : 0);
      Serial.print(F(" C2L      "));
      Serial.println(bitMaskSetChk(statusRegs[3], LTC2947_BM_STATC_C2L) ? 1 : 0);
      break;
    case 4:
      Serial.println(F("STATE:"));
      Serial.print(F(" E1H      "));
      Serial.println(bitMaskSetChk(statusRegs[4], LTC2947_BM_STATE_E1H) ? 1 : 0);
      Serial.print(F(" E1L      "));
      Serial.println(bitMaskSetChk(statusRegs[4], LTC2947_BM_STATE_E1L) ? 1 : 0);
      Serial.print(F(" E2H      "));
      Serial.println(bitMaskSetChk(statusRegs[4], LTC2947_BM_STATE_E2H) ? 1 : 0);
      Serial.print(F(" E2L      "));
      Serial.println(bitMaskSetChk(statusRegs[4], LTC2947_BM_STATE_E2L) ? 1 : 0);
      break;
    case 5:
      Serial.println(F("STATCEOF:"));
      Serial.print(F(" C1OF     "));
      Serial.println(bitMaskSetChk(statusRegs[5], LTC2947_BM_STATCEOF_C1OF) ? 1 : 0);
      Serial.print(F(" C2OF     "));
      Serial.println(bitMaskSetChk(statusRegs[5], LTC2947_BM_STATCEOF_C2OF) ? 1 : 0);
      Serial.print(F(" E1OF     "));
      Serial.println(bitMaskSetChk(statusRegs[5], LTC2947_BM_STATCEOF_E1OF) ? 1 : 0);
      Serial.print(F(" E2OF     "));
      Serial.println(bitMaskSetChk(statusRegs[5], LTC2947_BM_STATCEOF_E2OF) ? 1 : 0);
      break;
    case 6:
      Serial.println(F("STATTB:"));
      Serial.print(F(" TB1OF    "));
      Serial.println(bitMaskSetChk(statusRegs[6], LTC2947_BM_STATTB_TB1OF) ? 1 : 0);
      Serial.print(F(" TB2OF    "));
      Serial.println(bitMaskSetChk(statusRegs[6], LTC2947_BM_STATTB_TB2OF) ? 1 : 0);
      Serial.print(F(" TB1TH    "));
      Serial.println(bitMaskSetChk(statusRegs[6], LTC2947_BM_STATTB_TB1TH) ? 1 : 0);
      Serial.print(F(" TB2TH    "));
      Serial.println(bitMaskSetChk(statusRegs[6], LTC2947_BM_STATTB_TB2TH) ? 1 : 0);
      break;
    case 7:
      Serial.println(F("STATVDVCC:"));
      Serial.print(F(" VDVCCH   "));
      Serial.println(bitMaskSetChk(statusRegs[7], LTC2947_BM_STATVDVCC_VDVCCH) ? 1 : 0);
      Serial.print(F(" VDVCCL   "));
      Serial.println(bitMaskSetChk(statusRegs[7], LTC2947_BM_STATVDVCC_VDVCCL) ? 1 : 0);
      break;
    default:
      Serial.print(register_num);
      Serial.println(F(" is not available!"));
      break;
  }
}

//! Setup LTC2947's serial interface mode
void setup_ser_mode()
{
  do
  {
    Serial.println(F("\n\nSelect serial mode for LTC2947"));
    Serial.println(F("  1. I2C (DEFAULT)"));
    Serial.println(F("  2. SPI"));
    Serial.print(F("Enter: "));
    while (Serial.available())
      ;
    int32_t i = read_int();
    Serial.println(i); //! echo selection
    if (i == 0)
    {
      Serial.println(F("Will use DEFAULT (I2C)"));
      i = 1;
    }
    if (i == 1)
    {
      do
      {
        Serial.println(F("\n\nEnter 7-bit I2C slave address for LTC2947 (DEFAULT:0x5C)"));
        while (Serial.available())
          ;
        uint8_t slv = read_int();
        if (slv == 0)
        {
          Serial.println(F("Will use DEFAULT (0x5C)"));
          slv = 0x5C;
        }
        Serial.print(F("Slave address: 0x"));
        Serial.println(slv, HEX);
        switch (slv)
        {
          case LTC2947_I2C_ADDR_LL:
          case LTC2947_I2C_ADDR_LH:
          case LTC2947_I2C_ADDR_LR:
          case LTC2947_I2C_ADDR_RL:
          case LTC2947_I2C_ADDR_RH:
          case LTC2947_I2C_ADDR_RR:
            LTC2947_InitI2C(slv);   //! set LTC2947 library to I2C mode
            quikeval_I2C_init();    //! Configure I2C port for 100kHz
            quikeval_I2C_connect(); //! Connect I2C to main data port
            return;
          default:
            Serial.println(F("Unsupported slave address, please try again!"));
            break;
        }

      }
      while (true);
    }
    else if (i == 2)
    {
      LTC2947_InitSPI(); //! set LTC2947 library to SPI mode
      spi_enable(SPI_CLOCK_DIV16);  //! Configure the spi port for 1MHz SCK (16MHz / 16)
      quikeval_SPI_connect();       //! Connect SPI to main data port
      return;
    }
    else
    {
      Serial.println(F("Please enter 1 or 2!"));
    }
  }
  while (true);
}
