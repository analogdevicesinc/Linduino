/*!
LTC3335: Nanopower Buck-Boost DC/DC with Integrated Coulomb Counter

This header file contains the definitions that allow configuration
of the LTC3335 driver.  The definitions belong to two groups:

  1. The first set of definitions configure the driver for the
     IPEAK, VOUT, and battery capacity for the application.  Advanced
     features such as a battery current measurement and software correction
     can also be enabled.

  2. The LTC3335 driver was written so that no hardware references are made
     in the .cpp file.  Instead, this file records what links to hardware
     functions must be defined for the LTC3335 driver to operate.  Change
     these definitions to configure the LTC3335.c driver for a microprocessor
     other than the Linduino.

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
    Configuration Header File for LTC3335: Nanopower Buck-Boost DC/DC with Integrated Coulomb Counter
*/

#ifndef LTC3335_CONFIG_H
#define LTC3335_CONFIG_H

#include <Arduino.h>                  // provides boolean type.
#include "Linduino.h"                 // provides common macros.
#include "LT_I2C.h"                   // provides interface to i2c.

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Configuration definitions for the LTC3335.c driver.
// Define this to specify the LTC3335 configuration, battery characteristics, and advanced features to be used.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#define LTC3335_IPEAK_CONFIGURATION       LTC3335_IPEAK_CONFIGURATION_100MA //!< IPEAK setting from LTC3335_IPEAK_CONFIGURATION_TYPE in LTC3335.h
#define LTC3335_OUTPUT_VOLTAGE            LTC3335_OUTPUT_VOLTAGE_3_3V       //!< VOUT setting from LTC3335_NUM_OUTPUT_VOLTAGES in LTC3335.h
#define LTC3335_CAPACITY                  (2400)                            //!< in mAh, capacity of the battery
#define LTC3335_ALARM_CAPACITY            0.9*LTC3335_CAPACITY              //!< in mAh, the capacity at which the alarm should be activated.

#define LTC3335_USE_CURRENT_MEASUREMENT   true                              //!< Set to true to use the /IRQ pin to measure the battery current real time.

#define LTC3335_USE_SOFTWARE_CORRECTION   false                             //!< Set to true to use software correction of coulomb count and current measurement.
#define LTC3335_VBAT_TYP                  3600                              //!< in mV, the nominal battery voltage expected for the majority of the battery discharge.
#define LTC3335_CORRECTION_FACTOR_TYP     -487                              //!< Value of LTC3335_Software_Correction_Table[VBAT_TO_TABLE_INDEX(LTC3335_VBAT_TYP)].
//!< Arduino compiler is not smart enough to lookup the value in the const table when
//!< optimizing and makes a lot of runtime code if you don't #define the value manually.

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Hardware definitions for the LTC3335.c driver.
// Change these definitions to configure the LTC3335.c driver for a microprocessor other than the Linduino.
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//! @name Input Read Definitions
//! @{
//! Macros to read LTC3335 Inputs, defined to interface to the Arduino.
#define LTC3335_PGOOD_READ()             digitalRead(2)
#define LTC3335_NIRQ_READ()              digitalRead(5)
//! @}

//! @name Timing Definitions
//! @{
//! Macro to get timing data from microprocessor, defined to interface to the Arduino function which uses ATMega328P Timer0.
//! The timer resolution is specified in timer counts per sec.
#define LTC3335_TIMER_GET()               micros()
#define LTC3335_TIMER_COUNTS_PER_SEC      1000000
//! @}

//! @name Counter Definitions
//! @{
//! Macro to get hardware counter data from microprocess, defined to interface to function in ATmega_Counter.h which uses ATMega328P Timer1.
//! The number of bits of the hardware timer is specified in number of bits.
#define LTC3335_COUNTER_GET()             TCNT1
#define LTC3335_COUNTER_SIZE              16
//! @}

//! @name Hardware Initialization
//! @{
//! Macro to init micro hardware that is used by LTC3335 driver.
#define LTC3335_MICRO_INIT()              {                                                                                       \
                                            /* Setup GPIO used by LTC3335  */                                                     \
                                            pinMode(2, INPUT);                                                                    \
                                            pinMode(5, INPUT);                                                                    \
                                            /* Setup ATMega counter counter according to Atmel datasheet 8271G, section 16.11. */ \
                                            /* Turn off output compare functions for counter. */                                  \
                                            TCCR1A = 0;                                                                           \
                                            /* Configure to count rising edges on T1 pin. */                                      \
                                            TCCR1B =  (1<<CS10) + (1<<CS11) + (1<<CS12);                                          \
                                            /* Start with counter cleared.*/                                                      \
                                            TCNT1 = 0;                                                                            \
                                            /* Enable I2C module.*/                                                               \
                                            i2c_enable();                                                                         \
                                          }
//! @}

#endif  // LTC3335_CONFIG_H



