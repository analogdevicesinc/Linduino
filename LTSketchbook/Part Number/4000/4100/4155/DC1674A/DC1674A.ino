/*!
LTC4155: Dual-Input Power Manager / 3.5A Li-Ion Battery Charger with I²C Control and USB OTG

@verbatim
The LTC®4155 is a 15 watt I²C controlled power manager with PowerPath™ instant-
on operation, high efficiency switching battery charging and USB compatibility.
The LTC4155 seamlessly manages power distribution from two 5V sources, such as a
USB port and a wall adapter, to a single-cell rechargeable Lithium-Ion/Polymer
battery and a system load.
@endverbatim

http://www.linear.com/product/LTC4155

http://www.linear.com/product/LTC4155#demoboards


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

Generated on: 2016-01-19
*/



/*! @file
 *  @ingroup LTC4155
 *  @brief Arduino Example
 *
 *  This is an example sketch to demonstrate use of the API with the Linduino.
 *  In addition to the sketch, the following files are required::
 *    LTC4155.c
 *    LTC4155.h
 *    LTC4155_reg_defs.h
 */

#include "LTC4155.h"
#include <Arduino.h>
#include "Linduino.h"

#include "UserInterface.h"
#include "LT_I2CBus.h"
#include "LT_SMBusNoPec.h"

static LT_SMBus *smbus = new LT_SMBusNoPec();

int read_register(uint8_t addr,uint8_t command_code, uint8_t *data, port_configuration_t *port_configuration);
int write_register(uint8_t addr,uint8_t command_code, uint8_t data, port_configuration_t *port_configuration);

LTC4155_chip_cfg_t cfg =
{
  LTC4155_ADDR_09, //.addr (7-bit)
  read_register, //.read_register
  write_register, //.write_register
  0 //.port_configuration not used
};


LTC4155 chip;
uint8_t data;

void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC
  while (!Serial);              //! Wait for serial port to be opened in the case of Leonardo USB Serial
  Serial.println(F("Initializing LTC4155"));
  chip = LTC4155_init(&cfg);
}

void loop()
{
  /* the API functions can be used to read and write individual bit fields within a command code */
  Serial.println(F("Press any key to arm ship-and-store mode."));
  while (Serial.available() == 0);
  while (Serial.available() > 0)
    Serial.read();
  LTC4155_write_register(chip, LTC4155_ARM_SHIPMODE, LTC4155_ARM_SHIPMODE_PRESET_ARM);
  Serial.println(F("Ship-and-store mode armed.\n"));

  Serial.println(F("Press any key to disarm ship-and-store mode."));
  while (Serial.available() == 0);
  while (Serial.available() > 0)
    Serial.read();
  LTC4155_read_register(chip, LTC4155_REG6, &data);
  LTC4155_write_register(chip, LTC4155_REG6, data); // Write to register 6 to clear ship-mode without changing its contents.
  Serial.println(F("Ship-and-store mode disarmed.\n"));
}

/*! read_register function wraps C++ method LT_SMBus::readByte and places the returned data in *data.*/
int read_register(uint8_t addr, uint8_t command_code, uint8_t *data, port_configuration_t *port_configuration)
{
  //virtual uint8_t LT_SMBus::readByte(uint8_t address,uint8_t command);
  *data = smbus->readByte(addr, command_code);
  return 0;
}

/*! write_register function wraps C++ method LT_SMBus::writeByte.*/
int write_register(uint8_t addr, uint8_t command_code, uint8_t data, port_configuration_t *port_configuration)
{
  //virtual void LT_SMBus::writeByte(uint8_t address,uint8_t command,uint8_t data);
  smbus->writeByte(addr, command_code, data);
  return 0;
}

