/*!
LTC2983: Multi-Sensor High Accuracy Digital Temperature Measurement System.

@verbatim

configuration_constants_LTC2983.h:
The configuration constants used to configure the LTC2983.
@endverbatim

http://www.linear.com/product/LTC2983

http://www.linear.com/product/LTC2983#demoboards

$Revision: 5790 $
$Date: 2016-09-23 09:58:16 -0700 (Fri, 23 Sep 2016) $
Copyright (c) 2014, Linear Technology Corp.(LTC)
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
    @ingroup LTC2983
    Header for LTC2983: Multi-Sensor High Accuracy Digital Temperature Measurement System
*/


//**********************************************************************************************************
// -- SENSOR TYPES --
//**********************************************************************************************************
#define SENSOR_TYPE_LSB 27
// RTD
#define SENSOR_TYPE__RTD_PT_10 (uint32_t) 0xA << SENSOR_TYPE_LSB
#define SENSOR_TYPE__RTD_PT_50 (uint32_t) 0xB << SENSOR_TYPE_LSB
#define SENSOR_TYPE__RTD_PT_100 (uint32_t) 0xC << SENSOR_TYPE_LSB
#define SENSOR_TYPE__RTD_PT_200 (uint32_t) 0xD << SENSOR_TYPE_LSB
#define SENSOR_TYPE__RTD_PT_500 (uint32_t) 0xE << SENSOR_TYPE_LSB
#define SENSOR_TYPE__RTD_PT_1000 (uint32_t) 0xF << SENSOR_TYPE_LSB
#define SENSOR_TYPE__RTD_PT_1000_375 (uint32_t) 0x10 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__RTD_NI_120 (uint32_t) 0x11 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__RTD_CUSTOM (uint32_t) 0x12 << SENSOR_TYPE_LSB
// Sense Resistor
#define SENSOR_TYPE__SENSE_RESISTOR (uint32_t) 0x1D << SENSOR_TYPE_LSB
// -
#define SENSOR_TYPE__NONE (uint32_t) 0x0 << SENSOR_TYPE_LSB
// Direct ADC
#define SENSOR_TYPE__DIRECT_ADC (uint32_t) 0x1E << SENSOR_TYPE_LSB
// Thermistor
#define SENSOR_TYPE__THERMISTOR_44004_2P252K_25C (uint32_t) 0x13 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__THERMISTOR_44005_3K_25C (uint32_t) 0x14 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__THERMISTOR_44007_5K_25C (uint32_t) 0x15 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__THERMISTOR_44006_10K_25C (uint32_t) 0x16 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__THERMISTOR_44008_30K_25C (uint32_t) 0x17 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__THERMISTOR_YSI_400_2P252K_25C (uint32_t) 0x18 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__THERMISTOR_1003K_1K_25C (uint32_t) 0x19 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__THERMISTOR_CUSTOM_STEINHART_HART (uint32_t) 0x1A << SENSOR_TYPE_LSB
#define SENSOR_TYPE__THERMISTOR_CUSTOM_TABLE (uint32_t) 0x1B << SENSOR_TYPE_LSB
// Thermocouple
#define SENSOR_TYPE__TYPE_J_THERMOCOUPLE (uint32_t) 0x1 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__TYPE_K_THERMOCOUPLE (uint32_t) 0x2 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__TYPE_E_THERMOCOUPLE (uint32_t) 0x3 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__TYPE_N_THERMOCOUPLE (uint32_t) 0x4 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__TYPE_R_THERMOCOUPLE (uint32_t) 0x5 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__TYPE_S_THERMOCOUPLE (uint32_t) 0x6 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__TYPE_T_THERMOCOUPLE (uint32_t) 0x7 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__TYPE_B_THERMOCOUPLE (uint32_t) 0x8 << SENSOR_TYPE_LSB
#define SENSOR_TYPE__CUSTOM_THERMOCOUPLE (uint32_t) 0x9 << SENSOR_TYPE_LSB
// Off-Chip Diode
#define SENSOR_TYPE__OFF_CHIP_DIODE (uint32_t) 0x1C << SENSOR_TYPE_LSB
//**********************************************************************************************************
// -- RTD --
//**********************************************************************************************************
// rtd - rsense channel
#define RTD_RSENSE_CHANNEL_LSB 22
#define RTD_RSENSE_CHANNEL__NONE (uint32_t) 0x0 << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__1 (uint32_t) 0x1 << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__2 (uint32_t) 0x2 << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__3 (uint32_t) 0x3 << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__4 (uint32_t) 0x4 << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__5 (uint32_t) 0x5 << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__6 (uint32_t) 0x6 << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__7 (uint32_t) 0x7 << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__8 (uint32_t) 0x8 << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__9 (uint32_t) 0x9 << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__10 (uint32_t) 0xA << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__11 (uint32_t) 0xB << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__12 (uint32_t) 0xC << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__13 (uint32_t) 0xD << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__14 (uint32_t) 0xE << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__15 (uint32_t) 0xF << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__16 (uint32_t) 0x10 << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__17 (uint32_t) 0x11 << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__18 (uint32_t) 0x12 << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__19 (uint32_t) 0x13 << RTD_RSENSE_CHANNEL_LSB
#define RTD_RSENSE_CHANNEL__20 (uint32_t) 0x14 << RTD_RSENSE_CHANNEL_LSB
// rtd - num wires
#define RTD_NUM_WIRES_LSB 20
#define RTD_NUM_WIRES__2_WIRE (uint32_t) 0x0 << RTD_NUM_WIRES_LSB
#define RTD_NUM_WIRES__3_WIRE (uint32_t) 0x1 << RTD_NUM_WIRES_LSB
#define RTD_NUM_WIRES__4_WIRE (uint32_t) 0x2 << RTD_NUM_WIRES_LSB
#define RTD_NUM_WIRES__4_WIRE_KELVIN_RSENSE (uint32_t) 0x3 << RTD_NUM_WIRES_LSB
// rtd - excitation mode
#define RTD_EXCITATION_MODE_LSB 18
#define RTD_EXCITATION_MODE__NO_ROTATION_NO_SHARING (uint32_t) 0x0 << RTD_EXCITATION_MODE_LSB
#define RTD_EXCITATION_MODE__NO_ROTATION_SHARING (uint32_t) 0x1 << RTD_EXCITATION_MODE_LSB
#define RTD_EXCITATION_MODE__ROTATION_SHARING (uint32_t) 0x2 << RTD_EXCITATION_MODE_LSB
// rtd - excitation current
#define RTD_EXCITATION_CURRENT_LSB 14
#define RTD_EXCITATION_CURRENT__INVALID (uint32_t) 0x0 << RTD_EXCITATION_CURRENT_LSB
#define RTD_EXCITATION_CURRENT__5UA (uint32_t) 0x1 << RTD_EXCITATION_CURRENT_LSB
#define RTD_EXCITATION_CURRENT__10UA (uint32_t) 0x2 << RTD_EXCITATION_CURRENT_LSB
#define RTD_EXCITATION_CURRENT__25UA (uint32_t) 0x3 << RTD_EXCITATION_CURRENT_LSB
#define RTD_EXCITATION_CURRENT__50UA (uint32_t) 0x4 << RTD_EXCITATION_CURRENT_LSB
#define RTD_EXCITATION_CURRENT__100UA (uint32_t) 0x5 << RTD_EXCITATION_CURRENT_LSB
#define RTD_EXCITATION_CURRENT__250UA (uint32_t) 0x6 << RTD_EXCITATION_CURRENT_LSB
#define RTD_EXCITATION_CURRENT__500UA (uint32_t) 0x7 << RTD_EXCITATION_CURRENT_LSB
#define RTD_EXCITATION_CURRENT__1MA (uint32_t) 0x8 << RTD_EXCITATION_CURRENT_LSB
// rtd - standard
#define RTD_STANDARD_LSB 12
#define RTD_STANDARD__EUROPEAN (uint32_t) 0x0 << RTD_STANDARD_LSB
#define RTD_STANDARD__AMERICAN (uint32_t) 0x1 << RTD_STANDARD_LSB
#define RTD_STANDARD__JAPANESE (uint32_t) 0x2 << RTD_STANDARD_LSB
#define RTD_STANDARD__ITS_90 (uint32_t) 0x3 << RTD_STANDARD_LSB
// rtd - custom address
#define RTD_CUSTOM_ADDRESS_LSB 6
// rtd - custom length-1
#define RTD_CUSTOM_LENGTH_1_LSB 0
// rtd - custom values
#define RTD_CUSTOM_VALUES_LSB 31
//**********************************************************************************************************
// -- Sense Resistor --
//**********************************************************************************************************
// sense resistor - value
#define SENSE_RESISTOR_VALUE_LSB 0
//**********************************************************************************************************
// -- Direct ADC --
//**********************************************************************************************************
// Direct ADC - differential?
#define DIRECT_ADC_DIFFERENTIAL_LSB 26
#define DIRECT_ADC_DIFFERENTIAL (uint32_t) 0x0 << DIRECT_ADC_DIFFERENTIAL_LSB
#define DIRECT_ADC_SINGLE_ENDED (uint32_t) 0x1 << DIRECT_ADC_DIFFERENTIAL_LSB
//**********************************************************************************************************
// -- Thermistor --
//**********************************************************************************************************
// thermistor - rsense channel
#define THERMISTOR_RSENSE_CHANNEL_LSB 22
#define THERMISTOR_RSENSE_CHANNEL__NONE (uint32_t) 0x0 << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__1 (uint32_t) 0x1 << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__2 (uint32_t) 0x2 << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__3 (uint32_t) 0x3 << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__4 (uint32_t) 0x4 << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__5 (uint32_t) 0x5 << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__6 (uint32_t) 0x6 << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__7 (uint32_t) 0x7 << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__8 (uint32_t) 0x8 << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__9 (uint32_t) 0x9 << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__10 (uint32_t) 0xA << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__11 (uint32_t) 0xB << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__12 (uint32_t) 0xC << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__13 (uint32_t) 0xD << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__14 (uint32_t) 0xE << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__15 (uint32_t) 0xF << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__16 (uint32_t) 0x10 << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__17 (uint32_t) 0x11 << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__18 (uint32_t) 0x12 << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__19 (uint32_t) 0x13 << THERMISTOR_RSENSE_CHANNEL_LSB
#define THERMISTOR_RSENSE_CHANNEL__20 (uint32_t) 0x14 << THERMISTOR_RSENSE_CHANNEL_LSB
// thermistor - differential?
#define THERMISTOR_DIFFERENTIAL_LSB 21
#define THERMISTOR_DIFFERENTIAL (uint32_t) 0x0 << THERMISTOR_DIFFERENTIAL_LSB
#define THERMISTOR_SINGLE_ENDED (uint32_t) 0x1 << THERMISTOR_DIFFERENTIAL_LSB
// thermistor - excitation mode
#define THERMISTOR_EXCITATION_MODE_LSB 19
#define THERMISTOR_EXCITATION_MODE__NO_SHARING_NO_ROTATION (uint32_t) 0x0 << THERMISTOR_EXCITATION_MODE_LSB
#define THERMISTOR_EXCITATION_MODE__SHARING_ROTATION (uint32_t) 0x1 << THERMISTOR_EXCITATION_MODE_LSB
#define THERMISTOR_EXCITATION_MODE__SHARING_NO_ROTATION (uint32_t) 0x2 << THERMISTOR_EXCITATION_MODE_LSB
// thermistor - excitation current
#define THERMISTOR_EXCITATION_CURRENT_LSB 15
#define THERMISTOR_EXCITATION_CURRENT__INVALID (uint32_t) 0x0 << THERMISTOR_EXCITATION_CURRENT_LSB
#define THERMISTOR_EXCITATION_CURRENT__250NA (uint32_t) 0x1 << THERMISTOR_EXCITATION_CURRENT_LSB
#define THERMISTOR_EXCITATION_CURRENT__500NA (uint32_t) 0x2 << THERMISTOR_EXCITATION_CURRENT_LSB
#define THERMISTOR_EXCITATION_CURRENT__1UA (uint32_t) 0x3 << THERMISTOR_EXCITATION_CURRENT_LSB
#define THERMISTOR_EXCITATION_CURRENT__5UA (uint32_t) 0x4 << THERMISTOR_EXCITATION_CURRENT_LSB
#define THERMISTOR_EXCITATION_CURRENT__10UA (uint32_t) 0x5 << THERMISTOR_EXCITATION_CURRENT_LSB
#define THERMISTOR_EXCITATION_CURRENT__25UA (uint32_t) 0x6 << THERMISTOR_EXCITATION_CURRENT_LSB
#define THERMISTOR_EXCITATION_CURRENT__50UA (uint32_t) 0x7 << THERMISTOR_EXCITATION_CURRENT_LSB
#define THERMISTOR_EXCITATION_CURRENT__100UA (uint32_t) 0x8 << THERMISTOR_EXCITATION_CURRENT_LSB
#define THERMISTOR_EXCITATION_CURRENT__250UA (uint32_t) 0x9 << THERMISTOR_EXCITATION_CURRENT_LSB
#define THERMISTOR_EXCITATION_CURRENT__500UA (uint32_t) 0xA << THERMISTOR_EXCITATION_CURRENT_LSB
#define THERMISTOR_EXCITATION_CURRENT__1MA (uint32_t) 0xB << THERMISTOR_EXCITATION_CURRENT_LSB
#define THERMISTOR_EXCITATION_CURRENT__AUTORANGE (uint32_t) 0xC << THERMISTOR_EXCITATION_CURRENT_LSB
// thermistor - custom address
#define THERMISTOR_CUSTOM_ADDRESS_LSB 6
// thermistor - custom length-1
#define THERMISTOR_CUSTOM_LENGTH_1_LSB 0
// thermistor - custom values
#define THERMISTOR_CUSTOM_VALUES_LSB 31
//**********************************************************************************************************
// -- Thermocouple --
//**********************************************************************************************************
// tc - cold junction ch
#define TC_COLD_JUNCTION_CH_LSB 22
#define TC_COLD_JUNCTION_CH__NONE (uint32_t) 0x0 << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__1 (uint32_t) 0x1 << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__2 (uint32_t) 0x2 << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__3 (uint32_t) 0x3 << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__4 (uint32_t) 0x4 << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__5 (uint32_t) 0x5 << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__6 (uint32_t) 0x6 << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__7 (uint32_t) 0x7 << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__8 (uint32_t) 0x8 << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__9 (uint32_t) 0x9 << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__10 (uint32_t) 0xA << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__11 (uint32_t) 0xB << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__12 (uint32_t) 0xC << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__13 (uint32_t) 0xD << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__14 (uint32_t) 0xE << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__15 (uint32_t) 0xF << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__16 (uint32_t) 0x10 << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__17 (uint32_t) 0x11 << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__18 (uint32_t) 0x12 << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__19 (uint32_t) 0x13 << TC_COLD_JUNCTION_CH_LSB
#define TC_COLD_JUNCTION_CH__20 (uint32_t) 0x14 << TC_COLD_JUNCTION_CH_LSB
// tc - differential?
#define TC_DIFFERENTIAL_LSB 21
#define TC_DIFFERENTIAL (uint32_t) 0x0 << TC_DIFFERENTIAL_LSB
#define TC_SINGLE_ENDED (uint32_t) 0x1 << TC_DIFFERENTIAL_LSB
// tc - open ckt detect?
#define TC_OPEN_CKT_DETECT_LSB 20
#define TC_OPEN_CKT_DETECT__NO (uint32_t) 0x0 << TC_OPEN_CKT_DETECT_LSB
#define TC_OPEN_CKT_DETECT__YES (uint32_t) 0x1 << TC_OPEN_CKT_DETECT_LSB
// tc - open ckt detect current
#define TC_OPEN_CKT_DETECT_CURRENT_LSB 18
#define TC_OPEN_CKT_DETECT_CURRENT__10UA (uint32_t) 0x0 << TC_OPEN_CKT_DETECT_CURRENT_LSB
#define TC_OPEN_CKT_DETECT_CURRENT__100UA (uint32_t) 0x1 << TC_OPEN_CKT_DETECT_CURRENT_LSB
#define TC_OPEN_CKT_DETECT_CURRENT__500UA (uint32_t) 0x2 << TC_OPEN_CKT_DETECT_CURRENT_LSB
#define TC_OPEN_CKT_DETECT_CURRENT__1MA (uint32_t) 0x3 << TC_OPEN_CKT_DETECT_CURRENT_LSB
// tc - custom address
#define TC_CUSTOM_ADDRESS_LSB 6
// tc - custom length-1
#define TC_CUSTOM_LENGTH_1_LSB 0
// tc - custom values
#define TC_CUSTOM_VALUES_LSB 31
//**********************************************************************************************************
// -- Off-Chip Diode --
//**********************************************************************************************************
// diode - differential?
#define DIODE_DIFFERENTIAL_LSB 26
#define DIODE_DIFFERENTIAL (uint32_t) 0x0 << DIODE_DIFFERENTIAL_LSB
#define DIODE_SINGLE_ENDED (uint32_t) 0x1 << DIODE_DIFFERENTIAL_LSB
// diode - num readings
#define DIODE_NUM_READINGS_LSB 25
#define DIODE_NUM_READINGS__2 (uint32_t) 0x0 << DIODE_NUM_READINGS_LSB
#define DIODE_NUM_READINGS__3 (uint32_t) 0x1 << DIODE_NUM_READINGS_LSB
// diode - averaging on?
#define DIODE_AVERAGING_ON_LSB 24
#define DIODE_AVERAGING_OFF (uint32_t) 0x0 << DIODE_AVERAGING_ON_LSB
#define DIODE_AVERAGING_ON (uint32_t) 0x1 << DIODE_AVERAGING_ON_LSB
// diode - current
#define DIODE_CURRENT_LSB 22
#define DIODE_CURRENT__10UA_40UA_80UA (uint32_t) 0x0 << DIODE_CURRENT_LSB
#define DIODE_CURRENT__20UA_80UA_160UA (uint32_t) 0x1 << DIODE_CURRENT_LSB
#define DIODE_CURRENT__40UA_160UA_320UA (uint32_t) 0x2 << DIODE_CURRENT_LSB
#define DIODE_CURRENT__80UA_320UA_640UA (uint32_t) 0x3 << DIODE_CURRENT_LSB
// diode - ideality factor(eta)
#define DIODE_IDEALITY_FACTOR_LSB 0
//**********************************************************************************************************
// -- GLOBAL CONFIGURATION CONSTANTS --
//**********************************************************************************************************
#define REJECTION__50_60_HZ (uint8_t) 0x0
#define REJECTION__60_HZ    (uint8_t) 0x1
#define REJECTION__50_HZ    (uint8_t) 0x2
#define TEMP_UNIT__C        (uint8_t) 0x0
#define TEMP_UNIT__F        (uint8_t) 0x4
//**********************************************************************************************************
// -- STATUS BYTE CONSTANTS --
//**********************************************************************************************************
#define SENSOR_HARD_FAILURE (uint8_t) 0x80
#define ADC_HARD_FAILURE    (uint8_t) 0x40
#define CJ_HARD_FAILURE     (uint8_t) 0x20
#define CJ_SOFT_FAILURE     (uint8_t) 0x10
#define SENSOR_ABOVE        (uint8_t) 0x8
#define SENSOR_BELOW        (uint8_t) 0x4
#define ADC_RANGE_ERROR     (uint8_t) 0x2
#define VALID               (uint8_t) 0x1
//**********************************************************************************************************
// -- ADDRESSES --
//**********************************************************************************************************
#define COMMAND_STATUS_REGISTER          (uint16_t) 0x0000
#define CH_ADDRESS_BASE                  (uint16_t) 0x0200
#define VOUT_CH_BASE                     (uint16_t) 0x0060
#define READ_CH_BASE                     (uint16_t) 0x0010
#define CONVERSION_RESULT_MEMORY_BASE    (uint16_t) 0x0010
//**********************************************************************************************************
// -- MISC CONSTANTS --
//**********************************************************************************************************
#define WRITE_TO_RAM            (uint8_t) 0x02
#define READ_FROM_RAM           (uint8_t) 0x03
#define CONVERSION_CONTROL_BYTE (uint8_t) 0x80

#define VOLTAGE                 (uint8_t) 0x01
#define TEMPERATURE             (uint8_t) 0x02