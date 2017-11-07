#include <Arduino.h>
#include <stdint.h>
#include <Linduino.h>
#include <UserInterface.h>
#include <EEPROM.h>
#include <platform_drivers.h>
extern "C" {
#include "adt7420.h"
};

adt7420_init_param init_params = {
    GENERIC_I2C,    // i2c type
    0,              // i2c id
    100000,         // i2c max speed (hz)
    0x48,           // i2c slave address
    0,              // resolution setting
};

adt7420_dev * device;

void setup()
{
    //int32_t ret = adt7420_init(&device, init_params);
    Serial.begin(115200);
    Serial.println("Foobar");

    int32_t ret = i2c_init(nullptr);   

    Serial.println(ret);

    //Serial.println(ret);
}

void loop()
{
    
}