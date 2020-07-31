#ifndef __HN_SWD_STATUS_H__
#define __HN_SWD_STATUS_H__

#include <stdint.h>

#include <string>

class HNSWDStatus
{
    private:

        std::string m_date;
        std::string m_time;
        std::string m_tz;
        std::string m_swON;

        std::string m_schState;
        std::string m_inhUntil;

        std::string m_schUIStr;    // Scheduler Update Index
        std::string m_schCRC32Str;
        //schCRC32 = strtol( schCRC32Str.c_str(), NULL, 0 );
        uint m_schCRC32;
               
        std::string m_ohstat;
        std::string m_ohmsg; 

    public:
        HNSWDStatus();
       ~HNSWDStatus();

        uint getSMCRC32();
        std::string getSMCRC32Str();

        bool healthDegraded();

        void setFromJSON( std::string jsonStr );
};

#endif // __HN_SWD_STATUS_H__
