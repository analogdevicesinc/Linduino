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

REVISION HISTORY
$Revision: 4641 $
$Date: 2016-01-29 14:38:11 -0800 (Fri, 29 Jan 2016) $

Copyright (c) 2016, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1.  Redistributions  of source code must retain the above copyright notice, this
    list  of conditions and the following disclaimer.

2.  Redistributions  in  binary  form must reproduce the above copyright notice,
    this  list of conditions and  the following disclaimer in the  documentation
    and/or other materials provided with the distribution.

THIS  SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY  EXPRESS  OR  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES   OF  MERCHANTABILITY  AND  FITNESS  FOR  A  PARTICULAR  PURPOSE  ARE
DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY  DIRECT,  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING,  BUT  NOT  LIMITED  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS  OF  USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY  THEORY  OF  LIABILITY,  WHETHER  IN  CONTRACT,  STRICT  LIABILITY,  OR TORT
(INCLUDING  NEGLIGENCE  OR  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The  views and conclusions contained in the software and documentation are those
of  the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However,  the Linduino is only possible because of the Arduino team's commitment
to   the   open-source   community.   Please,  visit  http://www.arduino.cc  and
http://store.arduino.cc  ,  and  consider  a  purchase that will help fund their
ongoing work.

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

