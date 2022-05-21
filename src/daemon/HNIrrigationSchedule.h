#ifndef _HN_IRRIGATION_SCHEDULE_H_
#define _HN_IRRIGATION_SCHEDULE_H_

#include <stdint.h>
#include <unistd.h>

#include <string>
#include <list>
#include <vector>
#include <set>

#include <hnode2/HNodeConfig.h>

#include "HNIrrigationTypes.h"
#include "HNIrrigationPlacement.h"
#include "HNIrrigationZone.h"
#include "HNIrrigationModifier.h"

typedef enum HNISPeriodTypeEnum
{
    HNIS_PERIOD_TYPE_NOTSET,
    HNIS_PERIOD_TYPE_AVAILABLE,
    HNIS_PERIOD_TYPE_ZONE_ON
}HNIS_PERIOD_TYPE_T;

class HNISPeriod
{
    private:
        std::string m_id;

        HNIS_PERIOD_TYPE_T m_type;

        HNIS_DAY_INDX_T m_dayIndx;

        HNI24HTime m_startTime;
        HNI24HTime m_endTime;

        std::set< std::string > m_zoneSet;

    public:
        HNISPeriod();
       ~HNISPeriod();

        void setID( std::string id );
        void setType( HNIS_PERIOD_TYPE_T value );

        void setDayIndex( HNIS_DAY_INDX_T dindx );
        void setStartTime( HNI24HTime &time );
        void setStartTimeSeconds( uint seconds );

        void setEndTime( HNI24HTime &time );
        void setEndTimeSeconds( uint seconds );

        void setTimesFromStr( std::string startTime, std::string endTime ); 

        std::string getID();
        HNIS_PERIOD_TYPE_T getType();

        HNIS_DAY_INDX_T getDayIndex();

        HNI24HTime& getStartTime();
        uint getStartTimeSeconds();
        std::string getStartTimeStr();

        HNI24HTime& getEndTime();
        uint getEndTimeSeconds();
        std::string getEndTimeStr();

        bool hasZone( std::string zoneID );
        void clearZones();
        void addZone( std::string name );

        std::set< std::string >& getZoneSetRef();
        void addZoneSet( std::set<std::string> &srcSet );

        std::string getZoneSetAsStr();
};

class HNISDay
{
    private:
        HNIS_DAY_INDX_T m_dayIndex;
  
        std::list< HNISPeriod > m_periodList;

    public:
        HNISDay();
       ~HNISDay();

        void setIndex( HNIS_DAY_INDX_T dayIndex );

        void clear();

        HNIS_RESULT_T addPeriod( HNISPeriod value );

        HNIS_RESULT_T addPeriodZoneOn( std::string zoneID, uint startSec, uint durationSec );
        
        std::string getDayName();

        void getPeriodList( std::vector< HNISPeriod > &periodList );

        void debugPrint();
};

class HNISchedule
{
    private:
        std::string m_timezone;
        uint32_t    m_schCRC32;

        HNISDay  m_dayArr[ HNIS_DINDX_NOTSET ];
 
        void calculateSMCRC32();
        
    public:
        HNISchedule();
       ~HNISchedule();
      
        void clear();

        void setTimezoneStr( std::string tzs );
        std::string getTimezoneStr();

        uint getSMCRC32();

        HNIS_RESULT_T addPeriodZoneOn( HNIS_DAY_INDX_T dayIndex, std::string zoneID, uint startSec, uint durationSec );
        
        HNIS_RESULT_T finalize();
        
        std::string getDayName( HNIS_DAY_INDX_T dayIndex );       
        void getPeriodList( HNIS_DAY_INDX_T dayIndex, std::vector< HNISPeriod > &periodList );
        
        //HNIS_RESULT_T getScheduleInfoJSON( std::ostream &ostr );

        //std::string getSwitchDaemonJSON();
        
        void debugPrint();
};

class HNIrrigationSchedule
{
    private:
        HNISchedule m_schedule;

        HNIrrigationPlacementSet *m_placements;
        HNIrrigationZoneSet      *m_zones;
        HNIrrigationModifierSet  *m_modifiers;

    public:
        HNIrrigationSchedule();
       ~HNIrrigationSchedule();

        void init( HNIrrigationPlacementSet *placements, HNIrrigationZoneSet *zones, HNIrrigationModifierSet *modifiers );

        std::string getTimezoneStr();

        uint getSMCRC32();
        std::string getSMCRC32Str();

        void clear();

        HNIS_RESULT_T buildSchedule();

        HNIS_RESULT_T getScheduleInfoJSON( std::ostream &ostr );

        std::string getSwitchDaemonJSON();
};

#endif // _HN_IRRIGATION_SCHEDULE_H_

