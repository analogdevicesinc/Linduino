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
    Serial.begin(115200);
    Serial.println("Welcome to the new ADT7420 testing thingy");

    int32_t ret = adt7420_init(&device, init_params);

    if(ret != SUCCESS)
    {
        Serial.println("Connection to device failed :(");
    }
    else
    {
        Serial.println("Connection to device succeeded!");

        float temp = adt7420_get_temperature(device);
        Serial.print("Current temp is: ");
        Serial.println(temp);
    }
}

void loop()
{
    
}