/*!
Linear Technology DC2430 Demonstration Board

@verbatim

DC590B USB to Serial Controller

This file contains the routines to emulate the DC590B USB to Serial Converter. All commands
are supported except Uxxy the Write Port D bus. Added the 'D' delay ms command.
With this program, the Linduino can be used by the QuikEval program running on a PC
to communicate with QuikEval compatible demo boards.

The Kxy bit bang command uses the following pin mappings :
0-Linduino 2
1-Linduino 3
2-Linduino 4
3-Linduino 5
4-Linduino 6
5-Linduino 7

@endverbatim

http://www.linear.com/demo/DC2430A

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
    @ingroup DC2430A
*/


#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "QuikEval_EEPROM.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include <Wire.h>
#include <SPI.h>

// define a "spoof_board_id" to force the "i" command to return a specific ID string.
// This can be used to fake QuikEval into loading a specific GUI, even if the ID EEPROM
// is not present. A few examples are below. NOT defining will result in normal behavior,
// "i" command will cause EEPROM to be read.

//#define spoof_board_id "LTC4261,Cls,D4261,01,01,DC,DC998A,--------------"
//#define spoof_board_id "LTC2440,Cls,D2440,01,01,DC,DC570,---------------"
//#define spoof_board_id "LTC2946,Cls,D2946,01,01,DC,DC2156A,-------------"

// timeouts
#define READ_TIMEOUT  20
#define MISO_TIMEOUT  1000

//* for DC2430 test mode
int DC2430_testmode_ctr = 0;
int  DC2430_Bin = 1;
static int8_t demo_board_connected = 0;    //!< Demo Board Name stored in QuikEval EEPROM
static int8_t demo_board_connected_man = 0;    //!< Demo Board Name stored in QuikEval EEPROM

// recording mode constants
#define RECORDING_SIZE 50
const byte off = 0;
const byte playback = 1;

// serial mode constants
const byte spi_mode = 0;
const byte i2c_mode = 1;
const byte i2c_auxiliary_mode = 2;

// hex conversion constants
char hex_digits[16]=
{
  '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

// spi clock divider
const char spi_divider = SPI_CLOCK_DIV32;  // configure the spi port for 4MHz SCK (500kHz@div32??)

// global variables
byte serial_mode = spi_mode;  // current serial mode
byte recording_mode = off;        // recording mode off
char id_string[51]="USBSPI,PIC,01,01,DC,DC590,----------------------\n\0"; // id string
char hex_to_byte_buffer[5]=
{
  '0', 'x', '0', '0', '\0'
};               // buffer for ASCII hex to byte conversion
char byte_to_hex_buffer[3]=
{
  '\0','\0','\0'
};                     // buffer for byte to ASCII hex conversion
char recording_buffer[RECORDING_SIZE]=
{
  '\0'
}; // buffer for saving recording loop
byte recording_index = 0;                // index to the recording buffer

char get_char();

void byte_to_hex(byte value)
// convert a byte to two hex characters
{
  byte_to_hex_buffer[0]=hex_digits[value>>4];        // get upper nibble
  byte_to_hex_buffer[1]=hex_digits[(value & 0x0F)];  // get lower nibble
  byte_to_hex_buffer[2]='\0';                        // add NULL at end
}

byte read_hex()
// read 2 hex characters from the serial buffer and convert
// them to a byte
{
  byte data;
  hex_to_byte_buffer[2]=get_char();
  hex_to_byte_buffer[3]=get_char();
  data = strtol(hex_to_byte_buffer, NULL, 0);
  return(data);
}

char get_char()
// get the next character either from the serial port
// or the recording buffer
{
  char command='\0';
  if (recording_mode != playback)
  {
    // read a command from the serial port
    while (Serial.available() <= 0);
    return(Serial.read());
  }
  else
  {
    // read a command from the recording buffer
    if (recording_index < RECORDING_SIZE)
    {
      command = recording_buffer[recording_index++];
      // disregard loop commands during playback
      if (command == 'w') command='\1';
      if (command == 't') command='\1';
      if (command == 'v') command='\1';
      if (command == 'u') command='\1';
    }
    else
      command = '\0';
    if (command == '\0')
    {
      recording_index = 0;
      recording_mode = off;
    }
    return(command);
  }
}
int i = 0;
unsigned char pseudo_reset = 0;

//! Initialize Linduino
//! @return void
void setup()
// Setup the program
{
  digitalWrite(QUIKEVAL_GPIO, LOW);
  digitalWrite(QUIKEVAL_CS, HIGH);
  digitalWrite(2, LOW);
  digitalWrite(3, LOW);
  digitalWrite(4, LOW);
  digitalWrite(5, LOW);
  digitalWrite(6, LOW);
  digitalWrite(7, LOW);
  pinMode(QUIKEVAL_GPIO, OUTPUT);
  pinMode(QUIKEVAL_CS, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);

  Serial.begin(115200);          // enable the serial port for 115200 baud

  quikeval_SPI_init();
  quikeval_SPI_connect();     // Connect SPI to main data port

  quikeval_I2C_init();          // Configure the EEPROM I2C port for 100kHz SCK
  Serial.print("hello - DC2430 prog\n");
  Serial.flush();
}

//! Repeats Linduino loop
//! @return void
void loop()
{
  byte tx_data;
  byte rx_data;
  byte pin_value;
  int delay_value;
  int pin;
  char command;
  int byte_count;
  long delay_count;


  command = get_char();


  switch (command)
  {
    case 'A':
      DC2430_testprog();
      break;
    case 'B':
      DC2430_parallelsync_example();
      break;
    case 'D':
      // delay milliseconds
      delay_value = read_hex();
      delay_value<<=8;
      delay_value|=read_hex();
      delay(delay_value);
      break;
    case 'g':
      // IO pin low
      output_low(QUIKEVAL_GPIO);
      break;
    case 'G':
      // IO pin high
      output_high(QUIKEVAL_GPIO);
      break;
    case 'H':     // wait for MISO to go high with a timeout
      delay_count = 0;
      while (1)
      {
        if (input(MISO)==1) break;   // MISO is high so quit
        if (delay_count++>MISO_TIMEOUT)
        {
          //Serial.print('T');         // timeout occurred. Print 'T'
          break;
        }
        else delay(1);
      }
      break;
    case 'i':
      // send controller id string
      pseudo_reset = 0;
      Serial.print(id_string);
      Serial.print('\0');
      Serial.flush();
      break;
    case 'I':
#ifdef spoof_board_id
      Serial.print(spoof_board_id);
      Serial.print('\0');
      Serial.print('\0');
#else
      // get controller id string
      quikeval_SPI_connect();   // Connect SPI to main data port
      pseudo_reset = 0;
      byte_count = read_quikeval_id_string(&ui_buffer[0]);
      if (byte_count!=0)
      {
        Serial.print(ui_buffer);
        Serial.print("\n\0");
        Serial.flush();
      }
#endif
      break;
    case 'K':
      // Bang pin. The pin assignments are :
      // 0: PIND2, Arduino 2
      // 1: PIND3, Arduino 3
      // 2: PIND4, Arduino 4
      // 3: PIND5, Arduino 5
      // 4: PIND6, Arduino 6
      // 5: PIND6, Arduino 7

      pin_value = get_char();          // read the value
//      pin = get_char()-0x30;         // read the pin
      pin = get_char();  // read the pin
//      if (pin_value == '0') digitalWrite(pin+2, LOW);
//      else digitalWrite(pin+2, HIGH);

      digitalWrite(pin-0x30+2, pin_value == '0' ? LOW : HIGH);
      break;
    case 'k':
      // Get pin state. The digial pin assignments are the same as for 'K',
      // Analog pin mappings as follows:
      // 6: Analog A0 (read only)
      // 7: Analog A1 (read only)
      // 8: Analog A2 (read only)
      // 9: Analog A3 (read only)
      pin = get_char(); // read the pin
      if (pin <= '5')
      {
        Serial.print(digitalRead(pin-0x30+2));
      }
      else
      {
        int val = analogRead(pin - '6');
        if (val < 10)
        {
          Serial.print("000");
        }
        else if (val < 100)
        {
          Serial.print("00");
        }
        else if (val < 1000)
        {
          Serial.print("0");
        }
        Serial.print(val);
        break;
      }
      break;
    case 'j':
      // Set pin mode. The pin assignments are the same as for 'K':
      pin_value = get_char();  // read the value
      pin       = get_char();  // read the pin
      pinMode(pin-0x30+2, pin_value == '0' ? INPUT : OUTPUT);
      break;
    case 'L':
      // wait for MISO to go low with a timeout
      delay_count = 0;
      while (1)
      {
        if (input(MISO)==0) break;   // MISO is low so quit
        if (delay_count++>MISO_TIMEOUT)
        {
          //Serial.print('T');         // timeout occurred. Print 'T'
          break;
        }
        else delay(1);
      }
      break;
    case 'M':
      // change the serial mode
      command = get_char();
      switch (command)
      {
        case 'I':
          // I2C mode
          serial_mode = i2c_mode;
          // enable_main_I2C();
          quikeval_I2C_connect();
          break;
        case 'S':
          // spi mode
          serial_mode = spi_mode;
          // Need to send command to disable LTC4302
          // enable_main_SPI();
          quikeval_SPI_connect();
          break;
        case 'X': // axillary I2C mode - no hardware action necessary, always available.
          serial_mode = i2c_auxiliary_mode;
          quikeval_SPI_connect();
          break;
        case '0': // New for Enhanced version - set SPI mode on the fly. (Mainly for Altera SPI-Avalon MM bridge)
          SPI.setDataMode(SPI_MODE0);
          break;
        case '1':
          SPI.setDataMode(SPI_MODE1);
          break;
        case '2':
          SPI.setDataMode(SPI_MODE2);
          break;
        case '3':
          SPI.setDataMode(SPI_MODE3);
          break;
        case 'a':
          SPI.setClockDivider(SPI_CLOCK_DIV2);
          break;
        case 'b':
          SPI.setClockDivider(SPI_CLOCK_DIV4);
          break;
        case 'c':
          SPI.setClockDivider(SPI_CLOCK_DIV8);
          break;
        case 'd':
          SPI.setClockDivider(SPI_CLOCK_DIV16);
          break;
        case 'e':
          SPI.setClockDivider(SPI_CLOCK_DIV32);
          break;
        case 'f':
          SPI.setClockDivider(SPI_CLOCK_DIV64);
          break;
        case 'g':
          SPI.setClockDivider(SPI_CLOCK_DIV128);
          break;
      }
      //delay(1);
      break;
    case 'p':
      // I2C stop
      // Switching BACK to the i2c_stop() function call. See Rev 1405, LT_i2c.cpp, Linduino One branch
      if (serial_mode!=spi_mode) i2c_stop(); //   TWCR=(1<<TWINT) | (1<<TWEN) | (1<<TWSTO);  // I2C stop //i2c_stop();
      // if(serial_mode == i2c_auxiliary_mode) i2c_stop();
      break;
    case 'P':
      // ping
      Serial.print('P');
      delay(5);
      break;
    case 'Q':
      // Read byte in I2C mode only. Add ACK
      switch (serial_mode)
      {
        case i2c_mode:
          rx_data = i2c_read(WITH_ACK);
          byte_to_hex(rx_data);
          Serial.print(byte_to_hex_buffer);
          break;
        case i2c_auxiliary_mode:
          rx_data = i2c_read(WITH_ACK);
          byte_to_hex(rx_data);
          Serial.print(byte_to_hex_buffer);
          break;
      }
      break;

    case 'r':
      rx_data = spi_read(0);
      byte_to_hex(rx_data);
      Serial.print(byte_to_hex_buffer);
      break;

    case 'R':
      // Read byte, add NACK in I2C mode
      switch (serial_mode)
      {
        case spi_mode:
          rx_data = spi_read(0);
          byte_to_hex(rx_data);
          Serial.print(byte_to_hex_buffer);
          break;
        case i2c_mode:
          rx_data = i2c_read(WITH_NACK);
          byte_to_hex(rx_data);
          Serial.print(byte_to_hex_buffer);
          break;
        case i2c_auxiliary_mode:
          rx_data = i2c_read(WITH_NACK);
          byte_to_hex(rx_data);
          Serial.print(byte_to_hex_buffer);
          break;
      }
      break;
    case 's':   // I2C start
      if (serial_mode == i2c_mode) i2c_start();
      if (serial_mode == i2c_auxiliary_mode) i2c_start();
      break;
    case 'S':   // send byte
      tx_data = read_hex();
      switch (serial_mode)
      {
        case spi_mode:
          spi_write(tx_data);
          break;
        case i2c_mode:
          if (i2c_write(tx_data)==1) Serial.print('N');
          break;
        case i2c_auxiliary_mode:
          if (i2c_write(tx_data)==1) Serial.print('N');
          break;
      }
      break;
    case 't':   // recording loop
      recording_index = 0;
      do
      {
        command = get_char();
        if (command == 'u')   // stop recording
        {
          recording_buffer[recording_index]='\0';
          recording_index = 0;
          break;
        }
        else            // add character to recording buffer
        {
          if (recording_index < RECORDING_SIZE) recording_buffer[recording_index++]=command;
        }
      }
      while (1);
      break;
    case 'T':   // transceive byte
      tx_data = read_hex();
      if (serial_mode == spi_mode)
      {
        rx_data = spi_read(tx_data);
        byte_to_hex(rx_data);
        Serial.print(byte_to_hex_buffer);
      }
      break;
    case 'v':    // echo recording loop
      Serial.print(recording_buffer);
      break;
    case 'w':
      recording_mode = playback;
      break;
    case 'x':
      output_low(QUIKEVAL_CS);
      break;
    case 'X':
      output_high(QUIKEVAL_CS);
      break;
    case 'Z':    // line feed
      Serial.print('\n');
      Serial.flush();
      break;
    case 0x80:    // Reset
      if (pseudo_reset == 0)
      {
        delay(500); // The delay is needed for older GUI's
        Serial.print("hello\n");
        pseudo_reset = 1;
      }
      break;
  }
}

/* ******************************************************************************************* */
//!   DC2430 PARALLELSYNC EXAMPLE: SOFTWARE SITE SELECTION MODE
//!   - ENABLED BY SELECTING 'B'
//!   - this software example assumes the following
//!     * DC1954 J12 connected to DC2430 J5
//!     * DC2248 J12 connected to DC2430 J6
//!     * DC2248 J12 connected to DC2430 J7
//!     * all SMA connections described in DC2430 Demo Manual
//!     * DC2430 JP3 SYNC shorted to DC2430 GPIO7 (default state)
//!     * All demo boards are powered up
//!     (refer to the DC2430 demo manual)
//!
//!   - This function performs the following
//!     * confirms DC2430 & DC2026 using correct settings
//!     * selects DC2430 J5 site
//!     * programs the LTC6954
//!     * selects DC2430 J6 site
//!     * programs the LTC6951 #1
//!     * selects DC2430 J7 site
//!     * programs the LTC6951 #2
//!     * Performs ParallelSync, by toggling GPIO7
//! @return void
void DC2430_parallelsync_example()
{
  char demo_name1[] = "DC1954";    // LTC6954 Demo Board Name stored in QuikEval EEPROM
  char demo_name2[] = "DC2248";    // LTC6951 Demo Board Name stored in QuikEval EEPROM

  DC2430_Bin = 1;

  /* confirms DC2430 & DC2026 using correct settings */
  DC2430_test_supply();

  if (DC2430_Bin == 1)
  {
    /* selects DC2430 J5 site */
    Serial.println("1. Setting DC2430 to J5 site");
    changeDC2430site(5);
    // delay(5);
    demo_board_connected = discover_demo_board(demo_name1);
    if (demo_board_connected == 0)  Serial.println("\n\n  ***** LTC6954 Demo board not attached to J5, fix this and try again *****\n\n ");

    /* programs the LTC6954 */
    Serial.println("\n2. Programming LTC6954");
    Prog_LTC6954_parallelsync_example();

    /* selects DC2430 J6 site */
    Serial.println("\n3. Setting DC2430 to J6 site");
    changeDC2430site(6);
    // delay(5);
    demo_board_connected = discover_demo_board(demo_name2);
    if (demo_board_connected == 0)  Serial.println("\n\n  **** LTC6951 Demo board not attached to J6, fix this and try again ***** \n\n");

    /* programs the LTC6951 #1 */
    Serial.println("\n4. Programming LTC6951 #1");
    Prog_LTC6951_parallelsync_example();

    /* selects DC2430 J7 site */
    Serial.println("\n5. Setting DC2430 to J7 site");
    changeDC2430site(7);
    // delay(5);
    demo_board_connected = discover_demo_board(demo_name2);
    if (demo_board_connected == 0)  Serial.println("\n\n  **** LTC6951 Demo board not attached to J7, fix this and try again ****\n\n");

    /* programs the LTC6951 #2 */
    Serial.println("\n6. Programming LTC6951 #2");
    Prog_LTC6951_parallelsync_example();

    /* Performs ParallelSync, by toggling GPIO7 */
    Serial.println("\n7. Performing ParallelSync");
    digitalWrite(7, HIGH); /* forces SYNC1 and SYNC2 high when REFIN is clocked */
    delay(1);  /* LTC6951 parallelsync must be > 1ms */
    digitalWrite(7, LOW); /* forces SYNC1 and SYNC2 high when REFIN is clocked */

    Serial.println("\n7. Done!");
    changeDC2430site(8);   /* return to Manual Mode */

  } /* end of if (DC2430_Bin == 1) */
} /* end of DC2430_parallelsync_example */

//!  This function programs LTC6954-2 SPI registers.
//!  such that all 3 channels of the LTC6954-2 are set to /1.
//!  The LTC6954-2 frequency plan in this example matches that of the LTC6951
//!  ParallelSYNC datasheet application.
//! @return void
void Prog_LTC6954_parallelsync_example()
{
  uint8_t *rx;
  uint8_t val_addr, val_reg;
  uint8_t tx_input_array[7];

  /* Load initial register settings */
  tx_input_array[0] = 0x00;                       //! addr 1, shift 1 bit to account for RW bit (W=0)
  tx_input_array[1] = 0x80;
  tx_input_array[2] = 0x00;
  tx_input_array[3] = 0x80;
  tx_input_array[4] = 0x00;
  tx_input_array[5] = 0x80;
  tx_input_array[6] = 0x00;

  digitalWrite(QUIKEVAL_CS, LOW);
  val_addr = 0<<1;                       //! addr 0, shift 1 bit to account for RW bit (W=0)
  *rx = SPI.transfer(val_addr);          //! send ADDR2 byte

  for (i=0; i<7; i++)
  {
    *rx = SPI.transfer(tx_input_array[i]);           //! send byte
  }
  digitalWrite(QUIKEVAL_CS, HIGH);
}

//!   Prog_LTC6951_parallelsync_example()
//!      This function programs the LTC6951 registers for ParallelSYNC mode.
//!      In ParallelSYNC mode:
//!         step 1) program all LTC6951 registers
//!         step 2) calibrate LTC6951 VCO
//!      The LTC6951 frequency plan in this example matches that of the LTC6951
//!      ParallelSYNC datasheet application.
//!      Register settings in this function are identical to the LTC6951Wizard's set file,
//!     "Datasheet Typical Applications/6951_ParallelSyncw6954.6951set”
//! @return void
void Prog_LTC6951_parallelsync_example()
{
  uint8_t *rx;
  uint8_t val_addr, val_reg;
  uint8_t tx_input_array[19];

  /* Load initial register settings */
  tx_input_array[0] = 1<<1;                       //! addr 1, shift 1 bit to account for RW bit (W=0)
  tx_input_array[1] = 0xba;
  tx_input_array[2] = 0x00;
  tx_input_array[3] = 0x74;
  tx_input_array[4] = 0xb3;
  tx_input_array[5] = 0x04;
  tx_input_array[6] = 0x02;
  tx_input_array[7] = 0x07;
  tx_input_array[8] = 0x00;
  tx_input_array[9] = 0x14;
  tx_input_array[10] = 0xc0;
  tx_input_array[11] = 0x96;
  tx_input_array[12] = 0x06;
  tx_input_array[13] = 0x96;
  tx_input_array[14] = 0x0c;
  tx_input_array[15] = 0x96;
  tx_input_array[16] = 0x06;
  tx_input_array[17] = 0x30;
  tx_input_array[18] = 0x00;

  digitalWrite(QUIKEVAL_CS, LOW);
  for (i=0; i<19; i++)
  {
    *rx = SPI.transfer(tx_input_array[i]);           //! send byte
  }
  digitalWrite(QUIKEVAL_CS, HIGH);
  delay(1); /*wait just in case device hasn't let VCO settle - can decrease */

  /* toggle VCO cal bit */
  digitalWrite(QUIKEVAL_CS, LOW);
  val_addr = 2<<1;                       //! addr 2, shift 1 bit to account for RW bit (W=0)
  *rx = SPI.transfer(val_addr);          //! send ADDR2 byte
  val_reg = 0x01;                       //! set cal bit high
  *rx = SPI.transfer(val_reg);          //! send ADDR2 REG byte
  digitalWrite(QUIKEVAL_CS, HIGH);

}

/* ******************************************************************************************* */
//!   DC2430 TEST SECTION
//!   - ENABLED BY SELECTING 'A'
//! @return void
void DC2430_testprog()
{
  DC2430_Bin = 1;

  Serial.println(F("***************************"));
  Serial.println(F("*      START OF TEST      *"));
  Serial.println(F("***************************"));
  if (DC2430_Bin==1)
    DC2430_test_supply();
  if (DC2430_Bin==1)
    DC2430_test_SYNC();
  if (DC2430_Bin==1)
    DC2430_test_manualmode();

  for (DC2430_testmode_ctr=0; DC2430_testmode_ctr<8; DC2430_testmode_ctr++)
  {
    if (DC2430_Bin==1)
      DC2430_test_swmode();
  }
  DC2430_testmode_ctr++;
  changeDC2430site(DC2430_testmode_ctr);   // returns DC2430 to manual mode
  Serial.print(F("Bin = "));
  Serial.println(DC2430_Bin);
}

//! Function to check DC2430 voltages generated from the EEVCC
//! and VCCIO Linduion supply voltages
//! @return void
void DC2430_test_supply()
{
  int dc2430_Aval;
  float dc2430_Aval_V0;
  float dc2430_Aval_V1;
  float dc2430_Aval_V2;

  Serial.println(F("\n *** Testing Linduino Supplies on DC2430A board ***"));

  // For DC2430 shield, when mated with a linduino boards:
  // checks if DC2430 connected
  //   returns
  //     -9.99, if DC2430 not connected
  //     Measured VCCIO voltage, if DC2430 connected and Linduino's VCCIO (see Linduino JP3 jumper)
  dc2430_Aval = analogRead(0);     // measures DC2430 resistor divider, should be 3.33V
  dc2430_Aval_V0 = (5.0*dc2430_Aval)/1023;

  dc2430_Aval = analogRead(1);     // measures DC2430 resistor divider, should be 1.66V
  dc2430_Aval_V1 = (5.0*dc2430_Aval)/1023;

  dc2430_Aval_V2=-9.99;   // initialized to -9.99V, only reported if DC2430 not connected
  if ((dc2430_Aval_V0>3.0) && (dc2430_Aval_V0<3.6) && (dc2430_Aval_V1>1.5) && (dc2430_Aval_V1<1.8))
  {
    Serial.println(F("PASS: EEVCC voltage  =  5V"));
  }
  else
  {
    DC2430_Bin=2;
    Serial.print(F("\n**** FAIL: EVCC voltage test "));
    Serial.println(dc2430_Aval_V0);
    Serial.println(dc2430_Aval_V1);
  }

  if (DC2430_Bin == 1)
  {
    dc2430_Aval = analogRead(2);  // measures VCCIO voltage (see Linduino JP3 jumper)
    dc2430_Aval_V2 = (5.0*dc2430_Aval)/1023;
    if ((dc2430_Aval_V2>3.0) && (dc2430_Aval_V2<3.6))
    {
      Serial.print(F("PASS: VCCIO voltage  = "));
    }
    else
    {
      DC2430_Bin=3;
      Serial.println(F("\nIs the DC2026C JP3 jumper set to the 3.3V location?"));
      Serial.print(F("**** FAIL: VCCIO voltage  = "));
    }
    Serial.println(dc2430_Aval_V2);
  }
  Serial.print(F("\n\0"));
  Serial.flush();
}

//! Function to verify that the DC2430 ref to sync circtuiry is functional
//! @return void
void DC2430_test_SYNC()
{
  char command1, command2;
  Serial.println(F("\n\n *** Testing DC2430 SYNC circuitry ***"));

  digitalWrite(7, LOW);  /* forces SYNC1 and SYNC2 low when REFIN is clocked */
  Serial.println(F("Measure output voltages at J21 (SYNC1) & J22 (SYNC2)"));
  Serial.println(F("Are both voltages < 0.2V? [Y/N]"));
  get_char();
  command1 = get_char();
  if (command1=='Y')
    command1 = 'y';

  digitalWrite(7, HIGH); /* forces SYNC1 and SYNC2 high when REFIN is clocked */
  Serial.println(F("Measure output voltages at J21 (SYNC1) & J22 (SYNC2)"));
  Serial.println(F("Are both voltages > 3.1V and < 3.5V? [Y/N]"));
  get_char();
  command2 = get_char();
  if (command2=='Y')
    command2 = 'y';
  digitalWrite(7, LOW);  /* return to default state */

  if ((command1=='y') && (command2=='y'))
  {
    Serial.print(F("PASS: SYNC Test "));
  }
  else
  {
    DC2430_Bin=8;
    Serial.print(F("**** FAIL: SYNC Test "));
  }
}



//!   DC2430_test_swmode() - verified
//!    - each site can be selected via softward
//!    - EEPROM and SPI pins on the quick eval connectors work properly
//! @return void
void DC2430_test_swmode()
{
  char demo_name[] = "DC2248";    // Demo Board Name stored in QuikEval EEPROM
  byte tx_data, rx_data;
  uint8_t *rx;
  uint8_t val_addr, val_reg;
  uint8_t rx_data_array[5];
  uint8_t tx_input_array[5];

  if (DC2430_testmode_ctr==0) Serial.println(F("\n\n *** Testing QuickEval Connectors ***"));

  /* set DC2430 to a site based on the DC2430_testmode_ctr number */
  changeDC2430site(DC2430_testmode_ctr);

  Serial.print(F("\n>>>>>>>> Move Quickeval Connector to DC2430 connector J"));
  Serial.println(DC2430_testmode_ctr);
  Serial.println(F("Press Enter when complete"));
  get_char();

  /* ***** THIS TEST THE EEPROM & EVCC CONNECTIONS OF THE DC2430 to the DC2248 ******** */
  demo_board_connected = discover_demo_board_local(demo_name);
  if (demo_board_connected==0)
  {
    DC2430_Bin=4;
    Serial.print(F("\nEnsure a DC2248 is connected to DC2430 Connector J"));
    Serial.println(DC2430_testmode_ctr);
    Serial.print(F("**** FAIL: EEPROM I2C pins connector J"));
    Serial.println(DC2430_testmode_ctr);
  }
  else
  {
    Serial.print(F("PASS: EEPROM I2C pins connecter J"));
    Serial.println(DC2430_testmode_ctr);
  }  /* end of if(demo_board_connected==0) */

  /* ***** THIS TEST THE SPI & VCCIO CONNECTIONS OF THE DC2430 to the DC2248 ******** */
  if (DC2430_Bin==1)
  {
    /* initially - select POR bit - gets LTC6951 to known state */
    digitalWrite(QUIKEVAL_CS, LOW);
    val_addr = 2<<1;                       //! addr 2, shift 1 bit to account for RW bit (W=0)
    *rx = SPI.transfer(val_addr);          //! send ADDR2 byte
    val_reg= 0x02;                         //! send 0x02 - POR=1
    *rx = SPI.transfer(val_reg);           //! send ADDR2 reg - POR reset
    digitalWrite(QUIKEVAL_CS, HIGH);
    delay(50);

    /* force some values that ensure the read and write pins work appropriately */
    tx_input_array[0] = 0x49;
    tx_input_array[1] = 0x80;   /* turns on LED - visual test in is in progress */
    tx_input_array[2] = 0x00;   /* power down register - all 0's */
    tx_input_array[3] = 0xaa;   /* alternate 1-0-1-0... */
    tx_input_array[4] = 0x55;   /* alternate 0-1-0-1... */

    digitalWrite(QUIKEVAL_CS, LOW);
    val_addr = 1<<1;                       //! addr 1, shift 1 bit to account for RW bit (W=0)
    *rx = SPI.transfer(val_addr);          //! send ADDR1 byte
    for (i=1; i<5; i++)
    {
      *rx = SPI.transfer(tx_input_array[i]);           //! send byte
    }
    digitalWrite(QUIKEVAL_CS, HIGH);
    delay(200);

    digitalWrite(QUIKEVAL_CS, LOW);
    val_addr = 1   ;                       //! addr 3, shift 1 bit to account for RW bit (R=1)
    *rx = SPI.transfer(val_addr);          //! send ADDR3 byte
    for (i=0; i<5; i++)
    {
      rx_data_array[i] = spi_read(0);
      byte_to_hex(rx_data_array[i]);
      /* this line compares the input values to the readback values */
      if (tx_input_array[i]!=rx_data_array[i]) DC2430_Bin=5;
    }
    digitalWrite(QUIKEVAL_CS, HIGH);

    /* lastly - select POR bit - gets LTC6951 to known state - turns off LED */
    digitalWrite(QUIKEVAL_CS, LOW);
    val_addr = 2<<1;                       //! addr 2, shift 1 bit to account for RW bit (W=0)
    *rx = SPI.transfer(val_addr);          //! send ADDR2 byte
    val_reg= 0x02;                         //! send 0x02 - POR=1
    *rx = SPI.transfer(val_reg);           //! send ADDR2 reg - POR reset
    digitalWrite(QUIKEVAL_CS, HIGH);

    if (DC2430_Bin==5)
    {
      Serial.print(F("**** FAIL: SPI pins connector J"));
      Serial.println(DC2430_testmode_ctr);
    }
    else
    {
      Serial.print(F("PASS: SPI pins connecter J"));
      Serial.println(DC2430_testmode_ctr);
    }  /* end of if(DC2430_Bin==5) */
  } /* end of if (DC2430_Bin==5) */

}  /* end of function */


//!   DC2430_test_manualmode() - verified
//!    - test site 2 (b010) and site 5 (b101)
//!    - EEPROM and SPI pins on the quick eval connectors work properly
//! @return void
void DC2430_test_manualmode()
{
  char demo_name[] = "DC2248";    // Demo Board Name stored in QuikEval EEPROM
  int manual_site_selection;
  int jj;
  int did_not_follow_instructions;

  Serial.println(F("\n\n *** Testing Manual Switch (SW1) ***"));

  for (i=0; i<2; i++)
  {
    if (i==0) manual_site_selection=5; /* code 101 */
    if (i==1) manual_site_selection=2; /* code 010 */

    did_not_follow_instructions=0;
    jj=0;
    do
    {
      if (jj>0) Serial.println("\n\n OOPS!!!!! - please read instructions");
      Serial.print(F(">> STEP 1: Move Quickeval Connector to DC2430 connector J"));
      Serial.println(manual_site_selection);
      Serial.print(F(">> STEP 2: Turn SW1 so that LED by J"));
      Serial.print(manual_site_selection);
      Serial.println(F(" turns on"));
      Serial.println(F("Press Enter when complete"));
      if ((jj==0) && (i== 0)) get_char();
      get_char();

      /* ***** THIS TEST THE EEPROM & EVCC CONNECTIONS OF THE DC2430 to the DC2248 ******** */
      demo_board_connected_man = discover_demo_board_local(demo_name);

      /* this confirms the connector is at site 5 */
      changeDC2430site(manual_site_selection);  /* software site select */
      demo_board_connected = discover_demo_board_local(demo_name);
      changeDC2430site(9);  /* disables software site select */
      jj++;

      if ((demo_board_connected==0) || ((demo_board_connected_man==0) && (demo_board_connected==1)) )
        did_not_follow_instructions=1;

    }
    while ((did_not_follow_instructions== 1) && (jj<2));    /* giving tester a couple chances to move quick eval connector */

    if ((demo_board_connected_man==0) || (demo_board_connected==0))
    {
      DC2430_Bin=6;
      Serial.print(F("**** FAIL: Manual Switch Test - J"));
      Serial.println(manual_site_selection);
    }
    else
    {
      Serial.print(F("PASS: Manual Switch Test - J"));
      Serial.println(manual_site_selection);
    }  /* end of if(demo_board_connected==0) */
    Serial.print("\n");
  }  /* end of for loop */
}  /* end of function */



//! Read the ID string from the EEPROM and determine if the correct board is connected.
//! @return Returns 1 if successful, 0 if not successful
int8_t discover_demo_board_local(char *demo_name)
{
  int8_t connected;
  connected = 1;
  // read the ID from the serial EEPROM on the board
  // reuse the buffer declared in UserInterface
  if (read_quikeval_id_string(&ui_buffer[0]) == 0) connected = 0;

  return(connected);
}

//! Function to set DC2430 to a site based on the DC2430_testmode_ctr number
//! @return void
void changeDC2430site(int site_select)
{
  switch (site_select)
  {
    case 0:
      digitalWrite(5, HIGH);  // EN
      digitalWrite(4, LOW);   // DA2
      digitalWrite(3, LOW);   // DA1
      digitalWrite(2, LOW);   // DA0;
      break;

    case 1:
      digitalWrite(5, HIGH);  // EN
      digitalWrite(4, LOW);   // DA2
      digitalWrite(3, LOW);   // DA1
      digitalWrite(2, HIGH);  // DA0;
      break;

    case 2:
      digitalWrite(5, HIGH);  // EN
      digitalWrite(4, LOW);   // DA2
      digitalWrite(3, HIGH);  // DA1
      digitalWrite(2, LOW);   // DA0;
      break;

    case 3:
      digitalWrite(5, HIGH);  // EN
      digitalWrite(4, LOW);   // DA2
      digitalWrite(3, HIGH);  // DA1
      digitalWrite(2, HIGH);  // DA0;
      break;

    case 4:
      digitalWrite(5, HIGH);  // EN
      digitalWrite(4, HIGH);  // DA2
      digitalWrite(3, LOW);   // DA1
      digitalWrite(2, LOW);   // DA0;
      break;

    case 5:
      digitalWrite(5, HIGH);  // EN
      digitalWrite(4, HIGH);  // DA2
      digitalWrite(3, LOW);   // DA1
      digitalWrite(2, HIGH);  // DA0;
      break;

    case 6:
      digitalWrite(5, HIGH);  // EN
      digitalWrite(4, HIGH);  // DA2
      digitalWrite(3, HIGH);  // DA1
      digitalWrite(2, LOW);   // DA0;
      break;

    case 7:
      digitalWrite(5, HIGH);  // EN
      digitalWrite(4, HIGH);  // DA2
      digitalWrite(3, HIGH);  // DA1
      digitalWrite(2, HIGH);  // DA0;
      break;

    default:
      digitalWrite(5, LOW);  // EN
      digitalWrite(4, LOW);  // DA2
      digitalWrite(3, LOW);  // DA1
      digitalWrite(2, LOW);  // DA0;
      break;
  } /* end of case */
  delay(10);
}
