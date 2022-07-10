#ifndef __HN_IRRIGATION_ZONE_H__
#define __HN_IRRIGATION_ZONE_H__

#include <stdint.h>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <mutex>

#include <hnode2/HNodeConfig.h>

#include "HNIrrigationTypes.h"

typedef enum HNIrrigationZoneStatusFlags
{
    HNIZ_STATUS_NOTSET    = 0x0,
    HNIZ_STATUS_ACTIVE    = 0x1,
    HNIZ_STATUS_INHIBITED = 0x2
}HNIZ_STATUS_T;

class HNIrrigationZone
{
    private:
        std::string m_zoneID;
        std::string m_zoneName;
        std::string m_zoneDesc;

        uint m_weeklySec;
        uint m_maxCycleSec;
        uint m_minCycleSec;

        std::set< std::string > m_swidSet;

        uint32_t    m_statusFlags;
        std::string m_inhibitBy;

    public:
        HNIrrigationZone();
       ~HNIrrigationZone();

        void setID( std::string id );
        void setName( std::string name );
        void setDesc( std::string desc );
       
        void setWeeklySeconds( uint value );
        void setMinimumCycleTimeSeconds( uint value );
        void setMaximumCycleTimeSeconds( uint value );

        bool hasSwitch( std::string swid );
        void clearSWIDSet();
        void addSWID( std::string swid );
        void addSWIDSet( std::set< std::string > &swidSet );

        HNIS_RESULT_T validateSettings();

        void setActive();
        void clearActive();

        void setInhibited( std::string inhibitID );
        void clearInhibited();

        std::string getID();
        std::string getName();
        std::string getDesc();

        std::set< std::string >& getSWIDSetRef();
        std::string getSWIDListStr();

        uint getWeeklySeconds();
        uint getMinimumCycleTimeSeconds();
        uint getMaximumCycleTimeSeconds();

        bool isActive();
        bool isInhibited();

        std::string getInhibitedByID();
};

class HNIrrigationZoneSet
{
    private:
        // Protect access to the data members
        std::mutex m_accessMutex;

        std::map< std::string, HNIrrigationZone > m_zoneMap;

        HNIrrigationZone* internalUpdateZone( std::string id );

    public:
        HNIrrigationZoneSet();
       ~HNIrrigationZoneSet();

        void clear();

        HNIS_RESULT_T initZoneListSection( HNodeConfig &cfg );
        HNIS_RESULT_T readZoneListSection( HNodeConfig &cfg );
        HNIS_RESULT_T updateZoneListSection( HNodeConfig &cfg );

        bool hasID( std::string id );
        HNIrrigationZone* updateZone( std::string id );
        void deleteZone( std::string id );
        void getZoneList( std::vector< HNIrrigationZone > &zoneList );
        HNIS_RESULT_T getZone( std::string id, HNIrrigationZone &event );
        HNIS_RESULT_T getZoneName( std::string id, std::string &name );

        void setActive( std::string zoneid );
        void clearActive( std::string zoneid );

        void clearAllActive();
        void setActiveFromSWID( std::string swid );
        void clearActiveFromSWID( std::string swid );

        void setInhibited( std::string zoneid, std::string inhibitid );
        void clearInhibited( std::string zoneid );

        void getActiveZones( std::vector< HNIrrigationZone > &zoneList );
        void getInhibitedZones( std::vector< HNIrrigationZone > &zoneList );
        
        uint getMaxCycleTimeForZoneSet( std::set< std::string > &zoneIDSet );
};

#endif // __HN_IRRIGATION_ZONE_H__
