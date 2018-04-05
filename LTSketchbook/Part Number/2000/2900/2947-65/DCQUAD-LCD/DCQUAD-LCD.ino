/*!
@verbatim

Linear Technology DCXXXX Demonstration Board.
This board incorporates 4 LTC2947-65 ICs sharing the total load current.
Print the measurement results to an Adafruit LCD shield.
LTC2947-65: LTC2947-65 a high-precision power and energy monitor
with an internal sense resistor supporting up to 65A.

The LTC2947-65 is a high-precision power and energy
monitor with an internal sense resistor supporting up
to 65A. Three internal No Latency delta sigma ADCs ensure
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

http://www.linear.com/product/LTC2947-65
http://www.linear.com/product/LTC2947-65#demoboards

NOTES
Setup:
Set the terminal baud rate to 115200 and select the newline terminator.

USER INPUT DATA FORMAT:
decimal : 1024
hex     : 0x400
octal   : 02000  (leading 0 "zero")
binary  : B10000000000
float   : 1024.0

See the DC2334 demo manual and LTC2947-65 datasheet  for  more  details on
operation.

@endverbatim


REVISION HISTORY
$Revision: 5954 $
$Date: 2018-01-20 13:05:26 -0600 (Thu, 20 Oct 2016) $

Copyright (c) 2018, Linear Technology Corp.(LTC)
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

/*! @file
@ingroup LTC2947_65
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LT_I2C.h"
#include "UserInterface.h"
#include "QuikEval_EEPROM.h"
#include "LTC2947-65.h"
#include "LTC2947.h"
#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// For the Adafruit shield, these are the default.
#define TFT_DC 9
#define TFT_CS 10

// LCD Screen related items:
#define TFT_TEXT_Q11_X 10
#define TFT_TEXT_Q11_Y 20
#define TFT_TEXT_Q12_X 100
#define TFT_TEXT_Q12_Y 80
#define TFT_TEXT_Q21_X 10
#define TFT_TEXT_Q21_Y 140
#define TFT_TEXT_Q22_X 100
#define TFT_TEXT_Q22_Y 200

// Use hardware SPI (on Uno, #13, #12, #11) and the above for CS/DC
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

//! Global variables
static uint8_t demo_board_connected;                //!< Set to 1 if the board is connected
static uint8_t LTC2947_slv1_addr = 0x5c;
static uint8_t LTC2947_slv2_addr = 0x5d;
static uint8_t LTC2947_slv3_addr = 0x5e;
static uint8_t LTC2947_slv4_addr = 0x64;
static uint8_t LTC2947_slv5_addr = 0x65;
static uint8_t LTC2947_slv6_addr = 0x66;

static int foo = 0;

// uncalibrated LTC2947-65 parts require a current scale factor
#define LTC2947_calibration_scale 2.29

// number of LTC2947 slaves on the bus
#define LTC2947_slv_count 4

static uint8_t LTC2947_slv_addrs[LTC2947_slv_count]; //! a list of slave addresses (max of 6 configurable)

static float LTC2947_data[LTC2947_slv_count][5]; //! an array to carry current, power, voltage, temp, vcc data from the LTC2947
static float LTC2947_old_data[LTC2947_slv_count][5]; //! older data

//! Initialize Linduino
void setup()
{
  int i; // iteration var
//  ######CHANGE THE DEMO BOARD NUMBER BELOW
  char demo_name[] = "DC0000";  //! Demo Board Name stored in QuikEval EEPROM
  //  spi_enable(SPI_CLOCK_DIV16);  //! Configure the spi port for 1MHz SCK
  //  quikeval_SPI_connect();       //! Connect SPI to main data port (in case the demo board is
  //! configured to SPI mode this avoids pin conflicts when
  //! reading the EEPROM with discover_demo_board)
  quikeval_I2C_init();          //! Configure the EEPROM I2C port for 100kHz
  Serial.begin(115200);         //! Initialize the serial port to the PC
  //  print_title();          //! print title
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
  LTC2947_slv_addrs[0] = LTC2947_slv1_addr;
  LTC2947_slv_addrs[1] = LTC2947_slv2_addr;
  LTC2947_slv_addrs[2] = LTC2947_slv3_addr;
  LTC2947_slv_addrs[3] = LTC2947_slv4_addr;
  setup_ser_mode();   //! setup LTC2947-65's serial interface mode

  for(i = 0; i < LTC2947_slv_count; i++) {
    // Because there are 4 LTC2947-65s on the bus we have to abuse the I2C accesses a bit.
    // The LTC2947 library functions assume only one LTC2947 IC on the bus.
    LTC2947_InitI2C(LTC2947_slv_addrs[i]);   //! set LTC2947 library to I2C mode, address IC i

    // reset LTC2947
    //    LTC2947_WR_BYTE(LTC2947_REG_OPCTL, LTC2947_BM_OPCTL_RST);
    //    delay(120); //! give device some time to restart
    //    go_idle_cfg_tbctl();  //! go to idle and configure TBCTL

    // shutdown
    //    LTC2947_WR_BYTE(LTC2947_REG_OPCTL, LTC2947_BM_OPCTL_SHDN);

    // wake up
    LTC2947_wake_up();
    
    // sample continuously
    LTC2947_WR_BYTE(LTC2947_REG_OPCTL, LTC2947_BM_OPCTL_CONT);

    // init GPIO pins
    LTC2947_GPIO_PinMode(OUTPUT);
    switch (i)
      {
      case 0:
	LTC2947_GPIO_SetPinState(LOW);
	break;
      case 1:
	LTC2947_GPIO_SetPinState(HIGH);
	break;
      case 2:
	LTC2947_GPIO_SetPinState(LOW);
	break;
      case 3:
	LTC2947_GPIO_SetPinState(HIGH);
	break;
      case 4:
	LTC2947_GPIO_SetPinState(LOW);
	break;
      case 5:
	LTC2947_GPIO_SetPinState(HIGH);
	break;
      default:
	LTC2947_GPIO_SetPinState(HIGH);
	break;
      }
  }
  // start the LCD display
  tft.begin();
  tft.setRotation(3);
  tft.setTextSize(4);
  //  tft.setFont(&Anonymous_Pro_B14pt7b);
  tft.fillScreen(ILI9341_BLACK);
  tft.fillRoundRect((TFT_TEXT_Q11_X-8),(TFT_TEXT_Q11_Y-8),220,40,10,ILI9341_NAVY);
  tft.fillRoundRect((TFT_TEXT_Q21_X-8),(TFT_TEXT_Q21_Y-8),220,40,10,ILI9341_NAVY);
  tft.fillRoundRect((TFT_TEXT_Q12_X-8),(TFT_TEXT_Q12_Y-8),220,40,10,ILI9341_NAVY);
  tft.fillRoundRect((TFT_TEXT_Q22_X-8),(TFT_TEXT_Q22_Y-8),220,40,10,ILI9341_NAVY);

}

//! Repeats Linduino loop
void loop()
{
  
  // grab samples from 4 IC and display on the LCD
  mm3_measurements();
  print_currents();
  
  // toggle GPIO pins to show activity
  ltc2947_toggle_gpio();
  
  delay(1000);

  // sample continuously
  LTC2947_WR_BYTE(LTC2947_REG_OPCTL, LTC2947_BM_OPCTL_CONT);
}

//! Print LTC2947 currents to the LCD display
void print_currents()
{
  // device 1
  tft.setCursor(TFT_TEXT_Q11_X,TFT_TEXT_Q11_Y);
  tft.setTextColor(ILI9341_NAVY);
  tft.print("I1=");
  tft.println((LTC2947_calibration_scale*LTC2947_old_data[0][0]), 3);
  tft.setCursor(TFT_TEXT_Q11_X,TFT_TEXT_Q11_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("I1=");
  tft.println((LTC2947_calibration_scale*LTC2947_data[0][0]), 3);

  // device 2
  tft.setCursor(TFT_TEXT_Q12_X,TFT_TEXT_Q12_Y);
  tft.setTextColor(ILI9341_NAVY);
  tft.print("I2=");
  tft.println((LTC2947_calibration_scale*LTC2947_old_data[1][0]), 3);
  tft.setCursor(TFT_TEXT_Q12_X,TFT_TEXT_Q12_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("I2=");
  tft.println((LTC2947_calibration_scale*LTC2947_data[1][0]), 3);

  // device 3
  tft.setCursor(TFT_TEXT_Q21_X,TFT_TEXT_Q21_Y);
  tft.setTextColor(ILI9341_NAVY);
  tft.print("I3=");
  tft.println((LTC2947_calibration_scale*LTC2947_old_data[2][0]), 3);
  tft.setCursor(TFT_TEXT_Q21_X,TFT_TEXT_Q21_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("I3=");
  tft.println((LTC2947_calibration_scale*LTC2947_data[2][0]), 3);

  // device 4
  tft.setCursor(TFT_TEXT_Q22_X,TFT_TEXT_Q22_Y);
  tft.setTextColor(ILI9341_NAVY);
  tft.print("I4=");
  tft.println((LTC2947_calibration_scale*LTC2947_old_data[3][0]), 3);
  tft.setCursor(TFT_TEXT_Q22_X,TFT_TEXT_Q22_Y);
  tft.setTextColor(ILI9341_WHITE);
  tft.print("I4=");
  tft.println((LTC2947_calibration_scale*LTC2947_data[3][0]), 3);

  delay(1000);

  
}

//! Read measurement results
void mm3_measurements()
{
  int i;
  
  //  float current, power, voltage, temp, vcc;

  for(i = 0; i < LTC2947_slv_count; i++) {
    Serial.print(F("\nLTC2947-65 number "));
    Serial.println((i+1), 1);
    LTC2947_InitI2C(LTC2947_slv_addrs[i]);   //! address IC i
    LTC2947_SetPageSelect(false); //! make sure we are on page 0
    LTC2947_old_data[i][0] = LTC2947_data[i][0];
    LTC2947_old_data[i][1] = LTC2947_data[i][1];
    LTC2947_old_data[i][2] = LTC2947_data[i][2];
    LTC2947_old_data[i][3] = LTC2947_data[i][3];
    LTC2947_old_data[i][4] = LTC2947_data[i][4];
    LTC2947_Read_I_P_V_TEMP_VCC(&LTC2947_data[i][0], &LTC2947_data[i][1], &LTC2947_data[i][2], &LTC2947_data[i][3], &LTC2947_data[i][4]);

    Serial.print(F("Current(A): "));
    Serial.println(LTC2947_data[i][0], 3);
    
    Serial.print(F("Power(W)  : "));
    Serial.println(LTC2947_data[i][1], 2);
    
    Serial.print(F("Voltage(V): "));
    Serial.println(LTC2947_data[i][2], 3);
    
    Serial.print(F("Temp.(C)  : "));
    Serial.println(LTC2947_data[i][3], 1);
    
    Serial.print(F("Supply(V) : "));
    Serial.println(LTC2947_data[i][4], 2);
    
    Serial.println();
  }
}

//! Read accumulated measurement results
void mm4_accu_measurements()
{
  double C, E, TB;
  boolean accuSet1 = true; //! start with accu set 1
  int i;
  
  Serial.println(F("\nAccumulated measurements"));
  for(i = 0; i < LTC2947_slv_count; i++) {
    LTC2947_InitI2C(LTC2947_slv_addrs[i]);   //! address IC i
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
}

//! Initialize accumulators with user defined values
void mm5_accu_init()
{
  int i;
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

  for(i = 0; i < LTC2947_slv_count; i++) {
    LTC2947_InitI2C(LTC2947_slv_addrs[i]);   //! address IC i
    LTC2947_SetPageSelect(false); //! make sure we are on page 0
    LTC2947_WR_BYTES(accu_base_addr, 16, bytes); //! write new values to the device
  }

  //! readback values from the device
  Serial.println(F("\nRead back of both accu sets from device:"));
  mm4_accu_measurements();
}

//! Read tracking (min/max) measurement values
void mm6_min_max()
{
  int i;
  float min, max;
  byte bytes[8];
  Serial.println(F("\nMIN/MAX measurements"));

  for(i = 0; i < LTC2947_slv_count; i++) {
    Serial.print(F("\nLTC2947-65 number "));
    Serial.println((i+1), 1);
    LTC2947_InitI2C(LTC2947_slv_addrs[i]);   //! address IC i
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
}

//! LTC2947's GPIO control menu
void mm7_gpio_ctrl()
{
  int i, n;
  int32_t opt;

  Serial.println(F("\n\nGPIO control"));
  Serial.print(F("  Enter IC number (1 - 4): "));
  while (!Serial.available()); //! wait for user input
  n = read_int();    //! Read user input command
  Serial.println(n);
  LTC2947_InitI2C(LTC2947_slv_addrs[(n-1)]);   //! address IC n

  Serial.println(F("\n\nGPIO control"));
  Serial.println(F(" 1. Disable output driver"));
  Serial.println(F(" 2. Enable output driver"));
  Serial.println(F(" 3. Set GPO"));
  Serial.println(F(" 4. Clr GPO"));
  Serial.println(F(" 5. Read GPIO pin state"));
  Serial.print(F("Enter: "));

  while (!Serial.available()); //! wait for user input
  opt = read_int();    //! Read user input command
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


//! Setup LTC2947's serial interface mode
// NOTE: this demo board does not support a SPI interface, so we use the I2C in all cases
void setup_ser_mode()
{
  do
  {
    int32_t i = 1;

    if (i == 0)
    {
      Serial.println(F("Will use DEFAULT (I2C)"));
      i = 1;
    }
    if (i == 1)
    {
      do
      {
	LTC2947_InitI2C(LTC2947_slv_addrs[0]);   //! set LTC2947 library to I2C mode
	quikeval_I2C_init();    //! Configure I2C port for 100kHz
	quikeval_I2C_connect(); //! Connect I2C to main data port
	return;
      }
      while (true);
    }
    else
      {
	Serial.println(F("Please enter 1 or 2!"));
      }
  }
  while (true);
}


void ltc2947_toggle_gpio()
{
  int i, x;
  for(i = 0; i < LTC2947_slv_count; i++)
    {
      LTC2947_InitI2C(LTC2947_slv_addrs[i]);   //! address IC number n
      LTC2947_GPIO_PinMode(OUTPUT);
      // toggle GPIO pin state
      switch (i)
	{
	case 0:
	  LTC2947_GPIO_SetPinState((foo)
			       ? LOW 
			       : HIGH);
	  break;
	case 1:
	  LTC2947_GPIO_SetPinState((foo)
			       ? HIGH
			       : LOW);
	  break;
	case 2:
	  LTC2947_GPIO_SetPinState((foo)
			       ? LOW 
			       : HIGH);
	  break;
	case 3:
	  LTC2947_GPIO_SetPinState((foo)
			       ? HIGH
			       : LOW);
	  break;
	case 4:
	  LTC2947_GPIO_SetPinState((foo)
			       ? LOW 
			       : HIGH);
	  break;
	case 5:
	  LTC2947_GPIO_SetPinState((foo)
			       ? HIGH
			       : LOW);
	  break;
	default:
	  LTC2947_GPIO_SetPinState((foo)
			       ? HIGH
			       : LOW);
	  break;
	}
    }
  foo = (foo) ? 0 : 1;
}
