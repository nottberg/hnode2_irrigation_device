#include <iostream>
#include <regex>

#include "HNIrrigationInhibit.h"

HNIrrigationInhibit::HNIrrigationInhibit()
{
    m_type = HNII_TYPE_NOTSET;
    m_expiration = 0;
}

HNIrrigationInhibit::~HNIrrigationInhibit()
{

}

void 
HNIrrigationInhibit::setID( std::string id )
{
    m_id = id;
}

void 
HNIrrigationInhibit::setName( std::string value )
{
    m_name = value;
}

void 
HNIrrigationInhibit::setDesc( std::string value )
{
    m_desc = value;
}

void 
HNIrrigationInhibit::setType( HNII_TYPE_T type )
{
    m_type = type;
}

void 
HNIrrigationInhibit::setZoneID( std::string zoneid )
{
    m_zoneid = zoneid;
}

void
HNIrrigationInhibit::setExpiration( time_t timeVal )
{
    m_expiration = timeVal;
}
    
void 
HNIrrigationInhibit::setExpirationFromDurationStr( time_t curTime, std::string durationStr )
{
    HNI24HTime duration;

    m_expiration = curTime;

    if( duration.parseTime( durationStr ) == HNIS_RESULT_SUCCESS )
        m_expiration += duration.getSeconds();
}

std::string 
HNIrrigationInhibit::getID()
{
    return m_id;
}

std::string 
HNIrrigationInhibit::getName()
{
    return m_name;
}

std::string 
HNIrrigationInhibit::getDesc()
{
    return m_desc;
}

HNII_TYPE_T 
HNIrrigationInhibit::getType()
{
    return m_type;
}

std::string 
HNIrrigationInhibit::getZoneID()
{
    return m_zoneid;
}

time_t
HNIrrigationInhibit::getExpiration()
{
    return m_expiration;
}

HNIS_RESULT_T 
HNIrrigationInhibit::setTypeFromStr( std::string typeStr )
{
    m_type = HNII_TYPE_NOTSET;
    
    if( typeStr == "scheduler" )
        m_type = HNII_TYPE_SCHEDULER;
    else if( typeStr == "scheduler_noexpire" )
        m_type = HNII_TYPE_SCHEDULER_NOEXPIRE;
    else if( typeStr == "zone" )
        m_type = HNII_TYPE_ZONE;
    else if( typeStr == "zone_noexpire" )
        m_type = HNII_TYPE_ZONE_NOEXPIRE;
    else
        return HNIS_RESULT_FAILURE;
    
    return HNIS_RESULT_SUCCESS;
}

std::string 
HNIrrigationInhibit::getTypeAsStr()
{
    switch( m_type )
    {
        case HNII_TYPE_SCHEDULER:
            return "scheduler";
        break;

        case HNII_TYPE_SCHEDULER_NOEXPIRE:
            return "scheduler_noexpire";
        break;

        case HNII_TYPE_ZONE:
            return "zone";
        break;

        case HNII_TYPE_ZONE_NOEXPIRE:
            return "zone_noexpire";
        break;

        default:
        break;
    }
    
    return "notset";
}

HNIS_RESULT_T 
HNIrrigationInhibit::validateSettings()
{
    // Add validation checking here
    return HNIS_RESULT_SUCCESS;
}

HNIrrigationInhibitSet::HNIrrigationInhibitSet()
{

}

HNIrrigationInhibitSet::~HNIrrigationInhibitSet()
{

}

void
HNIrrigationInhibitSet::clear()
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    m_curSchInhibitID.clear();
    m_zoneIDMap.clear();

    m_inhibitsMap.clear();
}

bool 
HNIrrigationInhibitSet::hasID( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    std::map< std::string, HNIrrigationInhibit >::iterator it = m_inhibitsMap.find( id );

    if( it == m_inhibitsMap.end() )
        return false;

    return true;
}

HNIrrigationInhibit*
HNIrrigationInhibitSet::internalUpdateInhibit( std::string id )
{
    std::map< std::string, HNIrrigationInhibit >::iterator it = m_inhibitsMap.find( id );

    if( it == m_inhibitsMap.end() )
    {
        HNIrrigationInhibit nSpec;
        nSpec.setID( id );

        m_inhibitsMap.insert( std::pair< std::string, HNIrrigationInhibit >( id, nSpec ) );\
        return &( m_inhibitsMap[ id ] );
    }

    return &(it->second);
}

HNIrrigationInhibit*
HNIrrigationInhibitSet::updateInhibit( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    return internalUpdateInhibit( id );
}

void
HNIrrigationInhibitSet::reconcileNewObject( std::string newID )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Find the referenced zone
    std::map< std::string, HNIrrigationInhibit >::iterator it = m_inhibitsMap.find( newID );

    // If we can't find something is wrong, don't proceed
    if( it == m_inhibitsMap.end() )
        return;
    
    // Figure out what to do based on type
    switch( it->second.getType() )
    {
        case HNII_TYPE_SCHEDULER:
        case HNII_TYPE_SCHEDULER_NOEXPIRE:
        {
            if( m_curSchInhibitID.empty() == false )
                deleteInhibit( m_curSchInhibitID );

            m_curSchInhibitID = newID;
        }
        break;

        case HNII_TYPE_ZONE:
        case HNII_TYPE_ZONE_NOEXPIRE:
        {
            // Find the referenced inhibit
            std::map< std::string, std::string >::iterator zit = m_zoneIDMap.find( it->second.getZoneID() );

            if( zit != m_zoneIDMap.end() )
                deleteInhibit( zit->second );

            m_zoneIDMap.insert( std::pair< std::string, std::string> ( it->second.getZoneID(), newID ) );
        }
        break;
    }
}

void 
HNIrrigationInhibitSet::deleteInhibit( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Find the referenced inhibit
    std::map< std::string, HNIrrigationInhibit >::iterator it = m_inhibitsMap.find( id );

    // If already non-existant than nothing to do.
    if( it == m_inhibitsMap.end() )
        return;

    // Figure out what to do based on type
    switch( it->second.getType() )
    {
        case HNII_TYPE_SCHEDULER:
        case HNII_TYPE_SCHEDULER_NOEXPIRE:
        {
            // Cleanup the scheduler reference.
            if( m_curSchInhibitID == id )
                m_curSchInhibitID.clear();
        }
        break;

        case HNII_TYPE_ZONE:
        case HNII_TYPE_ZONE_NOEXPIRE:
        {
            // Cleanup the zone reference
            m_zoneIDMap.erase( it->second.getZoneID() );
        }
        break;
    }

    // Get rid of the inhibit record
    m_inhibitsMap.erase( it );
}

void 
HNIrrigationInhibitSet::getInhibitsList( std::vector< HNIrrigationInhibit > &inhibitsList )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    for( std::map< std::string, HNIrrigationInhibit >::iterator it = m_inhibitsMap.begin(); it != m_inhibitsMap.end(); it++ )
    {
        inhibitsList.push_back( it->second );
    }
}

HNIS_RESULT_T 
HNIrrigationInhibitSet::getInhibit( std::string id, HNIrrigationInhibit &event )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    std::map< std::string, HNIrrigationInhibit >::iterator it = m_inhibitsMap.find( id );

    if( it == m_inhibitsMap.end() )
        return HNIS_RESULT_FAILURE;

    event = it->second;
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationInhibitSet::getInhibitName( std::string id, std::string &name )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    std::map< std::string, HNIrrigationInhibit >::iterator it = m_inhibitsMap.find( id );

    name.clear();

    if( it == m_inhibitsMap.end() )
        return HNIS_RESULT_FAILURE;

    name = it->second.getName();
    return HNIS_RESULT_SUCCESS;
}

HNII_INHIBIT_ACTION_T 
HNIrrigationInhibitSet::checkSchedulerAction( time_t curTime, std::string &inhibitID )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);
  
    // Clear the inhibitID return value
    inhibitID.clear();

    // If no scheduler inbibit is in place then
    // return no action. 
    if( m_curSchInhibitID.empty() == true )
        return HNII_INHIBIT_ACTION_NONE;

    // Find the referenced inhibit
    std::map< std::string, HNIrrigationInhibit >::iterator it = m_inhibitsMap.find( m_curSchInhibitID );

    // If we can't find something is wrong, cleanup
    if( it == m_inhibitsMap.end() )
    {
        m_curSchInhibitID.clear();
        return HNII_INHIBIT_ACTION_NONE;
    }

    // Set the inhibit ID return value
    inhibitID = m_curSchInhibitID; 

    // If the inhibit has no expiration then
    // always return active.
    if( it->second.getType() == HNII_TYPE_SCHEDULER_NOEXPIRE )
        return HNII_INHIBIT_ACTION_ACTIVE;

    // Check if the inhibit is still in effect, if so then
    // return active.
    if( it->second.getExpiration() > curTime )
        return HNII_INHIBIT_ACTION_ACTIVE;

    // Inhibit has finished, signal expired for removal.
    return HNII_INHIBIT_ACTION_EXPIRED;
}

HNII_INHIBIT_ACTION_T 
HNIrrigationInhibitSet::checkZoneAction( time_t curTime, std::string zoneID, std::string &inhibitID )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Clear the inhibitID return value
    inhibitID.clear();

    // Check if there is an inhibit that cooresponds to the requested zoneID
    std::map< std::string, std::string >::iterator idit = m_zoneIDMap.find( zoneID );
    if( idit == m_zoneIDMap.end() )
        return HNII_INHIBIT_ACTION_NONE;

    // Find the referenced inhibit
    std::map< std::string, HNIrrigationInhibit >::iterator it = m_inhibitsMap.find( idit->second );

    // If we can't find something is wrong, cleanup
    if( it == m_inhibitsMap.end() )
    {
        m_zoneIDMap.erase( zoneID );
        return HNII_INHIBIT_ACTION_NONE;
    }

    // Set the inhibit ID return value
    inhibitID = idit->second; 

    // If the inhibit has no expiration then
    // always return active.
    if( it->second.getType() == HNII_TYPE_ZONE_NOEXPIRE )
        return HNII_INHIBIT_ACTION_ACTIVE;

    // Check if the inhibit is still in effect, if so then
    // return active.
    if( it->second.getExpiration() > curTime )
        return HNII_INHIBIT_ACTION_ACTIVE;

    // Inhibit has finished, signal expired for removal.
    return HNII_INHIBIT_ACTION_EXPIRED;
}

HNIS_RESULT_T 
HNIrrigationInhibitSet::initInhibitsListSection( HNodeConfig &cfg )
{
    HNCSection *secPtr;

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    cfg.updateSection( "irrInhibitsInfo", &secPtr );
    
    HNCObjList *listPtr;
    secPtr->updateList( "inhibitsList", &listPtr );

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationInhibitSet::readInhibitsListSection( HNodeConfig &cfg )
{
    HNCSection  *secPtr;

    std::cout << "rc1" << std::endl;

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Aquire a pointer to the "device" section
    cfg.updateSection( "irrInhibitsInfo", &secPtr );

    // Get a list pointer
    HNCObjList *listPtr;
    secPtr->updateList( "inhibitsList", &listPtr );

    std::cout << "rc2: " << listPtr->size() << std::endl;

    for( uint indx = 0; indx < listPtr->size(); indx++ )
    {
        std::string inhibitID;
        std::string rstStr;
        HNCObj *objPtr;

        if( listPtr->getObjPtr( indx, &objPtr ) != HNC_RESULT_SUCCESS )
            continue;

        // Get the zoneID first, if missing skip the record
        if( objPtr->getValueByName( "inhibitid", inhibitID ) != HNC_RESULT_SUCCESS )
        {
            continue;
        }

        // Get the internal reference to the zone.
        HNIrrigationInhibit *inhibitPtr = internalUpdateInhibit( inhibitID );

        if( objPtr->getValueByName( "name", rstStr ) == HNC_RESULT_SUCCESS )
        {
            inhibitPtr->setName( rstStr );
        }

        if( objPtr->getValueByName( "description", rstStr ) == HNC_RESULT_SUCCESS )
        {
            inhibitPtr->setDesc( rstStr );
        }

        if( objPtr->getValueByName( "type", rstStr ) == HNC_RESULT_SUCCESS )
        {
            inhibitPtr->setTypeFromStr( rstStr );
        }

        if( objPtr->getValueByName( "zoneid", rstStr ) == HNC_RESULT_SUCCESS )
        {
            inhibitPtr->setZoneID( rstStr );
        }

        if( objPtr->getValueByName( "expiration", rstStr ) == HNC_RESULT_SUCCESS )
        {
            inhibitPtr->setExpiration( strtol( rstStr.c_str(), NULL, 0 ) );
        }

    }
          
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationInhibitSet::updateInhibitsListSection( HNodeConfig &cfg )
{
    char tmpStr[256];
    HNCSection *secPtr;
    HNCObjList *listPtr;

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    cfg.updateSection( "irrInhibitsInfo", &secPtr );

    secPtr->updateList( "inhibitsList", &listPtr );

    for( std::map< std::string, HNIrrigationInhibit >::iterator it = m_inhibitsMap.begin(); it != m_inhibitsMap.end(); it++ )
    { 
        HNCObj *objPtr;
        char tmpStr[64];

        // Aquire a new list entry
        listPtr->appendObj( &objPtr );

        // Fill the entry with the static event info
        objPtr->updateValue( "inhibitid", it->second.getID() );

        objPtr->updateValue( "name", it->second.getName() );
        objPtr->updateValue( "description", it->second.getDesc() );
        objPtr->updateValue( "type", it->second.getTypeAsStr() );
        objPtr->updateValue( "zoneid", it->second.getZoneID() );

        sprintf( tmpStr, "%lu", it->second.getExpiration() );
        objPtr->updateValue( "expiration", tmpStr );
    }

    return HNIS_RESULT_SUCCESS;
}


