/*!
  General BMS Library
@verbatim

@endverbatim
REVISION HISTORY
$Revision: 7139 $
$Date: 2017-4

Copyright (c) 2017, Linear Technology Corp.(LTC)
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of Linear Technology Corp.

The Linear Technology Linduino is not affiliated with the official Arduino team.
However, the Linduino is only possible because of the Arduino team's commitment
to the open-source community.  Please, visit http://www.arduino.cc and
http://store.arduino.cc , and consider a purchase that will help fund their
ongoing work.

Copyright 2017 Linear Technology Corp. (LTC)
***********************************************************/
#include <stdint.h>
#include "LTC681x.h"
#include "bms_hardware.h"


#ifdef LINDUINO
#include <Arduino.h>
#endif


void wakeup_idle(uint8_t total_ic)
{
  for (int i =0; i<total_ic; i++)
  {
    cs_low(CS_PIN);
    //delayMicroseconds(2); //Guarantees the isoSPI will be in ready mode
    spi_read_byte(0xff);
    cs_high(CS_PIN);
  }
}

//Generic wakeup commannd to wake the LTC6813 from sleep
void wakeup_sleep(uint8_t total_ic)
{
  for (int i =0; i<total_ic; i++)
  {
    cs_low(CS_PIN);
    delay_u(300); // Guarantees the LTC6813 will be in standby
    cs_high(CS_PIN);
    delay_u(10);
  }
}

//Generic function to write 68xx commands. Function calculated PEC for tx_cmd data
void cmd_68(uint8_t tx_cmd[2])
{
  uint8_t cmd[4];
  uint16_t cmd_pec;
  uint8_t md_bits;

  cmd[0] = tx_cmd[0];
  cmd[1] =  tx_cmd[1];
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
  cs_low(CS_PIN);
  spi_write_array(4,cmd);
  cs_high(CS_PIN);
}

//Generic function to write 68xx commands and write payload data. Function calculated PEC for tx_cmd data
void write_68(uint8_t total_ic , uint8_t tx_cmd[2], uint8_t data[])
{
  const uint8_t BYTES_IN_REG = 6;
  const uint8_t CMD_LEN = 4+(8*total_ic);
  uint8_t *cmd;
  uint16_t data_pec;
  uint16_t cmd_pec;
  uint8_t cmd_index;

  cmd = (uint8_t *)malloc(CMD_LEN*sizeof(uint8_t));
  cmd[0] = tx_cmd[0];
  cmd[1] = tx_cmd[1];
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);
  cmd_index = 4;
  for (uint8_t current_ic = total_ic; current_ic > 0; current_ic--)       // executes for each LTC681x in daisy chain, this loops starts with
  {
    // the last IC on the stack. The first configuration written is
    // received by the last IC in the daisy chain

    for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
    {
      cmd[cmd_index] = data[((current_ic-1)*6)+current_byte];
      cmd_index = cmd_index + 1;
    }

    data_pec = (uint16_t)pec15_calc(BYTES_IN_REG, &data[(current_ic-1)*6]);    // calculating the PEC for each Iss configuration register data
    cmd[cmd_index] = (uint8_t)(data_pec >> 8);
    cmd[cmd_index + 1] = (uint8_t)data_pec;
    cmd_index = cmd_index + 2;
  }


  cs_low(CS_PIN);
  spi_write_array(CMD_LEN, cmd);
  cs_high(CS_PIN);
  free(cmd);
}

//Generic function to write 68xx commands and read data. Function calculated PEC for tx_cmd data
int8_t read_68( uint8_t total_ic, uint8_t tx_cmd[2], uint8_t *rx_data)
{
  const uint8_t BYTES_IN_REG = 8;
  uint8_t cmd[4];
  uint8_t data[256];
  int8_t pec_error = 0;
  uint16_t cmd_pec;
  uint16_t data_pec;
  uint16_t received_pec;

  // data = (uint8_t *) malloc((8*total_ic)*sizeof(uint8_t)); // This is a problem because it can fail

  cmd[0] = tx_cmd[0];
  cmd[1] = tx_cmd[1];
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);


  cs_low(CS_PIN);
  spi_write_read(cmd, 4, data, (BYTES_IN_REG*total_ic));         //Read the configuration data of all ICs on the daisy chain into
  cs_high(CS_PIN);                          //rx_data[] array

  for (uint8_t current_ic = 0; current_ic < total_ic; current_ic++)       //executes for each LTC681x in the daisy chain and packs the data
  {
    //into the r_comm array as well as check the received Config data
    //for any bit errors
    for (uint8_t current_byte = 0; current_byte < BYTES_IN_REG; current_byte++)
    {
      rx_data[(current_ic*8)+current_byte] = data[current_byte + (current_ic*BYTES_IN_REG)];
    }
    received_pec = (rx_data[(current_ic*8)+6]<<8) + rx_data[(current_ic*8)+7];
    data_pec = pec15_calc(6, &rx_data[current_ic*8]);
    if (received_pec != data_pec)
    {
      pec_error = -1;
    }
  }


  return(pec_error);
}


/*
  Calculates  and returns the CRC15
  */
uint16_t pec15_calc(uint8_t len, //Number of bytes that will be used to calculate a PEC
                    uint8_t *data //Array of data that will be used to calculate  a PEC
                   )
{
  uint16_t remainder,addr;

  remainder = 16;//initialize the PEC
  for (uint8_t i = 0; i<len; i++) // loops for each byte in data array
  {
    addr = ((remainder>>7)^data[i])&0xff;//calculate PEC table address
#ifdef MBED
    remainder = (remainder<<8)^crc15Table[addr];
#else
    remainder = (remainder<<8)^pgm_read_word_near(crc15Table+addr);
#endif
  }
  return(remainder*2);//The CRC15 has a 0 in the LSB so the remainder must be multiplied by 2
}

//Starts cell voltage conversion
void LTC681x_adcv(
  uint8_t MD, //ADC Mode
  uint8_t DCP, //Discharge Permit
  uint8_t CH //Cell Channels to be measured
)
{
  uint8_t cmd[4];
  uint8_t md_bits;

  md_bits = (MD & 0x02) >> 1;
  cmd[0] = md_bits + 0x02;
  md_bits = (MD & 0x01) << 7;
  cmd[1] =  md_bits + 0x60 + (DCP<<4) + CH;
  cmd_68(cmd);
}


//Starts cell voltage and SOC conversion
void LTC681x_adcvsc(
  uint8_t MD, //ADC Mode
  uint8_t DCP //Discharge Permit
)
{
  uint8_t cmd[4];
  uint8_t md_bits;
  md_bits = (MD & 0x02) >> 1;
  cmd[0] = md_bits | 0x04;
  md_bits = (MD & 0x01) << 7;
  cmd[1] =  md_bits | 0x60 | (DCP<<4) | 0x07;
  cmd_68(cmd);

}

// Starts cell voltage  and GPIO 1&2 conversion
void LTC681x_adcvax(
  uint8_t MD, //ADC Mode
  uint8_t DCP //Discharge Permit
)
{
  uint8_t cmd[4];
  uint8_t md_bits;
  md_bits = (MD & 0x02) >> 1;
  cmd[0] = md_bits | 0x04;
  md_bits = (MD & 0x01) << 7;
  cmd[1] =  md_bits | ((DCP&0x01)<<4) + 0x6F;
  cmd_68(cmd);
}

//Starts cell voltage overlap conversion
void LTC681x_adol(
  uint8_t MD, //ADC Mode
  uint8_t DCP //Discharge Permit
)
{
  uint8_t cmd[4];
  uint8_t md_bits;
  md_bits = (MD & 0x02) >> 1;
  cmd[0] = md_bits + 0x02;
  md_bits = (MD & 0x01) << 7;
  cmd[1] =  md_bits + (DCP<<4) +0x01;
  cmd_68(cmd);
}

//Starts cell voltage self test conversion
void LTC681x_cvst(
  uint8_t MD, //ADC Mode
  uint8_t ST //Self Test
)
{
  uint8_t cmd[2];
  uint8_t md_bits;

  md_bits = (MD & 0x02) >> 1;
  cmd[0] = md_bits + 0x02;
  md_bits = (MD & 0x01) << 7;
  cmd[1] =  md_bits + ((ST)<<5) +0x07;
  cmd_68(cmd);

}

//Start an Auxiliary Register Self Test Conversion
void LTC681x_axst(
  uint8_t MD, //ADC Mode
  uint8_t ST //Self Test
)
{
  uint8_t cmd[4];
  uint8_t md_bits;

  md_bits = (MD & 0x02) >> 1;
  cmd[0] = md_bits + 0x04;
  md_bits = (MD & 0x01) << 7;
  cmd[1] =  md_bits + ((ST&0x03)<<5) +0x07;
  cmd_68(cmd);

}

//Start a Status Register Self Test Conversion
void LTC681x_statst(
  uint8_t MD, //ADC Mode
  uint8_t ST //Self Test
)
{
  uint8_t cmd[2];
  uint8_t md_bits;

  md_bits = (MD & 0x02) >> 1;
  cmd[0] = md_bits + 0x04;
  md_bits = (MD & 0x01) << 7;
  cmd[1] =  md_bits + ((ST&0x03)<<5) +0x0F;
  cmd_68(cmd);

}

//Sends the poll adc command
uint8_t LTC681x_pladc()
{
  uint8_t cmd[4];
  uint8_t adc_state = 0xFF;
  uint16_t cmd_pec;

  cmd[0] = 0x07;
  cmd[1] = 0x14;
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);


  cs_low(CS_PIN);
  spi_write_array(4,cmd);
// adc_state = spi_read_byte(0xFF);

  cs_high(CS_PIN);
  return(adc_state);
}

//This function will block operation until the ADC has finished it's conversion
uint32_t LTC681x_pollAdc()
{
  uint32_t counter = 0;
  uint8_t finished = 0;
  uint8_t current_time = 0;
  uint8_t cmd[4];
  uint16_t cmd_pec;


  cmd[0] = 0x07;
  cmd[1] = 0x14;
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);

  cs_low(CS_PIN);
  spi_write_array(4,cmd);

  while ((counter<200000)&&(finished == 0))
  {
    current_time = spi_read_byte(0xff);
    if (current_time>0)
    {
      finished = 1;
    }
    else
    {
      counter = counter + 10;
    }
  }

  cs_high(CS_PIN);


  return(counter);
}

//Start a GPIO and Vref2 Conversion
void LTC681x_adax(
  uint8_t MD, //ADC Mode
  uint8_t CHG //GPIO Channels to be measured)
)
{
  uint8_t cmd[4];
  uint8_t md_bits;

  md_bits = (MD & 0x02) >> 1;
  cmd[0] = md_bits + 0x04;
  md_bits = (MD & 0x01) << 7;
  cmd[1] = md_bits + 0x60 + CHG ;
  cmd_68(cmd);

}

//Start an GPIO Redundancy test
void LTC681x_adaxd(
  uint8_t MD, //ADC Mode
  uint8_t CHG //GPIO Channels to be measured)
)
{
  uint8_t cmd[4];
  uint8_t md_bits;

  md_bits = (MD & 0x02) >> 1;
  cmd[0] = md_bits + 0x04;
  md_bits = (MD & 0x01) << 7;
  cmd[1] = md_bits + CHG ;
  cmd_68(cmd);
}

//Start a Status ADC Conversion
void LTC681x_adstat(
  uint8_t MD, //ADC Mode
  uint8_t CHST //GPIO Channels to be measured
)
{
  uint8_t cmd[4];
  uint8_t md_bits;

  md_bits = (MD & 0x02) >> 1;
  cmd[0] = md_bits + 0x04;
  md_bits = (MD & 0x01) << 7;
  cmd[1] = md_bits + 0x68 + CHST ;
  cmd_68(cmd);
}

// Start a Status register redundancy test Conversion
void LTC681x_adstatd(
  uint8_t MD, //ADC Mode
  uint8_t CHST //GPIO Channels to be measured
)
{
  uint8_t cmd[2];
  uint8_t md_bits;

  md_bits = (MD & 0x02) >> 1;
  cmd[0] = md_bits + 0x04;
  md_bits = (MD & 0x01) << 7;
  cmd[1] = md_bits + 0x08 + CHST ;
  cmd_68(cmd);

}


// Start an open wire Conversion
void LTC681x_adow(
  uint8_t MD, //ADC Mode
  uint8_t PUP //Discharge Permit
)
{
  uint8_t cmd[2];
  uint8_t md_bits;
  md_bits = (MD & 0x02) >> 1;
  cmd[0] = md_bits + 0x02;
  md_bits = (MD & 0x01) << 7;
  cmd[1] =  md_bits + 0x28 + (PUP<<6) ;//+ CH;
  cmd_68(cmd);
}

// Reads the raw cell voltage register data
void LTC681x_rdcv_reg(uint8_t reg, //Determines which cell voltage register is read back
                      uint8_t total_ic, //the number of ICs in the
                      uint8_t *data //An array of the unparsed cell codes
                     )
{
  const uint8_t REG_LEN = 8; //number of bytes in each ICs register + 2 bytes for the PEC
  uint8_t cmd[4];
  uint16_t cmd_pec;

  if (reg == 1)     //1: RDCVA
  {
    cmd[1] = 0x04;
    cmd[0] = 0x00;
  }
  else if (reg == 2) //2: RDCVB
  {
    cmd[1] = 0x06;
    cmd[0] = 0x00;
  }
  else if (reg == 3) //3: RDCVC
  {
    cmd[1] = 0x08;
    cmd[0] = 0x00;
  }
  else if (reg == 4) //4: RDCVD
  {
    cmd[1] = 0x0A;
    cmd[0] = 0x00;
  }
  else if (reg == 5) //4: RDCVE
  {
    cmd[1] = 0x09;
    cmd[0] = 0x00;
  }
  else if (reg == 6) //4: RDCVF
  {
    cmd[1] = 0x0B;
    cmd[0] = 0x00;
  }


  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);

  cs_low(CS_PIN);
  spi_write_read(cmd,4,data,(REG_LEN*total_ic));
  cs_high(CS_PIN);

}

//helper function that parses voltage measurement registers
int8_t parse_cells(uint8_t current_ic, uint8_t cell_reg, uint8_t cell_data[], uint16_t *cell_codes, uint8_t *ic_pec)
{

  const uint8_t BYT_IN_REG = 6;
  const uint8_t CELL_IN_REG = 3;
  int8_t pec_error = 0;
  uint16_t parsed_cell;
  uint16_t received_pec;
  uint16_t data_pec;
  uint8_t data_counter = current_ic*NUM_RX_BYT; //data counter


  for (uint8_t current_cell = 0; current_cell<CELL_IN_REG; current_cell++)  // This loop parses the read back data into cell voltages, it
  {
    // loops once for each of the 3 cell voltage codes in the register

    parsed_cell = cell_data[data_counter] + (cell_data[data_counter + 1] << 8);//Each cell code is received as two bytes and is combined to
    // create the parsed cell voltage code
    cell_codes[current_cell  + ((cell_reg - 1) * CELL_IN_REG)] = parsed_cell;
    data_counter = data_counter + 2;                       //Because cell voltage codes are two bytes the data counter
    //must increment by two for each parsed cell code
  }

  received_pec = (cell_data[data_counter] << 8) | cell_data[data_counter+1]; //The received PEC for the current_ic is transmitted as the 7th and 8th
  //after the 6 cell voltage data bytes
  data_pec = pec15_calc(BYT_IN_REG, &cell_data[(current_ic) * NUM_RX_BYT]);

  if (received_pec != data_pec)
  {
    pec_error = 1;                             //The pec_error variable is simply set negative if any PEC errors
    ic_pec[cell_reg-1]=1;
  }
  else
  {
    ic_pec[cell_reg-1]=0;
  }
  data_counter=data_counter+2;
  return(pec_error);
}

/*
The function reads a single GPIO voltage register and stores thre read data
in the *data point as a byte array. This function is rarely used outside of
the LTC6811_rdaux() command.
*/
void LTC681x_rdaux_reg(uint8_t reg, //Determines which GPIO voltage register is read back
                       uint8_t total_ic, //The number of ICs in the system
                       uint8_t *data //Array of the unparsed auxiliary codes
                      )
{
  const uint8_t REG_LEN = 8; // number of bytes in the register + 2 bytes for the PEC
  uint8_t cmd[4];
  uint16_t cmd_pec;


  if (reg == 1)     //Read back auxiliary group A
  {
    cmd[1] = 0x0C;
    cmd[0] = 0x00;
  }
  else if (reg == 2)  //Read back auxiliary group B
  {
    cmd[1] = 0x0e;
    cmd[0] = 0x00;
  }
  else if (reg == 3)  //Read back auxiliary group C
  {
    cmd[1] = 0x0D;
    cmd[0] = 0x00;
  }
  else if (reg == 4)  //Read back auxiliary group D
  {
    cmd[1] = 0x0F;
    cmd[0] = 0x00;
  }
  else          //Read back auxiliary group A
  {
    cmd[1] = 0x0C;
    cmd[0] = 0x00;
  }

  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);

  cs_low(CS_PIN);
  spi_write_read(cmd,4,data,(REG_LEN*total_ic));
  cs_high(CS_PIN);

}

/*
The function reads a single stat  register and stores the read data
in the *data point as a byte array. This function is rarely used outside of
the LTC6811_rdstat() command.
*/
void LTC681x_rdstat_reg(uint8_t reg, //Determines which stat register is read back
                        uint8_t total_ic, //The number of ICs in the system
                        uint8_t *data //Array of the unparsed stat codes
                       )
{
  const uint8_t REG_LEN = 8; // number of bytes in the register + 2 bytes for the PEC
  uint8_t cmd[4];
  uint16_t cmd_pec;


  if (reg == 1)     //Read back statiliary group A
  {
    cmd[1] = 0x10;
    cmd[0] = 0x00;
  }
  else if (reg == 2)  //Read back statiliary group B
  {
    cmd[1] = 0x12;
    cmd[0] = 0x00;
  }

  else          //Read back statiliary group A
  {
    cmd[1] = 0x10;
    cmd[0] = 0x00;
  }

  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);

  cs_low(CS_PIN);
  spi_write_read(cmd,4,data,(REG_LEN*total_ic));
  cs_high(CS_PIN);

}

/*
The command clears the cell voltage registers and intiallizes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.
*/
void LTC681x_clrcell()
{
  uint8_t cmd[2]= {0x07 , 0x11};
  cmd_68(cmd);
}


/*
The command clears the Auxiliary registers and initializes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.
*/
void LTC681x_clraux()
{
  uint8_t cmd[2]= {0x07 , 0x12};
  cmd_68(cmd);
}


/*
The command clears the Stat registers and intiallizes
all values to 1. The register will read back hexadecimal 0xFF
after the command is sent.

*/
void LTC681x_clrstat()
{
  uint8_t cmd[2]= {0x07 , 0x13};
  cmd_68(cmd);
}
/*
The command clears the Sctrl registers and initializes
all values to 0. The register will read back hexadecimal 0x00
after the command is sent.
*/
void LTC681x_clrsctrl()
{
  uint8_t cmd[2]= {0x00 , 0x18};
  cmd_68(cmd);
}
//Starts the Mux Decoder diagnostic self test
void LTC681x_diagn()
{
  uint8_t cmd[2] = {0x07 , 0x15};
  cmd_68(cmd);
}

//Reads and parses the LTC681x cell voltage registers.
uint8_t LTC681x_rdcv(uint8_t reg, // Controls which cell voltage register is read back.
                     uint8_t total_ic, // the number of ICs in the system
                     cell_asic ic[] // Array of the parsed cell codes
                    )
{
  int8_t pec_error = 0;
  uint8_t *cell_data;
  uint8_t c_ic = 0;
  cell_data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));

  if (reg == 0)
  {
    for (uint8_t cell_reg = 1; cell_reg<ic[0].ic_reg.num_cv_reg+1; cell_reg++)                   //executes once for each of the LTC6811 cell voltage registers
    {
      LTC681x_rdcv_reg(cell_reg, total_ic,cell_data );
      for (int current_ic = 0; current_ic<total_ic; current_ic++)
      {
        if (ic->isospi_reverse == false)
        {
          c_ic = current_ic;
        }
        else
        {
          c_ic = total_ic - current_ic - 1;
        }
        pec_error = pec_error + parse_cells(current_ic,cell_reg, cell_data,
                                            &ic[c_ic].cells.c_codes[0],
                                            &ic[c_ic].cells.pec_match[0]);
      }
    }
  }

  else
  {
    LTC681x_rdcv_reg(reg, total_ic,cell_data);

    for (int current_ic = 0; current_ic<total_ic; current_ic++)
    {
      if (ic->isospi_reverse == false)
      {
        c_ic = current_ic;
      }
      else
      {
        c_ic = total_ic - current_ic - 1;
      }
      pec_error = pec_error + parse_cells(current_ic,reg, &cell_data[8*c_ic],
                                          &ic[c_ic].cells.c_codes[0],
                                          &ic[c_ic].cells.pec_match[0]);
    }
  }
  LTC681x_check_pec(total_ic,CELL,ic);
  free(cell_data);
  return(pec_error);
}



/*
The function is used
to read the  parsed GPIO codes of the LTC6811. This function will send the requested
read commands parse the data and store the gpio voltages in aux_codes variable
*/
int8_t LTC681x_rdaux(uint8_t reg, //Determines which GPIO voltage register is read back.
                     uint8_t total_ic,//the number of ICs in the system
                     cell_asic ic[]//A two dimensional array of the gpio voltage codes.
                    )
{
  uint8_t *data;
  int8_t pec_error = 0;
  uint8_t c_ic =0;
  data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));

  if (reg == 0)
  {
    for (uint8_t gpio_reg = 1; gpio_reg<ic[0].ic_reg.num_gpio_reg+1; gpio_reg++)                 //executes once for each of the LTC6811 aux voltage registers
    {
      LTC681x_rdaux_reg(gpio_reg, total_ic,data);                 //Reads the raw auxiliary register data into the data[] array
      for (int current_ic = 0; current_ic<total_ic; current_ic++)
      {
        if (ic->isospi_reverse == false)
        {
          c_ic = current_ic;
        }
        else
        {
          c_ic = total_ic - current_ic - 1;
        }
        pec_error = parse_cells(current_ic,gpio_reg, data,
                                &ic[c_ic].aux.a_codes[0],
                                &ic[c_ic].aux.pec_match[0]);

      }
    }
  }
  else
  {
    LTC681x_rdaux_reg(reg, total_ic, data);

    for (int current_ic = 0; current_ic<total_ic; current_ic++)
    {
      if (ic->isospi_reverse == false)
      {
        c_ic = current_ic;
      }
      else
      {
        c_ic = total_ic - current_ic - 1;
      }
      pec_error = parse_cells(current_ic,reg, data,
                              &ic[c_ic].aux.a_codes[0],
                              &ic[c_ic].aux.pec_match[0]);
    }

  }
  LTC681x_check_pec(total_ic,AUX,ic);
  free(data);
  return (pec_error);
}

// Reads and parses the LTC681x stat registers.
int8_t LTC681x_rdstat(uint8_t reg, //Determines which Stat  register is read back.
                      uint8_t total_ic,//the number of ICs in the system
                      cell_asic ic[]
                     )

{

  const uint8_t BYT_IN_REG = 6;
  const uint8_t GPIO_IN_REG = 3;

  uint8_t *data;
  uint8_t data_counter = 0;
  int8_t pec_error = 0;
  uint16_t parsed_stat;
  uint16_t received_pec;
  uint16_t data_pec;
  uint8_t c_ic = 0;
  data = (uint8_t *) malloc((NUM_RX_BYT*total_ic)*sizeof(uint8_t));

  if (reg == 0)
  {

    for (uint8_t stat_reg = 1; stat_reg< 3; stat_reg++)                      //executes once for each of the LTC6811 stat voltage registers
    {
      data_counter = 0;
      LTC681x_rdstat_reg(stat_reg, total_ic,data);                            //Reads the raw statiliary register data into the data[] array

      for (uint8_t current_ic = 0 ; current_ic < total_ic; current_ic++)      // executes for every LTC6811 in the daisy chain
      {
        if (ic->isospi_reverse == false)
        {
          c_ic = current_ic;
        }
        else
        {
          c_ic = total_ic - current_ic - 1;
        }
        // current_ic is used as the IC counter
        if (stat_reg ==1)
        {
          for (uint8_t current_gpio = 0; current_gpio< GPIO_IN_REG; current_gpio++) // This loop parses the read back data into GPIO voltages, it
          {
            // loops once for each of the 3 gpio voltage codes in the register

            parsed_stat = data[data_counter] + (data[data_counter+1]<<8);              //Each gpio codes is received as two bytes and is combined to
            ic[c_ic].stat.stat_codes[current_gpio] = parsed_stat;
            data_counter=data_counter+2;                                               //Because gpio voltage codes are two bytes the data counter

          }
        }
        else if (stat_reg == 2)
        {
          parsed_stat = data[data_counter] + (data[data_counter+1]<<8);              //Each gpio codes is received as two bytes and is combined to
          data_counter = data_counter +2;
          ic[c_ic].stat.stat_codes[3] = parsed_stat;
          ic[c_ic].stat.flags[0] = data[data_counter++];
          ic[c_ic].stat.flags[1] = data[data_counter++];
          ic[c_ic].stat.flags[2] = data[data_counter++];
          ic[c_ic].stat.mux_fail[0] = (data[data_counter] & 0x02)>>1;
          ic[c_ic].stat.thsd[0] = data[data_counter++] & 0x01;
        }

        received_pec = (data[data_counter]<<8)+ data[data_counter+1];          //The received PEC for the current_ic is transmitted as the 7th and 8th
        //after the 6 gpio voltage data bytes
        data_pec = pec15_calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT]);

        if (received_pec != data_pec)
        {
          pec_error = -1; //The pec_error variable is simply set negative if any PEC errors
          ic[c_ic].stat.pec_match[stat_reg-1]=1;
          //are detected in the received serial data
        }
        else
        {
          ic[c_ic].stat.pec_match[stat_reg-1]=0;
        }

        data_counter=data_counter+2;                        //Because the transmitted PEC code is 2 bytes long the data_counter
        //must be incremented by 2 bytes to point to the next ICs gpio voltage data
      }


    }

  }
  else
  {

    LTC681x_rdstat_reg(reg, total_ic, data);
    for (int current_ic = 0 ; current_ic < total_ic; current_ic++)            // executes for every LTC6811 in the daisy chain
    {
      // current_ic is used as an IC counter
      if (ic->isospi_reverse == false)
      {
        c_ic = current_ic;
      }
      else
      {
        c_ic = total_ic - current_ic - 1;
      }
      if (reg ==1)
      {
        for (uint8_t current_gpio = 0; current_gpio< GPIO_IN_REG; current_gpio++) // This loop parses the read back data into GPIO voltages, it
        {
          // loops once for each of the 3 gpio voltage codes in the register
          parsed_stat = data[data_counter] + (data[data_counter+1]<<8);              //Each gpio codes is received as two bytes and is combined to
          // create the parsed gpio voltage code

          ic[c_ic].stat.stat_codes[current_gpio] = parsed_stat;
          data_counter=data_counter+2;                        //Because gpio voltage codes are two bytes the data counter
          //must increment by two for each parsed gpio voltage code

        }
      }
      else if (reg == 2)
      {
        parsed_stat = data[data_counter++] + (data[data_counter++]<<8);              //Each gpio codes is received as two bytes and is combined to
        ic[c_ic].stat.stat_codes[3] = parsed_stat;
        ic[c_ic].stat.flags[0] = data[data_counter++];
        ic[c_ic].stat.flags[1] = data[data_counter++];
        ic[c_ic].stat.flags[2] = data[data_counter++];
        ic[c_ic].stat.mux_fail[0] = (data[data_counter] & 0x02)>>1;
        ic[c_ic].stat.thsd[0] = data[data_counter++] & 0x01;
      }


      received_pec = (data[data_counter]<<8)+ data[data_counter+1];          //The received PEC for the current_ic is transmitted as the 7th and 8th
      //after the 6 gpio voltage data bytes
      data_pec = pec15_calc(BYT_IN_REG, &data[current_ic*NUM_RX_BYT]);
      if (received_pec != data_pec)
      {
        pec_error = -1;                             //The pec_error variable is simply set negative if any PEC errors
        ic[c_ic].stat.pec_match[reg-1]=1;

      }

      data_counter=data_counter+2;
    }
  }
  LTC681x_check_pec(total_ic,STAT,ic);
  free(data);
  return (pec_error);
}

//Write the LTC681x CFGRA
void LTC681x_wrcfg(uint8_t total_ic, //The number of ICs being written to
                   cell_asic ic[]
                  )
{
  uint8_t cmd[2] = {0x00 , 0x01} ;
  uint8_t write_buffer[256];
  uint8_t write_count = 0;
  uint8_t c_ic = 0;
  for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
  {
    if (ic->isospi_reverse == true)
    {
      c_ic = current_ic;
    }
    else
    {
      c_ic = total_ic - current_ic - 1;
    }

    for (uint8_t data = 0; data<6; data++)
    {
      write_buffer[write_count] = ic[c_ic].config.tx_data[data];
      write_count++;
    }
  }
  write_68(total_ic, cmd, write_buffer);
}

//Write the LTC681x CFGRB
void LTC681x_wrcfgb(uint8_t total_ic, //The number of ICs being written to
                    cell_asic ic[]
                   )
{
  uint8_t cmd[2] = {0x00 , 0x24} ;
  uint8_t write_buffer[256];
  uint8_t write_count = 0;
  uint8_t c_ic = 0;
  for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
  {
    if (ic->isospi_reverse == true)
    {
      c_ic = current_ic;
    }
    else
    {
      c_ic = total_ic - current_ic - 1;
    }

    for (uint8_t data = 0; data<6; data++)
    {
      write_buffer[write_count] = ic[c_ic].configb.tx_data[data];
      write_count++;
    }
  }
  write_68(total_ic, cmd, write_buffer);
}

//Read CFGA
int8_t LTC681x_rdcfg(uint8_t total_ic, //Number of ICs in the system
                     cell_asic ic[]
                    )
{
  uint8_t cmd[2]= {0x00 , 0x02};
  uint8_t read_buffer[256];
  int8_t pec_error = 0;
  uint16_t data_pec;
  uint16_t calc_pec;
  uint8_t c_ic = 0;
  pec_error = read_68(total_ic, cmd, read_buffer);
  for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
  {
    if (ic->isospi_reverse == false)
    {
      c_ic = current_ic;
    }
    else
    {
      c_ic = total_ic - current_ic - 1;
    }

    for (int byte=0; byte<8; byte++)
    {
      ic[c_ic].config.rx_data[byte] = read_buffer[byte+(8*current_ic)];
    }
    calc_pec = pec15_calc(6,&read_buffer[8*current_ic]);
    data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);
    if (calc_pec != data_pec )
    {
      ic[c_ic].config.rx_pec_match = 1;
    }
    else ic[c_ic].config.rx_pec_match = 0;
  }
  LTC681x_check_pec(total_ic,CFGR,ic);
  return(pec_error);
}

//Reads CFGB
int8_t LTC681x_rdcfgb(uint8_t total_ic, //Number of ICs in the system
                      cell_asic ic[]
                     )
{
  uint8_t cmd[2]= {0x00 , 0x26};
  uint8_t read_buffer[256];
  int8_t pec_error = 0;
  uint16_t data_pec;
  uint16_t calc_pec;
  uint8_t c_ic = 0;
  pec_error = read_68(total_ic, cmd, read_buffer);
  for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
  {
    if (ic->isospi_reverse == false)
    {
      c_ic = current_ic;
    }
    else
    {
      c_ic = total_ic - current_ic - 1;
    }

    for (int byte=0; byte<8; byte++)
    {
      ic[c_ic].configb.rx_data[byte] = read_buffer[byte+(8*current_ic)];
    }
    calc_pec = pec15_calc(6,&read_buffer[8*current_ic]);
    data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);
    if (calc_pec != data_pec )
    {
      ic[c_ic].configb.rx_pec_match = 1;
    }
    else ic[c_ic].configb.rx_pec_match = 0;
  }
  LTC681x_check_pec(total_ic,CFGRB,ic);
  return(pec_error);
}

//Looks up the result pattern for digital filter self test
uint16_t LTC681x_st_lookup(
  uint8_t MD, //ADC Mode
  uint8_t ST //Self Test
)
{
  uint16_t test_pattern = 0;
  if (MD == 1)
  {
    if (ST == 1)
    {
      test_pattern = 0x9565;
    }
    else
    {
      test_pattern = 0x6A9A;
    }
  }
  else
  {
    if (ST == 1)
    {
      test_pattern = 0x9555;
    }
    else
    {
      test_pattern = 0x6AAA;
    }
  }
  return(test_pattern);
}

//Clears all of the DCC bits in the configuration registers
void clear_discharge(uint8_t total_ic, cell_asic ic[])
{
  for (int i=0; i<total_ic; i++)
  {
    ic[i].config.tx_data[4] = 0;
    ic[i].config.tx_data[5] = 0;
  }
}

// Runs the Digital Filter Self Test
int16_t LTC681x_run_cell_adc_st(uint8_t adc_reg,uint8_t total_ic, cell_asic ic[])
{
  int16_t error = 0;
  uint16_t expected_result = 0;
  for (int self_test = 1; self_test<3; self_test++)
  {

    expected_result = LTC681x_st_lookup(2,self_test);
    wakeup_idle(total_ic);
    switch (adc_reg)
    {
      case CELL:
        wakeup_idle(total_ic);
        LTC681x_clrcell();
        LTC681x_cvst(2,self_test);
        LTC681x_pollAdc();//this isn't working
        wakeup_idle(total_ic);
        error = LTC681x_rdcv(0, total_ic,ic);
        for (int cic = 0; cic < total_ic; cic++)
        {
          for (int channel=0; channel< ic[cic].ic_reg.cell_channels; channel++)
          {
            if (ic[cic].cells.c_codes[channel] != expected_result)
            {
              error = error+1;
            }
          }
        }
        break;
      case AUX:
        error = 0;
        wakeup_idle(total_ic);
        LTC681x_clraux();
        LTC681x_axst(2,self_test);
        LTC681x_pollAdc();
        delay_m(10);
        wakeup_idle(total_ic);
        LTC681x_rdaux(0, total_ic,ic);
        for (int cic = 0; cic < total_ic; cic++)
        {
          for (int channel=0; channel< ic[cic].ic_reg.aux_channels; channel++)
          {
            if (ic[cic].aux.a_codes[channel] != expected_result)
            {
              error = error+1;
            }
          }
        }
        break;
      case STAT:
        wakeup_idle(total_ic);
        LTC681x_clrstat();
        LTC681x_statst(2,self_test);
        LTC681x_pollAdc();
        wakeup_idle(total_ic);
        error = LTC681x_rdstat(0,total_ic,ic);
        for (int cic = 0; cic < total_ic; cic++)
        {
          for (int channel=0; channel< ic[cic].ic_reg.stat_channels; channel++)
          {
            if (ic[cic].stat.stat_codes[channel] != expected_result)
            {
              error = error+1;
            }
          }
        }
        break;

      default:
        error = -1;
        break;
    }
  }
  return(error);
}

//runs the redundancy self test
int16_t LTC681x_run_adc_redundancy_st(uint8_t adc_mode, uint8_t adc_reg, uint8_t total_ic, cell_asic ic[])
{
  int16_t error = 0;
  for (int self_test = 1; self_test<3; self_test++)
  {
    wakeup_idle(total_ic);
    switch (adc_reg)
    {
      case AUX:
        LTC681x_clraux();
        LTC681x_adaxd(adc_mode,AUX_CH_ALL);
        LTC681x_pollAdc();
        wakeup_idle(total_ic);
        error = LTC681x_rdaux(0, total_ic,ic);
        for (int cic = 0; cic < total_ic; cic++)
        {
          for (int channel=0; channel< ic[cic].ic_reg.aux_channels; channel++)
          {
            if (ic[cic].aux.a_codes[channel] >= 65280)
            {
              error = error+1;
            }
          }
        }
        break;
      case STAT:
        LTC681x_clrstat();
        LTC681x_adstatd(adc_mode,STAT_CH_ALL);
        LTC681x_pollAdc();
        wakeup_idle(total_ic);
        error = LTC681x_rdstat(0,total_ic,ic);
        for (int cic = 0; cic < total_ic; cic++)
        {
          for (int channel=0; channel< ic[cic].ic_reg.stat_channels; channel++)
          {
            if (ic[cic].stat.stat_codes[channel] >= 65280)
            {
              error = error+1;
            }
          }
        }
        break;

      default:
        error = -1;
        break;
    }
  }
  return(error);
}

//Runs the datasheet algorithm for open wire
void LTC681x_run_openwire(uint8_t total_ic, cell_asic ic[])
{
  uint16_t OPENWIRE_THRESHOLD = 4000;
  const uint8_t  N_CHANNELS = ic[0].ic_reg.cell_channels;

  cell_asic pullUp_cell_codes[total_ic];
  cell_asic pullDwn_cell_codes[total_ic];
  cell_asic openWire_delta[total_ic];
  int8_t error;

  wakeup_sleep(total_ic);
  LTC681x_adow(MD_7KHZ_3KHZ,PULL_UP_CURRENT);
  LTC681x_pollAdc();
  wakeup_idle(total_ic);
  LTC681x_adow(MD_7KHZ_3KHZ,PULL_UP_CURRENT);
  LTC681x_pollAdc();
  wakeup_idle(total_ic);
  error = LTC681x_rdcv(0, total_ic,pullUp_cell_codes);

  wakeup_idle(total_ic);
  LTC681x_adow(MD_7KHZ_3KHZ,PULL_DOWN_CURRENT);
  LTC681x_pollAdc();
  wakeup_idle(total_ic);
  LTC681x_adow(MD_7KHZ_3KHZ,PULL_DOWN_CURRENT);
  LTC681x_pollAdc();
  wakeup_idle(total_ic);
  error = LTC681x_rdcv(0, total_ic,pullDwn_cell_codes);

  for (int cic=0; cic<total_ic; cic++)
  {
    ic[cic].system_open_wire =0;
    for (int cell=0; cell<N_CHANNELS; cell++)
    {
      if (pullDwn_cell_codes[cic].cells.c_codes[cell]>pullUp_cell_codes[cic].cells.c_codes[cell])
      {
        openWire_delta[cic].cells.c_codes[cell] = pullDwn_cell_codes[cic].cells.c_codes[cell] - pullUp_cell_codes[cic].cells.c_codes[cell]  ;
      }
      else
      {
        openWire_delta[cic].cells.c_codes[cell] = 0;
      }

    }
  }
  for (int cic=0; cic<total_ic; cic++)
  {
    for (int cell=1; cell<N_CHANNELS; cell++)
    {

      if (openWire_delta[cic].cells.c_codes[cell]>OPENWIRE_THRESHOLD)
      {
        ic[cic].system_open_wire += (1<<cell);

      }
    }
    if (pullUp_cell_codes[cic].cells.c_codes[0] == 0)
    {
      ic[cic].system_open_wire += 1;
    }
    if (pullUp_cell_codes[cic].cells.c_codes[N_CHANNELS-1] == 0)
    {
      ic[cic].system_open_wire += (1<<(N_CHANNELS));
    }
  }
}

// Runs the ADC overlap test for the IC
uint16_t LTC681x_run_adc_overlap(uint8_t total_ic, cell_asic ic[])
{
  uint16_t error = 0;
  int32_t measure_delta =0;
  int16_t failure_pos_limit = 20;
  int16_t failure_neg_limit = -20;
  wakeup_idle(total_ic);
  LTC681x_adol(MD_7KHZ_3KHZ,DCP_DISABLED);
  LTC681x_pollAdc();
  wakeup_idle(total_ic);
  error = LTC681x_rdcv(0, total_ic,ic);
  for (int cic = 0; cic<total_ic; cic++)
  {
    measure_delta = (int32_t)ic[cic].cells.c_codes[6]-(int32_t)ic[cic].cells.c_codes[7];
    if ((measure_delta>failure_pos_limit) || (measure_delta<failure_neg_limit))
    {
      error = error | (1<<(cic-1));
    }
  }
  return(error);
}

//Helper function that increments PEC counters
void LTC681x_check_pec(uint8_t total_ic,uint8_t reg, cell_asic ic[])
{
  switch (reg)
  {
    case CFGR:
      for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
      {
        ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].config.rx_pec_match;
        ic[current_ic].crc_count.cfgr_pec = ic[current_ic].crc_count.cfgr_pec + ic[current_ic].config.rx_pec_match;
      }
      break;

    case CFGRB:
      for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
      {
        ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].configb.rx_pec_match;
        ic[current_ic].crc_count.cfgr_pec = ic[current_ic].crc_count.cfgr_pec + ic[current_ic].configb.rx_pec_match;
      }
      break;
    case CELL:
      for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
      {
        for (int i=0; i<ic[0].ic_reg.num_cv_reg; i++)
        {
          ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].cells.pec_match[i];
          ic[current_ic].crc_count.cell_pec[i] = ic[current_ic].crc_count.cell_pec[i] + ic[current_ic].cells.pec_match[i];
        }
      }
      break;
    case AUX:
      for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
      {
        for (int i=0; i<ic[0].ic_reg.num_gpio_reg; i++)
        {
          ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + (ic[current_ic].aux.pec_match[i]);
          ic[current_ic].crc_count.aux_pec[i] = ic[current_ic].crc_count.aux_pec[i] + (ic[current_ic].aux.pec_match[i]);
        }
      }

      break;
    case STAT:
      for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
      {

        for (int i=0; i<ic[0].ic_reg.num_stat_reg-1; i++)
        {
          ic[current_ic].crc_count.pec_count = ic[current_ic].crc_count.pec_count + ic[current_ic].stat.pec_match[i];
          ic[current_ic].crc_count.stat_pec[i] = ic[current_ic].crc_count.stat_pec[i] + ic[current_ic].stat.pec_match[i];
        }
      }
      break;
    default:
      break;
  }
}

//Helper Function to reset PEC counters
void LTC681x_reset_crc_count(uint8_t total_ic, cell_asic ic[])
{
  for (int current_ic = 0 ; current_ic < total_ic; current_ic++)
  {
    ic[current_ic].crc_count.pec_count = 0;
    ic[current_ic].crc_count.cfgr_pec = 0;
    for (int i=0; i<6; i++)
    {
      ic[current_ic].crc_count.cell_pec[i]=0;

    }
    for (int i=0; i<4; i++)
    {
      ic[current_ic].crc_count.aux_pec[i]=0;
    }
    for (int i=0; i<2; i++)
    {
      ic[current_ic].crc_count.stat_pec[i]=0;
    }
  }
}

//Helper function to intialize CFG variables.
void LTC681x_init_cfg(uint8_t total_ic, cell_asic ic[])
{
  bool REFON = true;
  bool ADCOPT = false;
  bool gpioBits[5] = {true,true,true,true,true};
  bool dccBits[12] = {false,false,false,false,false,false,false,false,false,false,false,false};
  for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
  {
    for (int j =0; j<6; j++)
    {
      ic[current_ic].config.tx_data[j] = 0;
      ic[current_ic].configb.tx_data[j] = 0;
    }
    LTC681x_set_cfgr(current_ic ,ic,REFON,ADCOPT,gpioBits,dccBits);

  }
}

//Helper function to set CFGR variable
void LTC681x_set_cfgr(uint8_t nIC, cell_asic ic[], bool refon, bool adcopt, bool gpio[5],bool dcc[12])
{
  LTC681x_set_cfgr_refon(nIC,ic,refon);
  LTC681x_set_cfgr_adcopt(nIC,ic,adcopt);
  LTC681x_set_cfgr_gpio(nIC,ic,gpio);
  LTC681x_set_cfgr_dis(nIC,ic,dcc);
}

//Helper function to set the REFON bit
void LTC681x_set_cfgr_refon(uint8_t nIC, cell_asic ic[], bool refon)
{
  if (refon) ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0]|0x04;
  else ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0]&0xFB;
}

//Helper function to set the adcopt bit
void LTC681x_set_cfgr_adcopt(uint8_t nIC, cell_asic ic[], bool adcopt)
{
  if (adcopt) ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0]|0x01;
  else ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0]&0xFE;
}

//Helper function to set GPIO bits
void LTC681x_set_cfgr_gpio(uint8_t nIC, cell_asic ic[],bool gpio[5])
{
  for (int i =0; i<5; i++)
  {
    if (gpio[i])ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0]|(0x01<<(i+3));
    else ic[nIC].config.tx_data[0] = ic[nIC].config.tx_data[0]&(~(0x01<<(i+3)));
  }
}

//Helper function to control discharge
void LTC681x_set_cfgr_dis(uint8_t nIC, cell_asic ic[],bool dcc[12])
{
  for (int i =0; i<8; i++)
  {
    if (dcc[i])ic[nIC].config.tx_data[4] = ic[nIC].config.tx_data[4]|(0x01<<i);
    else ic[nIC].config.tx_data[4] = ic[nIC].config.tx_data[4]& (~(0x01<<i));
  }
  for (int i =0; i<4; i++)
  {
    if (dcc[i+8])ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]|(0x01<<i);
    else ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5]&(~(0x01<<i));
  }
}

//Helper Function to set uv value in CFG register
void LTC681x_set_cfgr_uv(uint8_t nIC, cell_asic ic[],uint16_t uv)
{
  uint16_t tmp = (uv/16)-1;
  ic[nIC].config.tx_data[1] = 0x00FF & tmp;
  ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2]&0xF0;
  ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2]|((0x0F00 & tmp)>>8);
}

//helper function to set OV value in CFG register
void LTC681x_set_cfgr_ov(uint8_t nIC, cell_asic ic[],uint16_t ov)
{
  uint16_t tmp = (ov/16);
  ic[nIC].config.tx_data[3] = 0x00FF & (tmp>>4);
  ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2]&0x0F;
  ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2]|((0x000F & tmp)<<4);
}

//Writes the comm register
void LTC681x_wrcomm(uint8_t total_ic, //The number of ICs being written to
                    cell_asic ic[]
                   )
{
  uint8_t cmd[2]= {0x07 , 0x21};
  uint8_t write_buffer[256];
  uint8_t write_count = 0;
  uint8_t c_ic = 0;
  for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
  {
    if (ic->isospi_reverse == true)
    {
      c_ic = current_ic;
    }
    else
    {
      c_ic = total_ic - current_ic - 1;
    }

    for (uint8_t data = 0; data<6; data++)
    {
      write_buffer[write_count] = ic[c_ic].com.tx_data[data];
      write_count++;
    }
  }
  write_68(total_ic, cmd, write_buffer);
}

/*
Reads COMM registers of a LTC6811 daisy chain
*/
int8_t LTC681x_rdcomm(uint8_t total_ic, //Number of ICs in the system
                      cell_asic ic[]
                     )
{
  uint8_t cmd[2]= {0x07 , 0x22};
  uint8_t read_buffer[256];
  int8_t pec_error = 0;
  uint16_t data_pec;
  uint16_t calc_pec;
  uint8_t c_ic=0;
  pec_error = read_68(total_ic, cmd, read_buffer);
  for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
  {
    if (ic->isospi_reverse == false)
    {
      c_ic = current_ic;
    }
    else
    {
      c_ic = total_ic - current_ic - 1;
    }

    for (int byte=0; byte<8; byte++)
    {
      ic[c_ic].com.rx_data[byte] = read_buffer[byte+(8*current_ic)];
    }
    calc_pec = pec15_calc(6,&read_buffer[8*current_ic]);
    data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);
    if (calc_pec != data_pec )
    {
      ic[c_ic].com.rx_pec_match = 1;
    }
    else ic[c_ic].com.rx_pec_match = 0;
  }
  return(pec_error);
}

/*
Shifts data in COMM register out over LTC6811 SPI/I2C port
*/
void LTC681x_stcomm()
{

  uint8_t cmd[4];
  uint16_t cmd_pec;

  cmd[0] = 0x07;
  cmd[1] = 0x23;
  cmd_pec = pec15_calc(2, cmd);
  cmd[2] = (uint8_t)(cmd_pec >> 8);
  cmd[3] = (uint8_t)(cmd_pec);

  cs_low(CS_PIN);
  spi_write_array(4,cmd);
  for (int i = 0; i<9; i++)
  {
    spi_read_byte(0xFF);
  }
  cs_high(CS_PIN);

}

// Writes the pwm register
void LTC681x_wrpwm(uint8_t total_ic,
                   uint8_t pwmReg,
                   cell_asic ic[]
                  )
{
  uint8_t cmd[2];
  uint8_t write_buffer[256];
  uint8_t write_count = 0;
  uint8_t c_ic = 0;
  if (pwmReg == 0)
  {
    cmd[0] = 0x00;
    cmd[1] = 0x20;
  }
  else
  {
    cmd[0] = 0x00;
    cmd[1] = 0x1C;
  }

  for (uint8_t current_ic = 0; current_ic<total_ic; current_ic++)
  {
    if (ic->isospi_reverse == true)
    {
      c_ic = current_ic;
    }
    else
    {
      c_ic = total_ic - current_ic - 1;
    }
    for (uint8_t data = 0; data<6; data++)
    {
      write_buffer[write_count] = ic[c_ic].pwm.tx_data[data];
      write_count++;
    }
  }
  write_68(total_ic, cmd, write_buffer);
}


/*
Reads pwm registers of a LTC6811 daisy chain
*/
int8_t LTC681x_rdpwm(uint8_t total_ic, //Number of ICs in the system
                     uint8_t pwmReg,
                     cell_asic ic[]
                    )
{
  const uint8_t BYTES_IN_REG = 8;

  uint8_t cmd[4];
  uint8_t read_buffer[256];
  int8_t pec_error = 0;
  uint16_t data_pec;
  uint16_t calc_pec;
  uint8_t c_ic = 0;

  if (pwmReg == 0)
  {
    cmd[0] = 0x00;
    cmd[1] = 0x22;
  }
  else
  {
    cmd[0] = 0x00;
    cmd[1] = 0x1E;
  }


  pec_error = read_68(total_ic, cmd, read_buffer);
  for (uint8_t current_ic =0; current_ic<total_ic; current_ic++)
  {
    if (ic->isospi_reverse == false)
    {
      c_ic = current_ic;
    }
    else
    {
      c_ic = total_ic - current_ic - 1;
    }
    for (int byte=0; byte<8; byte++)
    {
      ic[c_ic].pwm.rx_data[byte] = read_buffer[byte+(8*current_ic)];
    }
    calc_pec = pec15_calc(6,&read_buffer[8*current_ic]);
    data_pec = read_buffer[7+(8*current_ic)] | (read_buffer[6+(8*current_ic)]<<8);
    if (calc_pec != data_pec )
    {
      ic[c_ic].pwm.rx_pec_match = 1;
    }
    else ic[c_ic].pwm.rx_pec_match = 0;
  }
  return(pec_error);
}
