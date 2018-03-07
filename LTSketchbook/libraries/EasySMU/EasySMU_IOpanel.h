/*!
Header File for EasySMU_IOpanel Library That Enables Touchscreen Control of EasySMU

[User Guide](http://www.linear.com/docs/58670 "EasySMU User Guide") \n
[Schematic](http://www.linear.com/docs/58671 "Schematic") \n
[Top-Level Linduino Firmware for EasySMU](file:EasySMU__Run_8ino.html "Top-Level Linduino Firmware for EasySMU") \n
[EasySMU Class Reference](file:classEasySMU.html "EasySMU Class Reference") \n
[EasySMU_IOpanel Class Reference](file:classEasySMU__IOpanel.html "EasySMU_IOpanel Class Reference") \n
[EasySMU Webpage](http://www.linear.com/solutions/7943 "EasySMU Webpage") \n

@verbatim
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
  Header File for EasySMU_IOpanel Library That Enables Touchscreen Control of EasySMU
*/
#ifndef Dan_Touch_IOpanel_h
#define Dan_Touch_IOpanel_h

#include "Arduino.h"
#include "EasySMU.h"
#include <Adafruit_ILI9341.h>

#define ADAFRUIT_TFT_TOUCHSHIELD_1947
//#define SEEEDSTUDIO_TFT_TOUCHSHIELD_V2

#ifdef ADAFRUIT_TFT_TOUCHSHIELD_1947
#include <Adafruit_FT6206.h>
// The Adafruit touch sensing is implemented with an FT6206 and communicates using I2C (SCL/SDA)
// The Adafruit display uses hardware SPI, plus #9 & #10
#define TFT_CS 10
#define TFT_DC 9
#endif // ADAFRUIT_TFT_TOUCHSHIELD_1947

#ifdef SEEEDSTUDIO_TFT_TOUCHSHIELD_V2
// The SeeedStudio display uses hardware SPI, plus #5 & #6
#define TFT_CS 5
#define TFT_DC 6

#define YP A2   // must be an analog pin, use "An" notation!
#define XM A1   // must be an analog pin, use "An" notation!
#define YM 14   // can be a digital pin, this is A0
#define XP 17   // can be a digital pin, this is A3

#include <SeeedTouchScreen.h>
#endif // SEEEDSTUDIO_TFT_TOUCHSHIELD_V2

#include <Adafruit_GFX.h>


//! used to indicate which button is pressed by the user
enum { _SMU0=_CH0, _SMU1=_CH1, _SMU2=_CH2, _SMU3=_CH3, _VPLUS, _VMINUS, _VPLUSPLUS, _VMINUSMINUS, _IPLUS, _IMINUS, _IPLUSPLUS, _IMINUSMINUS, _NONE };

//! width of Adafruit TFT display in pixels
#define _LCD_WIDTH 320
//! height of Adafruit TFT display in pixels
#define _LCD_HEIGHT 240

//The results are in pixel dimensions.
//! @name Define dimensions of buttons and text on EasySMU_IOpanel
//! @{
#define _SMU_SPACE  5
#define _SMU_WIDTH  (_LCD_WIDTH-3*_SMU_SPACE)/2
#define _SMU_HEIGHT  ((_LCD_HEIGHT/4)-_SMU_SPACE)

#define _SMU0_TOP  _SMU_SPACE
#define _SMU0_LEFT  _SMU_SPACE
#define _SMU0_RIGHT  (_SMU0_LEFT + _SMU_WIDTH)
#define _SMU0_BOTTOM (_SMU0_TOP + _SMU_HEIGHT)

#define _SMU1_TOP  (_SMU0_BOTTOM +_SMU_SPACE)
#define _SMU1_LEFT  _SMU0_LEFT
#define _SMU1_RIGHT  _SMU0_RIGHT
#define _SMU1_BOTTOM (_SMU1_TOP + _SMU_HEIGHT)

#define _SMU2_TOP  _SMU0_TOP
#define _SMU2_LEFT  _SMU0_RIGHT+_SMU_SPACE
#define _SMU2_RIGHT   _SMU2_LEFT + _SMU_WIDTH
#define _SMU2_BOTTOM _SMU0_BOTTOM

#define _SMU3_TOP _SMU1_TOP
#define _SMU3_LEFT _SMU2_LEFT
#define _SMU3_RIGHT _SMU2_RIGHT
#define _SMU3_BOTTOM _SMU1_BOTTOM

#define _BUTTON_WIDTH  (_LCD_WIDTH-5*_SMU_SPACE)/4
#define _BUTTON_HEIGHT ((_LCD_HEIGHT-5*_SMU_SPACE)/4)
#define _BUTTON_RADIUS (_BUTTON_HEIGHT/12)

#define _VPLUS_TOP (_VPLUS_BOTTOM-_BUTTON_HEIGHT)
#define _VPLUS_LEFT _SMU_SPACE
#define _VPLUS_RIGHT (_VPLUS_LEFT+_BUTTON_WIDTH)
#define _VPLUS_BOTTOM  (_VMINUS_TOP-_SMU_SPACE)

#define _VMINUS_TOP (_VMINUS_BOTTOM-_BUTTON_HEIGHT)
#define _VMINUS_LEFT _SMU_SPACE
#define _VMINUS_RIGHT (_VMINUS_LEFT+_BUTTON_WIDTH)
#define _VMINUS_BOTTOM  (_LCD_HEIGHT-_SMU_SPACE)

#define _VPLUSPLUS_TOP _VPLUS_TOP
#define _VPLUSPLUS_LEFT (_VPLUS_RIGHT+_SMU_SPACE)
#define _VPLUSPLUS_RIGHT _SMU1_RIGHT
#define _VPLUSPLUS_BOTTOM _VPLUS_BOTTOM

#define _VMINUSMINUS_LEFT _VPLUSPLUS_LEFT
#define _VMINUSMINUS_RIGHT _VPLUSPLUS_RIGHT
#define _VMINUSMINUS_TOP _VMINUS_TOP
#define _VMINUSMINUS_BOTTOM _VMINUS_BOTTOM

#define _IPLUS_TOP _VPLUS_TOP
#define _IPLUS_LEFT _SMU3_LEFT
#define _IPLUS_RIGHT (_IPLUS_LEFT+_BUTTON_WIDTH)
#define _IPLUS_BOTTOM _VPLUS_BOTTOM

#define _IMINUSMINUS_TOP _IMINUS_TOP
#define _IMINUSMINUS_LEFT _IPLUSPLUS_LEFT
#define _IMINUSMINUS_RIGHT _IPLUSPLUS_RIGHT
#define _IMINUSMINUS_BOTTOM _IMINUS_BOTTOM

#define _IMINUS_LEFT _IPLUS_LEFT
#define _IMINUS_RIGHT _IPLUS_RIGHT
#define _IMINUS_TOP _VMINUS_TOP
#define _IMINUS_BOTTOM _VMINUS_BOTTOM

#define _IPLUSPLUS_TOP _VPLUSPLUS_TOP
#define _IPLUSPLUS_LEFT (_IPLUS_RIGHT + _SMU_SPACE)
#define _IPLUSPLUS_RIGHT _SMU3_RIGHT
#define _IPLUSPLUS_BOTTOM _VPLUSPLUS_BOTTOM

#define _TEXT_LEFT  5
#define _TEXT_LEFT_MID  75
#define _BUTTON_TEXT_TOP 35
#define _BUTTON_TEXT_LEFT 25
#define _BUTTON_TEXT_TOP 35
#define _BUTTON_TEXT_PLUSPLUS_LEFT 15
#define _SMU_TEXT_TOP 15
#define _SMU_TEXT_LEFT 65
#define _TEXT_TOP  32
#define _TEXT_TOP_MID  48
//! @}

/*!The EasySMU_IOpanel class provides an interface between the EasySMU and a touchscreen.

[User Guide](http://www.linear.com/docs/58670 "EasySMU User Guide") \n
[Schematic](http://www.linear.com/docs/58671 "Schematic") \n
[Top-Level Linduino Firmware for EasySMU](file:EasySMU__Run_8ino.html "Top-Level Linduino Firmware for EasySMU") \n
[EasySMU Class Reference](file:classEasySMU.html "EasySMU Class Reference") \n
[EasySMU_IOpanel Class Reference](file:classEasySMU__IOpanel.html "EasySMU_IOpanel Class Reference") \n
[EasySMU Webpage](http://www.linear.com/solutions/7943 "EasySMU Webpage") \n
*/
class EasySMU_IOpanel
{
  private:
    uint16_t text_width_;
    uint16_t text_height_;
    int16_t text_x_offset_;
    int16_t text_y_offset_;
    //! Draw text at the x and y pixel location
    void DrawText(  int16_t x, //!< horizontal location in pixels
                    int16_t y, //!< vertical location in pixels
                    const char *sDisplay //!< string to display
                 );
    //! Clear old text and write new text. The Adafruit library does not contain a fast way to clear an area of the screen, so this overwrites old characters with the background color.
    //! To reduce the flashing of clearing the entire string before writing the new string, this function clears and writes a single character at a time.
    void OverwriteOldString(uint16_t fg_color, //!< foreground color
                            uint16_t bg_color, //!< background color
                            char *old_string, //!< the *old_string to be cleared
                            const char *new_string //!< the *new_string to be written
                           );
    //! Used to put a '+' or '-' in front of a number (at the correct location)
    void AddLeadingSign(char chrSign, //!< '+' or '-'
                        char *string, //!< *string contains a text form of the number that will have leading sign character placed in front
                        int8_t strlen //!< length of text in *string
                       );

  public:
    Adafruit_ILI9341 lcd = Adafruit_ILI9341(TFT_CS, TFT_DC);

#ifdef ADAFRUIT_TFT_TOUCHSHIELD_1947
    Adafruit_FT6206 touchp = Adafruit_FT6206();
#endif // ADAFRUIT_TFT_TOUCHSHIELD_1947

#ifdef SEEEDSTUDIO_TFT_TOUCHSHIELD_V2
    TouchScreen touchp = TouchScreen(XP, YP, XM, YM); //init TouchScreen port pins
#endif // SEEEDSTUDIO_TFT_TOUCHSHIELD_V2

    //! Initialize the IO panel, including drawing buttons, etc.
    void Init();

    uint32_t start_button_pressed_;   //!< keep track of when the button was pressed to later calculate the duration it has been held
    uint32_t duration_button_pressed_;  //!< stores a value corresponding to how long the button has been pressed
    //! check if a button is pressed
    //! @return the button pressed, based on enum
    int CheckButton();
    int button_pressed_; //!< stores the value of the button pressed (from enum)
    int SMUselected_; //!< stores which SMU channel is selected on the TFT display
    uint8_t enabled_; //!< indicates if the TFT is enabled. 1=enabled, 0=disabled. (TFT may be disabled to reduce noise in the SMU output caused by SPI communication.)

    //! Draw the SMU labels. The selected channel will be green.
    void DisplaySMULabel() ;
    //! Draw the voltage source setting for a single channel.
    void DisplayVoltageSourceSetting(int16_t channel, //!< channel to draw
                                     float flt_old, //!< the old value to be overwitten in the background color
                                     float flt_new //!< the new value to be drawn in the foreground color
                                    ) ;
    //! Draw the measured voltage for a single channel.
    void DisplayMeasuredVoltage(int16_t channel,
                                float flt_old, //!< the old value to be overwitten in the background color
                                float flt_new  //!< the new value to be drawn in the foreground color
                               ) ;
    //! Draw the current source setting for a single channel.
    void DisplayCurrentSourceSetting(int16_t channel,
                                     float flt_old, //!< the old value to be overwitten in the background color
                                     float flt_new, //!< the new value to be drawn in the foreground color
                                     int8_t source_both_sink //!< If source only mode is set, a '+' will be drawn in front. If sink only mode is set, a '-' will be drawn in front.
                                    );
    //! Draw the measured current for a single channel.
    void DisplayMeasuredCurrent(int16_t channel,
                                float flt_old, //!< the old value to be overwitten in the background color
                                float flt_new  //!< the new value to be drawn in the foreground color
                               ) ;
    //! Draw the measured temperature from the current ADC. Presently, not used as it clutters up the screen.
    void DisplayTemperatureOfIadc(int16_t channel,
                                  float flt_old, //!< the old value to be overwitten in the background color
                                  float flt_new  //!< the new value to be drawn in the foreground color
                                 ) ;
    //! Draw the measured temperature from the voltage ADC. Presently, not used as it clutters up the screen.
    void DisplayTemperatureOfVadc(int16_t channel,
                                  float flt_old, //!< the old value to be overwitten in the background color
                                  float flt_new  //!< the new value to be drawn in the foreground color
                                 ) ;

};

#endif
