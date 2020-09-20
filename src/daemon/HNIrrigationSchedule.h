#ifndef _HN_IRRIGATION_SCHEDULE_H_
#define _HN_IRRIGATION_SCHEDULE_H_

#include <stdint.h>
#include <unistd.h>

#include <string>
#include <list>
#include <vector>
#include <set>

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
    HNIS_DINDX_NOTSET    = 7
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

typedef enum HNScheduleCriteriaDayBitsEnum
{
    HNSC_DBITS_DAILY     = 0x00,
    HNSC_DBITS_SUNDAY    = 0x01,
    HNSC_DBITS_MONDAY    = 0x02,
    HNSC_DBITS_TUESDAY   = 0x04,
    HNSC_DBITS_WEDNESDAY = 0x08,
    HNSC_DBITS_THURSDAY  = 0x10,
    HNSC_DBITS_FRIDAY    = 0x20,
    HNSC_DBITS_SATURDAY  = 0x40
}HNSC_DBITS_T;

class HNScheduleCriteria
{
    private:
        std::string  m_id;
        std::string  m_name;
        std::string  m_desc;

        HNI24HTime m_startTime;
        HNI24HTime m_endTime;

        uint m_rank;

        HNSC_DBITS_T m_dayBits;

        std::set< std::string > m_zoneSet;

    public:
        HNScheduleCriteria();
       ~HNScheduleCriteria();

        void setID( std::string id );
        void setName( std::string value );
        void setDesc( std::string value );

        HNIS_RESULT_T setTimesFromStr( std::string startTime, std::string endTime ); 
        HNIS_RESULT_T setStartTime( std::string startTime );
        HNIS_RESULT_T setEndTime( std::string endTime );

        void setRank( uint value );

        void clearDayBits();
        void setDayBits( uint value );
        void addDayByName( std::string name );

        bool hasZone( std::string zoneID );
        void clearZones();
        void addZone( std::string name );
         
        std::set< std::string >& getZoneSetRef();

        std::string getZoneSetAsStr();

        std::string getID();
        std::string getName();
        std::string getDesc();

        HNI24HTime &getStartTime();
        HNI24HTime &getEndTime();

        uint getRank();

        bool isForDay( HNIS_DAY_INDX_T dindx );
        uint getDayBits();

        HNIS_RESULT_T validateSettings();
};


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

class HNIrrigationZone
{
    private:
        std::string m_zoneID;
        std::string m_zoneName;
        std::string m_zoneDesc;

        uint m_weeklySec;
        uint m_maxCycleSec;
        uint m_minCycleSec;

        std::string m_swidList;

    public:
        HNIrrigationZone();
       ~HNIrrigationZone();

        void setID( std::string id );
        void setName( std::string name );
        void setDesc( std::string desc );
       
        void setWeeklySeconds( uint value );
        void setMinimumCycleTimeSeconds( uint value );
        void setMaximumCycleTimeSeconds( uint value );

        void setSWIDList( std::string swidList );

        HNIS_RESULT_T validateSettings();

        std::string getID();
        std::string getName();
        std::string getDesc();

        std::string getSWIDListStr();

        uint getWeeklySeconds();
        uint getMinimumCycleTimeSeconds();
        uint getMaximumCycleTimeSeconds();

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

        HNIS_RESULT_T applyCriteria( HNScheduleCriteria &criteria );

        HNIS_RESULT_T addPeriod( HNISPeriod value );

        void addPeriodZoneOn( std::string periodID, std::string zoneID, uint durationSec );

        std::string getDayName();

        void getPeriodList( std::vector< HNISPeriod > &periodList );

        HNIS_RESULT_T getAvailableSlotsForZone( std::string zoneID, std::vector< HNISPeriod > &slotList );

        void debugPrint();
};

class HNIrrigationSchedule
{
    private:

        std::string m_timezone;
        uint32_t    m_schCRC32;

        HNISDay  m_dayArr[ HNIS_DINDX_NOTSET ];

        std::map< std::string, HNScheduleCriteria >   m_criteriaMap;
        std::map< std::string, HNIrrigationZone >  m_zoneMap;

        HNIS_RESULT_T initZoneListSection( HNodeConfig &cfg );
        HNIS_RESULT_T readZoneListSection( HNodeConfig &cfg );
        HNIS_RESULT_T updateZoneListSection( HNodeConfig &cfg );

        HNIS_RESULT_T initCriteriaListSection( HNodeConfig &cfg );
        HNIS_RESULT_T readCriteriaListSection( HNodeConfig &cfg );
        HNIS_RESULT_T updateCriteriaListSection( HNodeConfig &cfg );

        void calculateSMCRC32();

    public:
        HNIrrigationSchedule();
       ~HNIrrigationSchedule();

        std::string getTimezoneStr();

        uint getSMCRC32();
        std::string getSMCRC32Str();

        void clear();

        bool hasCriteria( std::string eventID );
        HNScheduleCriteria *updateCriteria( std::string id );
        void deleteCriteria( std::string eventID );
        void getCriteriaList( std::vector< HNScheduleCriteria > &criteriaList );
        HNIS_RESULT_T getCriteria( std::string eventID, HNScheduleCriteria &event );

        bool hasZone( std::string zoneID );
        HNIrrigationZone *updateZone( std::string zoneID );
        void deleteZone( std::string zoneID );
        void getZoneList( std::vector< HNIrrigationZone > &zoneList );
        HNIS_RESULT_T getZone( std::string zoneID, HNIrrigationZone &zone );
        HNIS_RESULT_T getZoneName( std::string zoneID, std::string &name );

        HNIS_RESULT_T initConfigSections( HNodeConfig &cfg );
        HNIS_RESULT_T readConfigSections( HNodeConfig &cfg );
        HNIS_RESULT_T updateConfigSections( HNodeConfig &cfg );

        HNIS_RESULT_T buildSchedule();

        HNIS_RESULT_T getScheduleInfoJSON( std::ostream &ostr );

        std::string getSwitchDaemonJSON();
};

#endif // _HN_IRRIGATION_SCHEDULE_H_

