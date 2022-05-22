#ifndef __HN_IRRIGATION_PLACEMENT_H__
#define __HN_IRRIGATION_PLACEMENT_H__

#include <stdint.h>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <mutex>

#include <hnode2/HNodeConfig.h>

#include "HNIrrigationTypes.h"

typedef enum HNIrrigationPlacementDayBitsEnum
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

class HNIrrigationPlacement
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
        HNIrrigationPlacement();
       ~HNIrrigationPlacement();

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
        void addZoneSet( std::set<std::string> &srcSet );

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

class HNIrrigationPlacementSet
{
    private:
        // Protect access to the data members
        std::mutex m_accessMutex;

        std::map< std::string, HNIrrigationPlacement > m_placementsMap;

        HNIrrigationPlacement* internalUpdatePlacement( std::string id );

    public:
        HNIrrigationPlacementSet();
       ~HNIrrigationPlacementSet();

        bool hasID( std::string id );

        HNIrrigationPlacement* updatePlacement( std::string id );
        void deletePlacement( std::string placementID );
        void getPlacementsList( std::vector< HNIrrigationPlacement > &placementsList );
        HNIS_RESULT_T getPlacement( std::string placementID, HNIrrigationPlacement &event );

        void clear();

        HNIS_RESULT_T initPlacementsListSection( HNodeConfig &cfg );
        HNIS_RESULT_T readPlacementsListSection( HNodeConfig &cfg );
        HNIS_RESULT_T updatePlacementsListSection( HNodeConfig &cfg );
};

#endif // __HN_IRRIGATION_PLACEMENT_H__
