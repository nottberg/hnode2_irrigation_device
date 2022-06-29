#include <iostream>
#include <regex>

#include "HNIrrigationSequence.h"

HNIrrigationSequence::HNIrrigationSequence()
{
    m_type = HNISQ_TYPE_UNIFORM;
    m_onDuration  = 0;
    m_offDuration = 0;
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
HNIrrigationSequence::setOnDuration( uint seconds )
{
    m_onDuration = seconds;
}

void 
HNIrrigationSequence::setOffDuration( uint seconds )
{
    m_offDuration = seconds;
}

void 
HNIrrigationSequence::clearObjList()
{
    m_objList.clear();
}

void 
HNIrrigationSequence::addObj( std::string objID )
{
   m_objList.push_back( objID );
}

void 
HNIrrigationSequence::setObjListFromStr( std::string objListStr )
{
    const std::regex ws_re("\\s+"); // whitespace

    clearObjList();
    std::sregex_token_iterator it( objListStr.begin(), objListStr.end(), ws_re, -1 );
    const std::sregex_token_iterator end;
    while( it != end )
    {
        addObj( *it );
        it++;
    }
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

uint
HNIrrigationSequence::getOnDuration()
{
    return m_onDuration;
}

uint
HNIrrigationSequence::getOffDuration()
{
    return m_offDuration;
}

std::list< std::string >& 
HNIrrigationSequence::getObjListRef()
{
    return m_objList;
}

std::string 
HNIrrigationSequence::getObjListAsStr()
{
    std::string rspStr;

    bool first = true;
    for( std::list<std::string>::iterator it = m_objList.begin(); it != m_objList.end(); it++ )
    {
        if( first == false )
            rspStr += " ";
        rspStr += *it;
        first = false;
    }

    return rspStr;
}

HNIS_RESULT_T 
HNIrrigationSequence::setTypeFromStr( std::string typeStr )
{
    m_type = HNISQ_TYPE_NOTSET;
    
    if( typeStr == "uniform" )
        m_type = HNISQ_TYPE_UNIFORM;
    else if( typeStr == "chain" )
        m_type = HNISQ_TYPE_CHAIN;
    else
        return HNIS_RESULT_FAILURE;
    
    return HNIS_RESULT_SUCCESS;
}

std::string 
HNIrrigationSequence::getTypeAsStr()
{
    switch( m_type )
    {
        case HNISQ_TYPE_UNIFORM:
            return "uniform";
        break;
        
        case HNISQ_TYPE_CHAIN:
            return "chain";
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

        if( objPtr->getValueByName( "onDuration", rstStr ) == HNC_RESULT_SUCCESS )
        {
            sequencePtr->setOnDuration( strtol( rstStr.c_str(), NULL, 0) );
        }

        if( objPtr->getValueByName( "offDuration", rstStr ) == HNC_RESULT_SUCCESS )
        {
            sequencePtr->setOffDuration( strtol( rstStr.c_str(), NULL, 0) );
        }

        if( objPtr->getValueByName( "objList", rstStr ) == HNC_RESULT_SUCCESS )
        {
            sequencePtr->setObjListFromStr( rstStr );
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
        char tmpBuf[128];

        // Aquire a new list entry
        listPtr->appendObj( &objPtr );

        // Fill the entry with the static event info
        objPtr->updateValue( "sequenceid", it->second.getID() );

        objPtr->updateValue( "name", it->second.getName() );
        objPtr->updateValue( "description", it->second.getDesc() );
        objPtr->updateValue( "type", it->second.getTypeAsStr() );

        sprintf( tmpBuf, "%u", it->second.getOnDuration() );
        objPtr->updateValue( "onDuration", tmpBuf );

        sprintf( tmpBuf, "%u", it->second.getOffDuration() );        
        objPtr->updateValue( "offDuration", tmpBuf );

        objPtr->updateValue( "objList", it->second.getObjListAsStr() );
    }

    return HNIS_RESULT_SUCCESS;
}


