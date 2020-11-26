#include <iostream>
#include <sstream>

#include "HNIrrigationTypes.h"
#include "HNIrrigationZone.h"

HNIrrigationZone::HNIrrigationZone()
{
    m_weeklySec   = (((5 * 60) * 2) * 7);
    m_minCycleSec = (2 * 60);
    m_maxCycleSec = (2 * 10);
}

HNIrrigationZone::~HNIrrigationZone()
{

}

void 
HNIrrigationZone::setID( std::string id )
{
    m_zoneID = id;
}

void 
HNIrrigationZone::setName( std::string name )
{
    m_zoneName = name;
}

void 
HNIrrigationZone::setDesc( std::string desc )
{
    m_zoneDesc = desc;
}
     
void
HNIrrigationZone::setWeeklySeconds( uint value )
{
    m_weeklySec = value;
}

void
HNIrrigationZone::setMinimumCycleTimeSeconds( uint value )
{
    m_minCycleSec = value;
}
  
void
HNIrrigationZone::setMaximumCycleTimeSeconds( uint value )
{
    m_maxCycleSec = value;
}

void 
HNIrrigationZone::clearSWIDSet()
{
    m_swidSet.clear();
}

void 
HNIrrigationZone::addSWID( std::string swid )
{
    m_swidSet.insert( swid );
}

void 
HNIrrigationZone::addSWIDSet( std::set< std::string > &swidSet )
{
    std::cout << "addSWIDSet - input len: " << swidSet.size() << std::endl;

    for( std::set< std::string >::iterator it = swidSet.begin(); it != swidSet.end(); it++ )
    {
        std::cout << "addSWIDSet - value: " << *it << std::endl;
        m_swidSet.insert( *it );
    }
}

HNIS_RESULT_T 
HNIrrigationZone::validateSettings()
{
    // Add validation checking here
    return HNIS_RESULT_SUCCESS;
}

std::string 
HNIrrigationZone::getID()
{
    return m_zoneID;
}

std::string 
HNIrrigationZone::getName()
{
    return m_zoneName;
}

std::string 
HNIrrigationZone::getDesc()
{
    return m_zoneDesc;
}

std::set< std::string >& 
HNIrrigationZone::getSWIDSetRef()
{
    return m_swidSet;
}

std::string 
HNIrrigationZone::getSWIDListStr()
{
    std::ostringstream swidStr;

    bool first = true;
    for( std::set< std::string >::iterator it = m_swidSet.begin(); it != m_swidSet.end(); it++ )
    {
        if( first == false )
            swidStr << " ";

        swidStr << *it;

        first = false;
    }
    
    return swidStr.str();
}

uint 
HNIrrigationZone::getWeeklySeconds()
{
    return m_weeklySec; 
}

uint 
HNIrrigationZone::getMinimumCycleTimeSeconds()
{
    return m_minCycleSec; 
}

uint 
HNIrrigationZone::getMaximumCycleTimeSeconds()
{
    return m_maxCycleSec; 
}

HNIrrigationZoneSet::HNIrrigationZoneSet()
{

}

HNIrrigationZoneSet::~HNIrrigationZoneSet()
{

}

