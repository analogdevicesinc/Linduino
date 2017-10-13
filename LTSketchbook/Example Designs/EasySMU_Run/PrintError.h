#ifndef PrintError_h
#define PrintError_h
#include <stdint.h>
void printError(int16_t  errorNum);

#define _PRINT_ERROR_VOLTAGE_SOURCE_SETTING 0
#define _PRINT_ERROR_CURRENT_SOURCE_SETTING 1
#define _PRINT_ERROR_SERIAL_COMMAND 2
#define _PRINT_ERROR_NO_CALIBRATION_DATA_FOUND 3
#define _PRINT_ERROR_FACTORY_CALIBRATION_RESTORE_FAILED_CATASTROPHICALLY

void printError(int16_t  errorNum)
{
  Serial.print(F("Error("));
  Serial.print(errorNum);
  Serial.println(")");
}
#endif
