#ifndef _HN_IRRIGATION_ZONE_H_
#define _HN_IRRIGATION_ZONE_H_

#include <stdint.h>
#include <unistd.h>

#include <string>
#include <list>

typedef enum HNIrrigationZoneErrorEnum
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
        std::string zoneID;
        std::string zoneName;
        std::string zoneDesc;

        HNI24HTime totalWeeklyDuration;
        HNI24HTime minOnDuration;
        HNI24HTime maxOnDuration;

        std::string swID;

    public:
        HNIrrigationZone();
       ~HNIrrigationZone();



};

typedef enum HNISPeriodTypeEnum
{
    HNIS_PERIOD_TYPE_BLACKOUT,
    HNIS_PERIOD_TYPE_ZONE_ON
}HNIS_PERIOD_TYPE_T;

class HNISPeriod
{
    private:
        HNIS_PERIOD_TYPE_T type;

        HNI24HTime startTime;
        HNI24HTime endTime;

        HNIrrigationZone *zone;

    public:
        HNISPeriod();
       ~HNISPeriod();
};

typedef enum HNIrrigationScheduleDayIndexEnum
{
    HNIS_DINDX_SUNDAY    = 0,
    HNIS_DINDX_MONDAY    = 1,
    HNIS_DINDX_TUESDAY   = 2,
    HNIS_DINDX_WEDNESDAY = 3,
    HNIS_DINDX_THURSDAY  = 4,
    HNIS_DINDX_FRIDAY    = 5,
    HNIS_DINDX_SATURDAY  = 6,
    HNIS_DAY_CNT         = 7
}HNIS_DAY_INDX_T;

class HNISDay
{
    private:
        HNIS_DAY_INDX_T dindx;
        std::string    dayName;
  
        std::list< HNISPeriod > periodList;

    public:
        HNISDay();
       ~HNISDay();

};

class HNIrrigationSchedule
{
    private:

        std::string timezone;

        HNISDay  dayArr[ HNIS_DAY_CNT ];

    public:
        HNIrrigationSchedule();
       ~HNIrrigationSchedule();

};

#endif // _HN_IRRIGATION_ZONE_H_

