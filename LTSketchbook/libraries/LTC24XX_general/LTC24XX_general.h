/*!
LTC24XX General Library: Functions and defines for all SINC4 Delta Sigma ADCs.

@verbatim


LTC2442 / LTC2444 / LTC2445 / LTC2448 / LTC2449 (Are there don't care bits in the low channel counts?
SPI DATA FORMAT (MSB First):

            Byte #1                            Byte #2

Data Out :  !EOC DMY SIG D28 D27 D26 D25 D24   D23  D22  D21  D20  D19 D18 D17 D16
Data In  :  1    0   EN  SGL OS  S2  S1  S0    OSR3 OSR2 OSR1 OSR1 SPD X   X   X

            Byte #3                            Byte #4
            D15  D14 D13 D12 D11 D10 D9  D8    D7  D6  D5  D4 *D3  *D2 *D1 *D0
            X    X   X   X   X   X   X   X     X   X   X   X   X   X   X   X

!EOC : End of Conversion Bit (Active Low)
DMY  : Dummy Bit (Always 0)
SIG  : Sign Bit (1-data positive, 0-data negative)
Dx   : Data Bits
*Dx  : Data Bits Below lsb
EN   : Enable Bit (0-keep previous mode, 1-change mode)
SGL  : Enable Single-Ended Bit (0-differential, 1-single-ended)
OS   : ODD/Sign Bit
Sx   : Address Select Bit
0SRX : Over Sampling Rate Bits
SPD  : Double Output Rate Select Bit (0-Normal rate, auto-calibration on, 2x rate, auto_calibration off)

Command Byte #1
1    0    EN   SGL  OS   S2   S1   S0   Comments
1    0    0    X    X    X    X    X    Keep Previous Mode
1    0    1    0    X    X    X    X    Differential Mode
1    0    1    1    X    X    X    X    Single-Ended Mode

|  Coversion Rate    |  RMS  | ENOB | OSR  | Latency
Command Byte #2          |Internal | External | Noise |      |      |
|  9MHz   | 10.24MHz |       |      |      |
OSR3 OSR2 OSR1 OSR1 SPD  | Clock   |  Clock   |       |      |      |
0    0    0    0    0      Keep Previous Speed/Resolution
0    0    0    1    0      3.52kHz   4kHz       23uV    17     64     none
0    0    1    0    0      1.76kHz   2kHz       3.5uV   20.1   128    none
0    0    1    1    0      880Hz     1kHz       2uV     21.3   256    none
0    1    0    0    0      440Hz     500Hz      1.4uV   21.8   512    none
0    1    0    1    0      220Hz     250Hz      1uV     22.4   1024   none
0    1    1    0    0      110Hz     125Hz      750nV   22.9   2048   none
0    1    1    1    0      55Hz      62.5Hz     510nV   23.4   4096   none
1    0    0    0    0      27.5Hz    31.25Hz    375nV   24     8192   none
1    0    0    1    0      13.75Hz   15.625Hz   250nV   24.4   16384  none
1    1    1    1    0      6.87kHz   7.8125Hz   200nV   24.6   32768  none
0    0    0    0    1      Keep Previous Speed/Resolution
OSR3 OSR2 OSR1 OSR1 1      2X Mode  *all clock speeds double

Example Code:

Read Channel 0 in Single-Ended with OSR of 65536

    uint16_t miso_timeout = 1000;
    adc_command = LTC2449_CH0 | LTC2449_OSR_32768 | LTC2449_SPEED_2X;   // Build ADC command for channel 0
                                                                        // OSR = 32768*2 = 65536

    if(LTC2449_EOC_timeout(LTC2449_CS, miso_timeout))    // Check for EOC
        return;                                          // Exit if timeout is reached
    LTC2449_read(LTC2449_CS, adc_command, &adc_code);    // Throws out last reading

    if(LTC2449_EOC_timeout(LTC2449_CS, miso_timeout))    // Check for EOC
        return;                                          // Exit if timeout is reached
    LTC2449_read(LTC2449_CS, adc_command, &adc_code);    // Obtains the current reading and stores to adc_code variable

    // Convert adc_code to voltage
    adc_voltage = LTC2449_code_to_voltage(adc_code, LTC2449_lsb, LTC2449_offset_code);

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

/*! @file
    @ingroup LTC24XX_general
    Header for LTC2449: 24-Bit, 16-Channel Delta Sigma ADCs with Selectable Speed/Resolution
*/

#ifndef LTC24XX_general_H
#define LTC24XX_general_H

//! Define the SPI CS pin
#ifndef LTC24XX_CS
#define LTC24XX_CS QUIKEVAL_CS
#endif

//! In 2X Mode, A non offset binary 0 can be produced. This is corrected in the
//! differential code to voltage functions. To disable this correction, uncomment
//! The following #define.
//#define SKIP_EZDRIVE_2X_ZERO_CHECK

/*! @name Mode Configuration for High Speed Family
 @{
*/
#define LTC24XX_HS_MULTI_KEEP_PREVIOUS_MODE              0x80
#define LTC24XX_HS_MULTI_KEEP_PREVIOUS_SPEED_RESOLUTION  0x00
#define LTC24XX_HS_MULTI_SPEED_1X                        0x00
#define LTC24XX_HS_MULTI_SPEED_2X                        0x08
/*!
 @}
*/

/*! @name Mode Configuration for EasyDrive Family
 @{
*/
// Select ADC source - differential input or PTAT circuit
#define LTC24XX_EZ_MULTI_VIN    0b10000000
#define LTC24XX_EZ_MULTI_PTAT   0b11000000

// Select rejection frequency - 50, 55, or 60Hz
#define LTC24XX_EZ_MULTI_R50    0b10010000
#define LTC24XX_EZ_MULTI_R55    0b10000000
#define LTC24XX_EZ_MULTI_R60    0b10100000

// Speed settings is bit 7 in the 2nd byte
#define LTC24XX_EZ_MULTI_SLOW   0b10000000 // slow output rate with autozero
#define LTC24XX_EZ_MULTI_FAST   0b10001000 // fast output rate with no autozero
/*!
 @}
*/


/*! @name Single-Ended Channel Configuration
@verbatim
Channel selection for all multi-channel, differential input ADCs, even those that only require
8 bits of configuration (no further options.) Most devices in this category require a second
byte of configuration for speed mode, temperature sensor selection, etc., but for the sake
of simplicity a single function will be used to read all devices, sending zeros in the second
configuration byte if only the channel is specified.

Applicable devices:
Easy Drive:
LTC2486, LTC2487, LTC2488, LTC2489, LTC2492, LTC2493,
LTC2494, LTC2495, LTC2496, LTC2497, LTC2498, LTC2499
First Generation Differential:
LTC2414, LTC2418, LTC2439
High Speed:
LTC2442, LTC2444, LTC2445, LTC2448, LTC2449
@endverbatim
@{ */
#define LTC24XX_MULTI_CH_CH0            0xB0
#define LTC24XX_MULTI_CH_CH1            0xB8
#define LTC24XX_MULTI_CH_CH2            0xB1
#define LTC24XX_MULTI_CH_CH3            0xB9
#define LTC24XX_MULTI_CH_CH4            0xB2
#define LTC24XX_MULTI_CH_CH5            0xBA
#define LTC24XX_MULTI_CH_CH6            0xB3
#define LTC24XX_MULTI_CH_CH7            0xBB
#define LTC24XX_MULTI_CH_CH8            0xB4
#define LTC24XX_MULTI_CH_CH9            0xBC
#define LTC24XX_MULTI_CH_CH10           0xB5
#define LTC24XX_MULTI_CH_CH11           0xBD
#define LTC24XX_MULTI_CH_CH12           0xB6
#define LTC24XX_MULTI_CH_CH13           0xBE
#define LTC24XX_MULTI_CH_CH14           0xB7
#define LTC24XX_MULTI_CH_CH15           0xBF
/*! @} */

/*! @name Differential Channel Configuration
@verbatim
See note for single-ended configuration above.

@endverbatim
@{ */
#define LTC24XX_MULTI_CH_P0_N1          0xA0
#define LTC24XX_MULTI_CH_P1_N0          0xA8

#define LTC24XX_MULTI_CH_P2_N3          0xA1
#define LTC24XX_MULTI_CH_P3_N2          0xA9

#define LTC24XX_MULTI_CH_P4_N5          0xA2
#define LTC24XX_MULTI_CH_P5_N4          0xAA

#define LTC24XX_MULTI_CH_P6_N7          0xA3
#define LTC24XX_MULTI_CH_P7_N6          0xAB

#define LTC24XX_MULTI_CH_P8_N9          0xA4
#define LTC24XX_MULTI_CH_P9_N8          0xAC

#define LTC24XX_MULTI_CH_P10_N11        0xA5
#define LTC24XX_MULTI_CH_P11_N10        0xAD

#define LTC24XX_MULTI_CH_P12_N13        0xA6
#define LTC24XX_MULTI_CH_P13_N12        0xAE

#define LTC24XX_MULTI_CH_P14_N15        0xA7
#define LTC24XX_MULTI_CH_P15_N14        0xAF
/*! @} */

/*Commands
Construct a channel / resolution control word by bitwise ORing one choice from the channel configuration
and one choice from the Oversample ratio configuration. You can also enable 2Xmode, which will increase
sample rate by a factor of 2 but introduce one cycle of latency.

Example - read channel 3 single-ended at OSR2048, with 2X mode enabled.
adc_command = (LTC2449_CH3 | LTC2449_OSR_2048) | LTC2449_SPEED_2X;
*/

/*! @name Oversample Ratio (OSR) Commands
@{ */
#define LTC24XX_MULTI_CH_OSR_64         0x10
#define LTC24XX_MULTI_CH_OSR_128        0x20
#define LTC24XX_MULTI_CH_OSR_256        0x30
#define LTC24XX_MULTI_CH_OSR_512        0x40
#define LTC24XX_MULTI_CH_OSR_1024       0x50
#define LTC24XX_MULTI_CH_OSR_2048       0x60
#define LTC24XX_MULTI_CH_OSR_4096       0x70
#define LTC24XX_MULTI_CH_OSR_8192       0x80
#define LTC24XX_MULTI_CH_OSR_16384      0x90
#define LTC24XX_MULTI_CH_OSR_32768      0xF0
/*! @}*/

//! Checks for EOC with a specified timeout. Applies to all SPI interface delta sigma
//! ADCs that have SINC4 rejection, does NOT apply to LTC2450/60/70 family.
//! @return Returns 0=successful, 1=unsuccessful (exceeded timeout)
int8_t LTC24XX_EOC_timeout(uint8_t cs,           //!< Chip Select pin
                           uint16_t miso_timeout  //!< Timeout (in milliseconds)
                          );


// Read functions for SPI interface ADCs with a 32 bit output word. These functions are used with both
// Single-ended and differential parts, as there is no interpretation of the data done in
// the function. Also note that these functions can be used for devices that have shorter output lengths,
// the lower bits will read out as "1", as the conversion will be triggered by the last data bit being
// read, which causes SDO to go high.


//! Reads from LTC24XX ADC that has no configuration word and returns a 32 bit result.
//! @return  void
void LTC24XX_SPI_32bit_data(uint8_t cs,         //!< Chip Select pin
                            int32_t *adc_code   //!< 4 byte conversion code read from LTC24XX
                           );

//! Reads from LTC24XX ADC that accepts an 8 bit configuration and returns a 32 bit result.
//! @return  void
void LTC24XX_SPI_8bit_command_32bit_data(uint8_t cs,               //!< Chip Select pin
    uint8_t adc_command,      //!< 1 byte command written to LTC24XX
    int32_t *adc_code         //!< 4 byte conversion code read from LTC24XX
                                        );

//! Reads from LTC24XX ADC that accepts a 16 bit configuration and returns a 32 bit result.
//! @return  void
void LTC24XX_SPI_16bit_command_32bit_data(uint8_t cs,                  //!< Chip Select pin
    uint8_t adc_command_high,    //!< First command byte written to LTC24XX
    uint8_t adc_command_low,     //!< Second command written to LTC24XX
    int32_t *adc_code            //!< 4 byte conversion code read from LTC24XX
                                         );

//! Reads from LTC24XX two channel "Ping-Pong" ADC, placing the channel information in the adc_channel parameter
//! and returning the 32 bit result with the channel bit cleared so the data format matches the rest of the family
//! @return void
void LTC24XX_SPI_2ch_ping_pong_32bit_data(uint8_t cs,           //!< Chip Select pin
    uint8_t *adc_channel, //!< Returns channel number read.
    int32_t *code         //!< 4 byte conversion code read from LTC24XX
                                         );


// Read functions for SPI interface ADCs with a 24 bit or 19 bit output word. These functions
// are used with both Single-ended and differential parts, as there is no interpretation of
// the data done in the function. 24 bits will be read out of 19 bit devices
// (LTC2433, LTC2436, LTC2439), with the additional 5 bits being set to 1.

//! Reads from LTC24XX ADC that has no configuration word and returns a 32 bit result.
//! @return  void
void LTC24XX_SPI_24bit_data(uint8_t cs,         //!< Chip Select pin
                            int32_t *adc_code   //!< 4 byte conversion code read from LTC24XX
                           );

//! Reads from LTC24XX ADC that accepts an 8 bit configuration and returns a 32 bit result.
//! @return  void
void LTC24XX_SPI_8bit_command_24bit_data(uint8_t cs,            //!< Chip Select pin
    uint8_t adc_command,   //!< 1 byte command written to LTC24XX
    int32_t *adc_code      //!< 4 byte conversion code read from LTC24XX
                                        );

//! Reads from LTC24XX ADC that accepts a 16 bit configuration and returns a 32 bit result.
//! @return  void
void LTC24XX_SPI_16bit_command_24bit_data(uint8_t cs,                  //!< Chip Select pin
    uint8_t adc_command_high,    //!< First command byte written to LTC24XX
    uint8_t adc_command_low,     //!< Second command written to LTC24XX
    int32_t *adc_code            //!< 4 byte conversion code read from LTC24XX
                                         );

//! Reads from LTC24XX ADC that accepts a 8 bit configuration and returns a 16 bit result.
//! @return  void
void LTC24XX_SPI_8bit_command_16bit_data(uint8_t cs,                //!< Chip Select pin
    uint8_t adc_command,      //!< First command byte written to LTC24XX
    int32_t *adc_code         //!< 4 byte conversion code read from LTC24XX
                                        );


//! Reads from LTC24XX two channel "Ping-Pong" ADC, placing the channel information in the adc_channel parameter
//! and returning the 32 bit result with the channel bit cleared so the data format matches the rest of the family
//! @return void
void LTC24XX_SPI_2ch_ping_pong_24bit_data(uint8_t cs,           //!< Chip Select pin
    uint8_t *adc_channel, //!< Returns channel number read.
    int32_t *code         //!< 4 byte conversion code read from LTC24XX
                                         );

// Read functions for I2C interface ADCs with a 32 bit output word. These functions are used with both
// Single-ended and differential parts, as there is no interpretation of the data done in
// the function. Also note that these functions can be used for devices that have shorter output lengths,
// the lower bits will read out as "1", as the conversion will be triggered by the last data bit being
// read, which causes SDO to go high.
// Data is formatted to match the SPI devices, with the MSB in the bit 28 position.
// Unlike the SPI members of this family, checking for EOC MUST immediately be followed by reading the data. This
// is because a stop condition will trigger a new conversion.


//! Reads from LTC24XX ADC that has no configuration word and returns a 32 bit result.
//! @return  Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC24XX_I2C_32bit_data(uint8_t i2c_address,  //!< I2C address of device
                              int32_t *adc_code,      //!< 4 byte conversion code read from LTC24XX
                              uint16_t eoc_timeout    //!< Timeout (in milliseconds)
                             );


//! Reads from LTC24XX ADC that accepts an 8 bit configuration and returns a 32 bit result.
//! @return  Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC24XX_I2C_8bit_command_32bit_data(uint8_t i2c_address,     //!< I2C address of device
    uint8_t adc_command,     //!< 1 byte command written to LTC24XX
    int32_t *adc_code,       //!< 4 byte conversion code read from LTC24XX
    uint16_t eoc_timeout     //!< Timeout (in milliseconds)
                                          );


//! Reads from LTC24XX ADC that accepts a 16 bit configuration and returns a 32 bit result.
//! @return  Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC24XX_I2C_16bit_command_32bit_data(uint8_t i2c_address,        //!< I2C address of device
    uint8_t adc_command_high,   //!< First command byte written to LTC24XX
    uint8_t adc_command_low,    //!< Second command written to LTC24XX
    int32_t *adc_code,          //!< 4 byte conversion code read from LTC24XX
    uint16_t eoc_timeout        //!< Timeout (in milliseconds)
                                           );


// Read functions for I2C interface ADCs with a 24 bit or 19 bit output word. These functions
// are used with both Single-ended and differential parts, as there is no interpretation of
// the data done in the function. 24 bits will be read out of 19 bit devices
// (LTC2433, LTC2436, LTC2439), with the additional 5 bits being set to 1.


//! Reads from LTC24XX ADC that has no configuration word and returns a 32 bit result.
//! Applies to: LTC2483 (only this lonely one!)
//! @return  Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC24XX_I2C_24bit_data(uint8_t i2c_address,      //!< I2C address of device
                              int32_t *adc_code,        //!< 4 byte conversion code read from LTC24XX
                              uint16_t eoc_timeout      //!< Timeout (in milliseconds)
                             );


//! Reads from LTC24XX ADC that accepts an 8 bit configuration and returns a 32 bit result.
//! @return  Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC24XX_I2C_8bit_command_24bit_data(uint8_t i2c_address,      //!< I2C address of device
    uint8_t adc_command,      //!< 1 byte command written to LTC24XX
    int32_t *adc_code,        //!< 4 byte conversion code read from LTC24XX
    uint16_t eoc_timeout      //!< Timeout (in milliseconds)
                                          );

//! Reads from LTC24XX ADC that accepts a 16 bit configuration and returns a 32 bit result.
//! @return  Returns the state of the acknowledge bit after the I2C address write. 0=acknowledge, 1=no acknowledge.
int8_t LTC24XX_I2C_16bit_command_24bit_data(uint8_t i2c_address,         //!< I2C address of device
    uint8_t adc_command_high,    //!< First command byte written to LTC24XX
    uint8_t adc_command_low,     //!< Second command written to LTC24XX
    int32_t *adc_code,           //!< 4 byte conversion code read from LTC24XX
    uint16_t eoc_timeout         //!< Timeout (in milliseconds)
                                           );

//! Calculates the voltage corresponding to an ADC code, given the reference voltage.
//! Applies to Single-Ended input parts (LTC2400-type input)
//! @return Returns voltage calculated from ADC code.
float LTC24XX_SE_code_to_voltage(int32_t adc_code,  //!< Code read from ADC
                                 float vref         //!< Reference voltage
                                );
//! Calculates the voltage corresponding to an ADC code, given the reference voltage.
//! Applies to differential input parts (LTC2410 type input)
//! @return Returns voltage calculated from ADC code.
float LTC24XX_diff_code_to_voltage(int32_t adc_code,             //!< Code read from ADC
                                   float vref                    //!< Reference voltage
                                  );

//! Calculates the voltage corresponding to an ADC code, given lsb weight (in volts) and the calibrated
//! ADC offset code (zero code that is subtracted from adc_code).
//! Applies to differential input, SPI interface parts.
//! @return Returns voltage calculated from ADC code.
float LTC24XX_diff_code_to_calibrated_voltage(int32_t adc_code,               //!< Code read from ADC
    float LTC24XX_lsb,              //!< LSB weight (in volts)
    int32_t LTC24XX_offset_code     //!< The calibrated offset code (This is the ADC code zero code that will be subtracted from adc_code)
                                             );

//! Calculate the lsb weight and offset code given a full-scale code and a measured zero-code.
//! @return Void
void LTC24XX_calibrate_voltage(int32_t zero_code,             //!< Measured code with the inputs shorted to ground
                               int32_t fs_code,               //!< Measured code at nearly full-scale
                               float zero_voltage,            //!< Measured zero voltage
                               float fs_voltage,              //!< Voltage measured at input (with voltmeter) when fs_code was read from ADC
                               float *LTC24XX_lsb,            //!< Overwritten with lsb weight (in volts)
                               int32_t *LTC24XX_offset_code   //!< Overwritten with offset code (zero code)
                              );



// I2C Addresses for 8/16 channel parts (LTC2495/7/9)
//                                   ADDRESS     CA2   CA1    CA0
// #define LTC24XX_16CH_I2C_ADDRESS 0b0010100 // LOW   LOW    LOW
// #define LTC24XX_16CH_I2C_ADDRESS 0b0010110 // LOW   LOW    HIGH
// #define LTC24XX_16CH_I2C_ADDRESS 0b0010101 // LOW   LOW    FLOAT
// #define LTC24XX_16CH_I2C_ADDRESS 0b0100110 // LOW   HIGH   LOW
// #define LTC24XX_16CH_I2C_ADDRESS 0b0110100 // LOW   HIGH   HIGH
// #define LTC24XX_16CH_I2C_ADDRESS 0b0100111 // LOW   HIGH   FLOAT
// #define LTC24XX_16CH_I2C_ADDRESS 0b0010111 // LOW   FLOAT  LOW
// #define LTC24XX_16CH_I2C_ADDRESS 0b0100101 // LOW   FLOAT  HIGH
// #define LTC24XX_16CH_I2C_ADDRESS 0b0100100 // LOW   FLOAT  FLOAT
// #define LTC24XX_16CH_I2C_ADDRESS 0b1010110 // HIGH  LOW    LOW
// #define LTC24XX_16CH_I2C_ADDRESS 0b1100100 // HIGH  LOW    HIGH
// #define LTC24XX_16CH_I2C_ADDRESS 0b1010111 // HIGH  LOW    FLOAT
// #define LTC24XX_16CH_I2C_ADDRESS 0b1110100 // HIGH  HIGH   LOW
// #define LTC24XX_16CH_I2C_ADDRESS 0b1110110 // HIGH  HIGH   HIGH
// #define LTC24XX_16CH_I2C_ADDRESS 0b1110101 // HIGH  HIGH   FLOAT
// #define LTC24XX_16CH_I2C_ADDRESS 0b1100101 // HIGH  FLOAT  LOW
// #define LTC24XX_16CH_I2C_ADDRESS 0b1100111 // HIGH  FLOAT  HIGH
// #define LTC24XX_16CH_I2C_ADDRESS 0b1100110 // HIGH  FLOAT  FLOAT
// #define LTC24XX_16CH_I2C_ADDRESS 0b0110101 // FLOAT LOW   LOW
// #define LTC24XX_16CH_I2C_ADDRESS 0b0110111 // FLOAT LOW   HIGH
// #define LTC24XX_16CH_I2C_ADDRESS 0b0110110 // FLOAT LOW   FLOAT
// #define LTC24XX_16CH_I2C_ADDRESS 0b1000111 // FLOAT HIGH  LOW
// #define LTC24XX_16CH_I2C_ADDRESS 0b1010101 // FLOAT HIGH  HIGH
// #define LTC24XX_16CH_I2C_ADDRESS 0b1010100 // FLOAT HIGH  FLOAT
// #define LTC24XX_16CH_I2C_ADDRESS 0b1000100 // FLOAT FLOAT LOW
// #define LTC24XX_16CH_I2C_ADDRESS 0b1000110 // FLOAT FLOAT HIGH
// #define LTC24XX_16CH_I2C_ADDRESS 0b1000101 // FLOAT FLOAT FLOAT

// I2C Addresses for 2/4 channel parts
//                                  ADDRESS      CA1   CA0
// #define LTC24XX_4CH_I2C_ADDRESS 0b0010100  // LOW   LOW
// #define LTC24XX_4CH_I2C_ADDRESS 0b0010110  // LOW   HIGH
// #define LTC24XX_4CH_I2C_ADDRESS 0b0010101  // LOW   FLOAT
// #define LTC24XX_4CH_I2C_ADDRESS 0b0100110  // HIGH  LOW
// #define LTC24XX_4CH_I2C_ADDRESS 0b0110100  // HIGH  HIGH
// #define LTC24XX_4CH_I2C_ADDRESS 0b0100111  // HIGH  FLOAT
// #define LTC24XX_4CH_I2C_ADDRESS 0b0010111  // FLOAT LOW
// #define LTC24XX_4CH_I2C_ADDRESS 0b0100101  // FLOAT HIGH
// #define LTC24XX_4CH_I2C_ADDRESS 0b0100100  // FLOAT FLOAT


// I2C Addresses for Single channel parts (LTC2481/83/85)
//                                  ADDRESS      CA1   CA0/f0*
// #define LTC24XX_1CH_I2C_ADDRESS 0b0010100  // LOW   HIGH
// #define LTC24XX_1CH_I2C_ADDRESS 0b0010101  // LOW   FLOAT
// #define LTC24XX_1CH_I2C_ADDRESS 0b0010111  // FLOAT HIGH
// #define LTC24XX_1CH_I2C_ADDRESS 0b0100100  // FLOAT FLOAT
// #define LTC24XX_1CH_I2C_ADDRESS 0b0100110  // HIGH  HIGH
// #define LTC24XX_1CH_I2C_ADDRESS 0b0100111  // HIGH  FLOAT


#endif  // LTC24XX_general_H

