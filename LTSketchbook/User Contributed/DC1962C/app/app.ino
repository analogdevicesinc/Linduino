/*!
Linear Technology DC1962C Demonstration Board
LTC388X, LTC297X, LTMXXXX: Power Management Solution for Application Processors

This sketch has only been tested on a Mega 2560. It is known to fail
on Aarduino Uno and Linduino due to size of RAM.

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.

@endverbatim

Copyright 2021(c) Analog Devices, Inc.

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
    @ingroup DC1962
*/

#include <Arduino.h>
#include <stdint.h>

#include <LT_SMBusPec.h>
#include <LT_SMBusNoPec.h>
#include <LT_SMBusGroup.h>
#include <LT_PMBus.h>
#include <LT_PMBusMath.h>
#include <LT_PMBusDevice.h>
#include <LT_PMBusRail.h>
#include <LT_PMBusDetect.h>
#include <LT_FaultLog.h>

static bool doprint = false;

#define POLL_DELAY 200

static LT_SMBusNoPec *smbusNoPec = new LT_SMBusNoPec(100000);
static LT_SMBusPec *smbusPec = new LT_SMBusPec(100000);
static LT_PMBus *pmbusNoPec = new LT_PMBus(smbusNoPec);
static LT_PMBus *pmbusPec = new LT_PMBus(smbusPec);
static LT_SMBus *smbus = smbusNoPec;
static LT_PMBus *pmbus = pmbusNoPec;
static LT_PMBusDetect *detector = new LT_PMBusDetect(pmbusNoPec);
static LT_PMBusDevice **devices;
static LT_PMBusDevice **device;
static LT_PMBusRail **rails;
static LT_PMBusRail **rail;

//! Initialize Linduino
//! @return void
void setup()
{
  Serial.begin(115200);         //! Initialize the serial port to the PC

  detector->detect();
  device = (devices = detector->getDevices());
  print_title();
  print_prompt();
}

//! Serial.prints the title block when program first starts.
void print_title()
{
  Serial.print("\n***************************************************************\n");
  Serial.print("* PSM Management Program                                        *\n");
  Serial.print("*                                                               *\n");
  Serial.print("*****************************************************************\n");
}

//! Serial.prints main menu.
void print_prompt()
{
  Serial.print(F("\n  1-Basic Commands\n"));
  Serial.print(F("  2-Fault Log\n"));
  Serial.print(F("  3-System Tree\n"));
  Serial.print(F("  4-PEC On\n"));
  Serial.print(F("  5-PEC Off\n"));
  Serial.print(F("  6-Bus Probe\n"));
  Serial.print(F("  7-Reset\n"));
  Serial.print(F("  8-Quit\n"));
  Serial.print(F("\nEnter a command:"));
}

//! Serial.prints a warning if the demo board is not detected.
void print_warning_prompt()
{
  Serial.print(F("\nWarning: Demo board not detected. Linduino will attempt to proceed."));
}

void print_all_voltages()
{
  float   voltage;
  device = devices;
  while((*device) != NULL)
  {
    if((*device)->hasCapability(HAS_VOUT))
    {
      for (int page = 0; page < (*device)->getNumPages(); page++)
      {
        (*device)->setPage(page);
        voltage = (*device)->readVout(true);
        Serial.print(F("VOUT ")); 
        Serial.print(voltage, 6);
        Serial.print(F(" "));
        Serial.print((*device)->getAddress(), HEX);
        Serial.print(F(" "));
        Serial.println(page, HEX);
      }
    }
    device++;
  }
}

void print_all_currents()
{
  float   current;
  device = devices;
  while((*device) != NULL)
  {
    if((*device)->hasCapability(HAS_IOUT))
    {
      for (int page = 0; page < (*device)->getNumPages(); page++)
      {
        (*device)->setPage(page);
        current = (*device)->readIout(true);
        Serial.print(F("IOUT "));
        Serial.print(current, 6);
        Serial.print(F(" "));
        Serial.print((*device)->getAddress(), HEX);
        Serial.print(F(" "));
        Serial.println(page, HEX);
      }
    }
    device++;
  }
}

void print_all_status()
{
  uint16_t w;

  device = devices;
  while((*device) != NULL)
  {
    if((*device)->hasCapability(HAS_STATUS_WORD))
    {
      for (int page = 0; page < (*device)->getNumPages(); page++)
      {
        w = (*device)->readStatusWord();
        Serial.print(F("STATUS_WORD "));
        Serial.print(w, HEX);
        Serial.print(F(" "));
        Serial.print((*device)->getAddress(), HEX);
        Serial.print(F(" "));
        Serial.println(page, HEX);
      }
    }
    device++;
  }
}

void sequence_off_on()
{
  pmbus->sequenceOffGlobal();
  delay(2000);
  pmbus->sequenceOnGlobal();
}

void margin_high()
{
  pmbus->marginHighGlobal();
}

void margin_low()
{
  pmbus->marginLowGlobal();
}

void margin_off()
{
  pmbus->sequenceOnGlobal();
}

void menu_1_basic_commands()
{
  uint8_t user_command;

  do
  {
    //! Displays the Read/Write menu
    Serial.print(F("\nRead/Write\n\n"));
    Serial.print(F("  1-Read All Voltages\n"));
    Serial.print(F("  2-Read All Currents\n"));
    Serial.print(F("  3-Read All Status\n"));
    Serial.print(F("  4-Sequence Off/On\n"));
    Serial.print(F("  5-Margin High\n"));
    Serial.print(F("  6-Margin Low\n"));
    Serial.print(F("  7-Margin Off\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();
    Serial.println();

    //! Reads or writes to a LTC3676 and Serial.prints result.
    switch (user_command)
    {
      case 1:
      print_all_voltages();
      break;
      case 2:
      print_all_currents();
      break;
      case 3:
      print_all_status();
      break;
      case 4:
      sequence_off_on();
      break;
      case 5:
      margin_high();
      break;
      case 6:
      margin_low();
      break;
      case 7:
      margin_off();
      break;
      default:
      if (user_command != 'm')
        Serial.print("Invalid Selection\n");
      break;
    }
  }
  while (user_command != 'm');
}

void menu_2_basic_commands()
{
  uint32_t user_command;
  uint16_t timeout;

  do
  {
    //! Displays the Read/Write menu
    Serial.print(F("\n  1-Dump Fault Logs\n"));
    Serial.print(F("  2-Clear Fault Logs\n"));
    Serial.print(F("  3-Clear Faults\n"));
    Serial.print(F("  4-Enable Fault Logs\n"));
    Serial.print(F("  5-Disable Fault Logs\n"));
    Serial.print(F("  7-Reset\n"));
    Serial.print(F("  8-Store Fault Log\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command:"));

    user_command = read_int();
    Serial.println();

      uint8_t status;

      switch (user_command)                          //! Serial.prints the appropriate submenu
      {
        case 1:
          device = (devices = detector->getDevices());
          while (*device != NULL)
          {
            if((*device)->hasFaultLog())
            {
              Serial.print(F("Fault log for "));
              Serial.println((*device)->getAddress(), HEX);          
              (*device)->printFaultLog();
            }
            else
            {
              Serial.print(F("No fault log for "));
              Serial.println((*device)->getAddress(), HEX);
            }
            device++;
          }
          break;
        case 2:
          device = (devices = detector->getDevices());
          while (*device != NULL)
          {
            (*device)->clearFaultLog();
            device++;
          }
          break;
        case 3:
          device = (devices = detector->getDevices());
          while (*device != NULL)
          {
            (*device)->clearFaults();
            device++;
          }
          break;
        case 4:
          device = (devices = detector->getDevices());
      while (*device != NULL)
      {
        (*device)->enableFaultLog();
        device++;
      }
    break;
        case 5:
          device = (devices = detector->getDevices());
          while (*device != NULL)
          {
            (*device)->disableFaultLog();
            device++;
          }
          break;
            case 7:
              pmbus->restoreFromNvmGlobal();
              delay(2000);
              pmbus->resetGlobal();
              break;
            case 8:
              device = (devices = detector->getDevices());
          while (*device != NULL)
          {
            (*device)->storeFaultLog();
            device++;
          }
              break;
            default:
              Serial.print("Incorrect Option");
              break;
          }
  }
  while (user_command != 'm');
}


//! Repeats Linduino loop
//! @return void
void loop()
{
  char input[12];
  uint8_t user_command;
  uint8_t res;
  uint8_t model[7];
  uint8_t *addresses = NULL;

  user_command = read_int();
  Serial.println();

  switch (user_command)
  {
  case 1:
      menu_1_basic_commands();
      break;
  case 2 :
      menu_2_basic_commands();
      break;
  case 3 :
    device = (devices = detector->getDevices());
    while (*device != NULL)
    {
      Serial.print(F("  Device: "));
      ((*device)->getType());
      Serial.print(F(" Address: "));
      Serial.print((*device)->getAddress(), HEX);
      Serial.print(F(", Speed: "));;
      Serial.println((*device)->getMaxSpeed());
      device++;
    }

    rail = (rails = detector->getRails());
    while (*rail != NULL)
    {
      Serial.print(F("  Rail: "));
      Serial.print(F(" Address: "));
      Serial.print((*rail)->getAddress(), HEX);
      Serial.print(F(" Pages: "));
      Serial.println((*rail)->getNoPages(), 6);
      rail++;
    }
    break;
  case 4 :
    device = (devices = detector->getDevices());
    while (*device != NULL)
    {
      (*device)->enablePec();
      device++;
    }
    smbus = smbusPec;
    pmbus = pmbusPec;

    delete(detector);
    detector = new LT_PMBusDetect(pmbus);
    detector->detect();
    devices = detector->getDevices();
    rail = (rails = detector->getRails());
    break;
  case 5 :
    device = (devices = detector->getDevices());
    while (*device != NULL)
    {
      (*device)->disablePec();
      device++;
    }
    smbus = smbusNoPec;
    pmbus = pmbusNoPec;

    delete(detector);
    detector = new LT_PMBusDetect(pmbus);
    detector->detect();
    devices = detector->getDevices();
    rail = (rails = detector->getRails());
    break;
  case 6 :
    addresses = smbus->probe(0);
    while(*addresses != 0)
    {
      Serial.print(F("ADDR "));
      Serial.println( *addresses++, HEX);
    }
    break;
  case 7 :
     pmbus->resetGlobal();
     break;
  case 8 :
      return 0;
  default:
     Serial.print(F("Incorrect Option"));
     break;
  }

  print_prompt();
  return (1);
}
