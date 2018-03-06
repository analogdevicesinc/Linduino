/*!
EasySMU: I2C Address Translator Demonstration and a Simple Multi-Channel Source Measurement Unit

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


/*! @ingroup Example_Designs
@{
@defgroup EasySMU EasySMU: I2C Address Translator Demonstration and a Simple Multi-Channel Source Measurement Unit
@}
*/

/*! @file
    @ingroup EasySMU
    EasySMU Class: I2C Address Translator Demonstration and a Simple Multi-Channel Source Measurement Unit
*/

#include "EasySMU.h"
#include "EEPROM.h"
#include "UserInterface.h"

//! Maximum time allowed for LTC2485 conversion. If this time is exceeded, something is wrong.
#define LTC2485_TIMEOUT (_LTC2485_CONVERSION_TIME)

EasySMU::EasySMU(uint8_t EEPROM_I2C_address_param, uint8_t DAC_I2C_address_param, uint8_t ADC_Vsense_I2C_address_param, uint8_t ADC_Isense_I2C_address_param)
{
  EasySMU_EEPROM_I2C_address_=EEPROM_I2C_address_param;
  DAC_I2C_address_=DAC_I2C_address_param;
  ADC_Vsense_I2C_address_=ADC_Vsense_I2C_address_param;
  ADC_Isense_I2C_address_=ADC_Isense_I2C_address_param;
}


int8_t EasySMU::CommitVoltageSourceSetting()    //Set Voltage
{
  int8_t ack;

  ack = LTC2655_write(DAC_I2C_address_, LTC2655_CMD_WRITE_UPDATE, _EASYSMU_VOLTAGE_DAC, Vdac_code_);

  return (ack);
}

int8_t EasySMU::codeSetCommitVoltageSource(int32_t code_voltage_setting)
{
  if (code_voltage_setting<0)
  {
    Vdac_code_=0;
  }
  else if (code_voltage_setting>0xFFFF)
  {
    Vdac_code_=0xFFFF;
  }
  else
  {
    Vdac_code_=code_voltage_setting;
  }

  return CommitVoltageSourceSetting();
}

void EasySMU::codeStepVoltageSourceSetting(int16_t step_size)
{
  int32_t code_voltage_setting = Vdac_code_-step_size;
  Vdac_code_ = (code_voltage_setting < 0x000) ? 0x0000 :
               ((code_voltage_setting > 0xFFFF) ? 0xFFFF : (uint16_t) code_voltage_setting);
}

int8_t EasySMU::fltSetCommitVoltageSource(float flt_voltage_setting) //Later make this a float version of voltageSet with operators, etc.
{
  int32_t code_voltage_setting = eeprom.calibration.voltage_source_offset-round(flt_voltage_setting/eeprom.calibration.voltage_source_LSB);

  return codeSetCommitVoltageSource(code_voltage_setting); //voltageSet handles the <0 and >0xFFFF conditions
}

float EasySMU::fltReadVoltageSourceSetting()    //Later make this a float version of voltageSet with operators, etc.
{
  return  ( ((float)((int32_t)Vdac_code_-(int32_t)round(eeprom.calibration.voltage_source_offset))*(-1)*eeprom.calibration.voltage_source_LSB ));
}


int8_t EasySMU::CommitCurrentSourceSetting()
{
  int8_t ack;
  ack = LTC2655_write(DAC_I2C_address_, LTC2655_CMD_WRITE_UPDATE, _EASYSMU_CURRENT_PULLUP_DAC, Idac_pullup_code_);
  ack |= LTC2655_write(DAC_I2C_address_, LTC2655_CMD_WRITE_UPDATE, _EASYSMU_CURRENT_PULLDOWN_DAC, Idac_pulldown_code_);
  return (ack);
}

#define float_current_source_setting_equation(Idac_pullup_code,current_source_pullup_offset,current_source_pullup_LSB) (((int32_t)Idac_pullup_code_ + (int32_t)current_source_pullup_offset) * current_source_pullup_LSB)

int8_t EasySMU::fltSetCommitCurrentSource(float flt_current_setting, int8_t source_both_sink)
{
  int32_t code_current_pullup_setting = (flt_current_setting/eeprom.calibration.current_source_pullup_LSB)-eeprom.calibration.current_source_pullup_offset;

  if (code_current_pullup_setting<0)
  {
    Idac_pullup_code_=0;
  }
  else if (code_current_pullup_setting>0xFFFF)
  {
    Idac_pullup_code_=0xFFFF;
  }
  else
  {
    Idac_pullup_code_=code_current_pullup_setting;
  }

  int32_t code_current_pulldown_setting = (flt_current_setting/eeprom.calibration.current_source_pulldown_LSB)-eeprom.calibration.current_source_pulldown_offset;

  if (code_current_pulldown_setting<0)
  {
    Idac_pulldown_code_=0;
  }
  else if (code_current_pulldown_setting>0xFFFF)
  {
    Idac_pulldown_code_=0xFFFF;
  }
  else
  {
    Idac_pulldown_code_=code_current_pulldown_setting;
  }
  //Used the #defined equation above because this is also used elsewhere in the code. Without the above equation definition, later mistaken changes to the code might affect one but not the other.
  //float_current_source_setting_ = (Idac_pullup_code_ + eeprom.calibration.current_source_pullup_offset) * eeprom.calibration.current_source_pullup_LSB;
  float_current_source_setting_ = float_current_source_setting_equation(Idac_pullup_code_,eeprom.calibration.current_source_pullup_offset,eeprom.calibration.current_source_pullup_LSB);
  if (float_current_source_setting_<0)
  {
    float_current_source_setting_=0;
  }
  if (source_both_sink==_SINK_ONLY)
  {
    Idac_pullup_code_=0;
  }
  else if (source_both_sink==_SOURCE_ONLY)
  {
    Idac_pulldown_code_=0;
  }
  return CommitCurrentSourceSetting();

}

void EasySMU::fltStepCurrentSourceSetting(float flt_step_size)
{

  //Consider removing the following step_size test. It could unnecessarily use extra memory.
  //If the stepsize results in less than 1LSB of change, make it exactly 1LSB.
  if (abs(flt_step_size/eeprom.calibration.current_source_pullup_LSB) < 1)
  {
    if (flt_step_size > 0)
    {
      flt_step_size=1.2*eeprom.calibration.current_source_pullup_LSB;
    }
    else
    {
      flt_step_size=-1.2*eeprom.calibration.current_source_pullup_LSB;
    }
  }

  //Hardcoding a 0.6mA minimum current setting to avoid confusion between mismatched pullup and pulldown minimum settings.
  if ((float_current_source_setting_+flt_step_size)<0.0006)
  {
    flt_step_size=0.0006-float_current_source_setting_;
  }

  int32_t code_current_pullup_setting= ((float_current_source_setting_+flt_step_size) / eeprom.calibration.current_source_pullup_LSB)-eeprom.calibration.current_source_pullup_offset;
  int32_t code_current_pulldown_setting = ((float_current_source_setting_+flt_step_size) / eeprom.calibration.current_source_pulldown_LSB)-eeprom.calibration.current_source_pulldown_offset;
  int32_t code_delta_pullup_pulldown_setting=code_current_pulldown_setting-code_current_pullup_setting;

  if ((code_current_pullup_setting < 0x000) || (code_current_pulldown_setting < code_delta_pullup_pulldown_setting))
  {
    Idac_pullup_code_ = 0x0000;
    Idac_pulldown_code_=code_delta_pullup_pulldown_setting;
  }
  else if ((code_current_pullup_setting > (0xFFFF-code_delta_pullup_pulldown_setting)) || (code_current_pulldown_setting > 0xFFFF))
  {
    Idac_pullup_code_ = 0xFFFF-code_delta_pullup_pulldown_setting;
    Idac_pulldown_code_ = 0xFFFF;
  }
  else
  {
    Idac_pullup_code_ = code_current_pullup_setting;
    Idac_pulldown_code_=code_current_pulldown_setting;
  }

  if (((int32_t) Idac_pulldown_code_ + eeprom.calibration.current_source_pulldown_offset) <0 )
  {
    Idac_pulldown_code_ = 0 ;
    Idac_pullup_code_ = eeprom.calibration.current_source_pullup_offset - eeprom.calibration.current_source_pulldown_offset;
  }

  //float_current_source_setting_ = ((int32_t)Idac_pullup_code_ + (int32_t)eeprom.calibration.current_source_pullup_offset) * eeprom.calibration.current_source_pullup_LSB;
  float_current_source_setting_ = float_current_source_setting_equation(Idac_pullup_code_,eeprom.calibration.current_source_pullup_offset,eeprom.calibration.current_source_pullup_LSB);

}

float EasySMU::fltReadCurrentSourceSetting()
{

  return float_current_source_setting_;

}

int8_t EasySMU::MeasureVoltageCurrent()
{

  int8_t ack=0;
  ack  = LTC2485_read(ADC_Isense_I2C_address_, LTC2485_SPEED_1X | LTC2485_R60, &Iadc_code_, LTC2485_TIMEOUT);
  ack |= LTC2485_read(ADC_Vsense_I2C_address_, LTC2485_SPEED_1X | LTC2485_R60, &Vadc_code_, LTC2485_TIMEOUT);

  flt_measured_current_ = (float)(Iadc_code_+((Vadc_code_*eeprom.calibration.current_measure_output_resistance)-eeprom.calibration.current_measure_offset))*eeprom.calibration.current_measure_LSB; //ireadLSB is Amps/bit
  flt_measured_voltage_ = (float)Vadc_code_*eeprom.calibration.voltage_measure_LSB - (float)eeprom.calibration.voltage_measure_offset*eeprom.calibration.voltage_measure_LSB;
  return (ack);
}

void EasySMU::MeasureTemperatureOfADCs()
{
  delay(1);
  LTC2485_read(ADC_Isense_I2C_address_, LTC2485_INTERNAL_TEMP | LTC2485_SPEED_1X | LTC2485_R60, &Iadc_code_, LTC2485_TIMEOUT); //Setup temperature mode for next reading
  LTC2485_read(ADC_Vsense_I2C_address_, LTC2485_INTERNAL_TEMP | LTC2485_SPEED_1X | LTC2485_R60, &Vadc_code_, LTC2485_TIMEOUT); //Setup temperature mode for next reading
  //Restore voltage and current settings for next read from ADC, and read the previous temperature information.
  int32_t temperature_of_Iadc_code, temperature_of_Vadc_code;

  LTC2485_read(ADC_Isense_I2C_address_, LTC2485_SPEED_1X | LTC2485_R60, &temperature_of_Iadc_code, _LTC2485_CONVERSION_TIME_1X*2); //Forcing 1X conversion time since 2X is not possible for temperature reading.
  LTC2485_read(ADC_Vsense_I2C_address_, LTC2485_SPEED_1X | LTC2485_R60, &temperature_of_Vadc_code, _LTC2485_CONVERSION_TIME_1X*2);

  flt_temperature_of_Iadc_=((float)(uint32_t) temperature_of_Iadc_code)/(uint32_t) eeprom.calibration.temperature_Iadc_code * (25+273) - 273;
  flt_temperature_of_Vadc_=((float)(uint32_t) temperature_of_Vadc_code)/(uint32_t) eeprom.calibration.temperature_Vadc_code * (25+273) - 273;
}

void EasySMU::MeasureTemperatureOfADCs(int32_t *temperature_of_Vadc_code, int32_t *temperature_of_Iadc_code)
{
  delay(1);
  LTC2485_read(ADC_Isense_I2C_address_, LTC2485_INTERNAL_TEMP | LTC2485_SPEED_1X | LTC2485_R60, &Iadc_code_, LTC2485_TIMEOUT); //Setup temperature mode for next reading
  LTC2485_read(ADC_Vsense_I2C_address_, LTC2485_INTERNAL_TEMP | LTC2485_SPEED_1X | LTC2485_R60, &Vadc_code_, LTC2485_TIMEOUT); //Setup temperature mode for next reading
  //Restore voltage and current settings for next read from ADC, and read the previous temperature information.

  LTC2485_read(ADC_Isense_I2C_address_, LTC2485_SPEED_1X | LTC2485_R60, *(&temperature_of_Iadc_code), _LTC2485_CONVERSION_TIME_1X*2); //Forcing 1X conversion time since 2X is not possible for temperature reading.
  LTC2485_read(ADC_Vsense_I2C_address_, LTC2485_SPEED_1X | LTC2485_R60, *(&temperature_of_Vadc_code), _LTC2485_CONVERSION_TIME_1X*2);

  flt_temperature_of_Iadc_=((float)(uint32_t) temperature_of_Iadc_code)/(uint32_t) eeprom.calibration.temperature_Iadc_code * (25+273) - 273;
  flt_temperature_of_Vadc_=((float)(uint32_t) temperature_of_Vadc_code)/(uint32_t) eeprom.calibration.temperature_Vadc_code * (25+273) - 273;
}

float EasySMU::MeasureCurrent()
{

  LTC2485_read(ADC_Isense_I2C_address_, LTC2485_SPEED_1X | LTC2485_R60, &Iadc_code_, LTC2485_TIMEOUT);
  LTC2485_read(ADC_Vsense_I2C_address_, LTC2485_SPEED_1X | LTC2485_R60, &Vadc_code_, LTC2485_TIMEOUT);

  return (float)(Iadc_code_+(int32_t)((Vadc_code_*eeprom.calibration.current_measure_output_resistance)-eeprom.calibration.current_measure_offset))*eeprom.calibration.current_measure_LSB; //ireadLSB is Amps/bit

}

float EasySMU::MeasureVoltage()  //Remove _fVout if not needed.
{

  LTC2485_read(ADC_Vsense_I2C_address_, LTC2485_SPEED_1X | LTC2485_R60, &Vadc_code_, LTC2485_TIMEOUT);

  return (float)(Vadc_code_*eeprom.calibration.voltage_measure_LSB) - (eeprom.calibration.voltage_measure_offset*eeprom.calibration.voltage_measure_LSB);

}

void EasySMU::EnableOutput()
{
  LTC2655_write(DAC_I2C_address_, LTC2655_CMD_WRITE_UPDATE, _EASYSMU_ENABLE_OUTPUT_DAC, 0xFFFF);
}

void EasySMU::DisableOutput()
{
  LTC2655_write(DAC_I2C_address_, LTC2655_CMD_WRITE_UPDATE, _EASYSMU_ENABLE_OUTPUT_DAC, 0x0000);
}

//Reset the calibration to a typical value. This will rarely be used, as it is not factory calibration, but instead, a typical value.
void EasySMU::ResetCalibration()
{


  eeprom.calibration.cal_key=0;
  eeprom.calibration.voltage_source_offset=29801;
  eeprom.calibration.voltage_source_LSB=-0.0006852983;
  eeprom.calibration.current_source_pullup_offset=324;
  eeprom.calibration.current_source_pullup_LSB=6.273333E-07;
  eeprom.calibration.current_source_pulldown_offset=-262;
  eeprom.calibration.current_source_pulldown_LSB=6.226666E-07;
  eeprom.calibration.voltage_measure_offset=-1525982;
  eeprom.calibration.voltage_measure_LSB=2.158354E-06;
  eeprom.calibration.current_measure_LSB=-6.538718E-09;
  eeprom.calibration.current_measure_output_resistance=0.008131025;
  eeprom.calibration.current_measure_offset=-2420;
  eeprom.calibration.temperature_Vadc_code=2272596;
  eeprom.calibration.temperature_Iadc_code=2272596;

}

int8_t EasySMU::SetCalibrationSinglePoint(uint16_t code_voltage_setting, uint16_t code_current_setting, const char prompt[], float *flt_user_input, int32_t *code_voltage_measure, int32_t *code_current_measure)
{
  codeSetCommitVoltageSource(code_voltage_setting);
  Idac_pullup_code_=code_current_setting;
  Idac_pulldown_code_=code_current_setting;
  CommitCurrentSourceSetting();
  if (prompt[0]!='\0')
  {
    Serial.print(prompt);
    *flt_user_input=read_float();
    Serial.println(*flt_user_input,6);
  }
  delay(200);

  MeasureVoltageCurrent();
  MeasureVoltageCurrent();
  *code_voltage_measure = Vadc_code_;
  *code_current_measure = Iadc_code_;

  return (0); //Later allow a return failed condition placed on user input?
}

int8_t EasySMU::SetCalibration()
{
  float fltVsetP5V, fltVsetM5V;
  int32_t lngVoutP5V, lngVoutM5V;

  Serial.println(F("Connect to voltmeter and wait ten seconds."));

  //Force about +5V and -5V to determine LSB.  Voltage offset will be determined in subsequent step.
  const uint16_t _VSET_P5V=44337, _VSET_M5V=15234;
  const uint16_t _ISET=48000;
  codeSetCommitVoltageSource(_VSET_P5V);  //Set conditions and let temperature settle for 10 seconds.
  Idac_pullup_code_=_ISET;
  Idac_pulldown_code_=_ISET;
  CommitCurrentSourceSetting();

  delay(10000);
  int32_t temperature_Vadc_code, temperature_Iadc_code;
  MeasureTemperatureOfADCs(&temperature_Vadc_code, &temperature_Iadc_code);
  eeprom.calibration.temperature_Vadc_code=temperature_Vadc_code;
  eeprom.calibration.temperature_Iadc_code=temperature_Iadc_code;



  //Force +5V output and prompt user for value measured by voltmeter.
  int32_t lngThrowaway; //This is a throwaway.
  SetCalibrationSinglePoint(_VSET_P5V,_ISET, "(V)? ", &fltVsetP5V, &lngVoutP5V, &lngThrowaway);

  //Force -5V output and prompt user for value measured by voltmeter.
  SetCalibrationSinglePoint(_VSET_M5V,_ISET, "(V)? ", &fltVsetM5V, &lngVoutM5V, &lngThrowaway);

  //Remember fltVsetP5V and fltVsetM5V were the accurate values entered by the user.
  eeprom.calibration.voltage_measure_LSB=(fltVsetM5V-fltVsetP5V)/(float)(lngVoutM5V-lngVoutP5V);

  //Calculate Read and Set LSB's from -5V and +5V readings.
  eeprom.calibration.voltage_source_LSB=(fltVsetM5V-fltVsetP5V)/(_VSET_P5V-_VSET_M5V);

  //Estimate 0V offset from the above +5V and -5V readings.  But calculate more accurately in subsequent step.
  eeprom.calibration.voltage_source_offset=(int16_t) ((fltVsetP5V/eeprom.calibration.voltage_source_LSB)+_VSET_P5V);
  Serial.print(eeprom.calibration.voltage_source_offset);

  //Now calculate 0V offset more accurately.
  float fltVoff;
  int32_t lngVoutOff;
  SetCalibrationSinglePoint(eeprom.calibration.voltage_source_offset,_ISET, "(mV)? ", &fltVoff, &lngVoutOff, &lngThrowaway);
  fltVoff=fltVoff/1000;
  //Serial.print((int16_t) round(fltVoff/eeprom.calibration.voltage_source_LSB));

  eeprom.calibration.voltage_measure_offset=lngVoutOff-(int32_t) (fltVoff/eeprom.calibration.voltage_measure_LSB);
  //Serial.print("VMeasure Offset: ");
  //Serial.println(eeprom.calibration.voltage_measure_offset);
  //delay(100);
  //DJE: The output voltage accuracy could be improved by storing the actual floating point value where the code transition near 0V occurs.
  //Then, the code value could be determined to within 1/2LSB.
  eeprom.calibration.voltage_source_offset=(int16_t) (round(fltVoff/eeprom.calibration.voltage_source_LSB)+Vdac_code_);
  //Serial.print(eeprom.calibration.voltage_source_offset);
  codeSetCommitVoltageSource(eeprom.calibration.voltage_source_offset);

  //Remove remaining offset from voltage measurement. If there was sub-millivolt drift in source voltage, it still affects the offset cancellation based on user input.
  float fltTemp;
  //SetCalibrationSinglePoint(eeprom.calibration.voltage_source_offset, 0, "Short outputs. Press Enter.", &fltTemp, &lngVoutOff, &lngThrowaway);
  //eeprom.calibration.voltage_measure_offset=lngVoutOff;
  //Serial.println(lngVoutOff);

  Serial.println(F("Disconnect meter. Press ENTER"));

  read_float(); //Just waiting for Enter.

  //float fltTemp;
  int32_t lngIoutP5;
  SetCalibrationSinglePoint(_VSET_P5V,_ISET, "", &fltTemp, &lngThrowaway, &lngIoutP5);
  //Serial.print("+Iout: ");
  //Serial.println(lngIoutP5);
  //delay(100);

  int32_t lngIoutM5;
  SetCalibrationSinglePoint(_VSET_M5V,_ISET, "", &fltTemp, &lngThrowaway, &lngIoutM5);
  //Serial.print("-Iout: ");
  //Serial.println(lngIoutM5);
  //delay(100);

  float Rout;
  Rout=-1*(lngIoutM5-lngIoutP5)/(float)(lngVoutM5V-lngVoutP5V);
  eeprom.calibration.current_measure_output_resistance=Rout;
  //Serial.print("Rout: ");
  //Serial.println(Rout,16);
  //delay(100);

  int32_t lngIout0V, lngVout0V;
  SetCalibrationSinglePoint(eeprom.calibration.voltage_source_offset, _ISET, "", &fltTemp, &lngVout0V, &lngIout0V);
  //Serial.print("lngVout0V: ");
  //Serial.println(lngVout0V);
  //Serial.print("lngIout0V: ");
  //Serial.print(lngIout0V);
  //delay(100);

  eeprom.calibration.current_measure_offset=eeprom.calibration.current_measure_output_resistance*lngVout0V+lngIout0V;
  //Serial.print("Ioffset: ");
  //Serial.println(eeprom.calibration.current_measure_offset);
  //delay(100);

  uint16_t _ISET_8=48000;
  int32_t lngIout8, lngVout8;
  float fltIout8;
  SetCalibrationSinglePoint(_VSET_P5V, _ISET_8, "(mA)?", &fltIout8, &lngVout8, &lngIout8);
  //Serial.print("+Iout: ");
  //Serial.println(lngIout8);
  //delay(100);

  fltIout8=fltIout8/1000;

  uint16_t _ISET_9=48000;
  int32_t lngIout9, lngVout9;
  float fltIout9;
  SetCalibrationSinglePoint(_VSET_M5V, _ISET_9, "(mA)?", &fltIout9, &lngVout9, &lngIout9);
  //Serial.print("-Iout: ");
  //Serial.println(lngIout9);
  //delay(100);

  fltIout9=fltIout9/1000;

  eeprom.calibration.current_measure_LSB=(fltIout8-fltIout9)/((lngIout8-lngIout9)+(int32_t)(eeprom.calibration.current_measure_output_resistance*(lngVout8-lngVout9)));
  //Serial.print("ILSB: ");
  //Serial.println(eeprom.calibration.current_measure_LSB, 16);
  //delay(100);

  //Calibrate Iset offset and LSB

//  uint16_t _ISET_8=20000;
  _ISET_8=20000;
  //int32_t lngIout8;
  //float fltIout8;
  SetCalibrationSinglePoint(_VSET_P5V, _ISET_8, "(mA)?", &fltIout8, &lngThrowaway, &lngIout8);

  fltIout8=fltIout8/1000;

//  uint16_t _ISET_9=5000;
  _ISET_9=5000;
  //int32_t lngIout9;
  //float fltIout9;
  SetCalibrationSinglePoint(_VSET_P5V, _ISET_9, "(mA)?", &fltIout9, &lngThrowaway, &lngIout9);
  fltIout9=fltIout9/1000;

  eeprom.calibration.current_source_pullup_LSB=(fltIout8-fltIout9)/(_ISET_8-_ISET_9);
  eeprom.calibration.current_source_pullup_offset=(int16_t) ((fltIout9/eeprom.calibration.current_source_pullup_LSB)-_ISET_9);

  SetCalibrationSinglePoint(_VSET_M5V, _ISET_8, "(mA)?", &fltIout8, &lngThrowaway, &lngIout8);
  fltIout8=-1*fltIout8/1000;

  SetCalibrationSinglePoint(_VSET_M5V, _ISET_9, "(mA)?", &fltIout9, &lngThrowaway, &lngIout9);
  fltIout9=-1*fltIout9/1000;

  eeprom.calibration.current_source_pulldown_LSB=(fltIout8-fltIout9)/(_ISET_8-_ISET_9);
  eeprom.calibration.current_source_pulldown_offset=(int16_t) ((fltIout9/eeprom.calibration.current_source_pulldown_LSB)-_ISET_9);

  eeprom.calibration.current_source_pulldown_LSB=(fltIout8-fltIout9)/(_ISET_8-_ISET_9);
  eeprom.calibration.current_source_pulldown_offset=(int) ((fltIout9/eeprom.calibration.current_source_pulldown_LSB)-_ISET_9);

  eeprom.calibration.cal_key=EEPROM_CAL_KEY;

  Serial.print(F("Save calibration? (Y/N)"));
  char user_char=read_char();
  if (user_char!='y' || user_char!='Y')
  {
    if (user_char=='L' && read_char() == 'T' && read_char()=='C')
    {
      char buffer[CAL_INFO_STRLEN];
      WriteFactoryCalibration();
      Serial.println(F("Info: "));
      Serial.setTimeout(30000); //Allow thirty seconds for response
      size_t numbytes = Serial.readBytesUntil('\r', buffer, CAL_INFO_STRLEN-1); //Need to leave one last character for the null termination.

      buffer[numbytes] = '\0'; //readBytesUntil stored the <CR> terminates the string. This removes it.

      WriteFactoryCalibrationInfo(buffer);
      Serial.println(buffer);
      Serial.setTimeout(1000); //Restore 1 second timeout default
    }
    else
    {
      return false;
    }
  }

  WriteCalibration();

  codeSetCommitVoltageSource(eeprom.calibration.voltage_source_offset);

  while (Serial.peek()=='\n' || Serial.peek()=='\r') Serial.read();

  Serial.println(F("Written"));
  return true;

}

int16_t EasySMU::ReadCalibration()
{
  int16_t cal_key;

  if (eeprom_poll(EasySMU_EEPROM_I2C_address_))
  {
    present_=0;
    return (1);
  }

  present_=1;

  // Read the cal key from the EEPROM

  eeprom_read_int16(EasySMU_EEPROM_I2C_address_, &cal_key, EEPROM_CAL_STATUS_ADDRESS);

  if (cal_key == EEPROM_CAL_KEY)
  {
    eeprom_read_byte_array(EasySMU_EEPROM_I2C_address_, eeprom.byte_array, EEPROM_CAL_STATUS_ADDRESS, sizeof(eeprom_data_union));
    return (0);
  }
  //printError(_PRINT_ERROR_NO_CALIBRATION_DATA_FOUND);
  ResetCalibration(); //If a valid calibration were present in the EEPROM, this function would already have hit return(0); Instead read a default value.
  return (1);

}

int16_t EasySMU::WriteCalibration()
{

  eeprom.calibration.cal_key = EEPROM_CAL_KEY;
  //Unfortunately, the eeprom_write_byte_array takes an 8-bit address rather than 7-bit
  return (eeprom_write_byte_array(EasySMU_EEPROM_I2C_address_, eeprom.byte_array, EEPROM_CAL_STATUS_ADDRESS, sizeof(eeprom_data_union)));
}

int16_t EasySMU::WriteFactoryCalibration()
{

  eeprom.calibration.cal_key = EEPROM_CAL_KEY;
  //Unfortunately, the eeprom_write_byte_array takes an 8-bit address rather than 7-bit
  return (eeprom_write_byte_array(EasySMU_EEPROM_I2C_address_, eeprom.byte_array, EEPROM_FACTORY_CAL_STATUS_ADDRESS, sizeof(eeprom_data_union)));
}

int16_t EasySMU::PrintFactoryCalibrationInfo()
{
  int16_t cal_key;

  if (eeprom_poll(EasySMU_EEPROM_I2C_address_))
  {
    return (1);
  }

  char calInfo[CAL_INFO_STRLEN];
  eeprom_read_byte_array(EasySMU_EEPROM_I2C_address_, calInfo, EEPROM_FACTORY_CAL_INFO_ADDRESS, CAL_INFO_STRLEN);
  Serial.print(calInfo);

  return (0);
}

int16_t EasySMU::WriteFactoryCalibrationInfo(char *buffer)
{
  int16_t cal_key;

  if (eeprom_poll(EasySMU_EEPROM_I2C_address_))
  {
    return (1);
  }

  eeprom_write_byte_array(EasySMU_EEPROM_I2C_address_, buffer, EEPROM_FACTORY_CAL_INFO_ADDRESS, CAL_INFO_STRLEN);
  Serial.println(buffer);

  return (0);
}

uint8_t EasySMU::RestoreFactoryCalibration()
{

  uint8_t num_bytes=sizeof(eeprom_data_union);
  uint8_t i;
  char data[1];
  for (i = 0; i < num_bytes; i++)
  {
    if (eeprom_read_byte(EasySMU_EEPROM_I2C_address_, data, EEPROM_FACTORY_CAL_STATUS_ADDRESS+i) != 1)
    {
      //printError(_PRINT_ERROR_FACTORY_CALIBRATION_RESTORE_FAILED_CATASTROPHICALLY);
      break;
    }
    if (eeprom_write_byte(EasySMU_EEPROM_I2C_address_, data[0], EEPROM_CAL_STATUS_ADDRESS+i) != 1)
    {
      //printError(_PRINT_ERROR_FACTORY_CALIBRATION_RESTORE_FAILED_CATASTROPHICALLY);
      break;
    }
  }
  //Serial.println(F("RESTORED"));
  return (i);

}

int16_t EasySMU::IsPresent()
{
  return (present_);
}


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