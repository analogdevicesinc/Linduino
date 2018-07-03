/************************************
// CHANGE ALL THIS HEADER STUFF!!

REVISION HISTORY
$Revision: 3659 $
$Date: 2017-04-13

Copyright (c) 2018, Analog Devices Inc. (ADI)
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
either expressed or implied, of Analog Devices Inc.

The Analog Devices Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.

Copyright 2018 Analog Devices Inc. (ADI)
***********************************************************/

/*! @file
    @ingroup LTC6115
    Header for LTC6115 High-V High-side Isense and Vsense
*/

#ifndef LTC6115_H
#define LTC6115_H

#define NUMBITS 10     // Default of Linduino AtMega
#define MAXADCVAL 1023 // ((2^NUMBITS) - 1)
#define AREF float(5)  // ADC reference voltage, default to +5V on Linduino

/*!
 Pin Mapping
*/
#define LTC6115_VIN 3  //!< Maps to DC2026 J7.4, ADC channel 4
#define LTC6115_IIN 0  //!< Maps to DC2026 J7.1, ADC channel 1

/*!
 |Function Declarations|
*/

//! This function will initialize 6115 variables
//! @return void
void LTC6115_initialize();

//! This function converts ADC code to Voltage.
//! @return float
float LTC6115ConvertToVolts(uint32_t val //!< The ADC code value to convert to Voltage
                           );

//! This function converts ADC code to Current.
//! @return float
float LTC6115ConvertToAmps(uint32_t val //!< The ADC code value to convert to Current
                          );

//! This function changes the Voltage gain
//! @return void
void  LTC6115ChangeVGain(float val //!< The value to change the Voltage gain to
                        );

//! This function changes the Current gain.
//! @return float
void  LTC6115ChangeIGain(float val //!< The value to change the Current gain to
                        );
//! This function converts ADC code to Voltage without the divider.
//! @return float
float LTC6115ToVolts(uint32_t val);

//! This function converts ADC code to Current without the divider.
//! @return float
float LTC6115ToAmps(uint32_t val);

//! This function converts milliseconds to frequency (Hz).
//! @return uint32_t
uint32_t millsToHz(uint32_t val //!< The milliseconds to convert to Hz
                  );

//! This function returns the voltage gain (V/V)
//! @return float
float getVGain();

//! This function returns the current gain (V/A)
//! @return float
float getIGain();


#endif
