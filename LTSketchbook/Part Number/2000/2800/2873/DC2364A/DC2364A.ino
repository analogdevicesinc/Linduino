/*!
Linear Technology DC2364A Demonstration Board with SPI interface.
LTC2873: Two-Wire RS485/RS232 Transceiver with Switchable Termination.

@verbatim

NOTES
  Setup:
   Set the terminal baud rate to 115200 and select the newline terminator.
   Ensure all jumpers on the demo board are installed in their default positions
   from the factory. Refer to Demo Manual DC2364A.

  How to test:


USER INPUT DATA FORMAT:
 decimal : 1024
 hex     : 0x400
 octal   : 02000  (leading 0 "zero")
 binary  : B10000000000
 float   : 1024.0

@endverbatim
http://www.linear.com/product/LTC2873


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

//! @ingroup Transceivers
//! @{
//! @defgroup LTC2873 LTC2873: Two-Wire RS485/RS232 Transceiver with Switchable Termination.
//! @}

/*! @file
    @ingroup LTC2873
    Library for LTC2873: Two-Wire RS485/RS232 Transceiver with Switchable Termination.
*/

// Headerfiles
#include <UserInterface.h>  // LTC defined
#include <SoftwareSerial.h>

//! @name Assigned Pin Numbers
//! @{
#define RO              10 // soft UART with interrupt pin.  (SSerial RX)
#define DI              11 // soft UART with interrupt pin   (SSerial TX)
#define DE485_F232      3  // RE485B jumpered to here.  (SSerialTxControl)
#define RE485b          4  // beware of jumper tying to DE485
#define MODE485         5  // "select" RS485 if high, 232 if low.
#define TE485b          6  //  RS485 termination enable BAR
#define LB              7  // Loopback 232 or 485
#define SHDNb           2  // Shutdown BAR
//!@}

// Global Variables
int byteReceived;
int byteSend;
int di_toggle = 0;
int SDNb_val, MODE485_val, REb_val, DE485_F232_val, TEb_val, LB_val, DI_val;

//! Initialize Linduino
void setup()
{
  // Set pin directions
  pinMode(RO, INPUT_PULLUP);
  pinMode(DI, OUTPUT);
  pinMode(DE485_F232, OUTPUT);
  pinMode(RE485b, OUTPUT);
  pinMode(MODE485,OUTPUT);
  pinMode(TE485b,OUTPUT);
  pinMode(LB,OUTPUT);
  pinMode(SHDNb,OUTPUT);

  // Initialize pins
  digitalWrite(DI,LOW);
  digitalWrite(DE485_F232,LOW);
  digitalWrite(RE485b,HIGH);
  digitalWrite(MODE485,HIGH);
  digitalWrite(TE485b,HIGH);
  digitalWrite(LB,LOW);
  digitalWrite(SHDNb,LOW);

  TCCR2B = TCCR2B & 0b11111000 | 0x01;    // set PWM freq to 31372.55Hz (and scales timers!)
  //TCCR2B = TCCR2B & 0b11111000 | 0x04;  // set PWM freq on pins 2, 11 to 490.20Hz (default)

  // Open communications to serial monitor
  Serial.begin(115200);               // DON'T FORGET TO SET SERIAL MONITOR TO THIS SPEED!!!
  // RS485Serial.begin(4800);           // Start the software serial port, to another device
  print_title();                      // Menu on serial monitor
  print_status();
}

//! Repeats Linduino loop
void loop()
{
  uint8_t user_command = read_int();
  Serial.print(F("    "));

  if (user_command > 0 && user_command < 27)
  {
    update(user_command);
    Serial.print(user_command);
    print_status();             // Print status after operation
  }
  else if (user_command == 'm')
  {
    update(user_command);
    Serial.print(F("m"));
    print_status();             // Print status after operation
  }
  else
  {
    Serial.println(F("Wrong choice. TRY AGAIN!"));
  }
}

//! Prints the title block when program first starts.
void print_title(void)
{
  Serial.println(F(" "));
  Serial.println(F("**************************************************************************"));
  Serial.println(F("DC2364A Demonstration Board for the LTC2873 "));
  Serial.println(F("**************************************************************************"));
  Serial.println(F("PIN CONTROL:             MODES:"));
  Serial.println(F("1. Flip SHDNb            10. Shutdown               19. 485 Term"));
  Serial.println(F("2. Flip 485/232b         11. 232 RX+TX Slow         20. 485 RX+Term"));
  Serial.println(F("3. Flip RE485b           12. 232 RX+TX Fast         21. 485 TX+Term"));
  Serial.println(F("4. Flip DE485/F232       13. 232 Loopbk+TX Slow     22. 485 RX+TX+Term"));
  Serial.println(F("5. Flip TE485b           14. 232 Loopbk+TX Fast     23. 485 Loopbk"));
  Serial.println(F("6. Flip LB               15. 485 Ready              24. 485 Loopbk+TX"));
  Serial.println(F("7. Flip DI               16. 485 RX                 25. 485 Loopbk+Term"));
  Serial.println(F("8. 31.4kHz 50% on DI*    17. 485 TX                 26. 485 Loopbk+TX+Term"));
  Serial.println(F("9. 31.4kHz 10% on DI*    18. 485 RX+TX "));
  Serial.println(F("**************************************************************************"));
  Serial.println(F("Press m to stop 31.4 kHz signal "));
  Serial.println(F(" "));
  Serial.println(F("CURRENT STATE:"));
  Serial.println(F("User Entry    SHDNb    485   RE485b   DE485   TE485b    LB      DI      MODE"));
  Serial.println(F("-----------------------------------------------------------------------------"));
  Serial.print(F("    "));
}

//! Prints the status of each pin based on user selection.
void print_status(void)
{
  SDNb_val = digitalRead(SHDNb);
  MODE485_val = digitalRead(MODE485);
  REb_val = digitalRead(RE485b);
  DE485_F232_val = digitalRead(DE485_F232);
  TEb_val = digitalRead(TE485b);
  LB_val = digitalRead(LB);

  Serial.print(F("\t\t"));
  Serial.print(SDNb_val, DEC);
  Serial.print(F("\t"));
  Serial.print(MODE485_val, DEC);
  Serial.print(F("\t"));
  Serial.print(REb_val, DEC);
  Serial.print(F("\t"));
  Serial.print(DE485_F232_val, DEC);
  Serial.print(F("\t"));
  Serial.print(TEb_val, DEC);
  Serial.print(F("\t"));
  Serial.print(LB_val, DEC);
  Serial.print(F("\t"));

  if (di_toggle)             // must check if pwm mode to preserve, otherwise, digitalRead will kill it.
  {
    Serial.print("pwm");
  }
  else                       // not pwm'ing it.
  {
    DI_val = digitalRead(DI);
    Serial.print(DI_val,DEC);
  }
  Serial.print(F("\t"));

  String status_val = status();
  // String status_val = find_status();
  Serial.print(status_val);
  Serial.print(F("\n"));
}

//! Returns a descriptive string based on status of pins.
String status(void)
{
  if (!SDNb_val)
    return "SHUTDOWN";
  if (!MODE485_val && !DE485_F232_val && !LB_val)
    return "232 RX+TX SLOW";
  if (!MODE485_val && !DE485_F232_val && LB_val)
    return "232 LB+TX SLOW";
  if (!MODE485_val && DE485_F232_val && !LB_val)
    return "232 RX+TX FAST";
  if (!MODE485_val && DE485_F232_val && LB_val)
    return "232 LB+TX FAST";
  if (MODE485_val && !DE485_F232_val && REb_val && TEb_val && !LB_val)
    return "485 READY  ";
  if (MODE485_val && !DE485_F232_val && REb_val && !TEb_val && !LB_val)
    return "485 TERM  ";
  if (MODE485_val && DE485_F232_val && REb_val && TEb_val && !LB_val)
    return "485 TX    ";
  if (MODE485_val && DE485_F232_val && REb_val && !TEb_val && !LB_val)
    return "485 TX+TERM";
  if (MODE485_val && !DE485_F232_val && !REb_val && TEb_val && !LB_val)
    return "485 RX    ";
  if (MODE485_val && !DE485_F232_val && !REb_val && !TEb_val && !LB_val)
    return "485 RX+TERM";
  if (MODE485_val && DE485_F232_val && !REb_val && TEb_val && !LB_val)
    return "485 RX+TX";
  if (MODE485_val && DE485_F232_val && !REb_val && !TEb_val && !LB_val)
    return "485 RX+TX+TERM";
  if (MODE485_val && !DE485_F232_val && !REb_val && TEb_val && LB_val)
    return "485 LB    ";
  if (MODE485_val && !DE485_F232_val && !REb_val && !TEb_val && LB_val)
    return "485 LB+TERM";
  if (MODE485_val && DE485_F232_val && !REb_val && TEb_val && LB_val)
    return "485 LB+TX";
  if (MODE485_val && DE485_F232_val && !REb_val && !TEb_val && LB_val)
    return "485 LB+TX+TERM";
}

//! Returns a descriptive string based on status of pins.
String find_status(void)
{
  String status = "";
  if (!SDNb_val)
    status = status + "SHUTDOWN";
  else
  {
    if (MODE485_val)
    {
      status = status + "485 ";
      if (DE485_F232_val)
        status = status + "ENABLE ";
      else
        status = status + "DISABLE ";
    }
    else
    {
      status = status + "232 ";
      if (DE485_F232_val)
        status = status + "FAST ";
      else
        status = status + "SLOW ";
    }

    if (LB_val)
      status = status + "LB ";
    if (!REb_val)
      status = status + "RX ";
    if (DE485_F232_val)
      status = status + "TX ";

    if (!TEb_val)
      status = status + "TERM ";
  }
  return status;
}

//! Function to toggle the state of a pin.
void flipPin(uint8_t pin)
{
  int pinValue = digitalRead(pin);
  if (pinValue)         // if pin is high
    digitalWrite(pin, LOW);     // change to low
  else
    digitalWrite(pin, HIGH);    //otherwise, set high
}

//! Update the state of pins based on user selection.
void update(uint8_t user_command)
{
  switch (user_command)
  {
    case 1:
      {
        // Flip SHDNb
        flipPin(SHDNb);
        break;
      }
    case 2:
      {
        // Flip 485/232b
        flipPin(MODE485);
        break;
      }
    case 3:
      {
        // Flip RE485b
        flipPin(RE485b);
        break;
      }
    case 4:
      {
        // Flip DE485/F232
        flipPin(DE485_F232);
        break;
      }
    case 5:
      {
        // Flip TE485b
        flipPin(TE485b);
        break;
      }
    case 6:
      {
        // Flip LB
        flipPin(LB);
        break;
      }
    case 7:
      {
        // Flip DI
        di_toggle=0;                // clear the toggle variable so the status will print correctly
        flipPin(DI);
        break;
      }
    case 8:                    // pwm on DI.  Freq set by TCCR2B at top
      {
        // 31.4kHz 50% on DI*
        analogWrite(DI,128);     // 25% duty cycle is between  0 and 255
        di_toggle = 1;           // set flag for reporting status on this bit
        break;
      }
    case 109:          // 120 = 'x'
      {
        analogWrite(DI,0);       // Stop 31.4 kHz signal
        di_toggle = 0;           // set flag for reporting status on this bit
        break;
      }
    case 9:                    // pwm on DI.  Freq set by TCCR2B at top
      {
        // 31.4kHz 10% on DI*
        analogWrite(DI,26);      // 10% duty cycle is between  0 and 255
        di_toggle = 1;           // set flag for reporting status on this bit
        break;
      }
    case 10:
      {
        //SHUTDOWN
        digitalWrite(SHDNb,LOW);
        digitalWrite(MODE485,HIGH);
        digitalWrite(RE485b,HIGH);
        digitalWrite(DE485_F232,LOW);
        digitalWrite(TE485b,HIGH);
        digitalWrite(LB,LOW);
        //digitalWrite(DI,LOW);
        break;
      }
    case 11:
      {
        //232 RX + TX SLOW
        digitalWrite(SHDNb,HIGH);
        digitalWrite(MODE485,LOW);
        digitalWrite(RE485b,HIGH);
        digitalWrite(DE485_F232,LOW);
        digitalWrite(TE485b,HIGH);
        digitalWrite(LB,LOW);
        //digitalWrite(DI,LOW);
        break;
      }
    case 12:
      {
        //232 RX + TX FAST
        digitalWrite(SHDNb,HIGH);
        digitalWrite(MODE485,LOW);
        digitalWrite(RE485b,HIGH);
        digitalWrite(DE485_F232,HIGH);
        digitalWrite(TE485b,HIGH);
        digitalWrite(LB,LOW);
        //digitalWrite(DI,LOW);
        break;
      }
    case 13:
      {
        //232 LOOPBACK + TX SLOW
        digitalWrite(SHDNb,HIGH);
        digitalWrite(MODE485,LOW);
        digitalWrite(RE485b,HIGH);
        digitalWrite(DE485_F232,LOW);
        digitalWrite(TE485b,HIGH);
        digitalWrite(LB,HIGH);
        //digitalWrite(DI,LOW);
        break;
      }
    case 14:
      {
        //232 LOOPBACK + TX FAST
        digitalWrite(SHDNb,HIGH);
        digitalWrite(MODE485,LOW);
        digitalWrite(RE485b,HIGH);
        digitalWrite(DE485_F232,HIGH);
        digitalWrite(TE485b,HIGH);
        digitalWrite(LB,HIGH);
        //digitalWrite(DI,LOW);
        break;
      }
    case 15:
      {
        //485 READY
        digitalWrite(SHDNb,HIGH);
        digitalWrite(MODE485,HIGH);
        digitalWrite(RE485b,HIGH);
        digitalWrite(DE485_F232,LOW);
        digitalWrite(TE485b,HIGH);
        digitalWrite(LB,LOW);
        //digitalWrite(DI,LOW);
        break;
      }
    case 16:
      {
        //485 RX
        digitalWrite(SHDNb,HIGH);
        digitalWrite(MODE485,HIGH);
        digitalWrite(RE485b,LOW);
        digitalWrite(DE485_F232,LOW);
        digitalWrite(TE485b,HIGH);
        digitalWrite(LB,LOW);
        //digitalWrite(DI,LOW);
        break;
      }
    case 17:
      {
        //485 TX
        digitalWrite(SHDNb,HIGH);
        digitalWrite(MODE485,HIGH);
        digitalWrite(RE485b,HIGH);
        digitalWrite(DE485_F232,HIGH);
        digitalWrite(TE485b,HIGH);
        digitalWrite(LB,LOW);
        //digitalWrite(DI,LOW);
        break;
      }
    case 18:
      {
        //485 RX + TX
        digitalWrite(SHDNb,HIGH);
        digitalWrite(MODE485,HIGH);
        digitalWrite(RE485b,LOW);
        digitalWrite(DE485_F232,HIGH);
        digitalWrite(TE485b,HIGH);
        digitalWrite(LB,LOW);
        //digitalWrite(DI,LOW);
        break;
      }
    case 19:
      {
        //485 TERMINATION
        digitalWrite(SHDNb,HIGH);
        digitalWrite(MODE485,HIGH);
        digitalWrite(RE485b,HIGH);
        digitalWrite(DE485_F232,LOW);
        digitalWrite(TE485b,LOW);
        digitalWrite(LB,LOW);
        //digitalWrite(DI,LOW);
        break;
      }
    case 20:
      {
        //485 RX + TERMINATION
        digitalWrite(SHDNb,HIGH);
        digitalWrite(MODE485,HIGH);
        digitalWrite(RE485b,LOW);
        digitalWrite(DE485_F232,LOW);
        digitalWrite(TE485b,LOW);
        digitalWrite(LB,LOW);
        //digitalWrite(DI,LOW);
        break;
      }
    case 21:
      {
        //485 TX + TERMINATION
        digitalWrite(SHDNb,HIGH);
        digitalWrite(MODE485,HIGH);
        digitalWrite(RE485b,HIGH);
        digitalWrite(DE485_F232,HIGH);
        digitalWrite(TE485b,LOW);
        digitalWrite(LB,LOW);
        //digitalWrite(DI,LOW);
        break;
      }
    case 22:
      {
        //485 RX + TX + TERMINATION
        digitalWrite(SHDNb,HIGH);
        digitalWrite(MODE485,HIGH);
        digitalWrite(RE485b,LOW);
        digitalWrite(DE485_F232,HIGH);
        digitalWrite(TE485b,LOW);
        digitalWrite(LB,LOW);
        //digitalWrite(DI,LOW);
        break;
      }
    case 23:
      {
        //485 LOOPBACK
        digitalWrite(SHDNb,HIGH);
        digitalWrite(MODE485,HIGH);
        digitalWrite(RE485b,LOW);
        digitalWrite(DE485_F232,LOW);
        digitalWrite(TE485b,HIGH);
        digitalWrite(LB,HIGH);
        //digitalWrite(DI,LOW);
        break;
      }
    case 24:
      {
        //485 LOOPBACK + TX
        digitalWrite(SHDNb,HIGH);
        digitalWrite(MODE485,HIGH);
        digitalWrite(RE485b,LOW);
        digitalWrite(DE485_F232,HIGH);
        digitalWrite(TE485b,HIGH);
        digitalWrite(LB,HIGH);
        //digitalWrite(DI,LOW);
        break;
      }
    case 25:
      {
        //485 LOOPBACK + TERM
        digitalWrite(SHDNb,HIGH);
        digitalWrite(MODE485,HIGH);
        digitalWrite(RE485b,LOW);
        digitalWrite(DE485_F232,LOW);
        digitalWrite(TE485b,LOW);
        digitalWrite(LB,HIGH);
        //digitalWrite(DI,LOW);
        break;
      }
    case 26:
      {
        //485 LOOPBACK + TX + TERM
        digitalWrite(SHDNb,HIGH);
        digitalWrite(MODE485,HIGH);
        digitalWrite(RE485b,LOW);
        digitalWrite(DE485_F232,HIGH);
        digitalWrite(TE485b,LOW);
        digitalWrite(LB,HIGH);
        //digitalWrite(DI,LOW);
        break;
      }

  }
}
