#include <iostream>
#include <regex>

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

void
HNIrrigationCriteriaSet::clear()
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    m_criteriaMap.clear();
}

bool 
HNIrrigationCriteriaSet::hasID( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    std::map< std::string, HNIrrigationCriteria >::iterator it = m_criteriaMap.find( id );

    if( it == m_criteriaMap.end() )
        return false;

    return true;
}

HNIrrigationCriteria*
HNIrrigationCriteriaSet::internalUpdateCriteria( std::string id )
{
    std::map< std::string, HNIrrigationCriteria >::iterator it = m_criteriaMap.find( id );

    if( it == m_criteriaMap.end() )
    {
        HNIrrigationCriteria nSpec;
        nSpec.setID( id );
        m_criteriaMap.insert( std::pair< std::string, HNIrrigationCriteria >( id, nSpec ) );\
        return &( m_criteriaMap[ id ] );
    }

    return &(it->second);
}

HNIrrigationCriteria*
HNIrrigationCriteriaSet::updateCriteria( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    return internalUpdateCriteria( id );
}

void 
HNIrrigationCriteriaSet::deleteCriteria( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Find the referenced zone
    std::map< std::string, HNIrrigationCriteria >::iterator it = m_criteriaMap.find( id );

    // If already no existant than nothing to do.
    if( it == m_criteriaMap.end() )
        return;

    // Get rid of the zone record
    m_criteriaMap.erase( it );
}

void 
HNIrrigationCriteriaSet::getCriteriaList( std::vector< HNIrrigationCriteria > &criteriaList )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    for( std::map< std::string, HNIrrigationCriteria >::iterator it = m_criteriaMap.begin(); it != m_criteriaMap.end(); it++ )
    {
        criteriaList.push_back( it->second );
    }
}

HNIS_RESULT_T 
HNIrrigationCriteriaSet::getCriteria( std::string id, HNIrrigationCriteria &event )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    std::map< std::string, HNIrrigationCriteria >::iterator it = m_criteriaMap.find( id );

    if( it == m_criteriaMap.end() )
        return HNIS_RESULT_FAILURE;

    event = it->second;
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationCriteriaSet::initCriteriaListSection( HNodeConfig &cfg )
{
    HNCSection *secPtr;

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    cfg.updateSection( "irrCriteriaInfo", &secPtr );
    
    HNCObjList *listPtr;
    secPtr->updateList( "criteriaList", &listPtr );

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationCriteriaSet::readCriteriaListSection( HNodeConfig &cfg )
{
    HNCSection  *secPtr;

    std::cout << "rc1" << std::endl;

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Aquire a pointer to the "device" section
    cfg.updateSection( "irrCriteriaInfo", &secPtr );

    // Get a list pointer
    HNCObjList *listPtr;
    secPtr->updateList( "criteriaList", &listPtr );

    std::cout << "rc2: " << listPtr->size() << std::endl;

    for( uint indx = 0; indx < listPtr->size(); indx++ )
    {
        std::string criteriaID;
        std::string rstStr;
        HNCObj *objPtr;

        if( listPtr->getObjPtr( indx, &objPtr ) != HNC_RESULT_SUCCESS )
            continue;

        // Get the zoneID first, if missing skip the record
        if( objPtr->getValueByName( "criteriaid", criteriaID ) != HNC_RESULT_SUCCESS )
        {
            continue;
        }

        // Get the internal reference to the zone.
        HNIrrigationCriteria *criteriaPtr = internalUpdateCriteria( criteriaID );

        if( objPtr->getValueByName( "name", rstStr ) == HNC_RESULT_SUCCESS )
        {
            criteriaPtr->setName( rstStr );
        }

        if( objPtr->getValueByName( "description", rstStr ) == HNC_RESULT_SUCCESS )
        {
            criteriaPtr->setDesc( rstStr );
        }

        if( objPtr->getValueByName( "startTime", rstStr ) == HNC_RESULT_SUCCESS )
        {
            criteriaPtr->setStartTime( rstStr );
        }

        if( objPtr->getValueByName( "endTime", rstStr ) == HNC_RESULT_SUCCESS )
        {
            criteriaPtr->setEndTime( rstStr );
        }

        if( objPtr->getValueByName( "rank", rstStr ) == HNC_RESULT_SUCCESS )
        {
            uint offset = strtol( rstStr.c_str(), NULL, 0 );
            criteriaPtr->setRank( offset );
        }

        if( objPtr->getValueByName( "dayBits", rstStr ) == HNC_RESULT_SUCCESS )
        {
            criteriaPtr->clearDayBits();
            uint dayBits = strtol( rstStr.c_str(), NULL, 0 );
            criteriaPtr->setDayBits( dayBits );
        }

        if( objPtr->getValueByName( "zoneList", rstStr ) == HNC_RESULT_SUCCESS )
        {
            const std::regex ws_re("\\s+"); // whitespace

            criteriaPtr->clearZones();

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
                    criteriaPtr->addZone( zoneName );
                it++;
            }
        }
    }
          
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationCriteriaSet::updateCriteriaListSection( HNodeConfig &cfg )
{
    char tmpStr[256];
    HNCSection *secPtr;
    HNCObjList *listPtr;

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    cfg.updateSection( "irrCriteriaInfo", &secPtr );

    secPtr->updateList( "criteriaList", &listPtr );

    for( std::map< std::string, HNIrrigationCriteria >::iterator it = m_criteriaMap.begin(); it != m_criteriaMap.end(); it++ )
    { 
        HNCObj *objPtr;

        // Aquire a new list entry
        listPtr->appendObj( &objPtr );

        // Fill the entry with the static event info
        objPtr->updateValue( "criteriaid", it->second.getID() );

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

