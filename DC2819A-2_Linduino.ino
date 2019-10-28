/*! @file
    @ingroup LTM9057
*/
 
#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTM9057.h"

// Scaling factors, gains and device address assume demo card jumpers are per DC2819A-2 schematic
float sense_resistor = 0.02;    // Default resistance in ohms of demo card shunt, change as neccessary per application
float adin_gain = 1.0;          // Default, assumes direct input to ADIN, use: adin_gain = [R2/(R1+R2)] change as neccessary per application
uint8_t i2c_address = 0x6F;     // Default; assumes ADR0=ADR1=L as DC2819A is delivered, change as neccessary per application
                                // 7-bit version of LTM9057 I2C device addr determined by ADR0 & ADR1 (divide datasheet address by 2). ADR0/1 low: 0xDE -> 0x6F

//! Initialize Linduino
void setup()
{
  //Port initialization
  DDRD = DDRD | B00111100;      // Sets data direction of Digital Pins 0-7 (Logical OR to not interfere with Rx and Tx)
  PORTD = B00000000;            // Initializes Digital Pins 0-7 to LOW
  DDRB = B00011111;             // Sets data direction of Digital Pins 8-13
  PORTB = B00000001;            // Initializes Digital Pin 8 HIGH and 9-13 to LOW, Pin 8 is I2C Linduino select bit
  i2c_enable();                 // Initializes Linduino I2C port.
  Serial.begin(115200);         // Initialize the serial port to the PC
  print_title();
  print_prompt();
}
 
//! Repeats Linduino loop
void loop()
{
  int8_t ack=0;
  uint8_t user_command;
  uint8_t data;
  uint8_t user_register;
  
  if(Serial.available())
  {
    user_command = read_int();                              //! Reads the user command
    Serial.println(user_command);                         // Print user command

    //! Reads or writes to a LTM9057 and prints result.
    switch (user_command)
    {
      case 1:
        // Read every register and print it's data.
        ack |= LTM9057_print_all_registers(i2c_address);
        break;
      case 2:
        // Read a single register and print it's data.
        Serial.print(F("\nAddress (in hex with '0x' prefix) of register to read: "));
        user_register = read_int();
        Serial.print("0x");
        Serial.println(user_register, HEX);
        if (!valid_register(user_register, reg_read_list, sizeof(reg_read_list)))
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        ack |= LTM9057_register_read(i2c_address, user_register, &data);
        if(!ack)
        {
          Serial.print("Register data: 0x");
          Serial.println(data, HEX);
        }
        break;
      case 3:
        // Write a byte to a chosen register.
        Serial.print(F("\nAddress (in hex with '0x' prefix) of register to write: "));
        user_register = read_int();
        Serial.print("0x");
        Serial.println(user_register, HEX);
        if (!valid_register(user_register, reg_write_list, sizeof(reg_write_list)))
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.print(F("Data (in hex with '0x' prefix) to write: "));
        data = read_int();
        Serial.println(data, HEX);
        ack |= LTM9057_register_write(i2c_address, user_register, data);
        if(!ack)
        {
          Serial.print("0x");
          Serial.print(data, HEX);
          Serial.print(" written to register 0x");
          Serial.println(user_register, HEX); 
        }
        break;
      case 4:
        // Set a single bit within a chosen register.
        Serial.print(F("\nAddress (in hex with '0x' prefix) of register: "));
        user_register = read_int();
        Serial.print("0x");
        Serial.println(user_register, HEX);
        if (!valid_register(user_register, reg_write_list, sizeof(reg_write_list)))
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.print(F("Bit position (0-7) to set: "));
        data = read_int();
        if (data < 0 || data > 7)
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.println(data, DEC);
        ack |= LTM9057_bit_set(i2c_address, user_register, data);
        if(!ack)
        {
          Serial.print("Bit set. Register data is now 0x");
          ack |= LTM9057_register_read(i2c_address, user_register, &data);
          Serial.println(data, HEX); 
        }
        break;
      case 5:
        // Clear a single bit within a chosen register.
        Serial.print(F("\nAddress (in hex with '0x' prefix) of register: "));
        user_register = read_int();
        Serial.print("0x");
        Serial.println(user_register, HEX);
        if (!valid_register(user_register, reg_write_list, sizeof(reg_write_list)))
        {  
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.print(F("Bit position (0-7) to clear: "));
        data = read_int();
        if (data < 0 || data > 7)
        {
          Serial.println(F("Invalid input."));
          break;
        }
        Serial.println(data, DEC);
        ack |= LTM9057_bit_clear(i2c_address, user_register, data);
        if(!ack)
        {
          Serial.print("Bit cleared. Register data is now 0x");
          ack |= LTM9057_register_read(i2c_address, user_register, &data);
          Serial.println(data, HEX); 
        }
        break;
      case 6:
        // Write a byte to a chosen register via the Mass Write I2C address.
        Serial.print(F("\nAddress (in hex with '0x' prefix) of register to write: "));
        user_register = read_int();
        Serial.print("0x");
        Serial.println(user_register, HEX);
        if (!valid_register(user_register, reg_write_list, sizeof(reg_write_list)))
        {
          Serial.println(F("  Invalid input."));
          break;
        }
        Serial.print(F("Data (in hex with '0x' prefix) to write: "));
        data = read_int();
        Serial.println(data, HEX);
        ack |= LTM9057_register_write(0xCC, user_register, data);
        if(!ack)
        {
          Serial.print("0x");
          Serial.print(data, HEX);
          Serial.print(" written to register 0x");
          Serial.println(user_register, HEX); 
        }
        break;
      case 7:
        ack |= menu_read_status1();                      // Print status1 menu
        break;
      case 8:
        ack |= menu_read_status2();                  // Print status2 menu
        break;
      case 9:
        ack |= menu_read_write_fault1();                  // Print Fault1 menu
        break;
      case 10:
        ack |= menu_read_write_fault2();              // Print fault2 menu
        break;
      case 11:
        ack |= menu_read_write_alert1();            // Print ALERT1 menu
        break;
      case 12:
        ack |= menu_read_write_alert2();            // Print ALERT2 menu
        break;
      case 13:
        ack |= menu_configure_CTRLA(i2c_address);            // Print Conf. CTRLA menu
        break;
      case 14:
        ack |= menu_configure_CTRLB(i2c_address);            // Print Conf. CTRLB menu
        break;
      case 15:
        ack |= LTM9057_continuous_read_all_registers(i2c_address);
        break;
    }
    if (ack != 0)
    {
      Serial.print(F("Error: No Acknowledge. \n"));
    }
    print_prompt();
  }
}

// Function Definitions

//! Prints a warning if the demo board is not detected.
void print_warning_prompt()
{
  Serial.println(F("\nWarning: Demo board not detected. Linduino will attempt to proceed."));
}

//! Prints the title block when program first starts.
void print_title()
{
  Serial.print(F("\n*****************************************************************\n"));
  Serial.print(F("* LTM9057 Demonstration Program                                 *\n"));
  Serial.print(F("* Set ADDR0 = LOW, ADDR1 = LOW                                  *\n"));
  Serial.print(F("* This program demonstrates sending and receiving data from     *\n"));
  Serial.print(F("* the LTM9057 via I2C.                                          *\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("* Set the baud rate to 115200 and select the newline terminator.*\n"));
  Serial.print(F("*                                                               *\n"));
  Serial.print(F("*****************************************************************\n"));
}

// Prints main menu.
void print_prompt()
{
  //! Displays the Read/Write Registers menu
  Serial.print(F("\nRead/Write Registers\n\n"));
  Serial.print(F("  1-Read All Registers\n"));
  Serial.print(F("  2-Read Single Register\n"));
  Serial.print(F("  3-Write Single Register\n"));
  Serial.print(F("  4-Set Bit\n"));
  Serial.print(F("  5-Clear Bit\n"));
  Serial.print(F("  6-Write Register to Mass Write Address\n"));
  Serial.println(F("  7-Read Status1"));
  Serial.println(F("  8-Read Status2"));
  Serial.println(F("  9-Set/Clear FAULT1"));
  Serial.println(F("  10-Set/Clear FSULT2"));
  Serial.println(F("  11-Read/Write ALERT1"));
  Serial.println(F("  12-Read/Write ALERT2"));
  Serial.println(F("  13-Read/Write CTRLA"));
  Serial.println(F("  14-Read/Write CTRLB"));
  Serial.println(F("  15-Continuously Read Registers"));
  Serial.print(F("\nEnter a command: "));
}

int8_t menu_read_status1()
{
  int8_t ack=0;
  uint8_t user_command;
  uint8_t data;
  uint8_t user_register = LTM_9057_STATUS1_REG;
  uint8_t user_bit;
  do
  {
    //! Displays the Read Status menu
    Serial.print(F("\nPrimary Status\n\n"));
    Serial.print(F("  1-ADIN Undervalue\n"));
    Serial.print(F("  2-ADIN Overvalue\n"));
    Serial.print(F("  3-VIN Undervalue\n"));
    Serial.print(F("  4-VIN Overvalue\n"));
    Serial.print(F("  5-ISENSE Undervalue\n"));
    Serial.print(F("  6-ISENSE Overvalue\n"));
    Serial.print(F("  7-POWER Undervalue\n"));
    Serial.print(F("  8-POWER Overvalue\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
      Serial.print(F("m\n"));
    else
      Serial.println(user_command);                         // Print user command

    //! Reads LTM9057 and prints result.
    switch (user_command)
    {
      case 1:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_STATUS1_ADINUV, &data);
        Serial.print(F("ADIN Undervalue State: "));
        (data)?Serial.println(F("Below Minimum")):Serial.println(F("Above Minimum"));
        break;
      case 2:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_STATUS1_ADINOV, &data);
        Serial.print(F("ADIN Overvalue State: "));
        (data)?Serial.println(F("Above Maximum")):Serial.println(F("Below Maximum"));
        break;
      case 3:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_STATUS1_VINUV, &data);
        Serial.print(F("VIN Undervalue State: "));
        (data)?Serial.println(F("Below Minimum")):Serial.println(F("Above Minimum"));
        break;
      case 4:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_STATUS1_VINOV, &data);
        Serial.print(F("VIN Overvalue State: "));
        (data)?Serial.println(F("Above Maximum")):Serial.println(F("Below Maximum"));
        break;
      case 5:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_STATUS1_SNSUV, &data);
        Serial.print(F("dSENSE Undervalue State: "));
        (data)?Serial.println(F("Below Minimum")):Serial.println(F("Above Minimum"));
        break;
      case 6:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_STATUS1_SNSOV, &data);
        Serial.print(F("dSENSE Overvalue State: "));
        (data)?Serial.println(F("Above Maximum")):Serial.println(F("Below Maximum"));
        break;
      case 7:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_STATUS1_POWUV, &data);
        Serial.print(F("Overvalue: "));
        (data)?Serial.println(F("Below Minimum")):Serial.println(F("Above Minium"));
        break;
      case 8:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_STATUS1_POWOV, &data);
        Serial.print(F("Overvalue: "));
        (data)?Serial.println(F("Above Maximum")):Serial.println(F("Below Maximum"));
        break;
      default:
        if (user_command != 'm')
          Serial.println("Invalid Selection");
        break;
    }
    
  } while ((user_command != 'm') && (ack != 1));
  return(ack);
}

int8_t menu_read_status2()
{
  int8_t ack=0;
  uint8_t user_command;
  uint8_t data;
  uint8_t user_register = LTM_9057_STATUS2_REG;
  uint8_t user_bit;
  do
  {
    //! Displays the Read Status menu
    Serial.print(F("\nPrimary Status\n\n"));
    Serial.print(F("  1-Time Counter Register Overflow\n"));
    Serial.print(F("  2-Charge Register Overflow\n"));
    Serial.print(F("  3-Energy Register Overflow\n"));
    Serial.print(F("  4-ADC Busy\n"));
    Serial.print(F("  5-GPIO3 State\n"));
    Serial.print(F("  6-GPIO2 State\n"));
    Serial.print(F("  7-GPIO1 State\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
      Serial.print(F("m\n"));
    else
      Serial.println(user_command);                         // Print user command

    //! Reads LTM9057 and prints result.
    switch (user_command)
    {
      case 1:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_STATUS2_TCTR_OFLO, &data);
        Serial.print(F("Time Counter Register Overflow State: "));
        (data)?Serial.println(F("Max Count Exceeded")):Serial.println(F("Below Max Count"));
        break;
      case 2:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_STATUS2_CHGREG_OFLO, &data);
        Serial.print(F("Charge Register Overflow State: "));
        (data)?Serial.println(F("Max Count Exceeded")):Serial.println(F("Below Max Count"));
        break;
      case 3:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_STATUS2_ENGREG_OFLO, &data);
        Serial.print(F("Energy Register Overflow State: "));
        (data)?Serial.println(F("Max Count Exceeded")):Serial.println(F("Below Max Count"));
        break;
      case 4:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_STATUS2_ADC_BSY, &data);
        Serial.print(F("ADC Busy State: "));
        (data)?Serial.println(F("Busy")):Serial.println(F("Ready"));
        break;
      case 5:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_STATUS2_GPIO3, &data);
        Serial.print(F("GPIO3 State: "));
        (data)?Serial.println(F("High")):Serial.println(F("Low"));
        break;
      case 6:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_STATUS2_GPIO2, &data);
        Serial.print(F("GPIO2 State: "));
        (data)?Serial.println(F("High")):Serial.println(F("Lowo"));
        break;
      case 7:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_STATUS2_GPIO1, &data);
        Serial.print(F("GPIO1 State: "));
        (data)?Serial.println(F("High")):Serial.println(F("Low"));
        break;
      default:
        if (user_command != 'm')
          Serial.println("Invalid Selection");
        break;
    }
    
  } while ((user_command != 'm') && (ack != 1));
  return(ack);
}

int8_t menu_read_write_fault1()
{
  int8_t ack=0;
  uint8_t user_command;
  uint8_t data;
  uint8_t user_register = LTM_9057_FAULT1_REG;
  
  do
  {
    {
      Serial.print(F("\nPrimary Status\n\n"));
      Serial.print(F("  1-ADIN Undervalue Fault\n"));
      Serial.print(F("  2-ADIN Overvalue Fault\n"));
      Serial.print(F("  3-VIN Undervalue Fault\n"));
      Serial.print(F("  4-VIN Overvalue Fault\n"));
      Serial.print(F("  5-ISENSE Undervalue Fault\n"));
      Serial.print(F("  6-ISENSE Overvalue Fault\n"));
      Serial.print(F("  7-POWER Undervalue Fault\n"));
      Serial.print(F("  8-POWER Overvalue Fault\n"));
      Serial.print(F("  9-Disable/Enable ADIN UV Fault\n"));
      Serial.print(F("  10-Disable/Enable ADIN OV Fault\n"));
      Serial.print(F("  11-Disable/Enable VIN UV Fault\n"));
      Serial.print(F("  14-Disable/Enable VIN OV Fault\n"));
      Serial.print(F("  13-Disable/Enable ISENSE UVe Fault\n"));
      Serial.print(F("  14-Disable/Enable ISENSE OV Fault\n"));
      Serial.print(F("  15-Disable/Enable POWER UV Fault\n"));
      Serial.print(F("  16-Disable/Enable POWER OV Fault\n"));
    }
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
      Serial.print(F("m\n"));
    else
      Serial.println(user_command);                         // Print user command

    //! Reads or writes to a LTM9057 and prints result.
    switch (user_command)
    {
      case 1:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_FAULT1_ADIN_UV, &data);
        Serial.print(F("ADIN UNDERVALUE STATE: "));
        (data)?Serial.println(F("Below Minimum")):Serial.println(F("Above Minimum"));
        break;
      case 2:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_FAULT1_ADIN_OV, &data);
        Serial.print(F("ADIN OVERVALUE STATE: "));
        (data)?Serial.println(F("Above Maximum")):Serial.println(F("Below Maximum"));
        break;
      case 3:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_FAULT1_VIN_UV, &data);
        Serial.print(F("VIN UNDERVALUE STATE: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));
        break;
      case 4:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_FAULT1_VIN_OV, &data);
        Serial.print(F("VIN OVERVALUE STATE: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));
        break;
      case 5:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_FAULT1_ISNS_UV, &data);
        Serial.print(F("I SENSE UNDERVALUE STATE: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));
        break;
      case 6:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_FAULT1_ISNS_OV, &data);
        Serial.print(F("I SENSE OVERVALUE STATE: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));       
        break;
      case 7:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_FAULT1_PWR_UV, &data);
        Serial.print(F("POWER UNDERVALUE STATE: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));
        break;
      case 8:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_FAULT1_PWR_OV, &data);
        Serial.print(F("POWER OVERVALUE STATE: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));
        break;
      case 9:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_FAULT1_ADIN_UV);
        break;
      case 10:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_FAULT1_ADIN_OV);
        break;
      case 11:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_FAULT1_VIN_UV);
        break;
      case 12:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_FAULT1_VIN_OV);
        break;
      case 13:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_FAULT1_ISNS_UV);
        break;
      case 14:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_FAULT1_ISNS_OV);
        break;
      case 15:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_FAULT1_PWR_UV);
        break;
      case 16:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_FAULT1_PWR_OV);
        break;
      default:
        if (user_command != 'm')
          Serial.println("Invalid Selection");
        break;
    }
      
  } while ((user_command != 'm') && (ack != 1));
  return(ack);
}

int8_t menu_read_write_fault2()
{
  int8_t ack=0;
  uint8_t user_command;
  uint8_t data;
  uint8_t user_register = LTM_9057_FAULT1_REG;
  
  do
  {
    {
      Serial.print(F("\nPrimary Status\n\n"));
      Serial.print(F("  1-Time Counter Overflow Fault\n"));
      Serial.print(F("  2-Charge Register Overflow Fault\n"));
      Serial.print(F("  3-Energy Register Overflow Fault\n"));
      Serial.print(F("  4-Stuck-Bus Time-out wake-up Fault\n"));
      Serial.print(F("  5-GPIO3 Input Fault\n"));
      Serial.print(F("  6-GPIO2 Input Fault\n"));
      Serial.print(F("  7-GPIO1 Input Fault\n"));
      Serial.print(F("  8-Disable/Enable Time CTR OFLO Fault\n"));
      Serial.print(F("  9-Disable/Enable Charge REG OFLO Fault\n"));
      Serial.print(F("  10-Disable/Enable Energy REG OFLO Fault\n"));
      Serial.print(F("  11-Disable/Enable Stuck-Bus Fault\n"));
      Serial.print(F("  12-Disable/Enable GPIO3 Input Fault\n"));
      Serial.print(F("  13-Disable/Enable GPIO2 Input Fault\n"));
      Serial.print(F("  14-Disable/Enable GPIO1 Input Fault\n"));
    }
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
      Serial.print(F("m\n"));
    else
      Serial.println(user_command);                         // Print user command

    //! Reads or writes to a LTM9057 and prints result.
    switch (user_command)
    {
      case 1:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_FAULT2_TIME_CTR_REG_OFLO, &data);
        Serial.print(F("PGIO: "));
        (data)?Serial.println(F("Yes")):Serial.println(F("No"));
        break;
      case 2:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_FAULT2_CHG_REG_OFLO, &data);
        Serial.print(F("FET Short Detected: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));
        break;
      case 3:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_FAULT2_ENG_REG_OFLO, &data);
        Serial.print(F("Overcurrent: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));
        break;
      case 4:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_FAULT2_STK_BUS_TMO_WKUP, &data);
        Serial.print(F("Undervalue: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));
        break;
      case 5:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_FAULT2_GPIO3_IN, &data);
        Serial.print(F("Overvalue: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));
        break;
      case 6:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_FAULT2_GPIO2_IN, &data);
        Serial.print(F("Overvalue: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));
        break;
      case 7:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_FAULT2_GPIO1_IN, &data);
        Serial.print(F("Overvalue: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));
        break;
      case 8:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_FAULT2_TIME_CTR_REG_OFLO);
        break;
      case 9:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_FAULT2_CHG_REG_OFLO);
        break;
      case 10:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_FAULT2_ENG_REG_OFLO);
        break;
      case 11:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_FAULT2_STK_BUS_TMO_WKUP);
        break;
      case 12:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_FAULT2_GPIO3_IN);
        break;
      case 13:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_FAULT2_GPIO2_IN);
        break;
      case 14:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_FAULT2_GPIO1_IN);
        break;
      default:
        if (user_command != 'm')
          Serial.println("Invalid Selection");
        break;
    }
      
  } while ((user_command != 'm') && (ack != 1));
  return(ack);
}

int8_t menu_read_write_alert1()
{
  int8_t ack=0;
  uint8_t user_command;
  uint8_t data;
  uint8_t user_register = LTM_9057_ALERT1_REG;
  
  do
  {
    {
      Serial.print(F("\nPrimary Status\n\n"));
      Serial.print(F("  1-Minimum ADIN Alert\n"));
      Serial.print(F("  2-Maximum ADIN Alert\n"));
      Serial.print(F("  3-Minimum VIN Alert\n"));
      Serial.print(F("  4-Maximum VIN Alert\n"));
      Serial.print(F("  5-Minimum ISENSE Alert\n"));
      Serial.print(F("  6-Maximum ISENSE Alert\n"));
      Serial.print(F("  7-Minimum Power Alert\n"));
      Serial.print(F("  8-Maximum Power Alert\n"));
      Serial.print(F("  9-Disable/Enable Min ADIN Alert\n"));
      Serial.print(F("  10-Disable/Enable Max ADIN Alert\n"));
      Serial.print(F("  11-Disable/Enable Min VIN Alert\n"));
      Serial.print(F("  12-Disable/Enable Max VIN Alert\n"));
      Serial.print(F("  13-Disable/Enable Min ISENSE Alert\n"));
      Serial.print(F("  14-Disable/Enable Max ISENSE Alert\n"));
      Serial.print(F("  15-Disable/Enable Min Power Alert\n"));
      Serial.print(F("  16-Disable/Enable Max Power Alert\n"));
    }
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
      Serial.print(F("m\n"));
    else
      Serial.println(user_command);                         // Print user command

    //! Reads or writes to a LTM9057 and prints result.
    switch (user_command)
    {
      case 1:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_ALERT1_MIN_ADIN, &data);
        Serial.print(F("MIN ADIN ALERT STATE: "));
        (data)?Serial.println(F("TRUE")):Serial.println(F("FALSE"));
        break;
      case 2:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_ALERT1_MAX_ADIN, &data);
        Serial.print(F("MAX ADIN ALERT STATE: "));
        (data)?Serial.println(F("TRUE")):Serial.println(F("FALSE"));
        break;
      case 3:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_ALERT1_MIN_VIN, &data);
        Serial.print(F("MIN VIN ALERT STATE: "));
        (data)?Serial.println(F("TRUE")):Serial.println(F("FALSE"));
        break;
      case 4:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_ALERT1_MAX_VIN, &data);
        Serial.print(F("MAX VIN ALERT STATE: "));
        (data)?Serial.println(F("TRUE")):Serial.println(F("FALSE"));
        break;
      case 5:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_ALERT1_MIN_ISNS, &data);
        Serial.print(F("MIN I SENSE ALERT STATE: "));
        (data)?Serial.println(F("TRUE")):Serial.println(F("FALSE"));
        break;
      case 6:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_ALERT1_MAX_ISNS, &data);
        Serial.print(F("MAX I SENSE ALERT STATE: "));
        (data)?Serial.println(F("TRUE")):Serial.println(F("FALSE"));
        break;
      case 7:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_ALERT1_MIN_PWR, &data);
        Serial.print(F("MIN POWER ALERT STATE: "));
        (data)?Serial.println(F("TRUE")):Serial.println(F("FALSE"));
        break;
      case 8:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_ALERT1_MAX_PWR, &data);
        Serial.print(F("MAX POWER ALERT STATE: "));
        (data)?Serial.println(F("TRUE")):Serial.println(F("FALSE"));
        break;
      case 9:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT1_MIN_ADIN);
        break;
      case 10:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT1_MAX_ADIN);
        break;
      case 11:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT1_MIN_VIN);
        break;
      case 12:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT1_MAX_VIN);
        break;
      case 13:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT1_MIN_ISNS);
        break;
      case 14:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT1_MAX_ISNS);
        break;
      case 15:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT1_MIN_PWR);
        break;
      case 16:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT1_MAX_PWR);
        break;
      default:
        if (user_command != 'm')
          Serial.println("Invalid Selection");
        break;
    }
      
  } while ((user_command != 'm') && (ack != 1));
  return(ack);
}

int8_t menu_read_write_alert2()
{
  int8_t ack=0;
  uint8_t user_command;
  uint8_t data;
  uint8_t user_register = LTM_9057_ALERT2_REG;
  
  do
  {
    {
      Serial.print(F("\nPrimary Status\n\n"));
      Serial.print(F("  1-Time Counter Overflow Alert\n"));
      Serial.print(F("  2-Charge Overflow Alert\n"));
      Serial.print(F("  3-Energy Overflow Alert\n"));
      Serial.print(F("  4-Stuck Bus Timeout Wake-Up Alert\n"));
      Serial.print(F("  5-GPIO2 Input Alert\n"));
      Serial.print(F("  6-GPIO1 Input Alert\n"));
      Serial.print(F("  7-ADC Conversion Done Alert\n"));
      Serial.print(F("  8-Disable/Enable Time Counter Overflow Alert\n"));
      Serial.print(F("  9-Disable/Enable Charge Overflow Alert\n"));
      Serial.print(F("  10-Disable/Enable Energy Overflow Alert\n"));
      Serial.print(F("  11-Disable/Enable Stuck Bus Timeout Wake-Up Alert\n"));
      Serial.print(F("  12-Disable/Enable GPIO2 Input Alert\n"));
      Serial.print(F("  13-Disable/Enable GPIO1 Input Alert\n"));
      Serial.print(F("  14-Disable/Enable ADC Conversion Done Alert\n"));
    }
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
      Serial.print(F("m\n"));
    else
      Serial.println(user_command);                         // Print user command

    //! Reads or writes to a LTM9057 and prints result.
    switch (user_command)
    {
      case 1:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_ALERT2_TIME_CTR_OFLO, &data);
        Serial.print(F("TIME COUNT OVERFLOW STATE: "));
        (data)?Serial.println(F("OVER")):Serial.println(F("UNDER"));
        break;
      case 2:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_ALERT2_CHARGE_OFLO, &data);
        Serial.print(F("CHARGE OVERFLOW STATE: "));
        (data)?Serial.println(F("OVER")):Serial.println(F("UNDER"));
        break;
      case 3:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_ALERT2_ENERGY_OFLO, &data);
        Serial.print(F("ENERGY OVERFLOW STATE: "));
        (data)?Serial.println(F("OVER")):Serial.println(F("UNDER"));
        break;
      case 4:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_ALERT2_STK_BUS_TMO_WKUP, &data);
        Serial.print(F("STUCK BUS WAKE-UP: "));
        (data)?Serial.println(F("Fault")):Serial.println(F("No Fault"));
        break;
      case 5:
        ack |= LTM9057_bit_read(i2c_address, user_register, LTM_9057_ALERT2_GPIO2_IN, &data);
        Serial.print(F("GPIO2 INPUT STATE: "));
        (data)?Serial.println(F("High")):Serial.println(F("Low"));
        break;
      case 6:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT2_GPIO1_INR);
       Serial.print(F("GPIO1 INPUT STATE: "));
        (data)?Serial.println(F("High")):Serial.println(F("Low"));
        break;
      case 7:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT2_ADC_CNV_DONE);
       Serial.print(F("ADC Conversion: "));
        (data)?Serial.println(F("Done")):Serial.println(F("Not Finished"));
        break;
      case 8:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT2_TIME_CTR_OFLO);
        break;
      case 9:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT2_CHARGE_OFLO);
        break;
      case 10:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT2_ENERGY_OFLO);
        break;
      case 11:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT2_STK_BUS_TMO_WKUP);
        break;
      case 12:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT2_GPIO2_IN);
        break;
      case 13:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT2_GPIO1_INR);
        break;
      case 14:
        ack |= ask_clear_set_bit(i2c_address, user_register, LTM_9057_ALERT2_ADC_CNV_DONE);
        break;
      default:
        if (user_command != 'm')
          Serial.println("Invalid Selection");
        break;
    }
      
  } while ((user_command != 'm') && (ack != 1));
  return(ack);
}

static int8_t ask_clear_set_bit(uint8_t i2c_address, uint8_t register_address, uint8_t bit_number)
{
  int8_t ack = 0;
  uint8_t user_bit;
  Serial.println(F("0=Clear/Disable, 1=Set/Enable:"));
  user_bit = read_int();
  if (user_bit > 1)
    Serial.println(F("  Invalid input."));
  else
    ack |= (user_bit)?LTM9057_bit_set(i2c_address, register_address, bit_number):LTM9057_bit_clear(i2c_address, register_address, bit_number);

  return ack;
}

int8_t menu_configure_CTRLA(uint8_t i2c_address)
{
  int8_t ack=0;
  uint8_t user_command;
  float data;

  do
  {
    //! Displays the Configure CTRLA menu
    Serial.print(F("\nConfiguree ADCs \n\n"));
    Serial.print(F("  1-ADIN Reference value\n"));
    Serial.print(F("  2-Offset Calibration\n"));
    Serial.print(F("  3-ADC Input Select\n"));
    Serial.print(F("  4-ADC Channel Configuration\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
      Serial.print(F("m\n"));
    else
    {
      Serial.println(user_command);                         // Print user command
    }

    //! Reads or writes to a LTM9057 and prints result.
    switch (user_command)
    {
      case 1:
        ack |= LTM9057_prog_ADIN_REF(i2c_address);            // Print ADIN reference menu
        break;
      case 2:
        ack |= LTM9057_prog_OFFSET_CAL(i2c_address);          // Print offset calibration menu;
        break;
      case 3:
        ack |= LTM9057_prog_VOLTAGE_SELECTION(i2c_address);  // Print ADC input menu
        break;
      case 4:
        ack |= LTM9057_prog_ADC_CHAN_CONFIG(i2c_address);     // Print ADC channel configuration menu
        break;
      default:
        if (user_command != 'm')
          Serial.println("Invalid Selection");
        break;
    }
      
  } while ((user_command != 'm') && (ack != 1));
  return(ack);
}

int8_t LTM9057_prog_ADIN_REF(uint8_t i2c_address)
{
  int8_t ack = 0;
  uint8_t user_bit;
  Serial.println(F("0=VEE, 1=VCC2:"));
  user_bit = read_int();
  uint8_t bit_number = LTM_9057_CTRLA_ADIN_CONFIG;
  if (user_bit > 1)
    Serial.println(F("  Invalid input."));
  else
    ack |= (user_bit)?LTM9057_bit_set(i2c_address, LTM_9057_CTRLA_REG, bit_number):LTM9057_bit_clear(i2c_address, LTM_9057_CTRLA_REG, bit_number);

  return ack;
}

int8_t LTM9057_prog_OFFSET_CAL(uint8_t i2c_address)
{
  int8_t ack = 0;
  uint8_t user_select;
  Serial.println(F("0=Every Conversion, 1=Every 16th Conversion, 2=Every 128th Conversion, 3=1st Power UP:"));
  user_select = read_int();
  if (user_select > 3)
    Serial.println(F("  Invalid input."));
  else
    ack |= LTM_9057_set2bits(i2c_address, user_select, LTM_9057_CTRLA_REG, LTM_9057_CTRLA_OFFSET_CAL);
  return ack;
}

int8_t LTM9057_prog_VOLTAGE_SELECTION(uint8_t i2c_address)
{
  int8_t ack = 0;
  uint8_t user_select;
  Serial.println(F("0=dSENSE, 1=VDD, 2=ADIN, 3=SENSE+:"));
  user_select = read_int();
  if (user_select > 3)
    Serial.println(F("  Invalid input."));
  else
    ack |= LTM_9057_set2bits(i2c_address, user_select, LTM_9057_CTRLA_REG, LTM_9057_CTRLA_VOLT_SELECT);
  return ack;
}

int8_t LTM9057_prog_ADC_CHAN_CONFIG(uint8_t i2c_address)
{
  int8_t ack = 0;
  uint8_t user_select;
  Serial.println(F("0=Alternate Voltage/Current measurement"));
  Serial.println(F("1=Voltage/Current Measurement at 1/16 and 15/16 Duty Cycle"));
  Serial.println(F("2=Voltage/Current Measurement at 1/128 and 127/128 Duty Cycle"));
  Serial.println(F("3=Alternate ADIN, Voltage and Current Measurement"));
  Serial.println(F("4=ADIN, Voltage, Current Measurement at 1/32, 1/32 and 30/32 Duty Cycle"));
  Serial.println(F("5= ADIN, Voltage, Current Measurement at 1/256, 1/256 and 254/256 Duty Cycle"));
  Serial.println(F("6= Voltage Measurement Once followed by Current Measurement Indefinitely"));
  Serial.println(F("7= Snapshot Mode. No Power, Energy or Charge Data Generated:"));
  user_select = read_int();
  if (user_select > 7)
    Serial.println(F("  Invalid input."));
  else
    ack |= LTM_9057_set3bits(i2c_address, user_select, LTM_9057_CTRLA_REG, LTM_9057_CTRLA_CHANNEL_CONFIG);
  return ack;
}

int8_t menu_configure_CTRLB(uint8_t i2c_address)
{
  int8_t ack=0;
  uint8_t user_command;
  float data;

  do
  {
    //! Displays the Configure CTRLB menu
    Serial.print(F("\nConfiguree CTRLB Register \n\n"));
    Serial.print(F("  1-Auto Reset Mode\n"));
    Serial.print(F("  2-Accumulation Enable\n"));
    Serial.print(F("  3-Stuck Bus Timeout Auto Wake-Up\n"));
    Serial.print(F("  4-Cleared on Read Controln\n"));
    Serial.print(F("  5-Shutdown\n"));
    Serial.print(F("  6-ALERT Clear Enable\n"));
    Serial.print(F("  m-Main Menu\n"));
    Serial.print(F("\nEnter a command: "));

    user_command = read_int();                              //! Reads the user command
    if (user_command == 'm')                                // Print m if it is entered
      Serial.print(F("m\n"));
    else
    {
      Serial.println(user_command);                         // Print user command
    }

    //! Reads or writes to a LTM9057 and prints result.
    switch (user_command)
    {
      case 1:
        ack |= LTM9057_prog_AUTO_RESET(i2c_address);            // Program Auto Reset Mode
        break;
      case 2:
        ack |= LTM9057_prog_ACCUM_EN(i2c_address);             // Program Accumulation Enable
        break;
      case 3:
        ack |= LTM9057_prog__STUCK_BUS(i2c_address);           // Program Stuck Bus Auto Wake=Up
        break;
      case 4:
        ack |= LTM9057_prog_CLEARED_ON_READ(i2c_address);      // Program Cleared on Read Control
        break;
      case 5:
        ack |= LTM9057_prog__SHUTDOWN(i2c_address);            // Program Shutdown
        break;
      case 6:
        ack |= LTM9057_prog_ALERT_CLEAR_EN(i2c_address);       // Program Alert Clear Enable
        break;
      default:
        if (user_command != 'm')
          Serial.println("Invalid Selection");
        break;
    }
      
  } while ((user_command != 'm') && (ack != 1));
  return(ack);
}

int8_t LTM9057_prog_AUTO_RESET(uint8_t i2c_address)
{
  int8_t ack = 0;
  uint8_t user_select;
  Serial.println(F("0=Disable Auto-Reset, 1=Enable Auto-Reset, 2=Reset Accumulator, 3=Reset All Registers:"));
  user_select = read_int();
  if (user_select > 3)
    Serial.println(F("  Invalid input."));
  else
    ack |= LTM_9057_set2bits(i2c_address, user_select, LTM_9057_CTRLB_REG, LTM_9057_CTRLB_AUTO_RESET);
  return ack;
}

int8_t LTM9057_prog_ACCUM_EN(uint8_t i2c_address)
{
  int8_t ack = 0;
  uint8_t user_select;
  Serial.println(F("0=Accumulate, 1=No Accumulate, 2=Follow ACC State:"));
  user_select = read_int();
  if (user_select > 2)
    Serial.println(F("  Invalid input."));
  else
    ack |= LTM_9057_set2bits(i2c_address, user_select, LTM_9057_CTRLB_REG, LTM_9057_CTRLB_ENABLE_ACCUM);
  return ack;
}

int8_t LTM9057_prog__STUCK_BUS(uint8_t i2c_address)
{
  int8_t ack = 0;
  uint8_t user_bit;
  Serial.println(F("0=Disable, 1=Enable:"));
  user_bit = read_int();
  uint8_t bit_number = LTM_9057_CTRLB_STUCK_BUS;
  if (user_bit > 1)
    Serial.println(F("  Invalid input."));
  else
    ack |= (user_bit)?LTM9057_bit_set(i2c_address, LTM_9057_CTRLB_REG, bit_number):LTM9057_bit_clear(i2c_address, LTM_9057_CTRLA_REG, bit_number);
  return ack;
}

int8_t LTM9057_prog_CLEARED_ON_READ(uint8_t i2c_address)
{
  int8_t ack = 0;
  uint8_t user_bit;
  Serial.println(F("0=Register NOT affected on Read, 1=Cleared on Read:"));
  user_bit = read_int();
  uint8_t bit_number = LTM_9057_CTRLB_CLEAR_ON_READ;
  if (user_bit > 1)
    Serial.println(F("  Invalid input."));
  else
    ack |= (user_bit)?LTM9057_bit_set(i2c_address, LTM_9057_CTRLB_REG, bit_number):LTM9057_bit_clear(i2c_address, LTM_9057_CTRLA_REG, bit_number);
  return ack;
}

int8_t LTM9057_prog__SHUTDOWN(uint8_t i2c_address)
{
  int8_t ack = 0;
  uint8_t user_bit;
  Serial.println(F("0=Power-Up, 1=Shutdown:"));
  user_bit = read_int();
  uint8_t bit_number = LTM_9057_CTRLB_SHUTDOWN;
  if (user_bit > 1)
    Serial.println(F("  Invalid input."));
  else
    ack |= (user_bit)?LTM9057_bit_set(i2c_address, LTM_9057_CTRLB_REG, bit_number):LTM9057_bit_clear(i2c_address, LTM_9057_CTRLA_REG, bit_number);
  return ack;
}

int8_t LTM9057_prog_ALERT_CLEAR_EN(uint8_t i2c_address)
{
  int8_t ack = 0;
  uint8_t user_bit;
  Serial.println(F("0=Disable, 1=Enable:"));
  user_bit = read_int();
  uint8_t bit_number = LTM_9057_CTRLB_ALERT_CLEAR_EN;
  if (user_bit > 1)
    Serial.println(F("  Invalid input."));
  else
    ack |= (user_bit)?LTM9057_bit_set(i2c_address, LTM_9057_CTRLB_REG, bit_number):LTM9057_bit_clear(i2c_address, LTM_9057_CTRLA_REG, bit_number);
  return ack;
}

int8_t LTM_9057_set2bits(uint8_t i2c_address, uint8_t user_select, uint8_t register_address, uint8_t bit_number)
{  
  int8_t ack=0;
  if (user_select > 3)
  {
    Serial.println(F("  Invalid input."));
  }
  else if (user_select == 0)
  {
    ack |= LTM9057_bit_clear(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_clear(i2c_address, register_address, bit_number);
  }
    
  else if (user_select == 1)
  {
    ack |= LTM9057_bit_set(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_clear(i2c_address, register_address, bit_number);
  }
    
  else if (user_select == 2)
  {
    ack |= LTM9057_bit_clear(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_set(i2c_address, register_address, bit_number);
  }
    
  else
  {
    ack |= LTM9057_bit_set(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_set(i2c_address, register_address, bit_number);
  }

  return ack;
}

int8_t LTM_9057_set3bits(uint8_t i2c_address, uint8_t user_select, uint8_t register_address, uint8_t bit_number)
{ 
  int8_t ack=0;
  if (user_select > 7)
  {
    Serial.println(F("  Invalid input."));
  }
    
  else if (user_select == 0)
  {
    ack |= LTM9057_bit_clear(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_clear(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_clear(i2c_address, register_address, bit_number);
  }
    
  else if (user_select == 1)
  {
    ack |= LTM9057_bit_set(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_clear(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_clear(i2c_address, register_address, bit_number);
  }
    
  else if (user_select == 2)
  {
    ack |= LTM9057_bit_clear(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_set(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_clear(i2c_address, register_address, bit_number);
  }
    
  else if (user_select == 3)
  {
    ack |= LTM9057_bit_set(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_set(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_clear(i2c_address, register_address, bit_number);
  }
    
  else if (user_select == 4)
  {
    ack |= LTM9057_bit_clear(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_clear(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_set(i2c_address, register_address, bit_number);
  }
    
  else if (user_select == 5)
  {
    ack |= LTM9057_bit_set(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_clear(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_set(i2c_address, register_address, bit_number);
  }
    
  else if (user_select == 6)
  {
    ack |= LTM9057_bit_clear(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_set(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_set(i2c_address, register_address, bit_number);
  }
    
  else
  {
    ack |= LTM9057_bit_set(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_set(i2c_address, register_address, bit_number);
    bit_number += 1;
    ack |= LTM9057_bit_set(i2c_address, register_address, bit_number);
  }

  return ack;
}
