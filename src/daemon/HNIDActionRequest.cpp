#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/StreamCopier.h>

#include "HNIDActionRequest.h"

namespace pjs = Poco::JSON;
namespace pdy = Poco::Dynamic;

HNIDActionRequest::HNIDActionRequest()
{
    m_type = HNID_AR_TYPE_NOTSET;
    m_zoneUpdateMask = HNID_ZU_FLDMASK_CLEAR;
}

HNIDActionRequest::~HNIDActionRequest()
{

}

void
HNIDActionRequest::setType( HNID_AR_TYPE_T type )
{
    m_type = type;
}

void 
HNIDActionRequest::setZoneID( std::string value )
{
    m_zoneID = value;
}

void 
HNIDActionRequest::setPlacementID( std::string value )
{
    m_placementID = value;
}

void 
HNIDActionRequest::setModifierID( std::string value )
{
    m_modifierID = value;
}

void 
HNIDActionRequest::setSequenceID( std::string value )
{
    m_sequenceID = value;
}

void 
HNIDActionRequest::setInhibitID( std::string value )
{
    m_inhibitID = value;
}

void 
HNIDActionRequest::setOperationID( std::string value )
{
    m_operationID = value;
}

void 
HNIDActionRequest::setScheduleStateRequestType( HNID_SSR_T value )
{
    m_schReqType = value;
}

void 
HNIDActionRequest::setZoneControlRequestType( HNID_ZCR_T value )
{
    m_zoneReqType = value;
}

void 
HNIDActionRequest::setInhibitDuration( std::string value )
{
    m_inhibitDuration = value;
}

void 
HNIDActionRequest::setOnDuration( std::string value )
{
    m_onDuration = value;
}

void 
HNIDActionRequest::setOffDuration( std::string value )
{
   m_offDuration = value;
}

HNID_AR_TYPE_T
HNIDActionRequest::getType()
{
    return m_type;
}

std::string 
HNIDActionRequest::getZoneID()
{
    return m_zoneID;
}

std::string 
HNIDActionRequest::getPlacementID()
{
    return m_placementID;
}

std::string 
HNIDActionRequest::getModifierID()
{
    return m_modifierID;
}

std::string 
HNIDActionRequest::getSequenceID()
{
    return m_sequenceID;
}

std::string 
HNIDActionRequest::getInhibitID()
{
    return m_inhibitID;
}

std::string 
HNIDActionRequest::getOperationID()
{
    return m_operationID;
}

HNID_SSR_T 
HNIDActionRequest::getScheduleStateRequestType()
{
    return m_schReqType;
}

HNID_ZCR_T 
HNIDActionRequest::getZoneControlRequestType()
{
    return m_zoneReqType;
}

std::string
HNIDActionRequest::getInhibitDuration()
{
    return m_inhibitDuration;
}

std::string 
HNIDActionRequest::getOnDuration()
{
    return m_onDuration;
}

std::string 
HNIDActionRequest::getOffDuration()
{
   return m_offDuration;
}

bool
HNIDActionRequest::decodeZoneUpdate( std::istream& bodyStream )
{
    HNIrrigationZone zone;

    // Clear the update mask
    m_zoneUpdateMask = HNID_ZU_FLDMASK_CLEAR;

    // Parse the json body of the request
    try
    {
        // Attempt to parse the json    
        pjs::Parser parser;
        pdy::Var varRoot = parser.parse( bodyStream );

        // Get a pointer to the root object
        pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

        //HNIrrigationZone *zone = m_schedule.updateZone( zoneID );

        if( jsRoot->has( "name" ) )
        {
            zone.setName( jsRoot->getValue<std::string>( "name" ) );
            m_zoneUpdateMask |= HNID_ZU_FLDMASK_NAME;
        }

        if( jsRoot->has( "description" ) )
        {
            zone.setDesc( jsRoot->getValue<std::string>( "description" ) );
            m_zoneUpdateMask |= HNID_ZU_FLDMASK_DESC;
        }

        if( jsRoot->has( "secondsPerWeek" ) )
        {
            zone.setWeeklySeconds( jsRoot->getValue<uint>( "secondsPerWeek" ) );
            m_zoneUpdateMask |= HNID_ZU_FLDMASK_SPW;
        }

        if( jsRoot->has( "secondsMaxCycle" ) )
        {
            zone.setMaximumCycleTimeSeconds( jsRoot->getValue<uint>( "secondsMaxCycle" ) );
            m_zoneUpdateMask |= HNID_ZU_FLDMASK_MAXSPC;
        }

        if( jsRoot->has( "secondsMinCycle" ) )
        {
            zone.setMinimumCycleTimeSeconds( jsRoot->getValue<uint>( "secondsMinCycle" ) );
            m_zoneUpdateMask |= HNID_ZU_FLDMASK_MINSPC;
        }

        if( jsRoot->has( "swidList" ) )
        {
            pjs::Array::Ptr jsSWIDList = jsRoot->getArray( "swidList" );

            zone.clearSWIDSet();
            
            std::cout << "Zone Update - start" << std::endl;

            for( uint index = 0; index < jsSWIDList->size(); index++ )
            {
                std::string value = jsSWIDList->getElement<std::string>(index);
                std::cout << "Zone Update - value: " << value << std::endl;
                zone.addSWID( value );
            }
            
            m_zoneUpdateMask |= HNID_ZU_FLDMASK_SWLST;
        }
        
        if( zone.validateSettings() != HNIS_RESULT_SUCCESS )
        {
            std::cout << "updateZone validate failed" << std::endl;
            // zoneid parameter is required
            return true;
        }        
    }
    catch( Poco::Exception ex )
    {
        std::cout << "updateZone exception: " << ex.displayText() << std::endl;
        // Request body was not understood
        return true;
    }

    // Add the zone info to the list
    m_zoneList.push_back( zone );

    // Done
    return false;

}

void 
HNIDActionRequest::applyZoneUpdate( HNIrrigationZone *tgtZone )
{
    HNIrrigationZone *srcZone = &m_zoneList[0];

    if( m_zoneUpdateMask & HNID_ZU_FLDMASK_NAME )
        tgtZone->setName( srcZone->getName() );

    if( m_zoneUpdateMask & HNID_ZU_FLDMASK_DESC )
        tgtZone->setDesc( srcZone->getDesc() );

    if( m_zoneUpdateMask & HNID_ZU_FLDMASK_SPW )
        tgtZone->setWeeklySeconds( srcZone->getWeeklySeconds() );

    if( m_zoneUpdateMask & HNID_ZU_FLDMASK_MAXSPC )
        tgtZone->setMaximumCycleTimeSeconds( srcZone->getMaximumCycleTimeSeconds() );

    if( m_zoneUpdateMask & HNID_ZU_FLDMASK_MINSPC )
        tgtZone->setMinimumCycleTimeSeconds( srcZone->getMinimumCycleTimeSeconds() );

    if( m_zoneUpdateMask & HNID_ZU_FLDMASK_SWLST )
    {
        tgtZone->clearSWIDSet();
        tgtZone->addSWIDSet( srcZone->getSWIDSetRef() );
    }
}


bool
HNIDActionRequest::decodePlacementUpdate( std::istream& bodyStream )
{
    std::string rstStr;
    HNIrrigationPlacement placement;

    // Clear the update mask
    m_placementUpdateMask = HNID_PU_FLDMASK_CLEAR;

    // Parse the json body of the request
    try
    {
        // Attempt to parse the json    
        pjs::Parser parser;
        pdy::Var varRoot = parser.parse( bodyStream );

        // Get a pointer to the root object
        pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

        //HNIrrigationPlacement *event = m_schedule.updatePlacement( placementID );

        if( jsRoot->has( "name" ) )
        {
            placement.setName( jsRoot->getValue<std::string>( "name" ) );
            m_placementUpdateMask |= HNID_PU_FLDMASK_NAME;
        }

        if( jsRoot->has( "description" ) )
        {
            placement.setDesc( jsRoot->getValue<std::string>( "description" ) );
            m_placementUpdateMask |= HNID_PU_FLDMASK_DESC;
        }

        if( jsRoot->has( "startTime" ) )
        {
            placement.setStartTime( jsRoot->getValue<std::string>( "startTime" ) );
            m_placementUpdateMask |= HNID_PU_FLDMASK_START;
        }

        if( jsRoot->has( "endTime" ) )
        {
            placement.setEndTime( jsRoot->getValue<std::string>( "endTime" ) );
            m_placementUpdateMask |= HNID_PU_FLDMASK_END;
        }

        if( jsRoot->has( "rank" ) )
        {
            int value = jsRoot->getValue<int>( "rank" );
            placement.setRank( value );
            m_placementUpdateMask |= HNID_PU_FLDMASK_RANK;
        }

        if( jsRoot->has( "dayList" ) )
        {
            pjs::Array::Ptr jsDayList = jsRoot->getArray( "dayList" );

            placement.clearDayBits();
            
            for( uint index = 0; index < jsDayList->size(); index++ )
            {
                std::string value = jsDayList->getElement<std::string>(index);
                std::cout << "DayName: " << value << std::endl; 
                placement.addDayByName( value );
            }
            
            m_placementUpdateMask |= HNID_PU_FLDMASK_DAYBITS;
        }
     
        if( jsRoot->has( "zoneList" ) )
        {
            pjs::Array::Ptr jsZoneList = jsRoot->getArray( "zoneList" );

            placement.clearZones();
            
            for( uint index = 0; index < jsZoneList->size(); index++ )
            {
                std::string value = jsZoneList->getElement<std::string>(index);
                placement.addZone( value );
            }
            
            m_placementUpdateMask |= HNID_PU_FLDMASK_ZONELIST;
        }
   
        if( placement.validateSettings() != HNIS_RESULT_SUCCESS )
        {
            std::cout << "updatePlacement validate failed" << std::endl;
            // zoneid parameter is required
            //return HNID_RESULT_BAD_REQUEST;
            return true;
        }        
    }
    catch( Poco::Exception ex )
    {
        std::cout << "updatePlacement exception: " << ex.displayText() << std::endl;
        // Request body was not understood
        //return HNID_RESULT_BAD_REQUEST;
        return true;
    }

    // Add the zone info to the list
    m_placementsList.push_back( placement );

    return false;
}

void 
HNIDActionRequest::applyPlacementUpdate( HNIrrigationPlacement *tgtPlacement )
{
    HNIrrigationPlacement *srcPlacement = &m_placementsList[0];

    if( m_placementUpdateMask & HNID_PU_FLDMASK_NAME )
        tgtPlacement->setName( srcPlacement->getName() );

    if( m_placementUpdateMask & HNID_PU_FLDMASK_DESC )
        tgtPlacement->setDesc( srcPlacement->getDesc() );

    if( m_placementUpdateMask & HNID_PU_FLDMASK_START )
        tgtPlacement->setStartTime( srcPlacement->getStartTime().getHMSStr() );

    if( m_placementUpdateMask & HNID_PU_FLDMASK_END )
        tgtPlacement->setEndTime( srcPlacement->getEndTime().getHMSStr() );

    if( m_placementUpdateMask & HNID_PU_FLDMASK_RANK )
        tgtPlacement->setRank( srcPlacement->getRank() );

    if( m_placementUpdateMask & HNID_PU_FLDMASK_DAYBITS )
        tgtPlacement->setDayBits( srcPlacement->getDayBits() );

    if( m_placementUpdateMask & HNID_PU_FLDMASK_ZONELIST )
    {
        tgtPlacement->clearZones();
        tgtPlacement->addZoneSet( srcPlacement->getZoneSetRef() );
    }
}

bool
HNIDActionRequest::decodeModifierUpdate( std::istream& bodyStream )
{
    std::string rstStr;
    HNIrrigationModifier modifier;

    // Clear the update mask
    m_modifierUpdateMask = HNID_MU_FLDMASK_CLEAR;

    // Parse the json body of the request
    try
    {
        // Attempt to parse the json    
        pjs::Parser parser;
        pdy::Var varRoot = parser.parse( bodyStream );

        // Get a pointer to the root object
        pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

        //HNIrrigationModifier *event = m_schedule.updateModifier( modifierID );

        if( jsRoot->has( "name" ) )
        {
            modifier.setName( jsRoot->getValue<std::string>( "name" ) );
            m_modifierUpdateMask |= HNID_MU_FLDMASK_NAME;
        }

        if( jsRoot->has( "description" ) )
        {
            modifier.setDesc( jsRoot->getValue<std::string>( "description" ) );
            m_modifierUpdateMask |= HNID_MU_FLDMASK_DESC;
        }

        if( jsRoot->has( "type" ) )
        {
            modifier.setTypeFromStr( jsRoot->getValue<std::string>( "type" ) );
            m_modifierUpdateMask |= HNID_MU_FLDMASK_TYPE;
        }

        if( jsRoot->has( "value" ) )
        {
            modifier.setValue( jsRoot->getValue<std::string>( "value" ) );
            m_modifierUpdateMask |= HNID_MU_FLDMASK_VALUE;
        }

        if( jsRoot->has( "zoneid" ) )
        {
            modifier.setZoneID( jsRoot->getValue<std::string>( "zoneid" ) );
            m_modifierUpdateMask |= HNID_MU_FLDMASK_ZONEID;
        }
   
        if( modifier.validateSettings() != HNIS_RESULT_SUCCESS )
        {
            std::cout << "updateModifier validate failed" << std::endl;
            // zoneid parameter is required
            //return HNID_RESULT_BAD_REQUEST;
            return true;
        }        
    }
    catch( Poco::Exception ex )
    {
        std::cout << "updateModifier exception: " << ex.displayText() << std::endl;
        // Request body was not understood
        //return HNID_RESULT_BAD_REQUEST;
        return true;
    }

    // Add the zone info to the list
    m_modifiersList.push_back( modifier );

    return false;
}

void 
HNIDActionRequest::applyModifierUpdate( HNIrrigationModifier *tgtModifier )
{
    HNIrrigationModifier *srcModifier = &m_modifiersList[0];

    if( m_modifierUpdateMask & HNID_MU_FLDMASK_NAME )
        tgtModifier->setName( srcModifier->getName() );

    if( m_modifierUpdateMask & HNID_MU_FLDMASK_DESC )
        tgtModifier->setDesc( srcModifier->getDesc() );
        
    if( m_modifierUpdateMask & HNID_MU_FLDMASK_TYPE )
        tgtModifier->setType( srcModifier->getType() );
        
    if( m_modifierUpdateMask & HNID_MU_FLDMASK_VALUE )
        tgtModifier->setValue( srcModifier->getValue() );
        
    if( m_modifierUpdateMask & HNID_MU_FLDMASK_ZONEID )
        tgtModifier->setZoneID( srcModifier->getZoneID() );
        
}

bool
HNIDActionRequest::decodeSequenceUpdate( std::istream& bodyStream )
{
    std::string rstStr;
    HNIrrigationSequence sequence;

    // Clear the update mask
    m_sequenceUpdateMask = HNID_SQU_FLDMASK_CLEAR;

    // Parse the json body of the request
    try
    {
        // Attempt to parse the json    
        pjs::Parser parser;
        pdy::Var varRoot = parser.parse( bodyStream );

        // Get a pointer to the root object
        pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

        if( jsRoot->has( "name" ) )
        {
            sequence.setName( jsRoot->getValue<std::string>( "name" ) );
            m_sequenceUpdateMask |= HNID_SQU_FLDMASK_NAME;
        }

        if( jsRoot->has( "description" ) )
        {
            sequence.setDesc( jsRoot->getValue<std::string>( "description" ) );
            m_sequenceUpdateMask |= HNID_SQU_FLDMASK_DESC;
        }

        if( jsRoot->has( "type" ) )
        {
            sequence.setTypeFromStr( jsRoot->getValue<std::string>( "type" ) );
            m_sequenceUpdateMask |= HNID_SQU_FLDMASK_TYPE;
        }

        if( jsRoot->has( "onDuration" ) )
        {
            sequence.setOnDuration( strtol( jsRoot->getValue<std::string>( "onDuration" ).c_str(), NULL, 0 ) );
            m_sequenceUpdateMask |= HNID_SQU_FLDMASK_ONDUR;
        }

        if( jsRoot->has( "offDuration" ) )
        {
            sequence.setOffDuration( strtol( jsRoot->getValue<std::string>( "offDuration" ).c_str(), NULL, 0 ) );
            m_sequenceUpdateMask |= HNID_SQU_FLDMASK_OFFDUR;
        }

        if( jsRoot->has( "objList" ) )
        {
            pjs::Array::Ptr jsObjList = jsRoot->getArray( "objList" );

            sequence.clearObjList();
            
            for( uint index = 0; index < jsObjList->size(); index++ )
            {
                std::string value = jsObjList->getElement<std::string>(index);
                sequence.addObj( value );
            }
            
            m_sequenceUpdateMask |= HNID_SQU_FLDMASK_OBJLIST;
        }

        if( sequence.validateSettings() != HNIS_RESULT_SUCCESS )
        {
            std::cout << "updateSequence validate failed" << std::endl;
            // zoneid parameter is required
            //return HNID_RESULT_BAD_REQUEST;
            return true;
        }
    }
    catch( Poco::Exception ex )
    {
        std::cout << "updateSequence exception: " << ex.displayText() << std::endl;
        // Request body was not understood
        //return HNID_RESULT_BAD_REQUEST;
        return true;
    }

    // Add the zone info to the list
    m_sequencesList.push_back( sequence );

    return false;
}

void 
HNIDActionRequest::applySequenceUpdate( HNIrrigationSequence *tgtSequence )
{
    HNIrrigationSequence *srcSequence = &m_sequencesList[0];

    if( m_sequenceUpdateMask & HNID_SQU_FLDMASK_NAME )
        tgtSequence->setName( srcSequence->getName() );

    if( m_sequenceUpdateMask & HNID_SQU_FLDMASK_DESC )
        tgtSequence->setDesc( srcSequence->getDesc() );
        
    if( m_sequenceUpdateMask & HNID_SQU_FLDMASK_TYPE )
        tgtSequence->setType( srcSequence->getType() );
        
    if( m_sequenceUpdateMask & HNID_SQU_FLDMASK_ONDUR )
        tgtSequence->setOnDuration( srcSequence->getOnDuration() );
        
    if( m_sequenceUpdateMask & HNID_SQU_FLDMASK_OFFDUR )
        tgtSequence->setOffDuration( srcSequence->getOffDuration() );

    if( m_sequenceUpdateMask & HNID_SQU_FLDMASK_OBJLIST )
    {
        tgtSequence->setObjListFromStr( srcSequence->getObjListAsStr() );
    }
}

bool
HNIDActionRequest::decodeInhibitUpdate( std::istream& bodyStream )
{
    std::string rstStr;
    HNIrrigationInhibit inhibit;

    // Clear the update mask
    m_inhibitUpdateMask = HNID_INU_FLDMASK_CLEAR;

    // Parse the json body of the request
    try
    {
        // Attempt to parse the json    
        pjs::Parser parser;
        pdy::Var varRoot = parser.parse( bodyStream );

        // Get a pointer to the root object
        pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

        //HNIrrigationModifier *event = m_schedule.updateModifier( modifierID );

        if( jsRoot->has( "name" ) )
        {
            inhibit.setName( jsRoot->getValue<std::string>( "name" ) );
            m_inhibitUpdateMask |= HNID_INU_FLDMASK_NAME;
        }

        if( jsRoot->has( "description" ) )
        {
            inhibit.setDesc( jsRoot->getValue<std::string>( "description" ) );
            m_inhibitUpdateMask |= HNID_INU_FLDMASK_DESC;
        }
/*
        if( jsRoot->has( "type" ) )
        {
            modifier.setTypeFromStr( jsRoot->getValue<std::string>( "type" ) );
            m_modifierUpdateMask |= HNID_MU_FLDMASK_TYPE;
        }

        if( jsRoot->has( "value" ) )
        {
            modifier.setValue( jsRoot->getValue<std::string>( "value" ) );
            m_modifierUpdateMask |= HNID_MU_FLDMASK_VALUE;
        }

        if( jsRoot->has( "zoneid" ) )
        {
            modifier.setZoneID( jsRoot->getValue<std::string>( "zoneid" ) );
            m_modifierUpdateMask |= HNID_MU_FLDMASK_ZONEID;
        }
   
        if( modifier.validateSettings() != HNIS_RESULT_SUCCESS )
        {
            std::cout << "updateModifier validate failed" << std::endl;
            // zoneid parameter is required
            //return HNID_RESULT_BAD_REQUEST;
            return true;
        }
*/   
    }
    catch( Poco::Exception ex )
    {
        std::cout << "updateInhibit exception: " << ex.displayText() << std::endl;
        // Request body was not understood
        //return HNID_RESULT_BAD_REQUEST;
        return true;
    }

    // Add the zone info to the list
    m_inhibitsList.push_back( inhibit );

    return false;
}

void 
HNIDActionRequest::applyInhibitUpdate( HNIrrigationInhibit *tgtInhibit )
{
    HNIrrigationInhibit *srcInhibit = &m_inhibitsList[0];

    if( m_inhibitUpdateMask & HNID_INU_FLDMASK_NAME )
        tgtInhibit->setName( srcInhibit->getName() );

    if( m_inhibitUpdateMask & HNID_INU_FLDMASK_DESC )
        tgtInhibit->setDesc( srcInhibit->getDesc() );
/*        
    if( m_inhibitUpdateMask & HNID_INU_FLDMASK_TYPE )
        tgtInhibit->setType( srcInhibit->getType() );
        
    if( m_inhibitUpdateMask & HNID_INU_FLDMASK_VALUE )
        tgtInhibit->setValue( srcInhibit->getValue() );
        
    if( m_inhibitUpdateMask & HNID_INU_FLDMASK_ZONEID )
        tgtInhibit->setZoneID( srcInhibit->getZoneID() );
*/        
}

bool
HNIDActionRequest::decodeOperationUpdate( std::istream& bodyStream )
{
    std::string rstStr;
    HNIrrigationOperation operation;

    // Clear the update mask
    m_operationUpdateMask = HNID_OPU_FLDMASK_CLEAR;

    // Parse the json body of the request
    try
    {
        // Attempt to parse the json    
        pjs::Parser parser;
        pdy::Var varRoot = parser.parse( bodyStream );

        // Get a pointer to the root object
        pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

        if( jsRoot->has( "type" ) )
        {
            operation.setTypeFromStr( jsRoot->getValue<std::string>( "type" ) );
            m_operationUpdateMask |= HNID_OPU_FLDMASK_TYPE;
        }

        if( jsRoot->has( "objList" ) )
        {
            pjs::Array::Ptr jsObjList = jsRoot->getArray( "objList" );

            operation.clearObjIDList();
            
            for( uint index = 0; index < jsObjList->size(); index++ )
            {
                std::string value = jsObjList->getElement<std::string>(index);
                operation.addObjID( value );
            }
            
            m_operationUpdateMask |= HNID_OPU_FLDMASK_OBJLIST;
        }
   
        if( operation.validateSettings() != HNIS_RESULT_SUCCESS )
        {
            std::cout << "updateOperation validate failed" << std::endl;
            // zoneid parameter is required
            //return HNID_RESULT_BAD_REQUEST;
            return true;
        }
    }
    catch( Poco::Exception ex )
    {
        std::cout << "updateOperation exception: " << ex.displayText() << std::endl;
        // Request body was not understood
        //return HNID_RESULT_BAD_REQUEST;
        return true;
    }

    // Add the zone info to the list
    m_operationsList.push_back( operation );

    return false;
}

void 
HNIDActionRequest::applyOperationUpdate( HNIrrigationOperation *tgtOperation )
{
    HNIrrigationOperation *srcOperation = &m_operationsList[0];

    if( m_operationUpdateMask & HNID_OPU_FLDMASK_TYPE )
        tgtOperation->setType( srcOperation->getType() );

    if( m_operationUpdateMask & HNID_OPU_FLDMASK_OBJLIST )
    {
        tgtOperation->clearObjIDList();
        tgtOperation->addObjIDList( srcOperation->getObjIDListRef() );
    }
}

bool
HNIDActionRequest::decodeSchedulerState( std::istream& bodyStream )
{
    // Parse the json body of the request
    try
    {
        // Attempt to parse the json    
        pjs::Parser parser;
        pdy::Var varRoot = parser.parse( bodyStream );

        // Get a pointer to the root object
        pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

        if( jsRoot->has( "state" ) )
        {
            std::string newState = jsRoot->getValue<std::string>( "state" );
            if( "enable" == newState )
            {
                setScheduleStateRequestType( HNID_SSR_ENABLE );
            }
            else if( "disable" == newState )
            {
                setScheduleStateRequestType( HNID_SSR_DISABLE );
            }
            else if( "inhibit" == newState )
            {
                if( jsRoot->has( "inhibitDuration" ) == false )
                {
                    std::cout << "ERROR: schedule control request requires inhibit duration." << std::endl;
                    return true;
                }

                setScheduleStateRequestType( HNID_SSR_INHIBIT );
                setInhibitDuration( jsRoot->getValue<std::string>( "inhibitDuration" ) );
            }
            else
            {
                std::cout << "ERROR: Unsupported schedule control request" << std::endl;
                return true;
            }
        }

    }
    catch( Poco::Exception ex )
    {
        std::cout << "ERROR: decodeSchedulerState exception: " << ex.displayText() << std::endl;
        // Request body was not understood
        return true;
    }

    // Done
    return false;
}

bool
HNIDActionRequest::decodeZoneCtrl( std::istream& bodyStream )
{
    // Parse the json body of the request
    try
    {
        // Attempt to parse the json    
        pjs::Parser parser;
        pdy::Var varRoot = parser.parse( bodyStream );

        // Get a pointer to the root object
        pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

#if 0
        if( jsRoot->has( "state" ) )
        {
            std::string newState = jsRoot->getValue<std::string>( "state" );
            if( "enable" == newState )
            {
                setScheduleStateRequestType( HNID_SSR_ENABLE );
            }
            else if( "disable" == newState )
            {
                setScheduleStateRequestType( HNID_SSR_DISABLE );
            }
            else if( "inhibit" == newState )
            {
                if( jsRoot->has( "inhibitDuration" ) == false )
                {
                    std::cout << "ERROR: schedule control request requires inhibit duration." << std::endl;
                    return true;
                }

                setScheduleStateRequestType( HNID_SSR_INHIBIT );
                setInhibitDuration( jsRoot->getValue<std::string>( "inhibitDuration" ) );
            }
            else
            {
#endif
                std::cout << "ERROR: Unsupported schedule control request" << std::endl;
                return true;
#if 0
            }
        }
#endif
    }
    catch( Poco::Exception ex )
    {
        std::cout << "ERROR: decodeSchedulerState exception: " << ex.displayText() << std::endl;
        // Request body was not understood
        return true;
    }

    // Done
    return false;
}

bool 
HNIDActionRequest::hasRspContent( std::string &contentType )
{
    contentType.clear();

    switch( m_type )
    {
        case HNID_AR_TYPE_SWLIST:  
        case HNID_AR_TYPE_ZONELIST:
        case HNID_AR_TYPE_ZONEINFO:
        case HNID_AR_TYPE_SCHINFO: 
        case HNID_AR_TYPE_PLACELIST:
        case HNID_AR_TYPE_PLACEINFO:
        case HNID_AR_TYPE_IRRSTATUS:
        case HNID_AR_TYPE_MODIFIERSLIST:
        case HNID_AR_TYPE_MODIFIERINFO:
        case HNID_AR_TYPE_SEQUENCESLIST:
        case HNID_AR_TYPE_SEQUENCEINFO:
        case HNID_AR_TYPE_INHIBITSLIST:
        case HNID_AR_TYPE_INHIBITINFO:
        case HNID_AR_TYPE_OPERATIONSLIST:
        case HNID_AR_TYPE_OPERATIONINFO:
            contentType = "application/json";
            return true;

        default:
        break;
    }

    return false;
}

bool 
HNIDActionRequest::hasNewObject( std::string &newID )
{
    newID.clear();

    switch( m_type )
    {
        case HNID_AR_TYPE_ZONECREATE:
            newID = getZoneID();
            return true;

        case HNID_AR_TYPE_PLACECREATE:
            newID = getPlacementID();
            return true;

        case HNID_AR_TYPE_MODIFIERCREATE:
            newID = getModifierID();
            return true;

        case HNID_AR_TYPE_SEQUENCECREATE:
            newID = getSequenceID();
            return true;

        case HNID_AR_TYPE_INHIBITCREATE:
            newID = getInhibitID();
            return true;

        case HNID_AR_TYPE_OPERATIONCREATE:
            newID = getOperationID();
            return true;

        default:
        break;
    }

    return false;
}

bool 
HNIDActionRequest::generateRspContent( std::ostream &ostr )
{
    switch( m_type )
    {
        case HNID_AR_TYPE_SWLIST:
        {
            // Create a json root object
            pjs::Array jsRoot;

            for( std::vector< HNSWDSwitchInfo >::iterator sit = refSwitchList().begin(); sit != refSwitchList().end(); sit++ )
            { 
                pjs::Object swObj;

                swObj.set( "swid", sit->getID() );
                swObj.set( "description", sit->getDesc() );
 
                jsRoot.add( swObj );
            }

            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_ZONELIST:
        {
            // Create a json root object
            pjs::Array jsRoot;

            for( std::vector< HNIrrigationZone >::iterator zit = refZoneList().begin(); zit != refZoneList().end(); zit++ )
            { 
                pjs::Object znObj;
                pjs::Array  jsSwitchList;

                znObj.set( "zoneid", zit->getID() );
                znObj.set( "name", zit->getName() );
                znObj.set( "description", zit->getDesc() );
                znObj.set( "secondsPerWeek", zit->getWeeklySeconds() );
                znObj.set( "secondsMaxCycle", zit->getMaximumCycleTimeSeconds() );
                znObj.set( "secondsMinCycle", zit->getMinimumCycleTimeSeconds() );

                // Compose Switch List
                for( std::set< std::string >::iterator sit = zit->getSWIDSetRef().begin(); sit != zit->getSWIDSetRef().end(); sit++ )
                {
                    jsSwitchList.add( *sit );
                }

                // Add Switch List field
                znObj.set( "swidList", jsSwitchList );

                jsRoot.add( znObj );
            }

            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_ZONEINFO:
        {
            // Create a json root object
            pjs::Object  jsRoot;
            pjs::Array   jsSwitchList;
   
            std::vector< HNIrrigationZone >::iterator zone = refZoneList().begin();

            jsRoot.set( "zoneid", zone->getID() );
            jsRoot.set( "name", zone->getName() );
            jsRoot.set( "description", zone->getDesc() );
            jsRoot.set( "secondsPerWeek", zone->getWeeklySeconds() );
            jsRoot.set( "secondsMaxCycle", zone->getMaximumCycleTimeSeconds() );
            jsRoot.set( "secondsMinCycle", zone->getMinimumCycleTimeSeconds() );

            // Compose Switch List
            for( std::set< std::string >::iterator sit = zone->getSWIDSetRef().begin(); sit != zone->getSWIDSetRef().end(); sit++ )
            {
                jsSwitchList.add( *sit );
            }

            // Add Switch List field
            jsRoot.set( "swidList", jsSwitchList );

            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_PLACELIST:
        {
            // Create a json root object
            pjs::Array jsRoot;

            for( std::vector< HNIrrigationPlacement >::iterator cit = refPlacementsList().begin(); cit != refPlacementsList().end(); cit++ )
            { 
                pjs::Object cObj;
                pjs::Array dayList;
                pjs::Array zoneList;

                cObj.set( "placementid", cit->getID() );
                cObj.set( "name", cit->getName() );
                cObj.set( "description", cit->getDesc() );
                cObj.set( "startTime", cit->getStartTime().getHMSStr() );
                cObj.set( "endTime", cit->getEndTime().getHMSStr() );
                cObj.set( "rank", cit->getRank() );

                // Compose Day List, Empty equals everyday
                uint dayBits = cit->getDayBits();
 
                if( dayBits & HNSC_DBITS_SUNDAY )
                    dayList.add( "Sunday" );
                if( dayBits & HNSC_DBITS_MONDAY )
                    dayList.add( "Monday" );
                if( dayBits & HNSC_DBITS_TUESDAY )
                    dayList.add( "Tuesday" );
                if( dayBits & HNSC_DBITS_WEDNESDAY )
                    dayList.add( "Wednesday" );
                if( dayBits & HNSC_DBITS_THURSDAY )
                    dayList.add( "Thursday" );
                if( dayBits & HNSC_DBITS_FRIDAY )
                    dayList.add( "Friday" );
                if( dayBits & HNSC_DBITS_SATURDAY )
                    dayList.add( "Saturday" );

                // Add Daylist field
                cObj.set( "dayList", dayList );

                // Compose Zone List
                for( std::set< std::string >::iterator zit = cit->getZoneSetRef().begin(); zit != cit->getZoneSetRef().end(); zit++ )
                {
                    zoneList.add( *zit );
                }

                // Add Zonelist field
                cObj.set( "zoneList", zoneList );
                
                // Add new placement object to return list
                jsRoot.add( cObj );
            }

            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_PLACEINFO:
        {
            // Create a json root object
            pjs::Object      jsRoot;
            pjs::Array dayList;
            pjs::Array zoneList;

            std::vector< HNIrrigationPlacement >::iterator placement = refPlacementsList().begin();

            jsRoot.set( "placementid", placement->getID() );
            jsRoot.set( "name", placement->getName() );
            jsRoot.set( "description", placement->getDesc() );
            jsRoot.set( "startTime", placement->getStartTime().getHMSStr() );
            jsRoot.set( "endTime", placement->getEndTime().getHMSStr() );
            jsRoot.set( "rank", placement->getRank() );

            // Compose Day List, Empty equals everyday
            uint dayBits = placement->getDayBits();
 
            if( dayBits & HNSC_DBITS_SUNDAY )
                dayList.add( "Sunday" );
            if( dayBits & HNSC_DBITS_MONDAY )
                dayList.add( "Monday" );
            if( dayBits & HNSC_DBITS_TUESDAY )
                dayList.add( "Tuesday" );
            if( dayBits & HNSC_DBITS_WEDNESDAY )
                dayList.add( "Wednesday" );
            if( dayBits & HNSC_DBITS_THURSDAY )
                dayList.add( "Thursday" );
            if( dayBits & HNSC_DBITS_FRIDAY )
                dayList.add( "Friday" );
            if( dayBits & HNSC_DBITS_SATURDAY )
                dayList.add( "Saturday" );

            // Add Daylist field
            jsRoot.set( "dayList", dayList );

            // Compose Zone List
            for( std::set< std::string >::iterator zit = placement->getZoneSetRef().begin(); zit != placement->getZoneSetRef().end(); zit++ )
            {
                zoneList.add( *zit );
            }

            // Add Zonelist field
            jsRoot.set( "zoneList", zoneList );

            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_MODIFIERSLIST:
        {
            // Create a json root object
            pjs::Array jsRoot;

            for( std::vector< HNIrrigationModifier >::iterator mit = refModifiersList().begin(); mit != refModifiersList().end(); mit++ )
            { 
                pjs::Object mObj;

                mObj.set( "modifierid", mit->getID() );
                mObj.set( "name", mit->getName() );
                mObj.set( "description", mit->getDesc() );
                mObj.set( "type", mit->getTypeAsStr() );
                mObj.set( "value", mit->getValue() );
                mObj.set( "zoneid", mit->getZoneID() );

                // Add new placement object to return list
                jsRoot.add( mObj );
            }

            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_MODIFIERINFO:
        {
            // Create a json root object
            pjs::Object      jsRoot;

            std::vector< HNIrrigationModifier >::iterator modifier = refModifiersList().begin();

            jsRoot.set( "modifierid", modifier->getID() );
            jsRoot.set( "name", modifier->getName() );
            jsRoot.set( "description", modifier->getDesc() );
            jsRoot.set( "type", modifier->getTypeAsStr() );
            jsRoot.set( "value", modifier->getValue() );
            jsRoot.set( "zoneid", modifier->getZoneID() );

            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_SEQUENCESLIST:
        {
            // Create a json root object
            pjs::Array jsRoot;

            for( std::vector< HNIrrigationSequence >::iterator sit = refSequencesList().begin(); sit != refSequencesList().end(); sit++ )
            { 
                pjs::Object sObj;
                pjs::Array objIDList;

                sObj.set( "sequenceid", sit->getID() );
                sObj.set( "name", sit->getName() );
                sObj.set( "description", sit->getDesc() );
                sObj.set( "type", sit->getTypeAsStr() );
                sObj.set( "onDuration", sit->getOnDuration() );
                sObj.set( "offDuration", sit->getOffDuration() );
                
                // Compose Object ID List
                for( std::list< std::string >::iterator oit = sit->getObjListRef().begin(); oit != sit->getObjListRef().end(); oit++ )
                {
                    objIDList.add( *oit );
                }

                // Add Zonelist field
                sObj.set( "objIDList", objIDList );

                // Add new placement object to return list
                jsRoot.add( sObj );
            }

            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_SEQUENCEINFO:
        {
            // Create a json root object
            pjs::Object jsRoot;
            pjs::Array  objIDList;

            std::vector< HNIrrigationSequence >::iterator sequence = refSequencesList().begin();

            jsRoot.set( "sequenceid", sequence->getID() );
            jsRoot.set( "name", sequence->getName() );
            jsRoot.set( "description", sequence->getDesc() );

            jsRoot.set( "type", sequence->getTypeAsStr() );
            jsRoot.set( "onDuration", sequence->getOnDuration() );
            jsRoot.set( "offDuration", sequence->getOffDuration() );
                
            // Compose Object ID List
            for( std::list< std::string >::iterator oit = sequence->getObjListRef().begin(); oit != sequence->getObjListRef().end(); oit++ )
            {
                objIDList.add( *oit );
            }

            // Add Zonelist field
            jsRoot.set( "objIDList", objIDList );

            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_INHIBITSLIST:
        {
            // Create a json root object
            pjs::Array jsRoot;

            for( std::vector< HNIrrigationInhibit >::iterator iit = refInhibitsList().begin(); iit != refInhibitsList().end(); iit++ )
            { 
                pjs::Object iObj;

                iObj.set( "inhibitid", iit->getID() );
                iObj.set( "name", iit->getName() );
                iObj.set( "description", iit->getDesc() );

                // Add new placement object to return list
                jsRoot.add( iObj );
            }

            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_INHIBITINFO:
        {
            // Create a json root object
            pjs::Object      jsRoot;

            std::vector< HNIrrigationInhibit >::iterator inhibit = refInhibitsList().begin();

            jsRoot.set( "inhibitid", inhibit->getID() );
            jsRoot.set( "name", inhibit->getName() );
            jsRoot.set( "description", inhibit->getDesc() );

            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_OPERATIONSLIST:
        {
            // Create a json root object
            pjs::Array jsRoot;

            for( std::vector< HNIrrigationOperation >::iterator oit = refOperationsList().begin(); oit != refOperationsList().end(); oit++ )
            { 
                pjs::Object opObj;

                opObj.set( "operationid", oit->getID() );
                opObj.set( "type", oit->getTypeAsStr() );

                // Add new placement object to return list
                jsRoot.add( opObj );
            }

            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_OPERATIONINFO:
        {
            // Create a json root object
            pjs::Object      jsRoot;

            std::vector< HNIrrigationOperation >::iterator operation = refOperationsList().begin();

            jsRoot.set( "operationid", operation->getID() );
            jsRoot.set( "type", operation->getTypeAsStr() );


            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_SCHINFO:
        case HNID_AR_TYPE_IRRSTATUS:
            Poco::StreamCopier::copyStream( refRspStream(), ostr );
        break;
    }

    // Success
    return false;
}

std::vector< HNIrrigationZone >&
HNIDActionRequest::refZoneList()
{
    return m_zoneList;
}

std::vector< HNIrrigationPlacement >&
HNIDActionRequest::refPlacementsList()
{
    return m_placementsList;
}

std::vector< HNIrrigationModifier >&
HNIDActionRequest::refModifiersList()
{
    return m_modifiersList;
}

std::vector< HNIrrigationSequence >&
HNIDActionRequest::refSequencesList()
{
    return m_sequencesList;
}

std::vector< HNIrrigationInhibit >&
HNIDActionRequest::refInhibitsList()
{
    return m_inhibitsList;
}

std::vector< HNIrrigationOperation >&
HNIDActionRequest::refOperationsList()
{
    return m_operationsList;
}

std::vector< HNSWDSwitchInfo >&
HNIDActionRequest::refSwitchList()
{
    return m_swList;
}

std::stringstream&
HNIDActionRequest::refRspStream()
{
    return m_rspStream;
}


