#ifndef _HN_IRRIGATION_ZONE_H_
#define _HN_IRRIGATION_ZONE_H_

#include <stdint.h>
#include <unistd.h>

#include <string>
#include <list>

typedef enum HNIrrigationZoneResultEnum
{
    HNIZ_RESULT_SUCCESS,
    HNIZ_RESULT_FAILURE
}HNIZ_RESULT_T;

class HNI24HTime
{
    private:
        uint secOfDay;

    public:
        HNI24HTime();
       ~HNI24HTime();

        HNIZ_RESULT_T setFromHMS( uint hour, uint minute, uint second );
        HNIZ_RESULT_T setFromSeconds( uint seconds );
        HNIZ_RESULT_T parseTime( std::string value );

        void addSeconds( uint seconds );
        void addDuration( HNI24HTime &duration );

        uint getSeconds() const;
        void getHMS( uint &hour, uint &minute, uint &second );
        std::string getHMSStr();
};

class HNIrrigationZone
{
    private:
        std::string m_zoneID;
        std::string m_zoneName;
        std::string m_zoneDesc;

        HNI24HTime m_totalWeeklyDuration;
        HNI24HTime m_minOnDuration;
        HNI24HTime m_maxOnDuration;

        std::string m_swidList;

    public:
        HNIrrigationZone();
       ~HNIrrigationZone();

        void setID( std::string id );
        void setName( std::string name );
        void setDesc( std::string desc );
       
        void setSWIDList( std::string swidList );

        std::string getID();
        std::string getName();
        std::string getDesc();

        std::string getSWIDListStr();

        uint getWeeklySeconds();
        uint getTargetCyclesPerDay();
        uint getMinimumCycleTimeSeconds();
};

#endif // _HN_IRRIGATION_ZONE_H_

