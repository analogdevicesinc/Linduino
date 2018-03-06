/*!
Linear Technology DC934 Demonstration Board.
LTC3335: Nanopower Buck-Boost DC/DC with Integrated Coulomb Counter

@verbatim

This sketch demonstrates the LTC3335 using the DC2343A demo board.  The data read from
the data board is displayed on a serial terminal using 115200 baud, and newline terminations
for data input.

Code will build for configuration specified in LTC3335_Config.h.

 1. The selected output voltage is specified by LTC3335_OUTPUT_VOLTAGE.
 2. The LTC3335 prescaler will be chosen appropriately for the battery capacity specified
    by LTC3335_CAPACITY.
 3. The LTC3335 alarm will be chosen appropriately for the value specified by
    LTC3335_ALARM_CAPACITY.
 4. The coulomb count and battery current measurement will be scaled accordingly to the
    LTC3335 prescaler selected by LTC3335_CAPACITY and the LTC3335_IPEAK_CONFIGURATION,
    setting.
 5. Set LTC3335_USE_CURRENT_MEASUREMENT if you wish to use the LTC3335 battery current measurement.
 6. Set LTC3335_USE_SOFTWARE_CORRECTION if you wish to use correction of the LTC3335 coulomb
    count and battery current measurement.  A value of LTC3335_VBAT_TYP must be specified
    for this sketch as it can not measure the actual battery voltage, and must assume a constant
    value.  Applications reusing this code would improve performance if the actual battery voltage
    was measured and used for the correction functions.  See the DC2343A schematic for a circuit
    which measures the battery voltage with >10nA of average current drawn from the battery.

@endverbatim

To use the Linduino with the DC2343A, the PIC16 on the DC2343A must be disconnected by removing
the DC2343-ASSY1 PCB from JP5:
\image html "../../Part Number/3000/3300/3335/DC2343A/Assy_PCB_Removed.PNG"

A cable must then be constructed to connect the Linduino to the DC2343A as shown:
\image html "../../Part Number/3000/3300/3335/DC2343A/Linduino_Cable.png"

http://www.linear.com/product/LTC3335

http://www.linear.com/product/LTC3335#demoboards

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
    @ingroup LTC3335
*/

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Includes
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include <Arduino.h>
#include "Linduino.h"
#include "LT_I2C.h"
#include "LTC3335.h"
#include "printf.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Controls rate at which LTC3335 is polled and rate at which terminal is updated.
#define REFRESH_INTERVAL          1000    //! in ms, the rate at which the LTC3335 is polled and data output to terminal screen.
#define CURRENT_REFRESH_INTERVAL  5000    //! in ms, the period over which the LTC3335 current measurement is averaged.

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Global Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Local Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

uint16_t refresh_time_last;               //!< Variable to schedule LTC3335 polling and data output at REFRESH_INTERVAL
uint16_t current_refresh_time_last;       //!< Variable to schedule LTC3335 current measurement averaging at CURRENT_REFRESH_INTERVAL.

uint32_t data_point_counter;              //!< counts the number of datapoints output to terminal screen.
uint32_t discharged_capacity;             //!< in mAs, the discharged capacity calculated from the LTC3335 accumulator and
//!<  quiescent losses (if software correction is enabled).
LTC3335_ALARM_TYPE alarms;                //!< active alarms read from the LTC3335.
#if LTC3335_USE_CURRENT_MEASUREMENT == true
uint16_t current;                         //!< the current measurement from the LTC3335's counter test mode.
#endif // #if LTC3335_USE_CURRENT_MEASUREMENT == true
#if LTC3335_USE_SOFTWARE_CORRECTION == true
uint16_t vbat = LTC3335_VBAT_TYP;         //!< battery voltage, optimally measured with an adc, set to a constant in this sketch
#endif // #if LTC3335_USE_CURRENT_MEASUREMENT == true

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Local Prototypes
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// \cond
void print_separator();                   // Prints line on terminal to separate other text blocks.
void print_config();                      // Prints configuration of LTC3335 set in LTC3335_Config.h.
void print_data_header();                 // Prints header for data displayed in terminal at runtime.
void print_data();                        // Prints data displayed in terminal at runtime.
// \endcond

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Global Functions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


//! Initialize Linduino
void setup()
{

  Serial.begin(115200);           //! Initialize the serial port to the PC.
  while (!Serial);                //! Wait for serial port to be opened in the case of Leonardo USB Serial.

  //! Wait for LTC3335 to initialize successfully.
  while ((LTC3335_Init() != 0))
  {
    print_error();                //! Show reasons why LTC3335 might not have initialized.
    delay(5000);
  }

  //! Display configuration set in LTC3335_Config.h.
  print_config();
  print_data_header();

  //! Set up timing for tasks in loop.
  data_point_counter = 0;
  refresh_time_last = millis();
  current_refresh_time_last = refresh_time_last;

  return;
}

//! Repeats Linduino Loop
void loop()
{
  uint16_t time_new = millis(); // ! Get new time

  //! Check if it's time to poll LTC3335 and update the terminal.
  if (time_new - refresh_time_last > REFRESH_INTERVAL)
  {
    //! Read the discharged capacity.
#if LTC3335_USE_SOFTWARE_CORRECTION == false
    LTC3335_Get_Discharged_Capacity(&discharged_capacity);
#else
    LTC3335_Get_Discharged_Capacity(&discharged_capacity, vbat);
#endif // LTC3335_USE_SOFTWARE_CORRECTION == false

    //! If current measurement is being used, read it.  If software correction is used, pass the battery voltage
    //! to the function so that the proper correction factor will be used.
#if LTC3335_USE_CURRENT_MEASUREMENT == true
#if LTC3335_USE_SOFTWARE_CORRECTION == false
    LTC3335_Get_Counter_Test_Current(&current);
#else
    LTC3335_Get_Counter_Test_Current(&current, vbat);
#endif // LTC3335_USE_SOFTWARE_CORRECTION == false
#endif // #if LTC3335_USE_CURRENT_MEASUREMENT == true

    //! Only need to read alarms unless nIRQ is active (although not when nIRQ is used for current measurement).
    if ((LTC3335_NIRQ_READ() == 0) || (LTC3335_USE_CURRENT_MEASUREMENT == true))
    {
      LTC3335_Get_Alarms(&alarms);
    }

    //! Update terminal.
    print_data();

    // Schedule next loop.
    refresh_time_last += REFRESH_INTERVAL;
    data_point_counter++;
  }

#if LTC3335_USE_CURRENT_MEASUREMENT == true
  //! If using the current measurement, clear it periodically.  The current measurement will be averaged over time until it is reset.
  if (time_new - current_refresh_time_last > CURRENT_REFRESH_INTERVAL)
  {
    LTC3335_Reset_Counter_Test_Current();
    current_refresh_time_last += CURRENT_REFRESH_INTERVAL;
  }

  //! Update the Counter Test current in the background.  Must be called more often than LTC3335_MIN_CURRENT_TASK_RATE.
  LTC3335_Counter_Test_Current_Task();
#endif // #if LTC3335_USE_CURRENT_MEASUREMENT == true

  //! If any character is entered, send command to clear interrupt.
  if (Serial.available())
  {
    LTC3335_Clear_Int(&alarms);
    Serial.read();    // Clear out character that caused Clear Int command to be sent.
  }

}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Local Functions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// \cond
// Code for displaying operation of sketch on a serial terminal.
// It is not intended for any of the functions below to be useful in a customer application,
// and no attempt was made to optimize it.

//! Constants defined so that other constants don't include unexplained numbers in them.
#define MV_PER_V            1000
#define MA_PER_A            1000
#define UA_PER_MA           1000
#define SEC_PER_HR          3600
#define MS_PER_SEC          1000

//! Macro prints a float to the serial port with the precision selected by the size of the number.
#define PRINT_PICK_DEC(v)   Serial.print(v, v >= 1 ? 3 : 6);

// Prints line on terminal to separate other text blocks.
void print_separator()
{
  Serial.println(F("=========================================================================================="));
}

// Prints message to terminal about reasons why the LTC3335 may have failed to communicate.
void print_error()
{
  Serial.println(F("Initialization of LTC3335 failed."));
  Serial.println(F("Ensure Linduino is connected correctly to DC2343A, "));
  Serial.println(F("DC2343A is connected to VBAT between 1.8V and 5.5V,"));
  Serial.println(F("and that EN jumper is in the ON position."));
  print_separator();
}

// Prints configuration of LTC3335 set in LTC3335_Config.h.
void print_config()
{
  // Describe the application.
  print_separator();
  Serial.println(F("Sample application for LTC3335 Nanopower Buck-Boost DC/DC with Integrated Coulomb Counter."));
  print_separator();

  // Display the parameters set by user in LTC3335_Config.h.
  Serial.print(F("Capacity = "));
  Serial.print((uint32_t)(1L * LTC3335_CAPACITY * SEC_PER_HR));
  Serial.print(F("mAs ("));
  PRINT_PICK_DEC(1.0 * LTC3335_CAPACITY / MA_PER_A);
  Serial.print(F("Ah), IPeak = "));
  Serial.print(LTC3335_IPEAK_MA);
  Serial.print(F("mA, Output Voltage = "));
#if LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_1_8V
  Serial.print(1.8, 1);
#elif LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_5V
  Serial.print(2.5, 1);
#elif LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_2_8V
  Serial.print(2.8, 1);
#elif LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_0V
  Serial.print(3.0, 1);
#elif LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_3V
  Serial.print(3.3, 1);
#elif LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_3_6V
  Serial.print(3.6, 1);
#elif LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_4_5V
  Serial.print(4.5, 1);
#elif LTC3335_OUTPUT_VOLTAGE == LTC3335_OUTPUT_VOLTAGE_5_0V
  Serial.print(5.0, 1);
#endif
  Serial.println(F("V."));
  print_separator();

  // Display the prescaler and capacity range resulting from the parameters set by user in LTC3335_Config.h.
#if LTC3335_USE_SOFTWARE_CORRECTION == true
  if (LTC3335_Get_Software_Correction_Factor(LTC3335_VBAT_TYP) != LTC3335_CORRECTION_FACTOR_TYP)
  {
    Serial.println(F("LTC3335_CORRECTION_FACTOR_TYP in LTC3335_Config.h not set to value returned as LTC3335_Get_Software_Correction_Factor(LTC3335_VBAT_TYP)"));
    Serial.print(F("LTC3335_CORRECTION_FACTOR_TYP = "));
    Serial.print(LTC3335_CORRECTION_FACTOR_TYP);
    Serial.print(F(", LTC3335_VBAT_TYP = "));
    Serial.print(LTC3335_VBAT_TYP);
    Serial.println(F("V."));
    Serial.print(F("LTC3335_Get_Software_Correction_Factor(LTC3335_VBAT_TYP) = "));
    Serial.println(LTC3335_Get_Software_Correction_Factor(LTC3335_VBAT_TYP));
  }
  else
#endif // #if LTC3335_USE_SOFTWARE_CORRECTION == true
  {
    Serial.print(F("Prescaler Set to "));
    Serial.print(LTC3335_PRESCALER);
    Serial.print(F(" for a maximum accumulator range of "));
    Serial.print(LTC3335_RANGE(LTC3335_PRESCALER));
    Serial.print(F("mAs ("));
    PRINT_PICK_DEC(1.0 * LTC3335_RANGE(LTC3335_PRESCALER) / (1.0 * MA_PER_A * SEC_PER_HR));
    Serial.print(F("Ah)"));
#if LTC3335_USE_SOFTWARE_CORRECTION == false
    Serial.println(F("."));
    Serial.println(F("LTC3335_USE_SOFTWARE_CORRECTION not enabled"));
#else
    Serial.println(F(","));
    Serial.print(F("assuming an average battery voltage of "));
    Serial.print(1.0 * LTC3335_VBAT_TYP / MV_PER_V);
    Serial.println(F("V,"));
    Serial.print(F("resulting in an average correction of "));
    Serial.print(-100.0 * LTC3335_Get_Software_Correction_Factor(LTC3335_VBAT_TYP) / (1L << 16) );
    Serial.println(F("%."));
#endif // #if LTC3335_USE_SOFTWARE_CORRECTION == false
  }
  print_separator();

  // Display the alarm value selected to be closest by less than that set by user in LTC3335_Config.h.
#if LTC3335_USE_SOFTWARE_CORRECTION == false
  uint8_t alarm_reg = (1LL * LTC3335_ALARM_CAPACITY * SEC_PER_HR)/LTC3335_RESOLUTION(LTC3335_PRESCALER);
  uint32_t alarm_mas = alarm_reg * LTC3335_RESOLUTION(LTC3335_PRESCALER);
#else
  uint8_t alarm_reg = (1LL * LTC3335_ALARM_CAPACITY * SEC_PER_HR)/(LTC3335_RESOLUTION(LTC3335_PRESCALER) * ((1LL << 16) + LTC3335_CORRECTION_FACTOR_TYP) / (1L << 16));
  uint32_t alarm_mas = alarm_reg * LTC3335_RESOLUTION(LTC3335_PRESCALER) * ((1LL << 16) + LTC3335_CORRECTION_FACTOR_TYP) / (1L << 16);
#endif // #if LTC3335_USE_SOFTWARE_CORRECTION == false
  Serial.print(F("Alarm Register Set to "));
  Serial.print(alarm_mas);
  Serial.print(F("mAs ("));
  PRINT_PICK_DEC(1.0 * alarm_mas / (1.0 * MA_PER_A * SEC_PER_HR));
  Serial.println(F("Ah),"));
  Serial.print(F("which is "));
  Serial.print(100.0 * alarm_mas / (1L * LTC3335_CAPACITY * SEC_PER_HR), 2);
  Serial.println(F("% of the battery capacity."));
  print_separator();

#if LTC3335_USE_CURRENT_MEASUREMENT == true
  Serial.println(F("Counter Test mode used to measure battery current with nIRQ pin."));
  print_separator();
#endif // #if LTC3335_USE_CURRENT_MEASUREMENT == false

  Serial.println(F("Send any character to send a Clear Int command to the LTC3335."));
  print_separator();
}

// Prints header for data displayed in terminal at runtime.
void print_data_header()
{
  printf(F("Test Time (s), "));
  printf(F("Discharged Capacity (mAs), "));
#if LTC3335_USE_CURRENT_MEASUREMENT == true
  printf(F("Battery Current (mA), "));
#endif // #if LTC3335_USE_CURRENT_MEASUREMENT == true
  printf(F("Alarm Trip Alarm, "));
  printf(F("Coulomb Counter Overflow Alarm, "));
  printf(F("AC On Time Overflow Alarm, "));
  printf(F("PGOOD, "));
  printf(F("nIRQ\r\n"));
}

// Prints data displayed in terminal at runtime.
void print_data()
{
  printf(F("%13.2f,"), 1.0 * data_point_counter * REFRESH_INTERVAL / MS_PER_SEC);
  printf(F("%26d,"), discharged_capacity);
#if LTC3335_USE_CURRENT_MEASUREMENT == true
  printf(F("%21.3f,"), 1.0 * current / MA_PER_A);
#endif // #if LTC3335_USE_CURRENT_MEASUREMENT == true
  printf(F("%17b,"), alarms.alarm_trip);
  printf(F("%31b,"), alarms.coulomb_counter_overflow);
  printf(F("%26b,"), alarms.ac_on_time_overflow);
  printf(F("%6b,"), LTC3335_PGOOD_READ());
  printf(F("%5b\r\n"), LTC3335_NIRQ_READ());
}
// \endcond

