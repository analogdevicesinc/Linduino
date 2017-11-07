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
        Serial.println();
    }
}

void loop()
{
    print_prompt();
    
        uint8_t user_command = read_int();
        Serial.println(user_command);
        Serial.flush();
    
        switch (user_command)
        {
        case 1:
            menu_1_read_temperature();
            break;
    
        case 2:
            menu_2_set_resolution();
            break;
    
        case 3:
            //menu_3_set_op_mode();
            break;
    
        case 4:
            //menu_4_bunchoftemps();
            break;
    
        case 5:
            //menu_5_critical();
            break;
    
        case 9:
            //EEPROM_WRITE_TEST();
            break;
    
        default:
            Serial.println(F("Invalid option"));
            break;
        }
}

void print_title()
{
    Serial.println(F("*****************************************************************"));
    Serial.println(F("* EVAL-7420SDZ Demonstration Program                            *"));
    Serial.println(F("*                                                               *"));
    Serial.println(F("* This program demonstrates communication with the ADT7420      *"));
    Serial.println(F("* high accuracy digital temperature sensor                      *"));
    Serial.println(F("*                                                               *"));
    Serial.println(F("* Set the baud rate to 115200 select the newline terminator.    *"));
    Serial.println(F("*****************************************************************"));
}

void print_prompt()
{
    Serial.println(F("\nCommand Summary:"));

    Serial.println(F("  1- Read temperature"));
    Serial.println(F("  2- Set resolution"));
    Serial.println(F("  3- Set operation mode"));
    Serial.println(F("  4- Poll for a bunch of temperatures"));
    Serial.println(F("  5- Get critical temperature setting"));
    Serial.println(F("  9- Test & clear Linduino EEPROM"));
    Serial.println();

    Serial.print(F("Enter a command: "));
}

uint8_t menu_1_read_temperature()
{
    float temp = adt7420_get_temperature(device);

    Serial.print("Current temperature: ");
    Serial.print(temp);
    Serial.println(F(" C"));
}

uint8_t menu_2_set_resolution()
{
    Serial.println(F("  Available resolutions:"));
    Serial.println(F("    1- 13-bit"));
    Serial.println(F("    2- 16-bit"));
    Serial.print(F("  Select an option: "));

    uint8_t new_res = read_int();
    Serial.println(new_res);

    new_res = (new_res == 1) ? 0 : 1;

    adt7420_set_resolution(device, new_res);

    Serial.print(F("Set resolution to "));
    Serial.print((13 + 3 * new_res));
    Serial.println(F("-bit"));

    //EEPROM.write(64, 1);
    //EEPROM.write(65, new_res);

    return 0;
}