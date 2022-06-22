#include <iostream>
#include <regex>

#include "HNIrrigationInhibit.h"

HNIrrigationInhibit::HNIrrigationInhibit()
{
    m_type = HNII_TYPE_NOTSET;
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
HNIrrigationInhibit::setValue( std::string value )
{
    m_value = value;
}

void 
HNIrrigationInhibit::setZoneID( std::string zoneid )
{
    m_zoneid = zoneid;
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
HNIrrigationInhibit::getValue()
{
    return m_value;
}

std::string 
HNIrrigationInhibit::getZoneID()
{
    return m_zoneid;
}

HNIS_RESULT_T 
HNIrrigationInhibit::setTypeFromStr( std::string typeStr )
{
    m_type = HNII_TYPE_NOTSET;
    
    if( typeStr == "local.duration" )
        m_type = HNII_TYPE_LOCAL_DURATION;
    else if( typeStr == "local.percent" )
        m_type = HNII_TYPE_LOCAL_PERCENT;
    else
        return HNIS_RESULT_FAILURE;
    
    return HNIS_RESULT_SUCCESS;
}

std::string 
HNIrrigationInhibit::getTypeAsStr()
{
    switch( m_type )
    {
        case HNII_TYPE_LOCAL_DURATION:
            return "local.duration";
        break;
        
        case HNII_TYPE_LOCAL_PERCENT:
            return "local.percent";
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

double
HNIrrigationInhibit::calculateDelta( uint baseDuration, std::string &appliedValue )
{
    double delta = 0;
    char   avStr[256];
    
    switch( getType() )
    {
        case HNII_TYPE_LOCAL_DURATION:
        {
            double value = strtod( m_value.c_str(), NULL );
            delta = value;
            sprintf(avStr, "%f", delta);
        }
        break;
    
        case HNII_TYPE_LOCAL_PERCENT:
        {
            double value = strtod( m_value.c_str(), NULL );
            
            value /= 100.0;
            delta = ((double)baseDuration) * value;
            sprintf(avStr, "%f%%", delta);
        }
        break;
        
        default:
            avStr[0] = '\0';
        break;
    }
    
    appliedValue = avStr;

    return delta;
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
HNIrrigationInhibitSet::deleteInhibit( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Find the referenced zone
    std::map< std::string, HNIrrigationInhibit >::iterator it = m_inhibitsMap.find( id );

    // If already no existant than nothing to do.
    if( it == m_inhibitsMap.end() )
        return;

    // Get rid of the zone record
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

void 
HNIrrigationInhibitSet::getInhibitsForZone( std::string zoneID, std::vector< HNIrrigationInhibit > &inhibitsList )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    inhibitsList.clear();
    
    for( std::map< std::string, HNIrrigationInhibit >::iterator it = m_inhibitsMap.begin(); it != m_inhibitsMap.end(); it++ )
    {
        if( it->second.getZoneID() == zoneID )
            inhibitsList.push_back( it->second );
    }
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

        if( objPtr->getValueByName( "value", rstStr ) == HNC_RESULT_SUCCESS )
        {
            inhibitPtr->setValue( rstStr );
        }

        if( objPtr->getValueByName( "zoneid", rstStr ) == HNC_RESULT_SUCCESS )
        {
            inhibitPtr->setZoneID( rstStr );
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

        // Aquire a new list entry
        listPtr->appendObj( &objPtr );

        // Fill the entry with the static event info
        objPtr->updateValue( "inhibitid", it->second.getID() );

        objPtr->updateValue( "name", it->second.getName() );
        objPtr->updateValue( "description", it->second.getDesc() );
        objPtr->updateValue( "type", it->second.getTypeAsStr() );
        objPtr->updateValue( "value", it->second.getValue() );
        objPtr->updateValue( "zoneid", it->second.getZoneID() );
    }

    return HNIS_RESULT_SUCCESS;
}


