/*! LTC6812: Multicell Battery Monitors
*
*@verbatim
*The LTC6812 is multicell battery stack monitor that measures up to 15 series 
*connected battery cells with a total measurement error of less than 2.2mV. 
*The cell measurement range of 0V to 5V makes the LTC6812 suitable for most 
*battery chemistries. All 15 cell voltages can be captured in 245uS, and lower 
*data acquisition rates can be selected for high noise reduction.
*Using the LTC6812-1, multiple devices are connected in a daisy-chain with one 
*host processor connection for all devices, permitting simultaneous cell monitoring 
*of long, high voltage battery strings.
*@endverbatim
*
* https://www.analog.com/en/products/ltc6812-1.html
* The schematic of DC2350A : https://www.analog.com/media/en/technical-documentation/eval-board-schematic/DC2350A-4-SCH.PDF 
* ( Ensure that you modify the DC2350A board as mentioned on the schematic to work for LTC6812 (DC2350A-A) )
*
*********************************************************************************
* Copyright 2019(c) Analog Devices, Inc.
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*  - Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*  - Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*  - Neither the name of Analog Devices, Inc. nor the names of its
*    contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*  - The use of this software may or may not infringe the patent rights
*    of one or more patent holders.  This license does not release you
*    from the requirement that you obtain separate licenses from these
*    patent holders to use this software.
*  - Use of the software either in source or binary form, must be run
*    on or directly connected to an Analog Devices Inc. component.
*
* THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/*! @file
    @ingroup LTC6812-1
    Library Header for LTC6812-1 Multicell Battery Monitor
*/

#ifndef LTC6812_H
#define LTC6812_H

#include "stdint.h"
#include "LTC681x.h"

#define CELL 1
#define AUX 2
#define STAT 3

/*!
 Initialize the Register limits
 @return void 
 */
void LTC6812_init_reg_limits(uint8_t total_ic, //!< Number of ICs in the system
							 cell_asic *ic //!< A two dimensional array that will store the data
							 );

/*!
 Write the LTC6812 configuration register
 @return void 
 */
void LTC6812_wrcfg(uint8_t total_ic, //!< The number of ICs being written
                   cell_asic *ic //!< A two dimensional array of the configuration data that will be written
                  );

/*! 
 Write the LTC6812 configuration register B
 @return void	
 */			  
void LTC6812_wrcfgb(uint8_t total_ic, //!< The number of ICs being written
                   cell_asic *ic //!< A two dimensional array of the configuration data that will be written
                  );
				  
/*!
 Reads configuration registers of a LTC6812 daisy chain
 @return int8_t, pec_error PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC 
  */
int8_t LTC6812_rdcfg(uint8_t total_ic, //!< Number of ICs in the daisy chain
                     cell_asic *ic //!< A two dimensional array that the function stores the read configuration data
                    );

/*!
 Reads configuration registers of a LTC6812 daisy chain
 @return int8_t, pec_error PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC 
  */					
int8_t LTC6812_rdcfgb(uint8_t total_ic, //!< Number of ICs in the daisy chain
                     cell_asic *ic //!< A two dimensional array that the function stores the read configuration data
                    );
							 
/*!
 Starts cell voltage conversion
 @return void 
 */
void LTC6812_adcv(uint8_t MD, //!< ADC Conversion Mode
                  uint8_t DCP, //!< Controls if Discharge is permitted during conversion
                  uint8_t CH //!< Sets which Cell channels are converted
                 );

/*!
 Start a GPIO and Vref2 Conversion
 @return void 
 */
void LTC6812_adax(uint8_t MD, //!< ADC Conversion Mode
				  uint8_t CHG //!< Sets which GPIO channels are converted
				  );

/*!
 Start a Status ADC Conversion
 @return void 
 */
void LTC6812_adstat(uint8_t MD, //!< ADC Conversion Mode
					uint8_t CHST //!< Sets which Stat channels are converted
					);				  
				 
/*!
 Starts cell voltage  and GPIO 1&2 conversion
 @return void 
 */
void LTC6812_adcvax(uint8_t MD, //!< ADC Conversion Mode
					uint8_t DCP //!< Controls if Discharge is permitted during conversion
					);

/*!
 Starts cell voltage and SOC conversion
 @return void 
 */
void LTC6812_adcvsc(uint8_t MD, //!< ADC Conversion Mode
					uint8_t DCP //!< Controls if Discharge is permitted during conversion
					);					

/*!
 Reads and parses the LTC6812 cell voltage registers.
 @return uint8_t, pec_error PEC Status.
   0: No PEC error detected
  -1: PEC error detected, retry read 
  */
uint8_t LTC6812_rdcv(uint8_t reg, //!< Controls which cell voltage register is read back
                     uint8_t total_ic, //!< The number of ICs in the daisy chain
                     cell_asic *ic //!< Array of the parsed cell codes from lowest to highest
                    );

/*!
 Reads and parses the LTC6812 auxiliary registers.
 @return  int8_t, pec_error PEC Status
   0: No PEC error detected
  -1: PEC error detected, retry read 
  */
int8_t LTC6812_rdaux(uint8_t reg,  //!< Controls which GPIO voltage register is read back
                     uint8_t total_ic,  //!< The number of ICs in the daisy chain
                     cell_asic *ic //!< A two dimensional array of the parsed gpio voltage codes
                    );

/*!
 Reads and parses the LTC6812 stat registers.
 @return  int8_t, pec_error PEC Status
   0: No PEC error detected
  -1: PEC error detected, retry read 
  */
int8_t LTC6812_rdstat(uint8_t reg, //!<Determines which Stat  register is read back
                        uint8_t total_ic,//!<The number of ICs in the system
                        cell_asic *ic  //!< A two dimensional array that will store the data 
                       );	
					   
/*! 
 Sends the poll ADC command
 @returns 1 byte read back after a pladc command. If the byte is not 0xFF ADC conversion has completed 
  */
uint8_t LTC6812_pladc();

/*!
 This function will block operation until the ADC has finished it's conversion.
 @returns uint32_t, counter The approximate time it took for the ADC function to complete
  */
uint32_t LTC6812_pollAdc();
					   			   
/*!
 Clears the LTC6812 cell voltage registers
 @return void 
 */
void LTC6812_clrcell();

/*!
 Clears the LTC6812 Auxiliary registers
 @return void 
 */
void LTC6812_clraux();

/*!
  Clears the LTC6812 Stat registers
 @return void 
 */
void LTC6812_clrstat();

/*!
 Starts the Mux Decoder diagnostic self test
 Running this command will start the Mux Decoder Diagnostic Self Test.
 This test takes roughly 1ms to complete. The MUXFAIL bit will be updated.
 The bit will be set to 1 for a failure and 0 if the test has been passed.
 @return void 
 */
void LTC6812_diagn();
					   
/*!
 Starts cell voltage self test conversion
 @return void  
 */
void LTC6812_cvst(uint8_t MD, //!< ADC Conversion Mode
				uint8_t ST //!< Sets if self test 1 or 2 is run
				);

/*!
 Start an Auxiliary Register Self Test Conversion
 @return void 
 */
void LTC6812_axst(uint8_t MD, //!< ADC Conversion Mode
					uint8_t ST //!< Sets if self test 1 or 2 is run
					);

/*!
 Start a Status Register Self Test Conversion
 @return void 
 */
void LTC6812_statst(uint8_t MD, //!< ADC Conversion Mode
					uint8_t ST //!< Sets if self test 1 or 2 is run
					);					
								
/*!
 Starts Cell voltage overlap conversion
 @return void 
 */
void LTC6812_adol(uint8_t MD, //!< ADC Conversion Mode
					uint8_t DCP //!< Discharge permitted during conversion
					);

/*!
 Start an GPIO Redundancy test
 @return void 
 */
void LTC6812_adaxd(uint8_t MD, //!< ADC Conversion Mode
					uint8_t CHG //!< Sets which GPIO channels are converted
					);

/*!
 Start a Status register redundancy test Conversion
 @return void 
 */
void LTC6812_adstatd(uint8_t MD, //!< ADC Mode
					uint8_t CHST //!< Sets which Status channels are converted
					);					

/*!
 Helper function that runs the ADC Self Tests
 @return int16_t, error   Number of errors detected. 
 */
int16_t LTC6812_run_cell_adc_st(uint8_t adc_reg,  //!<  Type of register
								uint8_t total_ic, //!< Number of ICs in the daisy chain
								cell_asic *ic, //!< A two dimensional array that will store the data 
								uint8_t md, //!< ADC Mode
								bool adcopt //!< The ADCOPT bit in the configuration register
								);

/*!
 Helper Function that runs the ADC Overlap test
 @return uint16_t, error
  0: Pass
 -1: False, Error detected 
 */
uint16_t LTC6812_run_adc_overlap(uint8_t total_ic, //!< Number of ICs in the daisy chain
                                 cell_asic *ic //!< A two dimensional array that will store the data 
                                 );								
								
/*!
 Helper function that runs the ADC Digital Redundancy commands and checks output for errors
 @return int16_t, error   Number of errors detected. 
 */
int16_t LTC6812_run_adc_redundancy_st(uint8_t adc_mode, //!< ADC Mode
                                      uint8_t adc_reg,  //!< Type of register
                                      uint8_t total_ic, //!< Number of ICs in the daisy chain
                                      cell_asic *ic //!< A two dimensional array that will store the data 
									 );
					
/*!
 Start an open wire Conversion
 @return void 
 */
void LTC6812_adow(uint8_t MD, //!< ADC Conversion Mode
				 uint8_t PUP, //!< Pull up/Pull down current
				 uint8_t CH,  //!< Sets which Cell channels are converted
				 uint8_t DCP  //!< Discharge permitted during conversion
				 );
				 
/*!
 Start GPIOs open wire ADC conversion
 @return void 
 */
void LTC6812_axow( uint8_t MD, //!< ADC Mode
				  uint8_t PUP  //!< Pull up/Pull down current
				);				 

/*!
 Runs the data sheet algorithm for open wire for single cell detection 
 @return void 
 */
void LTC6812_run_openwire_single(uint8_t total_ic, //!< Number of ICs in the daisy chain
								 cell_asic *ic //!< A two dimensional array that will store the data 
								 );
								 
/*!
 Runs the data sheet algorithm for open wire for multiple cell and two consecutive cells detection 
 @return void 
 */ 
void LTC6812_run_openwire_multi(uint8_t total_ic, //!< Number of ICs in the daisy chain
								cell_asic *ic //!< A two dimensional array that will store the data 
								);				 

/*!
 Runs open wire for GPIOs
 @return void 
 */
void LTC6812_run_gpio_openwire(uint8_t total_ic, //!< Number of ICs in the daisy chain
								cell_asic *ic //!< A two dimensional array that will store the data 
								);		

/*!
 Helper function to set discharge bit in CFG register
 @return void 
 */                   
void LTC6812_set_discharge(int Cell, //!< The cell to be discharged
                           uint8_t total_ic, //!< Number of ICs in the system
                           cell_asic *ic//!< A two dimensional array that will store the data
							);
							
/*!
 Clears all of the DCC bits in the configuration registers
 @return void 
 */							   
void LTC6812_clear_discharge(uint8_t total_ic, //!< Number of ICs in the system
                             cell_asic *ic //!< A two dimensional array that will store the data
							 );
					
/*!
 Write the LTC6812 PWM register
 @return void 
 */
void LTC6812_wrpwm(uint8_t total_ic, //!< Number of ICs in the daisy chain
                   uint8_t pwmReg, //!< Select register
                   cell_asic *ic //!< a two dimensional array that will store the data 
                  );

/*!
 Reads pwm registers of a LTC6811 daisy chain
 @return int8_t, pec_error PEC Status.
   0: Data read back has matching PEC
  -1: Data read back has incorrect PEC 
  */
int8_t LTC6812_rdpwm(uint8_t total_ic, //!< Number of ICs in the daisy chain
                     uint8_t pwmReg, //!< Select register
                     cell_asic *ic //!< A two dimensional array that the function stores the read pwm data
                    );

/*!
  Write the LTC6812 Sctrl register
 @return void 
 */
void LTC6812_wrsctrl(uint8_t total_ic, //!< Number of ICs in the daisy chain
                     uint8_t sctrl_reg, //!< Select register
                     cell_asic *ic  //!< A two dimensional array that will store the data 
                    );

/*!
 Reads sctrl registers of a LTC6812 daisy chain
 @return int8_t, pec_error PEC Status.
   0: Data read back has matching PEC
  -1: Data read back has incorrect PEC 
   */
int8_t LTC6812_rdsctrl(uint8_t total_ic, //!< Number of ICs in the daisy chain
                       uint8_t sctrl_reg, //!< Select register
                       cell_asic *ic //!< A two dimensional array that the function stores the read data
                      );

/*!
 Start Sctrl data communication
 This command will start the sctrl pulse communication over the spins
 @return void 
 */
void LTC6812_stsctrl();

/*!
 Clears the LTC6812 Sctrl registers
 @return void 
 */
void LTC6812_clrsctrl();

/*!
 Write the 6812 PWM/Sctrl Register B 
 @return void 
 */
void LTC6812_wrpsb(uint8_t total_ic, //!< Number of ICs in the daisy chain
					cell_asic *ic //!< A two dimensional array that will store the data 
					
					);
					
/*!
 Reading pwm/s control register b
 @return int8_t, pec_error PEC Status.
   0: Data read back has matching PEC
  -1: Data read back has incorrect PEC 
  */
int8_t LTC6812_rdpsb(uint8_t total_ic, //!< Number of ICs in the daisy chain
                       cell_asic *ic //!< A two dimensional array that the function stores the read data
                      );

/*!
 Write the LTC6812 COMM register
 @return void 
 */
void LTC6812_wrcomm(uint8_t total_ic, //!< Number of ICs in the daisy chain
                    cell_asic *ic //!< A two dimensional array of the comm data that will be written
                   );

/*!
 Reads comm registers of a LTC6812 daisy chain
 @return int8_t, pec_error PEC Status.
   0: Data read back has matching PEC
  -1: Data read back has incorrect PEC 
 */
int8_t LTC6812_rdcomm(uint8_t total_ic, //!< Number of ICs in the daisy chain
                      cell_asic *ic //!< Two dimensional array that the function stores the read comm data
                     );

/*!
 Issues a stcomm command and clocks data out of the COMM register 
 @return void 
 */
void LTC6812_stcomm(uint8_t len);
				   									 
/*!
 Mutes the LTC6812 discharge transistors
 @return void 
 */
void LTC6812_mute();

/*!
 Clears the LTC6812 Mute Discharge
 @return void 
 */
void LTC6812_unmute();							  
								 
/*!
 Helper Function that counts overall PEC errors and register/IC PEC errors 
 @return void 
 */
void LTC6812_check_pec(uint8_t total_ic, //!< Number of ICs in the daisy chain
                       uint8_t reg, //!< Type of register
                       cell_asic *ic //!< A two dimensional array that will store the data 
						);
					   
/*!
 Helper Function that resets the PEC error counters  
 @return void 
 */
void LTC6812_reset_crc_count(uint8_t total_ic, //!< Number of ICs in the daisy chain
                             cell_asic *ic //!< A two dimensional array that will store the data 
							 );
							 
/*!
 Helper Function to initialize the CFGR data structures 
 @return void 
 */
void LTC6812_init_cfg(uint8_t total_ic, //!< Number of ICs in the daisy chain
                      cell_asic *ic //!< A two dimensional array that will store the data 
						);
					  
/*!
 Helper function to set appropriate bits in CFGR register based on bit function 
 @return void 
 */
void LTC6812_set_cfgr(uint8_t nIC, //!< Current IC
                      cell_asic *ic, //!< A two dimensional array that will store the data 
                      bool refon,  //!< The REFON bit
                      bool adcopt,  //!< The ADCOPT bit
                      bool gpio[5], //!< The GPIO bits
                      bool dcc[12], //!< The DCC bits 
					  bool dcto[4], //!< The Dcto bits
					  uint16_t uv, //!< The UV value
					  uint16_t  ov //!< The OV value
					  );

/*!
 Helper function to turn the refon bit HIGH or LOW 
 @return void 
 */
void LTC6812_set_cfgr_refon(uint8_t nIC,//!< Current IC
                            cell_asic *ic, //!< A two dimensional array that will store the data 
                            bool refon //!< The REFON bit
							);
                            
/*!
 Helper function to turn the ADCOPT bit HIGH or LOW 
 @return void 
 */
void LTC6812_set_cfgr_adcopt(uint8_t nIC,//!< Current IC
                             cell_asic *ic, //!< A two dimensional array that will store the data  
                             bool adcopt //!< The ADCOPT bit
							 );

/*!
 Helper function to turn the GPIO bits HIGH or LOW 
 @return void 
 */
void LTC6812_set_cfgr_gpio(uint8_t nIC, //!< Current IC
                           cell_asic *ic, //!< A two dimensional array that will store the data 
                           bool gpio[] //!< The GPIO bits
						   );

/*!
 Helper function to turn the DCC bits HIGH or LOW 
 @return void 
 */
void LTC6812_set_cfgr_dis(uint8_t nIC, //!< Current IC
                          cell_asic *ic, //!< A two dimensional array that will store the data 
                          bool dcc[] //!< The DCC bits 
						  );
						  
/*!
 Helper Function to set dcto value in CFG register 
 @return void 
 */
void LTC6812_set_cfgr_dcto(uint8_t nIC, //!< Current IC
						   cell_asic *ic, //!< A two dimensional array that will store the data 
						   bool dcto[4] //!< The Dcto bits
						   );		
						   
/*! Helper function to set uv field in CFGRA register 
 @return void 
 */
void LTC6812_set_cfgr_uv(uint8_t nIC, //!< Current IC
                         cell_asic *ic, //!< A two dimensional array that will store the data 
                         uint16_t uv //!< The UV value
						 );
                         
/*!
 Helper function to set OV field in CFGRA register 
 @return void 
 */
void LTC6812_set_cfgr_ov(uint8_t nIC, //!< Current IC
                         cell_asic *ic, //!< A two dimensional array that will store the data 
                         uint16_t ov //!< The OV value
						 );
 
/*!
 Helper Function to initialize the CFGR B data structures 
 @return void
 */
void LTC6812_init_cfgb(uint8_t total_ic, //!< Number of ICs in the daisy chain
                       cell_asic *ic//!< A two dimensional array that will store the data 
					  );
					  
/*!
 Helper function to set appropriate bits in CFGR register based on bit function 
 @return void 
 */
void LTC6812_set_cfgrb(uint8_t nIC, //!< Current IC
                      cell_asic *ic, //!< A two dimensional array that will store the data 
					  bool fdrf, //!< The FDRF bit
                      bool dtmen, //!< The DTMEN bit
                      bool ps[2], //!< Path selection bits
                      bool gpiobits[4], //!< The GPIO bits
					  bool dccbits[4] //!< The DCC bits 
					  );

/*!
 Helper function to turn the FDRF bit HIGH or LOW 
 @return void
 */
void LTC6812_set_cfgrb_fdrf(uint8_t nIC,//!< Current IC
                            cell_asic *ic, //!< A two dimensional array that will store the data 
                            bool fdrf //!< The FDRF bit
							);
							
/*!
 Helper function to turn the DTMEN bit HIGH or LOW 
 @return void 
 */
void LTC6812_set_cfgrb_dtmen(uint8_t nIC,//!< Current IC
                            cell_asic *ic, //!< A two dimensional array that will store the data 
                            bool dtmen //!< The DTMEN bit
							);
							
/*!
 Helper function to turn the Path Select bit HIGH or LOW 
 @return void 
 */
void LTC6812_set_cfgrb_ps(uint8_t nIC,//!< Current IC
                            cell_asic *ic, //!< A two dimensional array that will store the data 
                            bool ps[] //!< Path selection bits
							);
							
/*!
 Helper function to turn the GPIO bit HIGH or LOW 
 @return void
 */
void LTC6812_set_cfgrb_gpio_b(uint8_t nIC,//!< Current IC
                            cell_asic *ic, //!< A two dimensional array that will store the data 
                            bool gpiobits[] //!< The GPIO bits
							);
							
/*!
 Helper function to turn the DCC bit HIGH or LOW 
 @return void 
 */
void LTC6812_set_cfgrb_dcc_b(uint8_t nIC,//!< Current IC
                            cell_asic *ic, //!< A two dimensional array that will store the data 
                            bool dccbits[] //!< The DCC bits 
							);
							
#endif
