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

 SPI DATA FORMAT (MSB First):

 Byte #1                  Byte #2
 C2 C1 C0 A3 A2 A1 A0 X   D7 D6 D5 D4 D3 D2 D1 D0

 Cx   : Command     (0-Read Reg, 1-Write (no update), 2-Update all Reg,
                     3-Write one Reg and Update,      7-Reset)
 Ax   : REG Address (0-Reg0, 1-Reg1, 2-Reg2, ..., E-RegE)
 Dx   : REG Data
 X    : Don't care

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

/*! @file
    @ingroup LTC2874
*/

#ifndef LTC2874_H
#define LTC2874_H

// Define the CS pin
#ifndef LTC2874_CS
#define LTC2874_CS QUIKEVAL_CS
#endif

/*!
OR'd together with the register address to form the command byte
| LTC2874 Command String   | Value |
| :------------------------| :---: |
| LTC2874_READ             | 0x00  |
| LTC2874_WRITE_NO_UPDATE  | 0x20  |
| LTC2874_UPDATE_ALL       | 0x40  |
| LTC2874_WRITE_UPDATE_ALL | 0x60  |
| LTC2874_RESET            | 0xE0  |
*/
/*! @name Command Codes
@{ */

// Command Codes
#define   LTC2874_READ               0x00  //! READ REGISTER
#define   LTC2874_WRITE_NO_UPDATE    0x20  //! WRITE REGISTER (NO UPDATE)
#define   LTC2874_UPDATE_ALL         0x40  //! UPDATE ALL REGISTERS
#define   LTC2874_WRITE_UPDATE_ALL   0x60  //! WRITE ONE REGISTER AND UPDATE
#define   LTC2874_RESET              0xE0  //! RESET
//! @}


/*!
| Register             | Address |
| :--------------------| :-----: |
| LTC2874_IRQREG_REG0  | 0x00    |
| LTC2874_IRQMASK_REG1 | 0x01    |
| LTC2874_EVENT1_REG2  | 0x02    |
| LTC2874_EVENT2_REG3  | 0x03    |
| LTC2874_EVENT3_REG4  | 0x04    |
| LTC2874_EVENT4_REG5  | 0x05    |
| LTC2874_STATUS1_REG6 | 0x06    |
| LTC2874_STATUS2_REG7 | 0x07    |
| LTC2874_MODE1_REG8   | 0x08    |
| LTC2874_MODE2_REG9   | 0x09    |
| LTC2874_NSF_REGA     | 0x0A    |
| LTC2874_ILLM_REGB    | 0x0B    |
| LTC2874_TMRCTRL_REGC | 0x0C    |
| LTC2874_CTRL1_REGD   | 0x0D    |
| LTC2874_CTRL2_REGE   | 0x0E    |
*/
/*! @name Register Addresses
@{ */

// Register Addresses
#define   LTC2874_IRQREG_REG0    0x00
#define   LTC2874_IRQMASK_REG1   0x01
#define   LTC2874_EVENT1_REG2    0x02
#define   LTC2874_EVENT2_REG3    0x03
#define   LTC2874_EVENT3_REG4    0x04
#define   LTC2874_EVENT4_REG5    0x05
#define   LTC2874_STATUS1_REG6   0x06
#define   LTC2874_STATUS2_REG7   0x07
#define   LTC2874_MODE1_REG8     0x08
#define   LTC2874_MODE2_REG9     0x09
#define   LTC2874_NSF_REGA       0x0A
#define   LTC2874_ILLM_REGB      0x0B
#define   LTC2874_TMRCTRL_REGC   0x0C
#define   LTC2874_CTRL1_REGD     0x0D
#define   LTC2874_CTRL2_REGE     0x0E
//! @}

/*! @name Register Bit and Mask Definitions
@{ */

// register bit definitions / masks
#define    LTC2874_OT             (0x1<<7)      // REGISTER IRQREG
#define    LTC2874_SUPPLY         (0x1<<6)      // REGISTER IRQREG
#define    LTC2874_WU             (0x1<<5)      // REGISTER IRQREG
#define    LTC2874_TOC_LP         (0x1<<4)      // REGISTER IRQREG
#define    LTC2874_PWRCHNG        (0x1<<3)      // REGISTER IRQREG
#define    LTC2874_TOC_CQ         (0x1<<2)      // REGISTER IRQREG
#define    LTC2874_CSENSE         (0x1<<1)      // REGISTER IRQREG

#define    LTC2874_OT_MASK        (0x1<<7)      // REGISTER IRQMASK
#define    LTC2874_SUPPLY_MASK    (0x1<<6)      // REGISTER IRQMASK
#define    LTC2874_WU_MASK        (0x1<<5)      // REGISTER IRQMASK
#define    LTC2874_TOC_LP_MASK    (0x1<<4)      // REGISTER IRQMASK
#define    LTC2874_PWRCHNG_MASK   (0x1<<3)      // REGISTER IRQMASK
#define    LTC2874_TOC_CQ_MASK    (0x1<<2)      // REGISTER IRQMASK
#define    LTC2874_CSENSE_MASK    (0x1<<1)      // REGISTER IRQMASK

#define    LTC2874_OT_SD          (0x1<<7)      // REGISTER EVENT1
#define    LTC2874_OT_WARN        (0x1<<6)      // REGISTER EVENT1
#define    LTC2874_UVLO_VL        (0x1<<4)      // REGISTER EVENT1
#define    LTC2874_UVLO_VDD       (0x1<<3)      // REGISTER EVENT1
#define    LTC2874_UV_VDD         (0x1<<2)      // REGISTER EVENT1
#define    LTC2874_OV_VDD         (0x1<<1)      // REGISTER EVENT1

#define    LTC2874_WU4            (0x1<<7)      // REGISTER EVENT2
#define    LTC2874_WU3            (0x1<<6)      // REGISTER EVENT2
#define    LTC2874_WU2            (0x1<<5)      // REGISTER EVENT2
#define    LTC2874_WU1            (0x1<<4)      // REGISTER EVENT2
#define    LTC2874_TOC_LP4        (0x1<<3)      // REGISTER EVENT2
#define    LTC2874_TOC_LP3        (0x1<<2)      // REGISTER EVENT2
#define    LTC2874_TOC_LP2        (0x1<<1)      // REGISTER EVENT2
#define    LTC2874_TOC_LP1         0x1          // REGISTER EVENT2

#define    LTC2874_PWRCHNG4       (0x1<<7)      // REGISTER EVENT3
#define    LTC2874_PWRCHNG3       (0x1<<6)      // REGISTER EVENT3
#define    LTC2874_PWRCHNG2       (0x1<<5)      // REGISTER EVENT3
#define    LTC2874_PWRCHNG1       (0x1<<4)      // REGISTER EVENT3
#define    LTC2874_TOC_CQ4        (0x1<<3)      // REGISTER EVENT3
#define    LTC2874_TOC_CQ3        (0x1<<2)      // REGISTER EVENT3
#define    LTC2874_TOC_CQ2        (0x1<<1)      // REGISTER EVENT3
#define    LTC2874_TOC_CQ1         0x1          // REGISTER EVENT3

#define    LTC2874_CQ_SNS4        (0x1<<7)      // REGISTER EVENT4
#define    LTC2874_CQ_SNS3        (0x1<<6)      // REGISTER EVENT4
#define    LTC2874_CQ_SNS2        (0x1<<5)      // REGISTER EVENT4
#define    LTC2874_CQ_SNS1        (0x1<<4)      // REGISTER EVENT4
#define    LTC2874_CSENSE4        (0x1<<3)      // REGISTER EVENT4
#define    LTC2874_CSENSE3        (0x1<<2)      // REGISTER EVENT4
#define    LTC2874_CSENSE2        (0x1<<1)      // REGISTER EVENT4
#define    LTC2874_CSENSE1         0x1          // REGISTER EVENT4

#define    LTC2874_OT_STAT        (0x1<<7)      // REGISTER STATUS1
#define    LTC2874_WU_COOL_STAT   (0x1<<6)      // REGISTER STATUS1
#define    LTC2874_UVLO_VDD_STAT  (0x1<<5)      // REGISTER STATUS1
#define    LTC2874_OV_VDD_STAT    (0x1<<4)      // REGISTER STATUS1
#define    LTC2874_OC_LP4_STAT    (0x1<<3)      // REGISTER STATUS1
#define    LTC2874_OC_LP3_STAT    (0x1<<2)      // REGISTER STATUS1
#define    LTC2874_OC_LP2_STAT    (0x1<<1)      // REGISTER STATUS1
#define    LTC2874_OC_LP1_STAT     0x1          // REGISTER STATUS1

#define    LTC2874_PWRGD4         (0x1<<7)      // REGISTER STATUS2
#define    LTC2874_PWRGD3         (0x1<<6)      // REGISTER STATUS2
#define    LTC2874_PWRGD2         (0x1<<5)      // REGISTER STATUS2
#define    LTC2874_PWRGD1         (0x1<<4)      // REGISTER STATUS2
#define    LTC2874_OC_CQ4         (0x1<<3)      // REGISTER STATUS2
#define    LTC2874_OC_CQ3         (0x1<<2)      // REGISTER STATUS2
#define    LTC2874_OC_CQ2         (0x1<<1)      // REGISTER STATUS2
#define    LTC2874_OC_OQ1          0x1          // REGISTER STATUS2

#define    LTC2874_24VMODE        (0x1<<7)      // REGISTER MODE1
#define    LTC2874_CSENSE_MODE    (0x1<<6)      // REGISTER MODE1
// multibit definition
#define    LTC2874_2XPTC_pos         4          // REGISTER MODE1
#define    LTC2874_2XPTC_msk      (0x3<<4)      // REGISTER MODE1
#define    LTC2874_2XPTC(value)   (LTC2874_2XPTC_msk & ((value) << LTC2874_2XPTC_pos))

#define    LTC2874_FLDBK_MODE     (0x1<<3)      // REGISTER MODE1
#define    LTC2874_RETRY_OV       (0x1<<2)      // REGISTER MODE1
#define    LTC2874_RETRY_LP       (0x1<<1)      // REGISTER MODE1
#define    LTC2874_RETRY_CQ        0x1          // REGISTER MODE1

#define    LTC2874_SLEW4          (0x1<<7)      // REGISTER MODE2
#define    LTC2874_SLEW3          (0x1<<6)      // REGISTER MODE2
#define    LTC2874_SLEW2          (0x1<<5)      // REGISTER MODE2
#define    LTC2874_SLEW1          (0x1<<4)      // REGISTER MODE2
// multibit definition
#define    LTC2874_OV_TH_pos         2          // REGISTER MODE2
#define    LTC2874_OV_TH_msk      (0x3<<2)      // REGISTER MODE2
#define    LTC2874_OV_TH(value)   (LTC2874_OV_TH_msk & ((value) << LTC2874_OV_TH_pos))

#define    LTC2874_OV_ALLOW       (0x1<<1)      // REGISTER MODE2
#define    LTC2874_CQ_SNS_MODE     0x1          // REGISTER MODE2

// multibit definitions
#define    LTC2874_NSF4_pos          6          // REGISTER NSF
#define    LTC2874_NSF4_msk       (0x3<<6)      // REGISTER NSF
#define    LTC2874_NSF4(value)    (LTC2874_NSF4_msk & ((value) << LTC2874_NSF4_pos))
#define    LTC2874_NSF3_pos          4          // REGISTER NSF
#define    LTC2874_NSF3_msk       (0x3<<4)      // REGISTER NSF
#define    LTC2874_NSF3(value)     LTC2874_NSF3_msk & ((value) << LTC2874_NSF3_pos))
#define    LTC2874_NSF2_pos          2          // REGISTER NSF
#define    LTC2874_NSF2_msk       (0x3<<2)      // REGISTER NSF
#define    LTC2874_NSF2(value)    (LTC2874_NSF2_msk & ((value) << LTC2874_NSF2_pos))
#define    LTC2874_NSF1_pos          0          // REGISTER NSF
#define    LTC2874_NSF1_msk         0x3         // REGISTER NSF
#define    LTC2874_NSF1(value)     (LTC2874_NSF1_msk & ((value) << LTC2874_NSF1_pos))

#define    LTC2874_ILLM4_pos         6          // REGISTER ILLM
#define    LTC2874_ILLM4_msk       (0x3<<6)     // REGISTER ILLM
#define    LTC2874_ILLM4(value)    (LTC2874_ILLM4_msk & ((value) << LTC2874_ILLM4_pos))
#define    LTC2874_ILLM3_pos        4           // REGISTER ILLM
#define    LTC2874_ILLM3_msk       (0x3<<4)     // REGISTER ILLM
#define    LTC2874_ILLM3(value)    (LTC2874_ILLM3_msk & ((value) << LTC2874_ILLM3_pos))
#define    LTC2874_ILLM2_pos        2           // REGISTER ILLM
#define    LTC2874_ILLM2_msk       (0x3<<2)     // REGISTER ILLM
#define    LTC2874_ILLM2(value)    (LTC2874_ILLM2_msk & ((value) << LTC2874_ILLM2_pos))
#define    LTC2874_ILLM1_pos        0           // REGISTER ILLM
#define    LTC2874_ILLM1_msk        0x3         // REGISTER ILLM
#define    LTC2874_ILLM1(value)    (LTC2874_ILLM1_msk & ((value) << LTC2874_ILLM1_pos))

#define    LTC2874_LPTC_pos         4           // REGISTER TMRCTRL
#define    LTC2874_LPTC_msk        (0xF<<4)     // REGISTER TMRCTRL
#define    LTC2874_LPTC(value)     (LTC2874_LPTC_msk & ((value) << LTC2874_LPTC_pos))

#define    LTC2874_RETRYTC_pos     0            // REGISTER TMRCTRL
#define    LTC2874_RETRYTC_msk     0x07         // REGISTER TMRCTRL
#define    LTC2874_RETRYTC(value) (LTC2874_RETRYTC_msk & (value))

#define    LTC2874_WKUP4          (0x1<<7)      // REGISTER CTRL1
#define    LTC2874_WKUP3          (0x1<<6)      // REGISTER CTRL1
#define    LTC2874_WKUP2          (0x1<<5)      // REGISTER CTRL1
#define    LTC2874_WKUP1          (0x1<<4)      // REGISTER CTRL1
#define    LTC2874_DRVEN4         (0x1<<3)      // REGISTER CTRL1
#define    LTC2874_DRVEN3         (0x1<<2)      // REGISTER CTRL1
#define    LTC2874_DRVEN2         (0x1<<1)      // REGISTER CTRL1
#define    LTC2874_DRVEN1          0x1          // REGISTER CTRL1

#define    LTC2874_ENLP4          (0x1<<7)      // REGISTER CTRL2
#define    LTC2874_ENLP3          (0x1<<6)      // REGISTER CTRL2
#define    LTC2874_ENLP2          (0x1<<5)      // REGISTER CTRL2
#define    LTC2874_ENLP1          (0x1<<4)      // REGISTER CTRL2
#define    LTC2874_SIO_MODE4      (0x1<<3)      // REGISTER CTRL2
#define    LTC2874_SIO_MODE3      (0x1<<2)      // REGISTER CTRL2
#define    LTC2874_SIO_MODE2      (0x1<<1)      // REGISTER CTRL2
#define    LTC2874_SIO_MODE1       0x1          // REGISTER CTRL2

#define    LTC2874_CQ1     0x01
#define    LTC2874_CQ2     0x02
#define    LTC2874_CQ3     0x04
#define    LTC2874_CQ4     0x08
#define    LTC2874_CQ_ALL  0x0F

#define    LTC2874_LP1     0x10
#define    LTC2874_LP2     0x20
#define    LTC2874_LP3     0x40
#define    LTC2874_LP4     0x80
#define    LTC2874_LP_ALL  0xF0

//! @}

//************************************************************************************
//! Enables or Disables CQ output for specified port, then Updates.
//! @return void
//************************************************************************************
void LTC2874_cq_output(uint8_t port,             //!< Port number (1-4 or 5=ALL)
                       uint8_t value             //!< Enable (=1) or Disable (=0)
                      );

//************************************************************************************
//! Enables or Disables L+ output for specified port, then Updates.
//! @return void
//************************************************************************************
void LTC2874_lplus_output(uint8_t port,                  //!< Port number (1-4 or 5=ALL)
                          uint8_t value                  //!< Enable (=1) or Disable (=0)
                         );

//************************************************************************************
//! Sets SIO_MODE bit (and also Clears SLEW bit) for specified port, then Updates.
//! @return void
//************************************************************************************
void LTC2874_sio_mode(uint8_t port               //!< Port number (1-4 or 5=ALL)
                     );

//************************************************************************************
//! Changes value of the 24VMODE bit, then Updates.
//! @return void
//************************************************************************************
void LTC2874_24v_mode(uint8_t value              //!< 24VMODE setting (0 or 1)
                     );

//************************************************************************************
//! Changes NSF (Noise Suppression Filter) setting for specified port
//! without Updating.
//! value definitions: 0x0=disabled, 0x1=20.3us, 0x2=2.8us, 0x3=0.6us (default)
//! @return void
//************************************************************************************
void LTC2874_write_NSF_value(uint8_t port,          //!< Port number (1-4)
                             uint8_t value          //!< NSF setting (0x0-0x3)
                            );

//************************************************************************************
//! Changes ILLM (Sinking current) setting for specified port without Updating.
//! value definitions: 0x0=500kohm, 0x1=2.5mA, 0x2=3.7mA, 0x3=6.2mA (default)
//! @return void
//************************************************************************************
void LTC2874_write_ILLM_value(uint8_t port,         //!< Port number (1-4 or 5=ALL)
                              uint8_t value         //!< ILLM setting (0x0-0x3)
                             );

//************************************************************************************
//! Change L+ Overcurrent Timer Control (LPTC) setting without Updating.
//! value definitions: 0x0=17us, 0x1=30us, 0x2=60us, 0x3=120us, 0x4=0.25ms, 0x5=0.5ms,
//!                    0x6=1ms, 0x7=2ms, 0x8=3.9ms, 0x9=7.8ms, 0xA=16ms, 0xB=30ms,
//!                    0xC=0.60ms, 0xD=0.13s, 0xE=0.25s, 0xF=0.25s
//! @return void
//************************************************************************************
void LTC2874_write_LPTC_value(uint8_t value     //!< LPTC setting (0x0-0xF)
                             );

//************************************************************************************
//! Changes Auto-Retry Timer Control (RETRYTC) setting without Updating.
//! value definitions: 0x0=0.12s, 0x1=0.24s, 0x2=0.49s, 0x3=0.98s, 0x4=2.0s,
//!                    0x5=3.9s, 0x6=7.9s, 0x7=15.7s
//! @return void
//************************************************************************************
void LTC2874_write_RETRYTC_value(uint8_t value    //!< RETRYTC setting (0x0-0x7)
                                );

//************************************************************************************
//! Changes 2X Current Pulse Timer Control (2XPTC) setting without Updating.
//! value definitions: 0x0=60ms (default), 0x1=disabled, 0x2=30ms, 0x3=120ms
//! @return void
//************************************************************************************
void LTC2874_write_2XPTC_value(uint8_t value      //!< 2XPTC setting (0x0-0x3)
                              );

//************************************************************************************
//! Changes VDD Overvoltage Threshold (OV_TH) setting without Updating.
//! value definitions: 0x0=18V, 0x1=32V (default), 0x2=34V, 0x3=36V
//! @return void
//************************************************************************************
void LTC2874_write_OV_TH_value(uint8_t value      //!< OV_TH setting (0x0-0x3)
                              );

//************************************************************************************
//! Sends WURQ (Wake-up Request) on specified port.
//! @return void
//************************************************************************************
void LTC2874_wakeup_request(uint8_t port          //!< Port number (1-4)
                           );

//************************************************************************************
//! Updates all registers.
//! @return void
//************************************************************************************
void LTC2874_update_all(void                      //!< void
                       );

//************************************************************************************
//! Writes byte of data to a register, then Updates.
//! @return void
//************************************************************************************
void LTC2874_write_register_update_all(uint8_t LTC2874_register,  //!< Register address (0x0-0xE)
                                       uint8_t value              //!< Data to be written into register
                                      );

//************************************************************************************
//! Changes value of a register bit, then Updates.
//! @return void
//************************************************************************************
void LTC2874_write_bit_value_update_all(uint8_t LTC2874_register, //!< Register address (0x0-0xE)
                                        uint8_t LTC2874_bit,      //!< Mask of bit to be written (e.g. 0x40)
                                        uint8_t value             //!< Bit value to be written (0 or 1)
                                       );

//************************************************************************************
//! Sets a register bit, then Updates.
//! @return void
//************************************************************************************
void LTC2874_write_bit_set_update_all(uint8_t LTC2874_register,   //!< Register address (0x0-0xE)
                                      uint8_t LTC2874_bit         //!< Mask of bit to be set (e.g. 0x40)
                                     );

//************************************************************************************
//! Clears a register bit, then Updates.
//! @return void
//************************************************************************************
void LTC2874_write_bit_clr_update_all(uint8_t LTC2874_register,   //!< Register address (0x0-0xE)
                                      uint8_t LTC2874_bit         //!< Mask of bit to be cleared (e.g. 0x40)
                                     );

//************************************************************************************
//! Resets LTC2874, returning default values to registers.
//! @return void
//************************************************************************************
void LTC2874_reset(void //!< void
                  );

//************************************************************************************
//! Reads data byte from specified register.
//! @return value of byte read
//************************************************************************************
uint8_t LTC2874_read_reg(uint8_t LTC2874_register      //!< Register address (0x0-0xE)
                        );

//************************************************************************************
//! Reads a data BIT from specified register.
//! @return value of bit read
//************************************************************************************
uint8_t LTC2874_read_bit(uint8_t LTC2874_register,     //!< Register address (0x0-0xE)
                         uint8_t LTC2874_bit           //!< Mask of bit to be read (e.g. 0x40)
                        );

//************************************************************************************
//! Writes byte of data to a register without Updating.
//! @return void
//************************************************************************************
void LTC2874_write_register(uint8_t LTC2874_register,  //!< Register address (0x0-0xE)
                            uint8_t value              //!< Data to be written into register
                           );

//************************************************************************************
//! Sets a register bit without Updating.
//! @return void
//************************************************************************************
void LTC2874_write_bit_set(uint8_t LTC2874_register,   //!< Register address (0x0-0xE)
                           uint8_t LTC2874_bit         //!< Mask of bit to be set (e.g. 0x40)
                          );

//************************************************************************************
//! Clears a register bit without Updating.
//! @return void
//************************************************************************************
void LTC2874_write_bit_clr(uint8_t LTC2874_register,   //!< Register address (0x0-0xE)
                           uint8_t LTC2874_bit         //!< Mask of bit to be set (e.g. 0x40)
                          );

//************************************************************************************
//! Changes value of a register bit without Updating.
//! @return void
//************************************************************************************
void LTC2874_write_bit_value(uint8_t LTC2874_register, //!< Register address (0x0-0xE)
                             uint8_t LTC2874_bit,      //!< Mask of bit to be written (e.g. 0x40)
                             uint8_t value             //!< Bit value to be written (0 or 1)
                            );

#endif  // LTC2874_H