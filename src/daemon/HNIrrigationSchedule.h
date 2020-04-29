#ifndef _HN_IRRIGATION_SCHEDULE_H_
#define _HN_IRRIGATION_SCHEDULE_H_

#include <stdint.h>
#include <unistd.h>

#include <string>
#include <list>
#include <vector>

#include "HNIrrigationZone.h"

// Forward Declaration
class HNISDay;

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
        HNIS_EXCLUDE_TYPE_T type;

        HNI24HTime startTime;
        HNI24HTime endTime;
      
        HNIS_DAY_INDX_T dayIndx;

    public:
        HNExclusionSpec();
       ~HNExclusionSpec();

        HNIS_EXCLUDE_TYPE_T getType();

        HNI24HTime &getStartTime();
        HNI24HTime &getEndTime();

        HNIS_DAY_INDX_T getDayIndex();
};


typedef enum HNISPeriodTypeEnum
{
    HNIS_PERIOD_TYPE_EXCLUSION,
    HNIS_PERIOD_TYPE_ZONE_ON
}HNIS_PERIOD_TYPE_T;

class HNISPeriod
{
    private:
        HNIS_PERIOD_TYPE_T type;

        HNI24HTime startTime;
        HNI24HTime endTime;

        HNIrrigationZone *zone;

    protected:
        static bool sortCompare( const HNISPeriod& first, const HNISPeriod& second );

    public:
        HNISPeriod();
       ~HNISPeriod();

        void setType( HNIS_PERIOD_TYPE_T value );

        HNIS_RESULT_T setStartTime( HNI24HTime &time );
        HNIS_RESULT_T setEndTime( HNI24HTime &time );

        HNI24HTime& getStartTime();
        HNI24HTime& getEndTime();

    friend HNISDay;
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
        HNIS_DAY_INDX_T dindx;
        std::string    dayName;
  
        std::list< HNISPeriod > periodList;

        HNIS_CAR_T assessCollision( HNISPeriod &value, bool moveLater, uint &boundary );

    public:
        HNISDay();
       ~HNISDay();

        void clear();

        void sort();
        void coalesce();

        HNIS_RESULT_T addPeriod( HNISPeriod &value );

        HNIS_RESULT_T scheduleTimeSlots( uint totalSeconds, uint cycleCnt, HNIrrigationZone &zone );
};

class HNIrrigationSchedule
{
    private:

        std::string timezone;

        HNISDay  dayArr[ HNIS_DAY_CNT ];

    public:
        HNIrrigationSchedule();
       ~HNIrrigationSchedule();

        HNIS_RESULT_T buildSchedule( std::vector< HNExclusionSpec > &exclusionList, std::vector< HNIrrigationZone > &zoneList );
};

#endif // _HN_IRRIGATION_SCHEDULE_H_

