/*!
LTC2607: 16-Bit, Dual Rail-to-Rail DACs with I2C Interface.
LTC2609: Quad 16-/14-/12-Bit Rail-to-Rail DACs with I²C Interface.
LTC2606: 16-Bit Rail-to-Rail DACs with I²C Interface.

@verbatim

The LTC2607/LTC2617/LTC2627 are dual 16-, 14- and 12-bit, 2.7V to 5.5V
rail-to-rail voltage output DACs in a 12-lead DFN package. They have built-in
high performance output buffers and are guaranteed monotonic.

These parts establish new board-density benchmarks for 16- and 14-bit DACs and
advance performance standards for output drive and load regulation in single-
supply, voltage-output DACs.

The parts use a 2-wire, I2C compatible serial interface. The
LTC2607/LTC2617/LTC2627 operate in both the standard mode (clock rate of 100kHz)
and the fast mode (clock rate of 400kHz). An asynchronous DAC update pin (LDAC)
is also included.

The LTC2607/LTC2617/LTC2627 incorporate a power-on reset circuit. During power-
up, the voltage outputs rise less than 10mV above zero scale; and after power-
up, they stay at zero scale until a valid write and update take place. The
power-on reset circuit resets the LTC2607-1/LTC2617-1/ LTC2627-1 to mid-scale.
The voltage outputs stay at midscale until a valid write and update takes place.

@endverbatim

http://www.linear.com/product/LTC2607
http://www.linear.com/product/LTC2609
http://www.linear.com/product/LTC2606

http://www.linear.com/product/LTC2607#demoboards
http://www.linear.com/product/LTC2609#demoboards
http://www.linear.com/product/LTC2606#demoboards


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

//! @ingroup Digital_to_Analog_Converters
//! @{
//! @defgroup LTC2607 LTC2607: 16-Bit, Dual Rail-to-Rail DACs with I2C Interface
//! @}

/*! @file
    @ingroup LTC2607
    Library for LTC2607: 16-Bit, Dual Rail-to-Rail DACs with I2C Interface
*/

#include <Arduino.h>
#include <stdint.h>
#include <math.h>
#include "Linduino.h"
//#include "LT_I2C.h"
#include "LTC2607.h"
#include <Wire.h>

#include "USE_WIRE.h"

#ifdef USEWIRE
#include "LT_I2C_Wire.h"
#else
#include "LT_I2C.h"
#endif

// Writes command, DAC address, and DAC code to the LTC2607.
// Configures command (write, update, power down, etc.), address (DAC A, DAC B, or BOTH), and 16-bit dac_code for output voltage.
int8_t LTC2607_write(uint8_t i2c_address, uint8_t dac_command, uint8_t dac_address, uint16_t dac_code)
{
  int8_t ack;

  ack = i2c_write_word_data(i2c_address, (dac_command | dac_address), dac_code);
  return(ack);
}

// Calculates an LTC2607 DAC code for the desired output voltage.
// Based on the desired output voltage, the offset, and lsb parameters, return the corresponding DAC code that should be written to the LTC2607.
uint16_t LTC2607_voltage_to_code(float dac_voltage, float LTC2607_lsb, int32_t LTC2607_offset)
{
  int32_t dac_code;
  float float_code;
  float_code = dac_voltage / LTC2607_lsb - LTC2607_offset;                                        //! 1) Calculate the DAC code
  float_code = (float_code > (floor(float_code) + 0.5)) ? ceil(float_code) : floor(float_code);   //! 2) Round
  dac_code = (uint16_t) (float_code);                                                             //! 3) Convert to unsigned integer
  if (dac_code > 65535)
    dac_code = 65535;
  if (dac_code < 0)
    dac_code = 0;
  return ((uint16_t)dac_code);
}

// Calculates the LTC2607 offset and lsb voltage given two measured voltages and their corresponding DAC codes.
// Prior to calling this function, write two DAC codes to the LTC2607, and measure the output voltage for each DAC code.
// When passed the DAC codes and measured voltages as parameters, this function calculates the calibrated lsb and offset values.
void LTC2607_calibrate(uint16_t dac_code1, uint16_t dac_code2, float voltage1, float voltage2, float *LTC2607_lsb, int32_t *LTC2607_offset)
{
  float temp_offset;
  *LTC2607_lsb = (voltage2 - voltage1) / ((float) (dac_code2 - dac_code1));                             //! 1) Calculate the LSB
  temp_offset = voltage1/(*LTC2607_lsb) - dac_code1;                                                    //! 2) Calculate the offset
  temp_offset = (temp_offset > (floor(temp_offset) + 0.5)) ? ceil(temp_offset) : floor(temp_offset);    //! 3) Round
  *LTC2607_offset = (int32_t)temp_offset;                                                               //! 4) Cast as int32_t
}
