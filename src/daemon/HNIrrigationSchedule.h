#ifndef _HN_IRRIGATION_SCHEDULE_H_
#define _HN_IRRIGATION_SCHEDULE_H_

#include <stdint.h>
#include <unistd.h>

#include <string>
#include <list>
#include <vector>

#include <hnode2/HNodeConfig.h>

#define HNIS_SECONDS_IN_24H  (24 * 60 *60)

typedef enum HNIrrigationScheduleResultEnum
{
    HNIS_RESULT_SUCCESS,
    HNIS_RESULT_FAILURE,
    HNIS_RESULT_SCH_CONTINUE,
    HNIS_RESULT_SCH_NONE,
    HNIS_RESULT_NO_SLOT
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

class HNI24HTime
{
    private:
        uint secOfDay;

    public:
        HNI24HTime();
       ~HNI24HTime();

        HNIS_RESULT_T setFromHMS( uint hour, uint minute, uint second );
        HNIS_RESULT_T setFromSeconds( uint seconds );
        HNIS_RESULT_T parseTime( std::string value );

        void addSeconds( uint seconds );
        void subtractSeconds( uint seconds );

        uint getSeconds() const;
        void getHMS( uint &hour, uint &minute, uint &second );
        std::string getHMSStr();
};

typedef enum HNExclusionSpecTypeEnum
{
    HNIS_EXCLUDE_TYPE_EVERYDAY,
    HNIS_EXCLUDE_TYPE_SINGLE
}HNIS_EXCLUDE_TYPE_T;

class HNExclusionSpec
{
    private:
        std::string m_id;
        HNIS_EXCLUDE_TYPE_T m_type;

        HNI24HTime m_startTime;
        HNI24HTime m_endTime;
      
        HNIS_DAY_INDX_T m_dayIndx;

    public:
        HNExclusionSpec();
       ~HNExclusionSpec();

        void setID( std::string id );

        void setType( HNIS_EXCLUDE_TYPE_T value );

        HNIS_RESULT_T setTimesFromStr( std::string startTime, std::string endTime ); 

        std::string getID();

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
        std::string m_id;

        HNIS_PERIOD_TYPE_T m_type;

        HNI24HTime m_startTime;
        HNI24HTime m_endTime;

        bool m_slideLater;

    public:
        HNISPeriod();
       ~HNISPeriod();

        void setID( std::string id );
        void setType( HNIS_PERIOD_TYPE_T value );

        void setSlideLater( bool value );

        void setStartTime( HNI24HTime &time );
        void setStartTimeSeconds( uint seconds );

        void setEndTime( HNI24HTime &time );
        void setEndTimeSeconds( uint seconds );

        void setTimesFromStr( std::string startTime, std::string endTime ); 

        std::string getID();
        HNIS_PERIOD_TYPE_T getType();

        HNI24HTime& getStartTime();
        uint getStartTimeSeconds();
        std::string getStartTimeStr();

        HNI24HTime& getEndTime();
        uint getEndTimeSeconds();
        std::string getEndTimeStr();

        bool isSlideLater();

        void moveStartToSecond( uint seconds );
        void moveEndToSecond( uint seconds );

        static bool sortCompare( const HNISPeriod& first, const HNISPeriod& second );

};

class HNIZScheduleState
{
    private:
        bool m_nextTop;

        uint m_lastBottomSec;
        uint m_lastTopSec;

    public:
        HNIZScheduleState();
       ~HNIZScheduleState();

        void setNextTop( bool value );
        void setTopSeconds( uint value );
        void setBottomSeconds( uint value );

        void toggleNextTop();
        bool isTopNext();

        uint getTopSeconds();
        uint getBottomSeconds();
};

class HNIrrigationZone
{
    private:
        std::string m_zoneID;
        std::string m_zoneName;
        std::string m_zoneDesc;

        uint m_weeklySec;
        uint m_dailyCycles;
        uint m_minCycleSec;

        std::string m_swidList;

    public:
        HNIrrigationZone();
       ~HNIrrigationZone();

        void setID( std::string id );
        void setName( std::string name );
        void setDesc( std::string desc );
       
        void setWeeklySeconds( uint value );
        void setTargetCyclesPerDay( uint value );
        void setMinimumCycleTimeSeconds( uint value );

        void setSWIDList( std::string swidList );

        HNIS_RESULT_T validateSettings();

        std::string getID();
        std::string getName();
        std::string getDesc();

        std::string getSWIDListStr();

        uint getWeeklySeconds();
        uint getTargetCyclesPerDay();
        uint getMinimumCycleTimeSeconds();

        HNIS_RESULT_T getNextSchedulingPeriod( uint dayIndex, uint cycleIndex, HNIZScheduleState &schState, HNISPeriod &tgtPeriod );
        HNIS_RESULT_T accountPeriodPlacement( uint dayIndex, uint cycleIndex, HNIZScheduleState &schState, HNISPeriod &tgtPeriod );
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

        HNIS_CAR_T assessCollision( HNISPeriod &value, uint &boundary );

    public:
        HNISDay();
       ~HNISDay();

        void setIndex( HNIS_DAY_INDX_T dayIndex );

        void clear();

        void sort();
        void coalesce();

        HNIS_RESULT_T addPeriod( HNISPeriod value );

        HNIS_RESULT_T scheduleTimeSlots( uint cycleIndex, HNIZScheduleState &state, HNIrrigationZone &zone );

        std::string getDayName();

        void getPeriodList( std::vector< HNISPeriod > &periodList );

};

class HNIrrigationSchedule
{
    private:

        std::string m_timezone;

        HNISDay  m_dayArr[ HNIS_DAY_CNT ];

        std::map< std::string, HNExclusionSpec >   m_exclusionMap;
        std::map< std::string, HNIrrigationZone >  m_zoneMap;

    public:
        HNIrrigationSchedule();
       ~HNIrrigationSchedule();

        void clear();

        HNExclusionSpec  *updateExclusion( std::string id );


        bool hasZone( std::string zoneID );
        HNIrrigationZone *updateZone( std::string zoneID );
        void deleteZone( std::string zoneID );
        void getZoneList( std::vector< HNIrrigationZone > &zoneList );
        HNIS_RESULT_T getZone( std::string zoneID, HNIrrigationZone &zone );

        HNIS_RESULT_T initZoneListSection( HNodeConfig &cfg );
        HNIS_RESULT_T readZoneListSection( HNodeConfig &cfg );
        HNIS_RESULT_T updateZoneListSection( HNodeConfig &cfg );

        HNIS_RESULT_T buildSchedule();

        std::string getSwitchDaemonJSON();
};

#endif // _HN_IRRIGATION_SCHEDULE_H_

