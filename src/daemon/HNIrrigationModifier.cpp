#include <iostream>
#include <regex>
#include <cmath>

#include "HNIrrigationModifier.h"

HNIrrigationModifier::HNIrrigationModifier()
{
    m_type = HNIM_TYPE_NOTSET;
}

HNIrrigationModifier::~HNIrrigationModifier()
{

}

void 
HNIrrigationModifier::setID( std::string id )
{
    m_id = id;
}

void 
HNIrrigationModifier::setName( std::string value )
{
    m_name = value;
}

void 
HNIrrigationModifier::setDesc( std::string value )
{
    m_desc = value;
}

void 
HNIrrigationModifier::setType( HNIM_TYPE_T type )
{
    m_type = type;
}

void 
HNIrrigationModifier::setValue( std::string value )
{
    m_value = value;
}

void 
HNIrrigationModifier::setZoneID( std::string zoneid )
{
    m_zoneid = zoneid;
}

std::string 
HNIrrigationModifier::getID()
{
    return m_id;
}

std::string 
HNIrrigationModifier::getName()
{
    return m_name;
}

std::string 
HNIrrigationModifier::getDesc()
{
    return m_desc;
}

HNIM_TYPE_T 
HNIrrigationModifier::getType()
{
    return m_type;
}

std::string 
HNIrrigationModifier::getValue()
{
    return m_value;
}

std::string 
HNIrrigationModifier::getZoneID()
{
    return m_zoneid;
}

HNIS_RESULT_T 
HNIrrigationModifier::setTypeFromStr( std::string typeStr )
{
    m_type = HNIM_TYPE_NOTSET;
    
    if( typeStr == "local.duration" )
        m_type = HNIM_TYPE_LOCAL_DURATION;
    else if( typeStr == "local.percent" )
        m_type = HNIM_TYPE_LOCAL_PERCENT;
    else
        return HNIS_RESULT_FAILURE;
    
    return HNIS_RESULT_SUCCESS;
}

std::string 
HNIrrigationModifier::getTypeAsStr()
{
    switch( m_type )
    {
        case HNIM_TYPE_LOCAL_DURATION:
            return "local.duration";
        break;
        
        case HNIM_TYPE_LOCAL_PERCENT:
            return "local.percent";
        break;
        
        default:
        break;
    }
    
    return "notset";
}

HNIS_RESULT_T 
HNIrrigationModifier::validateSettings()
{
    // Add validation checking here
    return HNIS_RESULT_SUCCESS;
}

double
HNIrrigationModifier::calculateDelta( uint baseDuration, std::string &calculationStr )
{
    double delta = 0;
    char   calcStr[512];
    
    calculationStr.clear();

    switch( getType() )
    {
        case HNIM_TYPE_LOCAL_DURATION:
        {
            HNI24HTime duration;
            if( duration.parseTime( m_value ) == HNIS_RESULT_SUCCESS )
                delta = duration.getSeconds();
            else
                delta = 0;

            sprintf(calcStr, "%s %.0f seconds", ((delta < 0) ? "Subtract" : "Add"), delta);
        }
        break;
    
        case HNIM_TYPE_LOCAL_PERCENT:
        {
            double value = strtod( m_value.c_str(), NULL );
            
            value /= 100.0;
            if( value < 0 )
            {
                delta = ((double)baseDuration) * value;

                sprintf(calcStr, "Subtract %.0f%% of %d sec base duration", (double)std::abs(value), baseDuration);

            }
            else
            {
                delta = ((double)baseDuration) * value;

                sprintf(calcStr, "Add %.0f%% of %d sec base duration", value, baseDuration);
            }
        }
        break;
        
        default:
            calcStr[0] = '\0';
        break;
    }
    
    calculationStr = calcStr;

    return delta;
}

HNIrrigationModifierSet::HNIrrigationModifierSet()
{

}

HNIrrigationModifierSet::~HNIrrigationModifierSet()
{

}

void
HNIrrigationModifierSet::clear()
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    m_modifiersMap.clear();
}

bool 
HNIrrigationModifierSet::hasID( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    std::map< std::string, HNIrrigationModifier >::iterator it = m_modifiersMap.find( id );

    if( it == m_modifiersMap.end() )
        return false;

    return true;
}

HNIrrigationModifier*
HNIrrigationModifierSet::internalUpdateModifier( std::string id )
{
    std::map< std::string, HNIrrigationModifier >::iterator it = m_modifiersMap.find( id );

    if( it == m_modifiersMap.end() )
    {
        HNIrrigationModifier nSpec;
        nSpec.setID( id );
        m_modifiersMap.insert( std::pair< std::string, HNIrrigationModifier >( id, nSpec ) );\
        return &( m_modifiersMap[ id ] );
    }

    return &(it->second);
}

HNIrrigationModifier*
HNIrrigationModifierSet::updateModifier( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    return internalUpdateModifier( id );
}

void 
HNIrrigationModifierSet::deleteModifier( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Find the referenced zone
    std::map< std::string, HNIrrigationModifier >::iterator it = m_modifiersMap.find( id );

    // If already no existant than nothing to do.
    if( it == m_modifiersMap.end() )
        return;

    // Get rid of the zone record
    m_modifiersMap.erase( it );
}

void 
HNIrrigationModifierSet::getModifiersList( std::vector< HNIrrigationModifier > &modifiersList )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    for( std::map< std::string, HNIrrigationModifier >::iterator it = m_modifiersMap.begin(); it != m_modifiersMap.end(); it++ )
    {
        modifiersList.push_back( it->second );
    }
}

HNIS_RESULT_T 
HNIrrigationModifierSet::getModifier( std::string id, HNIrrigationModifier &event )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    std::map< std::string, HNIrrigationModifier >::iterator it = m_modifiersMap.find( id );

    if( it == m_modifiersMap.end() )
        return HNIS_RESULT_FAILURE;

    event = it->second;
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationModifierSet::getModifierName( std::string id, std::string &name )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    std::map< std::string, HNIrrigationModifier >::iterator it = m_modifiersMap.find( id );

    name.clear();

    if( it == m_modifiersMap.end() )
        return HNIS_RESULT_FAILURE;

    name = it->second.getName();
    return HNIS_RESULT_SUCCESS;
}

void 
HNIrrigationModifierSet::getModifiersForZone( std::string zoneID, std::vector< HNIrrigationModifier > &modifiersList )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    modifiersList.clear();
    
    for( std::map< std::string, HNIrrigationModifier >::iterator it = m_modifiersMap.begin(); it != m_modifiersMap.end(); it++ )
    {
        if( it->second.getZoneID() == zoneID )
            modifiersList.push_back( it->second );
    }
}

HNIS_RESULT_T 
HNIrrigationModifierSet::initModifiersListSection( HNodeConfig &cfg )
{
    HNCSection *secPtr;

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    cfg.updateSection( "irrModifiersInfo", &secPtr );
    
    HNCObjList *listPtr;
    secPtr->updateList( "modifiersList", &listPtr );

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationModifierSet::readModifiersListSection( HNodeConfig &cfg )
{
    HNCSection  *secPtr;

    std::cout << "rc1" << std::endl;

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Aquire a pointer to the "device" section
    cfg.updateSection( "irrModifiersInfo", &secPtr );

    // Get a list pointer
    HNCObjList *listPtr;
    secPtr->updateList( "modifiersList", &listPtr );

    std::cout << "rc2: " << listPtr->size() << std::endl;

    for( uint indx = 0; indx < listPtr->size(); indx++ )
    {
        std::string modifierID;
        std::string rstStr;
        HNCObj *objPtr;

        if( listPtr->getObjPtr( indx, &objPtr ) != HNC_RESULT_SUCCESS )
            continue;

        // Get the zoneID first, if missing skip the record
        if( objPtr->getValueByName( "modifierid", modifierID ) != HNC_RESULT_SUCCESS )
        {
            continue;
        }

        // Get the internal reference to the zone.
        HNIrrigationModifier *modifierPtr = internalUpdateModifier( modifierID );

        if( objPtr->getValueByName( "name", rstStr ) == HNC_RESULT_SUCCESS )
        {
            modifierPtr->setName( rstStr );
        }

        if( objPtr->getValueByName( "description", rstStr ) == HNC_RESULT_SUCCESS )
        {
            modifierPtr->setDesc( rstStr );
        }

        if( objPtr->getValueByName( "type", rstStr ) == HNC_RESULT_SUCCESS )
        {
            modifierPtr->setTypeFromStr( rstStr );
        }

        if( objPtr->getValueByName( "value", rstStr ) == HNC_RESULT_SUCCESS )
        {
            modifierPtr->setValue( rstStr );
        }

        if( objPtr->getValueByName( "zoneid", rstStr ) == HNC_RESULT_SUCCESS )
        {
            modifierPtr->setZoneID( rstStr );
        }

    }
          
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationModifierSet::updateModifiersListSection( HNodeConfig &cfg )
{
    char tmpStr[256];
    HNCSection *secPtr;
    HNCObjList *listPtr;

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    cfg.updateSection( "irrModifiersInfo", &secPtr );

    secPtr->updateList( "modifiersList", &listPtr );

    for( std::map< std::string, HNIrrigationModifier >::iterator it = m_modifiersMap.begin(); it != m_modifiersMap.end(); it++ )
    { 
        HNCObj *objPtr;

        // Aquire a new list entry
        listPtr->appendObj( &objPtr );

        // Fill the entry with the static event info
        objPtr->updateValue( "modifierid", it->second.getID() );

        objPtr->updateValue( "name", it->second.getName() );
        objPtr->updateValue( "description", it->second.getDesc() );
        objPtr->updateValue( "type", it->second.getTypeAsStr() );
        objPtr->updateValue( "value", it->second.getValue() );
        objPtr->updateValue( "zoneid", it->second.getZoneID() );
    }

    return HNIS_RESULT_SUCCESS;
}


