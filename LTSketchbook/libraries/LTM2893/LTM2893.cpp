/*!
LTM2893-1: 100MHz ADC SPI Isolator

@verbatim

@endverbatim

http://www.linear.com/product/LTM2893

http://www.linear.com/product/LTM2893#demoboards



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

//! @ingroup Analog_to_Digital_Converters
//! @{
//! @defgroup LTM2893 LTM2893: Isolator for 1Msps ADC
//! @}

/*! @file
    @ingroup LTM2893
    Library for LTM2893: Isolator for 16/18-Bit 1Msps ADC
*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTM2893.h"
#include <SPI.h>


// Reads from a SPI LTM2893-XX device that has no configuration word and a 20 bit output word in 2's complement format.
void LTM2893_read(uint8_t cs, int32_t *ptr_adc_code)
{
  LT_union_int16_2bytes command;    // LTM2893 data and command
  command.LT_uint16 = 0;                // Set to zero, not necessary but avoids
  // random data in scope shots.

//Form a four byte object to hold four bytes of data
  LT_union_int32_4bytes data; //instantiate the union
  data.LT_byte[3] = 0;
  data.LT_byte[2] = 0;
  data.LT_byte[1] = 0;
  data.LT_byte[0] = 0;

  spi_transfer_block(cs,command.LT_byte,data.LT_byte,3);  // read 3 bytes 2893 is setup as 24 bits. overreading will cause a fault.

  *ptr_adc_code = data.LT_int32;

  return;
}

void LTM2893_config(uint8_t user_config)
{
  uint8_t *rx_data;
  spi_transfer_byte(LTM2893_CSC_PIN,user_config,rx_data);
}


// Calculates the voltage corresponding to an adc code in 2's complement, given the reference voltage (in volts)
/*
float LTM2893_code_to_voltage(int32_t adc_code, float vref)
{
  float voltage;
  if (adc_code >= pow(2,23)) {
  voltage = (float)adc_code - pow(2,23);
  voltage = voltage / (pow(2,22)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
  voltage = voltage * -vref - (vref/pow(2,23));           //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)
  }
  else {
  voltage = (float)adc_code;
  voltage = voltage / (pow(2,22)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
  voltage = voltage * vref;           //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)
  }
  return(voltage);
}
*/
float LTM2893_code_to_voltage(int32_t adc_code, float vref)
{
  float voltage;
  if (adc_code >= pow(2,23))
  {
    voltage = (float)adc_code - pow(2,24);
    voltage = voltage / (pow(2,23)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
    voltage = voltage * vref;           //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)
  }
  else
  {
    voltage = (float)adc_code;
    voltage = voltage / (pow(2,23)-1);    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
    voltage = voltage * vref;           //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)
  }
  return(voltage);
}
