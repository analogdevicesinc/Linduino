/*!
LTC3335: Nanopower Buck-Boost DC/DC with Integrated Coulomb Counter

This driver file defines a minimal set of functions to allow access to
the features of the LTC3335 part while abstracting away from the register
transactions.  Configuration of this driver is done by modifying the
definitions in LTC3335_Config.h.

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

//! @ingroup Switching_Regulators
//! @{
//! @defgroup LTC3335 LTC3335: Nanopower Buck-Boost DC/DC with Integrated Coulomb Counter
//! @}

/*! @file
    @ingroup LTC3335
    Driver File for LTC3335: Nanopower Buck-Boost DC/DC with Integrated Coulomb Counter
*/

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Includes
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "LTC3335.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Definitions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// \cond
//! @name I2C Address Definitions
//! @{
//! I2C address and format from datasheet page 5
#define LTC3335_BASE_ADDRESS    0x64 //!< BASE_ADDRESS for LTC3335.
#define LTC3335_WRITE_BIT       0    //!< I2C R/W bit value for WRITE.
#define LTC3335_READ_BIT        1    //!< I2C R/W bit value for READ.
//! @}

//! @name LTC3335 Register Definitions
//! @{
//! Register Map from datasheet table 3
#define LTC3335_REGISTER_A      01 //!< VOUT selection and prescaler selection register.
#define LTC3335_REGISTER_B      02 //!< Alarm threshold register.
#define LTC3335_REGISTER_C      03 //!< Accumulator register.
#define LTC3335_REGISTER_D      04 //!< Alarms register.
#define LTC3335_REGISTER_E      05 //!< Status register.
#define LTC3335_INVALID         0xFF //!< code to indicate that cached data has not be set yet.
//! @}
// \endcond

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Global Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Local Data
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//! @name Cached LTC3335 Data
//! @{
//! Cached copies of values in registers, so that they do not need to be read/modified/written each time a function is called to only set a portion of that register.
static uint8_t ltc3335_subaddress_last;                                //!<  last subaddress written, so that it is not repeatedly sent when the same register is polled.
static LTC3335_OUTPUT_VOLTAGE_TYPE ltc3335_voltage_selection_last;     //!<  last voltage selected through software.
static boolean ltc3335_voltage_selection_enabled_last;                 //!<  last enable for software selected voltage.
static uint8_t ltc3335_prescaler_last;                                 //!<  last prescaler selected.
static boolean ltc3335_counter_test_last;                              //!<  value of the counter test bit last written to the LTC3335.
//! @}

//! @name Counter Test Variables
//! Variables used to count edges per second when the Counter Test feature is turned on, providing an instantaneous measurement of the battery current.
#if LTC3335_USE_CURRENT_MEASUREMENT == true
static uint16_t ltc3335_hw_timer_last;                                 //!<  last value of the hardware timer accessed by LTC3335 driver.
static uint16_t ltc3335_hw_counter_last;                               //!<  last value of the hardware counter accessed by LTC3335 driver.
static uint32_t ltc3335_counter_test_edge_count;                       //!<  the number of rising edges on the /IRQ pin since the Counter Test results were last cleared.
static uint32_t ltc3335_counter_test_time;                             //!<  the amount of timer ticks since the Counter Test results were last cleared.
#endif // #if LTC3335_USE_CURRENT_MEASUREMENT == true
//! @}

//! @name
//!
#if LTC3335_USE_SOFTWARE_CORRECTION == true
static uint8_t ltc3335_accumulator_last;                                //!<
static uint32_t ltc3335_quiescent_current_timer;                        //!<
static uint32_t ltc3335_discharged_capacity;                            //!<
#endif // #if LTC3335_USE_SOFTWARE_CORRECTION == true
//! @}

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Local Prototypes
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

static int8_t ltc3335_set_register(uint8_t subaddress, uint8_t *ltc3335_data);
static int8_t ltc3335_get_register(uint8_t subaddress, uint8_t *ltc3335_data);
static uint8_t ltc3335_encode_register_a(boolean enabled, LTC3335_OUTPUT_VOLTAGE_TYPE voltage, uint8_t prescaler);
static void ltc3335_decode_register_a(uint8_t register_a);

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Global Functions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Initializes the LTC3335 driver.
int8_t LTC3335_Init(void)
{
  int8_t result;
  uint8_t ltc3335_data;

  // Init all of the microprocessor hardware used by the LTC3335 driver.
  LTC3335_MICRO_INIT();

  // Init to INVALID, so that subaddress will be sent for first right.  After this, subaddress will only be written if needed.
  ltc3335_subaddress_last = LTC3335_INVALID;

  // Send command to set the voltage and prescaler set in LTC3335_Config.h with one i2c transaction.
  ltc3335_data = ltc3335_encode_register_a(true, (LTC3335_OUTPUT_VOLTAGE_TYPE) LTC3335_OUTPUT_VOLTAGE, LTC3335_PRESCALER);
  result = ltc3335_set_register(LTC3335_REGISTER_A, &ltc3335_data);

  // Set cached values in case voltage or prescaler are changed during runtime.
  ltc3335_voltage_selection_last = (LTC3335_OUTPUT_VOLTAGE_TYPE) LTC3335_OUTPUT_VOLTAGE;
  ltc3335_prescaler_last = LTC3335_PRESCALER;
  ltc3335_voltage_selection_enabled_last = true;

  // Set the alarm threshold.  Round down so that set value is always <= to the desired value.
#if LTC3335_USE_SOFTWARE_CORRECTION == false
  ltc3335_data = (1LL * LTC3335_ALARM_CAPACITY * SEC_PER_HR)/LTC3335_RESOLUTION(LTC3335_PRESCALER);
#else
  ltc3335_data = (1LL * LTC3335_ALARM_CAPACITY * SEC_PER_HR)/(LTC3335_RESOLUTION(LTC3335_PRESCALER) * ((1LL << 16) + LTC3335_CORRECTION_FACTOR_TYP) / (1L << 16));
#endif // #if LTC3335_USE_SOFTWARE_CORRECTION == false
  result |= ltc3335_set_register(LTC3335_REGISTER_B, &ltc3335_data);

  // Turn counter test on or off given setting in in LTC3335_Config.h.
  result |= LTC3335_Set_Counter_Test(LTC3335_USE_CURRENT_MEASUREMENT);

#if LTC3335_USE_SOFTWARE_CORRECTION == true
  // Initialize the corrected discharged capacity and timer for quiescent current accumulation..
  ltc3335_discharged_capacity = 0;
  ltc3335_accumulator_last = 0;
  ltc3335_quiescent_current_timer = LTC3335_TIMER_GET();
  result |= LTC3335_Get_Discharged_Capacity(&ltc3335_discharged_capacity, LTC3335_VBAT_TYP);
#endif // #if LTC3335_USE_SOFTWARE_CORRECTION == true

  return result;
}

// Enables/Disables software control of the LTC3335 output voltage.
// If software control is enabled, the voltage is set to the specified setting.
int8_t LTC3335_Set_Voltage(boolean enabled, LTC3335_OUTPUT_VOLTAGE_TYPE voltage)
{
  int8_t result;
  uint8_t ltc3335_data;

  // Can not set out of range.
  if (LTC3335_NUM_OUTPUT_VOLTAGES <= voltage)
  {
    return 1;
  }

  // Send command to set the voltage.
  ltc3335_data = ltc3335_encode_register_a(enabled, voltage, ltc3335_prescaler_last);
  result = ltc3335_set_register(LTC3335_REGISTER_A, &ltc3335_data);

  return result;
}

// Gets the alarms active from the LTC3335.
int8_t LTC3335_Get_Alarms(LTC3335_ALARM_TYPE *alarms)
{
  int8_t result;
  uint8_t ltc3335_data;

  result = ltc3335_get_register(LTC3335_REGISTER_D, &ltc3335_data);

  if (result == 0)
  {
    // Decode the register into its alarm bits.
    if (ltc3335_data & MASK(1, 0))
    {
      alarms->ac_on_time_overflow = 1;
    }
    else
    {
      alarms->ac_on_time_overflow = 0;
    }

    if (ltc3335_data & MASK(1, 1))
    {
      alarms->coulomb_counter_overflow = 1;
    }
    else
    {
      alarms->coulomb_counter_overflow = 0;
    }

    if (ltc3335_data & MASK(1, 2))
    {
      alarms->alarm_trip = 1;
    }
    else
    {
      alarms->alarm_trip = 0;
    }
  }

  return result;
}

// Sends the command to clear the INT condition.
// NOTE!  Additional registers are rewritten in order for the INT condition to be reset.
int8_t LTC3335_Clear_Int(LTC3335_ALARM_TYPE *alarms)
{
  int8_t result = 0;
  uint8_t ltc3335_data;

  // Write to other registers required depending upon the specific type of alarm
  if (alarms->coulomb_counter_overflow)
  {
    // The accumulator must be rewritten in order for the clear int command to clear these alarms.
    result |= ltc3335_get_register(LTC3335_REGISTER_C, &ltc3335_data);
    if (result == 0)  result |= ltc3335_set_register(LTC3335_REGISTER_C, &ltc3335_data);
  }

  if (alarms->alarm_trip)
  {
    // Note that if the accumulator is >= the alarm value, the alarm_trip bit will immediately trip again.
  }

  if (alarms->ac_on_time_overflow)
  {
    //  Note AC ON Time Alarm can not be cleared until PK[2:0] (Pin 15, 14, 13) are changed to match inductor value.
  }

  // If currently doing a Counter Test, do not turn it off when clearing the int.
  ltc3335_data = 1;
  if (ltc3335_counter_test_last == true)
  {
    ltc3335_data |= 2;
  }

  result |= ltc3335_set_register(LTC3335_REGISTER_E, &ltc3335_data);
  ltc3335_data++;

  return result;
}

#if LTC3335_USE_SOFTWARE_CORRECTION == false
// Reads the accumulator and translates into discharged capacity.
int8_t LTC3335_Get_Discharged_Capacity(uint32_t *discharged_capacity)
{
  int8_t result = true;
  uint8_t ltc3335_data;

  result = ltc3335_get_register(LTC3335_REGISTER_C, &ltc3335_data);

  if (result == 0)
  {
    *discharged_capacity = ltc3335_data * LTC3335_RESOLUTION(LTC3335_PRESCALER);
  }

  return result;
}
#else
// Tracks changes in the accumulator and translates changes in discharged capacity, accounting
// for the resolution available at the current battery voltage, output voltage, and ipeak.
// The quiescent current is accumulated over time and used to update the discharged capacity.
int8_t LTC3335_Get_Discharged_Capacity(uint32_t *discharged_capacity, uint16_t vbat)
{
  int8_t result;
  uint8_t ltc3335_data;

  result = ltc3335_get_register(LTC3335_REGISTER_C, &ltc3335_data);

  if (result == 0)
  {
    if (ltc3335_data != ltc3335_accumulator_last)
    {
      // Add in changes in the accumulator, adjusted by the software correction factor.
      int16_t temp16 = LTC3335_Get_Software_Correction_Factor(vbat);
      uint32_t mAs = (uint8_t)(ltc3335_data - ltc3335_accumulator_last) * LTC3335_RESOLUTION(LTC3335_PRESCALER);
      mAs += ((mAs * temp16 + (1L << 15)) >> 16) | (temp16 < 0 ? 0xFFFF0000LL : 0);
      ltc3335_discharged_capacity += mAs;
      ltc3335_accumulator_last = ltc3335_data;
    }
  }

  // Look for enough time to pass to increment discharged capacity by 1mAs.
  if (LTC3335_TIMER_GET() - ltc3335_quiescent_current_timer > LTC3335_TIMER_COUNTS_PER_IQ_MAS)
  {
    ltc3335_discharged_capacity++;
    ltc3335_quiescent_current_timer += LTC3335_TIMER_COUNTS_PER_IQ_MAS;
  }

  *discharged_capacity = ltc3335_discharged_capacity;

  return result;

}
#endif // LTC3335_USE_SOFTWARE_CORRECTION == false

// Enables/Disables the LTC3335 Counter Test feature.  This can be used to verify the
// LTC3335 is functional, or it can be used to measure the battery current.
int8_t LTC3335_Set_Counter_Test(boolean enabled)
{
  int8_t result;
  uint8_t ltc3335_data;

  // Send command to start the test.
  ltc3335_data = ((enabled == true) ? MASK(1, 1) : 0);
  result = ltc3335_set_register(LTC3335_REGISTER_E, &ltc3335_data);

  // Store results of the test so that we don't need to poll the IC for that info.
  if (result == 0)
  {
    ltc3335_counter_test_last = enabled;
  }

#if LTC3335_USE_CURRENT_MEASUREMENT == true
  // Initialize after the command is sent, as the first edge that occurs
  // when the test is started should not be counted.
  if (enabled == true)
  {
    // Take initial readings when test is started.
    LTC3335_Counter_Test_Current_Task();
  }
  else
  {
    // Reset test results when test is stopped.
    LTC3335_Reset_Counter_Test_Current();
  }
#endif

  return result;
}


#if LTC3335_USE_CURRENT_MEASUREMENT == true
// Resets the number of edges and the amount of time stored for the Counter Test feature.
int8_t LTC3335_Reset_Counter_Test_Current(void)
{
  ltc3335_counter_test_edge_count = 0;
  ltc3335_counter_test_time = 0;
  ltc3335_hw_timer_last = LTC3335_TIMER_GET();
  ltc3335_hw_counter_last = LTC3335_COUNTER_GET();

  return 0;
}

// Accumulates the number of edges on the /IRQ pins provided by the LTC3335 Counter Test feature over a period of time,
// the ratio of which can be used to calculate battery current.
// Task that must be run periodically at a rate faster than LTC3335_MIN_CURRENT_TASK_RATE.
void LTC3335_Counter_Test_Current_Task(void)
{
  if (ltc3335_counter_test_last == true)
  {
    // Get new timer and counter values
    uint16_t hw_timer_new = LTC3335_TIMER_GET();
    uint16_t hw_counter_new = LTC3335_COUNTER_GET();

    // Add difference between last timer and counter to summations.
    ltc3335_counter_test_edge_count += (hw_counter_new - ltc3335_hw_counter_last);
    ltc3335_counter_test_time += (hw_timer_new - ltc3335_hw_timer_last);

    // Save new timer and counter values for next execution of task.
    ltc3335_hw_timer_last = hw_timer_new;
    ltc3335_hw_counter_last = hw_counter_new;
  }

  return;
}

#if LTC3335_USE_SOFTWARE_CORRECTION == false
// Calculates the battery current from the number of edges on the /IRQ pins over a period of time accumulated in LTC3335_Counter_Test_Current_Task.
int8_t LTC3335_Get_Counter_Test_Current(uint16_t *microamps)
{
  if ((ltc3335_counter_test_last == true) && ((ltc3335_counter_test_time != 0)))
  {
    if (ltc3335_counter_test_edge_count != 0)
    {
      uint64_t uAs = (uint64_t)(1LL * LTC3335_IPEAK_MA * UA_PER_MA * LTC3335_TFS * LTC3335_TIMER_COUNTS_PER_SEC) * ltc3335_counter_test_edge_count;
      uAs += ltc3335_counter_test_time/2;
      uAs /= ltc3335_counter_test_time;
      if (uAs >= (1L << 16))
      {
        *microamps = (1L << 16) - 1;
      }
      else
      {
        *microamps = (uint16_t) uAs;
      }
    }
    else
    {
      *microamps = 0;
    }
    return 0;
  }
  else
  {
    return 1;
  }
}
#else
// Calculates the battery current from the number of edges on the /IRQ pins over a period of time accumulated in,
// LTC3335_Counter_Test_Current_Task accounting for the resolution available at the current battery voltage, output voltage, and ipeak.
int8_t LTC3335_Get_Counter_Test_Current(uint16_t *microamps, uint16_t vbat)
{

  if ((ltc3335_counter_test_last == true) && ((ltc3335_counter_test_time != 0)))
  {
    if (ltc3335_counter_test_edge_count != 0)
    {
      int16_t temp16 = LTC3335_Get_Software_Correction_Factor(vbat);
      uint64_t uAs = (uint64_t)(1LL * LTC3335_IPEAK_MA * UA_PER_MA * LTC3335_TFS * LTC3335_TIMER_COUNTS_PER_SEC) * ltc3335_counter_test_edge_count;
      uAs += ((uAs * temp16 + (1L << 15)) >> 16) | (temp16 < 0 ? 0xFFFF000000000000LL : 0);
      uAs += ltc3335_counter_test_time / 2;
      uAs /= ltc3335_counter_test_time;
      if (uAs >= (1L << 16))
      {
        *microamps = (1L << 16) - 1;
      }
      else
      {
        *microamps = (uint16_t) uAs;
      }
    }
    else
    {
      *microamps = 0;
    }
    return 0;
  }
  else
  {
    return 1;
  }
}
#endif // #if LTC3335_USE_SOFTWARE_CORRECTION == false
#endif // #if LTC3335_USE_CURRENT_MEASUREMENT == true

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Local Functions
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

// Writes to an LTC3335 register.
static int8_t ltc3335_set_register(uint8_t subaddress, uint8_t *ltc3335_data_ptr)
{
  int8_t result = 0;
  result |= i2c_write_byte_data(LTC3335_BASE_ADDRESS, subaddress, *ltc3335_data_ptr);
  return result;
}

// Reads from a LTC3335 register.
static int8_t ltc3335_get_register(uint8_t subaddress, uint8_t *ltc3335_data_ptr)
{
  int8_t result = 0;
  if (subaddress != ltc3335_subaddress_last)
  {
    result |= i2c_write_byte(LTC3335_BASE_ADDRESS, subaddress);
  }

  result |= i2c_read_byte(LTC3335_BASE_ADDRESS, ltc3335_data_ptr);
  return result;
}

// Encodes elements of register A.
static uint8_t ltc3335_encode_register_a(boolean enabled, LTC3335_OUTPUT_VOLTAGE_TYPE voltage, uint8_t prescaler)
{
  return ((enabled == true) ? (1 << 7) : 0) | (voltage << 4) | prescaler;
}

// Decodes elements of register A.
static void ltc3335_decode_register_a(uint8_t register_a)
{
  ltc3335_voltage_selection_enabled_last = ((register_a & MASK(1, 7)) == MASK(1, 7));
  ltc3335_voltage_selection_last = (LTC3335_OUTPUT_VOLTAGE_TYPE) ((register_a & MASK(3, 4)) >> 4);
  ltc3335_prescaler_last = register_a & MASK(4, 0);
  return;
}
