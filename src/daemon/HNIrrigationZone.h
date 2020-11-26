#ifndef __HN_IRRIGATION_ZONE_H__
#define __HN_IRRIGATION_ZONE_H__

#include <stdint.h>

#include <string>
#include <map>
#include <set>

class HNIrrigationZone
{
    private:
        std::string m_zoneID;
        std::string m_zoneName;
        std::string m_zoneDesc;

        uint m_weeklySec;
        uint m_maxCycleSec;
        uint m_minCycleSec;

        //std::string m_swidList;
        std::set< std::string > m_swidSet;

    public:
        HNIrrigationZone();
       ~HNIrrigationZone();

        void setID( std::string id );
        void setName( std::string name );
        void setDesc( std::string desc );
       
        void setWeeklySeconds( uint value );
        void setMinimumCycleTimeSeconds( uint value );
        void setMaximumCycleTimeSeconds( uint value );

        void clearSWIDSet();
        void addSWID( std::string swid );
        void addSWIDSet( std::set< std::string > &swidSet );

        HNIS_RESULT_T validateSettings();

        std::string getID();
        std::string getName();
        std::string getDesc();

        std::set< std::string >& getSWIDSetRef();
        std::string getSWIDListStr();

        uint getWeeklySeconds();
        uint getMinimumCycleTimeSeconds();
        uint getMaximumCycleTimeSeconds();

};

class HNIrrigationZoneSet
{
    private:
        std::map< std::string, HNIrrigationZone > m_zoneMap;

    public:
        HNIrrigationZoneSet();
       ~HNIrrigationZoneSet();

};

#endif // __HN_IRRIGATION_ZONE_H__
