#include <iostream>
#include <sstream>
#include <regex>

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

bool 
HNIrrigationZone::hasSwitch( std::string swid )
{
    std::set< std::string >::iterator it = m_swidSet.find( swid );

    if( it == m_swidSet.end() )
        return false;

    return true;
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

void 
HNIrrigationZone::clearStatus()
{
    m_statusFlags = HNIZ_STATUS_NOTSET;
    m_inhibitUntil.clear();
}

void 
HNIrrigationZone::setStatusActive()
{
    m_statusFlags |= HNIZ_STATUS_ACTIVE;
}

void 
HNIrrigationZone::setStatusDisabled()
{
    m_statusFlags |= HNIZ_STATUS_DISABLED;
}

void 
HNIrrigationZone::setStatusInhibited( std::string until )
{
    m_statusFlags |= HNIZ_STATUS_INHIBITED;
    m_inhibitUntil = until;
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

std::string 
HNIrrigationZone::getInhibitedUntil()
{
    return m_inhibitUntil;
}

bool 
HNIrrigationZone::isActive()
{
    return ( ( m_statusFlags & HNIZ_STATUS_ACTIVE ) == HNIZ_STATUS_ACTIVE );
}

bool 
HNIrrigationZone::isDisabled()
{
    return ( ( m_statusFlags & HNIZ_STATUS_DISABLED ) == HNIZ_STATUS_DISABLED );
}

bool 
HNIrrigationZone::isInhibited()
{
    return ( ( m_statusFlags & HNIZ_STATUS_INHIBITED ) == HNIZ_STATUS_INHIBITED );
}

HNIrrigationZoneSet::HNIrrigationZoneSet()
{

}

HNIrrigationZoneSet::~HNIrrigationZoneSet()
{

}

void
HNIrrigationZoneSet::clear()
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    m_zoneMap.clear();
}

bool 
HNIrrigationZoneSet::hasID( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.find( id );

    if( it == m_zoneMap.end() )
        return false;

    return true;
}

void 
HNIrrigationZoneSet::getZoneList( std::vector< HNIrrigationZone > &zoneList )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    for( std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.begin(); it != m_zoneMap.end(); it++ )
    {
        zoneList.push_back( it->second );
    }
}

HNIS_RESULT_T 
HNIrrigationZoneSet::getZone( std::string id, HNIrrigationZone &zone )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.find( id );

    if( it == m_zoneMap.end() )
        return HNIS_RESULT_FAILURE;

    zone = it->second;
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationZoneSet::getZoneName( std::string id, std::string &name )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.find( id );

    name.clear();

    if( it == m_zoneMap.end() )
        return HNIS_RESULT_FAILURE;

    name = it->second.getName();
    return HNIS_RESULT_SUCCESS;
}

HNIrrigationZone*
HNIrrigationZoneSet::internalUpdateZone( std::string id )
{
    std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.find( id );

    if( it == m_zoneMap.end() )
    {
        HNIrrigationZone nZone;
        nZone.setID( id );
        m_zoneMap.insert( std::pair< std::string, HNIrrigationZone >( id, nZone ) );\
        return &( m_zoneMap[ id ] );
    }

    return &(it->second);
}

HNIrrigationZone*
HNIrrigationZoneSet::updateZone( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    return internalUpdateZone( id );
}

void 
HNIrrigationZoneSet::deleteZone( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Find the referenced zone
    std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.find( id );

    // If already no existant than nothing to do.
    if( it == m_zoneMap.end() )
        return;

    // Get rid of the zone record
    m_zoneMap.erase( it );
}

HNIS_RESULT_T 
HNIrrigationZoneSet::initZoneListSection( HNodeConfig &cfg )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    HNCSection *secPtr;

    cfg.updateSection( "irrZoneInfo", &secPtr );
    
    HNCObjList *listPtr;
    secPtr->updateList( "zoneList", &listPtr );

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationZoneSet::readZoneListSection( HNodeConfig &cfg )
{
    HNCSection  *secPtr;

    std::cout << "rz1" << std::endl;

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Aquire a pointer to the "device" section
    cfg.updateSection( "irrZoneInfo", &secPtr );

    // Get a list pointer
    HNCObjList *listPtr;
    secPtr->updateList( "zoneList", &listPtr );

    std::cout << "rz2: " << listPtr->size() << std::endl;

    for( uint indx = 0; indx < listPtr->size(); indx++ )
    {
        std::string zoneID;
        std::string rstStr;
        HNCObj *objPtr;

        if( listPtr->getObjPtr( indx, &objPtr ) != HNC_RESULT_SUCCESS )
            continue;

        // Get the zoneID first, if missing skip the record
        if( objPtr->getValueByName( "zoneid", zoneID ) != HNC_RESULT_SUCCESS )
        {
            continue;
        }

        // Get the internal reference to the zone.
        HNIrrigationZone *zonePtr = internalUpdateZone( zoneID );

        if( objPtr->getValueByName( "name", rstStr ) == HNC_RESULT_SUCCESS )
        {
            std::cout << "== READ ZONE NAME: " << rstStr << std::endl;
            zonePtr->setName( rstStr );
        }

        if( objPtr->getValueByName( "description", rstStr ) == HNC_RESULT_SUCCESS )
        {
            zonePtr->setDesc( rstStr );
        }

        if( objPtr->getValueByName( "secondsPerWeek", rstStr ) == HNC_RESULT_SUCCESS )
        {
            zonePtr->setWeeklySeconds( strtol( rstStr.c_str(), NULL, 0 ) );
        }

        if( objPtr->getValueByName( "secondsMaxCycle", rstStr ) == HNC_RESULT_SUCCESS )
        {
            zonePtr->setMaximumCycleTimeSeconds( strtol( rstStr.c_str(), NULL, 0 ) );
        }

        if( objPtr->getValueByName( "secondsMinCycle", rstStr ) == HNC_RESULT_SUCCESS )
        {
            zonePtr->setMinimumCycleTimeSeconds( strtol( rstStr.c_str(), NULL, 0 ) );
        }

        if( objPtr->getValueByName( "swidList", rstStr ) == HNC_RESULT_SUCCESS )
        {
            const std::regex ws_re("\\s+"); // whitespace

            std::cout << "== READ SWIDLIST: " << rstStr << std::endl;

            zonePtr->clearSWIDSet();

            // Walk the switch List string
            std::sregex_token_iterator it( rstStr.begin(), rstStr.end(), ws_re, -1 );
            const std::sregex_token_iterator end;
            while( it != end )
            {
                // Add a new switch id.
                zonePtr->addSWID( *it );
                it++;
            }
        }

    }
          
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationZoneSet::updateZoneListSection( HNodeConfig &cfg )
{
    char tmpStr[256];

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    HNCSection *secPtr;
    cfg.updateSection( "irrZoneInfo", &secPtr );

    HNCObjList *listPtr;
    secPtr->updateList( "zoneList", &listPtr );

    for( std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.begin(); it != m_zoneMap.end(); it++ )
    { 
        HNCObj *objPtr;

        // Aquire a new list entry
        listPtr->appendObj( &objPtr );

        // Fill the entry with the zone info
        objPtr->updateValue( "zoneid", it->second.getID() );
        objPtr->updateValue( "name", it->second.getName() );
        objPtr->updateValue( "description", it->second.getDesc() );

        sprintf( tmpStr, "%d", it->second.getWeeklySeconds() );
        objPtr->updateValue( "secondsPerWeek", tmpStr );

        sprintf( tmpStr, "%d", it->second.getMaximumCycleTimeSeconds() );
        objPtr->updateValue( "secondsMaxCycle", tmpStr );

        sprintf( tmpStr, "%d", it->second.getMinimumCycleTimeSeconds() );
        objPtr->updateValue( "secondsMinCycle", tmpStr );

        objPtr->updateValue( "swidList", it->second.getSWIDListStr() );    
    }

    return HNIS_RESULT_SUCCESS;
}

void 
HNIrrigationZoneSet::clearStatus()
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    for( std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.begin(); it != m_zoneMap.end(); it++ )
        it->second.clearStatus(); 

}

void 
HNIrrigationZoneSet::setStatusActive( std::string swid )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    for( std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.begin(); it != m_zoneMap.end(); it++ )
    {
        if( it->second.hasSwitch( swid ) )
            it->second.setStatusActive();
    }
}

void 
HNIrrigationZoneSet::setStatusDisabled( std::string swid )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    for( std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.begin(); it != m_zoneMap.end(); it++ )
    {
        if( it->second.hasSwitch( swid ) )
            it->second.setStatusDisabled();
    }
}

void 
HNIrrigationZoneSet::setStatusInhibited( std::string swid, std::string until )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    for( std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.begin(); it != m_zoneMap.end(); it++ )
    {
        if( it->second.hasSwitch( swid ) )
            it->second.setStatusInhibited( until );
    }
}

void 
HNIrrigationZoneSet::getActiveZones( std::vector< HNIrrigationZone > &zoneList )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    for( std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.begin(); it != m_zoneMap.end(); it++ )
    {
        if( it->second.isActive() )
            zoneList.push_back( it->second );
    }
}

void 
HNIrrigationZoneSet::getDisabledZones( std::vector< HNIrrigationZone > &zoneList )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    for( std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.begin(); it != m_zoneMap.end(); it++ )
    {
        if( it->second.isDisabled() )
            zoneList.push_back( it->second );
    }
}

void 
HNIrrigationZoneSet::getInhibitedZones( std::vector< HNIrrigationZone > &zoneList )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    for( std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.begin(); it != m_zoneMap.end(); it++ )
    {
        if( it->second.isInhibited() )
            zoneList.push_back( it->second );
    }
}

