/*!
Top-Level Linduino Firmware for EasySMU: I2C Address Translator Demonstration and a Simple Multi-Channel Source Measurement Unit

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
  Firmware Loaded on Linduino to Control EasySMU Shields
*/

// Used for original factory calibration. Requires more memory than normal operation, so touchscreen is disabled in this mode.
// It should not be necessary for the user to perform calibration, so please do not enable calibration.
// #define ENABLE_CALIBRATION

//# Include
//# ==================================
#include "Arduino.h"
#include "LT_I2C.h"
#include "Wire.h"
#include "LTC2485.h"
#include "LTC2655.h"
#include "EasySMU.h"
#include "PrintError.h"
#include "MasterAddressConstants.h"

// The following header files are only required for compatibility with Arduino 1.6.4 build process
// They are not required in later versions of Arduino IDE.
#include "Linduino.h"
#include "QuikEval_EEPROM.h"
#include <Adafruit_ILI9341.h>
#include <Adafruit_GFX.h>
#include <Adafruit_FT6206.h>
#include "EEPROM.h"
#include "UserInterface.h"
#include <SPI.h>

#ifndef ENABLE_CALIBRATION
#include <EasySMU_IOpanel.h>
#endif

void DelayAndUserInput(uint32_t delay);

//# Define
//# ==================================

//! Give the user 500ms to adjust a value before setting the output. This allows the user to adjust a value before it shows up at the output.
#define _BUTTON_WAIT_TIME 500

//! Touchscreen step size increases after _BUTTON_TIME_1 milliseconds. This allows faster user adjustment when the button is held longer.
#define _BUTTON_TIME_1 4000
//! Touchscreen step size increases after _BUTTON_TIME_2 milliseconds. This allows faster user adjustment when the button is held longer.
#define _BUTTON_TIME_2 2000
//! Voltage step size after _BUTTON_TIME_1
#define _VBUTTON_STEP_1 3
//! Voltage step size after _BUTTON_TIME_2
#define _VBUTTON_STEP_2 1
//! Current step size after _BUTTON_TIME_1
#define _IBUTTON_STEP_1 0.0000030
//! Current step size after _BUTTON_TIME_2
#define _IBUTTON_STEP_2 0.0000010

//! Array of eight EasySMU instances that hold state for up to eight stacked EasySMU shields.
//! The I2C addresses of each board are configured while calling these constructors.
EasySMU SMU[8] =
{
  EasySMU(I2C_Board0_EEPROM_ADDR0, I2C_Board0_LTC2655_LLL_ADDR0, I2C_Board0_LTC2485_Vout_LF_ADDR0, I2C_Board0_LTC2485_Iout_LH_ADDR0),
  EasySMU(I2C_Board1_EEPROM_ADDR0, I2C_Board1_LTC2655_LLL_ADDR0, I2C_Board1_LTC2485_Vout_LF_ADDR0, I2C_Board1_LTC2485_Iout_LH_ADDR0),
  EasySMU(I2C_Board2_EEPROM_ADDR0, I2C_Board2_LTC2655_LLL_ADDR0, I2C_Board2_LTC2485_Vout_LF_ADDR0, I2C_Board2_LTC2485_Iout_LH_ADDR0),
  EasySMU(I2C_Board3_EEPROM_ADDR0, I2C_Board3_LTC2655_LLL_ADDR0, I2C_Board3_LTC2485_Vout_LF_ADDR0, I2C_Board3_LTC2485_Iout_LH_ADDR0),
  EasySMU(I2C_Board4_EEPROM_ADDR0, I2C_Board4_LTC2655_LLL_ADDR0, I2C_Board4_LTC2485_Vout_LF_ADDR0, I2C_Board4_LTC2485_Iout_LH_ADDR0),
  EasySMU(I2C_Board5_EEPROM_ADDR0, I2C_Board5_LTC2655_LLL_ADDR0, I2C_Board5_LTC2485_Vout_LF_ADDR0, I2C_Board5_LTC2485_Iout_LH_ADDR0),
  EasySMU(I2C_Board6_EEPROM_ADDR0, I2C_Board6_LTC2655_LLL_ADDR0, I2C_Board6_LTC2485_Vout_LF_ADDR0, I2C_Board6_LTC2485_Iout_LH_ADDR0),
  EasySMU(I2C_Board7_EEPROM_ADDR0, I2C_Board7_LTC2655_LLL_ADDR0, I2C_Board7_LTC2485_Vout_LF_ADDR0, I2C_Board7_LTC2485_Iout_LH_ADDR0)
};

#ifndef ENABLE_CALIBRATION
EasySMU_IOpanel IOpanel; //!< IOpanel provides an interface between the EasySMU hardware and a touchscreen
#endif

//! Used to buffer data from serial interface.
char serial_data[100];

//! Check if first three characters match a three character command in chrCommands.
//! @return Index of location in chrCommands of the match. -1 if no match is found.
int16_t getToken(char strData[], //!< string to be checked to see if a match is found in chrCommands
                 const char chrCommands[][3], //!< list of valid three character commands
                 int16_t sizeSerialCommands //!< the number of commands in chrCommands
                )
{
  int16_t i;
  char *strchr1;

  for (i=0; i<sizeSerialCommands; i++)
  {
    if (chrCommands[i][0]==toupper(strData[0]) && chrCommands[i][1]==toupper(strData[1]) && chrCommands[i][2]==toupper(strData[2]))
    {
      if ((strchr1=strchr(strData,':'))==NULL) strchr1=strchr(strData,' '); //returns location of next ':' or ' ' else it returns NULL
      if (strchr1==NULL) strData[0]='\0';
      else strcpy(strData,strchr1+1); //Removes token from strData string
      return i;
    }
  }
  return -1;
}

//! Read available characters from the serial port and append the result to the *serialData string.
//! @return Returns true when '\\n' or '\\r' is read. Otherwise, it returns false.
boolean ReadLine(char *serialData //!< string used to store all collected data collected since the last '\\n' or '\\r'
                )
{
  static int16_t i=0;
  while (Serial.available()>0 && Serial.peek()!='\n' && Serial.peek()!='\r')
  {
    if (i<100) serialData[i++]= Serial.read();  //Rolls over to beginning of string if too many characters
    else i=0;
  }
  serialData[i]='\0';

  if (Serial.peek()=='\n')  //Remove \n and also \r if present
  {
    Serial.read();
    if (Serial.peek()=='\r') Serial.read();
    i=0;
    return true;
  }
  if (Serial.peek()=='\r')   //Remove \r and also \n if present
  {
    Serial.read();
    if (Serial.peek()=='\n') Serial.read();
    i=0;
    return true;
  }
  return false;
}

#define SERIAL_COMMANDS_ERROR -1
/*!
Checks for the following valid commands from the serial interface and performs the corresponding actions.

Where indicated, 'x' is substituted with 0-7, a number corresponding to the appropriate channel.
|Command        | Description              |
|:--------------|:-------------------------|
|*RST           | Reset to power-up state  |
|*IDN or *IDN?  | Return ID string that includes revision, channels that are present, serial number of each channel, etc. |
|LCD:ENA        | Enable the LCD/TFT display (default) |
|LCD:DIS        | Disable the LCD/TFT display. This might be useful to reduce noise at the output caused by SPI communication with the screen. |
|CHx:DIS        | Disable the output to put it in a relatively high-impedance state. |
|CHx:ENA        | Enable the channel. (Default state) |
|CHx:VOL value  | Set the output voltage to 'value' in volts |
|CHx:CUR value  | Set the output current to 'current' in amps. If current is preceeded by '+' or '-' it will force source or sink only operation. |
|CHx:CAL:SET    | Calibration routine. (Factory use only. Disabled by default.) |
|CHx:CAL:RES    | Restore factory calibration. (Only useful if the user has overwritten the calibration, which is not possible without editing the firmware.) |
|CHx:MEA:VOL    | Returns the measured output voltage in volts. |
|CHx:MEA:CUR    | Returns the measured output current in amps. |
*/
void ParseSerialData(char chrSerialData[] //!< data received from the serial interface
                    )
{
  float fValue;
  uint8_t intSMUchan=_CH0;

  if (chrSerialData[0]=='*')
  {
#define SERIAL_COMMANDS_SIZE_0 2
    const char SerialCommands_0[SERIAL_COMMANDS_SIZE_0][3]=
    {
      {'R','S','T'},
      {'I','D','N'}
    };
    enum {SERIAL_0_RST=0, SERIAL_0_IDN=1};

    switch (getToken(chrSerialData+1, SerialCommands_0, SERIAL_COMMANDS_SIZE_0))
    {
      case SERIAL_0_RST:
        setup();
        Serial.println("Reset");
        break;
      case SERIAL_0_IDN:
        Serial.print(F(_EASYSMU_ID_STRING));
        for (int8_t iChan=_CH0; iChan <= _CH7; iChan++)
        {
          if (SMU[iChan].IsPresent())
          {
            Serial.print(", CH");
            Serial.print(iChan);
            Serial.print(":");
            SMU[iChan].PrintFactoryCalibrationInfo();
          }
        }
        Serial.println();
        break;
    }
    return;
  }

#define SERIAL_COMMANDS_SIZE_1 9
  const char SerialCommands_1[SERIAL_COMMANDS_SIZE_1][3]=
  {
    {'C','H','0'},
    {'C','H','1'},
    {'C','H','2'},
    {'C','H','3'},
    {'C','H','4'},
    {'C','H','5'},
    {'C','H','6'},
    {'C','H','7'},
    {'L','C','D'}
  };
  enum {SERIAL_1_CH0=0, SERIAL_1_CH1=1, SERIAL_1_CH2=2, SERIAL_1_CH3=3, SERIAL_1_CH4=4, SERIAL_1_CH5=5, SERIAL_1_CH6=6, SERIAL_1_CH7=7, SERIAL_1_LCD=8 };

  switch (getToken(chrSerialData, SerialCommands_1, SERIAL_COMMANDS_SIZE_1))
  {
    case SERIAL_1_CH0:
      intSMUchan=_CH0;
      break;
    case SERIAL_1_CH1:
      intSMUchan=_CH1;
      break;
    case SERIAL_1_CH2:
      intSMUchan=_CH2;
      break;
    case SERIAL_1_CH3:
      intSMUchan=_CH3;
      break;
    case SERIAL_1_CH4:
      intSMUchan=_CH4;
      break;
    case SERIAL_1_CH5:
      intSMUchan=_CH5;
      break;
    case SERIAL_1_CH6:
      intSMUchan=_CH6;
      break;
    case SERIAL_1_CH7:
      intSMUchan=_CH7;
      break;
    case SERIAL_1_LCD:
#ifndef ENABLE_CALIBRATION
#define SERIAL_COMMANDS_SIZE_LCD 2
      const char SerialCommands_LCD[SERIAL_COMMANDS_SIZE_LCD][3]=
      {
        {'D','I','S'},
        {'E','N','A'}
      };
      enum {SERIAL_LCD_DIS=0, SERIAL_LCD_ENA=1};
      if (getToken(chrSerialData, SerialCommands_LCD, SERIAL_COMMANDS_SIZE_LCD)==SERIAL_LCD_DIS)
      {
        IOpanel.enabled_=0;
        Serial.println(F("Disabled"));
      }
      else
      {
        IOpanel.enabled_=1;
        Serial.println(F("Enabled"));
      }
#endif
      return;
      //break;
  }

#define SERIAL_COMMANDS_SIZE_12 6
  const char SerialCommands_12[SERIAL_COMMANDS_SIZE_12][3]=
  {
    {'E','N','A'},
    {'D','I','S'},
    {'V','O','L'},
    {'C','U','R'},
    {'M','E','A'},
    {'C','A','L'}
  };
  enum {SERIAL_12_ENA=0, SERIAL_12_DIS=1, SERIAL_12_VOL=2, SERIAL_12_CUR=3, SERIAL_12_MEA=4, SERIAL_12_CAL=5};

  switch (getToken(chrSerialData, SerialCommands_12, SERIAL_COMMANDS_SIZE_12))
  {
#ifndef ENABLE_CALIBRATION
      //DJE: Consider adding check for success for failure.
    case SERIAL_12_ENA:
      {
        SMU[intSMUchan].EnableOutput();
        Serial.println(F("Enabled"));
        return;
      }

    case SERIAL_12_DIS:
      //DJE: Consider adding check for success for failure.
      {
        SMU[intSMUchan].DisableOutput();
        Serial.println(F("Disabled"));
        return;
      }
#endif
    case SERIAL_12_VOL:
      {
        fValue=atof(chrSerialData);
        float flt_old=SMU[intSMUchan].fltReadVoltageSourceSetting();
        if (SMU[intSMUchan].fltSetCommitVoltageSource(fValue)) printError(_PRINT_ERROR_VOLTAGE_SOURCE_SETTING);
        float flt_new=SMU[intSMUchan].fltReadVoltageSourceSetting();
#ifndef ENABLE_CALIBRATION
        if (intSMUchan <= _CH3)
        {
          IOpanel.DisplayVoltageSourceSetting(intSMUchan,flt_old,flt_new);
        }
#endif
      }

      Serial.println(fValue,4);
      return;
    case SERIAL_12_CUR:
      {
        //If the user places an explicit '+' or '-' in front of the current source setting in the serial terminal, it indicates a sink or source only mode.
        int8_t source_both_sink;
        if (chrSerialData[0]=='+')
        {
          source_both_sink=_SOURCE_ONLY;
          fValue=atof(chrSerialData+1);
          Serial.print('+');
        }
        else if (chrSerialData[0]=='-')
        {
          source_both_sink=_SINK_ONLY;
          fValue=atof(chrSerialData+1);
          Serial.print('-');
        }
        else
        {
          source_both_sink=_SOURCE_AND_SINK;
          fValue=atof(chrSerialData);
        }

        float flt_old=SMU[intSMUchan].fltReadCurrentSourceSetting();
        if (SMU[intSMUchan].fltSetCommitCurrentSource(fValue,source_both_sink)) printError(_PRINT_ERROR_CURRENT_SOURCE_SETTING);
        float flt_new=SMU[intSMUchan].fltReadCurrentSourceSetting();
#ifndef ENABLE_CALIBRATION
        if (intSMUchan <= _CH3)
        {
          //Consider adding + or - to display to indicate if it is sink only or source only
          IOpanel.DisplayCurrentSourceSetting(intSMUchan,flt_old,flt_new,source_both_sink);
        }
#endif
      }
      Serial.println(fValue,4);
      return;
    case SERIAL_12_MEA:
      {
#define SERIAL_COMMANDS_SIZE_121 2

        const char SerialCommands_121[SERIAL_COMMANDS_SIZE_121][3]=
        {
          {'V','O','L'},
          {'C','U','R'}
        };
        enum {SERIAL_121_VOL=0, SERIAL_121_CUR=1};

        switch (getToken(chrSerialData, SerialCommands_121, SERIAL_COMMANDS_SIZE_121))
        {
          case SERIAL_121_VOL:
            {
              SMU[intSMUchan].MeasureVoltage();
              char sDisplay[16];
              dtostrf(SMU[intSMUchan].MeasureVoltage(),8,4,sDisplay);
              Serial.println(sDisplay);
              return;
            }
          case SERIAL_121_CUR:
            {
              char sDisplay[16];
              dtostre(SMU[intSMUchan].MeasureCurrent(),sDisplay,5,0);
              Serial.println(sDisplay);
              return;
            }
        }
      }
    case SERIAL_12_CAL:
#define SERIAL_COMMANDS_SIZE_122 2
      const char SerialCommands_122[SERIAL_COMMANDS_SIZE_122][3]=
      {
        {'S','E','T'},
        {'R','E','S'}
      };
      enum {SERIAL_122_SET=0, SERIAL_122_RES=1};

      switch (getToken(chrSerialData, SerialCommands_122, SERIAL_COMMANDS_SIZE_12))
      {
        case SERIAL_122_SET:
#ifdef ENABLE_CALIBRATION
          SMU[intSMUchan].SetCalibration();
#else
          Serial.println(F("Calibration not enabled."));
#endif
          return;
        case SERIAL_122_RES:
          //SMU[intSMUchan].ResetCalibration();
          //SMU[intSMUchan].WriteCalibration();
          SMU[intSMUchan].RestoreFactoryCalibration();
          SMU[intSMUchan].ReadCalibration();
          Serial.println(F("Calibration Restored"));
          return;
      }
  }
  printError(_PRINT_ERROR_SERIAL_COMMAND);
  return;

}

#ifndef ENABLE_CALIBRATION
//! Check if the user is pressing the V+, V++, V-, or V-- buttons. If so, adjust voltage.
void SetVoltage()
{
  int16_t step_size;
  int16_t down_up;
  int16_t SMUnum;

  SMUnum=IOpanel.SMUselected_;
  IOpanel.CheckButton();
  while ((IOpanel.button_pressed_>=_VPLUS) && (IOpanel.button_pressed_<=_VMINUSMINUS))
  {
    if ((IOpanel.button_pressed_==_VPLUS) || (IOpanel.button_pressed_==_VPLUSPLUS)) down_up=-1;
    else down_up=1;
    //else return;

    //Could also just set value based on duration button is pressed.
    step_size=(IOpanel.duration_button_pressed_ > _BUTTON_TIME_1) ? _VBUTTON_STEP_1 : _VBUTTON_STEP_2;
    if ((IOpanel.button_pressed_==_VPLUSPLUS) || (IOpanel.button_pressed_==_VMINUSMINUS))
    {
      step_size=step_size*16;
    }
    step_size*=down_up;
    delay(10);
    float flt_old=SMU[SMUnum].fltReadVoltageSourceSetting();
    SMU[SMUnum].codeStepVoltageSourceSetting(step_size); //Changes Set Voltage by step_size LSB's (positive or negative) while preventing rollover
    float flt_new=SMU[SMUnum].fltReadVoltageSourceSetting();
    IOpanel.DisplayVoltageSourceSetting(SMUnum,flt_old,flt_new);
    while (IOpanel.duration_button_pressed_<_BUTTON_WAIT_TIME && ((IOpanel.button_pressed_>=_VPLUS) && (IOpanel.button_pressed_<=_VMINUSMINUS)))  //Allow single button press for LSB increment.
    {
      IOpanel.CheckButton();
    }
    IOpanel.CheckButton();

    //Give user _BUTTON_WAIT_TIME to push other button to reverse before changing output voltage.
    //Use of cast to (int32_t) in comparison should avoid rollover issues according to http://playground.arduino.cc/Code/TimingRollover
    uint32_t end_time=millis()+_BUTTON_WAIT_TIME;
    while (((int32_t)(millis()-end_time)<0) && (IOpanel.button_pressed_==_NONE))  //Give user _BUTTON_WAIT_TIME to push other button to reverse before changing output voltage.
    {
      IOpanel.CheckButton();
    }
  }

  SMU[SMUnum].CommitVoltageSourceSetting();
}

//! Check if the user is pressing the I+, I++, I-, or I-- buttons. If so, adjust the current.
void SetCurrent()
{
  float step_size;
  int16_t up_down;
  int16_t CHnum;

  CHnum=IOpanel.SMUselected_;

  IOpanel.CheckButton();
  while ((IOpanel.button_pressed_>=_IPLUS) && (IOpanel.button_pressed_<=_IMINUSMINUS))
  {
    if ((IOpanel.button_pressed_==_IPLUS) || (IOpanel.button_pressed_==_IPLUSPLUS)) up_down=1;
    else up_down=-1;

    //Could also just set value based on duration button is pressed.
    step_size=(IOpanel.duration_button_pressed_ > _BUTTON_TIME_1) ? _IBUTTON_STEP_1 : _IBUTTON_STEP_2;
    if ((IOpanel.button_pressed_==_IPLUSPLUS) || (IOpanel.button_pressed_==_IMINUSMINUS))
    {
      step_size=step_size*16;
    }
    step_size*=up_down;
    delay(10);
    float flt_old=SMU[CHnum].fltReadCurrentSourceSetting();
    SMU[CHnum].fltStepCurrentSourceSetting(step_size); //Changes Set Current by step_size LSB's (positive or negative) while preventing rollover
    float flt_new=SMU[CHnum].fltReadCurrentSourceSetting();
    IOpanel.DisplayCurrentSourceSetting(CHnum,flt_old,flt_new,_SOURCE_AND_SINK);

    while (IOpanel.duration_button_pressed_<_BUTTON_WAIT_TIME && ((IOpanel.button_pressed_>=_IPLUS) && (IOpanel.button_pressed_<=_IMINUSMINUS)))  //Allow single button press for LSB increment.
    {
      IOpanel.CheckButton();
    }
    IOpanel.CheckButton();

    //Give user _BUTTON_WAIT_TIME to push other button to reverse before changing output voltage.
    //Use of cast to (int32_t) in comparison should avoid rollover issues according to http://playground.arduino.cc/Code/TimingRollover
    uint32_t end_time=millis()+_BUTTON_WAIT_TIME;
    while (((int32_t)(millis()-end_time)<0) && (IOpanel.button_pressed_==_NONE))  //Give user _BUTTON_WAIT_TIME to push other button to reverse before changing output voltage.
    {
      IOpanel.CheckButton();
    }
  }
  SMU[CHnum].CommitCurrentSourceSetting();
}
#endif

//! Wait for 'delay' milliseconds while handling button presses on the touchscreen.
void DelayAndUserInput(uint32_t delay //!< Number of milliseconds to wait in this loop
                      )
{
  uint32_t end_time=millis()+delay;

  while ((int32_t)(millis()-end_time)<0)
  {
#ifndef ENABLE_CALIBRATION
    IOpanel.CheckButton();
    if ((IOpanel.button_pressed_>=_VPLUS) && (IOpanel.button_pressed_<=_VMINUSMINUS)) SetVoltage();
    if ((IOpanel.button_pressed_>=_IPLUS) && (IOpanel.button_pressed_<=_IMINUSMINUS)) SetCurrent();
#endif
  }
}

//! Initialize the Linduino, EasySMU, screen, etc.
void setup()
{

  Wire.begin();
  i2c_enable(); //Set Linduino default I2C speed instead of Wire default settings.

  pinMode(QUIKEVAL_MUX_MODE_PIN, OUTPUT);   //Set Linduino MUX pin to disable the I2C MUX. Otherwise it can cause unpredictable behavior.
  digitalWrite(QUIKEVAL_MUX_MODE_PIN, LOW);

  for (int8_t iChan=_CH0; iChan <= _CH7; iChan++)
  {
    SMU[iChan].ReadCalibration();
    if (SMU[iChan].present_ != 0)
    {
      SMU[iChan].fltSetCommitCurrentSource(0.001,0);
      SMU[iChan].fltSetCommitVoltageSource(0);
      SMU[iChan].EnableOutput();
      SMU[iChan].MeasureVoltage();
      SMU[iChan].MeasureCurrent();
    }
  }

#ifndef ENABLE_CALIBRATION
  IOpanel.Init();  //Note Adafruit calls Wire.begin().

  for (int8_t iChan=_CH0; iChan <= _CH7; iChan++)
  {
    if (SMU[iChan].present_ != 0)
    {
      float fltIout=SMU[iChan].fltReadCurrentSourceSetting();
      float fltVout=SMU[iChan].fltReadVoltageSourceSetting();
      if (iChan <= _CH3)
      {
        IOpanel.DisplayCurrentSourceSetting(iChan, fltIout,fltIout, _SOURCE_AND_SINK);
        IOpanel.DisplayVoltageSourceSetting(iChan, fltVout,fltVout);
      }
    }
  }
#endif

  Serial.begin(9600);

}

//! Arduino/Linduino loop that runs continuously.
void loop()
{
  //With this delay on every cycle, ideally the converters will never enter a time_out because they will finish conversion before polled each time.
  DelayAndUserInput(_LTC2485_CONVERSION_TIME);
  for (int8_t iChan=_CH0; iChan<=_CH7; iChan++)
  {
    if (ReadLine(serial_data))
    {
      ParseSerialData(serial_data);
    }
    if (SMU[iChan].IsPresent()!=0)
    {
#ifndef ENABLE_CALIBRATION
      float fltOldVout=SMU[iChan].flt_measured_voltage_;
      float fltOldIout=SMU[iChan].flt_measured_current_;
#endif
      SMU[iChan].MeasureVoltageCurrent();
#ifndef ENABLE_CALIBRATION
      if (iChan <= _CH3)
      {
        IOpanel.DisplayMeasuredVoltage(iChan, fltOldVout, SMU[iChan].flt_measured_voltage_);
        IOpanel.DisplayMeasuredCurrent(iChan, fltOldIout, SMU[iChan].flt_measured_current_);
      }
#endif
    }
  }

  //Poll temperature once every ten seconds.
  /*  static uint32_t next_temperature_update=0;
    if ((int32_t)(millis()-next_temperature_update) >= 0)
    {
      for (int8_t iChan=_CH0; iChan<=_CH7; iChan++)
      {
        if (SMU[iChan].IsPresent()!=0)
        {
  #ifndef ENABLE_CALIBRATION
          float fltOldTofIadc=SMU[iChan].flt_temperature_of_Iadc_;
          float fltOldTofVadc=SMU[iChan].flt_temperature_of_Vadc_;
  #endif
          SMU[iChan].MeasureTemperatureOfADCs();
  #ifndef ENABLE_CALIBRATION
          IOpanel.DisplayTemperatureOfIadc(iChan, fltOldTofIadc, SMU[iChan].flt_TofIadc_);
          IOpanel.DisplayTemperatureOfVadc(iChan, fltOldTofVadc, SMU[iChan].flt_TofVadc_);
  #endif
        }


      }
      next_temperature_update += 10000;
    }*/
}



