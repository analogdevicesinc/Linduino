/*!
LTC2874: Quad IO-Link Master Hot Swap Power Controller and PHY

@verbatim

The LTC2874 provides a rugged, 4-port IO-Link power and communications
interface to remote devices connected by cables up to 20m in length.

Output supply voltage and inrush current are ramped up in a controlled
manner  using  external N-channel MOSFETs, providing improved robustness
compared to fully integrated solutions.

Wake-up pulse generation, line noise suppression, connection sensing and
automatic restart after fault conditions are supported, along with
signaling at rates up to 4.8kb/s, 38.4kb/s, and 230.4kb/s.

Configuration and fault reporting are exchanged using a SPI-compatible
4-wire interface that operates at clock rates up to 20MHz.

@endverbatim

http://www.linear.com/product/LTC2874

http://www.linear.com/product/LTC2874#demoboards



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

IO-Link is a registered trademark of PROFIBUS User Organization (PNO).
*/

//! @ingroup Transceivers
//! @{
//! @defgroup LTC2874 LTC2874: Quad IO-Link Master Hot Swap Power Controller and PHY
//! @}

/*! @file
    @ingroup LTC2874
    Library for LTC2874: Quad IO-Link Master Hot Swap Power Controller and PHY
*/

#include <stdint.h>
#include <math.h>
#include <Arduino.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "LTC2874.h"



//************************************************************************************
//  Enables or Disables CQ output for specified port, then Updates.
//  parameters: port (1-4, or 5=ALL), value (0=Disable, 1=Enable)
//  returns: nothing
//************************************************************************************
void LTC2874_cq_output(uint8_t port, uint8_t value)
{
  uint8_t data_byte, command_byte;

  command_byte = LTC2874_READ | (LTC2874_CTRL1_REGD << 1);

  output_low(LTC2874_CS);               //! 1) Pull CS low
  spi_write(command_byte);              //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);           //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);

  if (port == 5)
  {
    port = 0xF;
  }
  else
  {
    port = 0x01 << (port - 1);      // make mask
  }

  if (value)
  {
    data_byte |= port;                // bitwise OR
  }
  else
  {
    data_byte &= ~port;
  }

  command_byte = LTC2874_WRITE_UPDATE_ALL | (LTC2874_CTRL1_REGD << 1);

  output_low(LTC2874_CS);              //! 1) Pull CS low to start transaction
  spi_write(command_byte);             //! 2) Write the command and address
  spi_write(data_byte);                //! 3) Write the data
  output_high(LTC2874_CS);             //! 4) Pull CS high to finish transaction
}


//************************************************************************************
//  Enables or Disables L+ output for specified port, then Updates.
//  parameters: port (1-4, or 5=ALL), value (0=Disable, 1=Enable)
//  returns: nothing
//************************************************************************************
void LTC2874_lplus_output(uint8_t port, uint8_t value)
{
  uint8_t data_byte, command_byte;

  command_byte = LTC2874_READ | (LTC2874_CTRL2_REGE << 1);

  output_low(LTC2874_CS);            //! 1) Pull CS low
  spi_write(command_byte);           //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);        //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);

  if (port == 5)
  {
    port = 0xF0;
  }
  else
  {
    port = 0x01 << (port + 3);         //make mask
  }

  if (value)
  {
    data_byte |= port;                   //bitwise OR
  }
  else
  {
    data_byte &= ~port;
  }

  command_byte = LTC2874_WRITE_UPDATE_ALL | (LTC2874_CTRL2_REGE << 1);

  output_low(LTC2874_CS);                //! 1) Pull CS low to start transaction
  spi_write(command_byte);               //! 2) Write the command and address
  spi_write(data_byte);                  //! 3) Write the data
  output_high(LTC2874_CS);               //! 4) Pull CS high to finish transaction
}


//************************************************************************************
//  Sets SIO_MODE bit (and also Clears SLEW bit) for specified port, then Updates.
//  parameter: port (1-4, or 5=ALL)
//  returns: nothing
//************************************************************************************
void LTC2874_sio_mode(uint8_t port)
{
  uint8_t data_byte_9, data_byte_E, command_byte_9, command_byte_E, position_9, position_E;

  if (port == 5)                       // If 5 set to 0x0F
  {
    position_E = 0x0f;
  }
  else
  {
    position_E = (0x01 << (port - 1));  // convert port number to position
  }

  position_9 = position_E << 4;          // move position for slew bits

  command_byte_9 = LTC2874_READ | (LTC2874_MODE2_REG9 << 1);
  command_byte_E = LTC2874_READ | (LTC2874_CTRL2_REGE << 1);

  output_low(LTC2874_CS);                //! 1) Pull CS low
  spi_write(command_byte_9);             //! 2) Write first byte and send command1 | address
  data_byte_9 = spi_read(0x00);          //! 3) Read last byte (while sending null data)
  spi_write(command_byte_E);             //! 4) Write first byte and send command2 | address
  data_byte_E = spi_read(0x00);          //! 5) Read last byte (while sending null data)
  output_high(LTC2874_CS);               //! 6) Pull CS high to finish transaction

  data_byte_9 &= ~position_9;            // SLEW=0
  data_byte_E |= position_E;             // SIO_MODE=1

  command_byte_9 = LTC2874_WRITE_NO_UPDATE | (LTC2874_MODE2_REG9 << 1);
  command_byte_E = LTC2874_WRITE_UPDATE_ALL | (LTC2874_CTRL2_REGE << 1);

  output_low(LTC2874_CS);                //! 1) Pull CS low to start transaction
  spi_write(command_byte_9);             //! 2) Write the command and address
  spi_write(data_byte_9);                //! 3) Write the data
  spi_write(command_byte_E);             //! 4) Write the command and address
  spi_write(data_byte_E);                //! 5) Write the data
  output_high(LTC2874_CS);               //! 6) Pull CS high to finish transaction
}

//************************************************************************************
//  Changes value of the 24VMODE bit, then Updates.
//  parameter: value (0=Disable, 1=Enable)
//  returns: nothing
//************************************************************************************
void LTC2874_24v_mode(uint8_t value)
{
  uint8_t command_byte, data_byte;
  command_byte = LTC2874_READ | (LTC2874_MODE1_REG8 << 1);

  output_low(LTC2874_CS);                //! 1) Pull CS low
  spi_write(command_byte);               //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);            //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);               //! 4) Pull CS high to finish transaction

  if (value)
  {
    data_byte |= LTC2874_24VMODE;        // set bit
  }
  else
  {
    data_byte &= ~LTC2874_24VMODE;     // clr bit
  }

  command_byte = LTC2874_WRITE_UPDATE_ALL | (LTC2874_MODE1_REG8 << 1);

  output_low(LTC2874_CS);                //! 1) Pull CS low to start transaction
  spi_write(command_byte);               //! 2) Write the command and address
  spi_write(data_byte);                  //! 3) Write the data
  output_high(LTC2874_CS);               //! 4) Pull CS high to finish transaction
}


//************************************************************************************
//  Changes NSF (Noise Suppression Filter) setting for specified port.
//  parameters: port#, value
//    port#: [1,2,3, or 4]
//    value: 0x0=disabled, 0x1=20.3us, 0x2=2.8us, 0x3=0.6us (default)
//  Higher bits are ignored. No update. Returns: nothing.
//************************************************************************************
void LTC2874_write_NSF_value(uint8_t port, uint8_t value)
{
  uint8_t command_byte , data_byte, position;
  command_byte = LTC2874_READ | (LTC2874_NSF_REGA << 1);    // Create command and address byte

  output_low(LTC2874_CS);               //! 1) Pull CS low
  spi_write(command_byte);              //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);           //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);              //! 4) Pull CS high to finish transaction

  position = ((port - 1) << 1);         // convert port number to position,Equivalent to multiply by 2
  data_byte &= ~(0x3<<position);        // clear NSF bits with inverse mask
  data_byte |= ((value) << position);   // bitwise OR value into NSF port position

  command_byte = LTC2874_WRITE_NO_UPDATE | (LTC2874_NSF_REGA << 1);

  output_low(LTC2874_CS);               //! 1) Pull CS low to start transaction
  spi_write(command_byte);              //! 2) Write the command and address
  spi_write(data_byte);                 //! 3) Write the data
  output_high(LTC2874_CS);              //! 4) Pull CS high to finish transaction
}


//************************************************************************************
//  Changes ILLM (Sinking current) setting for specified port.
//  parameters: port#, value
//    port#: (1-4, or 5=ALL)
//    value: 0x0=500kohm, 0x1=2.5mA, 0x2=3.7mA, 0x3=6.2mA (default)
//  Higher bits are ignored. No update. Returns: nothing.
//************************************************************************************
void LTC2874_write_ILLM_value(uint8_t port, uint8_t value)
{
  uint8_t command_byte , data_byte, position;
  // Create command and address byte
  command_byte = LTC2874_READ | (LTC2874_ILLM_REGB << 1);

  output_low(LTC2874_CS);                    //! 1) Pull CS low
  spi_write(command_byte);                   //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);                //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);                   //! 4) Pull CS high to finish transaction

  if (port == 5)                           // If 5 load each port same value
  {
    data_byte = value + (value << 2) + (value << 4) + (value << 6);
  }
  else
  {
    position = ((port - 1) << 1);          // convert port number to position
    data_byte &= ~(0x3<<position);         // clear ILLM bits with inverse mask
    data_byte |= ((value) << position);    // bitwise OR value into ILLM port position
  }

  command_byte = LTC2874_WRITE_NO_UPDATE | (LTC2874_ILLM_REGB << 1);

  output_low(LTC2874_CS);                    //! 1) Pull CS low to start transaction
  spi_write(command_byte);                   //! 2) Write the command and address
  spi_write(data_byte);                      //! 3) Write the data
  output_high(LTC2874_CS);                   //! 4) Pull CS high to finish transaction
}


//************************************************************************************
//  Change L+ Overcurrent Timer Control (LPTC) setting.
//  parameter: value (4 bits, 0x0-0xF)
//    0x0=17us, 0x1=30us, 0x2=60us, 0x3=120us, 0x4=0.25ms, 0x5=0.5ms, 0x6=1ms, 0x7=2ms
//    0x8=3.9ms, 0x9=7.8ms, 0xA=16ms, 0xB=30ms, 0xC=0.60ms, 0xD=0.13s, 0xE/0xF=0.25s
//  Higher bits are ignored. No update. Returns: nothing.
//************************************************************************************
void LTC2874_write_LPTC_value(uint8_t value)
{
  uint8_t command_byte , data_byte, position;
  // Create command and address byte
  command_byte = LTC2874_READ | (LTC2874_TMRCTRL_REGC << 1);

  output_low(LTC2874_CS);                  //! 1) Pull CS low
  spi_write(command_byte);                 //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);              //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);                 //! 4) Pull CS high to finish transaction

  data_byte &= ~(LTC2874_LPTC_msk);        // clear LPTC bits with inverse mask
  data_byte |= (LTC2874_LPTC(value));      // bitwise OR value into LPTC bits

  command_byte = LTC2874_WRITE_NO_UPDATE | (LTC2874_TMRCTRL_REGC << 1);

  output_low(LTC2874_CS);                  //! 1) Pull CS low to start transaction
  spi_write(command_byte);                 //! 2) Write the command and address
  spi_write(data_byte);                    //! 3) Write the data
  output_high(LTC2874_CS);                 //! 4) Pull CS high to finish transaction
}


//************************************************************************************
//  Changes Auto-Retry Timer Control (RETRYTC) setting.
//  parameter: value (0x0=0.12s, 0x1=0.24s, 0x2=0.49s, 0x3=0.98s, 0x4=2.0s, 0x5=3.9s, 0x6=7.9s, 0x7=15.7s)
//  Higher bits are ignored. No update. Returns: nothing.
//************************************************************************************
void LTC2874_write_RETRYTC_value(uint8_t value)
{
  uint8_t command_byte , data_byte, position;
  // Create command and address byte
  command_byte = LTC2874_READ | (LTC2874_TMRCTRL_REGC << 1);

  output_low(LTC2874_CS);                   //! 1) Pull CS low
  spi_write(command_byte);                  //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);               //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);                  //! 4) Pull CS high to finish transaction

  data_byte &= ~(LTC2874_RETRYTC_msk);      // clear RETRYTC bits with inverse mask
  data_byte |= (LTC2874_RETRYTC(value));    // bitwise OR value into RETRYTC bits

  command_byte = LTC2874_WRITE_NO_UPDATE | (LTC2874_TMRCTRL_REGC << 1);

  output_low(LTC2874_CS);                   //! 1) Pull CS low to start transaction
  spi_write(command_byte);                  //! 2) Write the command and address
  spi_write(data_byte);                     //! 3) Write the data
  output_high(LTC2874_CS);                  //! 4) Pull CS high to finish transaction
}


//************************************************************************************
//  Changes 2X Current Pulse Timer Control (2XPTC) setting.
//  parameter: value (0x0=60ms (default), 0x1=disabled, 0x2=30ms, 0x3=120ms)
//  Higher bits are ignored. No update. Returns: nothing.
//************************************************************************************
void LTC2874_write_2XPTC_value(uint8_t value)
{
  uint8_t command_byte , data_byte, position;
  // Create command and address byte
  command_byte = LTC2874_READ | (LTC2874_MODE1_REG8 << 1);

  output_low(LTC2874_CS);                   //! 1) Pull CS low
  spi_write(command_byte);                  //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);               //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);                  //! 4) Pull CS high to finish transaction

  data_byte &= ~(LTC2874_2XPTC_msk);        // clear 2XPTC bits with inverse mask
  data_byte |= (LTC2874_2XPTC(value));      // bitwise OR value into 2XPTC bits

  command_byte = LTC2874_WRITE_NO_UPDATE | (LTC2874_MODE1_REG8 << 1);

  output_low(LTC2874_CS);                   //! 1) Pull CS low to start transaction
  spi_write(command_byte);                  //! 2) Write the command and address
  spi_write(data_byte);                     //! 3) Write the data
  output_high(LTC2874_CS);                  //! 4) Pull CS high to finish transaction
}


//************************************************************************************
//  Changes VDD Overvoltage Threshold (OV_TH) setting.
//  parameter: value (0x0=18V, 0x1=32V (default), 0x2=34V, 0x3=36V)
//  Higher bits are ignored. No update. Returns: nothing.
//************************************************************************************
void LTC2874_write_OV_TH_value(uint8_t value)
{
  uint8_t command_byte , data_byte, position;
  // Create command and address byte
  command_byte = LTC2874_READ | (LTC2874_MODE2_REG9 << 1);

  output_low(LTC2874_CS);                 //! 1) Pull CS low
  spi_write(command_byte);                //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);             //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);                //! 4) Pull CS high to finish transaction

  data_byte &= ~(LTC2874_OV_TH_msk);      // clear OV_TH bits with inverse mask
  data_byte |= (LTC2874_OV_TH(value));    // bitwise OR value into OV_TH bits

  command_byte = LTC2874_WRITE_NO_UPDATE | (LTC2874_MODE2_REG9 << 1);

  output_low(LTC2874_CS);                 //! 1) Pull CS low to start transaction
  spi_write(command_byte);                //! 2) Write the command and address
  spi_write(data_byte);                   //! 3) Write the data
  output_high(LTC2874_CS);                //! 4) Pull CS high to finish transaction
}


//************************************************************************************
// Sends WURQ (Wake-up Request) on specified port.
// parameter: port# [1,2,3, or 4; only one at a time]
// returns: nothing.
//************************************************************************************
void LTC2874_wakeup_request(uint8_t port)
{
  uint8_t data_byte, command_byte;
  port = 0x01 << (port + 3);         //move to MS of byte, make mask

  command_byte = LTC2874_READ | (LTC2874_CTRL1_REGD << 1);

  output_low(LTC2874_CS);            //! 1) Pull CS low
  spi_write(command_byte);           //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);        //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);

  data_byte |= port;                 // bitwise OR

  command_byte = LTC2874_WRITE_UPDATE_ALL | (LTC2874_CTRL1_REGD << 1);

  output_low(LTC2874_CS);              //! 1) Pull CS low to start transaction
  spi_write(command_byte);             //! 2) Write the command and address
  spi_write(data_byte);                //! 3) Write the data
  output_high(LTC2874_CS);             //! 4) Pull CS high to finish transaction
}


//************************************************************************************
//  Updates all registers.
//  parameters: none
//  returns: nothing
//************************************************************************************
void LTC2874_update_all(void)
{
  uint8_t command_byte;
  command_byte = LTC2874_UPDATE_ALL;

  output_low(LTC2874_CS);                 //! 1) Pull CS low to start transaction
  spi_write(command_byte);                //! 2) Write the command and address
  spi_write(0x00);                        //! 3) Write null data
  output_high(LTC2874_CS);                //! 4) Pull CS high to finish transaction
}


//************************************************************************************
//  Writes byte of data to a register, then Updates.
//  parameters: register, data
//  returns: nothing
//************************************************************************************
void LTC2874_write_register_update_all(uint8_t LTC2874_register, uint8_t LTC2874_data)
{
  uint8_t command_byte;
  command_byte = LTC2874_WRITE_UPDATE_ALL | (LTC2874_register << 1);

  output_low(LTC2874_CS);                 //! 1) Pull CS low to start transaction
  spi_write(command_byte);                //! 2) Write the command and address
  spi_write(LTC2874_data);                //! 3) Write the data
  output_high(LTC2874_CS);                //! 4) Pull CS high to finish transaction
}

//************************************************************************************
//  Changes value of a register bit, then Updates.
//    NOTE: multi-bit handled by specific functions such as LTC2874_2XPTC_value()
//  parameters: register, bit, value
//    port#: [1,2,3, or 4]
//    value: 0 or 1
//  returns: nothing
//************************************************************************************
void LTC2874_write_bit_value_update_all(uint8_t LTC2874_register, uint8_t LTC2874_bit, uint8_t value)
{
  uint8_t command_byte , data_byte;
  command_byte = LTC2874_READ | (LTC2874_register << 1);

  output_low(LTC2874_CS);                 //! 1) Pull CS low
  spi_write(command_byte);                //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);             //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);                //! 4) Pull CS high to finish transaction

  if (value)
  {
    data_byte |= LTC2874_bit;            //bitwise or
  }
  else
  {
    data_byte &= ~LTC2874_bit;
  }

  command_byte = LTC2874_WRITE_UPDATE_ALL | (LTC2874_register << 1);

  output_low(LTC2874_CS);                 //! 1) Pull CS low to start transaction
  spi_write(command_byte);                //! 2) Write the command and address
  spi_write(data_byte);                   //! 3) Write the data
  output_high(LTC2874_CS);                //! 4) Pull CS high to finish transaction

}

//************************************************************************************
//  Sets a register bit, then Updates.
//  parameters: register, bit
//  returns: nothing
//************************************************************************************
void LTC2874_write_bit_set_update_all(uint8_t LTC2874_register, uint8_t LTC2874_bit)
{
  uint8_t command_byte , mask, data_byte;
  command_byte = LTC2874_READ | (LTC2874_register << 1);

  output_low(LTC2874_CS);                 //! 1) Pull CS low
  spi_write(command_byte);                //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);             //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);                //! 4) Pull CS high to finish transaction

  data_byte |= LTC2874_bit;               // compound bitwise OR

  command_byte = LTC2874_WRITE_UPDATE_ALL | (LTC2874_register << 1);

  output_low(LTC2874_CS);                 //! 1) Pull CS low to start transaction
  spi_write(command_byte);                //! 2) Write the command and address
  spi_write(data_byte);                   //! 3) Write the data
  output_high(LTC2874_CS);                //! 4) Pull CS high to finish transaction
}


//************************************************************************************
//  Clears a register bit, then Updates.
//  parameters: register, bit
//  returns: nothing
//************************************************************************************
void LTC2874_write_bit_clr_update_all(uint8_t LTC2874_register, uint8_t LTC2874_bit)
{
  uint8_t command_byte , mask, data_byte;
  command_byte = LTC2874_READ | (LTC2874_register << 1);

  output_low(LTC2874_CS);            //! 1) Pull CS low
  spi_write(command_byte);           //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);        //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);           //! 4) Pull CS high to finish transaction

  mask = ~LTC2874_bit;               // bitwise NOT
  data_byte &= mask;                 // clears the bit, leaves the rest unchanged

  command_byte = LTC2874_WRITE_UPDATE_ALL | (LTC2874_register << 1);

  output_low(LTC2874_CS);            //! 1) Pull CS low to start transaction
  spi_write(command_byte);           //! 2) Write the command and address
  spi_write(data_byte);              //! 3) Write the data
  output_high(LTC2874_CS);           //! 4) Pull CS high to finish transaction
}

//************************************************************************************
//  Resets LTC2874, returning default values to registers.
//  parameter: none
//  returns: nothing
//************************************************************************************
void LTC2874_reset(void)
{
  uint8_t command_byte;
  command_byte = LTC2874_RESET;

  output_low(LTC2874_CS);             //! 1) Pull CS low to start transaction
  spi_write(command_byte);            //! 2) Write the command and address
  spi_write(0x00);                    //! 3) Write null data
  output_high(LTC2874_CS);            //! 4) Pull CS high to finish transaction
}

//************************************************************************************
//  Reads data byte from specified register.
//  parameter: register address
//  returns: data byte
//************************************************************************************
uint8_t LTC2874_read_reg(uint8_t LTC2874_register)
{
  uint8_t command_byte, data_byte;
  //!  Build the reg command byte
  command_byte = LTC2874_READ |  (LTC2874_register << 1);

  output_low(LTC2874_CS);                //! 1) Pull CS low
  spi_write(command_byte);               //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);            //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);               //! 4) Pull CS high to finish transaction

  return data_byte;
}


//************************************************************************************
//  Reads a data BIT from specified register.
//  parameters: register address, bit mask
//  returns: bit value.
//************************************************************************************
uint8_t LTC2874_read_bit(uint8_t LTC2874_register, uint8_t LTC2874_bit)
{
  uint8_t command_byte, data_byte;

  command_byte = LTC2874_READ |  (LTC2874_register << 1);

  output_low(LTC2874_CS);                 //! 1) Pull CS low
  spi_write(command_byte);                //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);             //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);                //! 4) Pull CS high to finish transaction

  data_byte &= LTC2874_bit;
  data_byte = data_byte != 0;             //! 5) If the bit is not zero set data_byte to 1

  return data_byte;
}


//************************************************************************************
//  Writes byte of data to a register without Updating.
//  parameters: register, data
//  returns: nothing
//************************************************************************************
void LTC2874_write_register(uint8_t LTC2874_register, uint8_t LTC2874_data)
{
  uint8_t command_byte;
  command_byte = LTC2874_WRITE_NO_UPDATE |  (LTC2874_register << 1);

  output_low(LTC2874_CS);               //! 1) Pull CS low to start transaction
  spi_write(command_byte);              //! 2) Write the command and address
  spi_write(LTC2874_data);              //! 3) Write the data
  output_high(LTC2874_CS);              //! 4) Pull CS high to finish transaction
}


//************************************************************************************
//  Sets a register bit without Updating.
//  parameters: register, bit
//  returns: nothing
//************************************************************************************
void LTC2874_write_bit_set(uint8_t LTC2874_register, uint8_t LTC2874_bit)
{
  uint8_t command_byte , mask, data_byte;
  command_byte = LTC2874_READ | (LTC2874_register << 1);

  output_low(LTC2874_CS);               //! 1) Pull CS low
  spi_write(command_byte);              //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);           //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);              //! 4) Pull CS high to finish transaction

  data_byte |= LTC2874_bit;             // compound bitwise OR

  command_byte = LTC2874_WRITE_NO_UPDATE |  (LTC2874_register << 1);

  output_low(LTC2874_CS);                //! 1) Pull CS low to start transaction
  spi_write(command_byte);               //! 2) Write the command and address
  spi_write(data_byte);                  //! 3) Write the data
  output_high(LTC2874_CS);               //! 4) Pull CS high to finish transaction
}

//************************************************************************************
//  Clears a register bit without Updating.
//  parameters: register, bit
//  returns: nothing.
//************************************************************************************
void LTC2874_write_bit_clr(uint8_t LTC2874_register, uint8_t LTC2874_bit)
{
  uint8_t command_byte , mask, data_byte;
  command_byte = LTC2874_READ |  (LTC2874_register << 1);

  spi_write(command_byte);              //! 2) Write first byte and send command | address
  output_high(LTC2874_CS);              //! 4) Pull CS high to finish transaction

  mask = ~LTC2874_bit;                  // bitwise NOT
  data_byte &= mask;                    // clears the bit, leaves the rest unchanged

  command_byte = LTC2874_WRITE_NO_UPDATE |  (LTC2874_register << 1);

  output_low(LTC2874_CS);               //! 1) Pull CS low to start transaction
  spi_write(command_byte);              //! 2) Write the command and address
  spi_write(data_byte);                 //! 3) Write the data
  output_high(LTC2874_CS);              //! 4) Pull CS high to finish transaction
}


//************************************************************************************
//  Changes value of a register bit without Updating.
//    NOTE: multi-bit handled by specific functions such as LTC2874_NSF_value()
//  parameters: register, bit, value
//    port#: [1,2,3, or 4]
//    value: 0 or 1
//  returns: nothing
//************************************************************************************
void LTC2874_write_bit_value(uint8_t LTC2874_register, uint8_t LTC2874_bit, uint8_t value)
{
  uint8_t command_byte , data_byte;
  command_byte = LTC2874_READ |  (LTC2874_register << 1);

  output_low(LTC2874_CS);                   //! 1) Pull CS low
  spi_write(command_byte);                  //! 2) Write first byte and send command | address
  data_byte = spi_read(0x00);               //! 3) Read last byte (while sending null data)
  output_high(LTC2874_CS);                  //! 4) Pull CS high to finish transaction

  if (value)
  {
    data_byte |= LTC2874_bit;
  }
  else
  {
    data_byte &= ~LTC2874_bit;
  }

  command_byte = LTC2874_WRITE_NO_UPDATE |  (LTC2874_register << 1);

  output_low(LTC2874_CS);                     //! 1) Pull CS low to start transaction
  spi_write(command_byte);                    //! 2) Write the command and address
  spi_write(data_byte);                       //! 3) Write the data
  output_high(LTC2874_CS);                    //! 4) Pull CS high to finish transaction
}

