#include <Arduino.h>
#include <stdint.h>
#include "Linduino.h"
#include "LT_SPI.h"
#include "UserInterface.h"
#include "LT_I2C.h"
#include "QuikEval_EEPROM.h"
#include "LTC6904.h"
#include "LTC2484.h"
#include <SPI.h>
#include <Wire.h>
#include "LTC24XX_general.h"

int8_t restore_calibration();                   // Read the DAC calibration from EEPROM, Return 1 if successful, 0 if not
void store_calibration();                       // Store the ADC calibration to the EEPROM
uint8_t calibrate_voltage();

// Global variables
static float start, stop, step;
const uint16_t MISO_TIMEOUT = 1000;       //!< The MISO timeout (ms)

// Calibration variables
static float LTC2484_lsb = 9.3132258E-9;  //!< Ideal LSB size, 5V/(2^29) for a 5V reference
static int32_t LTC2484_offset_code = 0;   //!< Ideal offset

void setup()
{
  Serial.begin(115200);
  quikeval_SPI_init();      // Configures the SPI port for 4MHz SCK
  quikeval_I2C_init();      // Configure the EEPROM I2C port for 100kHz SCK
  restore_calibration();
}

void loop()
{
  int16_t user_command;   // User input command

  display_menu();

  user_command = read_int();  // Reads the user command (if available).
  Serial.println(user_command);

  switch (user_command)
  {
    case 1:
      settings();
      break;
    case 2:
      analyze();
      break;
    case 3:
      test();
      break;
    case 4:
      calibrate_voltage();
    default:
      Serial.println(F("incorrect option"));
      Serial.println();
      break;
  }
}

void display_menu()
{
  Serial.println();
  Serial.println(F("Main Menu"));
  Serial.println(F("1-Settings"));
  Serial.println(F("2-Analyze"));
  Serial.println(F("3-Test Clock"));
  Serial.println(F("4-Calibrate ADC"));
  Serial.println();
  Serial.print(F("Enter a command: "));
}

void settings()
{
  Serial.println(F("Enter the Start frequency (KHz)"));
  start = read_float();
  Serial.println(start, 4);

  Serial.println(F("Enter the Stop frequency (KHz)"));
  stop = read_float();
  Serial.println(stop, 4);

  Serial.println(F("Enter Step size (kHz)"));
  step = read_float();
  Serial.println(step, 4);
}

void analyze()
{

  float clock;
  int32_t clock_code;
  uint16_t i;
  uint8_t adc_command;


  clock = start/10; // Set Clock to 100 times the start frequency

  Serial.println();
  Serial.println();
  Serial.println(F("Sweeping..."));
  Serial.println();
  Serial.println(F("         --------------------------------0 dBV-|"));
  Serial.println(F("                                               V"));
  for (i=0; i<= (stop-start)/step; i++)
  {
    float adc_voltage = 0;
    int32_t adc_code = 0;

    quikeval_I2C_connect();                 // Connects I2C port to the QuikEval connector

    clock_code = LTC6904_frequency_to_code(clock, (uint8_t)LTC6904_CLK_ON_CLK_INV_ON);
    clock += (step)/10;
    LTC6904_write(LTC6904_ADDRESS, (uint32_t)clock_code);

    delay(500);

    quikeval_SPI_connect();                 // Connects SPI to QuikEval port

    delay (500);

    // Build ADC command to read the ADC voltage
    adc_command = LTC2484_ENABLE;
    adc_command |= LTC2484_EXTERNAL_INPUT;
    adc_command |= LTC2484_AUTO_CALIBRATION;
    adc_command |= LTC2484_REJECTION_60HZ;


    if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))   // Check for EOC
      return;
    LTC2484_read(LTC2484_CS, adc_command, &adc_code);   // Throw away last reading

    if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))   // Check for EOC
      return;
    LTC2484_read(LTC2484_CS, adc_command, &adc_code);   // Obtains the current reading and stores to adc_code variable

    // Convert adc_code to voltage
    adc_voltage = LTC2484_code_to_voltage(adc_code, LTC2484_lsb, LTC2484_offset_code);

    adc_voltage *= 2;                                   // Scale by 2 because Vrms was devided by two by the filter.

    if (i==0)
    {
      if (start<=9)
        Serial.print(F("  "));
      else if (start<=99)
        Serial.print(F(" "));

      Serial.print(start);
      Serial.print(F("KHz"));
    }
    else if (i == (stop-start)/step)
    {
      if (stop<=9)
        Serial.print(F("  "));
      else if (stop<=99)
        Serial.print(F(" "));
      Serial.print(stop);
      Serial.print(F("KHz"));
    }
    else if (adc_voltage > .1)
    {
      if (start+i*step <= 9)
        Serial.print(F("  "));
      else if (start+i*step <= 99)
        Serial.print(F(" "));
      Serial.print(start+i*step);
      Serial.print(F("KHz"));
    }
    else
      Serial.print(F("         "));
    display_graph(adc_voltage);
  }

  // Display data
  Serial.println();
  Serial.print(F("Step: "));
  Serial.println(step, 4);
  for (i=0; i<= (stop-start)/step; i++)
  {

  }

}

void display_graph(float data)
{
  int16_t i, j;

  data /= 0.025;

  j = (int16_t) data;

  if (j > 40)
    j = 40;
  for (i=0; i<=j; i++)
  {
    Serial.print(F("#"));
  }
  Serial.println();
}

void test()
{

  float freq;
  int32_t clock_code;
  float adc_voltage;
  int32_t adc_code;
  uint8_t adc_command;

  Serial.print(F("Enter the desired clock freq (KHz)"));

  freq = read_float();

  Serial.println(freq, 4);

  clock_code = LTC6904_frequency_to_code(freq/1000, (uint8_t)LTC6904_CLK_ON_CLK_INV_ON);

  if (clock_code == -1)
  {
    Serial.println("No Sloution");
    return;
  }
  LTC6904_write(LTC6904_ADDRESS, (uint32_t)clock_code);
  delay(3000);

  // Build ADC command to read the ADC voltage
  adc_command = LTC2484_ENABLE;
  adc_command |= LTC2484_EXTERNAL_INPUT;
  adc_command |= LTC2484_AUTO_CALIBRATION;
  adc_command |= LTC2484_REJECTION_60HZ;


  if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))   // Check for EOC
    return;
  LTC2484_read(LTC2484_CS, adc_command, &adc_code);   // Throw away last reading

  if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))   // Check for EOC
    return;
  LTC2484_read(LTC2484_CS, adc_command, &adc_code);   // Obtains the current reading and stores to adc_code variable

  // Convert adc_code to voltage
  adc_voltage = LTC2484_code_to_voltage(adc_code, LTC2484_lsb, LTC2484_offset_code);

  Serial.print("Voltage: ");
  Serial.println(adc_voltage, 4);
}

//! Read stored calibration parameters from nonvolatile EEPROM on demo board
//! @return 0 if successful, 1 if failure
int8_t restore_calibration()
// Read the DAC calibration from EEPROM
{
  int16_t cal_key;
  // Read the cal key from the EEPROM
  eeprom_read_int16(EEPROM_I2C_ADDRESS, &cal_key, EEPROM_CAL_STATUS_ADDRESS);
  if (cal_key == EEPROM_CAL_KEY)
  {
    // Calibration has been stored, read offset and lsb
    eeprom_read_int32(EEPROM_I2C_ADDRESS, &LTC2484_offset_code, EEPROM_CAL_STATUS_ADDRESS + 2);  // Offset
    eeprom_read_float(EEPROM_I2C_ADDRESS, &LTC2484_lsb, EEPROM_CAL_STATUS_ADDRESS + 6);          // LSB
    Serial.println(F("Calibration Restored"));
    return (1);
  }
  else
  {
    Serial.println(F("Calibration not found"));
    return (0);
  }
}

//! Calibrate ADC given two known inputs
//! @return 0 if successful, 1 if failure
uint8_t calibrate_voltage()
{
  // Calibrate voltage measurement
  float zero_voltage;       // Measured cal voltage
  float fs_voltage;         // Measured cal voltage
  int32_t zero_code;       // Cal zero code
  int32_t fs_code;         // Cal full scale code
  uint8_t user_command;     // The user input command
  int32_t adc_code = 0;    // The LTC2484 code
  uint8_t adc_command;      // The LTC2484 command byte

  Serial.println(F("Short the inputs to ground calibrate the offset."));
  Serial.println(F("or apply a voltage for the lower point in two point calibration"));
  Serial.print(F("Enter the measured input voltage:"));
  zero_voltage = read_float();
  Serial.println(zero_voltage, 6);

  // Build ADC command to read the ADC voltage
  adc_command = LTC2484_ENABLE;
  adc_command |= LTC2484_EXTERNAL_INPUT;
  adc_command |= LTC2484_AUTO_CALIBRATION;
  adc_command |= LTC2484_REJECTION_60HZ;

  if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2484_read(LTC2484_CS, adc_command, &adc_code);         // Throw away previous reading
  if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2484_read(LTC2484_CS, adc_command, &zero_code);        // Measure zero

  Serial.println(F("Apply ~2.49V to +IN"));
  Serial.println(F("Enter the measured input voltage:"));
  fs_voltage = read_float();

  if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2484_read(LTC2484_CS, adc_command, &adc_code);         // Throw away previous reading
  if (LTC2484_EOC_timeout(LTC2484_CS, MISO_TIMEOUT))        // Check for EOC
    return(1);
  LTC2484_read(LTC2484_CS, adc_command, &fs_code);          // Measure full scale

  LTC2484_cal_voltage(zero_code, fs_code, zero_voltage, fs_voltage, &LTC2484_lsb, &LTC2484_offset_code);

  Serial.print(F("ADC offset : "));
  Serial.print(LTC2484_offset_code);
  Serial.print(F(" ADC lsb : "));
  Serial.print(LTC2484_lsb * 1.0e9, 4);
  Serial.println(F("nV (32-bits)"));
  store_calibration();
  return(0);
}

//! Store measured calibration parameters to nonvolatile EEPROM on demo board
void store_calibration()
// Store the ADC calibration to the EEPROM
{
  eeprom_write_int16(EEPROM_I2C_ADDRESS, EEPROM_CAL_KEY, EEPROM_CAL_STATUS_ADDRESS);           // Cal key
  eeprom_write_int32(EEPROM_I2C_ADDRESS, LTC2484_offset_code, EEPROM_CAL_STATUS_ADDRESS + 2);  // Offset
  eeprom_write_float(EEPROM_I2C_ADDRESS, LTC2484_lsb, EEPROM_CAL_STATUS_ADDRESS + 6);          // LSB
  Serial.println(F("Calibration Stored to EEPROM"));
}