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
HNIDActionRequest::setCriteriaID( std::string value )
{
    m_criteriaID = value;
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
HNIDActionRequest::getCriteriaID()
{
    return m_criteriaID;
}

bool
HNIDActionRequest::setZoneUpdate( std::istream& bodyStream )
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

        if( jsRoot->has( "cyclesPerDay" ) )
        {
            zone.setTargetCyclesPerDay( jsRoot->getValue<uint>( "cyclesPerDay" ) );
            m_zoneUpdateMask |= HNID_ZU_FLDMASK_CPD;
        }

        if( jsRoot->has( "secondsMinCycle" ) )
        {
            zone.setMinimumCycleTimeSeconds( jsRoot->getValue<uint>( "secondsMinCycle" ) );
            m_zoneUpdateMask |= HNID_ZU_FLDMASK_SMC;
        }

        if( jsRoot->has( "swidList" ) )
        {
            zone.setSWIDList( jsRoot->getValue<std::string>( "swidList" ) );
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

    if( m_zoneUpdateMask & HNID_ZU_FLDMASK_CPD )
        tgtZone->setTargetCyclesPerDay( srcZone->getTargetCyclesPerDay() );

    if( m_zoneUpdateMask & HNID_ZU_FLDMASK_SMC )
        tgtZone->setMinimumCycleTimeSeconds( srcZone->getMinimumCycleTimeSeconds() );

    if( m_zoneUpdateMask & HNID_ZU_FLDMASK_SWLST )
        tgtZone->setSWIDList( srcZone->getSWIDListStr() );

}


bool
HNIDActionRequest::setCriteriaUpdate( std::istream& bodyStream )
{
    std::string rstStr;
    HNScheduleCriteria criteria;

    // Clear the update mask
    m_criteriaUpdateMask = HNID_CU_FLDMASK_CLEAR;

    // Parse the json body of the request
    try
    {
        // Attempt to parse the json    
        pjs::Parser parser;
        pdy::Var varRoot = parser.parse( bodyStream );

        // Get a pointer to the root object
        pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

        //HNScheduleCriteria *event = m_schedule.updateCriteria( criteriaID );

        if( jsRoot->has( "name" ) )
        {
            criteria.setName( jsRoot->getValue<std::string>( "name" ) );
            m_criteriaUpdateMask |= HNID_CU_FLDMASK_NAME;
        }

        if( jsRoot->has( "description" ) )
        {
            criteria.setDesc( jsRoot->getValue<std::string>( "description" ) );
            m_criteriaUpdateMask |= HNID_CU_FLDMASK_DESC;
        }

        if( jsRoot->has( "startTime" ) )
        {
            criteria.setStartTime( jsRoot->getValue<std::string>( "startTime" ) );
            m_criteriaUpdateMask |= HNID_CU_FLDMASK_START;
        }

        if( jsRoot->has( "endTime" ) )
        {
            criteria.setEndTime( jsRoot->getValue<std::string>( "endTime" ) );
            m_criteriaUpdateMask |= HNID_CU_FLDMASK_END;
        }

/*
        if( jsRoot->has( "dayName" ) )
        {
            criteria.setDayIndexFromNameStr( jsRoot->getValue<std::string>( "dayName" ) );
            m_criteriaUpdateMask |= HNID_CU_FLDMASK_DAYNAME;
        }
*/
        
        if( criteria.validateSettings() != HNIS_RESULT_SUCCESS )
        {
            std::cout << "updateCriteria validate failed" << std::endl;
            // zoneid parameter is required
            //return HNID_RESULT_BAD_REQUEST;
            return true;
        }        
    }
    catch( Poco::Exception ex )
    {
        std::cout << "updateCriteria exception: " << ex.displayText() << std::endl;
        // Request body was not understood
        //return HNID_RESULT_BAD_REQUEST;
        return true;
    }

    // Add the zone info to the list
    m_criteriaList.push_back( criteria );

    return false;
}

void 
HNIDActionRequest::applyCriteriaUpdate( HNScheduleCriteria *tgtCriteria )
{
    HNScheduleCriteria *srcCriteria = &m_criteriaList[0];

    if( m_criteriaUpdateMask & HNID_CU_FLDMASK_NAME )
        tgtCriteria->setName( srcCriteria->getName() );

    if( m_criteriaUpdateMask & HNID_CU_FLDMASK_DESC )
        tgtCriteria->setDesc( srcCriteria->getDesc() );

    if( m_criteriaUpdateMask & HNID_CU_FLDMASK_START )
        tgtCriteria->setStartTime( srcCriteria->getStartTime().getHMSStr() );

    if( m_criteriaUpdateMask & HNID_CU_FLDMASK_END )
        tgtCriteria->setEndTime( srcCriteria->getEndTime().getHMSStr() );

    //if( m_criteriaUpdateMask & HNID_CU_FLDMASK_DAYNAME )
    //    tgtCriteria->setDayIndexFromNameStr( srcCriteria->getDayName() );
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
        case HNID_AR_TYPE_CRITLIST:
        case HNID_AR_TYPE_CRITINFO:
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

        case HNID_AR_TYPE_CRITCREATE:
            newID = getCriteriaID();
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

                znObj.set( "zoneid", zit->getID() );
                znObj.set( "name", zit->getName() );
                znObj.set( "description", zit->getDesc() );
                znObj.set( "secondsPerWeek", zit->getWeeklySeconds() );
                znObj.set( "cyclesPerDay", zit->getTargetCyclesPerDay() );
                znObj.set( "secondsMinCycle", zit->getMinimumCycleTimeSeconds() );
                znObj.set( "swidList", zit->getSWIDListStr() );

                jsRoot.add( znObj );
            }

            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_ZONEINFO:
        {
            // Create a json root object
            pjs::Object      jsRoot;

            std::vector< HNIrrigationZone >::iterator zone = refZoneList().begin();

            jsRoot.set( "zoneid", zone->getID() );
            jsRoot.set( "name", zone->getName() );
            jsRoot.set( "description", zone->getDesc() );
            jsRoot.set( "secondsPerWeek", zone->getWeeklySeconds() );
            jsRoot.set( "cyclesPerDay", zone->getTargetCyclesPerDay() );
            jsRoot.set( "secondsMinCycle", zone->getMinimumCycleTimeSeconds() );
            jsRoot.set( "swidList", zone->getSWIDListStr() );

            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_CRITLIST:
        {
            // Create a json root object
            pjs::Array jsRoot;

            for( std::vector< HNScheduleCriteria >::iterator cit = refCriteriaList().begin(); cit != refCriteriaList().end(); cit++ )
            { 
                pjs::Object cObj;

                cObj.set( "criteriaid", cit->getID() );
                cObj.set( "name", cit->getName() );
                cObj.set( "description", cit->getDesc() );
                cObj.set( "startTime", cit->getStartTime().getHMSStr() );
                cObj.set( "endTime", cit->getEndTime().getHMSStr() );
                //cObj.set( "dayName", cit->getDayName() );

                jsRoot.add( cObj );
            }

            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_CRITINFO:
        {
            // Create a json root object
            pjs::Object      jsRoot;

            std::vector< HNScheduleCriteria >::iterator criteria = refCriteriaList().begin();

            jsRoot.set( "criteriaid", criteria->getID() );
            jsRoot.set( "name", criteria->getName() );
            jsRoot.set( "description", criteria->getDesc() );
            //jsRoot.set( "type", criteria->getTypeStr() );
            jsRoot.set( "startTime", criteria->getStartTime().getHMSStr() );
            jsRoot.set( "endTime", criteria->getEndTime().getHMSStr() );
            //jsRoot.set( "dayName", criteria->getDayName() );

            try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return true; }
        }
        break;

        case HNID_AR_TYPE_SCHINFO:
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

std::vector< HNScheduleCriteria >&
HNIDActionRequest::refCriteriaList()
{
    return m_criteriaList;
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


