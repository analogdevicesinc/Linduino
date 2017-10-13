/*!
LTC SMBus Support: Implementation for a LTC3887 Fault Log

@verbatim

This API is shared with Linduino and RTOS code. End users should code to this
API to enable use of the PMBus code without modifications.

@endverbatim

REVISION HISTORY
$Revision: 4376 $
$Date: 2015-11-20 14:31:33 -0700 (Fri, 20 Nov 2015) $

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
    @ingroup LT_3887FaultLog
    Library Header File for LT_3887FaultLog
*/

#ifndef LT_3887FaultLog_H_
#define LT_3887FaultLog_H_

#include "../LT_PMBUS/LT_PMBus.h"
#include "../LT_PMBUS/LT_PMBusMath.h"
#include "LT_EEDataFaultLog.h"


//! class that handles LTC3887 fault logs.
//! contains structs for interpreting the data read from the part.
class LT_3887FaultLog : public LT_EEDataFaultLog
{

  public:
#pragma pack(push, 1)


    /********************************************************************
     * LTC3887 types
     ********************************************************************/

    struct FaultLogTelemetrySummaryLtc3887
    {
      public:
        struct Lin16WordReverse mfr_vout_peak_p0;
        struct Lin16WordReverse mfr_vout_peak_p1;

        struct Lin5_11WordReverse mfr_iout_peak_p0;
        struct Lin5_11WordReverse mfr_iout_peak_p1;

        struct Lin5_11WordReverse mfr_vin_peak;
        struct Lin5_11WordReverse read_temperature_1_p0;
        struct Lin5_11WordReverse read_temperature_1_p1;
        struct Lin5_11WordReverse read_temperature_2;
    };

    struct FaultLogPreambleLtc3887
    {
      public:
        uint16_t lt;
        uint16_t mfr_special_id;
        uint8_t position_fault;
        struct FaultLogTimeStamp shared_time;
        struct FaultLogTelemetrySummaryLtc3887 peaks;
    };

    struct FaultLogReadLoopLtc3887
    {
      public:
        struct Lin16WordReverse read_vout_p0;
        struct Lin16WordReverse read_vout_p1;

        struct Lin5_11WordReverse read_iout_p0;
        struct Lin5_11WordReverse read_iout_p1;

        struct Lin5_11WordReverse read_vin ;
        struct Lin5_11WordReverse read_iin;

        struct RawByte status_vout_p0;
        struct RawByte status_vout_p1;

        struct RawWordReverse status_word_p0;
        struct RawWordReverse status_word_p1;

        struct RawByte status_mfr_specificP0;
        struct RawByte status_mfr_specificP1;
    };

    struct FaultLogLtc3887
    {
      public:
        struct FaultLogPreambleLtc3887 preamble;
        struct FaultLogReadLoopLtc3887 fault_log_loop[6];
    };

#pragma pack(pop)

  protected:
    FaultLogLtc3887   *faultLog3887;

  public:
    //! Constructor
    LT_3887FaultLog(LT_PMBus *pmbus   //!< pmbus object reference for this fault log handler to use.
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
    struct FaultLogLtc3887 *get()
    {
      return faultLog3887;
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
