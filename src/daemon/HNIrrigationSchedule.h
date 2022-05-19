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
#include "HNIrrigationCriteria.h"
#include "HNIrrigationZone.h"

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

        uint m_rank;

        std::set< std::string > m_zoneSet;

    public:
        HNISPeriod();
       ~HNISPeriod();

        void setID( std::string id );
        void setType( HNIS_PERIOD_TYPE_T value );

        void setSlideLater( bool value );

        void setDayIndex( HNIS_DAY_INDX_T dindx );
        void setStartTime( HNI24HTime &time );
        void setStartTimeSeconds( uint seconds );

        void setEndTime( HNI24HTime &time );
        void setEndTimeSeconds( uint seconds );

        void setTimesFromStr( std::string startTime, std::string endTime ); 

        void setRank( uint value );

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
        
        uint getRank();

        void moveStartToSecond( uint seconds );
        void moveEndToSecond( uint seconds );

        static bool rankCompare( const HNISPeriod& first, const HNISPeriod& second );

};


// Enumerate the possible overlap cases
typedef enum OverlapScenarioEnumeration
{
    OVLP_TYPE_CRIT_NOTSET = 0xFF, // Init value since 0 is meaningful
    OVLP_TYPE_CRIT_BEFORE = 0xF,  // The criteria is completely before the period
    OVLP_TYPE_CRIT_AFTER  = 0x0,  // The criteria is completely after the period
    OVLP_TYPE_CRIT_FRONT  = 0xD,  // The criteria overlap the start of the period
    OVLP_TYPE_CRIT_AROUND = 0x5,  // The criteria encapsulates the period
    OVLP_TYPE_CRIT_WITHIN = 0xC,  // The criteria is encapsulated by the period
    OVLP_TYPE_CRIT_BACK   = 0x4   // The criteria overlaps the end of the period
}OVLP_TYPE_T;

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

        void applyZoneSet( std::string periodID, std::set< std::string > &zoneSet );
        std::string addAvailablePeriod( uint startSec, uint endSec, uint rank, std::set< std::string > &zoneSet );
        std::string insertBeforeAvailablePeriod( std::list< HNISPeriod >::iterator &it, uint startSec, uint endSec, uint rank, std::set< std::string > &zoneSet );
        std::string insertAfterAvailablePeriod( std::list< HNISPeriod >::iterator &it, uint startSec, uint endSec, uint rank, std::set< std::string > &zoneSet );

        OVLP_TYPE_T compareOverlap( uint cs, uint ce, HNISPeriod &period );

    public:
        HNISDay();
       ~HNISDay();

        void setIndex( HNIS_DAY_INDX_T dayIndex );

        void clear();

        void coalesce();

        HNIS_RESULT_T applyCriteria( HNIrrigationCriteria &criteria );

        HNIS_RESULT_T addPeriod( HNISPeriod value );

        //void addPeriodZoneOn( std::string periodID, std::string zoneID, uint durationSec );
        HNIS_RESULT_T addPeriodZoneOn( std::string zoneID, uint startSec, uint durationSec );
        
        std::string getDayName();

        void getPeriodList( std::vector< HNISPeriod > &periodList );

        HNIS_RESULT_T getAvailableSlotsForZone( std::string zoneID, std::vector< HNISPeriod > &slotList );

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

//      std::string m_timezone;
//      uint32_t    m_schCRC32;

//        HNISDay  m_dayArr[ HNIS_DINDX_NOTSET ];
        HNISchedule m_schedule;

        HNIrrigationCriteriaSet *m_criteria;
        HNIrrigationZoneSet     *m_zones;

//        void calculateSMCRC32();


    public:
        HNIrrigationSchedule();
       ~HNIrrigationSchedule();

        void init( HNIrrigationCriteriaSet *criteria, HNIrrigationZoneSet *zones );

        std::string getTimezoneStr();

        uint getSMCRC32();
        std::string getSMCRC32Str();

        void clear();

        //HNIS_RESULT_T initConfigSections( HNodeConfig &cfg );
        //HNIS_RESULT_T readConfigSections( HNodeConfig &cfg );
        //HNIS_RESULT_T updateConfigSections( HNodeConfig &cfg );

        HNIS_RESULT_T buildSchedule();
        //HNIS_RESULT_T buildTimeSource();
        //HNIS_RESULT_T buildSlotQueue();

        HNIS_RESULT_T getScheduleInfoJSON( std::ostream &ostr );

        std::string getSwitchDaemonJSON();
};

#endif // _HN_IRRIGATION_SCHEDULE_H_

