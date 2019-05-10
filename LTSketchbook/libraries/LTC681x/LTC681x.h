/*! General BMS Library
*******************************************************************************
*   @file     LTC681x.h  
*   @author BMS (bms.support@analog.com)

********************************************************************************
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
    Library for LTC681x Multi-cell Battery Monitor
*/

#ifndef LTC681x_h
#define LTC681x_h
#include <stdint.h>
#include <Arduino.h>
#define LINDUINO

#ifdef MBED
#include "mbed.h"//remove when using with LINDUINO
#endif

#ifdef LINDUINO
#include <Arduino.h>
#endif

#define IC_LTC6813

#define MD_422HZ_1KHZ 0
#define MD_27KHZ_14KHZ 1
#define MD_7KHZ_3KHZ 2
#define MD_26HZ_2KHZ 3

#define ADC_OPT_ENABLED 1
#define ADC_OPT_DISABLED 0

#define CELL_CH_ALL 0
#define CELL_CH_1and7 1
#define CELL_CH_2and8 2
#define CELL_CH_3and9 3
#define CELL_CH_4and10 4
#define CELL_CH_5and11 5
#define CELL_CH_6and12 6

#define SELFTEST_1 1
#define SELFTEST_2 2

#define AUX_CH_ALL 0
#define AUX_CH_GPIO1 1
#define AUX_CH_GPIO2 2
#define AUX_CH_GPIO3 3
#define AUX_CH_GPIO4 4
#define AUX_CH_GPIO5 5
#define AUX_CH_VREF2 6

#define STAT_CH_ALL 0
#define STAT_CH_SOC 1
#define STAT_CH_ITEMP 2
#define STAT_CH_VREGA 3
#define STAT_CH_VREGD 4

#define REG_ALL 0
#define REG_1 1
#define REG_2 2
#define REG_3 3
#define REG_4 4
#define REG_5 5
#define REG_6 6

#define DCP_DISABLED 0
#define DCP_ENABLED 1

#define PULL_UP_CURRENT 1
#define PULL_DOWN_CURRENT 0

#define NUM_RX_BYT 8
#define CELL 1
#define AUX 2
#define STAT 3
#define CFGR 0
#define CFGRB 4
#define CS_PIN 10

/*! Cell Voltage data structure. */
typedef struct
{
  uint16_t c_codes[18]; //!< Cell Voltage Codes
  uint8_t pec_match[6]; //!< If a PEC error was detected during most recent read cmd
} cv;

/*! AUX Reg Voltage Data structure */
typedef struct
{
  uint16_t a_codes[9]; //!< Aux Voltage Codes
  uint8_t pec_match[4]; //!< If a PEC error was detected during most recent read cmd
} ax;

/*! Status Reg data structure. */
typedef struct
{
  uint16_t stat_codes[4]; //!< Status codes.
  uint8_t flags[3]; //!< Byte array that contains the uv/ov flag data
  uint8_t mux_fail[1]; //!< Mux self test status flag
  uint8_t thsd[1]; //!< Thermal shutdown status
  uint8_t pec_match[2]; //!< If a PEC error was detected during most recent read cmd
} st;

/*! IC register structure. */
typedef struct
{
  uint8_t tx_data[6];  //!< Stores data to be transmitted 
  uint8_t rx_data[8];  //!< Stores received data 
  uint8_t rx_pec_match; //!< If a PEC error was detected during most recent read cmd
} ic_register;

/*! PEC error counter structure. */
typedef struct
{
  uint16_t pec_count; //!< Overall PEC error count
  uint16_t cfgr_pec;  //!< Configuration register data PEC error count
  uint16_t cell_pec[6]; //!< Cell voltage register data PEC error count
  uint16_t aux_pec[4];  //!< Aux register data PEC error count
  uint16_t stat_pec[2]; //!< Status register data PEC error count
} pec_counter;

/*! Register configuration structure */
typedef struct
{
  uint8_t cell_channels; //!< Number of Cell channels
  uint8_t stat_channels; //!< Number of Stat channels
  uint8_t aux_channels;  //!< Number of Aux channels
  uint8_t num_cv_reg;    //!< Number of Cell voltage register
  uint8_t num_gpio_reg;  //!< Number of Aux register
  uint8_t num_stat_reg;  //!< Number of  Status register
} register_cfg;

/*! Cell variable structure */
typedef struct
{
  ic_register config;
  ic_register configb;
  cv  cells;
  ax  aux;
  st  stat;
  ic_register com;
  ic_register pwm;
  ic_register pwmb;
  ic_register sctrl;
  ic_register sctrlb;
  uint8_t sid[6];
  bool isospi_reverse;
  pec_counter crc_count;
  register_cfg ic_reg;
  long system_open_wire;
} cell_asic;

/*!
 Wake isoSPI up from IDlE state and enters the READY state
 @return void
 */
void wakeup_idle(uint8_t total_ic);//!< Number of ICs in the daisy chain

/*!
 Wake the LTC681x from the sleep state 
 @return void  
 */
void wakeup_sleep(uint8_t total_ic); //!< Number of ICs in the daisy chain

/*!
 Sends a command to the BMS IC. This code will calculate the PEC code for the transmitted command
 @return void  
 */
void cmd_68(uint8_t tx_cmd[2]); //!< 2 byte array containing the BMS command to be sent

/*!
 Writes an array of data to the daisy chain
 @return void  
 */
void write_68(uint8_t total_ic , //!< Number of ICs in the daisy chain
              uint8_t tx_cmd[2], //!< 2 byte array containing the BMS command to be sent
              uint8_t data[] //!< Array containing the data to be written to the BMS ICs
             );
			 
/*!
 Issues a command onto the daisy chain and reads back 6*total_ic data in the rx_data array
 @return int8_t, PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC  
 */
int8_t read_68( uint8_t total_ic, //!< Number of ICs in the daisy chain
                uint8_t tx_cmd[2], //!< 2 byte array containing the BMS command to be sent
                uint8_t *rx_data); //!< Array that the read back data will be stored in.
				
/*!
 Calculates  and returns the CRC15
 @returns The calculated pec15 as an unsigned int
  */
uint16_t pec15_calc(uint8_t len, //!< The length of the data array being passed to the function
                    uint8_t *data //!< The array of data that the PEC will be generated from
                   );
				   
/*!
 Write the LTC681x CFGRA register
 This command will write the configuration registers of the LTC681xs connected in a daisy chain stack. 
 The configuration is written in descending order so the last device's configuration is written first.
 @return void	 
 */
void LTC681x_wrcfg(uint8_t total_ic, //!< The number of ICs being written to
                   cell_asic *ic //!< A two dimensional array of the configuration data that will be written
                  );
				  
/*!
 Write the LTC681x CFGRB register
 This command will write the configuration registers of the LTC681xs connected in a daisy chain stack. 
 The configuration is written in descending order so the last device's configuration is written first.
 @return void	 
 */
void LTC681x_wrcfgb(uint8_t total_ic, //!< The number of ICs being written to
                    cell_asic *ic //!< A two dimensional array of the configuration data that will be written
                   );
				   
/*!
 Reads the LTC681x CFGRA register 
 @return int8_t, PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC 
 */
int8_t LTC681x_rdcfg(uint8_t total_ic, //!< Number of ICs in the system
                     cell_asic *ic //!< A two dimensional array that the function stores the read configuration data.
                    );

/*!
 Reads the LTC681x CFGRB register 
 @return int8_t, PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC 
 */
int8_t LTC681x_rdcfgb(uint8_t total_ic, //!< Number of ICs in the system
                      cell_asic *ic //!< A two dimensional array that the function stores the read configuration data.
                     );		   

/*!
 Starts cell voltage conversion
 Starts ADC conversions of the LTC681x Cpin inputs.
 The type of ADC conversion executed can be changed by setting the following parameters:
 @return void 
 */
void LTC681x_adcv(uint8_t MD, //!< ADC conversion Mode
                  uint8_t DCP, //!< Controls if Discharge is permitted during conversion
                  uint8_t CH //!< Sets which Cell channels are converted
                 );

/*!
 Start a GPIO and Vref2 Conversion
 @return void 
 */
void LTC681x_adax( uint8_t MD, //!< ADC Conversion Mode
				  uint8_t CHG //!< Sets which GPIO channels are converted
				);				 
				 
/*!
 Start a Status ADC Conversion
 @return void
 */ 
void LTC681x_adstat(uint8_t MD, //!< ADC Conversion Mode
					  uint8_t CHST //!< Sets which Stat channels are converted
					);

/*!
 Starts cell voltage  and GPIO 1&2 conversion
 @return void 
 */ 
void LTC681x_adcvax( uint8_t MD, //!< ADC Conversion Mode
					  uint8_t DCP //!< Controls if Discharge is permitted during conversion
					);					
					
/*!
 Starts cell voltage and SOC conversion
 @return void  
 */
void LTC681x_adcvsc(uint8_t MD, //!< ADC Conversion Mode
					  uint8_t DCP //!< Controls if Discharge is permitted during conversion
					);

/*!
 Reads and parses the LTC681x cell voltage registers.
 The function is used to read the cell codes of the LTC681x.
 This function will send the requested read commands parse the data and store the cell voltages in the cell_asic structure.
 @return uint8_t, PEC Status.
  0: No PEC error detected
 -1: PEC error detected, retry read 
 */
uint8_t LTC681x_rdcv(uint8_t reg, //!< Controls which cell voltage register is read back.
                     uint8_t total_ic, //!< The number of ICs in the system
                     cell_asic *ic //!< Array of the parsed cell codes
                    );

/*! 
 Reads and parses the LTC681x auxiliary registers.
 The function is used to read the  parsed GPIO codes of the LTC681x. 
 This function will send the requested read commands parse the data and store the gpio voltages in the cell_asic structure.
 @return  int8_t, PEC Status
  0: No PEC error detected
 -1: PEC error detected, retry read 
  */
int8_t LTC681x_rdaux(uint8_t reg, //!< Determines which GPIO voltage register is read back.
                     uint8_t total_ic,//!< the number of ICs in the system
                     cell_asic *ic//!<  Array of the parsed aux codes
                    );

/*! 
 Reads and parses the LTC681x stat registers.
 The function is used to read the  parsed status codes of the LTC681x. 
 This function will send the requested read commands parse the data and store the status voltages in the cell_asic structure
 @return  int8_t, PEC Status
  0: No PEC error detected
 -1: PEC error detected, retry read 
 */
int8_t LTC681x_rdstat(uint8_t reg, //!< Determines which Stat  register is read back.
                      uint8_t total_ic,//!< The number of ICs in the system
                      cell_asic *ic//!< Array of the parsed stat codes
                     );

/*! 
 Reads the raw cell voltage register data
 @return void 
 */				   
void LTC681x_rdcv_reg(uint8_t reg, //!< Determines which cell voltage register is read back
                      uint8_t total_ic, //!< The number of ICs in the
                      uint8_t *data //!< An array of the unparsed cell codes
                     );				   

/*! 
 Read the raw data from the LTC681x auxiliary register
 The function reads a single GPIO voltage register and stores the read data in the *data point as a byte array. 
 This function is rarely used outside of the LTC681x_rdaux() command.
 @return void 
 */	
void LTC681x_rdaux_reg(  uint8_t reg, //!< Determines which GPIO voltage register is read back
                         uint8_t total_ic, //!< The number of ICs in the system
                         uint8_t *data //!< Array of the unparsed auxiliary codes
                      );
					  
/*! 
 Read the raw data from the LTC681x stat register
 The function reads a single Status register and stores the read data in the *data point as a byte array. 
 This function is rarely used outside of the LTC681x_rdstat() command.
 @return void 
 */	
void LTC681x_rdstat_reg(uint8_t reg, //!< Determines which stat register is read back
                        uint8_t total_ic, //!< The number of ICs in the system
                        uint8_t *data //!< Array of the unparsed stat codes
                       );

/*! 
 Helper function that parses voltage measurement registers
 @return int8_t, pec_error PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC 
 */
int8_t parse_cells(uint8_t current_ic, //!< Current IC
                   uint8_t cell_reg, //!< Type of register
                   uint8_t cell_data[], //!< Unparsed data
                   uint16_t *cell_codes, //!< Parsed data
                   uint8_t *ic_pec //!< PEC error
				   );

/*! 
 Sends the poll ADC command
 @returns uint8_t adc_state 1 byte read back after a pladc command. If the byte is not 0xFF ADC conversion has completed
 */
uint8_t LTC681x_pladc();

/*! 
  This function will block operation until the ADC has finished it's conversion
  @returns uint32_t counter The approximate time it took for the ADC function to complete.
  */
uint32_t LTC681x_pollAdc();

/*! 
 Clears the LTC681x Cell voltage registers
 The command clears the cell voltage registers and initializes all values to 1.
 The register will read back hexadecimal 0xFF after the command is sent.
 @return void 
 */	
void LTC681x_clrcell();

/*! 
 Clears the LTC681x Auxiliary registers
 The command clears the Auxiliary registers and initializes all values to 1. 
 The register will read back hexadecimal 0xFF after the command is sent.
 @return void 
 */	
void LTC681x_clraux();

/*! 
 Clears the LTC681x Stat registers
 The command clears the Stat registers and initializes all values to 1. 
 The register will read back hexadecimal 0xFF after the command is sent.
 @return void 
 */	
void LTC681x_clrstat();

/*! 
 Starts the Mux Decoder diagnostic self test
 Running this command will start the Mux Decoder Diagnostic Self Test
 This test takes roughly 1ms to complete. The MUXFAIL bit will be updated,
 the bit will be set to 1 for a failure and 0 if the test has been passed.
 @return void 
 */
void LTC681x_diagn();
				   
/*!
 Starts cell voltage self test conversion
 @return void  
 */
void LTC681x_cvst(uint8_t MD, //!< ADC Conversion Mode
				  uint8_t ST //!<  Sets if self test 1 or 2 is run
				);				

/*! 
 Start an Auxiliary Register Self Test Conversion
 @return void  
 */
void LTC681x_axst(uint8_t MD, //!< ADC Conversion Mode
				  uint8_t ST //!< Sets if self test 1 or 2 is run
				);

/*! 
 Start a Status Register Self Test Conversion
 @return void  
 */
void LTC681x_statst( uint8_t MD, //!< ADC Conversion Mode
					  uint8_t ST //!< Sets if self test 1 or 2 is run
					);	
				
/*!
 Starts cell voltage overlap conversion
 @return void 
 */
void LTC681x_adol(uint8_t MD, //!< ADC Conversion Mode
				  uint8_t DCP //!< Discharge permitted during conversion
				);
				
/*!
 Start an GPIO Redundancy test
 @return void  
 */
void LTC681x_adaxd(uint8_t MD, //!< ADC Conversion Mode
				  uint8_t CHG //!< Sets which GPIO channels are converted
				);

/*!
 Start a Status register redundancy test Conversion
 @return void  
 */
void LTC681x_adstatd(uint8_t MD, //!< ADC Mode
					  uint8_t CHST //!< Sets which Status channels are converted
					);	
					
/*!
 Helper function that runs the ADC Self Tests 
 @return int16_t, error   Number of errors detected. 
 */
int16_t LTC681x_run_cell_adc_st(uint8_t adc_reg, //!< Type of register
                                uint8_t total_ic, //!< Number of ICs in the daisy chain
                                cell_asic *ic, //!< A two dimensional array that will store the data
								uint8_t md, //!< ADC Mode
								bool adcopt //!< ADCOPT bit in the configuration register
								);

/*!
 Self Test Helper Function 
 @return uint16_t test_pattern returns the register data pattern for a given ADC MD and Self test 
 */
uint16_t LTC681x_st_lookup( uint8_t MD, //!< ADC Mode
						  uint8_t ST, //!< Self Test
						  bool adcopt //!< ADCOPT bit in the configuration register
						);	

/*!
 Helper Function that runs the ADC Overlap test 
 @return uint16_t, error
  0: Pass
 -1: False, Error detected 
 */
uint16_t LTC681x_run_adc_overlap(uint8_t total_ic, //!< Number of ICs in the daisy chain
                                 cell_asic *ic //!< A two dimensional array that will store the data
								 );						
						
/*!
 Helper function that runs the ADC Digital Redundancy commands and checks output for errors
 @return int16_t, error   Number of errors detected.  
 */
int16_t LTC681x_run_adc_redundancy_st(uint8_t adc_mode, //!< ADC Mode
                                      uint8_t adc_reg, //!< Type of register
                                      uint8_t total_ic, //!< Number of ICs in the daisy chain
                                      cell_asic *ic //!< A two dimensional array that will store the data
									  );					

/*!
 Start an open wire Conversion 
 @return void  
 */
void LTC681x_adow(uint8_t MD, //!< ADC Conversion Mode
				  uint8_t PUP, //!< Pull up/Pull down current
				  uint8_t CH, //!< Channels
				  uint8_t DCP//!< Discharge Permit
				);

/*!
 Start GPIOs open wire ADC conversion
 @return void 
 */
void LTC681x_axow(uint8_t MD, //!< ADC Mode
				  uint8_t PUP //!<Pull up/Pull down current
				 );
				 
/*!
 Helper function that runs the data sheet algorithm for open wire for single cell detection
 @return void 
 */	
void LTC681x_run_openwire_single(uint8_t total_ic, //!< Number of ICs in the daisy chain
								cell_asic *ic //!< A two dimensional array that will store the data
								);
								
/*!
 Helper function that runs open wire for multiple cell and two consecutive cells detection
 @return void	 
 */
 void LTC681x_run_openwire_multi(uint8_t total_ic, //!< Number of ICs in the daisy chain
						         cell_asic *ic //!< A two dimensional array that will store the data
						        );								
				 
/*!
 Runs open wire for GPIOs
 @return void	 
 */
void LTC681x_run_gpio_openwire(uint8_t total_ic, //!< Number of ICs in the daisy chain
								cell_asic *ic //!< A two dimensional array that will store the data
								);								

/*!
 Helper Function to clear DCC bits in the CFGR Registers 
 @return void	 
 */
void LTC681x_clear_discharge(uint8_t total_ic,//!< Number of ICs in the daisy chain
                             cell_asic *ic //!< A two dimensional array that will store the data
							 );

/*!
 Write the LTC681x PWM register
 This command will write the pwm registers of the LTC681x connected in a daisy chain stack. 
 The pwm is written in descending order so the last device's pwm is written first. 
 @return void	 
 */
void LTC681x_wrpwm(uint8_t total_ic, //!< The number of ICs being written to
                   uint8_t pwmReg,  //!< The PWM Register to be written
                   cell_asic *ic //!< A two dimensional array that will store the data to be written 
                  );

/*!
  Reads pwm registers of a LTC681x daisy chain
  @return int8_t, pec_error PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC 
 */
int8_t LTC681x_rdpwm(uint8_t total_ic, //!< Number of ICs in the system
                     uint8_t pwmReg, //!< The PWM Register to be written A or B
                     cell_asic *ic //!< A two dimensional array that will store the read data
                    );
					
/*!
 Write the LTC681x Sctrl register
 @return void	 
 */ 					 
void LTC681x_wrsctrl(uint8_t total_ic, //!< Number of ICs in the daisy chain
                     uint8_t sctrl_reg, //!< The Sctrl Register to be written A or B
                     cell_asic *ic //!< A two dimensional array that will store the data to be written 
                    );                  

/*!
 Reads sctrl registers of a LTC681x daisy chain    
 @return int8_t, pec_error PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC	 
 */				
int8_t LTC681x_rdsctrl(uint8_t total_ic, //!< Number of ICs in the daisy chain
                       uint8_t sctrl_reg, //!< The Sctrl Register to be written A or B
                       cell_asic *ic //!< A two dimensional array that the function stores the read data
                      );					 

/*!
 Start Sctrl data communication       
 This command will start the sctrl pulse communication over the spins	
 @return void	 
 */				  
void LTC681x_stsctrl();

/*! 
 Clears the LTC681x SCTRL registers
 The command clears the SCTRL registers and initializes all values to 0. 
 The register will read back hexadecimal 0x00 after the command is sent.
 @return void	
 */
void LTC681x_clrsctrl();
								
/*!
 Write the LTC681x COMM register
 This command will write the comm registers of the LTC681x connected in a daisy chain stack. 
 The comm is written in descending order so the last device's configuration is written first.
 @return void	 
 */
void LTC681x_wrcomm(uint8_t total_ic, //!<  The number of ICs being written to
                    cell_asic *ic //!< A two dimensional array that will store the data to be written 
                   );
				   
/*!
 Reads comm registers of a LTC681x daisy chain
 @return int8_t, pec_error PEC Status.
  0: Data read back has matching PEC
 -1: Data read back has incorrect PEC 
 */
int8_t LTC681x_rdcomm(uint8_t total_ic, //!< Number of ICs in the system
                      cell_asic *ic //!< A two dimensional array that the function stores the read data
                     );

/*!
 Issues a stcomm command and clocks data out of the COMM register  
 @return void	 
 */
void LTC681x_stcomm(uint8_t len //!< Length of data to be transmitted 
					);				 
								 
/*!
 Helper Function that counts overall PEC errors and register/IC PEC errors
 @return void	 
 */ 
void LTC681x_check_pec(uint8_t total_ic, //!< Number of ICs in the daisy chain
                       uint8_t reg, //!< Type of register
                       cell_asic *ic //!< A two dimensional array that will store the data
					   );

/*!
 Helper Function that resets the PEC error counters
 @return void	 
 */  
void LTC681x_reset_crc_count(uint8_t total_ic, //!< Number of ICs in the daisy chain
                             cell_asic *ic //!< A two dimensional array that will store the data
							 );
							 								
/*!
 Helper Function to initialize the CFGR data structures 
 @return void 
 */
void LTC681x_init_cfg(uint8_t total_ic, //!< Number of ICs in the daisy chain
                      cell_asic *ic //!< A two dimensional array that will store the data
					  );

/*!
 Helper function to set appropriate bits in CFGR register based on bit function
 @return void  
 */
void LTC681x_set_cfgr(uint8_t nIC, //!< Current IC
                      cell_asic *ic, //!< A two dimensional array that will store the data
                      bool refon,  //!< The REFON bit
                      bool adcopt, //!< The ADCOPT bit
                      bool gpio[5],//!< The GPIO bits
                      bool dcc[12],//!< The DCC bits 
					  bool dcto[4],//!< The Dcto bits
					  uint16_t uv, //!< The UV value
					  uint16_t ov  //!< The OV value
					  );

/*!
 Helper function to turn the REFON bit HIGH or LOW 
 @return void  
 */
void LTC681x_set_cfgr_refon(uint8_t nIC, //!< Current IC
                            cell_asic *ic, //!< A two dimensional array that will store the data
                            bool refon //!< The REFON bit
							);

/*!
 Helper function to turn the ADCOPT bit HIGH or LOW 
 @return void  
 */
void LTC681x_set_cfgr_adcopt(uint8_t nIC, //!< Current IC
                             cell_asic *ic, //!< A two dimensional array that will store the data
                             bool adcopt //!< The ADCOPT bit
							 );

/*!
 Helper function to turn the GPIO bits HIGH or LOW 
 @return void 
 */
void LTC681x_set_cfgr_gpio(uint8_t nIC, //!< Current IC
                           cell_asic *ic, //!< A two dimensional array that will store the data
                           bool gpio[] //!< The GPIO bits
						   );

/*!
 Helper function to turn the DCC bits HIGH or LOW 
 @return void  
 */
void LTC681x_set_cfgr_dis(uint8_t nIC, //!< Current IC
                          cell_asic *ic, //!< A two dimensional array that will store the data
                          bool dcc[] //!< The DCC bits 
						  );
						  
/*!
 Helper function to control discharge time value
 @return void 
 */
void LTC681x_set_cfgr_dcto(uint8_t nIC,  //!< Current IC
						   cell_asic *ic, //!< A two dimensional array that will store the data
						   bool dcto[] //!< The Dcto bits
						   );

/*!
 Helper function to set uv field in CFGRA register
 @return void  
 */
void LTC681x_set_cfgr_uv(uint8_t nIC, //!< Current IC
                         cell_asic *ic, //!< A two dimensional array that will store the data
                         uint16_t uv //!< The UV value
						 );

/*!
 Helper function to set ov field in CFGRA register
 @return void   
 */
void LTC681x_set_cfgr_ov(uint8_t nIC, //!< Current IC
                         cell_asic *ic, //!< A two dimensional array that will store the data
                         uint16_t ov //!< The OV value
						 );		
						 
#ifdef MBED
//This needs a PROGMEM =  when using with a LINDUINO
const uint16_t crc15Table[256] {0x0,0xc599, 0xceab, 0xb32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac,  // precomputed CRC15 Table
                                0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa, 0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1,
                                0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d, 0x5b2e,
                                0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b,
                                0x77e6, 0xb27f, 0xb94d, 0x7cd4, 0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd,
                                0x2544, 0x2be, 0xc727, 0xcc15, 0x98c, 0xda71, 0x1fe8, 0x14da, 0xd143, 0xf3c5, 0x365c,
                                0x3d6e, 0xf8f7,0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x7c2, 0xc25b, 0xc969, 0xcf0, 0xdf0d,
                                0x1a94, 0x11a6, 0xd43f, 0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,
                                0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8, 0xa8eb, 0x6d72, 0x6640,
                                0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba,
                                0x4a88, 0x8f11, 0x57c, 0xc0e5, 0xcbd7, 0xe4e, 0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b,
                                0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286, 0xa213, 0x678a, 0x6cb8, 0xa921,
                                0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070,
                                0x85e9, 0xf84, 0xca1d, 0xc12f, 0x4b6, 0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a, 0x3528,
                                0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e, 0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59,
                                0x2ac0, 0xd3a, 0xc8a3, 0xc391, 0x608, 0xd5f5, 0x106c, 0x1b5e, 0xdec7, 0x54aa, 0x9133, 0x9a01,
                                0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9,
                                0x7350, 0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a,
                                0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c, 0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25,
                                0x2fbc, 0x846, 0xcddf, 0xc6ed, 0x374, 0xd089, 0x1510, 0x1e22, 0xdbbb, 0xaf8, 0xcf61, 0xc453,
                                0x1ca, 0xd237, 0x17ae, 0x1c9c, 0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b,
                                0x2d02, 0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3,
                                0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095
                               };

#else
const uint16_t crc15Table[256] PROGMEM =  {0x0,0xc599, 0xceab, 0xb32, 0xd8cf, 0x1d56, 0x1664, 0xd3fd, 0xf407, 0x319e, 0x3aac,  // precomputed CRC15 Table
                               0xff35, 0x2cc8, 0xe951, 0xe263, 0x27fa, 0xad97, 0x680e, 0x633c, 0xa6a5, 0x7558, 0xb0c1,
                               0xbbf3, 0x7e6a, 0x5990, 0x9c09, 0x973b, 0x52a2, 0x815f, 0x44c6, 0x4ff4, 0x8a6d, 0x5b2e,
                               0x9eb7, 0x9585, 0x501c, 0x83e1, 0x4678, 0x4d4a, 0x88d3, 0xaf29, 0x6ab0, 0x6182, 0xa41b,
                               0x77e6, 0xb27f, 0xb94d, 0x7cd4, 0xf6b9, 0x3320, 0x3812, 0xfd8b, 0x2e76, 0xebef, 0xe0dd,
                               0x2544, 0x2be, 0xc727, 0xcc15, 0x98c, 0xda71, 0x1fe8, 0x14da, 0xd143, 0xf3c5, 0x365c,
                               0x3d6e, 0xf8f7,0x2b0a, 0xee93, 0xe5a1, 0x2038, 0x7c2, 0xc25b, 0xc969, 0xcf0, 0xdf0d,
                               0x1a94, 0x11a6, 0xd43f, 0x5e52, 0x9bcb, 0x90f9, 0x5560, 0x869d, 0x4304, 0x4836, 0x8daf,
                               0xaa55, 0x6fcc, 0x64fe, 0xa167, 0x729a, 0xb703, 0xbc31, 0x79a8, 0xa8eb, 0x6d72, 0x6640,
                               0xa3d9, 0x7024, 0xb5bd, 0xbe8f, 0x7b16, 0x5cec, 0x9975, 0x9247, 0x57de, 0x8423, 0x41ba,
                               0x4a88, 0x8f11, 0x57c, 0xc0e5, 0xcbd7, 0xe4e, 0xddb3, 0x182a, 0x1318, 0xd681, 0xf17b,
                               0x34e2, 0x3fd0, 0xfa49, 0x29b4, 0xec2d, 0xe71f, 0x2286, 0xa213, 0x678a, 0x6cb8, 0xa921,
                               0x7adc, 0xbf45, 0xb477, 0x71ee, 0x5614, 0x938d, 0x98bf, 0x5d26, 0x8edb, 0x4b42, 0x4070,
                               0x85e9, 0xf84, 0xca1d, 0xc12f, 0x4b6, 0xd74b, 0x12d2, 0x19e0, 0xdc79, 0xfb83, 0x3e1a, 0x3528,
                               0xf0b1, 0x234c, 0xe6d5, 0xede7, 0x287e, 0xf93d, 0x3ca4, 0x3796, 0xf20f, 0x21f2, 0xe46b, 0xef59,
                               0x2ac0, 0xd3a, 0xc8a3, 0xc391, 0x608, 0xd5f5, 0x106c, 0x1b5e, 0xdec7, 0x54aa, 0x9133, 0x9a01,
                               0x5f98, 0x8c65, 0x49fc, 0x42ce, 0x8757, 0xa0ad, 0x6534, 0x6e06, 0xab9f, 0x7862, 0xbdfb, 0xb6c9,
                               0x7350, 0x51d6, 0x944f, 0x9f7d, 0x5ae4, 0x8919, 0x4c80, 0x47b2, 0x822b, 0xa5d1, 0x6048, 0x6b7a,
                               0xaee3, 0x7d1e, 0xb887, 0xb3b5, 0x762c, 0xfc41, 0x39d8, 0x32ea, 0xf773, 0x248e, 0xe117, 0xea25,
                               0x2fbc, 0x846, 0xcddf, 0xc6ed, 0x374, 0xd089, 0x1510, 0x1e22, 0xdbbb, 0xaf8, 0xcf61, 0xc453,
                               0x1ca, 0xd237, 0x17ae, 0x1c9c, 0xd905, 0xfeff, 0x3b66, 0x3054, 0xf5cd, 0x2630, 0xe3a9, 0xe89b,
                               0x2d02, 0xa76f, 0x62f6, 0x69c4, 0xac5d, 0x7fa0, 0xba39, 0xb10b, 0x7492, 0x5368, 0x96f1, 0x9dc3,
                               0x585a, 0x8ba7, 0x4e3e, 0x450c, 0x8095
                               };
     #endif
#endif
