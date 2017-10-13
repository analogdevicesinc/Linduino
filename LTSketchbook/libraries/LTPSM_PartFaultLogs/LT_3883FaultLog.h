/*!
LTC SMBus Support: Implementation for a LTC3883 Fault Log

@verbatim

This API is shared with Linduino and RTOS code. End users should code to this
API to enable use of the PMBus code without modifications.

@endverbatim

REVISION HISTORY
$Revision: 4593 $
$Date: 2016-01-20 10:09:54 -0800 (Wed, 20 Jan 2016) $

Copyright (c) 2014, Linear Technology Corp.(LTC)
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
*/

/*! @file
    @ingroup LT_3883FaultLog
    Library Header File for LT_3883FaultLog
*/

#ifndef LT_3883FaultLog_H_
#define LT_3883FaultLog_H_

#include "../LT_PMBUS/LT_PMBus.h"
#include "../LT_PMBUS/LT_PMBusMath.h"
#include "LT_EEDataFaultLog.h"


//! class that handles LTC3883 fault logs.
//! contains structs for interpreting the data read from the part.
class LT_3883FaultLog : public LT_EEDataFaultLog
{

  public:
#pragma pack(push, 1)


    /********************************************************************
     * LTC3883 types
     ********************************************************************/

    struct FaultLogTelemetrySummaryLtc3883
    {
      public:
        struct Lin16WordReverse mfr_vout_peak_p0;
        struct Lin16WordReverse not_used0;

        struct Lin5_11WordReverse mfr_iout_peak_p0;
        struct Lin5_11WordReverse mfr_iin_peak_p0;

        struct Lin5_11WordReverse mfr_vin_peak;
        struct Lin5_11WordReverse read_temperature_1_p0;
        struct Lin5_11WordReverse not_used1;
        struct Lin5_11WordReverse read_temperature_2;
        struct Lin5_11WordReverse read_temperature_1_peak_p0;
        struct Lin5_11WordReverse not_used2;
    };

    struct FaultLogPreambleLtc3883
    {
      public:
        uint8_t position_fault;
        struct FaultLogTimeStamp shared_time;
        struct FaultLogTelemetrySummaryLtc3883 peaks;
    };

    struct FaultLogReadLoopLtc3883
    {
      public:
        struct Lin16WordReverse read_vout_p0;
        struct Lin16WordReverse not_used0;

        struct Lin5_11WordReverse read_iout_p0;
        struct Lin5_11WordReverse read_iin_chan_p1;

        struct Lin5_11WordReverse read_vin ;
        struct Lin5_11WordReverse read_iin;

        struct RawByte status_vout_p0;
        struct RawByte not_used1;

        struct RawWordReverse status_word_p0;
        struct RawWordReverse not_used2;

        struct RawByte status_mfr_specificP0;
        struct RawByte not_used3;
    };

    struct FaultLogLtc3883
    {
      public:
        struct FaultLogPreambleLtc3883 preamble;
        struct FaultLogReadLoopLtc3883 fault_log_loop[6];
    };

#pragma pack(pop)

  protected:
    FaultLogLtc3883   *faultLog3883;

  public:
    //! Constructor
    LT_3883FaultLog(LT_PMBus *pmbus //!< pmbus object reference for this fault log handler to use.
                   );

    //! Pretty prints this part's fault log to a Print inheriting object, or Serial if none specified.
    void print(Print *printer = 0 //!< Print inheriting object to print the fault log to.
              );

    //! Get binary of the fault log or NULL if no log
    uint8_t *getBinary();

    //! Get size of binary data
    uint16_t getBinarySize();

    //! Dumps binary of the fault log to a Print inheriting object, or Serial if none specified.
    void dumpBinary(Print *printer = 0  //!< Print inheriting object to print the binary to.
                   );

    //! Reads the fault log from the specified address, reserves memory to hold the data.
    //! @return a reference to the data read from the part.
    void read(uint8_t address  //!< the address to read the fault log from.
             );

    // ! Get the fault log data
    struct FaultLogLtc3883 *get()
    {
      return faultLog3883;
    }

    //! Frees the memory reserved for the fault log.
    void release();

  private:
    char *buffer;

    void printTitle(Print *);
    void printTime(Print *);
    void printPeaks(Print *);
    void printAllLoops(Print *);
    void printLoop(uint8_t index, Print *);

};

#endif /* LT_FaultLog_H_ */
