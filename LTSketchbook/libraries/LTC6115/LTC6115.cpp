/*!
 * CHANGE ALL THIS HEADER STUFF !!!
 * 
 * 
  LTC6804-1 Multicell Battery Monitor
@verbatim
  The LTC6804 is a 3rd generation multicell battery stack
  monitor that measures up to 12 series connected battery
  cells with a total measurement error of less than 1.2mV. The
  cell measurement range of 0V to 5V makes the LTC6804
  suitable for most battery chemistries. All 12 cell voltages
  can be captured in 290uS, and lower data acquisition rates
  can be selected for high noise reduction.

  Using the LTC6804-1, multiple devices are connected in
  a daisy-chain with one host processor connection for all
  devices.
@endverbatim
REVISION HISTORY
$Revision: 3659 $
$Date: 2013-12-13

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
//! @defgroup LTC6115 LTC6115: High-Voltage High Side Current and Voltage Sense

/*! @file
    @ingroup LTC6115
    Library for LTC6115 High-Voltage High Side Current and Voltage Sense
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC6115.h"
#include <SPI.h>


/*!
  6115 Primary  Variables
*/
float LTC6115_VGain = 0;
float LTC6115_IGain = 0;


// Function Definitions

//! This function will initialize 6115 variables.
//! @return void
void LTC6115_initialize()
{
  LTC6115_VGain = 2; // default buffer config
  LTC6115_IGain = 5; // 5V/A default

  // debug
  //Serial.println(MAXADCVAL);
}

//! This function returns the voltage gain (V/V).
//! @return float
float getVGain()
{
  float temp;
  temp = LTC6115_VGain;
  return temp;
}

//! This function returns the current gain (V/A).
//! @return float
float getIGain()
{
  float temp;
  temp = LTC6115_IGain;
  return temp;
}

//! This function converts ADC code to Voltage.
//! @return float
float LTC6115ConvertToVolts(uint32_t val)
{
  float temp;
 
  temp = (float)val / (float)MAXADCVAL * (float) AREF;
  Serial.print("Converted Voltage:   ");
  Serial.println(temp);
  return temp / (float) LTC6115_VGain * float(40); /* 6115 Vsense fixed input divider 1:40 */
}

//! This function converts ADC code to Voltage without the converted voltage print statement.
//! @return float
float LTC6115ToVolts(uint32_t val)
{
  float temp;
  temp = (float)val / (float)MAXADCVAL * (float) AREF;
  return temp / (float) LTC6115_VGain * float(40); /* 6115 Vsense fixed input divider 1:40 */
}

//! This function converts ADC code to Current.
//! @return float
float LTC6115ConvertToAmps(uint32_t val)
{
  float temp;
  temp = (float)val / (float)MAXADCVAL * (float) AREF;
  Serial.print("Converted Current:   ");
  Serial.println(temp);
  return temp / (float) LTC6115_IGain;
}

//! This function converts ADC code to Current without the converted current print statement.
//! @return float
float LTC6115ToAmps(uint32_t val)
{
  float temp; 
  temp = (float)val / (float)MAXADCVAL * (float) AREF;
  return temp / (float) LTC6115_IGain;
}

//! This function changes the Voltage gain.
//! @return void
void LTC6115ChangeVGain(float val)
{
  LTC6115_VGain = val;
}

//! This function changes the Current gain.
//! @return float
void LTC6115ChangeIGain(float val)
{
  LTC6115_IGain = val;
}

//! This function converts milliseconds to frequency (Hz).
//! @return uint32_t
uint32_t millsToHz(uint32_t val)
{
  float temp;
  float seconds;
  uint32_t fr;
  
  seconds = (float) val / 1000.0;
  temp = 1 / seconds;
  fr = (uint32_t) round(temp);
  return fr;
}





