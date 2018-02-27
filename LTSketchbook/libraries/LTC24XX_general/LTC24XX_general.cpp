/*!
LTC24XX General Library: Functions and defines for all SINC4 Delta Sigma ADCs.

@verbatim

These functions and defines apply to all No Latency Delta Sigmas in the
LTC2480 EasyDrive family, LTC2410 differential family, LTC2400 single-ended family,
and the LTC2440 High Speed family with selectable speed / resolution.

It does not cover the LTC2450 tiny, low cost delta sigma ADC famliy.

Please refer to the No Latency Delta Sigma ADC selector guide available at:

http://www.linear.com/docs/41341


@endverbatim

http://www.linear.com/product/LTC2449

http://www.linear.com/product/LTC2449#demoboards


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
//! @defgroup LTC24XX LTC24XX: All no-latency delta sigma ADCs with SINC4 rejection
//! @}

/*! @file
    @ingroup LTC24XX
    Library for LTC24XX no-latency delta sigma ADCs with SINC4 rejection
*/

#include <stdint.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include <SPI.h>
#include "LTC24XX_general.h"
#include "USE_WIRE.h"

#ifdef USEWIRE
#include "LT_I2C_Wire.h"
#else
#include "LT_I2C.h"
#endif

int8_t LTC24XX_EOC_timeout(uint8_t cs, uint16_t miso_timeout)
// Checks for EOC with a specified timeout (ms)
{
  uint16_t timer_count = 0;             // Timer count for MISO
  output_low(cs);                       //! 1) Pull CS low
  while (1)                             //! 2) Wait for SDO (MISO) to go low
  {
    if (input(MISO) == 0) break;        //! 3) If SDO is low, break loop
    if (timer_count++>miso_timeout)     // If timeout, return 1 (failure)
    {
      output_high(cs);                  // Pull CS high
      return(1);
    }
    else
      delay(1);
  }
  output_high(cs);                  // Pull CS high
  return(0);
}

// Reads from LTC24XX ADC that has no configuration word and a 32 bit output word.
void LTC24XX_SPI_32bit_data(uint8_t cs, int32_t *adc_code)
{
  LT_union_int32_4bytes data, command;  // LTC2449 data and command
  command.LT_uint32 = 0;                // Set to zero, not necessary but avoids
  // random data in scope shots.
  output_low(cs);                                                       //! 1) Pull CS low

  spi_transfer_block(cs, command.LT_byte, data.LT_byte, (uint8_t)4);    //! 2) Transfer arrays

  output_high(cs);                                                      //! 3) Pull CS high
  *adc_code = data.LT_int32;
}


// Reads from a SPI LTC24XX device that has an 8 bit command and a 32 bit output word.
void LTC24XX_SPI_8bit_command_32bit_data(uint8_t cs, uint8_t adc_command, int32_t *adc_code)
{
  LT_union_int32_4bytes data, command;  // LTC2449 data and command
  command.LT_byte[3] = adc_command;
  command.LT_byte[2] = 0;
  command.LT_byte[1] = 0;
  command.LT_byte[0] = 0;

  output_low(cs);                                                       //! 1) Pull CS low

  spi_transfer_block(cs, command.LT_byte, data.LT_byte, (uint8_t)4);    //! 2) Transfer arrays

  output_high(cs);                                                      //! 3) Pull CS high
  *adc_code = data.LT_int32;
}


// Reads from a SPI LTC24XX device that has a 16 bit command and a 32 bit output word.
void LTC24XX_SPI_16bit_command_32bit_data(uint8_t cs, uint8_t adc_command_high, uint8_t adc_command_low, int32_t *adc_code)
{


  LT_union_int32_4bytes data, command;  // LTC24XX data and command
  command.LT_byte[3] = adc_command_high;
  command.LT_byte[2] = adc_command_low;
  command.LT_byte[1] = 0;
  command.LT_byte[0] = 0;

  output_low(cs);                                                       //! 1) Pull CS low
  spi_transfer_block(cs, command.LT_byte, data.LT_byte, (uint8_t)4);    //! 2) Transfer arrays
  output_high(cs);                                                      //! 3) Pull CS high
  *adc_code = data.LT_int32;
}

//! Reads from LTC24XX two channel "Ping-Pong" ADC, placing the channel information in the adc_channel parameter
//! and returning the 32 bit result with the channel bit cleared so the data format matches the rest of the family
//! @return void
void LTC24XX_SPI_2ch_ping_pong_32bit_data(uint8_t cs, uint8_t *adc_channel, int32_t *code)
{
  LT_union_int32_4bytes data, command;      // ADC data

  command.LT_int32 = 0x00000000;  // This is a "don't care"

  spi_transfer_block(cs, command.LT_byte , data.LT_byte, (uint8_t)4);
  if (data.LT_byte[3] & 0x40) // Obtains Channel Number
  {
    *adc_channel = 1;
  }
  else
  {
    *adc_channel = 0;
  }
  data.LT_byte[3] &= 0x3F;                  // Clear channel bit here so code to voltage function doesn't have to.
  *code = data.LT_int32;                    // Return data
}

//! Reads from LTC24XX ADC that has no configuration word and returns a 32 bit result.
//! @return  void
void LTC24XX_SPI_24bit_data(uint8_t cs, int32_t *adc_code)
{
  LT_union_int32_4bytes data, command;  // LTC24XX data and command
  command.LT_int32 = 0;

  output_low(cs);                                                       //! 1) Pull CS low
  spi_transfer_block(cs, command.LT_byte, data.LT_byte, (uint8_t)3);    //! 2) Transfer arrays
  output_high(cs);                                                      //! 3) Pull CS high

  data.LT_byte[3] = data.LT_byte[2]; // Shift bytes up by one. We read out 24 bits,
  data.LT_byte[2] = data.LT_byte[1]; // which are loaded into bytes 2,1,0. Need to left-
  data.LT_byte[1] = data.LT_byte[0]; // justify.
  data.LT_byte[0] = 0x00;

  *adc_code = data.LT_int32;
}

//! Reads from LTC24XX ADC that accepts an 8 bit configuration and returns a 24 bit output word.
//! @return  void
void LTC24XX_SPI_8bit_command_24bit_data(uint8_t cs, uint8_t adc_command, int32_t *adc_code)
{
  LT_union_int32_4bytes data, command;  // LTC24XX data and command
  command.LT_byte[2] = adc_command;
  command.LT_byte[1] = 0;
  command.LT_byte[0] = 0;

  output_low(cs);                                                       //! 1) Pull CS low
  spi_transfer_block(cs, command.LT_byte, data.LT_byte, (uint8_t)3);    //! 2) Transfer arrays
  output_high(cs);                                                      //! 3) Pull CS high

  data.LT_byte[3] = data.LT_byte[2]; // Shift bytes up by one. We read out 24 bits,
  data.LT_byte[2] = data.LT_byte[1]; // which are loaded into bytes 2,1,0. Need to left-
  data.LT_byte[1] = data.LT_byte[0]; // justify.
  data.LT_byte[0] = 0x00;

  *adc_code = data.LT_int32;
}

//! Reads from LTC24XX ADC that accepts a 16 bit configuration and returns a 24 bit output word.
//! @return  void
void LTC24XX_SPI_16bit_command_24bit_data(uint8_t cs, uint8_t adc_command_high, uint8_t adc_command_low, int32_t *adc_code)
{
  LT_union_int32_4bytes data, command;  // LTC24XX data and command
  command.LT_byte[2] = adc_command_high;
  command.LT_byte[1] = adc_command_low;
  command.LT_byte[0] = 0;


  output_low(cs);                                                       //! 1) Pull CS low
  spi_transfer_block(cs, command.LT_byte, data.LT_byte, (uint8_t)3);    //! 2) Transfer arrays
  output_high(cs);                                                      //! 3) Pull CS high

  data.LT_byte[3] = data.LT_byte[2]; // Shift bytes up by one. We read out 24 bits,
  data.LT_byte[2] = data.LT_byte[1]; // which are loaded into bytes 2,1,0. Need to left-
  data.LT_byte[1] = data.LT_byte[0]; // justify.
  data.LT_byte[0] = 0x00;

  *adc_code = data.LT_int32;
}

//! Reads from LTC24XX two channel "Ping-Pong" ADC, placing the channel information in the adc_channel parameter
//! and returning the 24 bit result with the channel bit cleared so the data format matches the rest of the family
//! @return void
void LTC24XX_SPI_2ch_ping_pong_24bit_data(uint8_t cs, uint8_t *adc_channel, int32_t *code)
{
  LT_union_int32_4bytes data, command;      // ADC data

  command.LT_int32 = 0x00000000;  // This is a "don't care"

  spi_transfer_block(cs, command.LT_byte , data.LT_byte, (uint8_t)3);
  data.LT_byte[3] = data.LT_byte[2]; // Shift bytes up by one. We read out 24 bits,
  data.LT_byte[2] = data.LT_byte[1]; // which are loaded into bytes 2,1,0. Need to left-
  data.LT_byte[1] = data.LT_byte[0]; // justify.
  data.LT_byte[0] = 0x00;

  if (data.LT_byte[3] & 0x40) // Obtains Channel Number
  {
    *adc_channel = 1;
  }
  else
  {
    *adc_channel = 0;
  }
  data.LT_byte[3] &= 0x3F;                  // Clear channel bit here so code to voltage function doesn't have to.
  *code = data.LT_int32;                    // Return data
}


//I2C functions

//! Reads from LTC24XX ADC that accepts an 8 bit configuration and returns a 24 bit result.
//! @return  Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC24XX_I2C_8bit_command_24bit_data(uint8_t i2c_address, uint8_t adc_command, int32_t *adc_code, uint16_t eoc_timeout)
{
  int8_t ack;
  uint16_t timer_count = 0;        // Timer count to wait for ACK
  int8_t buf[4];
  LT_union_int32_4bytes data;      // LTC24XX data
  while (1)
  {
    ack = i2c_read_block_data(i2c_address, adc_command, 3, data.LT_byte);
    if (!ack) break; // !ack indicates success
    if (timer_count++>eoc_timeout)     // If timeout, return 1 (failure)
      return(1);
    else
      delay(1);
  }
  data.LT_byte[3] = data.LT_byte[2]; // Shift bytes up by one. We read out 24 bits,
  data.LT_byte[2] = data.LT_byte[1]; // which are loaded into bytes 2,1,0. Need to left-
  data.LT_byte[1] = data.LT_byte[0]; // justify.
  data.LT_byte[0] = 0x00;
  data.LT_uint32 >>= 2;  // Shifts data 2 bits to the right; operating on unsigned member shifts in zeros.
  data.LT_byte[3] = data.LT_byte[3] & 0x3F; // Clear upper 2 bits JUST IN CASE. Now the data format matches the SPI parts.
  *adc_code = data.LT_int32;
  return(ack); // Success
}



//! Reads from LTC24XX ADC that has no configuration word and returns a 32 bit result.
//! Data is formatted to match the SPI devices, with the MSB in the bit 28 position.
//! @return  Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC24XX_I2C_32bit_data(uint8_t i2c_address,                   //!< I2C address of device
                              int32_t *adc_code,                      //!< 4 byte conversion code read from LTC24XX
                              uint16_t eoc_timeout                     //!< Timeout (in milliseconds)
                             )
{
  int8_t ack;
  uint16_t timer_count = 0;        // Timer count to wait for ACK
  int8_t buf[4];
  LT_union_int32_4bytes data;      // LTC24XX data
  while (1)
  {
    ack = i2c_read_block_data(i2c_address, 4, data.LT_byte);
    if (!ack) break; // !ack indicates success
    if (timer_count++>eoc_timeout)     // If timeout, return 1 (failure)
      return(1);
    else
      delay(1);
  }

  data.LT_uint32 >>= 2;  // Shifts data 2 bits to the right; operating on unsigned member shifts in zeros.
  data.LT_byte[3] = data.LT_byte[3] & 0x3F; // Clear upper 2 bits JUST IN CASE. Now the data format matches the SPI parts.
  *adc_code = data.LT_int32;
  return(ack); // Success
}


//! Reads from LTC24XX ADC that accepts an 8 bit configuration and returns a 32 bit result.
//! @return  Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC24XX_I2C_8bit_command_32bit_data(uint8_t i2c_address, uint8_t adc_command, int32_t *adc_code, uint16_t eoc_timeout)
{
  int8_t ack;
  uint16_t timer_count = 0;        // Timer count to wait for ACK
  int8_t buf[4];
  LT_union_int32_4bytes data;      // LTC24XX data
  while (1)
  {
    ack = i2c_read_block_data(i2c_address, adc_command, 4, data.LT_byte);
    if (!ack) break; // !ack indicates success
    if (timer_count++>eoc_timeout)     // If timeout, return 1 (failure)
      return(1);
    else
      delay(1);
  }

  data.LT_uint32 >>= 2;  // Shifts data 2 bits to the right; operating on unsigned member shifts in zeros.
  data.LT_byte[3] = data.LT_byte[3] & 0x3F; // Clear upper 2 bits JUST IN CASE. Now the data format matches the SPI parts.
  *adc_code = data.LT_int32;
  return(ack); // Success
}

//! Reads from LTC24XX ADC that accepts a 16 bit configuration and returns a 32 bit result.
//! @return  Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC24XX_I2C_16bit_command_32bit_data(uint8_t i2c_address,uint8_t adc_command_high,
    uint8_t adc_command_low,int32_t *adc_code,uint16_t eoc_timeout)
{
  int8_t ack;
  uint16_t adc_command, timer_count = 0;        // Timer count to wait for ACK
  int8_t buf[4];
  LT_union_int32_4bytes data;      // LTC24XX data
  adc_command = (adc_command_high << 8) | adc_command_low;
  while (1)
  {
    ack = i2c_two_byte_command_read_block(i2c_address, adc_command, 4, data.LT_byte);
    if (!ack) break; // !ack indicates success
    if (timer_count++>eoc_timeout)     // If timeout, return 1 (failure)
      return(1);
    else
      delay(1);
  }

  data.LT_uint32 >>= 2;  // Shifts data 2 bits to the right; operating on unsigned member shifts in zeros.
  data.LT_byte[3] = data.LT_byte[3] & 0x3F; // Clear upper 2 bits JUST IN CASE. Now the data format matches the SPI parts.
  *adc_code = data.LT_int32;
  return(ack); // Success
}

// Calculates the voltage corresponding to an adc code, given the reference voltage (in volts)
float LTC24XX_SE_code_to_voltage(int32_t adc_code, float vref)
{
  float voltage;
  adc_code -= 0x20000000;             //! 1) Subtract offset
  voltage=(float) adc_code;
  voltage = voltage / 268435456.0;    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
  voltage = voltage * vref;           //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)
  return(voltage);
}

// Calculates the voltage corresponding to an adc code, given the reference voltage (in volts)
// This function handles all differential input parts, including the "single-ended" mode on multichannel
// differential parts. Data from I2C parts must be right-shifted by two bit positions such that the MSB
// is in bit 28 (the same as the SPI parts.)
float LTC24XX_diff_code_to_voltage(int32_t adc_code, float vref)
{
  float voltage;

#ifndef SKIP_EZDRIVE_2X_ZERO_CHECK
  if (adc_code == 0x00000000)
  {
    adc_code = 0x20000000;
  }
#endif

  adc_code -= 0x20000000;             //! 1) Converts offset binary to binary
  voltage=(float) adc_code;
  voltage = voltage / 536870912.0;    //! 2) This calculates the input as a fraction of the reference voltage (dimensionless)
  voltage = voltage * vref;           //! 3) Multiply fraction by Vref to get the actual voltage at the input (in volts)
  return(voltage);
}

// Calculates the voltage corresponding to an adc code, given lsb weight (in volts) and the calibrated
// adc offset code (zero code that is subtracted from adc_code). For use with the LTC24XX_cal_voltage() function.
float LTC24XX_diff_code_to_calibrated_voltage(int32_t adc_code, float LTC2449_lsb, int32_t LTC2449_offset_code)
{
  float adc_voltage;

#ifndef SKIP_EZDRIVE_2X_ZERO_CHECK
  if (adc_code == 0x00000000)
  {
    adc_code = 0x20000000;
  }
#endif

  adc_code -= 536870912;                                            //! 1) Converts offset binary to binary
  adc_voltage=(float)(adc_code+LTC2449_offset_code)*LTC2449_lsb;    //! 2) Calculate voltage from ADC code, lsb, offset.
  return(adc_voltage);
}


// Calculate the lsb weight and offset code given a full-scale code and a measured zero-code.
void LTC24XX_calibrate_voltage(int32_t zero_code, int32_t fs_code, float zero_voltage, float fs_voltage, float *LTC24XX_lsb, int32_t *LTC24XX_offset_code)
{
  zero_code -= 536870912;   //! 1) Converts zero code from offset binary to binary
  fs_code -= 536870912;     //! 2) Converts full scale code from offset binary to binary

  float temp_offset;
  *LTC24XX_lsb = (fs_voltage-zero_voltage)/((float)(fs_code - zero_code));                              //! 3) Calculate the LSB

  temp_offset = (zero_voltage/ *LTC24XX_lsb) - zero_code;                                               //! 4) Calculate Unipolar offset
  temp_offset = (temp_offset > (floor(temp_offset) + 0.5)) ? ceil(temp_offset) : floor(temp_offset);    //! 5) Round
  *LTC24XX_offset_code = (int32_t)temp_offset;                                                          //! 6) Cast as int32_t
}
