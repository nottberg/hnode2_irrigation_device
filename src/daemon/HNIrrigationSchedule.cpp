#include <iostream>
#include <sstream>
#include <regex>

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>
#include <Poco/Checksum.h>

#include "HNIrrigationSchedule.h"

namespace pjs = Poco::JSON;
namespace pdy = Poco::Dynamic;

// Must match the day index enumeration from 
// the header file.
static const char *s_dayNames[] =
{
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday",
    "Not Set"
};

HNISPeriod::HNISPeriod()
{
    m_type = HNIS_PERIOD_TYPE_NOTSET;
    m_rank = 0;
}

HNISPeriod::~HNISPeriod()
{

}

void 
HNISPeriod::setID( std::string id )
{
    m_id = id;
}

void 
HNISPeriod::setType( HNIS_PERIOD_TYPE_T value )
{
    m_type = value;
}

void
HNISPeriod::setStartTime( HNI24HTime &time )
{
    m_startTime.setFromSeconds( time.getSeconds() );
}

void
HNISPeriod::setStartTimeSeconds( uint seconds )
{
    m_startTime.setFromSeconds( seconds );
}

void
HNISPeriod::setEndTime( HNI24HTime &time )
{
    m_endTime.setFromSeconds( time.getSeconds() );
}

void
HNISPeriod::setEndTimeSeconds( uint seconds )
{
    m_endTime.setFromSeconds( seconds );
}

void
HNISPeriod::setTimesFromStr( std::string startTime, std::string endTime )
{
    m_startTime.parseTime( startTime );
    m_endTime.parseTime( endTime );
}

void 
HNISPeriod::setRank( uint value )
{
    m_rank = value;
}

std::string
HNISPeriod::getID()
{
    return m_id;
}

HNIS_PERIOD_TYPE_T 
HNISPeriod::getType()
{
    return m_type;
}

void 
HNISPeriod::setDayIndex( HNIS_DAY_INDX_T dindx )
{
    m_dayIndx = dindx;
}

bool 
HNISPeriod::hasZone( std::string zoneID )
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
HNISPeriod::clearZones()
{
    m_zoneSet.clear();
}

void 
HNISPeriod::addZone( std::string name )
{
    m_zoneSet.insert( name );
}

void 
HNISPeriod::addZoneSet( std::set<std::string> &srcSet )
{
    for( std::set<std::string>::iterator it = srcSet.begin(); it != srcSet.end(); it++ )
    {
        m_zoneSet.insert( *it );
    }
}

std::set< std::string >& 
HNISPeriod::getZoneSetRef()
{
    return m_zoneSet;
}

std::string 
HNISPeriod::getZoneSetAsStr()
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

HNIS_DAY_INDX_T 
HNISPeriod::getDayIndex()
{
    return m_dayIndx;
}

HNI24HTime& 
HNISPeriod::getStartTime()
{
    return m_startTime;
}

uint
HNISPeriod::getStartTimeSeconds()
{
    return m_startTime.getSeconds();
}

std::string 
HNISPeriod::getStartTimeStr()
{
    return m_startTime.getHMSStr();
}

HNI24HTime& 
HNISPeriod::getEndTime()
{
    return m_endTime;
}

uint
HNISPeriod::getEndTimeSeconds()
{
    return m_endTime.getSeconds();
}

std::string 
HNISPeriod::getEndTimeStr()
{
    return m_endTime.getHMSStr();
}

uint 
HNISPeriod::getRank()
{
    return m_rank;
}

void 
HNISPeriod::moveStartToSecond( uint seconds )
{
    std::cout << "moveStartToSeconds - " << seconds << "  " << getStartTimeSeconds() << std::endl;

    if( seconds <= getStartTimeSeconds() )
        return;

    uint duration = ( seconds - getStartTimeSeconds() );

    std::cout << "moveStartToSeconds - duration: " << duration << std::endl;

    m_startTime.addSeconds( duration );
    m_endTime.addSeconds( duration );

    std::cout << "moveStartToSeconds - fin: " << getStartTimeSeconds() << "  " << getEndTimeSeconds() << std::endl;
}

void 
HNISPeriod::moveEndToSecond( uint seconds )
{
    std::cout << "moveEndToSeconds - " << seconds << "  " << getEndTimeSeconds() << std::endl;

    if( seconds >= getEndTimeSeconds() )
        return;

    uint duration = ( getEndTimeSeconds() - seconds );

    std::cout << "moveEndToSeconds - duration: " << duration << std::endl;

    m_startTime.subtractSeconds( duration );
    m_endTime.subtractSeconds( duration );

    std::cout << "moveEndToSeconds - fin: " << getStartTimeSeconds() << "  " << getEndTimeSeconds() << std::endl;
}

bool 
HNISPeriod::rankCompare( const HNISPeriod& first, const HNISPeriod& second )
{
  // First sort by rank
  if( first.m_rank < second.m_rank )
      return true;
  else if( first.m_rank < second.m_rank )
      return false;

  // If ranks are equal then sort by time
  return ( first.m_startTime.getSeconds() < second.m_startTime.getSeconds() );
}

HNISDay::HNISDay()
{
    m_dayIndex = HNIS_DINDX_NOTSET;
}

HNISDay::~HNISDay()
{

}

void 
HNISDay::clear()
{
    m_periodList.clear();
}

void
HNISDay::setIndex( HNIS_DAY_INDX_T dayIndex )
{
    if( dayIndex > HNIS_DINDX_NOTSET )
        m_dayIndex = HNIS_DINDX_NOTSET;
    else
        m_dayIndex = dayIndex;
}

void 
HNISDay::coalesce()
{
   // Cycle through all periods,
   // Combine exclusions that overlapp

}


std::string 
HNISDay::addAvailablePeriod( uint startSec, uint endSec, uint rank, std::set< std::string > &zoneSet )
{
    HNISPeriod period;

    char newID[64];
    sprintf( newID, "%ld", m_periodList.size() );

    period.setID( newID );
    period.setType( HNIS_PERIOD_TYPE_AVAILABLE );
    period.setDayIndex( m_dayIndex );
    period.setStartTimeSeconds( startSec );
    period.setEndTimeSeconds( endSec );
    period.setRank( rank );

    period.clearZones();
    period.addZoneSet( zoneSet );

    m_periodList.push_back( period );

    return newID;
}

std::string
HNISDay::insertBeforeAvailablePeriod( std::list< HNISPeriod >::iterator &it, uint startSec, uint endSec, uint rank, std::set< std::string > &zoneSet )
{
    HNISPeriod period;

    char newID[64];
    sprintf( newID, "%ld", m_periodList.size() );

    period.setID( newID );
    period.setType( HNIS_PERIOD_TYPE_AVAILABLE );
    period.setDayIndex( m_dayIndex );
    period.setStartTimeSeconds( startSec );
    period.setEndTimeSeconds( endSec );
    period.setRank( rank );

    period.clearZones();
    period.addZoneSet( zoneSet ); 

    m_periodList.insert( it, period );

    return newID;
}

std::string
HNISDay::insertAfterAvailablePeriod( std::list< HNISPeriod >::iterator &it, uint startSec, uint endSec, uint rank, std::set< std::string > &zoneSet )
{
    HNISPeriod period;

    char newID[64];
    sprintf( newID, "%ld", m_periodList.size() );

    // Add a period to represent the overlap region
    period.setID( newID );
    period.setType( HNIS_PERIOD_TYPE_AVAILABLE );
    period.setDayIndex( m_dayIndex );
    period.setStartTimeSeconds( startSec );
    period.setEndTimeSeconds( endSec );
    period.setRank( rank );

    period.clearZones();
    period.addZoneSet( zoneSet );

    // Add the new period after the current one.
    std::list< HNISPeriod >::iterator iit = it;
    iit++;
    if( iit != m_periodList.end() )
        m_periodList.insert( iit, period );
    else
        m_periodList.push_back( period );

    return newID;
}

void
HNISDay::applyZoneSet( std::string periodID, std::set< std::string > &zoneSet )
{
    for( std::list< HNISPeriod >::iterator it = m_periodList.begin(); it != m_periodList.end(); it++ )
    {
        if( it->getID() == periodID )
        {
            it->addZoneSet( zoneSet );
            return;
        }
    }     
}

OVLP_TYPE_T 
HNISDay::compareOverlap( uint cs, uint ce, HNISPeriod &period )
{       
    // Day matches so check times
    uint overlapType = 0;

    uint ps = period.getStartTime().getSeconds();
    uint pe = period.getEndTime().getSeconds();

    std::cout << "ovcmp - cs: " << cs << "  ce: " << ce << "  ps:" << ps << "  pe:" << pe << std::endl;

    if( cs < ps )
        overlapType |= 1;
    if( ce <= ps )
        overlapType |= 2;
    if( cs <= pe )
        overlapType |= 4;
    if( ce <= pe )
        overlapType |= 8;
             
    return (OVLP_TYPE_T) overlapType;
}

HNIS_RESULT_T 
HNISDay::applyCriteria( HNIrrigationCriteria &criteria )
{
    HNISPeriod period;

    std::cout << std::endl << "applyCriteria: " << criteria.getID() << "  dayIndex: " << m_dayIndex << std::endl;

    uint spanStart = criteria.getStartTime().getSeconds();
    uint spanEnd   = criteria.getEndTime().getSeconds();

    // Find the insertion point
    // The list should be in order, so scan until we are overlapping an entry or beyond possible entries. 
    uint index = 0;
    for( std::list< HNISPeriod >::iterator pit = m_periodList.begin(); pit != m_periodList.end(); pit++ )
    {
        // Determine the overlap type between the regions
        OVLP_TYPE_T overlapType = compareOverlap( spanStart, spanEnd, *pit );
             
        std::cout << "applyCriteria - iter: " << index << "  sov: " << overlapType << "  segment: " << criteria.getID() << std::endl;

        // Take action based on the type of overlap
        switch( overlapType )
        {
            // The criteria is completely after the current period,
            // so keep scanning to check for overlap with later periods.
            // If we reach the end then this criteria can be inserted there.
            case OVLP_TYPE_CRIT_AFTER:
                continue;

            // The criteria is completely before the current period,
            // since the list should be in order, we are finished with
            // this criteria.
            case OVLP_TYPE_CRIT_BEFORE:

                // If there is still part of this criteria left to insert
                // then add it now.
                if( spanStart != spanEnd )
                {
                    std::cout << "applyCriteria - before add" << std::endl;
                    insertBeforeAvailablePeriod( pit, spanStart, spanEnd, criteria.getRank(), criteria.getZoneSetRef() );
                }
                
                // Criteria complete
                return HNIS_RESULT_SUCCESS;
            break;

            // The criteria overlap the start of the period
            // adjust the overlapped period 
            case OVLP_TYPE_CRIT_FRONT:
            {
                std::cout << "  front overlap - c zones - shrink and add" << std::endl;

                std::cout << "  front overlap - spanStart: " << spanStart << "  pitStart: " << pit->getStartTime().getSeconds() << std::endl;
                // If the overlap has a region before the period, add a new period for it.
                if( spanStart < pit->getStartTime().getSeconds() )
                {
                    std::cout << "  front overlap - before add: " << spanStart << " : " << pit->getStartTime().getSeconds() << std::endl;
                    insertBeforeAvailablePeriod( pit, spanStart, pit->getStartTime().getSeconds(), criteria.getRank(), criteria.getZoneSetRef() );

                    spanStart = pit->getStartTime().getSeconds();
                }

                // Create a new period to represent the overlap
                std::cout << "  front overlap - overlap add: " << spanStart << " : " << spanEnd << std::endl;

                std::string pID = insertBeforeAvailablePeriod( pit, spanStart, spanEnd, criteria.getRank(), criteria.getZoneSetRef() );

                applyZoneSet( pID, pit->getZoneSetRef() );

                // Shrink/eliminate the existing period
                if( spanEnd == pit->getEndTime().getSeconds() )
                {
                    std::cout << "  front overlap - erase tail" << std::endl;
                    m_periodList.erase( pit );
                }
                else
                {
                    std::cout << "  front overlap - shrink tail: " << spanEnd << std::endl;
                    pit->setStartTimeSeconds( spanEnd );
                }

                // Finished with this criteria
                return HNIS_RESULT_SUCCESS;
            }
            break;

            // The criteria is encapsulated by the period
            // Check whether to split the existing period, or to discard this
            // criteria.
            case OVLP_TYPE_CRIT_WITHIN:
            {
                std::cout << "  criteria within existing period" << std::endl;

                // Add a new period for the first portion of the original period
                if( spanStart != pit->getStartTime().getSeconds() )
                {
                    insertBeforeAvailablePeriod( pit, pit->getStartTime().getSeconds(), spanStart, pit->getRank(), pit->getZoneSetRef() );
                }

                // Add a period for the overlap section
                std::string pID = insertBeforeAvailablePeriod( pit, spanStart, spanEnd, criteria.getRank(), criteria.getZoneSetRef() );

                applyZoneSet( pID, pit->getZoneSetRef() );

                // Shrink/eliminate the existing period
                if( criteria.getEndTime().getSeconds() == pit->getEndTime().getSeconds() )
                    m_periodList.erase( pit );
                else
                    pit->setStartTime( criteria.getEndTime() );            

                // Finished with this criteria
                return HNIS_RESULT_SUCCESS;
            }
            break;

            // Must keep going to ensure all of criteria has
            // been accounted for.
            case OVLP_TYPE_CRIT_BACK:
            {
                // Add a period to represent the overlap region
                std::string pID = insertAfterAvailablePeriod( pit, spanStart, pit->getEndTime().getSeconds(), criteria.getRank(), criteria.getZoneSetRef() );

                applyZoneSet( pID, pit->getZoneSetRef() );

                // Calculate the used up portion of the criteria
                uint origEnd = pit->getEndTime().getSeconds();

                // Shorten the original period to subtract the overlap
                pit->setEndTimeSeconds( spanStart );

                // Adjust the criteria remainder
                spanStart = origEnd;
             
                std::cout << "  back adj - origEnd: " << origEnd << "  spanStart: " << spanStart << std::endl;

                // Bump the iterator to point at the element which
                // was just inserted and run the loop again.
                pit++; 
            }
            break;

            // Must keep going to ensure all of criteria has
            // been accounted for.
            case OVLP_TYPE_CRIT_AROUND:
            {
                // Update the current period zone with
                // new criteria additions
                pit->addZoneSet( criteria.getZoneSetRef() );
 
                // Update the criteria start and next iteration
                spanStart = pit->getEndTime().getSeconds();
            }
            break;
        }

        index++;
    }

    // If the criteria was not all used up
    // in the collision scan then add any remainder
    // here
    if( spanStart != spanEnd )
    {
        std::cout << "applyCriteria - loop finish add" << std::endl;

        addAvailablePeriod( criteria.getStartTime().getSeconds(), criteria.getEndTime().getSeconds(), criteria.getRank(), criteria.getZoneSetRef() );
    }

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNISDay::getAvailableSlotsForZone( std::string zoneID, std::vector< HNISPeriod > &slotList )
{
    // Build a list of available slots for this zone.
    for( std::list< HNISPeriod >::iterator it = m_periodList.begin(); it != m_periodList.end(); it++ )
    {
        if( it->getType() == HNIS_PERIOD_TYPE_AVAILABLE )
        {
            if( it->hasZone( zoneID ) )
            {
                slotList.push_back( *it );
            }
        }
    }

    // Sort the zone list by rank and time.
    // Lower Ranks and earlier times are favored
    std::sort( slotList.begin(), slotList.end(), HNISPeriod::rankCompare );

    return HNIS_RESULT_SUCCESS;
}

void
HNISDay::addPeriodZoneOn( std::string periodID, std::string zoneID, uint durationSec )
{
    for( std::list< HNISPeriod >::iterator it = m_periodList.begin(); it != m_periodList.end(); it++ )
    {
        if( it->getID() != periodID )
            continue;

        if( it->getType() != HNIS_PERIOD_TYPE_AVAILABLE )
            continue;

        uint periodSec = it->getEndTime().getSeconds() - it->getStartTime().getSeconds();

        if( durationSec > periodSec )
        {
            return;
        }
        else if( durationSec == periodSec )
        {
            // Current period will be consumed
            // so just turn it into the zone-on period
            it->setType( HNIS_PERIOD_TYPE_ZONE_ON );
            it->clearZones();
            it->addZone( zoneID );
        }
        else
        {
            // Determine the dividing time
            uint dTime = it->getStartTime().getSeconds() + durationSec;

            // Create a new period for the zone on time
            char newID[64];
            HNISPeriod period;

            sprintf( newID, "%ld", m_periodList.size() );

            // Add a period to represent the overlap region
            period.setID( newID );
            period.setType( HNIS_PERIOD_TYPE_ZONE_ON );
            period.setDayIndex( m_dayIndex );
            period.setStartTime( it->getStartTime() );
            period.setEndTimeSeconds( dTime );

            period.clearZones();
            period.addZone( zoneID );

            m_periodList.insert( it, period );

            // Shorten the available time period
            it->setStartTimeSeconds( dTime );
        }        
    }
}

HNIS_RESULT_T 
HNISDay::addPeriod( HNISPeriod value )
{
    std::cout << "addPeriod: " << value.getID() << std::endl;

    m_periodList.push_back( value );
}

void 
HNISDay::getPeriodList( std::vector< HNISPeriod > &periodList )
{
    periodList.clear();

    std::cout << "getPeriodList: " << m_periodList.size() << std::endl;

    for( std::list< HNISPeriod >::iterator it = m_periodList.begin(); it != m_periodList.end(); it++ )
    {
        //std::cout << "PL: " << it->getType() << "  " << it->getStartTimeStr() << std::endl;
        periodList.push_back( *it );
    }

}

std::string 
HNISDay::getDayName()
{
    return s_dayNames[ m_dayIndex ];
}

void
HNISDay::debugPrint()
{
    std::cout << "==== Day: " << getDayName() << " ====" << std::endl;
    for( std::list< HNISPeriod >::iterator it = m_periodList.begin(); it != m_periodList.end(); it++ )
    {
        std::cout << "   " << it->getType() << "  " << it->getStartTimeStr() << "  " << it->getEndTimeStr() << "  " << it->getID() << "  " << it->getZoneSetAsStr() << std::endl;
    }    
}

HNIrrigationSchedule::HNIrrigationSchedule()
{
    std::cout << "HNIrrigationSchedule -- create" << std::endl;

    m_timezone = "Americas/Denver";
    m_schCRC32 = 0;

    for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
        m_dayArr[ indx ].setIndex( (HNIS_DAY_INDX_T) indx );

}

HNIrrigationSchedule::~HNIrrigationSchedule()
{
    std::cout << "HNIrrigationSchedule -- destroy" << std::endl;

}

void 
HNIrrigationSchedule::clear()
{
    // Clear any existing data
    for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
        m_dayArr[ indx ].clear();

    m_criteriaMap.clear();
    m_zoneMap.clear();
    m_schCRC32 = 0;
}

std::string
HNIrrigationSchedule::getTimezoneStr()
{
    return m_timezone;
}

uint 
HNIrrigationSchedule::getSMCRC32()
{
    return m_schCRC32;
}

std::string
HNIrrigationSchedule::getSMCRC32Str()
{
    char tmpStr[64];
    sprintf( tmpStr, "0x%x", m_schCRC32 );
    return tmpStr;
}

bool 
HNIrrigationSchedule::hasCriteria( std::string eventID )
{
    std::map< std::string, HNIrrigationCriteria >::iterator it = m_criteriaMap.find( eventID );

    if( it == m_criteriaMap.end() )
        return false;

    return true;
}

HNIrrigationCriteria*
HNIrrigationSchedule::updateCriteria( std::string id )
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

void 
HNIrrigationSchedule::deleteCriteria( std::string eventID )
{
    // Find the referenced zone
    std::map< std::string, HNIrrigationCriteria >::iterator it = m_criteriaMap.find( eventID );

    // If already no existant than nothing to do.
    if( it == m_criteriaMap.end() )
        return;

    // Get rid of the zone record
    m_criteriaMap.erase( it );
}

void 
HNIrrigationSchedule::getCriteriaList( std::vector< HNIrrigationCriteria > &criteriaList )
{
    for( std::map< std::string, HNIrrigationCriteria >::iterator it = m_criteriaMap.begin(); it != m_criteriaMap.end(); it++ )
    {
        criteriaList.push_back( it->second );
    }
}

HNIS_RESULT_T 
HNIrrigationSchedule::getCriteria( std::string eventID, HNIrrigationCriteria &event )
{
    std::map< std::string, HNIrrigationCriteria >::iterator it = m_criteriaMap.find( eventID );

    if( it == m_criteriaMap.end() )
        return HNIS_RESULT_FAILURE;

    event = it->second;
    return HNIS_RESULT_SUCCESS;
}

HNIrrigationZone*
HNIrrigationSchedule::updateZone( std::string zoneID )
{
    std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.find( zoneID );

    if( it == m_zoneMap.end() )
    {
        HNIrrigationZone nZone;
        nZone.setID( zoneID );
        m_zoneMap.insert( std::pair< std::string, HNIrrigationZone >( zoneID, nZone ) );\
        return &( m_zoneMap[ zoneID ] );
    }

    return &(it->second);
}

void 
HNIrrigationSchedule::deleteZone( std::string zoneID )
{
    // Find the referenced zone
    std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.find( zoneID );

    // If already no existant than nothing to do.
    if( it == m_zoneMap.end() )
        return;

    // Get rid of the zone record
    m_zoneMap.erase( it );
}

HNIS_RESULT_T 
HNIrrigationSchedule::initZoneListSection( HNodeConfig &cfg )
{
    HNCSection *secPtr;

    cfg.updateSection( "irrZoneInfo", &secPtr );
    
    HNCObjList *listPtr;
    secPtr->updateList( "zoneList", &listPtr );

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationSchedule::initCriteriaListSection( HNodeConfig &cfg )
{
    HNCSection *secPtr;

    cfg.updateSection( "irrCriteriaInfo", &secPtr );
    
    HNCObjList *listPtr;
    secPtr->updateList( "criteriaList", &listPtr );

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationSchedule::initConfigSections( HNodeConfig &cfg )
{
    HNIS_RESULT_T result;

    result = initZoneListSection( cfg );
    if( result != HNIS_RESULT_SUCCESS )
        return result;

    result = initCriteriaListSection( cfg );
    if( result != HNIS_RESULT_SUCCESS )
        return result;

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationSchedule::readZoneListSection( HNodeConfig &cfg )
{
    HNCSection  *secPtr;

    // Aquire a pointer to the "device" section
    cfg.updateSection( "irrZoneInfo", &secPtr );

    // Get a list pointer
    HNCObjList *listPtr;
    secPtr->updateList( "zoneList", &listPtr );

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
        HNIrrigationZone *zonePtr = updateZone( zoneID );

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
HNIrrigationSchedule::readCriteriaListSection( HNodeConfig &cfg )
{
    HNCSection  *secPtr;

    // Aquire a pointer to the "device" section
    cfg.updateSection( "irrCriteriaInfo", &secPtr );

    // Get a list pointer
    HNCObjList *listPtr;
    secPtr->updateList( "criteriaList", &listPtr );

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
        HNIrrigationCriteria *criteriaPtr = updateCriteria( criteriaID );

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
HNIrrigationSchedule::readConfigSections( HNodeConfig &cfg )
{
    HNIS_RESULT_T result;

    result = readZoneListSection( cfg );
    if( result != HNIS_RESULT_SUCCESS )
        return result;

    result = readCriteriaListSection( cfg );
    if( result != HNIS_RESULT_SUCCESS )
        return result;

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationSchedule::updateZoneListSection( HNodeConfig &cfg )
{
    char tmpStr[256];
    HNCSection *secPtr;
    cfg.updateSection( "irrZoneInfo", &secPtr );
    //secPtr->updateValue( "test1", "value1" );

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

HNIS_RESULT_T 
HNIrrigationSchedule::updateCriteriaListSection( HNodeConfig &cfg )
{
    char tmpStr[256];
    HNCSection *secPtr;
    cfg.updateSection( "irrCriteriaInfo", &secPtr );
    //secPtr->updateValue( "test1", "value1" );

    HNCObjList *listPtr;
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

HNIS_RESULT_T 
HNIrrigationSchedule::updateConfigSections( HNodeConfig &cfg )
{
    HNIS_RESULT_T result;

    result = updateZoneListSection( cfg );
    if( result != HNIS_RESULT_SUCCESS )
        return result;

    result = updateCriteriaListSection( cfg );
    if( result != HNIS_RESULT_SUCCESS )
        return result;

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationSchedule::buildSchedule()
{
    std::cout << "BuildSchedule - start" << std::endl;

    // Clear any schedule data
    for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
        m_dayArr[ indx ].clear();

    // Create an array covering a week of non-overlapping available scheduling slots
    for( std::map< std::string, HNIrrigationCriteria >::iterator cit = m_criteriaMap.begin(); cit != m_criteriaMap.end(); cit++ )
    {
        // Check each possible day.
        for( int dayIndx = 0; dayIndx < HNIS_DINDX_NOTSET; dayIndx++ )
        {
            bool inserted = false;

            // Check whether this day applies to the criteria
            if( cit->second.isForDay( (HNIS_DAY_INDX_T) dayIndx ) == false )
                continue;

            // Insert the new criteria
            m_dayArr[ dayIndx ].applyCriteria( cit->second );
        }
    }    

    for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
        m_dayArr[ indx ].debugPrint();
    

    // Attempt to schedule each zone
    for( std::map< std::string, HNIrrigationZone >::iterator zit = m_zoneMap.begin(); zit != m_zoneMap.end(); zit++ )
    {
        std::vector<HNISPeriod> availSlotLists[ HNIS_DINDX_NOTSET ];
        uint maxLayer = 0;
        uint totalAvailSeconds = 0;

        // Generate a list of available zone slots for each day.
        for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
        {
            m_dayArr[ indx ].getAvailableSlotsForZone( zit->second.getID(), availSlotLists[ indx ] );

            if( maxLayer < availSlotLists[ indx ].size() )
                maxLayer = availSlotLists[ indx ].size();
        }

        // Now generate an ordered list across all days by traversing the individual day list in a specific
        // pattern. Order the slots so that they are spread temporally, so that as the 
        // amount of watering time goes up/down the watering is still distributed
        // roughly evenly over the whole available time period.
        uint insOrdArr[] = { HNIS_DINDX_SUNDAY, HNIS_DINDX_MONDAY, HNIS_DINDX_TUESDAY, HNIS_DINDX_WEDNESDAY, HNIS_DINDX_THURSDAY, HNIS_DINDX_FRIDAY, HNIS_DINDX_SATURDAY };
        std::vector<HNISPeriod> orderedSlotList;

        for( uint layerIndex = 0; layerIndex < maxLayer; layerIndex++ )
        {
            for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
            {
                HNISPeriod slot;
                uint dayIndx = insOrdArr[ indx ];
                if( layerIndex < availSlotLists[ dayIndx ].size() )
                {
                    slot = availSlotLists[ dayIndx ][ layerIndex ];
                    totalAvailSeconds += ( slot.getEndTime().getSeconds() - slot.getStartTime().getSeconds() );
                    orderedSlotList.push_back( slot );
                }
            }
        }

        // Calculate how many of the slots to use.
        uint uniqueSlots = orderedSlotList.size();

        std::cout << "==== Zone: " << zit->second.getID() << "  availSec: " << totalAvailSeconds << "  slotCount: " << uniqueSlots << std::endl;

        // Calculate the total weekly time needed for the zone.
        uint totalWeeklySeconds = zit->second.getWeeklySeconds();
        
        // Make sure it will fit.
        if( totalWeeklySeconds > totalAvailSeconds )
        {
            // Mark the zone as unable to schedule
            std::cout << "   Error - not enough available time to schedule zone - needed: " << totalWeeklySeconds << "  available: " << totalAvailSeconds << std::endl;

            // Attempt next zone
            continue;
        }

        // Check that the number of available slots is sufficient
        uint minCycleCnt = totalWeeklySeconds / zit->second.getMaximumCycleTimeSeconds();
        if( minCycleCnt > uniqueSlots )
        {
            // Mark zone as unschedulable
            std::cout << "   Error - not enough slots available to schedule zone - needed: " << minCycleCnt << "  available: " << uniqueSlots << std::endl;

            // Attempt next zone
            continue;
        }

        // Shoot for an average time between lower and upper bound
        uint cycleTimePerSlot = totalWeeklySeconds / uniqueSlots;
        uint targetCycleTime = zit->second.getMinimumCycleTimeSeconds();
        if( targetCycleTime < cycleTimePerSlot )
            targetCycleTime = cycleTimePerSlot;

        targetCycleTime = ( ( targetCycleTime + zit->second.getMaximumCycleTimeSeconds() ) / 2 );
        uint targetSlotCnt = totalWeeklySeconds / targetCycleTime;
    
        std::cout << "  targetCycleTime: " << targetCycleTime << "  targetSlotCnt: " << targetSlotCnt << std::endl;

        // Make the allocations of slots from the ordered list
        std::vector<HNISPeriod>::iterator slit = orderedSlotList.begin();
        for( uint slotIndex = 0; slotIndex < targetSlotCnt; slit++, slotIndex++ )
        {
            std::cout << "    day: " << slit->getDayIndex() << "  rank: " << slit->getRank() << "  startSec: " << slit->getStartTime().getSeconds() << std::endl;
            m_dayArr[ slit->getDayIndex() ].addPeriodZoneOn( slit->getID(), zit->second.getID(), targetCycleTime );
        }

    }

    for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
        m_dayArr[ indx ].debugPrint();

    // Calculate a hash value for this schedule
    // which will be used to determine update flow.
    calculateSMCRC32();

    return HNIS_RESULT_SUCCESS;

}

void 
HNIrrigationSchedule::getZoneList( std::vector< HNIrrigationZone > &zoneList )
{
    for( std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.begin(); it != m_zoneMap.end(); it++ )
    {
        zoneList.push_back( it->second );
    }
}

bool 
HNIrrigationSchedule::hasZone( std::string zoneID )
{
    std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.find( zoneID );

    if( it == m_zoneMap.end() )
        return false;

    return true;
}

HNIS_RESULT_T 
HNIrrigationSchedule::getZone( std::string zoneID, HNIrrigationZone &zone )
{
    std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.find( zoneID );

    if( it == m_zoneMap.end() )
        return HNIS_RESULT_FAILURE;

    zone = it->second;
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationSchedule::getZoneName( std::string zoneID, std::string &name )
{
    std::map< std::string, HNIrrigationZone >::iterator it = m_zoneMap.find( zoneID );

    name.clear();

    if( it == m_zoneMap.end() )
        return HNIS_RESULT_FAILURE;

    name = it->second.getName();
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationSchedule::getScheduleInfoJSON( std::ostream &ostr )
{
    // Create a json root object
    pjs::Object jsRoot;

    // Add the timezone name field
    jsRoot.set( "scheduleTimezone", "Americas/Denver" );

    // Add data for each day
    pjs::Object jsDays;

    for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
    {
        pjs::Array jsActions;

        std::vector< HNISPeriod > periodList;
        m_dayArr[ indx ].getPeriodList( periodList );

        for( std::vector< HNISPeriod >::iterator it = periodList.begin(); it != periodList.end(); it++ )
        {
            pjs::Object jsSWAction;

            if( it->getType() != HNIS_PERIOD_TYPE_ZONE_ON )
            {
                std::cout << "js continue" << std::endl;                
                continue;
            }

            jsSWAction.set( "action", "on" );
            jsSWAction.set( "startTime", it->getStartTimeStr() );
            jsSWAction.set( "endTime", it->getEndTimeStr() );
            jsSWAction.set( "zoneid", it->getID() );

            std::string zName;
            getZoneName( it->getID(), zName );
            jsSWAction.set( "name", zName );

            jsActions.add( jsSWAction );
        }
        
        jsDays.set( m_dayArr[ indx ].getDayName(), jsActions );
    }

    jsRoot.set( "scheduleMatrix", jsDays );

    try
    {
        // Write out the generated json
        pjs::Stringifier::stringify( jsRoot, ostr, 1 );
    }
    catch( ... )
    {
        return HNIS_RESULT_FAILURE;
    }

    return HNIS_RESULT_SUCCESS;
}

void
HNIrrigationSchedule::calculateSMCRC32()
{
    // Get a string to build the data into
    Poco::Checksum digest;

    digest.update( getTimezoneStr() );

    for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
    {
        std::vector< HNISPeriod > periodList;
        m_dayArr[ indx ].getPeriodList( periodList );

        for( std::vector< HNISPeriod >::iterator it = periodList.begin(); it != periodList.end(); it++ )
        {
            pjs::Object jsSWAction;
          
            if( it->getType() != HNIS_PERIOD_TYPE_ZONE_ON )
            {
                continue;
            }

            digest.update( "swon" );
            digest.update( it->getStartTimeStr() );
            digest.update( it->getEndTimeStr() );
            digest.update( m_zoneMap[ it->getZoneSetAsStr() ].getSWIDListStr() );
        }
        
    }

    std::cout << "Calculate SMCRC32: " << digest.checksum() << std::endl;

    m_schCRC32 = digest.checksum();
}

std::string 
HNIrrigationSchedule::getSwitchDaemonJSON()
{
    // Open an output stream to the temporary file
    std::stringstream msg;

    // Create a json root object
    pjs::Object jsRoot;

    // Add the timezone name field
    jsRoot.set( "scheduleTimezone", getTimezoneStr() ); // "Americas/Denver" );

    // Add the scheduleMatrix hash value
    jsRoot.set( "scheduleCRC32", getSMCRC32Str() );

    // Add data for each day
    pjs::Object jsDays;

    for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
    {
        pjs::Array jsActions;

        std::cout << "js chk day: " << indx << std::endl;

        std::vector< HNISPeriod > periodList;
        m_dayArr[ indx ].getPeriodList( periodList );

        for( std::vector< HNISPeriod >::iterator it = periodList.begin(); it != periodList.end(); it++ )
        {
            pjs::Object jsSWAction;
          
            std::cout << "js chk period: " << it->getType() << std::endl;

            if( it->getType() != HNIS_PERIOD_TYPE_ZONE_ON )
            {
                std::cout << "js continue" << std::endl;                
                continue;
            }

            std::cout << "js add action" << std::endl;

            jsSWAction.set( "action", "swon" );

            std::cout << "act st: " << it->getStartTimeStr() << std::endl;

            jsSWAction.set( "startTime", it->getStartTimeStr() );
            jsSWAction.set( "endTime", it->getEndTimeStr() );

            std::cout << "zone id: " << it->getID() << std::endl;

            jsSWAction.set( "swid", m_zoneMap[ it->getZoneSetAsStr() ].getSWIDListStr() );

            jsActions.add( jsSWAction );
        }
        
        jsDays.set( m_dayArr[ indx ].getDayName(), jsActions );
    }

    jsRoot.set( "scheduleMatrix", jsDays );

    try
    {
        // Write out the generated json
        pjs::Stringifier::stringify( jsRoot, msg, 1 );
    }
    catch( ... )
    {
        return "";
    }

    return msg.str();
}


