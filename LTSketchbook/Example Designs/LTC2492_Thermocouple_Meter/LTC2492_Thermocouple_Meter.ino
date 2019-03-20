/*
Complete Digital Thermocouple Meter Using the LTC2492
24 bit Delta Sigma ADC with onboard temperature sensor

Mark Thoren
Linear Technonlgy Corporation
September 9, 2004

*** Much of this description needs updating - 4 line display, improved
curve fitting, LTC2492 stuff**********


Cold Junction Compensated from -40 to 85C (industrial temperature range)
using second order curve-fit.
Type K thermocouple range -100C to 300C. Fourth order curve fit is accurate
to 1 degree C in this range.

Output is to a 2 line by 16 character LCD display using the standard
Hitachi 44780 (or compatible) controller.

Example Output:
__________________
|V 2.35 TC 58.2   |     (thermocouple voltage in mv, calculated thermocouple temp.)
|PT 424.1 CJ 21.5 |     (PTAT voltage, calculated local temp.)
------------------

A calibration feature allows the inital error in the internal
reference and PTAT circuits to be calibrated out. The user applies a precise
500mV to the thermocouple input and measuring the local temperature of the LTC2480.
Output is calculated reference voltage on first line, calculated PTAT tempco and
local temperature on second line. During calibration, UP / DOWN buttons adjust
the local temperature in 0.1C increments. Pulling the jumper exits calibration
and kicks you into standard running mode.

Example Calibration Screen:
__________________
|VREF:1.2401      |     (Calculated Vref based on 500mV input)
|P:1.415 CJ:26.8  |     (PTAT coefficient (mv/K), local temperature.)
------------------

*** Code was written for clarity, not efficiency. There are lots of extra
variables and steps that could be eliminated.

Copyright 2011(c) Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
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
   
THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A
PARTICULAR PURPOSE ARE DISCLAIMED.

IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, INTELLECTUAL PROPERTY
RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF 
THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


*/

#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LTC2492_original.h"
#define HARDCODED_CONSTANTS
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <SPI.h>
#include <LT_SPI.h>
#include <Wire.h>
#include <LT_I2C.h>
#include "LTC24XX_general.h"

#define CAL_JUMPER 1
#define INC_BUTTON 2
#define DEC_BUTTON 3
#define CS 9 // REMAPPED - requires a cut trace

// LCD constants - This is a great pinout, does not step on any
// "Digital" pins, 
const int16_t RS = 3;
const int16_t E = 2;
const int16_t D4 = A0;
const int16_t D5 = A1;
const int16_t D6 = A2;
const int16_t D7 = A3;

struct fourbytes  // Define structure of four consecutive bytes
   {              // To allow byte access to a 32 bit int or float.
   int8_t te0;      //
   int8_t te1;      // The make32() function in this compiler will
   int8_t te2;      // also work, but a union of 4 bytes and a 32 bit int
   int8_t te3;      // is probably more portable.
   };

// Global Variables.
   // 'x' is a dummy for holding integer conversion result. It appears the compiler
   // will only cast a variable, not a function. So if x is an int32, (float) x works
   // but if function() returns an int32, (float) function() does not work.
   int32_t x;
//   float tc_voltage;    // Measured thermocouple voltage
   float cj_voltage;    // Calculated cold junct voltage based on type K thermocouple

   float poly_voltage;  // For raising tc_voltage to higher powers
   float ptat_voltage;  // Voltage from the internal PTAT circuit
   float cj_raw;
   float tc_raw;
   float cj_temp;       // Calculated cold junction temperature based on PTAT voltage

   float vref = 1.25;

   // vin_cal and ptat_cal are done this way to allow storing to local EEPROM
   // as four bytes.

// 
// LT_union_float_4bytes vin_cal;

  union                         // vin_cal.fp        32 bit floating point
     {                          // vin_cal.by.te0    byte 0
     float fp;                  // vin_cal.by.te1    byte 1
     struct fourbytes by;       // vin_cal.by.te2    byte 2
     } vin_cal;                 // vin_cal.by.te3    byte 3

   union                         // ptat_cal.fp        32 bit floating point
      {                          // ptat_cal.by.te0    byte 0
      float fp;                  // ptat_cal.by.te1    byte 1
      struct fourbytes by;       // ptat_cal.by.te2    byte 2
      } ptat_cal;                // ptat_cal.by.te3    byte 3

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(RS, E, D4, D5, D6, D7);


/*** setup() **************************************************************
Basic hardware initialization of controller and LCD, send Hello message to LCD
*******************************************************************************/
void setup()
   {
   // General initialization stuff.
//   setup_adc_ports(NO_ANALOGS);
//   setup_adc(ADC_OFF);
//   setup_counters(RTCC_INTERNAL,RTCC_DIV_1);
//   setup_timer_1(T1_DISABLED);
//   setup_timer_2(T2_DISABLED,0,1);

// This is the important part - configuring the SPI port
//   setup_spi(SPI_MASTER|SPI_L_TO_H|SPI_CLK_DIV_16|SPI_SS_DISABLED); // fast SPI clock
//   CKP = 0; // Set up clock edges - clock idles low, data changes on
//   CKE = 1; // falling edges, valid on rising edges.

    lcd.begin(16, 4);               // Initialize LCD
    Serial.begin(115200);         // Initialize the serial port to the PC

    // Display Message
    lcd.print("LTC2492 Type K TC Meter");
    Serial.println("LTC2492 Type K TC Meter");
    delay(200);
    quikeval_SPI_init();            // Configure the spi port for 4volts SCK
//    quikeval_I2C_init();            // Configure the EEPROM I2C port for 100kHz
    quikeval_SPI_connect();         // Connect SPI to main data port

   // lcd_init();                   // Initialize LCD
   // delay_ms(6);
   // printf(lcd_putc, "Hello!");   // Obligatory hello message
   // delay_ms(500);                // for half a second
   } // End of initialize()


/*** main() ********************************************************************
Main program initializes microcontroller registers, checks to see if calibration
mode is selected, reads calibration constants from nonvolatile memory.
Main loop reads ADC input and PTAT voltage, calculates temperatures and voltages,
then sends this information to the display.
*******************************************************************************/
void loop()
   {

   float ch0_voltage, ch0_temp, ch1_voltage, ch1_temp;
//   initialize();
//   if(!digitalRead(CAL_JUMPER)) calibrate();    // See if calibration jumper is installed
   // Read stored calibration values from first 8 EEPROM locations.

//   output_high(PIN_C1);
//   read_calibration();

   while(1)
      {
      Serial.println("Reading ADC...");
      LTC24XX_EOC_timeout(QUIKEVAL_CS, 250);
      LTC24XX_SPI_16bit_command_32bit_data(QUIKEVAL_CS, LTC24XX_MULTI_CH_P1_N0 ,  LTC24XX_EZ_MULTI_VIN | LTC24XX_EZ_MULTI_R55 | LTC24XX_EZ_MULTI_SLOW,  &x);

//      x = read_LTC2492(CH0_1, VIN | R55 | SLOW);   // Read PTAT voltage, program to read input 0-1
      Serial.print("Got Code: "); Serial.println(x, BIN);
      Serial.println("Calculating Temp");
      // If we have room, check to see if there is a big step function, then reset filter.
//      cj_raw = 0.95 * cj_raw + 0.05 * (float) x;   // convert to float and apply digital filter
      cj_raw = (float) x;
                                                   // 500 is the assumed calibration signal (see calibrate function)
      ptat_voltage = LTC24XX_diff_code_to_voltage(x, vref);;  // this is not necessary, but useful for debugging.
      cj_temp = (ptat_voltage / 0.0000935) - 273.15;   // Calculate local temperature in C

      // Compensate measured thermocouple voltage for cold junction temperature based on
      // second order curve fit. Second order is adequate because it only needs to
      // compensate from -40 to 85C (industrial temperature range for LTC24XX.)
      cj_voltage = .0005265 + .0393 * cj_temp;        // Constant and 1st order term
      cj_voltage += .000018477 * cj_temp * cj_temp;   // Squared term

      LTC24XX_EOC_timeout(QUIKEVAL_CS, 250);
LTC24XX_SPI_16bit_command_32bit_data(QUIKEVAL_CS, LTC24XX_MULTI_CH_P3_N2 ,  LTC24XX_EZ_MULTI_VIN | LTC24XX_EZ_MULTI_R55 | LTC24XX_EZ_MULTI_SLOW,  &x);
      x = read_LTC2492(CH2_3, VIN | R55 | SLOW);   // Read input 0-1 voltage, program to convert input 0-1
      ch0_voltage = LTC24XX_diff_code_to_voltage(x, vref);
      ch0_temp = type_K_V2C(ch0_voltage, cj_voltage);

      LTC24XX_EOC_timeout(QUIKEVAL_CS, 250);
LTC24XX_SPI_16bit_command_32bit_data(QUIKEVAL_CS, LTC24XX_MULTI_CH_P1_N0 ,  LTC24XX_EZ_MULTI_PTAT ,  &x);
      x = read_LTC2492(NO_CH, PTAT | R55);         // Read input voltage, program to convert PTAT
      ch1_voltage = LTC24XX_diff_code_to_voltage(x, vref);
      ch1_temp = type_K_V2C(ch1_voltage, cj_voltage);


   lcd.setCursor(1,1);
   Serial.print("0T  "); Serial.println(ch0_temp, 1);
   lcd.setCursor(11,1);
   Serial.print("1T  "); Serial.println(ch1_temp, 1);

   lcd.setCursor(1,2);
   Serial.print("0V  "); Serial.println(ch0_voltage,3);
   lcd.setCursor(11,2);
   Serial.print("1V  "); Serial.println(ch1_voltage,3);

// Print calculated thermocouple temperature
//   lcd.setCursor(8,1);

// Print PTAT circuit voltage
   lcd.setCursor(1,3);
   Serial.print("CJ TEMP %01.1f "); Serial.print(cj_temp, 1);
// Print local temperature based on PTAT voltage
   lcd.setCursor(1,4);
   Serial.print("PT V "); Serial.print(ptat_voltage,2); Serial.print("CJ V "); Serial.println(cj_voltage, 2);
      } // End of main loop
   } // End of main()



//////////////////////////////////////////////////////////////////////////////
// Calculate type K thermocouple temperature in degrees C from thermocouple //
// voltage in volts and cold junction temperature in degrees C.             //
//////////////////////////////////////////////////////////////////////////////
float type_K_V2C(float tc_voltage, float cj_voltage)
   {
   float tc_temp;       // Calculated thermocouple temperature
   tc_voltage = tc_voltage + cj_voltage;                       // Adjust thermocouple voltage
// Calculate temperature based on 5th order curve fit.
// These values are 3 degrees accurate from -100C to +1370C.
   tc_temp = 0.0;                          // a0 (constant)
   poly_voltage = tc_voltage;                      // First power
   tc_temp += poly_voltage * 25.57848;            // a1
   poly_voltage = poly_voltage * tc_voltage;       // Squared
   tc_temp += poly_voltage * -0.1165412;          // a2
   poly_voltage = poly_voltage * tc_voltage;       // Cubed
   tc_temp += poly_voltage * 0.002922928;          // a3
   poly_voltage = poly_voltage * tc_voltage;       // Fourth power
   tc_temp += poly_voltage * -0.00003270702;        // a4
   poly_voltage = poly_voltage * tc_voltage;       // Fifth power
   tc_temp += poly_voltage * 0.0000002670777;        // a5
   return tc_temp;
   }


/*** read_LTC2480() ************************************************************
This is the funciton that actually does all the work of talking to the LTC2480.
the spi_read() function performs an 8 bit bidirectional transfer on the SPI bus.
Data changes state on falling clock edges and is valid on rising edges, as
determined by the setup_spi() line in the initialize() function.

A good starting point when porting to other processors is to write your own
spi_write function. Note that each processor has its own way of configuring
the SPI port, and different compilers may or may not have built-in functions
for the SPI port. Also, since the state of the LTC2480's SDO line indicates
when a conversion is complete you need to be able to read the state of this line
through the processor's serial data input. Most processors will let you read
this pin as if it were a general purpose I/O line, but there may be some that
don't.

When in doubt, you can always write a "bit bang" function for troubleshooting
purposes.

The "fourbytes" structure allows byte access to the 32 bit return value:

struct fourbytes  // Define structure of four consecutive bytes
   {              // To allow byte access to a 32 bit int or float.
   int8 te0;      //
   int8 te1;      // The make32() function in this compiler will
   int8 te2;      // also work, but a union of 4 bytes and a 32 bit int
   int8 te3;      // is probably more portable.
   };

Also note that the lower 4 bits are the configuration word from the previous
conversion. The 4 LSBs are cleared so that
they don't affect any subsequent mathematical operations. While you can do a
right shift by 4, there is no point if you are going to convert to floating point
numbers - just adjust your scaling constants appropriately.
*******************************************************************************/
int32_t read_LTC2492(char channel, char config)
   {
   union                         // adc_code.bits32    all 32 bits
      {                          // adc_code.by.te0    byte 0
      int32_t bits32;       // adc_code.by.te1    byte 1
      struct fourbytes by;       // adc_code.by.te2    byte 2
      } adc_code;                // adc_code.by.te3    byte 3

   output_low(CS);               // Enable LTC2480 SPI interface
   delay(150);
   while(digitalRead(MISO)) {}       // Wait for end of conversion. The longest
                                 // you will ever wait is one whole conversion period

   adc_code.by.te3 = spi_read(channel);                   // Set to zero.
   adc_code.by.te2 = spi_read(config);    // Read first byte, send config byte
   adc_code.by.te1 = spi_read(0);         // Read 2nd byte, send speed bit
   adc_code.by.te0 = spi_read(0);         // Read 3rd byte. '0' argument is necessary
                                          // to act as SPI master!! (compiler
                                          // and processor specific.)
   output_high(CS);                       // Disable LTC2480 SPI interface
   adc_code.bits32 -= 536870912;
   return adc_code.bits32;
   } // End of read_LTC2480()





/*** calibrate() ***************************************************************
Calibration routine. During calibration, apply 500mV to inputs and adjust local
temperature display to match a known good temperature reading. Pull jumper when
finished.
*******************************************************************************/
void calibrate(void)
   {
//   #ifdef HARDCODED_CONSTANTS



//   #else

   read_calibration();
   cj_temp = 300.15;           // Initial calibration guess
//   lcd_putc('\f');
   lcd.setCursor(1,1);
//   printf(lcd_putc, "vin_c %02X %02X %02X %02X  ", vin_cal.by.te3, vin_cal.by.te2, vin_cal.by.te1, vin_cal.by.te0);
   Serial.print("vin_c dummy");
   lcd.setCursor(1,2);
//   printf(lcd_putc, "ptat_c %02X %02X %02X %02X  ", ptat_cal.by.te3, ptat_cal.by.te2, ptat_cal.by.te1, ptat_cal.by.te0);
   Serial.print("ptat_c dummy");

   while(digitalRead(INC_BUTTON));  // Wait for INC button to be pressed.

   while(!digitalRead(CAL_JUMPER))  // Check state of calibration jumper
      {

// Read input voltage, program to read PTAT
      x = read_LTC2492(NO_CH, PTAT | R55);
      vin_cal.fp = (float) x; // Calculate Counts / 500mV

// Read PTAT voltage, program to read input
      x = read_LTC2492(CH0_1, VIN | R55 | SLOW);

// Calculate Counts / Kelvin based on PTAT voltage and
// local temperature entered from pushbuttons
      ptat_cal.fp = (float) x / cj_temp;

// Display calculated reference voltage (nominally 1.22V). The 1048576 factor
// is results from the 500mV assumed calibration constant and the ADC full-scale
// output of 2^20.
      lcd.setCursor(1,1);
//      printf(lcd_putc, "VREF:%1.4f", ((0.8 * 1.25 * 268435456.0) / vin_cal.fp));

// Display calculated PTAT slope (nominally 1.4mV/K) and local temp in Celcius.
      lcd.setCursor(1,2);
//      printf(lcd_putc,"P:%1.3f CJ:%1.1f",
//                           (ptat_cal.fp * 500 / vin_cal.fp), (cj_temp-273.15));
      //lcd.setCursor(1,3);
      delay(50);

// Adjust local temperature. 50ms delay above functions as a crude
// De-bounce and auto-repeat.
      if(!digitalRead(INC_BUTTON)) cj_temp += 0.1; // Increment local temperature
      if(!digitalRead(DEC_BUTTON)) cj_temp -= 0.1; // Decrement local temperature
      }

// If you are here, jumper was pulled. Write constants to nonvolatile memory.
// The EEPROM.write function writes to the controller's onboard memory. This
// is compiler and processor specific.
   EEPROM.write(0, ptat_cal.by.te0);
   EEPROM.write(1, ptat_cal.by.te1);
   EEPROM.write(2, ptat_cal.by.te2);
   EEPROM.write(3, ptat_cal.by.te3);
   EEPROM.write(4, vin_cal.by.te0);
   EEPROM.write(5, vin_cal.by.te1);
   EEPROM.write(6, vin_cal.by.te2);
   EEPROM.write(7, vin_cal.by.te3);
// Return to main program loop.
//   #endif
   } // End of calibrate()

void read_calibration(void)
   {
#ifdef HARDCODED_CONSTANTS

   ptat_cal.by.te0 = 0x8E;
   ptat_cal.by.te1 = 0x1C;
   ptat_cal.by.te2 = 0xA9;
   ptat_cal.by.te3 = 0xB0;
   vin_cal.by.te0 = 0x9A;
   vin_cal.by.te1 = 0x4C;
   vin_cal.by.te2 = 0xCC;
   vin_cal.by.te3 = 0x1B;

#else

   ptat_cal.by.te0 = EEPROM.read(0);
   ptat_cal.by.te1 = EEPROM.read(1);
   ptat_cal.by.te2 = EEPROM.read(2);
   ptat_cal.by.te3 = EEPROM.read(3);
   vin_cal.by.te0 = EEPROM.read(4);
   vin_cal.by.te1 = EEPROM.read(5);
   vin_cal.by.te2 = EEPROM.read(6);
   vin_cal.by.te3 = EEPROM.read(7);

#endif

   }
