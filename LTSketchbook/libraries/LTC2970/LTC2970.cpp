#include "LTC2970.h"

//! Set GPIO_n high
//! @return void
void ltc2970_gpio_up(LT_SMBus *smbus, uint8_t ltc2970_i2c_address, int gpio_number)
{
  uint16_t return_val;
  if (gpio_number == 0)
  {
    return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_IO);
    return_val = (return_val & 0xFEFE) | 0x0012;
    smbus->writeWord(ltc2970_i2c_address, LTC2970_IO, return_val);
  }
  else if (gpio_number == 1)
  {
    return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_IO);
    return_val = (return_val & 0xFEFB) | 0x0028;
    smbus->writeWord(ltc2970_i2c_address, LTC2970_IO, return_val);
  }
  else
  {
    // error, no such GPIO
  }
}

//! Set GPIO_n low
//! @return void
void ltc2970_gpio_down(LT_SMBus *smbus, uint8_t ltc2970_i2c_address, int gpio_number)
{
  uint16_t return_val;
  if (gpio_number == 0)
  {
    return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_IO);
    return_val = (return_val & 0xFEEE) | 0x0002;
    smbus->writeWord(ltc2970_i2c_address, LTC2970_IO, return_val);
  }
  else if (gpio_number == 1)
  {
    return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_IO);
    return_val = (return_val & 0xFEDB) | 0x0008;
    smbus->writeWord(ltc2970_i2c_address, LTC2970_IO, return_val);
  }
  else
  {
    // error, no such GPIO
  }
}

//! Disconnect a DAC from its channel
//! @return void
void ltc2970_dac_disconnect(LT_SMBus *smbus, uint8_t ltc2970_i2c_address, int dac_number)
{
  uint16_t return_val,
           idac_reg;

  if (dac_number == 0)
  {
    Serial.print(F("\nDISCONNECT CHANNEL 0 : "));
    return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_CH0_A_IDAC);
    idac_reg = 0xFCFF & return_val;
    smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_A_IDAC, idac_reg);
  }
  else if (dac_number == 1)
  {
    Serial.print(F("\nDISCONNECT CHANNEL 1 : "));
    return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_CH1_A_IDAC);
    idac_reg = 0xFCFF & return_val;
    smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_A_IDAC, idac_reg);
  }
  else
  {
    Serial.print(F("\nERROR CANNOT DISCONNECT NON-EXISTENT CHANNEL"));
    // error, no such DAC
  }
  Serial.print(F("\n\n"));
}

//! Unceremoniously connect DAC0 to the control node
//!  set DAC to a hard value
//!  no attempt to equalize voltages
//! @return void
void ltc2970_hard_connect_dac(LT_SMBus *smbus, uint8_t ltc2970_i2c_address, int dac_number, uint8_t dac_value_8)
{
  uint16_t return_val,
           idac_ctrl,
           idac_reg,
           dac_value = (uint16_t)dac_value_8;

  if (dac_number == 0)
  {
    Serial.print(F("\nHARD CONNECT CHANNEL 0 : "));
    return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_CH0_A_IDAC);
    idac_ctrl = 0x0300;
    idac_reg = (0xFC00 & return_val) | (0x00FF & dac_value) | idac_ctrl ;
    smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_A_IDAC, idac_reg);
  }
  else if (dac_number == 1)
  {
    Serial.print(F("\nHARD CONNECT CHANNEL 1 : "));
    return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_CH1_A_IDAC);
    idac_ctrl = 0x0300;
    idac_reg = (0xFC00 & return_val) | (0x00FF & dac_value) | idac_ctrl ;
    smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_A_IDAC, idac_reg);
  }
  else
  {
    Serial.print(F("\nERROR CANNOT HARD CONNECT NON-EXISTENT CHANNEL"));
    // error, no such DAC
  }
}

//! soft-connect DACn to its controlled node
//! @return int
int ltc2970_soft_connect_dac(LT_SMBus *smbus, uint8_t ltc2970_i2c_address, int dac_number)
{
  uint16_t return_val,
           idac_reg,
           idac_ctrl,
           idac_value;

  int soft_connect_delay = 1000; // milliseconds to wait for soft connect

  if (dac_number == 0)
  {
    // check for existing faults
    return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_FAULT);
    if ((return_val & 0x001B) == 0x0000)
    {
      // make sure that the channel is not already connected
      return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_CH0_A_IDAC);
      if ((return_val & 0x0300) == 0x0000)
      {
        // the soft-connect operation can succeed with no faults, setting IDAC[9] = 1
        // or it can fail, and not set IDAC[9]
        // we wait a safe amount of time, then check for results
        Serial.print(F("\nSOFT CONNECT CHANNEL 0 : "));
        idac_ctrl = 0x0100;
        idac_value = 0x0080; // soft connect will override this value automatically
        idac_reg = (0xFC00 & return_val) | (0x00FF & idac_value) | idac_ctrl ;
        smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_A_IDAC, idac_reg);
        delay(soft_connect_delay);
        return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_CH0_A_IDAC);
        if ((return_val & 0x0300) == 0x0000)
        {
          Serial.print(F("\nCHANNEL 0 FAILED TO CONNECT"));
          Serial.print(F("\n  FAULT REGISTER: "));
          return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_FAULT);
          Serial.print(F("0x"));
          Serial.println(return_val, HEX);
        }
        else
        {
          Serial.print(F("\nCHANNEL 0 SOFT CONNECT SUCCESS"));
          //          return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_CH0_A_ADC);
          //          servo0_value = (return_val & 0x7FFF);
        }
      }
      else
      {
        Serial.print(F("\nCHANNEL 0 ALREADY CONNECTED"));
      }
    }
    else
    {
      Serial.print(F("\nERROR: CANNOT SOFT-CONNECT WITH FAULTS ON CHANNEL 0: "));
      Serial.println(return_val, HEX);
    }
  }
  else if (dac_number == 1)
  {
    // check for existing faults
    return_val = smbus->readWord(ltc2970_i2c_address,LTC2970_FAULT);
    //    if ((return_val & 0x03E0) == 0x0000) {
    if ((return_val & 0x0360) == 0x0000)
    {
      // make sure that the channel is not already connected
      return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_CH1_A_IDAC);
      if ((return_val & 0x0300) == 0x0000)
      {
        // the soft-connect operation can succeed with no faults, setting IDAC[9] = 1
        // or it can fail, and not set IDAC[9]
        // we wait a safe amount of time, then check for results
        Serial.print(F("\nSOFT CONNECT CHANNEL 1 : "));
        idac_ctrl = 0x0100;
        idac_value = 0x0080; // soft connect will override this value automatically
        idac_reg = (0xFC00 & return_val) | (0x00FF & idac_value) | idac_ctrl ;
        smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_A_IDAC, idac_reg);
        delay(soft_connect_delay);
        return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_CH1_A_IDAC);
        if ((return_val & 0x0300) == 0x0000)
        {
          Serial.print(F("\nCHANNEL 1 FAILED TO CONNECT"));
          Serial.print(F("\n  FAULT REGISTER: "));
          return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_FAULT);
          Serial.print(F("0x"));
          Serial.println(return_val, HEX);
        }
        else
        {
          Serial.print(F("\nCHANNEL 1 SOFT CONNECT SUCCESS"));
          //          return_val = smbus->readWord(ltc2970_i2c_address, LTC2970_CH1_A_ADC);
          //          servo1_value = (return_val & 0x7FFF);
        }
      }
      else
      {
        Serial.print(F("\nCHANNEL 1 ALREADY CONNECTED"));
      }
    }
    else
    {
      Serial.print(F("\nERROR: CANNOT SOFT-CONNECT WITH FAULTS ON CHANNEL 1: "));
      Serial.println(return_val, HEX);
    }
  }
  else
  {
    Serial.print(F("\nERROR: CANNOT SOFT CONNECT NON-EXISTENT CHANNEL"));
    // error, no such DAC
  }
  Serial.print(F("\n\n"));
}


//! Servo once to a given ADC value
//! @return void
void ltc2970_servo_to_adc_val(LT_SMBus *smbus, uint8_t ltc2970_i2c_address, int channel_number, uint16_t code)
{
  uint16_t return_val;

  uint16_t code_in;
  //  uint16_t code_max = 0x18B7,
  //           code_min = 0x0AE0;

  // get rid of bit 15 to make limit calculations easier
  // note that limits may be different per channel
  code_in = (code & 0x7FFF);
  //  code_in = (code_in > code_max) ? code_max : code_in;
  //  code_in = (code_in < code_min) ? code_min : code_in;

  // ensure that bit 15 is high to enable servoing
  code_in = (code_in + 0x8000);

  if (channel_number == 0)
  {
    Serial.print(F("\nSERVO CHANNEL 0."));
    smbus->writeWord(ltc2970_i2c_address, LTC2970_CH0_A_SERVO, code_in);

  }
  else if (channel_number == 1)
  {
    Serial.print(F("\nSERVO CHANNEL 1."));
    smbus->writeWord(ltc2970_i2c_address, LTC2970_CH1_A_SERVO, code_in);
  }
  else
  {
    Serial.print(F("\nERROR CANNOT SERVO NON-EXISTENT CHANNEL"));
    // error, no such channel
  }
  Serial.print(F("\n\n"));
}

//! Prints die temperature on the LTC2970
//! @return void
void ltc2970_print_die_temp (LT_SMBus *smbus, uint8_t ltc2970_i2c_address)
{
  static float temp_scale = 4;
  static float temp_offset = 1093; //adc codes

  float temperature;
  uint16_t return_val;
  //print the on-die temperature for the LTC2970
  return_val =
    smbus->readWord(ltc2970_i2c_address, LTC2970_TEMP_ADC);
  return_val = return_val & 0x7FFF; // drop bit 15

  temperature = ((float(return_val) - temp_offset) / temp_scale);

  Serial.print(F("\n LTC_2970 DIE TEMP: "));
  Serial.println(temperature, DEC);
  Serial.print(F("        HEX VALUE : 0x"));
  Serial.println(return_val, HEX);
  Serial.print(F("\n\n"));
}

//! Read FAULT, FAULT_LA, and FAULT_LA_INDEX registers
//!  print the results
//! @return void
void ltc2970_read_faults(LT_SMBus *smbus, uint8_t ltc2970_i2c_address)
{
  uint16_t return_val;

  return_val =
    smbus->readWord(ltc2970_i2c_address,LTC2970_FAULT);
  Serial.print(F("\n LTC2970_FAULT: "));
  Serial.print(F("0x"));
  Serial.println(return_val, HEX);
  return_val =
    smbus->readWord(ltc2970_i2c_address,LTC2970_FAULT_LA);
  Serial.print(F("\n LTC2970_FAULT_LA (CLEARED ON READ): "));
  Serial.print(F("0x"));
  Serial.println(return_val, HEX);
  return_val =
    smbus->readWord(ltc2970_i2c_address,LTC2970_FAULT_LA_INDEX);
  Serial.print(F("\n LTC2970_FAULT_LA_INDEX: "));
  Serial.print(F("0x"));
  Serial.println(return_val, HEX);

}

