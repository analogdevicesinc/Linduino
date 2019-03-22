/***************************************************************************//**
*   @file LTC6810.h  
*   @brief Header for LTC6810-1 Multi-cell Battery Monitor
*   @author BMS (bms.support@analog.com)

REVISION HISTORY
$Revision: 1.0 $
$Date: 2019-3 $
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
    @ingroup LTC6810-1
    Header for LTC6810-1 Multicell Battery Monitor
*/

#ifndef LTC6810_H
#define LTC6810_H

#include "stdint.h"
#include "LTC681x.h"

#define CELL 1
#define AUX 2
#define STAT 3

//! Initialize the Register limits
//!@return void
void LTC6810_init_reg_limits(uint8_t total_ic, //!< Number of ICs in the system
							cell_asic *ic //!< A two dimensional array that will store the data
							);
    
//! Starts the Mux Decoder diagnostic self test
//! Running this command will start the Mux Decoder Diagnostic Self Test.
//! This test takes roughly 1ms to complete. The MUXFAIL bit will be updated.
//! The bit will be set to 1 for a failure and 0 if the test has been passed.
//!@return void
void LTC6810_diagn();


//! Sends the poll ADC command
//! @returns 1 byte read back after a pladc command. If the byte is not 0xFF ADC conversion has completed
uint8_t LTC6810_pladc();


//! This function will block operation until the ADC has finished it's conversion
//! @returns the approximate time it took for the ADC function to complete.
uint32_t LTC6810_pollAdc();

//! Starts cell voltage conversion
//!@return void
void LTC6810_adcv(uint8_t MD, //!< ADC Conversion Mode
                  uint8_t DCP, //!< Controls if Discharge is permitted during conversion
                  uint8_t CH //!< Sets which Cell channels are converted
                 );

//!  Starts cell voltage, Cell 0 and GPIO 1 conversion
//!@return void
void LTC6810_adcvax(
  uint8_t MD, //!< ADC Conversion Mode
  uint8_t DCP //!< Controls if Discharge is permitted during conversion
);


//!  Starts cell voltage self test conversion
//!@return void
void LTC6810_cvst(
  uint8_t MD, //!< ADC Conversion Mode
  uint8_t ST //!< Self Test Mode
);

//!  Starts cell voltage and SOC conversion
//!@return void
void LTC6810_adcvsc(
  uint8_t MD, //!< ADC Conversion Mode
  uint8_t DCP //!< Controls if Discharge is permitted during conversion
);

//!  Starts cell voltage overlap conversion
//!@return void
void LTC6810_adol(
  uint8_t MD, //!< ADC Conversion Mode
  uint8_t DCP //!< Discharge permitted during conversion
);

//!  Start an open wire Conversion
//!@return void
void LTC6810_adow(
 uint8_t MD, //!<  ADC Conversion Mode
 uint8_t PUP, //!<  Controls if Discharge is permitted during conversion
 uint8_t CH, //!< Channels
 uint8_t DCP//!< Discharge Permit
);


//!  Start a GPIO and Vref2 Conversion
//!@return void
void LTC6810_adax(
  uint8_t MD, //!< ADC Conversion Mode
  uint8_t CHG //!< Sets which GPIO channels are converted
);

//!  Start an GPIO Redundancy test
//!@return void
void LTC6810_adaxd(
  uint8_t MD, //!< ADC Conversion Mode
  uint8_t CHG //!< Sets which GPIO channels are converted
);

//!  Start an Auxiliary Register Self Test Conversion
//!@return void
void LTC6810_axst(
  uint8_t MD, //!< ADC Conversion Mode
  uint8_t ST //!< Sets if self test 1 or 2 is run
);

//!  Start a Status ADC Conversion
//!@return void
void LTC6810_adstat(
  uint8_t MD, //!< ADC Conversion Mode
  uint8_t CHST //!< Sets which Stat channels are converted
);

//!   Start a Status register redundancy test Conversion
//!@return void
void LTC6810_adstatd(
  uint8_t MD, //!< ADC Mode
  uint8_t CHST //!< Sets which Status channels are converted
);


//!  Start a Status Register Self Test Conversion
//!@return void
void LTC6810_statst(
  uint8_t MD, //!< ADC Conversion Mode
  uint8_t ST //!< Sets if self test 1 or 2 is run
);

/*!  Reads and parses the LTC6810 cell voltage registers.
  @return int8_t, PEC Status.
    0: No PEC error detected
    -1: PEC error detected, retry read
*/
uint8_t LTC6810_rdcv(uint8_t reg, //!< controls which cell voltage register is read back.
                     uint8_t total_ic, //!< the number of ICs in the daisy chain(-1 only)
                     cell_asic *ic //!< array of the parsed cell codes from lowest to highest.
                    );


/*!  Reads and parses the LTC6810 auxiliary registers.
@return  int8_t, PEC Status
  0: No PEC error detected
 -1: PEC error detected, retry read
*/
int8_t LTC6810_rdaux(uint8_t reg,        //!< controls which GPIO voltage register is read back
                     uint8_t nIC,        //!< the number of ICs in the daisy chain
                     cell_asic *ic //!< A two dimensional array of the parsed gpio voltage codes
                    );

/*!  Reads and parses the LTC6810 stat registers.
@return  int8_t, PEC Status
  0: No PEC error detected
 -1: PEC error detected, retry read
*/
int8_t LTC6810_rdstat(uint8_t reg, //!< Determines which Stat  register is read back.
                        uint8_t total_ic, //!< The number of ICs in the system
                        cell_asic *ic //!< A two dimensional array of the parsed gpio voltage codes
                       );
					
//!  Clears the LTC6812 cell voltage registers
//!@return void
void LTC6810_clrcell();

/*! Clears the LTC6810 Auxiliary registers
@return void
*/
void LTC6810_clraux();

/*!  Clears the LTC6810 Stat registers
@return void
*/
void LTC6810_clrstat();

/*!  Clears the LTC6810 Sctrl registers
@return void
*/
void LTC6810_clrsctrl();

/*!  Write the LTC6810 configuration register
@return void
*/
void LTC6810_wrcfg(uint8_t nIC, //!< The number of ICs being written
                   cell_asic *ic //!< a two dimensional array of the configuration data that will be written
                  );

/*!  Reads configuration registers of a LTC6810 daisy chain
@return int8_t, PEC Status.
  0: Data read back has matching PEC
   -1: Data read back has incorrect PEC
*/
int8_t LTC6810_rdcfg(uint8_t nIC, //!< number of ICs in the daisy chain
                     cell_asic *ic //!< a two dimensional array that the function stores the read configuration data
                    );

/*!  Write the LTC6810 PWM register
@return void
*/
void LTC6810_wrpwm(uint8_t nIC, //!< number of ICs in the daisy chain
                   uint8_t pwmReg, //!< Select register
                   cell_asic *ic //!< A two dimensional array of the parsed gpio voltage codes
                  );

/*!  Reads pwm registers of a LTC6810 daisy chain
@return void 
*/
int8_t LTC6810_rdpwm(uint8_t nIC, //!< number of ICs in the daisy chain
                     uint8_t pwmReg, //!< Select register
                     cell_asic *ic //!< a two dimensional array that the function stores the read pwm data
                    );

/*!  Write the LTC6810 Sctrl register
@return void
*/
void LTC6810_wrsctrl(uint8_t nIC, //!< number of ICs in the daisy chain
                     uint8_t sctrl_reg, //!< Select register
                     cell_asic *ic //!< A two dimensional array of the parsed gpio voltage codes
                    );


/*!  Reads sctrl registers of a LTC6810 daisy chain
@return int8_t, PEC Status.
  0: Data read back has matching PEC
  -1: Data read back has incorrect PEC
*/
int8_t LTC6810_rdsctrl(uint8_t nIC, //!< number of ICs in the daisy chain
                       uint8_t sctrl_reg, //!< Select register
                       cell_asic *ic //!< a two dimensional array that the function stores the read pwm data
                      );


/*!  Start Sctrl data communication
This command will start the sctrl pulse communication over the spins
@return void
*/
void LTC6810_stsctrl();


/*!  Write the LTC6810 COMM register
@return void
*/
void LTC6810_wrcomm(uint8_t total_ic, //!< Number of ICs in the daisy chain
                    cell_asic *ic //!< A two dimensional array of the comm data that will be written
                   );

/*!  Reads comm registers of a LTC6810 daisy chain
@return int8_t, PEC Status.

  0: Data read back has matching PEC

  -1: Data read back has incorrect PEC

*/
int8_t LTC6810_rdcomm(uint8_t total_ic, //!< number of ICs in the daisy chain
                      cell_asic *ic //!< Two dimensional array that the function stores the read comm data.
                     );

/*!  issues a stcomm command and clocks data out of the COMM register 
@return void
*/
void LTC6810_stcomm();


/*! Looks up the result pattern for digital filter self test 
@returns returns the register data pattern for a given ADC MD and Self test 
*/
uint16_t LTC6810_st_lookup(
  uint8_t MD, //!< ADC Conversion Mode
  uint8_t ST //!< Self test number
);
  
//! Helper function to set discharge bit in CFG register
//!@return void    
void LTC6810_set_discharge(int Cell, //!< The cell to be discharged
                           uint8_t total_ic, //!< number of ICs in the system
                           cell_asic *ic //!< a two dimensional array that will store the data
						   );
                   
/*! Helper function that runs the ADC Self Tests*/
//!@return int16_t, error
//! Number of errors detected.
int16_t LTC6810_run_cell_adc_st(uint8_t adc_reg, //!<  Type of register
                                uint8_t total_ic, //!< number of ICs in the daisy chain
                                cell_asic *ic, //!< A two dimensional array that will store the data 
								uint8_t md, //!< ADC Mode
								bool adcopt //!< the adcopt bit in the configuration register
								);

/*! Helper function that runs the ADC Digital Redundancy commands and checks output for errors*/
//!@return int16_t, error
int16_t LTC6810_run_adc_redundancy_st(uint8_t adc_mode, //!< ADC Mode
                                      uint8_t adc_reg,  //!<  Type of register
                                      uint8_t total_ic, //!< number of ICs in the daisy chain
                                      cell_asic *ic //!< A two dimensional array that will store the data 
									  );
									  
/*! Runs open wire for GPIOs*/
//!@return void
void LTC6810_run_gpio_openwire(uint8_t total_ic, //!< number of ICs in the daisy chain
								cell_asic *ic //!< A two dimensional array that will store the data 
								);

/*! Helper function that runs the data sheet open wire algorithm*/
//!@return void
void LTC6810_run_openwire_single(uint8_t total_ic,  //!< number of ICs in the daisy chain
                          cell_asic *ic //!< A two dimensional array that will store the data 
						  );

/*! Runs the data sheet algorithm for open wire for multiple cell and two consecutive cells detection*/
//!@return void
void LTC6810_run_openwire_multi(uint8_t total_ic, //!< number of ICs in the daisy chain 
						         cell_asic *ic   //!< A two dimensional array that will store the data 
						        );
								
/*!Clears all of the DCC bits in the configuration registers*/
//!@return void
void LTC6810_clear_discharge(uint8_t total_ic, //!< number of ICs in the daisy chain 
                             cell_asic *ic //!< A two dimensional array that will store the data 
							 );
                                 
/*! Helper Function that counts overall PEC errors and register/IC PEC errors*/
//!@return void
void LTC6810_check_pec(uint8_t total_ic, //!< number of ICs in the daisy chain 
                       uint8_t reg, //!<  Type of register
                       cell_asic *ic //!< A two dimensional array that will store the data 
					   );

/*! Helper Function that resets the PEC error counters */
//!@return void
void LTC6810_reset_crc_count(uint8_t total_ic, //!< number of ICs in the daisy chain 
                             cell_asic *ic//!< A two dimensional array that will store the data 
							 );

/*! Helper Function to initialize the CFGR data structures*/
//!@return void
void LTC6810_init_cfg(uint8_t total_ic, //!< number of ICs in the daisy chain 
                      cell_asic *ic //!< A two dimensional array that will store the data 
					  );

/*! Helper function to set appropriate bits in CFGR register based on bit function*/
//!@return void
void LTC6810_set_cfgr(uint8_t nIC, //!< Current IC
					  cell_asic *ic, //!< A two dimensional array that will store the data 
					  bool refon, //!< the REFON bit
					  bool adcopt, //!< the ADCOPT bit
					  bool gpio[4], //!< the GPIO bits
					  bool dcc[6],  //!< the DCC bits 
					  bool dcc_0,  //!< the DCC bit 
					  bool mcal, //!< Enable Multi-Calibration
					  bool en_dtmen, //!< Enable Discharge timer monitor
					  bool dis_red, //!< Disable Digital Redundancy Check
					  bool fdrf, //!< Force digital Redundancy Failure
					  bool sconv, //!< Enable Cell Measurement Redundancy using S Pin
					  bool dcto[4], //!<  Discharge Time Out Value
					  uint16_t uv, //!< the UV value
					  uint16_t  ov //!< the OV value
					  );

/*! Helper function to turn the refon bit HIGH or LOW*/
//!@return void
void LTC6810_set_cfgr_refon(uint8_t nIC, //!< Current IC
                            cell_asic *ic, //!< A two dimensional array that will store the data 
                            bool refon //!< the REFON bit
							);
                            
/*! Helper function to turn the ADCOPT bit HIGH or LOW*/
//!@return void
void LTC6810_set_cfgr_adcopt(uint8_t nIC, //!< Current IC
                             cell_asic *ic, //!< A two dimensional array that will store the data 
                             bool adcopt //!< the ADCOPT bit
							 );

/*! Helper function to turn the GPIO bits HIGH or LOW*/
//!@return void
void LTC6810_set_cfgr_gpio(uint8_t nIC, //!< Current IC
                           cell_asic *ic, //!< A two dimensional array that will store the data 
                           bool gpio[] //!< the GPIO bits
						   );
						   
/*! Helper function to turn the DCC bits HIGH or LOW*/
//!@return void
void LTC6810_set_cfgr_dis(uint8_t nIC, //!< Current IC
                          cell_asic *ic, //!< A two dimensional array that will store the data 
                          bool dcc[] //!< the DCC bits
						  ); 
						  
/*!  Helper function to set uv field in CFGRA register*/
//!@return void
void LTC6810_set_cfgr_uv(uint8_t nIC, //!< Current IC
                         cell_asic *ic, //!< A two dimensional array that will store the data 
                         uint16_t uv //!< the UV value
						 );
                         
/*!  Helper function to set ov field in CFGRA register*/
//!@return void
void LTC6810_set_cfgr_ov(uint8_t nIC, //!< Current IC
                         cell_asic *ic, //!< A two dimensional array that will store the data 
                         uint16_t ov //!< the OV value
						 );
						 
/*!  Helper function to set the REFON bit*/
//!@return void
void LTC6810_set_cfgr_mcal(uint8_t nIC, //!< Current IC
						   cell_asic *ic, //!< A two dimensional array that will store the data  
						   bool mcal //!< Enable Multi-Calibration
						   );

/*!  Helper function to set the REFON bit*/
//!@return void
void LTC6810_set_cfgr_dcc_0(uint8_t nIC, //!< Current IC
							cell_asic *ic, //!< A two dimensional array that will store the data 
							bool dcc_0 //!< the DCC bit
							);

/*!  Helper function to set the REFON bit*/
//!@return void
void LTC6810_set_cfgr_en_dtmen(uint8_t nIC, //!< Current IC
								cell_asic *ic, //!< A two dimensional array that will store the data 
								bool en_dtmen //!< Enable Discharge timer monitor
								);

/*!  Helper function to set the REFON bit*/
//!@return void
void LTC6810_set_cfgr_dis_red(uint8_t nIC, //!< Current IC
							  cell_asic *ic, //!< A two dimensional array that will store the data 
							  bool dis_red //!< Disable Digital Redundancy Check
							  );

/*!  Helper function to set the REFON bit*/
//!@return void
void LTC6810_set_cfgr_fdrf(uint8_t nIC, //!< Current IC
						   cell_asic *ic, //!< A two dimensional array that will store the data 
						   bool fdrf //!< Force digital Redundancy Failure
						   );

/*!  Helper function to set the REFON bit*/
//!@return void
void LTC6810_set_cfgr_sconv(uint8_t nIC, //!< Current IC
							cell_asic *ic, //!< A two dimensional array that will store the data 
							bool sconv //!< Enable Cell Measurement Redundancy using S Pin
							);

/*!  Helper function to set the REFON bit*/
//!@return void
void LTC6810_set_cfgr_dcto(uint8_t nIC, //!< Current IC
						   cell_asic *ic, //!< A two dimensional array that will store the data 
						   bool dcto[] //!<  Discharge Time Out Value
						   );
                         
void LTC6810_init_max_min(uint8_t total_ic, //!< number of ICs in the daisy chain  
                          cell_asic *ic, //!< A two dimensional array that will store the data 
                          cell_asic ic_max[],
                          cell_asic ic_min[]);

void LTC6810_max_min(uint8_t total_ic, 
			cell_asic ic_cells[],
             cell_asic ic_min[],
             cell_asic ic_max[],
             cell_asic ic_delta[]);
			 
/*!  Reads and parses the LTC681x S voltage registers*/
//!@return uint16_t, pec_error
uint8_t LTC6810_rds(uint8_t reg, //!< Controls which cell voltage register is read back.
                     uint8_t total_ic, //!< the number of ICs in the system
                     cell_asic *ic //!< Array of the parsed cell codes
                    );
					
/*!  Reads the raw S voltage register data*/
//!@return void
void LTC6810_rds_reg(uint8_t reg, //!< Determines which cell voltage register is read back
                      uint8_t total_ic, //!< The number of ICs in the
                      uint8_t *data //!< An array of the unparsed cell codes
                     );
					 
/*!  Reads Serial ID registers group*/
//!@return uint16_t, pec_error
uint8_t LTC6810_rdsid(uint8_t total_ic, //!< the number of ICs in the system
                     cell_asic *ic //!< Array of the parsed cell codes
                    );		 			 
			 
/*!  Mutes the LTC6813 discharge transistors*/
//!@return void
void LTC6810_mute();

/*! Clears the LTC6813 Mute Discharge*/
//!@return void
void LTC6810_unmute();

#endif
