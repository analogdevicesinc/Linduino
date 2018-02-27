/*!
LTC2662: 5-Channel 16-/12-Bit 300mA SoftSpan Current Source DACs

@verbatim

The LTC2662 is a family of 5-channel, 16-/12-bit current source digital-to-analog converters with precision
integrated reference and accurate full-scale current. The LTC2662 is guaranteed monotonic and offers independent
SoftSpan programmability of the current output range from 3.125mA to 300mA.

The LTC2662 provides a separate VDD0-4 supply for each OUT0-4 channel. The flexible VDD0-4 supply allows
operation from 2.85V to 33V. Internal 12 ohm switches allow any OUT0-4 pin to be connected to an optional
negative V- supply voltage and sink up to 80mA.

The SPI/Microwire-compatible 3-wire serial interface operates on logic levels as low as 1.71V, at clock
rates up to 50MHz.


 SPI DATA FORMAT (MSB First):

 24-Bit Load Sequence:

              Byte #1                       Byte #2                       Byte #3
              Command                   MSB                             LSB
 LTC2662-16 : C3 C2 C1 C0 A3 A2 A1 A0   D15 D14 D13 D12 D11 D10 D9 D8   D7 D6 D5 D4 D3 D2 D1 D0
 LTC2662-12 : C3 C2 C1 C0 A3 A2 A1 A0   D11 D10 D9  D8  D7  D6  D5 D4   D3 D2 D1 D0 X  X  X  X

 32-Bit Load Sequence:
              Byte #1                       Byte #2                       Byte #3                 Byte #4
              Command                   MSB                             LSB
 LTC2662-16 : X  X  X  X  X  X  X  X    C3 C2 C1 C0 A3 A2 A1 A0   D15 D14 D13 D12 D11 D10 D9 D8   D7 D6 D5 D4 D3 D2 D1 D0
 LTC2662-12 : X  X  X  X  X  X  X  X    C3 C2 C1 C0 A3 A2 A1 A0   D11 D10 D9  D8  D7  D6  D5 D4   D3 D2 D1 D0 X  X  X  X

 Cx   : DAC Command Code
 Ax   : DAC Address (0 to F, corresponding to DAC 0 to DAC 15)
 Dx   : DAC Data Bits
 X    : Don't care

@endverbatim

http://www.linear.com/product/LTC2662

http://www.linear.com/product/LTC2662#demoboards


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
    @ingroup LTC2662
    Header for LTC2662: 5-Channel 16-/12-Bit 300mA SoftSpan Current Source DACs
*/

#ifndef LTC2662_H
#define LTC2662_H

//! Define the SPI CS pin
#ifndef LTC2662_CS
#define LTC2662_CS QUIKEVAL_CS //! SPI Chip Select Pin
#endif

//! @name LTC2662 Command Codes
//! OR'd together with the DAC address to form the command byte
#define  LTC2662_CMD_WRITE_N              0x00  //!< Write to input register n
#define  LTC2662_CMD_UPDATE_N             0x10  //!< Update (power up) DAC register n
#define  LTC2662_CMD_WRITE_N_UPDATE_ALL   0x20  //!< Write to input register n, update (power-up) all
#define  LTC2662_CMD_WRITE_N_UPDATE_N     0x30  //!< Write to input register n, update (power-up) 
#define  LTC2662_CMD_POWER_DOWN_N         0x40  //!< Power down n
#define  LTC2662_CMD_POWER_DOWN_ALL       0x50  //!< Power down chip (all DAC's, MUX and reference)

#define  LTC2662_CMD_SPAN                 0x60  //!< Write span to dac n
#define  LTC2662_CMD_CONFIG               0x70  //!< Configure reference / toggle
#define  LTC2662_CMD_WRITE_ALL            0x80  //!< Write to all input registers
#define  LTC2662_CMD_UPDATE_ALL           0x90  //!< Update all DACs
#define  LTC2662_CMD_WRITE_ALL_UPDATE_ALL 0xA0  //!< Write to all input reg, update all DACs
#define  LTC2662_CMD_MUX                  0xB0  //!< Select MUX channel (controlled by 5 LSbs in data word)
#define  LTC2662_CMD_TOGGLE_SEL           0xC0  //!< Select which DACs can be toggled (via toggle pin or global toggle bit)
#define  LTC2662_CMD_GLOBAL_TOGGLE        0xD0  //!< Software toggle control via global toggle bit
#define  LTC2662_CMD_SPAN_ALL             0xE0  //!< Set span for all DACs
#define  LTC2662_CMD_NO_OPERATION         0xF0  //!< No operation
//! @}

//! @name LTC2662 Span Codes
//! @{
//! Descriptions are valid for a 1.25V reference and 20k R_FSADJ.
#define  LTC2662_SPAN_HI_Z                0x0000
#define  LTC2662_SPAN_3_125MA             0x0001
#define  LTC2662_SPAN_6_25MA              0x0002
#define  LTC2662_SPAN_12_5MA              0x0003
#define  LTC2662_SPAN_25MA                0x0004
#define  LTC2662_SPAN_50MA                0x0005
#define  LTC2662_SPAN_100MA               0x0006
#define  LTC2662_SPAN_200MA               0x0007
#define  LTC2662_SPAN_SW_TO_NEG_V         0x0008
#define  LTC2662_SPAN_300MA               0x0009
//! @}

//! @name LTC2662 Minimums and Maximums for each Span
//! @{
//! Lookup tables for minimum and maximum outputs for a given span
const float LTC2662_MIN_OUTPUT[10] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
const float LTC2662_MAX_OUTPUT[10] = {0.0, 3.125, 6.25, 12.5, 25.0, 50.0, 100.0, 200.0, 0, 300.0};
//! @}

//! @name LTC2662 Configuration options
//! @{
//! Used in conjunction with LTC2662_CMD_CONFIG command
#define  LTC2662_REF_DISABLE                 0x0001  //! Disable internal reference to save power when using an ext. ref.
#define  LTC2662_THERMAL_SHUTDOWN            0x0002  //! Disable thermal shutdown (NOT RECOMMENDED)
#define  LTC2662_POWERLIMIT_PROTECTION       0x0003  //! Disable power limit protection (NOT RECOMMENDED)
#define  LTC2662_OPEN_CIRCUIT_DETECT_DISABLE 0x0004  //! Disable open circuit detection
//! @}

//! @name LTC2662 Global Toggle
//! @{
//! Used in conjunction with LTC2662_CMD_GLOBAL_TOGGLE command, affects DACs whose
//! Toggle Select bits have been set to 1
#define  LTC2662_TOGGLE_REG_A              0x0000  //! Update DAC with register A
#define  LTC2662_TOGGLE_REG_B              0x0010  //! Update DAC with register B
//! @}

//! Write the 16-bit dac_code to the LTC2662
//! @return Zero if readback (SDO) data matches the value previously sent, One if mismatched (indicating data error somewhere)
int8_t LTC2662_write(uint8_t cs,            //!< Chip select
                     uint8_t dac_command,   //!< DAC command word
                     uint8_t dac_address,   //!< DAC channel address
                     uint16_t dac_code      //!< 2 byte dac code
                    );

//! Calculate a LTC2662 DAC code given the desired output current
//! @return The 16 bit code to send to the DAC
uint16_t LTC2662_current_to_code(float dac_current, //!< Desired current
                                 float max_output   //!< Full scale current
                                );

//! Calculate the LTC2662 DAC output current given the DAC code and max output.
//! @return the output voltage
float LTC2662_code_to_current(uint16_t dac_code,    //!< DAC code value
                              float max_output      //!< Full scale current
                             );

#endif  // LTC2662_H
