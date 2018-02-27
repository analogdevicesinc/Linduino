
/*!

@verbatim


tinyDC590 is used to give basic DC590B compatibility to e.g. DC2574A_KIT sketch so
it can also be operated together with the LTC2947 GUI

DC590B compatibility:
tinyDC590 provides a subset of the DC590B commands.
This way the e.g. the LTC2947 GUI is also able to communicate with the DC2574A_KIT sketch which enables the
user to use the GUI to change LTC2947 configuration e.g. to set alarm thresholds etc.
The main limitations of this compatibility mode are:
- EEPROM read from the DC2334A demo board is faked (it will always report the right EEPROM string)
- SPI operation is not supported, due to incompatibility with the optional LCD Keypad Shield

Command  Function                          Notes
Q        Receive Byte *                    I2C only, send ACK
R        Receive Byte *                    I2C only, sends NACK
Sxx      Send byte (2 hex chars) *         SPI and I2C. Returns ‘N’ if slave NACKs.
s        Send start condition              I2C only
p        Send stop condition               I2C only
Z        Send line feed to host (0x0A)
I        Read demo board info.
i        Read contoller ID and firmware rev
O        Turn isolated power on
o        Turn isolated power off
MI       Switch to isolated I2C mode
Kxy      Bang pin, x argument is ‘0’ or ‘1’ for low or high, y is pin address, see below.
jxy      Set pin mode, x argument is ‘0’ or ‘1’ for INPUT or OUTPUT, y is pin address, see below.
ky       Read pin, answer is ‘0’ or ‘1’ for low or high, y is pin address, see below.
Uxxy     Write Port D bus, xx is ASCII Hex data, y is strobe pin address, strobed low.
D         Delay milliseconds
P         PingPause (echo P, wait 5 ms)
g         IO pin low
G         IO pin high
H         Wait for SDO=1                  (SPI only) ‘T’ on timeout
L         Wait for SDO=0                  (SPI only) ‘T’ on timeout
0x80      DC590B pseudo Reset


the following DC590B commands are NOT implemented:
r         Receive 7 bits                  In SPI mode only.
t         Begin recording loop
u         Stop recording loop
v         Echo loop
Txx       Tranceive (send and receive) *  (SPI only)
w         Loop until another command rec’d
X         CS=1
x         CS=0
MS        Switch to isolated SPI mode
MX        Switch to auxillary I2C mode

@endverbatim



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

#include <SPI.h>
#include <LT_SPI.h>
#include <LT_I2C.h>
#include <Linduino.h>
#include "arduino.h"

// for quikeval compatibility only
#define MISO_TIMEOUT  1000

unsigned char pseudo_reset = 0;
// serial mode constants
const byte spi_mode = 0;
const byte i2c_mode = 1;
const byte i2c_auxiliary_mode = 2;
byte serial_mode = spi_mode;  // current serial mode

/** \brief  DC590 busy flag. Asserted while DC590B command is processed */
boolean dc590Busy = false;

/** \brief hex conversion constants */
char hex_digits[16] =
{
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

/** \brief buffer for ASCII hex to byte conversion */
char hex_to_byte_buffer[5] =
{
  '0', 'x', '0', '0', '\0'
};


/** \brief buffer for byte to ASCII hex conversion */
char byte_to_hex_buffer[3] =
{
  '\0', '\0', '\0'
};



// Queue structure to store/buffer characters received from serial port:
// only necessary to buffer data from the serial interface if DC590B compatibility mode is used

/** \brief  max elements in the queue*/
#define QUEUE_ELEMENTS 250

/** \brief queue size. */
#define QUEUE_SIZE (QUEUE_ELEMENTS + 1)

/** \brief  The queue */
uint8_t Queue[QUEUE_SIZE];

/** \brief  The queue in element. */
uint8_t QueueIn = 0;

/** \brief  The queue out element */
uint8_t QueueOut = 0;

/** \fn  void EnqueueSerialData()
\brief  Enqueue serial data. */
void EnqueueSerialData()
{
  uint8_t help, help2;
  do
  {
    help = Serial.available(); // max 64 bytes!
  }
  while ((help == 0) && (QueueIn == QueueOut));   // no new data and queue is empty

  // helper is needed to check if queue is full
  help2 = ((QueueOut + QUEUE_ELEMENTS) % QUEUE_SIZE);

  while (
    (help != 0) &&     // data available
    (QueueIn != help2) // queue not full
  )
  {
    // store byte to queue
    Queue[QueueIn] = Serial.read();
    QueueIn = (QueueIn + 1) % QUEUE_SIZE;
    help = Serial.available(); // update number of remaining bytes
  }
}

/** \fn  char DequeueChar()
\brief  Dequeue character.
\return  A char. */
char DequeueChar()
{
  // dequeue oldest byte from queue
  uint8_t help = Queue[QueueOut];
  QueueOut = (QueueOut + 1) % QUEUE_SIZE;
  return help;
}

/** \fn  char get_char()
\brief  Gets the character. get the next character either from the serial port or the internal queue
\return  The character. */
char get_char()
{
  EnqueueSerialData();
  return DequeueChar();
}

/** \fn  boolean CheckSerialData()
\brief  Determines if  serial data is available and stores data in the queue
\return  true if new data is available */
boolean CheckSerialData()
{
  if (Serial.available())
    EnqueueSerialData();

  return QueueIn != QueueOut;
}

/** \fn  boolean DelayAndCheckSerialData(unsigned long ms)
\brief  Delay and check serial data. (while delaying we periodically check for new serial data and store the data)
\param  ms  delay in milliseconds.
\return  true if new serial data is available */
boolean DelayAndCheckSerialData(unsigned long ms)
{
  ms += millis();
  do
  {
    if (Serial.available())
      EnqueueSerialData();
  }
  while (millis() < ms);

  return QueueIn != QueueOut;
}

/** \fn  void byte_to_hex(byte value)
\brief  convert a Byte to two hexadecimal characters. Result stored to byte_to_hex_buffer
\param  value  byte to be converted */
void byte_to_hex(byte value)
{
  byte_to_hex_buffer[0] = hex_digits[value >> 4];        // get upper nibble
  byte_to_hex_buffer[1] = hex_digits[(value & 0x0F)];  // get lower nibble
  //byte_to_hex_buffer[2] = '\0'; // add NULL at end (value is never changed and already initialized to value 0)
}

/** \fn  byte read_hex()
\brief  Reads hexadecimal value from serial port
read 2 hex characters from the serial buffer and convert
them to a byte
\return  the hexadecimal value as a byte. */
byte read_hex()
{
  byte data;
  hex_to_byte_buffer[2] = get_char();
  hex_to_byte_buffer[3] = get_char();
  data = strtol(hex_to_byte_buffer, NULL, 0);
  return (data);
}

/** \fn  boolean tinyDC590B(char command)
\brief  process commands received via serial interface. Those are the m,l commands and the basic DC590B commands
used for DC590B compatibility mode that allows the GUI to communicate with this sketch.
(basic set of DC590 commands to allow I2C communication to the LTC2947)
\param  command  the command to be decoded
\return  true if command was recognized and decoded, false for unknown command */
boolean tinyDC590B(char command)
{
  dc590Busy = true;// DC590 is busy
  switch (command)
  {
    case 't':
    case 'v':
    case 'w':
      // non supported DC590B commands
      //'u': // this is a sub-command of 't', so no need to define it here!!!
      break;
    case 'x':
      output_low(QUIKEVAL_CS);
      break;
    case 'X':
      output_high(QUIKEVAL_CS);
      break;
    case 'D': // delay milliseconds
      delay(read_hex() << 8 | read_hex());
      break;
    case 'g': // IO pin low
      output_low(QUIKEVAL_GPIO);
      break;
    case 'G': // IO pin high
      output_high(QUIKEVAL_GPIO);
      break;
    case 'H': // wait for MISO to go high with a timeout
      {
        long delay_count = 0;
        while (1)
        {
          if (input(MISO) == 1) break;   // MISO is high so quit
          if (delay_count++ > MISO_TIMEOUT)
          {
            //Serial.print('T');         // timeout occurred. Print 'T'
            break;
          }
          else delay(1);
        }
      }
      break;
    case 'L': // wait for MISO to go low with a timeout
      {
        long delay_count = 0;
        while (1)
        {
          if (input(MISO) == 0) break;   // MISO is low so quit
          if (delay_count++ > MISO_TIMEOUT)
          {
            //Serial.print('T');         // timeout occurred. Print 'T'
            break;
          }
          else delay(1);
        }
      }
      break;
    case 'P': // ping
      Serial.print('P');
      delay(5);
      break;
    case 'i': // send controller id string
      pseudo_reset = 0;
      ////////////////////////////////////////////////////
      // CHANGED MAJOR VERSION to 2 FOR ENHANCED VERSION//
      // CHANGED MINOR VERSION to 2 FOR TINY VERSION    //
      // added suffix to identify tiny version          //
      ////////////////////////////////////////////////////
      //Serial.print(F("USBSPI,PIC,01,01,DC,DC590,----------------------\n"));
      Serial.print(F("USBSPI,PIC,02,02,DC,DC590,tinyDC590-DC2574A_KIT-\n"));
      Serial.print('\0'); // quikeval wants to read the null termination character!
      Serial.flush();
      dc590Busy = false;
      break;
    case 'I': // get controller id string
      pseudo_reset = 0;
      // this is DC2334 only, so we will just send its string (no read from demoboard's EEPROM here!)
      Serial.print(F("LTC2947,Cls,D2947,01,01,DC,DC2334A,-------------\n"));
      Serial.print('\0'); // quikeval wants to read the null termination character!
      Serial.flush();
      dc590Busy = false;
      break;
    case 'K': // Set pin state. The pin assignments are :
      {
        // 0: PIND2, Arduino 2
        // 1: PIND3, Arduino 3
        // 2: PIND4, Arduino 4
        // 3: PIND5, Arduino 5
        // 4: PIND6, Arduino 6
        // 5: PIND6, Arduino 7
        char pin_value = get_char();  // read the value
        char pin = get_char();  // read the pin
        digitalWrite(pin - 0x30 + 2, pin_value == '0' ? LOW : HIGH);
      }
      break;
    case 'k': // Get pin state. The pin assignments are the same as for 'K':
      {
        char pin = get_char(); // read the pin
        Serial.print(digitalRead(pin - 0x30 + 2));
      }
      break;
    case 'j': // Set pin mode. The pin assignments are the same as for 'K':
      {
        char pin_value = get_char();  // read the value
        char pin = get_char();  // read the pin
        pinMode(pin - 0x30 + 2, pin_value == '0' ? INPUT : OUTPUT);
      }
      break;
    case 'M': // change the serial mode (I2C only!)
      switch (get_char())
      {
        case 'I': // I2C mode
          serial_mode = i2c_mode;
          quikeval_I2C_connect();
          break;
        case 'S':// spi mode
          serial_mode = spi_mode;
          quikeval_SPI_connect();
          break;
        case 'X':// axillary I2C mode - no hardware action necessary, always available.
          serial_mode = i2c_auxiliary_mode;
          quikeval_SPI_connect();
          break;
      }
      break;
    case 'p': // I2C stop //i2c_stop();
      if (serial_mode != spi_mode) TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
      break;
    case 'Q': // Read byte in I2C mode only. Add ACK
      if (serial_mode != spi_mode)
      {
        byte_to_hex(i2c_read(WITH_ACK));
        Serial.print(byte_to_hex_buffer);
      }
      break;
    case 'r':
      byte_to_hex(spi_read(0));
      Serial.print(byte_to_hex_buffer);
      break;
    case 'R': // Read byte, add NACK in I2C mode
      if (serial_mode != spi_mode)
      {
        byte_to_hex(i2c_read(WITH_NACK));
        Serial.print(byte_to_hex_buffer);
      }
      else
      {
        byte_to_hex(spi_read(0));
        Serial.print(byte_to_hex_buffer);
      }
      break;
    case 's': // I2C start
      if (serial_mode != spi_mode)
        i2c_start();
      break;
    case 'S': // send byte
      if (serial_mode != spi_mode)
      {
        if (i2c_write(read_hex()) == 1) Serial.print('N');
      }
      else
      {
        spi_write(read_hex());
      }
      break;
    case 'T':   // transceive byte
      if (serial_mode == spi_mode)
      {
        byte_to_hex(spi_read(read_hex()));
        Serial.print(byte_to_hex_buffer);
      }
      break;
    case 'Z': // line feed
      Serial.print('\n');
      Serial.flush();
      // The last character the GUI sends for every transaction is always Z
      // this means if we reached this state the GUI finished its communication
      // with DC590 and we are not busy anymore
      dc590Busy = false;
      break;
    case '\x80': // 0x80:Reset
      if (pseudo_reset == 0)
      {
        delay(500); // The delay is needed for older GUI's
        Serial.print("hello\n");
        pseudo_reset = 1;
      }
      break;
    default:
      dc590Busy = false; // if we don't set this to false here, we have to deassert it within processCommand
      return false; // non-DC590B command
  }
  return true;// processed DC590B command
}


