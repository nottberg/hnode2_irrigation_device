#include <iostream>
#include <regex>

#include "HNIrrigationSequence.h"

HNIrrigationSequence::HNIrrigationSequence()
{
    m_type = HNISQ_TYPE_NOTSET;
}

HNIrrigationSequence::~HNIrrigationSequence()
{

}

void 
HNIrrigationSequence::setID( std::string id )
{
    m_id = id;
}

void 
HNIrrigationSequence::setName( std::string value )
{
    m_name = value;
}

void 
HNIrrigationSequence::setDesc( std::string value )
{
    m_desc = value;
}

void 
HNIrrigationSequence::setType( HNISQ_TYPE_T type )
{
    m_type = type;
}

void 
HNIrrigationSequence::setValue( std::string value )
{
    m_value = value;
}

void 
HNIrrigationSequence::setZoneID( std::string zoneid )
{
    m_zoneid = zoneid;
}

std::string 
HNIrrigationSequence::getID()
{
    return m_id;
}

std::string 
HNIrrigationSequence::getName()
{
    return m_name;
}

std::string 
HNIrrigationSequence::getDesc()
{
    return m_desc;
}

HNISQ_TYPE_T 
HNIrrigationSequence::getType()
{
    return m_type;
}

std::string 
HNIrrigationSequence::getValue()
{
    return m_value;
}

std::string 
HNIrrigationSequence::getZoneID()
{
    return m_zoneid;
}

HNIS_RESULT_T 
HNIrrigationSequence::setTypeFromStr( std::string typeStr )
{
    m_type = HNISQ_TYPE_NOTSET;
    
    if( typeStr == "local.duration" )
        m_type = HNISQ_TYPE_LOCAL_DURATION;
    else if( typeStr == "local.percent" )
        m_type = HNISQ_TYPE_LOCAL_PERCENT;
    else
        return HNIS_RESULT_FAILURE;
    
    return HNIS_RESULT_SUCCESS;
}

std::string 
HNIrrigationSequence::getTypeAsStr()
{
    switch( m_type )
    {
        case HNISQ_TYPE_LOCAL_DURATION:
            return "local.duration";
        break;
        
        case HNISQ_TYPE_LOCAL_PERCENT:
            return "local.percent";
        break;
        
        default:
        break;
    }
    
    return "notset";
}

HNIS_RESULT_T 
HNIrrigationSequence::validateSettings()
{
    // Add validation checking here
    return HNIS_RESULT_SUCCESS;
}

double
HNIrrigationSequence::calculateDelta( uint baseDuration, std::string &appliedValue )
{
    double delta = 0;
    char   avStr[256];
    
    switch( getType() )
    {
        case HNISQ_TYPE_LOCAL_DURATION:
        {
            double value = strtod( m_value.c_str(), NULL );
            delta = value;
            sprintf(avStr, "%f", delta);
        }
        break;
    
        case HNISQ_TYPE_LOCAL_PERCENT:
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

HNIrrigationSequenceSet::HNIrrigationSequenceSet()
{

}

HNIrrigationSequenceSet::~HNIrrigationSequenceSet()
{

}

void
HNIrrigationSequenceSet::clear()
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    m_sequencesMap.clear();
}

bool 
HNIrrigationSequenceSet::hasID( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    std::map< std::string, HNIrrigationSequence >::iterator it = m_sequencesMap.find( id );

    if( it == m_sequencesMap.end() )
        return false;

    return true;
}

HNIrrigationSequence*
HNIrrigationSequenceSet::internalUpdateSequence( std::string id )
{
    std::map< std::string, HNIrrigationSequence >::iterator it = m_sequencesMap.find( id );

    if( it == m_sequencesMap.end() )
    {
        HNIrrigationSequence nSpec;
        nSpec.setID( id );
        m_sequencesMap.insert( std::pair< std::string, HNIrrigationSequence >( id, nSpec ) );\
        return &( m_sequencesMap[ id ] );
    }

    return &(it->second);
}

HNIrrigationSequence*
HNIrrigationSequenceSet::updateSequence( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    return internalUpdateSequence( id );
}

void 
HNIrrigationSequenceSet::deleteSequence( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Find the referenced zone
    std::map< std::string, HNIrrigationSequence >::iterator it = m_sequencesMap.find( id );

    // If already no existant than nothing to do.
    if( it == m_sequencesMap.end() )
        return;

    // Get rid of the zone record
    m_sequencesMap.erase( it );
}

void 
HNIrrigationSequenceSet::getSequencesList( std::vector< HNIrrigationSequence > &sequencesList )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    for( std::map< std::string, HNIrrigationSequence >::iterator it = m_sequencesMap.begin(); it != m_sequencesMap.end(); it++ )
    {
        sequencesList.push_back( it->second );
    }
}

HNIS_RESULT_T 
HNIrrigationSequenceSet::getSequence( std::string id, HNIrrigationSequence &event )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    std::map< std::string, HNIrrigationSequence >::iterator it = m_sequencesMap.find( id );

    if( it == m_sequencesMap.end() )
        return HNIS_RESULT_FAILURE;

    event = it->second;
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationSequenceSet::getSequenceName( std::string id, std::string &name )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    std::map< std::string, HNIrrigationSequence >::iterator it = m_sequencesMap.find( id );

    name.clear();

    if( it == m_sequencesMap.end() )
        return HNIS_RESULT_FAILURE;

    name = it->second.getName();
    return HNIS_RESULT_SUCCESS;
}

void 
HNIrrigationSequenceSet::getSequencesForZone( std::string zoneID, std::vector< HNIrrigationSequence > &sequencesList )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    sequencesList.clear();
    
    for( std::map< std::string, HNIrrigationSequence >::iterator it = m_sequencesMap.begin(); it != m_sequencesMap.end(); it++ )
    {
        if( it->second.getZoneID() == zoneID )
            sequencesList.push_back( it->second );
    }
}

HNIS_RESULT_T 
HNIrrigationSequenceSet::initSequencesListSection( HNodeConfig &cfg )
{
    HNCSection *secPtr;

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    cfg.updateSection( "irrSequencesInfo", &secPtr );
    
    HNCObjList *listPtr;
    secPtr->updateList( "sequencesList", &listPtr );

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationSequenceSet::readSequencesListSection( HNodeConfig &cfg )
{
    HNCSection  *secPtr;

    std::cout << "rc1" << std::endl;

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Aquire a pointer to the "device" section
    cfg.updateSection( "irrSequencesInfo", &secPtr );

    // Get a list pointer
    HNCObjList *listPtr;
    secPtr->updateList( "sequencesList", &listPtr );

    std::cout << "rc2: " << listPtr->size() << std::endl;

    for( uint indx = 0; indx < listPtr->size(); indx++ )
    {
        std::string sequenceID;
        std::string rstStr;
        HNCObj *objPtr;

        if( listPtr->getObjPtr( indx, &objPtr ) != HNC_RESULT_SUCCESS )
            continue;

        // Get the zoneID first, if missing skip the record
        if( objPtr->getValueByName( "sequenceid", sequenceID ) != HNC_RESULT_SUCCESS )
        {
            continue;
        }

        // Get the internal reference to the zone.
        HNIrrigationSequence *sequencePtr = internalUpdateSequence( sequenceID );

        if( objPtr->getValueByName( "name", rstStr ) == HNC_RESULT_SUCCESS )
        {
            sequencePtr->setName( rstStr );
        }

        if( objPtr->getValueByName( "description", rstStr ) == HNC_RESULT_SUCCESS )
        {
            sequencePtr->setDesc( rstStr );
        }

        if( objPtr->getValueByName( "type", rstStr ) == HNC_RESULT_SUCCESS )
        {
            sequencePtr->setTypeFromStr( rstStr );
        }

        if( objPtr->getValueByName( "value", rstStr ) == HNC_RESULT_SUCCESS )
        {
            sequencePtr->setValue( rstStr );
        }

        if( objPtr->getValueByName( "zoneid", rstStr ) == HNC_RESULT_SUCCESS )
        {
            sequencePtr->setZoneID( rstStr );
        }

    }
          
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationSequenceSet::updateSequencesListSection( HNodeConfig &cfg )
{
    char tmpStr[256];
    HNCSection *secPtr;
    HNCObjList *listPtr;

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    cfg.updateSection( "irrSequencesInfo", &secPtr );

    secPtr->updateList( "sequencesList", &listPtr );

    for( std::map< std::string, HNIrrigationSequence >::iterator it = m_sequencesMap.begin(); it != m_sequencesMap.end(); it++ )
    { 
        HNCObj *objPtr;

        // Aquire a new list entry
        listPtr->appendObj( &objPtr );

        // Fill the entry with the static event info
        objPtr->updateValue( "sequenceid", it->second.getID() );

        objPtr->updateValue( "name", it->second.getName() );
        objPtr->updateValue( "description", it->second.getDesc() );
        objPtr->updateValue( "type", it->second.getTypeAsStr() );
        objPtr->updateValue( "value", it->second.getValue() );
        objPtr->updateValue( "zoneid", it->second.getZoneID() );
    }

    return HNIS_RESULT_SUCCESS;
}


