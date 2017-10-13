#include "LT_PMBusDeviceLTC3882.h"

uint32_t LT_PMBusDeviceLTC3882::cap_ =  HAS_VOUT
                                        | HAS_VIN
                                        | HAS_IOUT
                                        | HAS_POUT
                                        | HAS_TEMP
                                        | HAS_DC
                                        | HAS_STATUS_WORD
                                        | HAS_STATUS_EXT
                                        ;