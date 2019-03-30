/*!
LTC6115 High Voltage High Side Current and Voltage Sense

@verbatim

The LTC6115 is a versatile combination of a high voltage, high side current
sense amplifier and a voltage sense amplifier.

@endverbatim

https://www.analog.com/en/products/ltc6115.html


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

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Analog Devices Inc.

The Analog Devices Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.

Copyright 2018 Analog Devices Inc. (ADI)

*/


//! @ingroup Power_Monitors
//! @{
//! @defgroup LTC6115 LTC6115: High-Voltage High Side Current and Voltage Sense
//! @}

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
  LTC6115_VGain = 2; // 2.0 V/V default
  LTC6115_IGain = 2.5; // 2.5 V/A default, equiv to 50 V/V with Rsense = 50mOhm
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





