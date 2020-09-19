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

// Must match the static event type enum from the header file.
static const char *s_criteriaTypeStrs[] =
{
    "notset",            // HNIS_CTYPE_NOTSET
    "everyday-keepout",  // HNIS_CTYPE_EVERYDAY_KEEPOUT,
    "single-keepout",    // HNIS_CTYPE_SINGLE_KEEPOUT,
    "everyday-zone",     // HNIS_CTYPE_EVERYDAY_ZONE,
    "single-zone",       // HNIS_CTYPE_SINGLE_ZONE,
    "unknown"            // HNIS_CTYPE_LASTENTRY
};

HNI24HTime::HNI24HTime()
{
    secOfDay = 0;
}

HNI24HTime::~HNI24HTime()
{

}

uint
HNI24HTime::getSeconds() const
{
    return secOfDay;
}

void
HNI24HTime::getHMS( uint &hour, uint &minute, uint &second )
{
    hour    = (secOfDay / (60 * 60));
    minute  = (secOfDay - (hour * 60 * 60))/60;
    second  = secOfDay - ((hour * 60 * 60) + (minute * 60));
}

//#include <string.h>

std::string
HNI24HTime::getHMSStr()
{
    std::string result;
    char tmpBuf[256];
    uint hour, minute, second;

    getHMS( hour, minute, second );

//    std::cout << "hms: " << hour << " " << minute << " " << second << std::endl;

    sprintf( tmpBuf, "%2.2d:%2.2d:%2.2d", hour, minute, second );

    //std::cout << "sl: " << strlen(tmpBuf) << "<str>" << tmpBuf << "<eos>" << std::endl;

    result.assign( tmpBuf );

    return result;
}

HNIS_RESULT_T
HNI24HTime::setFromHMS( uint hour, uint min, uint sec )
{
    secOfDay = (hour * 60 * 60) + (min * 60) + sec;

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNI24HTime::setFromSeconds( uint seconds )
{
    // Set to provided value, cieling 24 hours.
    secOfDay = seconds;
    if( secOfDay > HNIS_SECONDS_IN_24H )
        secOfDay = HNIS_SECONDS_IN_24H;

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNI24HTime::parseTime( std::string value )
{
    uint hour;
    uint min;
    uint sec;

    sscanf( value.c_str(), "%d:%d:%d", &hour, &min, &sec );

    return setFromHMS( hour, min, sec );
}

void
HNI24HTime::addSeconds( uint seconds )
{
    // Add the two times, cap 24 hours
    secOfDay = (secOfDay + seconds); 
    if( secOfDay > HNIS_SECONDS_IN_24H )
        secOfDay = HNIS_SECONDS_IN_24H;
}

void
HNI24HTime::subtractSeconds( uint seconds )
{
    // Subtract seconds, cap at 0 hour

    if( secOfDay <= seconds )
        secOfDay = 0;

    secOfDay = (secOfDay - seconds); 
}

HNScheduleCriteria::HNScheduleCriteria()
{
    m_rank = 5;
    m_dayBits    = HNSC_DBITS_DAILY;
}

HNScheduleCriteria::~HNScheduleCriteria()
{

}

void 
HNScheduleCriteria::setID( std::string id )
{
    m_id = id;
}

void 
HNScheduleCriteria::setName( std::string value )
{
    m_name = value;
}

void 
HNScheduleCriteria::setDesc( std::string value )
{
    m_desc = value;
}

HNIS_RESULT_T 
HNScheduleCriteria::setTimesFromStr( std::string startTime, std::string endTime )
{
    m_startTime.parseTime( startTime );
 
    std::cout << "mst: " << m_startTime.getHMSStr() << std::endl;

    m_endTime.parseTime( endTime );
}

HNIS_RESULT_T 
HNScheduleCriteria::setStartTime( std::string startTime )
{
    m_startTime.parseTime( startTime );
}

HNIS_RESULT_T 
HNScheduleCriteria::setEndTime( std::string endTime )
{
    m_endTime.parseTime( endTime );
}

void 
HNScheduleCriteria::setRank( uint value )
{
    m_rank = value;
}

void 
HNScheduleCriteria::clearDayBits()
{
    m_dayBits = HNSC_DBITS_DAILY;
}

void 
HNScheduleCriteria::setDayBits( uint value )
{
    m_dayBits = (HNSC_DBITS_T) value;
}

void 
HNScheduleCriteria::addDayByName( std::string name )
{
    // Looking for match
    uint index;
    for( index = 0; index <= HNIS_DINDX_NOTSET; index++ )
    {
        // If found break out
        if( s_dayNames[ index ] == name )
            break;
    }

/* FIXME
    // Set the index, NOT_SET if not found.
    m_dayIndex = (HNIS_DAY_INDX_T) index;
*/
}

bool 
HNScheduleCriteria::hasZone( std::string zoneID )
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
HNScheduleCriteria::clearZones()
{
    m_zoneSet.clear();
}

void 
HNScheduleCriteria::addZone( std::string name )
{
    m_zoneSet.insert( name );
}
         
std::set< std::string >& 
HNScheduleCriteria::getZoneSetRef()
{
    return m_zoneSet;
}

std::string 
HNScheduleCriteria::getZoneSetAsStr()
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
HNScheduleCriteria::getID()
{
    return m_id;
}

std::string 
HNScheduleCriteria::getName()
{
    return m_name;
}

std::string 
HNScheduleCriteria::getDesc()
{
    return m_desc;
}

HNI24HTime&
HNScheduleCriteria::getStartTime()
{
    return m_startTime;
}

HNI24HTime&
HNScheduleCriteria::getEndTime()
{
    return m_endTime;
}

uint 
HNScheduleCriteria::getRank()
{
    return m_rank;
}

bool 
HNScheduleCriteria::isForDay( HNIS_DAY_INDX_T dindx )
{
    uint dayMask = (1 << dindx);

    if( m_dayBits == HNSC_DBITS_DAILY )
        return true;

    if( m_dayBits & dayMask )
        return true;

    return false;
}

uint 
HNScheduleCriteria::getDayBits()
{
    return m_dayBits;
}

HNIS_RESULT_T 
HNScheduleCriteria::validateSettings()
{
    // Add validation checking here
    return HNIS_RESULT_SUCCESS;
}

HNISPeriod::HNISPeriod()
{
    m_type = HNIS_PERIOD_TYPE_NOTSET;
    m_rank = 0;
    m_slideLater = true;
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
HNISPeriod::setSlideLater( bool value )
{
    m_slideLater = value;
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

bool
HNISPeriod::isSlideLater()
{
    return m_slideLater;
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

HNIZScheduleState::HNIZScheduleState()
{
    m_nextTop = false;

    m_lastBottomSec = 0;

    m_lastTopSec = HNIS_SECONDS_IN_24H;
}

HNIZScheduleState::~HNIZScheduleState()
{

}

void 
HNIZScheduleState::setNextTop( bool value )
{
    m_nextTop = value;
}

void 
HNIZScheduleState::toggleNextTop()
{
    m_nextTop = ((m_nextTop == false) ? true : false);
}

void 
HNIZScheduleState::setTopSeconds( uint value )
{
    m_lastTopSec = value;
}

void 
HNIZScheduleState::setBottomSeconds( uint value )
{
    m_lastBottomSec = value;
}

bool 
HNIZScheduleState::isTopNext()
{
    return m_nextTop;
}

uint 
HNIZScheduleState::getTopSeconds()
{
    return m_lastTopSec;
}

uint 
HNIZScheduleState::getBottomSeconds()
{
    return m_lastBottomSec;
}

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

void 
HNIrrigationZone::setSWIDList( std::string swidList )
{
    m_swidList = swidList;
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

std::string 
HNIrrigationZone::getSWIDListStr()
{
    return m_swidList;
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

#if 0
HNIS_RESULT_T 
HNIrrigationZone::getNextSchedulingPeriod( uint dayIndex, uint cycleIndex, HNIZScheduleState &schState, HNISPeriod &tgtPeriod )
{
    std::cout << "getNextSchedulingPeriod: " << dayIndex << "  " << cycleIndex << std::endl;

    // Nothing to do if already completely scheduled.
    if( cycleIndex >= getTargetCyclesPerDay() )
        return HNIS_RESULT_SCH_NONE;

    // Figure out how many days of the week can be supported
    uint perDayMinimum = getMinimumCycleTimeSeconds() * getTargetCyclesPerDay();

    uint dayCnt = getWeeklySeconds() / perDayMinimum;

    std::cout << "weeklySec: " << getWeeklySeconds() << std::endl;
    std::cout << "cyclePerDay: " << getTargetCyclesPerDay() << std::endl;
    std::cout << "minCycleDuration: " << getMinimumCycleTimeSeconds() << std::endl;
    std::cout << "perDayMinimum: " << perDayMinimum << std::endl;
    std::cout << "dayCnt: " << dayCnt << std::endl;

    if( dayCnt > 7 )
        dayCnt = 7;

    uint perDaySec   = getWeeklySeconds() / dayCnt;
    uint perCycleSec = perDaySec / getTargetCyclesPerDay();

    switch( dayCnt )
    {
        // Less than one days worth of time
        case 0:
            if( dayIndex != HNIS_DINDX_MONDAY )
                return HNIS_RESULT_SCH_NONE;
        break;

        case 1:
            if( dayIndex != HNIS_DINDX_TUESDAY )
                return HNIS_RESULT_SCH_NONE;
        break;

        case 2:
            if( (dayIndex != HNIS_DINDX_TUESDAY) && (dayIndex != HNIS_DINDX_THURSDAY) )
                return HNIS_RESULT_SCH_NONE;     
        break;

        case 3:
            if( (dayIndex != HNIS_DINDX_MONDAY) && (dayIndex != HNIS_DINDX_WEDNESDAY) && (dayIndex != HNIS_DINDX_FRIDAY) )
                return HNIS_RESULT_SCH_NONE;                
        break;

        case 4:
            if( (dayIndex != HNIS_DINDX_SUNDAY) && (dayIndex != HNIS_DINDX_TUESDAY) && (dayIndex != HNIS_DINDX_THURSDAY) && (dayIndex != HNIS_DINDX_FRIDAY) )
                return HNIS_RESULT_SCH_NONE;                
        break;

        case 5:
             if( (dayIndex == HNIS_DINDX_TUESDAY) && (dayIndex == HNIS_DINDX_THURSDAY) )
                return HNIS_RESULT_SCH_NONE;     
        break;

        case 6:
            if( dayIndex == HNIS_DINDX_WEDNESDAY )
                return HNIS_RESULT_SCH_NONE;
        break;
 
        // Schedule something on every week day.
        case 7:
        break;
    }
   
    uint cycleWidth = (60 * 60 * 24) / getTargetCyclesPerDay();

    uint cycleOffset = cycleWidth * ( cycleIndex / 2 );

    tgtPeriod.setID( getID() );
    tgtPeriod.setType( HNIS_PERIOD_TYPE_ZONE_ON );

    if( schState.isTopNext() )
    {
        uint endSec = HNIS_SECONDS_IN_24H - cycleOffset;
        if( schState.getTopSeconds() < endSec )
            endSec = schState.getTopSeconds();

        tgtPeriod.setStartTimeSeconds( endSec - perCycleSec );
        tgtPeriod.setEndTimeSeconds( endSec );
        tgtPeriod.setSlideLater( false );
    }
    else
    {
        uint startSec = cycleOffset;
        if( schState.getBottomSeconds() > startSec )
            startSec = schState.getBottomSeconds();

        tgtPeriod.setStartTimeSeconds( startSec );
        tgtPeriod.setEndTimeSeconds( startSec + perCycleSec );
        tgtPeriod.setSlideLater( true );
    }

    if( (cycleIndex + 1) >= getTargetCyclesPerDay() )
        return HNIS_RESULT_SUCCESS;

    return HNIS_RESULT_SCH_CONTINUE;
}

HNIS_RESULT_T 
HNIrrigationZone::accountPeriodPlacement( uint dayIndex, uint cycleIndex, HNIZScheduleState &schState, HNISPeriod &tgtPeriod )
{
    if( tgtPeriod.isSlideLater() )
    {
        schState.setBottomSeconds( tgtPeriod.getEndTimeSeconds() );
    }
    else
    {
        schState.setTopSeconds( tgtPeriod.getStartTimeSeconds() );
    }

    // Swap the direction of attack
    schState.toggleNextTop();

    std::cout << "New schedule state - nextTop: " << schState.isTopNext() << "  bottom: " << schState.getBottomSeconds() << "  top: " << schState.getTopSeconds() << std::endl;
    return HNIS_RESULT_SUCCESS;
}
#endif

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

void
HNISDay::collapseSegments()
{
    // Walk through the period list and collapse equivalent adjacent segments


}

HNIS_RESULT_T 
HNISDay::applyCriteria( HNScheduleCriteria &criteria )
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
                    insertBeforeAvailablePeriod( pit, pit->getStartTime().getSeconds(), spanStart, criteria.getRank(), pit->getZoneSetRef() );
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

#if 0
HNIS_RESULT_T 
HNISDay::addAvailableScheduleForZone( std::string zoneID, uint &secAvailable, std::vector<HNISPeriod> &availableList )
{
    for( std::list< HNISPeriod >::iterator it = m_periodList.begin(); it != m_periodList.end(); it++ )
    {
        if( it->getType() == HNIS_PERIOD_TYPE_AVAILABLE )
        {
            if( it->hasZone( zoneID ) )
            {
                // Add to the running total of available scheduling seconds
                secAvailable += (it->getEndTime().getSeconds() - it->getStartTime().getSeconds());

                // Add the record for the scheduleable period
                availableList.push_back( *it );
            }
        }
    }         
}
#endif

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

HNIS_CAR_T
HNISDay::assessCollision( HNISPeriod &value, uint &boundary )
{
     uint insStartSec = value.getStartTime().getSeconds();
     uint insEndSec   = value.getEndTime().getSeconds();

     boundary = 0;

     for( std::list< HNISPeriod >::iterator pit = m_periodList.begin(); pit != m_periodList.end(); pit++ )
     {
         uint curStartSec = pit->getStartTime().getSeconds();
         uint curEndSec   = pit->getEndTime().getSeconds();
       
         std::cout << "assessCol start: " << insStartSec << "  " << curStartSec << std::endl;
         std::cout << "assessCol end: " << insEndSec << "  " << curEndSec << std::endl;

         if( (insStartSec >= curStartSec) && (insEndSec <= curEndSec) )
         {
             if( value.isSlideLater() )
                 boundary = curEndSec;
             else
                 boundary = curStartSec;

             std::cout << "assessCol CONTAINED: " << boundary << std::endl;
             return HNIS_CAR_CONTAINED;
         }
         else if( (insStartSec >= curStartSec) && (insEndSec >= curEndSec) && (insStartSec < curEndSec) )
         {
             if( value.isSlideLater() )
                 boundary = curEndSec;
             else
                 boundary = curStartSec;

             std::cout << "assessCol BEFORE: " << boundary << std::endl;
             return HNIS_CAR_BEFORE;
         }
         else if( (insStartSec <= curStartSec) && (insEndSec <= curEndSec) && (insEndSec > curStartSec) )
         {
             if( value.isSlideLater() )
                 boundary = curEndSec;
             else
                 boundary = curStartSec;

             std::cout << "assessCol AFTER: " << boundary << std::endl;
             return HNIS_CAR_AFTER;
         }
     }

     std::cout << "assessCol NONE: " << boundary << std::endl;
     return HNIS_CAR_NONE;
}

#if 0
HNIS_RESULT_T 
HNISDay::scheduleTimeSlots( uint cycleIndex, HNIZScheduleState &state, HNIrrigationZone &zone )
{
    HNISPeriod znPeriod;

    std::cout << "\n\n=== scheduleTimeSlots - Day: " << m_dayIndex << "  Cycle: " << cycleIndex << " ===" << std::endl;

    // Get the zones next desired period position
    HNIS_RESULT_T result = zone.getNextSchedulingPeriod( m_dayIndex, cycleIndex, state, znPeriod );

    if( ( result != HNIS_RESULT_SCH_CONTINUE ) && ( result != HNIS_RESULT_SUCCESS ) )
        return result;

    std::cout << "\nscheduleTimeSlots - slide later: " << znPeriod.isSlideLater() << std::endl;

    // Process collisions
    uint loopCnt = 0;
    while( znPeriod.getEndTimeSeconds() <= HNIS_SECONDS_IN_24H )
    {
        uint nextOpening;

        std::cout << "\nscheduleTimeSlots - collision loop " << loopCnt << ": " << znPeriod.getStartTimeSeconds() << "  " << znPeriod.getEndTimeSeconds() << std::endl;

        if( assessCollision( znPeriod, nextOpening  ) == HNIS_CAR_NONE )
        {
            // Found a spot, add the zone on period
            addPeriod( znPeriod );

            // Done looking
            break;
        }

        std::cout << "scheduleTimeSlots - nextOpening: " << nextOpening << std::endl;

        // Shift to the next boundary and try again.
        if( znPeriod.isSlideLater() == true )
            znPeriod.moveStartToSecond( nextOpening ); 
        else
            znPeriod.moveEndToSecond( nextOpening ); 

        loopCnt += 1;
    }

    // Check if a spot was found
    if( znPeriod.getStartTimeSeconds() >= HNIS_SECONDS_IN_24H )
    {
        return HNIS_RESULT_NO_SLOT;
    }

    // Notify zone of final position of period
    zone.accountPeriodPlacement( m_dayIndex, cycleIndex, state, znPeriod );
    
    std::cout << "=== scheduleTimeSlots - Placed ===" << std::endl << std::endl ;

    // Done scheduling this zone.
    return result;
}
#endif

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
    std::map< std::string, HNScheduleCriteria >::iterator it = m_criteriaMap.find( eventID );

    if( it == m_criteriaMap.end() )
        return false;

    return true;
}

HNScheduleCriteria*
HNIrrigationSchedule::updateCriteria( std::string id )
{
    std::map< std::string, HNScheduleCriteria >::iterator it = m_criteriaMap.find( id );

    if( it == m_criteriaMap.end() )
    {
        HNScheduleCriteria nSpec;
        nSpec.setID( id );
        m_criteriaMap.insert( std::pair< std::string, HNScheduleCriteria >( id, nSpec ) );\
        return &( m_criteriaMap[ id ] );
    }

    return &(it->second);
}

void 
HNIrrigationSchedule::deleteCriteria( std::string eventID )
{
    // Find the referenced zone
    std::map< std::string, HNScheduleCriteria >::iterator it = m_criteriaMap.find( eventID );

    // If already no existant than nothing to do.
    if( it == m_criteriaMap.end() )
        return;

    // Get rid of the zone record
    m_criteriaMap.erase( it );
}

void 
HNIrrigationSchedule::getCriteriaList( std::vector< HNScheduleCriteria > &criteriaList )
{
    for( std::map< std::string, HNScheduleCriteria >::iterator it = m_criteriaMap.begin(); it != m_criteriaMap.end(); it++ )
    {
        criteriaList.push_back( it->second );
    }
}

HNIS_RESULT_T 
HNIrrigationSchedule::getCriteria( std::string eventID, HNScheduleCriteria &event )
{
    std::map< std::string, HNScheduleCriteria >::iterator it = m_criteriaMap.find( eventID );

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
            std::cout << "== READ SWIDLIST: " << rstStr << std::endl;
            zonePtr->setSWIDList( rstStr );
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
        HNScheduleCriteria *criteriaPtr = updateCriteria( criteriaID );

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

            // Walk the zoneList string
            std::sregex_token_iterator it( rstStr.begin(), rstStr.end(), ws_re, -1 );
            const std::sregex_token_iterator end;
            while( it != end )
            {
                // Add a new switch action to the queue.
                criteriaPtr->addZone( *it );
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

    for( std::map< std::string, HNScheduleCriteria >::iterator it = m_criteriaMap.begin(); it != m_criteriaMap.end(); it++ )
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
    for( std::map< std::string, HNScheduleCriteria >::iterator cit = m_criteriaMap.begin(); cit != m_criteriaMap.end(); cit++ )
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
        // Generate an ordered list 
        // of available slots for scheduling.
        // Order the slots so that they are
        // spread temporally, so that as the 
        // amount of watering time goes up/down
        // the watering is still spread evenly.
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

        // Now generate an ordered list across all days
        // by traversing the individual day list in a specific
        // pattern
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

            // Attempt next zone
            continue;
        }

        // Calculate the desired number of cycles.
        uint maxCycleCnt = totalWeeklySeconds / zit->second.getMinimumCycleTimeSeconds();
        uint minCycleCnt = totalWeeklySeconds / zit->second.getMaximumCycleTimeSeconds();

        uint cycleTimePerSlot = totalWeeklySeconds / uniqueSlots;

        std::cout << "  totWSec: " << totalWeeklySeconds << "  minCycleCnt: " << minCycleCnt << "  maxCycleCnt: " << maxCycleCnt << " PerSlotTime: " << cycleTimePerSlot << std::endl;

        // Determine the number of slots to use.
        // First check that the number of available slots is sufficient
        if( minCycleCnt > uniqueSlots )
        {
            // Mark zone as unschedulable

            // Attempt next zone
            continue;
        }

        // Shoot for an average time between lower and upper bound
        uint targetCycleTime = zit->second.getMinimumCycleTimeSeconds();
        if( targetCycleTime < cycleTimePerSlot )
            targetCycleTime = cycleTimePerSlot;

        targetCycleTime = ( ( targetCycleTime + zit->second.getMaximumCycleTimeSeconds() ) / 2 );
        uint targetSlotCnt = totalWeeklySeconds / targetCycleTime;
    
        std::cout << "  targetCycleTime: " << targetCycleTime << "  targetSlotCnt: " << targetSlotCnt << std::endl;

        // Make the allocations
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

#if 0
/*
        HNISPeriod period;

        HNScheduleCriteria *curSpec = &(eit->second);

        period.setID( curSpec->getID() );
        period.setStartTime( curSpec->getStartTime() );
        period.setEndTime( curSpec->getEndTime() );

        switch( curSpec->getType() )
        {
            case HNIS_CTYPE_EVERYDAY_KEEPOUT:
            {
                period.setType( HNIS_PERIOD_TYPE_EXCLUSION );

                for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
                    m_dayArr[ indx ].addPeriod( period );
            }
            break;

            case HNIS_CTYPE_SINGLE_KEEPOUT:
                period.setType( HNIS_PERIOD_TYPE_EXCLUSION );

                m_dayArr[ curSpec->getDayIndex() ].addPeriod( period );
            break;
        }
*/

    // Schedule zone time slots.
    for( int dayIndex = 0; dayIndex < HNIS_DINDX_NOTSET; dayIndex++ )
    {
        uint cycleIndex = 0;
        std::map< std::string, HNIZScheduleState > stateMap;

        bool slotsToSchedule = true;
        while( slotsToSchedule )
        {
            // Start out assuming this loop will finish it.
            slotsToSchedule = false;

            for( std::map< std::string, HNIrrigationZone >::iterator zit = m_zoneMap.begin(); zit != m_zoneMap.end(); zit++ )
            {
                // Maintain a scheduling state on a per-day, per-zone basis.
                std::map< std::string, HNIZScheduleState >::iterator sit = stateMap.find( zit->first );

                // If a state hasn't been created yet, the start a new one.
                if( sit == stateMap.end() )
                {
                    HNIZScheduleState nstate;
                    stateMap.insert( std::pair< std::string, HNIZScheduleState >( zit->first, nstate ) );
                    sit = stateMap.find( zit->first );
                }

                HNIS_RESULT_T result = m_dayArr[ dayIndex ].scheduleTimeSlots( cycleIndex, sit->second, zit->second );

                switch( result )
                {
                    // Zone has successfully completed scheduling
                    case HNIS_RESULT_SCH_NONE:
                    case HNIS_RESULT_SUCCESS:
                    break;

                    // This cycle completed successfully,
                    // but more slots need to be scheduled.
                    case HNIS_RESULT_SCH_CONTINUE:
                        // Continue scheduling
                        slotsToSchedule = true;
                    break;

                    // An error occurred during scheduling.
                    default:
                        return result;
                    break;
                }
            }

            // Add the next cycle of times
            cycleIndex += 1;

            std::cout << "Next Cycle: " << cycleIndex << std::endl;
        }

        // Get rid of this days state records.
        stateMap.clear();
    }

    // Calculate a hash value for this schedule
    // which will be used to determine update flow.
    calculateSMCRC32();
#endif

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


