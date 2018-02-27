/*!
LTC3335: Nanopower Buck-Boost DC/DC with Integrated Coulomb Counter

This header file contains the API to the LTC3335 driver.  A minimal
set of functions were declared to allow access to the features of the
part while abstracting away from the register transactions.

@verbatim
The LTC®3335 is a high efficiency, low quiescent current
(680nA) buck-boost DC/DC converter with an integrated
precision coulomb counter which monitors accumulated
battery discharge in long life battery powered applications.
The buck-boost can operate down to 1.8V on its input and
provides eight pin-selectable output voltages with up to
50mA of output current.

The coulomb counter stores the accumulated battery discharge
in an internal register accessible via an I2C interface.
The LTC3335 features a programmable discharge alarm
threshold. When the threshold is reached, an interrupt is
generated at the IRQ pin.

To accommodate a wide range of battery types and sizes,
the peak input current can be selected from as low as 5mA
to as high as 250mA and the full-scale coulomb counter
has a programmable range of 32,768:1.

@endverbatim

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
    API Header File for LTC3335: Nanopower Buck-Boost DC/DC with Integrated Coulomb Counter
*/

#ifndef LTC3335_H
#define LTC3335_H

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Includes
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "LTC3335_Config.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// \cond
//! @name General Purpose Macros
//! @{
//! Macros to return the number of bits needed to store a number
#define NBITS2(n)           ((n&2)?1:0)
#define NBITS4(n)           ((n&(0xC))?(2+NBITS2(n>>2)):(NBITS2(n)))
#define NBITS8(n)           ((n&0xF0)?(4+NBITS4(n>>4)):(NBITS4(n)))
#define NBITS16(n)          ((n&0xFF00)?(8+NBITS8(n>>8)):(NBITS8(n)))
#define NBITS32(n)          ((n&0xFFFF0000)?(16+NBITS16(n>>16)):(NBITS16(n)))
#define NBITS(n)            (n==0?0:NBITS32(1L*n)+1)

//! macro for bitmask.
#define MASK(size, shift)   (((1LL << (size)) - 1) << (shift))

//! Constants defined so that other constants don't include unexplained numbers in them.
#define MV_PER_V            1000
#define MA_PER_A            1000
#define UA_PER_MA           1000
#define SEC_PER_HR          3600
#define MS_PER_SEC          1000
//! @}
// \endcond

//! @name LTC3335 Driver Properties
//! @{
#define LTC3335_BAUD_RATE       400             //!< in kHz, Max Clock Operating Frequency (fSCL from datasheet page 5)
#define LTC3335_PRESCALER_MAX   15              //!< the maximum prescaler that be selected for the LTC3335.
#define LTC3335_TFS             (11.74e-6)      //!< in s, full scale ON time from page 15 of datasheet.
#define LTC3335_IQ              (680e-9)        //!< in A, Input Quiescent Current from page 1 of datasheet.
//! @}

//! @name Voltage Settings
//! @{
//! Options available for setting output voltage via I2C.
typedef enum { } LTC3335_OUTPUT_VOLTAGE_TYPE;
#define LTC3335_OUTPUT_VOLTAGE_1_8V 0
#define LTC3335_OUTPUT_VOLTAGE_2_5V 1
#define LTC3335_OUTPUT_VOLTAGE_2_8V 2
#define LTC3335_OUTPUT_VOLTAGE_3_0V 3
#define LTC3335_OUTPUT_VOLTAGE_3_3V 4
#define LTC3335_OUTPUT_VOLTAGE_3_6V 5
#define LTC3335_OUTPUT_VOLTAGE_4_5V 6
#define LTC3335_OUTPUT_VOLTAGE_5_0V 7
#define LTC3335_NUM_OUTPUT_VOLTAGES 8
//! @}

//! @name IPeak Settings
//! @{
//! Options available for IPeak setting.
//! NOTE! This can not be set via I2C.  It can only be set with resistors on pins 13-15
//! of the LTC3335, along with the an appropriately sized inductor.  In order to
//! translate accumulator values to coulombs, and Counter Test results to amps,
//! this setting must be known.
typedef enum { } LTC3335_IPEAK_CONFIGURATION_TYPE;
#define LTC3335_IPEAK_CONFIGURATION_5MA   0
#define LTC3335_IPEAK_CONFIGURATION_10MA  1
#define LTC3335_IPEAK_CONFIGURATION_15MA  2
#define LTC3335_IPEAK_CONFIGURATION_25MA  3
#define LTC3335_IPEAK_CONFIGURATION_50MA  4
#define LTC3335_IPEAK_CONFIGURATION_100MA 5
#define LTC3335_IPEAK_CONFIGURATION_150MA 6
#define LTC3335_IPEAK_CONFIGURATION_250MA 7
#define LTC3335_NUM_IPEAK                 8
//! @}

//! The alarm conditions which cause the LTC3335 to activate the /INT pin.
typedef struct
{
  unsigned ac_on_time_overflow        :1;     //!< AC(ON) time operating fault (tAC > tFS) due to improperly chosen inductor value timing out the AC(ON) measurement.
  unsigned coulomb_counter_overflow   :1;     //!< Coulomb counter operating fault due to an improperly chosen prescalar causing the ripple counter to overflow.
  unsigned alarm_trip                 :1;     //!< Accumulator value has met or exceeded the alarm threshold value.
  unsigned unused                     :5;
} LTC3335_ALARM_TYPE;

//!  counts for iq current to accumulate to 1 mAs
#define  LTC3335_TIMER_COUNTS_PER_IQ_MAS  ((uint32_t)(LTC3335_TIMER_COUNTS_PER_SEC/(LTC3335_IQ * MA_PER_A)))

//! LTC3335_Counter_Test_Current_Task() must be called often more often than this rate for hardware counter to not overflow
#define  LTC3335_MIN_CURRENT_TASK_RATE  ((1LL << LTC3335_COUNTER_SIZE) * 2 * LTC3335_TFS)

// Value of IPEAK in mA, used in calculations of coulomb count and current.
#if LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_5MA
#define LTC3335_IPEAK_MA  5                   //!< mA = 5mA, used in calculations of coulomb count and current.
#elif LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_10MA
#define LTC3335_IPEAK_MA  10                  //!< mA = 10mA, used in calculations of coulomb count and current.  
#elif LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_15MA
#define LTC3335_IPEAK_MA  15                  //!< mA = 15mA, used in calculations of coulomb count and current.
#elif LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_25MA
#define LTC3335_IPEAK_MA  25                  //!< mA = 25mA, used in calculations of coulomb count and current.
#elif LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_50MA
#define LTC3335_IPEAK_MA  50                  //!< mA = 50mA, used in calculations of coulomb count and current.
#elif LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_100MA
#define LTC3335_IPEAK_MA  100                 //!< mA = 100mA, used in calculations of coulomb count and current.
#elif LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_150MA
#define LTC3335_IPEAK_MA  150                 //!< mA = 150mA, used in calculations of coulomb count and current.
#elif LTC3335_IPEAK_CONFIGURATION == LTC3335_IPEAK_CONFIGURATION_250MA
#define LTC3335_IPEAK_MA  250                 //!< mA = 250mA, used in calculations of coulomb count and current.
#else
#error Must configure firmware for IPEAK value.
#endif

//! Macro to select the optimal prescaler for the specified battery capacity at compile time.
#define LTC3335_PRESCALER       (LTC3335_PRESCALER_MAX - NBITS( (uint32_t)(1L*LTC3335_CAPACITY*SEC_PER_HR/LTC3335_RANGE(LTC3335_PRESCALER_MAX)) ) )

//! Macro to retrieve the LTC3335 coulomb count range with a given prescaler.
#if LTC3335_USE_SOFTWARE_CORRECTION == true
#define LTC3335_RANGE(p)        ((uint32_t)(255LL * LTC3335_RESOLUTION(p) * ((1LL << 16) + LTC3335_CORRECTION_FACTOR_TYP) / (1L << 16)))
#else
#define LTC3335_RANGE(p)        ((uint32_t)(255LL * LTC3335_RESOLUTION(p)))
#endif

//! Macro to retrieve the LTC3335 coulomb count resolution with a given prescaler.
#define LTC3335_RESOLUTION(p)   ((uint32_t)(LTC3335_IPEAK_MA * LTC3335_TFS * (1LL << (42 - 1 - p)) / 255 + 0.5))

//! Verify that battery capacity isn't so gigantic that it would overflow a 32 bit number.
#if LTC3335_CAPACITY*SEC_PER_HR > ((1LL << 32) - 1)
#error Battery Capacity is larger than 32 bit number.  Change 32bit capacity variables into 64bit.
#endif

//! Verify that capacity resolution isn't so tiny that half bits in units of mAs result in 1% discretization error.
#if LTC3335_CAPACITY*SEC_PER_HR < ((1L << 8) * 100 / 2)
#error Battery Capacity is so small that units of mAs result in more than 1% discretization error.  Change code units to uAs.
#endif

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Global Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Global Functions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//! Initializes the LTC3335 driver, setting the prescaler, output voltage, and alarm threshold selected in LTC3335_Confg.h
//! @return 0 if the init was successful.
int8_t LTC3335_Init(void);

//! Enables/Disables software control of the LTC3335 output voltage.
//! If software control is enabled, the voltage is set to the specified setting.
//! @return 0 if the LTC3335 communication was successful.
int8_t LTC3335_Set_Voltage(boolean enabled,                      //!< TRUE to enable software control of output voltage, FALSE to disable software control (output voltage set by pins 3-5).
                           LTC3335_OUTPUT_VOLTAGE_TYPE voltage  //!< The output voltage setting if software control is enabled.
                          );

//! Gets the alarms active from the LTC3335.
//! @return 0 if the LTC3335 communication was successful.
int8_t LTC3335_Get_Alarms(LTC3335_ALARM_TYPE *alarms            //!< Bitmap containing the LTC3335 active alarms.
                         );

//! Sends the command to clear the INT condition.
//! NOTE!  Additional registers are rewritten in order for the INT condition to be reset.
//! @return 0 if the LTC3335 communication was successful.
int8_t LTC3335_Clear_Int(LTC3335_ALARM_TYPE *alarms
                        );

//! Enables/Disables the LTC3335 Counter Test feature.
//! @return 0 if the LTC3335 communication was successful.
int8_t LTC3335_Set_Counter_Test(boolean enabled                  //!< TRUE to enable Counter Test feature, FALSE to disable Counter Test feature.
                               );

#if LTC3335_USE_SOFTWARE_CORRECTION == false
//! Gets the discharged capacity from the battery in mAs.
//! @return 0 if the LTC3335 communication was successful.
int8_t LTC3335_Get_Discharged_Capacity(uint32_t *discharged_capacity    //!< Pointer to where discharged capacity will be returned.
                                      );
#else
//! Gets the discharged capacity from the battery in mAs.
//! @return 0 if the LTC3335 communication was successful.
int8_t LTC3335_Get_Discharged_Capacity(uint32_t *discharged_capacity,   //!< Pointer to where discharged capacity will be returned.
                                       uint16_t vbat                    //!< Battery voltage used to select software correction factor.
                                      );
#endif // LTC3335_USE_SOFTWARE_CORRECTION == false

#if LTC3335_USE_CURRENT_MEASUREMENT == true
#if LTC3335_USE_SOFTWARE_CORRECTION == false
//! Gets the battery current in uA.
//! @return 0 if the LTC3335 communication was successful.
int8_t LTC3335_Get_Counter_Test_Current(uint16_t *microamps             //!< Pointer to where current will be returned.
                                       );
#else
//! Gets the battery current in uA.
//! @return 0 if the LTC3335 communication was successful.
int8_t LTC3335_Get_Counter_Test_Current(uint16_t *microamps,            //!< Pointer to where current will be returned.
                                        uint16_t vbat                   //!< Battery voltage used to select software correction factor.
                                       );
#endif // LTC3335_USE_SOFTWARE_CORRECTION == false

//! Resets the number of edges and the amount of time stored for the Counter Test feature.
//! @return TRUE if the LTC3335 communication was successful.
int8_t LTC3335_Reset_Counter_Test_Current(void);

//! Task that must be run periodically, for the edges and time to be stored for the
//! LTC3335 Counter Test feature.
//! @return TRUE if the LTC3335 communication was successful.
void LTC3335_Counter_Test_Current_Task(void);
#endif // LTC3335_USE_CURRENT_MEASUREMENT == true

#if LTC3335_USE_SOFTWARE_CORRECTION == true
//! Returns the software correction factor for a specified LTC3335_IPEAK_CONFIGURATION, LTC3335_OUTPUT_VOLTAGE, and battery voltage.
//! Note! - These corrections factors are only valid at room temperature and for the recommended Coilcraft LPS5030-xxxMRB inductor.
//! @return TRUE if the LTC3335 communication was successful.
int16_t LTC3335_Get_Software_Correction_Factor(uint16_t vbat            //!< Battery voltage used to select software correction factor.
                                              );
#endif // LTC3335_USE_SOFTWARE_CORRECTION == true

#endif  // LTC3335_H

