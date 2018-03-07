//! @todo Move to "User Contributed" and change name from FAE..., document with Doxygen, etc.
/*
FAE Demonstration of DC934A and RGB Diode

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
    @ingroup LTC2607
    DC934A Science Project
    FAE Meeting April 2013
*/

#include <Arduino.h>  // Always include this in Arduino programs.
#include <stdint.h>   // Standard 'C' library that defines fixed-length datatypes 
#include "Linduino.h" // Include this in every Linduino program.
#include "LT_I2C.h"   // Functions to talk to I2C parts through QuikEval connector
#include "LT_SPI.h"   // Functions to talk to SPI 
#include "LTC2607.h"
#include "LTC2422.h"
#include "Wire.h"
#include "SPI.h"

const uint16_t LTC2422_timeout = 1000;  //!< The timeout for the LTC2422

//! Initialize Linduino once on power-up or reset.
void setup()
{

  Serial.begin(115200);       //! Initializes the serial port to the PC.

  output_high(LTC2422_CS);    //! Pull LTC2442 Chip Select High.

  quikeval_I2C_init();        //! Initializes Linduino I2C port.
  quikeval_I2C_connect();

  pinMode(2, OUTPUT);      //! Configure Arduino Pins 2, 3, and 4 as outputs.
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);

  uint16_t dac_code;
  //! Find LTC2607 dac_code that corresponds to 4.5V
  dac_code = LTC2607_voltage_to_code(4.5, LTC2607_TYPICAL_lsb, LTC2607_TYPICAL_OFFSET);
  //! Set DAC_A output to 4.5V
  LTC2607_write(LTC2607_I2C_GLOBAL_ADDRESS, LTC2607_WRITE_UPDATE_COMMAND, LTC2607_DAC_B, dac_code);
  //! Make DAC_B output high impedance (power down)
  LTC2607_write(LTC2607_I2C_GLOBAL_ADDRESS, LTC2607_POWER_DOWN_COMMAND, LTC2607_DAC_A, 0);

  quikeval_SPI_init();  //! Configures the SPI port for 4Mhz SCK.
  quikeval_SPI_connect();  //! Connects SPI to QuikEval port.

  uint8_t adc_channel;
  uint32_t  adc_code;

  if (LTC2422_EOC_timeout(LTC2422_CS, LTC2422_timeout))     // Check for EOC
    return;
  LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);    // Align channel by reading again if adc_channel=0

  if (adc_channel == 0)
  {
    if (LTC2422_EOC_timeout(LTC2422_CS, LTC2422_timeout))     // Check for EOC
      return;
    LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);
  }
}

void loop()
{
  static int32_t previous_adc_code; //stores ADC code from previous time through loop
  uint32_t adc_code;
  int32_t delta_adc_code;
  uint8_t adc_channel;

  if (LTC2422_EOC_timeout(LTC2422_CS, LTC2422_timeout))     // Check for EOC
    return;
  LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);    // Read channel 0 and throw it away

  if (LTC2422_EOC_timeout(LTC2422_CS, LTC2422_timeout))     // Check for EOC
    return;
  LTC2422_adc_read(LTC2422_CS, &adc_channel, &adc_code);    // Read channel 1 into adc_code
  delay(300);

  delta_adc_code = adc_code - previous_adc_code;

  uint8_t pin2=0, pin3=0, pin4=0;
  if (delta_adc_code > 30) pin2 = 1;
  if ((delta_adc_code <= 30) && (delta_adc_code >=-30)) pin3=1;
  if (delta_adc_code < -30) pin4=1;

  digitalWrite(2,pin2);
  digitalWrite(3,pin3);
  digitalWrite(4,pin4);

  Serial.println();
  Serial.print("RED:   ");      //! Print "RED:  " to serial port
  Serial.println(pin4);
  Serial.print("GREEN: ");
  Serial.println(pin3);
  Serial.print("BLUE:  ");
  Serial.println(pin2);

  previous_adc_code=adc_code;
}
