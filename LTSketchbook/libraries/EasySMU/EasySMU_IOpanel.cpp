/*!
EasySMU_IOpanel Library to Enable Touchscreen Control of EasySMU

[User Guide](http://www.linear.com/docs/58670 "EasySMU User Guide") \n
[Schematic](http://www.linear.com/docs/58671 "Schematic") \n
[Top-Level Linduino Firmware for EasySMU](file:EasySMU__Run_8ino.html "Top-Level Linduino Firmware for EasySMU") \n
[EasySMU Class Reference](file:classEasySMU.html "EasySMU Class Reference") \n
[EasySMU_IOpanel Class Reference](file:classEasySMU__IOpanel.html "EasySMU_IOpanel Class Reference") \n
[EasySMU Webpage](http://www.linear.com/solutions/7943 "EasySMU Webpage") \n

@verbatim

EasySMU: I2C Address Translator Demonstration and a Simple Multi-Channel Source Measurement Unit

LTC4316: Single I2C/SMBus Address Translator
LT1970A: Power Op Amp with Adjustable Precision Current Limit
LT5400:  Quad Matched Resistor Network
LTC2655: Quad I2C 16-/12-Bit Rail-to-Rail DACs with 10ppm/°C Max Reference
LTC3265: Low Noise Dual Supply with Boost and Inverting Charge Pumps
LTC2051: Dual Zero-Drift Operational Amplifier
LT3010:  50mA, 3V to 80V Low Dropout Micropower Linear Regulator
LT1991:  Precision, 100µA Gain Selectable Amplifier
LTC6655: 0.25ppm Noise, Low Drift Precision Reference
LTC2485: 24-Bit ΔΣ ADC with Easy Drive Input Current Cancellation and I2C Interface

EasySMU is a single-channel ±12V/40mA programmable-voltage/programmable-current
source with accurate voltage/current measurement capability. The LTC4316 I2C
Address Translator enables up to eight independent EasySMUs to be controlled
by a single I2C master.

In this demonstration, each EasySMU board contains four I2C slaves and the
associated components to implement a single-channel ±12V/40mA programmable-
voltage/programmable-current source. The LTC4316 translates the I2C addresses
of each EasySMU to a unique set of addresses, enabling up to eight EasySMU
boards to be stacked on a single Linduino (I2C master). In this form, it
resembles a multi-channel automated test system. Alternatively, an optional
touchscreen allows the user to interactively control up to four channels,
forming a compact multi-channel programmable-voltage/programmable-current
bench source for lab testing, powered from a single 12V AC wall adapter.

The primary purpose of the EasySMU is to demonstrate the LTC4316 I2C Address
Translator. The programmable-voltage/programmable-current source and meter
also provide a convenient demonstration of the associated components: LT1970A,
LT5400-3, LTC2655-H, LTC3265, LTC2051, LT3010, LT1991, LTC6655, and LTC2485.
While the EasySMU is not designed to demonstrate the ultimate performance that
can be obtained from each of those components, the EasySMU does provide
impressive results from a reasonably simple circuit.

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
    @ingroup EasySMU
  EasySMU_IOpanel Library to Enable Touchscreen Control of EasySMU
*/

#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include "EasySMU_IOpanel.h"
#include <Anonymous_Pro6pt7b.h>
#include <Anonymous_ProBold14pt7b.h>

#ifdef ADAFRUIT_TFT_TOUCHSHIELD_1947
#include <Adafruit_FT6206.h>
#endif

//#include guard not implemented properly in SeeedTouchScreen.h so it was not included here.
//#ifdef SEEEDSTUDIO_TFT_TOUCHSHIELD_V2
//#include <SeeedTouchScreen.h>
//#endif

void EasySMU_IOpanel::Init()
{
  SMUselected_=_SMU0;
  button_pressed_=_NONE;
  //init Display
  lcd.begin();
  lcd.setRotation(1);

  //init Touch-Controller
#ifdef ADAFRUIT_TFT_TOUCHSHIELD_1947
  if (! touchp.begin(40))    // pass in 'sensitivity' coefficient
  {
    enabled_ = 0;
    Serial.println("No touchscreen present.");
    return;
  }

#endif // ADAFRUIT_TFT_TOUCHSHIELD_1947

  enabled_ = 1;

  lcd.fillScreen(ILI9341_WHITE);

  int16_t lcdWidth, lcdHeight;

  lcdWidth=lcd.width();

  lcdHeight=lcd.height();

  lcd.fillRoundRect(_SMU0_LEFT,  _SMU0_TOP, _SMU_WIDTH, _SMU_HEIGHT, _BUTTON_RADIUS , ILI9341_BLUE);
  lcd.fillRoundRect(_SMU1_LEFT,  _SMU1_TOP, _SMU_WIDTH, _SMU_HEIGHT, _BUTTON_RADIUS , ILI9341_BLUE);
  lcd.fillRoundRect(_SMU2_LEFT,  _SMU2_TOP, _SMU_WIDTH, _SMU_HEIGHT, _BUTTON_RADIUS , ILI9341_BLUE);
  lcd.fillRoundRect(_SMU3_LEFT,  _SMU3_TOP, _SMU_WIDTH, _SMU_HEIGHT, _BUTTON_RADIUS , ILI9341_BLUE);
  lcd.setTextColor(ILI9341_WHITE,ILI9341_BLUE);
  lcd.setFont(&Anonymous_Pro6pt7b);

  DisplaySMULabel();

  lcd.fillRoundRect(_VPLUS_LEFT,  _VPLUS_TOP, _BUTTON_WIDTH, _BUTTON_HEIGHT, _BUTTON_RADIUS , ILI9341_BLUE);
  lcd.fillRoundRect(_VMINUS_LEFT,  _VMINUS_TOP, _BUTTON_WIDTH, _BUTTON_HEIGHT,  _BUTTON_RADIUS , ILI9341_BLUE);
  lcd.fillRoundRect(_IPLUS_LEFT,  _IPLUS_TOP, _BUTTON_WIDTH, _BUTTON_HEIGHT,  _BUTTON_RADIUS , ILI9341_BLUE);
  lcd.fillRoundRect(_IMINUS_LEFT,  _IMINUS_TOP, _BUTTON_WIDTH, _BUTTON_HEIGHT,  _BUTTON_RADIUS , ILI9341_BLUE);
  lcd.fillRoundRect(_VPLUSPLUS_LEFT,  _VPLUSPLUS_TOP, _BUTTON_WIDTH, _BUTTON_HEIGHT,  _BUTTON_RADIUS , ILI9341_BLUE);
  lcd.fillRoundRect(_VMINUSMINUS_LEFT,  _VMINUSMINUS_TOP, _BUTTON_WIDTH, _BUTTON_HEIGHT,  _BUTTON_RADIUS , ILI9341_BLUE);
  lcd.fillRoundRect(_IPLUSPLUS_LEFT,  _IPLUSPLUS_TOP, _BUTTON_WIDTH, _BUTTON_HEIGHT,  _BUTTON_RADIUS , ILI9341_BLUE);
  lcd.fillRoundRect(_IMINUSMINUS_LEFT,  _IMINUSMINUS_TOP, _BUTTON_WIDTH, _BUTTON_HEIGHT,  _BUTTON_RADIUS , ILI9341_BLUE);

  lcd.setTextColor(ILI9341_WHITE,ILI9341_BLUE);
  lcd.setFont(&Anonymous_Pro_B14pt7b);

  lcd.setCursor(_VPLUS_LEFT+_BUTTON_TEXT_LEFT, _VPLUS_TOP+_BUTTON_TEXT_TOP);
  lcd.print("V+");
  lcd.setCursor(_VMINUS_LEFT+_BUTTON_TEXT_LEFT, _VMINUS_TOP+_BUTTON_TEXT_TOP);
  lcd.print("V-");
  lcd.setCursor(_IPLUS_LEFT+_BUTTON_TEXT_LEFT, _IPLUS_TOP+_BUTTON_TEXT_TOP);
  lcd.print("I+");
  lcd.setCursor(_IMINUS_LEFT+_BUTTON_TEXT_LEFT, _IMINUS_TOP+_BUTTON_TEXT_TOP);
  lcd.print("I-");
  lcd.setCursor(_VPLUSPLUS_LEFT+_BUTTON_TEXT_PLUSPLUS_LEFT, _VPLUSPLUS_TOP+_BUTTON_TEXT_TOP);
  lcd.print("V++");
  lcd.setCursor(_VMINUSMINUS_LEFT+_BUTTON_TEXT_PLUSPLUS_LEFT, _VMINUSMINUS_TOP+_BUTTON_TEXT_TOP);
  lcd.print("V--");
  lcd.setCursor(_IPLUSPLUS_LEFT+_BUTTON_TEXT_PLUSPLUS_LEFT, _IPLUSPLUS_TOP+_BUTTON_TEXT_TOP);
  lcd.print("I++");
  lcd.setCursor(_IMINUSMINUS_LEFT+_BUTTON_TEXT_PLUSPLUS_LEFT, _IMINUSMINUS_TOP+_BUTTON_TEXT_TOP);
  lcd.print("I--");

  lcd.setFont(&Anonymous_Pro6pt7b);

}

int16_t EasySMU_IOpanel::CheckButton()
{
  int16_t x, y;

  int16_t previous_button_pressed=button_pressed_;
  button_pressed_=_NONE;
  if (enabled_==0)
  {
    return (button_pressed_);
  }
#ifdef ADAFRUIT_TFT_TOUCHSHIELD_1947
  if (touchp.touched())
#endif // ADAFRUIT_TFT_TOUCHSHIELD_1947

#ifdef SEEEDSTUDIO_TFT_TOUCHSHIELD_V2
    if (touchp.isTouching())
#endif // SEEEDSTUDIO_TFT_TOUCHSHIELD_V2
    {
#ifdef ADAFRUIT_TFT_TOUCHSHIELD_1947
      TS_Point p = touchp.getPoint();
      y = map(p.x, 0, 240, 0, 240);
      x = map(p.y, 0, 320, 320, 0);
#endif // ADAFRUIT_TFT_TOUCHSHIELD_1947

#ifdef SEEEDSTUDIO_TFT_TOUCHSHIELD_V2
      Point p = touchp.getPoint();
      y = map(p.x, 1700, 300, 0, 240);
      x = map(p.y, 1800, 200, 320, 0);
#endif // SEEEDSTUDIO_TFT_TOUCHSHIELD_V2


      /*  Serial.print("pX:");
        Serial.print(p.x);
        Serial.print(" pY:");
        Serial.print(p.y);
        Serial.print(" pZ:");
        Serial.println(p.z);
        Serial.print("X:");
        Serial.print(x);
        Serial.print(" Y:");
        Serial.println(y);*/

      if ((x >= _SMU0_LEFT) && (x <= _SMU0_RIGHT) && (y >= _SMU0_TOP) && (y <= _SMU0_BOTTOM))
      {
        button_pressed_ =_SMU0;
      }
      else if ((x >= _SMU1_LEFT) && (x <= _SMU1_RIGHT) && (y >= _SMU1_TOP) && (y <= _SMU1_BOTTOM))
      {
        button_pressed_ =_SMU1;
      }
      else if ((x >= _SMU2_LEFT) && (x <= _SMU2_RIGHT) && (y >= _SMU2_TOP) && (y <= _SMU2_BOTTOM))
      {
        button_pressed_ = _SMU2;
      }
      else if ((x >= _SMU3_LEFT) && (x <= _SMU3_RIGHT) && (y >= _SMU3_TOP) && (y <= _SMU3_BOTTOM))
      {
        button_pressed_ =_SMU3;
      }
      else if ((x >= _VPLUS_LEFT) && (x <= _VPLUS_RIGHT) && (y >= _VPLUS_TOP) && (y <= _VPLUS_BOTTOM))
      {
        button_pressed_ =_VPLUS;
      }
      else if ((x >= _VMINUS_LEFT) && (x <= _VMINUS_RIGHT) && (y >= _VMINUS_TOP) && (y <= _VMINUS_BOTTOM))
      {
        button_pressed_ =_VMINUS;
      }
      else if ((x >= _IPLUS_LEFT) && (x <= _IPLUS_RIGHT) && (y >= _IPLUS_TOP) && (y <= _IPLUS_BOTTOM))
      {
        button_pressed_ =_IPLUS;
      }
      else if ((x >= _IMINUS_LEFT) && (x <= _IMINUS_RIGHT) && (y >= _IMINUS_TOP) && (y <= _IMINUS_BOTTOM))
      {
        button_pressed_ =_IMINUS;
      }
      else if ((x >= _VPLUSPLUS_LEFT) && (x <= _VPLUSPLUS_RIGHT) && (y >= _VPLUSPLUS_TOP) && (y <= _VPLUSPLUS_BOTTOM))
      {
        button_pressed_ =_VPLUSPLUS;
      }
      else if ((x >= _VMINUSMINUS_LEFT) && (x <= _VMINUSMINUS_RIGHT) && (y >= _VMINUSMINUS_TOP) && (y <= _VMINUSMINUS_BOTTOM))
      {
        button_pressed_ =_VMINUSMINUS;
      }
      else if ((x >= _IPLUSPLUS_LEFT) && (x <= _IPLUSPLUS_RIGHT) && (y >= _IPLUSPLUS_TOP) && (y <= _IPLUSPLUS_BOTTOM))
      {
        button_pressed_ =_IPLUSPLUS;
      }
      else if ((x >= _IMINUSMINUS_LEFT) && (x <= _IMINUSMINUS_RIGHT) && (y >= _IMINUSMINUS_TOP) && (y <= _IMINUSMINUS_BOTTOM))
      {
        button_pressed_ =_IMINUSMINUS;
      }

    }

  if (button_pressed_!=previous_button_pressed) start_button_pressed_=millis();
  duration_button_pressed_=millis()-start_button_pressed_;
  if (button_pressed_>=_SMU0 && button_pressed_<=_SMU3)
  {
    SMUselected_=button_pressed_;
    DisplaySMULabel();
  }
  return button_pressed_;
}

void EasySMU_IOpanel::DisplaySMULabel()
{
  if (enabled_== 0) return;
  lcd.setTextColor((SMUselected_==_SMU0 ? ILI9341_GREEN : ILI9341_WHITE),ILI9341_BLUE);
  lcd.setCursor(_SMU0_LEFT+_SMU_TEXT_LEFT, _SMU0_TOP+_SMU_TEXT_TOP);
  lcd.print("CH0");
  lcd.setTextColor((SMUselected_==_SMU1 ? ILI9341_GREEN : ILI9341_WHITE),ILI9341_BLUE);
  lcd.setCursor(_SMU1_LEFT+_SMU_TEXT_LEFT, _SMU1_TOP+_SMU_TEXT_TOP);
  lcd.print("CH1");
  lcd.setTextColor((SMUselected_==_SMU2 ? ILI9341_GREEN : ILI9341_WHITE),ILI9341_BLUE);
  lcd.setCursor(_SMU2_LEFT+_SMU_TEXT_LEFT, _SMU2_TOP+_SMU_TEXT_TOP);
  lcd.print("CH2");
  lcd.setTextColor((SMUselected_==_SMU3 ? ILI9341_GREEN : ILI9341_WHITE),ILI9341_BLUE);
  lcd.setCursor(_SMU3_LEFT+_SMU_TEXT_LEFT, _SMU3_TOP+_SMU_TEXT_TOP);
  lcd.print("CH3");
  lcd.setTextColor(ILI9341_WHITE,ILI9341_BLUE);
}

void EasySMU_IOpanel::OverwriteOldString(uint16_t fg_color, uint16_t bg_color, char *old_string, const char *new_string)
{
  //lcd.setCursor(x, y);
  if (enabled_== 0) return;
  uint16_t new_x, new_y;
  for (int8_t i=0; (i < 16) && (old_string[i] != '\0') && (new_string[i] != '\0'); i++)
  {

    new_x=lcd.getCursorX();
    new_y=lcd.getCursorY();

    lcd.setTextColor(bg_color,bg_color);
    lcd.print(old_string[i]);

    lcd.setCursor(new_x, new_y);
    lcd.setTextColor(fg_color,bg_color);
    lcd.print(new_string[i]);
  }
}

void EasySMU_IOpanel::DisplayVoltageSourceSetting(int16_t channel, float flt_old, float flt_new)
{

  if (enabled_== 0) return;
  switch (channel)
  {
    case _CH0:
      lcd.setCursor(_SMU0_LEFT+_TEXT_LEFT, _SMU0_TOP+_TEXT_TOP);
      break;
    case _CH1:
      lcd.setCursor(_SMU1_LEFT+_TEXT_LEFT, _SMU1_TOP+_TEXT_TOP);
      break;
    case _CH2:
      lcd.setCursor(_SMU2_LEFT+_TEXT_LEFT, _SMU2_TOP+_TEXT_TOP);
      break;
    case _CH3:
      lcd.setCursor(_SMU3_LEFT+_TEXT_LEFT, _SMU3_TOP+_TEXT_TOP);
      break;
  }

  char str_old[16], str_new[16];
  dtostrf(flt_old,8,4,str_old);
  strcat(str_old,"V");
  dtostrf(flt_new,8,4,str_new);
  strcat(str_new,"V");
  OverwriteOldString(ILI9341_WHITE,ILI9341_BLUE,str_old,str_new);


}

void EasySMU_IOpanel::DisplayMeasuredVoltage(int16_t channel, float flt_old, float flt_new)
{
  if (enabled_== 0) return;

  switch (channel)
  {
    case _CH0:
      lcd.setCursor(_SMU0_LEFT+_TEXT_LEFT_MID, _SMU0_TOP+_TEXT_TOP);
      break;
    case _CH1:
      lcd.setCursor(_SMU1_LEFT+_TEXT_LEFT_MID, _SMU1_TOP+_TEXT_TOP);
      break;
    case _CH2:
      lcd.setCursor(_SMU2_LEFT+_TEXT_LEFT_MID, _SMU2_TOP+_TEXT_TOP);
      break;
    case _CH3:
      lcd.setCursor(_SMU3_LEFT+_TEXT_LEFT_MID, _SMU3_TOP+_TEXT_TOP);
      break;
  }
  char str_old[16], str_new[16];
  dtostrf(flt_old,8,4,str_old);
  strcat(str_old,"V");
  dtostrf(flt_new,8,4,str_new);
  strcat(str_new,"V");
  OverwriteOldString(ILI9341_WHITE,ILI9341_BLUE,str_old,str_new);

}

void EasySMU_IOpanel::DisplayTemperatureOfIadc(int16_t channel, float flt_old, float flt_new)
{
  if (enabled_== 0) return;
  switch (channel)
  {
    case _CH0:
      lcd.setCursor(_SMU0_LEFT+10, _SMU0_TOP+_SMU_TEXT_TOP);
      break;
    case _CH1:
      lcd.setCursor(_SMU1_LEFT+10, _SMU1_TOP+_SMU_TEXT_TOP);
      break;
    case _CH2:
      lcd.setCursor(_SMU2_LEFT+10, _SMU2_TOP+_SMU_TEXT_TOP);
      break;
    case _CH3:
      lcd.setCursor(_SMU3_LEFT+10, _SMU3_TOP+_SMU_TEXT_TOP);
      break;
  }
  char str_old[16], str_new[16];
  dtostrf(flt_old,3,1,str_old);
  strcat(str_old,"C");
  dtostrf(flt_new,3,1,str_new);
  strcat(str_new,"C");
  OverwriteOldString(ILI9341_WHITE,ILI9341_BLUE,str_old,str_new);

}

void EasySMU_IOpanel::DisplayTemperatureOfVadc(int16_t channel, float flt_old, float flt_new)
{
  if (enabled_== 0) return;
  switch (channel)
  {
    case _CH0:
      lcd.setCursor(_SMU0_RIGHT-40, _SMU0_TOP+_SMU_TEXT_TOP);
      break;
    case _CH1:
      lcd.setCursor(_SMU1_RIGHT-40, _SMU1_TOP+_SMU_TEXT_TOP);
      break;
    case _CH2:
      lcd.setCursor(_SMU2_RIGHT-40, _SMU2_TOP+_SMU_TEXT_TOP);
      break;
    case _CH3:
      lcd.setCursor(_SMU3_RIGHT-40, _SMU3_TOP+_SMU_TEXT_TOP);
      break;
  }
  char str_old[16], str_new[16];
  dtostrf(flt_old,3,1,str_old);
  strcat(str_old,"C");
  dtostrf(flt_new,3,1,str_new);
  strcat(str_new,"C");
  OverwriteOldString(ILI9341_WHITE,ILI9341_BLUE,str_old,str_new);

}

void EasySMU_IOpanel::AddLeadingSign(char chrSign, char *string,int8_t strlen)
{
  int8_t i;
  for (i=0; (i<strlen) && (string[i+1]==' '); i++);
  if (string[i]==' ')
  {
    string[i]=chrSign;
  }
}

void EasySMU_IOpanel::DisplayCurrentSourceSetting(int16_t channel, float flt_old, float flt_new, int8_t source_both_sink)
{
  if (enabled_== 0) return;
  switch (channel)
  {
    case _CH0:
      lcd.setCursor(_SMU0_LEFT+_TEXT_LEFT, _SMU0_TOP+_TEXT_TOP_MID);
      break;
    case _CH1:
      lcd.setCursor(_SMU1_LEFT+_TEXT_LEFT, _SMU1_TOP+_TEXT_TOP_MID);
      break;
    case _CH2:
      lcd.setCursor(_SMU2_LEFT+_TEXT_LEFT, _SMU2_TOP+_TEXT_TOP_MID);
      break;
    case _CH3:
      lcd.setCursor(_SMU3_LEFT+_TEXT_LEFT, _SMU3_TOP+_TEXT_TOP_MID);
      break;
  }
  char str_old[16], str_new[16];
  dtostrf(flt_old*1000,7,3,str_old);
  strcat(str_old,"mA");
  AddLeadingSign('+',str_old,16);
  dtostrf(flt_new*1000,7,3,str_new);
  strcat(str_new,"mA");
  if (source_both_sink==_SOURCE_ONLY)
  {
    AddLeadingSign('+',str_new,16);
  }
  else if (source_both_sink==_SINK_ONLY)
  {
    AddLeadingSign('-',str_new,16);
  }
  OverwriteOldString(ILI9341_WHITE,ILI9341_BLUE,str_old,str_new);
}

void EasySMU_IOpanel::DisplayMeasuredCurrent(int16_t channel, float flt_old, float flt_new)
{
  if (enabled_== 0) return;
  switch (channel)
  {
    case _CH0:
      lcd.setCursor(_SMU0_LEFT+_TEXT_LEFT_MID, _SMU0_TOP+_TEXT_TOP_MID);
      break;
    case _CH1:
      lcd.setCursor(_SMU1_LEFT+_TEXT_LEFT_MID, _SMU1_TOP+_TEXT_TOP_MID);
      break;
    case _CH2:
      lcd.setCursor(_SMU2_LEFT+_TEXT_LEFT_MID, _SMU2_TOP+_TEXT_TOP_MID);
      break;
    case _CH3:
      lcd.setCursor(_SMU3_LEFT+_TEXT_LEFT_MID, _SMU3_TOP+_TEXT_TOP_MID);
      break;
  }
  char str_old[16], str_new[16];
  dtostrf(flt_old*1000,7,3,str_old);
  strcat(str_old,"mA");
  dtostrf(flt_new*1000,7,3,str_new);
  strcat(str_new,"mA");
  OverwriteOldString(ILI9341_WHITE,ILI9341_BLUE,str_old,str_new);
}

