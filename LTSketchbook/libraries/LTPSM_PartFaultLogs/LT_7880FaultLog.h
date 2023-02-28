/*!
LTC SMBus Support: Implementation for a LTC7880 Fault Log

@verbatim

This API is shared with Linduino and RTOS code. End users should code to this
API to enable use of the PMBus code without modifications.

@endverbatim


Copyright 2021(c) Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
 - Neither the name of Analog Devices, Inc. nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.
 - The use of this software may or may not infringe the patent rights
   of one or more patent holders.  This license does not release you
   from the requirement that you obtain separate licenses from these
   patent holders to use this software.
 - Use of the software either in source or binary form, must be run
   on or directly connected to an Analog Devices Inc. component.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*! @file
    @ingroup LT_7880FaultLog
    Library Header File for LT_7880FaultLog
*/

#ifndef LT_7880FaultLog_H_
#define LT_7880FaultLog_H_

#include "../LT_PMBUS/LT_PMBus.h"
#include "../LT_PMBUS/LT_PMBusMath.h"
#include "LT_EEDataFaultLog.h"

//! class that handles LTC7880 fault logs.
//! contains structs for interpreting the data read from the part.
class LT_7880FaultLog : public LT_EEDataFaultLog
{

  public:
#pragma pack(push, 1)


    /********************************************************************
     * LTC7880 types
     ********************************************************************/

    struct FaultLogTelemetrySummaryLtc7880
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

        // uint8_t space0;
        // uint8_t space1;
        // uint8_t space2;
        // uint8_t space3;
        // uint8_t space4;
        // uint8_t space5;
        // uint8_t space6;
        // uint8_t space7;
    };

    struct FaultLogPreambleLtc7880
    {
      public:
        uint16_t lt;
        uint16_t mfr_special_id;
        uint8_t position_fault;
        struct FaultLogTimeStamp shared_time;
        struct FaultLogTelemetrySummaryLtc7880 peaks;
    };

    struct FaultLogReadLoopLtc7880
    {
      public:
        struct Lin16WordReverse read_vout_p0;
        struct Lin16WordReverse read_vout_p1;

        struct Lin5_11WordReverse read_iout_p0;
        struct Lin5_11WordReverse read_iout_p1;

        struct Lin5_11WordReverse read_vin;

        // Not used. Reserved.
        struct Lin5_11WordReverse read_iin;

        struct RawByte status_vout_p0;
        struct RawByte status_vout_p1;

        struct RawWordReverse status_word_p0;
        struct RawWordReverse status_word_p1;

        struct RawByte status_mfr_specificP0;
        struct RawByte status_mfr_specificP1;
    };

    struct FaultLogLtc7880
    {
      public:
        struct FaultLogPreambleLtc7880 preamble;
        struct FaultLogReadLoopLtc7880 fault_log_loop[6];
    };

#pragma pack(pop)

  protected:
    FaultLogLtc7880   *faultLog7880;

  public:
    //! Constructor
    LT_7880FaultLog(LT_PMBus *pmbus   //!< pmbus object reference for this fault log handler to use.
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
    struct FaultLogLtc7880 *get()
    {
      return faultLog7880;
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
