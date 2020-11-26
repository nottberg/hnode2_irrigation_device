#include <iostream>

#include "HNIrrigationCriteria.h"

HNIrrigationCriteria::HNIrrigationCriteria()
{
    m_rank = 5;
    m_dayBits    = HNSC_DBITS_DAILY;
}

HNIrrigationCriteria::~HNIrrigationCriteria()
{

}

void 
HNIrrigationCriteria::setID( std::string id )
{
    m_id = id;
}

void 
HNIrrigationCriteria::setName( std::string value )
{
    m_name = value;
}

void 
HNIrrigationCriteria::setDesc( std::string value )
{
    m_desc = value;
}

HNIS_RESULT_T 
HNIrrigationCriteria::setTimesFromStr( std::string startTime, std::string endTime )
{
    m_startTime.parseTime( startTime );
 
    std::cout << "mst: " << m_startTime.getHMSStr() << std::endl;

    m_endTime.parseTime( endTime );
}

HNIS_RESULT_T 
HNIrrigationCriteria::setStartTime( std::string startTime )
{
    m_startTime.parseTime( startTime );
}

HNIS_RESULT_T 
HNIrrigationCriteria::setEndTime( std::string endTime )
{
    m_endTime.parseTime( endTime );
}

void 
HNIrrigationCriteria::setRank( uint value )
{
    m_rank = value;
}

void 
HNIrrigationCriteria::clearDayBits()
{
    m_dayBits = HNSC_DBITS_DAILY;
}

void 
HNIrrigationCriteria::setDayBits( uint value )
{
    m_dayBits = (HNSC_DBITS_T) value;
}

void 
HNIrrigationCriteria::addDayByName( std::string name )
{
    std::cout << "addDayByName - name: " << name << std::endl; 

    // Looking for match
    if( "Sunday" == name )
        m_dayBits = (HNSC_DBITS_T)( m_dayBits | HNSC_DBITS_SUNDAY );
    else if( "Monday" == name )
        m_dayBits = (HNSC_DBITS_T)( m_dayBits | HNSC_DBITS_MONDAY );
    else if( "Tuesday" == name )
        m_dayBits = (HNSC_DBITS_T)( m_dayBits | HNSC_DBITS_TUESDAY );
    else if( "Wednesday" == name )
        m_dayBits = (HNSC_DBITS_T)( m_dayBits | HNSC_DBITS_WEDNESDAY );
    else if( "Thursday" == name )
        m_dayBits = (HNSC_DBITS_T)( m_dayBits | HNSC_DBITS_THURSDAY );
    else if( "Friday" == name )
        m_dayBits = (HNSC_DBITS_T)( m_dayBits | HNSC_DBITS_FRIDAY );
    else if( "Saturday" == name )
        m_dayBits = (HNSC_DBITS_T)( m_dayBits | HNSC_DBITS_SATURDAY );

    std::cout << "addDayByName - bits: " << m_dayBits << std::endl; 
}

bool 
HNIrrigationCriteria::hasZone( std::string zoneID )
{
    // No zones is interpreted as all zones.
    if( m_zoneSet.size() == 0 )
        return true;

    // Check against the specified set.
    if( m_zoneSet.find( zoneID ) != m_zoneSet.end() )
        return true;

    // No found
    return false;
}

void 
HNIrrigationCriteria::clearZones()
{
    m_zoneSet.clear();
}

void 
HNIrrigationCriteria::addZone( std::string name )
{
    m_zoneSet.insert( name );
}
 
void 
HNIrrigationCriteria::addZoneSet( std::set<std::string> &srcSet )
{
    for( std::set<std::string>::iterator it = srcSet.begin(); it != srcSet.end(); it++ )
    {
        m_zoneSet.insert( *it );
    }
}
        
std::set< std::string >& 
HNIrrigationCriteria::getZoneSetRef()
{
    return m_zoneSet;
}

std::string 
HNIrrigationCriteria::getZoneSetAsStr()
{
    std::string rspStr;

    bool first = true;
    for( std::set<std::string>::iterator it = m_zoneSet.begin(); it != m_zoneSet.end(); it++ )
    {
        if( first == false )
            rspStr += " ";
        rspStr += *it;
        first = false;
    }

    return rspStr;
}

std::string 
HNIrrigationCriteria::getID()
{
    return m_id;
}

std::string 
HNIrrigationCriteria::getName()
{
    return m_name;
}

std::string 
HNIrrigationCriteria::getDesc()
{
    return m_desc;
}

HNI24HTime&
HNIrrigationCriteria::getStartTime()
{
    return m_startTime;
}

HNI24HTime&
HNIrrigationCriteria::getEndTime()
{
    return m_endTime;
}

uint 
HNIrrigationCriteria::getRank()
{
    return m_rank;
}

bool 
HNIrrigationCriteria::isForDay( HNIS_DAY_INDX_T dindx )
{
    uint dayMask = (1 << dindx);

    if( m_dayBits == HNSC_DBITS_DAILY )
        return true;

    if( m_dayBits & dayMask )
        return true;

    return false;
}

uint 
HNIrrigationCriteria::getDayBits()
{
    return m_dayBits;
}

HNIS_RESULT_T 
HNIrrigationCriteria::validateSettings()
{
    // Add validation checking here
    return HNIS_RESULT_SUCCESS;
}

HNIrrigationCriteriaSet::HNIrrigationCriteriaSet()
{

}

HNIrrigationCriteriaSet::~HNIrrigationCriteriaSet()
{

}


