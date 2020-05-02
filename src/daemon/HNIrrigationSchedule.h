#ifndef _HN_IRRIGATION_SCHEDULE_H_
#define _HN_IRRIGATION_SCHEDULE_H_

#include <stdint.h>
#include <unistd.h>

#include <string>
#include <list>
#include <vector>

#include "HNIrrigationZone.h"

// Forward Declaration
//class HNISDay;
//class HNIrrigationSchedule;

typedef enum HNIrrigationScheduleResultEnum
{
    HNIS_RESULT_SUCCESS,
    HNIS_RESULT_FAILURE
}HNIS_RESULT_T;

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

typedef enum HNExclusionSpecTypeEnum
{
    HNIS_EXCLUDE_TYPE_EVERYDAY,
    HNIS_EXCLUDE_TYPE_SINGLE
}HNIS_EXCLUDE_TYPE_T;

class HNExclusionSpec
{
    private:
        HNIS_EXCLUDE_TYPE_T m_type;

        HNI24HTime m_startTime;
        HNI24HTime m_endTime;
      
        HNIS_DAY_INDX_T m_dayIndx;

    public:
        HNExclusionSpec();
       ~HNExclusionSpec();

        void setType( HNIS_EXCLUDE_TYPE_T value );

        HNIS_RESULT_T setTimesFromStr( std::string startTime, std::string endTime ); 

        HNIS_EXCLUDE_TYPE_T getType();

        HNI24HTime &getStartTime();
        HNI24HTime &getEndTime();

        HNIS_DAY_INDX_T getDayIndex();
};


typedef enum HNISPeriodTypeEnum
{
    HNIS_PERIOD_TYPE_NOTSET,
    HNIS_PERIOD_TYPE_EXCLUSION,
    HNIS_PERIOD_TYPE_ZONE_ON
}HNIS_PERIOD_TYPE_T;

class HNISPeriod
{
    private:
        HNIS_PERIOD_TYPE_T m_type;

        HNI24HTime m_startTime;
        HNI24HTime m_endTime;

        HNIrrigationZone *m_zone;

    public:
        HNISPeriod();
       ~HNISPeriod();

        void setType( HNIS_PERIOD_TYPE_T value );
        void setZone( HNIrrigationZone *zone );

        HNIS_RESULT_T setStartTime( HNI24HTime &time );
        HNIS_RESULT_T setEndTime( HNI24HTime &time );

        HNIS_RESULT_T setTimesFromStr( std::string startTime, std::string endTime ); 

        HNIS_PERIOD_TYPE_T getType();
        HNIrrigationZone *getZone();

        HNI24HTime& getStartTime();
        std::string getStartTimeStr();

        HNI24HTime& getEndTime();
        std::string getEndTimeStr();

        static bool sortCompare( const HNISPeriod& first, const HNISPeriod& second );

};

typedef enum HNISDayCollisionAssessResultEnum
{
    HNIS_CAR_CONTAINED,
    HNIS_CAR_BEFORE,
    HNIS_CAR_AFTER,
    HNIS_CAR_NONE
}HNIS_CAR_T;

class HNISDay
{
    private:
        HNIS_DAY_INDX_T m_dayIndex;
  
        std::list< HNISPeriod > m_periodList;

        HNIS_CAR_T assessCollision( HNISPeriod &value, bool moveLater, uint &boundary );

    public:
        HNISDay();
       ~HNISDay();

        void setIndex( HNIS_DAY_INDX_T dayIndex );

        void clear();

        void sort();
        void coalesce();

        HNIS_RESULT_T addPeriod( HNISPeriod value );

        HNIS_RESULT_T scheduleTimeSlots( uint totalSeconds, uint cycleCnt, HNIrrigationZone *zone );

        std::string getDayName();

        void getPeriodList( std::vector< HNISPeriod > &periodList );

};

class HNIrrigationSchedule
{
    private:

        std::string timezone;

        HNISDay  dayArr[ HNIS_DAY_CNT ];

    public:
        HNIrrigationSchedule();
       ~HNIrrigationSchedule();

        void clear();

        HNIS_RESULT_T addExclusion( HNExclusionSpec *exclusion );

        HNIS_RESULT_T addZone( HNIrrigationZone *zone );

        //HNIS_RESULT_T buildSchedule( std::vector< HNExclusionSpec > &exclusionList, std::vector< HNIrrigationZone > &zoneList );

        std::string getSwitchDaemonJSON();
};

#endif // _HN_IRRIGATION_SCHEDULE_H_

