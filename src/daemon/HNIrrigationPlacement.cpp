#include <iostream>
#include <regex>

#include "HNIrrigationPlacement.h"

HNIrrigationPlacement::HNIrrigationPlacement()
{
    m_rank = 5;
    m_dayBits    = HNSC_DBITS_DAILY;
}

HNIrrigationPlacement::~HNIrrigationPlacement()
{

}

void 
HNIrrigationPlacement::setID( std::string id )
{
    m_id = id;
}

void 
HNIrrigationPlacement::setName( std::string value )
{
    m_name = value;
}

void 
HNIrrigationPlacement::setDesc( std::string value )
{
    m_desc = value;
}

HNIS_RESULT_T 
HNIrrigationPlacement::setTimesFromStr( std::string startTime, std::string endTime )
{
    m_startTime.parseTime( startTime );
 
    std::cout << "mst: " << m_startTime.getHMSStr() << std::endl;

    m_endTime.parseTime( endTime );
    
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationPlacement::setStartTime( std::string startTime )
{
    m_startTime.parseTime( startTime );
    
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationPlacement::setEndTime( std::string endTime )
{
    m_endTime.parseTime( endTime );
    
    return HNIS_RESULT_SUCCESS;
}

void 
HNIrrigationPlacement::setRank( uint value )
{
    m_rank = value;
}

void 
HNIrrigationPlacement::clearDayBits()
{
    m_dayBits = HNSC_DBITS_DAILY;
}

void 
HNIrrigationPlacement::setDayBits( uint value )
{
    m_dayBits = (HNSC_DBITS_T) value;
}

void 
HNIrrigationPlacement::addDayByName( std::string name )
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
HNIrrigationPlacement::hasZone( std::string zoneID )
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
HNIrrigationPlacement::clearZones()
{
    m_zoneSet.clear();
}

void 
HNIrrigationPlacement::addZone( std::string name )
{
    m_zoneSet.insert( name );
}
 
void 
HNIrrigationPlacement::addZoneSet( std::set<std::string> &srcSet )
{
    for( std::set<std::string>::iterator it = srcSet.begin(); it != srcSet.end(); it++ )
    {
        m_zoneSet.insert( *it );
    }
}
        
std::set< std::string >& 
HNIrrigationPlacement::getZoneSetRef()
{
    return m_zoneSet;
}

std::string 
HNIrrigationPlacement::getZoneSetAsStr()
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
HNIrrigationPlacement::getID()
{
    return m_id;
}

std::string 
HNIrrigationPlacement::getName()
{
    return m_name;
}

std::string 
HNIrrigationPlacement::getDesc()
{
    return m_desc;
}

HNI24HTime&
HNIrrigationPlacement::getStartTime()
{
    return m_startTime;
}

HNI24HTime&
HNIrrigationPlacement::getEndTime()
{
    return m_endTime;
}

uint 
HNIrrigationPlacement::getRank()
{
    return m_rank;
}

bool 
HNIrrigationPlacement::isForDay( HNIS_DAY_INDX_T dindx )
{
    uint dayMask = (1 << dindx);

    if( m_dayBits == HNSC_DBITS_DAILY )
        return true;

    if( m_dayBits & dayMask )
        return true;

    return false;
}

uint 
HNIrrigationPlacement::getDayBits()
{
    return m_dayBits;
}

HNIS_RESULT_T 
HNIrrigationPlacement::validateSettings()
{
    // Add validation checking here
    return HNIS_RESULT_SUCCESS;
}

HNIrrigationPlacementSet::HNIrrigationPlacementSet()
{

}

HNIrrigationPlacementSet::~HNIrrigationPlacementSet()
{

}

void
HNIrrigationPlacementSet::clear()
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    m_placementsMap.clear();
}

bool 
HNIrrigationPlacementSet::hasID( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    std::map< std::string, HNIrrigationPlacement >::iterator it = m_placementsMap.find( id );

    if( it == m_placementsMap.end() )
        return false;

    return true;
}

HNIrrigationPlacement*
HNIrrigationPlacementSet::internalUpdatePlacement( std::string id )
{
    std::map< std::string, HNIrrigationPlacement >::iterator it = m_placementsMap.find( id );

    if( it == m_placementsMap.end() )
    {
        HNIrrigationPlacement nSpec;
        nSpec.setID( id );
        m_placementsMap.insert( std::pair< std::string, HNIrrigationPlacement >( id, nSpec ) );\
        return &( m_placementsMap[ id ] );
    }

    return &(it->second);
}

HNIrrigationPlacement*
HNIrrigationPlacementSet::updatePlacement( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    return internalUpdatePlacement( id );
}

void 
HNIrrigationPlacementSet::deletePlacement( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Find the referenced zone
    std::map< std::string, HNIrrigationPlacement >::iterator it = m_placementsMap.find( id );

    // If already no existant than nothing to do.
    if( it == m_placementsMap.end() )
        return;

    // Get rid of the zone record
    m_placementsMap.erase( it );
}

void 
HNIrrigationPlacementSet::getPlacementsList( std::vector< HNIrrigationPlacement > &placementsList )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    for( std::map< std::string, HNIrrigationPlacement >::iterator it = m_placementsMap.begin(); it != m_placementsMap.end(); it++ )
    {
        placementsList.push_back( it->second );
    }
}

HNIS_RESULT_T 
HNIrrigationPlacementSet::getPlacement( std::string id, HNIrrigationPlacement &event )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    std::map< std::string, HNIrrigationPlacement >::iterator it = m_placementsMap.find( id );

    if( it == m_placementsMap.end() )
        return HNIS_RESULT_FAILURE;

    event = it->second;
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationPlacementSet::initPlacementsListSection( HNodeConfig &cfg )
{
    HNCSection *secPtr;

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    cfg.updateSection( "irrPlacementsInfo", &secPtr );
    
    HNCObjList *listPtr;
    secPtr->updateList( "placementsList", &listPtr );

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationPlacementSet::readPlacementsListSection( HNodeConfig &cfg )
{
    HNCSection  *secPtr;

    std::cout << "rc1" << std::endl;

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Aquire a pointer to the "device" section
    cfg.updateSection( "irrPlacementsInfo", &secPtr );

    // Get a list pointer
    HNCObjList *listPtr;
    secPtr->updateList( "placementsList", &listPtr );

    std::cout << "rc2: " << listPtr->size() << std::endl;

    for( uint indx = 0; indx < listPtr->size(); indx++ )
    {
        std::string placementID;
        std::string rstStr;
        HNCObj *objPtr;

        if( listPtr->getObjPtr( indx, &objPtr ) != HNC_RESULT_SUCCESS )
            continue;

        // Get the zoneID first, if missing skip the record
        if( objPtr->getValueByName( "placementid", placementID ) != HNC_RESULT_SUCCESS )
        {
            continue;
        }

        // Get the internal reference to the zone.
        HNIrrigationPlacement *placementPtr = internalUpdatePlacement( placementID );

        if( objPtr->getValueByName( "name", rstStr ) == HNC_RESULT_SUCCESS )
        {
            placementPtr->setName( rstStr );
        }

        if( objPtr->getValueByName( "description", rstStr ) == HNC_RESULT_SUCCESS )
        {
            placementPtr->setDesc( rstStr );
        }

        if( objPtr->getValueByName( "startTime", rstStr ) == HNC_RESULT_SUCCESS )
        {
            placementPtr->setStartTime( rstStr );
        }

        if( objPtr->getValueByName( "endTime", rstStr ) == HNC_RESULT_SUCCESS )
        {
            placementPtr->setEndTime( rstStr );
        }

        if( objPtr->getValueByName( "rank", rstStr ) == HNC_RESULT_SUCCESS )
        {
            uint offset = strtol( rstStr.c_str(), NULL, 0 );
            placementPtr->setRank( offset );
        }

        if( objPtr->getValueByName( "dayBits", rstStr ) == HNC_RESULT_SUCCESS )
        {
            placementPtr->clearDayBits();
            uint dayBits = strtol( rstStr.c_str(), NULL, 0 );
            placementPtr->setDayBits( dayBits );
        }

        if( objPtr->getValueByName( "zoneList", rstStr ) == HNC_RESULT_SUCCESS )
        {
            const std::regex ws_re("\\s+"); // whitespace

            placementPtr->clearZones();

            std::cout << "Config Read ZoneList: '" << rstStr << "'" << std::endl;

            // Ignore the empty string.
            if( rstStr.empty() == true )
                continue;

            // Walk the zoneList string
            std::sregex_token_iterator it( rstStr.begin(), rstStr.end(), ws_re, -1 );
            const std::sregex_token_iterator end;
            while( it != end )
            {
                std::cout << "Config Read Add Zone: '" << *it << "'" << std::endl;

                // Add a new switch action to the queue.
                std::string zoneName = *it;
                if( zoneName.empty() == false )    
                    placementPtr->addZone( zoneName );
                it++;
            }
        }
    }
          
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationPlacementSet::updatePlacementsListSection( HNodeConfig &cfg )
{
    char tmpStr[256];
    HNCSection *secPtr;
    HNCObjList *listPtr;

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    cfg.updateSection( "irrPlacementsInfo", &secPtr );

    secPtr->updateList( "placementsList", &listPtr );

    for( std::map< std::string, HNIrrigationPlacement >::iterator it = m_placementsMap.begin(); it != m_placementsMap.end(); it++ )
    { 
        HNCObj *objPtr;

        // Aquire a new list entry
        listPtr->appendObj( &objPtr );

        // Fill the entry with the static event info
        objPtr->updateValue( "placementid", it->second.getID() );

        objPtr->updateValue( "name", it->second.getName() );
        objPtr->updateValue( "description", it->second.getDesc() );
        objPtr->updateValue( "startTime", it->second.getStartTime().getHMSStr() );
        objPtr->updateValue( "endTime", it->second.getEndTime().getHMSStr() );

        sprintf( tmpStr, "%d", it->second.getRank() );
        objPtr->updateValue( "rank", tmpStr );

        sprintf( tmpStr, "%d", it->second.getDayBits() );
        objPtr->updateValue( "dayBits", tmpStr );

        objPtr->updateValue( "zoneList", it->second.getZoneSetAsStr() );
    }

    return HNIS_RESULT_SUCCESS;
}

