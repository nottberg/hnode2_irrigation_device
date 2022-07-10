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

// Track some zone statitics
// that result from building a schedule
class HNISZoneAppliedModifier
{
    public:
       HNISZoneAppliedModifier();
       HNISZoneAppliedModifier( std::string modID, std::string appliedValue, int deltaSec );
      ~HNISZoneAppliedModifier();

       void setID( std::string modID );
       void setAppliedValue( std::string appliedValue );
       void setDeltaSeconds( int deltaSec );

       std::string getID();
       std::string getAppliedValue();
       int getDeltaSeconds();

    private:
       std::string m_id;
       std::string m_appliedValue;
       int         m_deltaSec;
};

class HNISZoneStats
{
    public:
        HNISZoneStats();
        HNISZoneStats( std::string zoneID, uint baseSeconds );
       ~HNISZoneStats();

        void setZoneID( std::string zoneID );
        void setBaseSeconds( uint value );
        void setTotalSeconds( uint value );
        void setSecondsPerDay( HNIS_DAY_INDX_T dayIndx, uint value );
        void addToSecondsPerDay( HNIS_DAY_INDX_T dayIndx, uint value );
        void addAppliedModifier( std::string modID, std::string appliedValue, double deltaSeconds );

        std::string getZoneID();
        uint getBaseSeconds();
        uint getTotalSeconds();
        double getAverageSecondsPerDay();
        uint getSecondsForDay( HNIS_DAY_INDX_T dayIndx );

        void getAppliedModifiersList( std::vector< HNISZoneAppliedModifier > &amList );

        void finalize();

    private:
        std::string m_zoneID;

        uint m_baseSeconds;
        uint m_totalSeconds;

        double m_avgSecondsPerDay;

        std::vector< HNISZoneAppliedModifier > m_appliedModList;

        uint m_secPerDay[ HNIS_DINDX_NOTSET ];
};

class HNISPeriod
{
    private:
        std::string m_id;

        HNIS_DAY_INDX_T m_dayIndx;

        HNI24HTime m_startTime;
        HNI24HTime m_endTime;

        std::set< std::string > m_zoneSet;

    public:
        HNISPeriod();
       ~HNISPeriod();

        void setID( std::string id );

        void setDayIndex( HNIS_DAY_INDX_T dindx );
        void setStartTime( HNI24HTime &time );
        void setStartTimeSeconds( uint seconds );

        void setEndTime( HNI24HTime &time );
        void setEndTimeSeconds( uint seconds );

        void setTimesFromStr( std::string startTime, std::string endTime ); 

        std::string getID();

        HNIS_DAY_INDX_T getDayIndex();

        HNI24HTime& getStartTime();
        uint getStartTimeSeconds();
        std::string getStartTimeStr();

        HNI24HTime& getEndTime();
        uint getEndTimeSeconds();
        std::string getEndTimeStr();

        bool containsTimeSeconds( uint seconds );
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

        bool proceedingZonePeriod( std::string zoneID, uint startSec );
        
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
 
        std::vector< HNISZoneStats > m_zoneStats;

        void calculateSMCRC32();
        
    public:
        HNISchedule();
       ~HNISchedule();
      
        void clear();

        void setTimezoneStr( std::string tzs );
        std::string getTimezoneStr();

        uint getSMCRC32();

        HNIS_RESULT_T addPeriodZoneOn( HNIS_DAY_INDX_T dayIndex, std::string zoneID, uint startSec, uint durationSec );

        bool proceedingZonePeriod( HNIS_DAY_INDX_T dayIndex, std::string zoneID, uint startSec );
        
        HNIS_RESULT_T finalize();
       
        std::string getDayName( HNIS_DAY_INDX_T dayIndex );       
        void getPeriodList( HNIS_DAY_INDX_T dayIndex, std::vector< HNISPeriod > &periodList );
                
        HNISZoneStats *createZoneStatsTracker( std::string zoneID, uint baseSeconds );
        void addToZoneStatDailyTime( std::string zoneID, HNIS_DAY_INDX_T dayIndex, uint seconds );
        void getZoneStatList( std::vector< HNISZoneStats > &zsList );

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

        HNIS_RESULT_T buildSchedule( bool schedulerEnabled );

        HNIS_RESULT_T getScheduleInfoJSON( std::ostream &ostr );

        std::string getSwitchDaemonJSON();
};

#endif // _HN_IRRIGATION_SCHEDULE_H_

