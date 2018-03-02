/*!
Class Library Header File for EasySMU: I2C Address Translator Demonstration and a Simple Multi-Channel Source Measurement Unit

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
*/


/*! @file
@ingroup EasySMU
Class Library Header File for EasySMU: I2C Address Translator Demonstration and a Simple Multi-Channel Source Measurement Unit
*/

#ifndef EasySMU_h
#define EasySMU_h

#include "Arduino.h"
#include "LTC2655.h"

#include "LTC2485.h"
#include "QuikEval_EEPROM.h"

//!ID string written to EasySMU EEPROM. Value that will be returned by *IDN? serial command.
#define _EASYSMU_ID_STRING "Linear Technology, EasySMU-1.01 DC2591A"

//Constants used to specify channel
//! @name Channel name used as index in EasySMU array
//! @{
#define _CH0 0
#define _CH1 1
#define _CH2 2
#define _CH3 3
#define _CH4 4
#define _CH5 5
#define _CH6 6
#define _CH7 7
//! @}

//! @name DAC channel name (LTC2655 is a four channel DAC).
//! @{
//!Channel A of the DAC is configures the output voltage.
#define _EASYSMU_VOLTAGE_DAC LTC2655_DAC_A
//!Channel B of the DAC configures the maximum pullup current
#define _EASYSMU_CURRENT_PULLUP_DAC LTC2655_DAC_B
//!Channel C of the DAC configures the maximum pulldown current.
#define _EASYSMU_CURRENT_PULLDOWN_DAC LTC2655_DAC_C
//!Channel D of the DAC is used to enable or disable the LTC1970's output. It is essentially used as a binary output here.
#define _EASYSMU_ENABLE_OUTPUT_DAC LTC2655_DAC_D
//! @}

//! @name ADC conversion times.
//! @{
//! LTC2485 ADC conversion time in milliseconds (1X mode)
#define _LTC2485_CONVERSION_TIME_1X 150
//! LTC2485 ADC conversion time in milliseconds (2X mode)
#define _LTC2485_CONVERSION_TIME_2X 75
//! Use 1X mode conversion time for EasySMU conversions
#define _LTC2485_CONVERSION_TIME _LTC2485_CONVERSION_TIME_1X
//! @}

//! A backup version of the factory calibration is stored at this address. Factory calibration may be recovered from here to override a user calibration.
#define EEPROM_FACTORY_CAL_STATUS_ADDRESS EEPROM_CAL_STATUS_ADDRESS+sizeof(eeprom_data_union)
//! Channel ID information returned by *IDN? command
#define EEPROM_FACTORY_CAL_INFO_ADDRESS EEPROM_FACTORY_CAL_STATUS_ADDRESS+sizeof(eeprom_data_union)

//! @name Used as a parameter to functions where source, sink, or both is specified.
//! @{
#define _SOURCE_ONLY 1
#define _SOURCE_AND_SINK 0
#define _SINK_ONLY -1
//! @}

//! Defines length of the calibartion info string (serial number) which is stored in the EEPROM
#define CAL_INFO_STRLEN 32

/*!The EasySMU class contains functions for interacting with the EasySMU source measurement unit.
At a higher level (Top-Level Linduino Firmware), this EasySMU class is stored in an eight element array. Each element of the array
contains data for one channel, allowing one Linduino to control up to eight EasySMU shields.

[User Guide](http://www.linear.com/docs/58670 "EasySMU User Guide") \n
[Schematic](http://www.linear.com/docs/58671 "Schematic") \n
[Top-Level Linduino Firmware for EasySMU](file:EasySMU__Run_8ino.html "Top-Level Linduino Firmware for EasySMU") \n
[EasySMU Class Reference](file:classEasySMU.html "EasySMU Class Reference") \n
[EasySMU_IOpanel Class Reference](file:classEasySMU__IOpanel.html "EasySMU_IOpanel Class Reference") \n
[EasySMU Webpage](http://www.linear.com/solutions/7943 "EasySMU Webpage") \n
*/
class EasySMU

{
  private:

  public:
    int8_t present_;                     //!< Indicates if this channel is present. (Useful when the EasySMU class is stored in an array where not every possible channel is populated.)
    uint8_t EasySMU_EEPROM_I2C_address_; //!< I2C address of the EEPROM that stores calibration data
    uint8_t DAC_I2C_address_;            //!< I2C address of LTC2655 used to control voltage and current settings
    uint8_t ADC_Vsense_I2C_address_;     //!< I2C address of LTC2485 used to measure output voltage
    uint8_t ADC_Isense_I2C_address_;     //!< I2C address of LTC2485 used to measure output current

    int32_t Vadc_code_,                  //!< raw code of voltage ADC
            Iadc_code_;                  //!< raw code of current ADC

    float flt_measured_voltage_,         //!< measured voltage (in volts)
          flt_measured_current_;         //!< measured current (in amps)

    float flt_temperature_of_Vadc_,                  //!< Temperature of voltage ADC
          flt_temperature_of_Iadc_;                  //!< current output setting (float)

    float float_current_source_setting_; //!< Current source setting. (The output does not change immediately upon updating this value. It must be "committed" first.)

    uint16_t Vdac_code_,                 //!< raw code of voltage DAC
             Idac_pullup_code_,          //!< raw code of current pullup DAC
             Idac_pulldown_code_;        //!< raw code of current pulldown DAC

    void ResetCalibration();             //!< Reset the calibration to a typical value. Rarely used, as it is not factory calibration, but instead, a typical value.
    int8_t SetCalibration();             //!< Perform calibration. Configures various voltage or current settings and prompts for value measured from accurate lab-grade multimeter.

    //! Configure a voltage and current setting and prompt for measurement from lab-grade multimeter. Only used during calibration procedures.
    //! @return Presently, only returns 0
    int8_t SetCalibrationSinglePoint(uint16_t voltageSetValue, //!< code to load into voltage DAC
                                     uint16_t currentSetValue, //!< code to load into current DAC
                                     const char prompt[], //!< prompt to user during calibration.
                                     float *fltUserOutput, //!< value entered by user
                                     int32_t *lngVadc_code, //!< code read from voltage ADC
                                     int32_t *lngIadc_code //!< code read from current ADC
                                    );

    //! Check if this EasySMU is present. (Useful when the EasySMU class is stored in an array where not every possible channel is populated.)
    //! @return 1 if present. 0 if not present.
    int16_t IsPresent();

    //! Read calibration from EEPROM. This normally runs during power-up to restore all offset, LSB size, etc. calibration values.
    //! @return 0=success, 1=failure
    int16_t ReadCalibration();

    //! Write calibration to EEPROM. This is normally run once at the factory after calibration is performed.
    //! @return 0=success, 1=failure
    int16_t WriteCalibration();

    //! FOR FACTORY USE ONLY! Write calibration to factory calibration section of EEPROM.
    //! When the EasySMU leaves the factory, the "calibration" and "factory calibration" sections of the EEPROM are the same. This allows factory calibration to be restored in case the user recalibrates the EasySMU.
    //! @return 0=success, 1=failure
    int16_t WriteFactoryCalibration();

    //! Write factory calibration info to EEPROM. This is the string returned by the *IDN? serial command.
    //! @return 0=success, 1=failure
    int16_t WriteFactoryCalibrationInfo(char *buffer //!< string to be written into the calibration information section of the EEPROM. This is an information string, not calibration data. It is returned by the *IDN? serial command.
                                       );

    //! Prints the factory calibration info from EEPROM to the serial port. This is the string returned by the *IDN? serial command.
    //! @return 0=success, 1=failure
    int16_t PrintFactoryCalibrationInfo();

    //! Restore factory calibration from the EEPROM. This is used in case the user has overwritten the calibration.
    //! @return 0=success, 1=failure
    uint8_t RestoreFactoryCalibration();

    //! Commit the voltage source setting. The DAC that sets the output voltage is loaded with the value in Vdac_code_, and the output changes immediately.
    //! (Other functions such as codeStepVoltageSourceSetting, etc. update the screen but do not actually change the output.)
    //! @return 0=success, 1=failure
    int8_t CommitVoltageSourceSetting();

    //! Set and commit the voltage source setting immediately.
    //! The DAC that sets the output voltage is loaded with a raw code and the output voltage changes immediately.
    //! (Vdac_code_ holds a copy of the raw code.)
    //! @return 0=success, 1=failure
    int8_t codeSetCommitVoltageSource(int32_t lngVoltageSetParam //!< the code written to the voltage DAC (and Vdac_code_)
                                     );
    //! Increase or decrease the voltage source setting. Updates the screen but does not actually change the output. This gives the user time to see (and optionally modify) the value before it is driven at the output.
    //! (Vdac_code_ is modified but not written to DAC yet.)
    //! @return none
    void codeStepVoltageSourceSetting(int16_t stepSize //!< increment the voltage DAC by this amount (raw code of step size)
                                     );

    //! Commit the voltage source setting immediately to this float value.
    //! The DAC that sets the output voltage is loaded with a code based on the scaled version of fVoltage and the output changes immediately.
    //! (Idac_pullup_code_ and Idac_pulldown_code_ are updated also. Note that other functions such as codeStepCurrentSourceSetting, etc. update the screen but do not actually change the output.)
    //! @return 0=success, 1=failure
    int8_t fltSetCommitVoltageSource(float fVoltage //!< the voltage to driven at the output
                                    );
    //! Read the voltage source setting
    //! @return voltage source setting in volts
    float fltReadVoltageSourceSetting();

    //! Commit the current source setting. The DACs that set pullup and pulldown current are loaded with the values from Idac_pullup_code_ and Idac_pulldown_code_, and the output changes immediately.
    //! (Other functions such as codeStepCurrentSourceSetting, etc. update the screen but do not actually change the output.)
    //! @return 0=success, 1=failure
    int8_t CommitCurrentSourceSetting();

    //! Change the current source setting immediately.
    //! The DAC that sets the output current is loaded with a code based on the scaled version of fCurrent and the output changes immediately.
    //! (Idac_pullup_code_ and Idac_pulldown_code_ are updated also. Note that other functions such as codeStepCurrentSourceSetting, etc. update the screen but do not actually change the output.)
    //! @return 0=success, 1=failure
    int8_t fltSetCommitCurrentSource(float fCurrent, //<! current in amps to be used as the current source/sink setting
                                     int8_t up_down_both //!< determines whether output sources current, sinks current, or both. Set to _SOURCE_ONLY, _SOURCE_AND_SINK, or _SINK_ONLY
                                    );

    //! Increase or decrease the current source setting by this value in amps. Updates the screen but does not actually change the output. This gives the user time to modify the value before it is driven at the output.
    //! @return none
    void fltStepCurrentSourceSetting(float fltStepSize //!< adjust the current source setting by this value in amps
                                    );

    //! Read the current source setting
    //! @return current source setting in amps
    float fltReadCurrentSourceSetting();

    //! Measure the voltage and current (with ADCs). Use other member functions to read the value.
    //! @return 0=success, 1=failure
    int8_t MeasureVoltageCurrent();

    //! Measure the voltage (with ADC).
    //! @return voltage in volts

    float MeasureVoltage();
    //! Measure the current (with ADC).
    //! @return current in amps
    float MeasureCurrent();

    //! Measure the temperature of the voltage and current ADCs.
    //! The results are stored in temperature_of_Iadc_code, temperature_of_Vadc_code, flt_temperature_of_Iadc_, and flt_TofVadc_
    //! @return none
    void MeasureTemperatureOfADCs();

    //! Measure the temperature of the voltage and current ADCs.
    //! This will also update flt_temperature_of_Iadc_ and flt_temperature_of_Vadc_
    //! @return none
    void MeasureTemperatureOfADCs(int32_t *temperature_of_Vadc_code, //!< returns the raw code of the voltage ADC temperature reading
                                  int32_t *temperature_of_Iadc_code  //!< returns the raw code of the current ADC temperature reading
                                 );

    //! Enables the LT1970 output. Before this command is run, the output is high-impedance.
    //! @return none
    void EnableOutput();

    //! Disables the LT1970 output. After this command is run, the output is relatively high-impedance.
    //! Note that in this state the output is pulled to 2.76V with a roughly 50kohm impedance.
    //! @return none
    void DisableOutput();

    //! EasySMU constructor. Remember each EasySMU instance has unique I2C addresses, so this configures each I2C address of this instance.
    EasySMU(uint8_t EEPROM_I2C_address_param,   //!< The EasySMU EEPROM I2C address
            uint8_t DAC_I2C_address_param,      //!< The LTC2655 four-channel DAC I2C address. This configures the voltage and current settings.
            uint8_t ADC_Vsense_I2C_address_param, //!< The I2C address of the voltage ADC
            uint8_t ADC_Isense_I2C_address_param  //!< The I2C address of the current ADC
           );

    //! Union that defines the calibration data stored in the EasySMU EEPROM
    union eeprom_data_union

    {
      //! This struct holds all of the calibration data, i.e. offset integers, LSB floats, etc. The eeprom_data_union contains a byte_array that is used to write and read this data to/from the EEPROM.
      struct data_struct_type
      {
        int16_t cal_key; //!< If this is set to the proper value, it indicates that a calibration has been performed. (If not, this EasySMU has never been calibrated.)
        //Consider changing voltage_source_offset to uint16_t. Make sure it is not cast to (int16_t) elsewhere.
        int16_t voltage_source_offset; //!< The output voltage is 0V when this code is stored in the voltage channel of the DAC
        float voltage_source_LSB; //!< the voltage of one LSB in volts (from the voltage channel of the DAC)
        int16_t current_source_pullup_offset; //!< current DAC code that ideally would result in 0A at output, if the output current could be configured below 0.5mA
        float current_source_pullup_LSB; //!< the current of one LSB in amps from the current pullup DAC channel
        int16_t current_source_pulldown_offset; //!< current DAC code that ideally would result in 0A at output, if the output current could be configured below 0.5mA
        float current_source_pulldown_LSB; //!< the current of one LSB in amps from the current pulldown DAC channel
        int32_t voltage_measure_offset; //!< the voltage ADC code that corresponds to 0V at the output
        float voltage_measure_LSB; //!< the voltage of one LSB in volts from the voltage ADC
        float current_measure_LSB; //!< the current of one LSB in amps from the current ADC
        float current_measure_output_resistance; //!< This value is used to modify the output current measurement result based on output voltage. This compensates for the current that flows through the feedback network, etc.
        int32_t current_measure_offset; //!< the code from the current ADC that corresponds to 0A at the output
        int32_t temperature_Vadc_code; //!< This is the temperature code read from the voltage ADC during calibration. (It is probably 25C, but an actual measurement/calibration is not performed.)
        int32_t temperature_Iadc_code; //!< This is the temperature code read from the current ADC during calibration. (It is probably 25C, but an actual measurement/calibration is not performed.)

      } calibration;

      char byte_array[sizeof(data_struct_type)];

    } eeprom;

};

#endif

/*!
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
