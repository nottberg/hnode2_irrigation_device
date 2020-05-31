#include <iostream>
#include <sstream>

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>

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
static const char *s_staticEventTypeStrs[] =
{
    "notset",            // HNIS_SETYPE_NOTSET
    "everyday-keepout",  // HNIS_SETYPE_EVERYDAY_KEEPOUT,
    "single-keepout",    // HNIS_SETYPE_SINGLE_KEEPOUT,
    "everyday-zone",     // HNIS_SETYPE_EVERYDAY_ZONE,
    "single-zone",       // HNIS_SETYPE_SINGLE_ZONE,
    "unknown"            // HNIS_SETYPE_LASTENTRY
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

HNScheduleStaticEvent::HNScheduleStaticEvent()
{
    m_type = HNIS_SETYPE_NOTSET;
    m_dayIndex = HNIS_DINDX_NOTSET;
}

HNScheduleStaticEvent::~HNScheduleStaticEvent()
{

}

void 
HNScheduleStaticEvent::setID( std::string id )
{
    m_id = id;
}

void 
HNScheduleStaticEvent::setType( HNIS_SETYPE_T value )
{
    m_type = value;
}

void 
HNScheduleStaticEvent::setTypeFromStr( std::string type )
{
    for( uint indx = 0; indx < HNIS_SETYPE_LASTENTRY; indx++ )
    {
        if( s_staticEventTypeStrs[ indx ] == type )
        {
            m_type = (HNIS_SETYPE_T) indx;
            return;
        }
    }

    m_type = HNIS_SETYPE_NOTSET;
}

HNIS_RESULT_T 
HNScheduleStaticEvent::setTimesFromStr( std::string startTime, std::string endTime )
{
    m_startTime.parseTime( startTime );
 
    std::cout << "mst: " << m_startTime.getHMSStr() << std::endl;

    m_endTime.parseTime( endTime );
}

HNIS_RESULT_T 
HNScheduleStaticEvent::setStartTime( std::string startTime )
{
    m_startTime.parseTime( startTime );
}

HNIS_RESULT_T 
HNScheduleStaticEvent::setEndTime( std::string endTime )
{
    m_endTime.parseTime( endTime );
}

void 
HNScheduleStaticEvent::setDayIndex( HNIS_DAY_INDX_T dayIndx )
{
    m_dayIndex = dayIndx;
}

void 
HNScheduleStaticEvent::setDayIndexFromNameStr( std::string name )
{
    // Looking for match
    uint index;
    for( index = 0; index <= HNIS_DINDX_NOTSET; index++ )
    {
        // If found break out
        if( s_dayNames[ index ] == name )
            break;
    }

    // Set the index, NOT_SET if not found.
    m_dayIndex = (HNIS_DAY_INDX_T) index;
}

std::string 
HNScheduleStaticEvent::getID()
{
    return m_id;
}

HNIS_SETYPE_T 
HNScheduleStaticEvent::getType()
{
     return m_type;
}

std::string 
HNScheduleStaticEvent::getTypeStr()
{
    if( m_type > HNIS_SETYPE_LASTENTRY )
        return s_staticEventTypeStrs[ HNIS_SETYPE_LASTENTRY ];

    return s_staticEventTypeStrs[ m_type ];
}

HNI24HTime&
HNScheduleStaticEvent::getStartTime()
{
    return m_startTime;
}

HNI24HTime&
HNScheduleStaticEvent::getEndTime()
{
    return m_endTime;
}

HNIS_DAY_INDX_T 
HNScheduleStaticEvent::getDayIndex()
{
    return m_dayIndex;
}

std::string 
HNScheduleStaticEvent::getDayName()
{
    if( m_dayIndex > HNIS_DINDX_NOTSET )
        return s_dayNames[ HNIS_DINDX_NOTSET ];

    return s_dayNames[ m_dayIndex ];
}

HNIS_RESULT_T 
HNScheduleStaticEvent::validateSettings()
{
    // Add validation checking here
    return HNIS_RESULT_SUCCESS;
}

HNISPeriod::HNISPeriod()
{
    m_type = HNIS_PERIOD_TYPE_NOTSET;

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
HNISPeriod::sortCompare( const HNISPeriod& first, const HNISPeriod& second )
{
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
    m_dailyCycles = 2;
    m_minCycleSec = (2 * 60);
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
HNIrrigationZone::setTargetCyclesPerDay( uint value )
{
    m_dailyCycles = value;
}

void
HNIrrigationZone::setMinimumCycleTimeSeconds( uint value )
{
    m_minCycleSec = value;
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
    return m_weeklySec; // (((5 * 60) * 2) * 7);
}

uint 
HNIrrigationZone::getTargetCyclesPerDay()
{
    return m_dailyCycles; // 2;
}

uint 
HNIrrigationZone::getMinimumCycleTimeSeconds()
{
    return m_minCycleSec; // (2 * 60);
}

HNIS_RESULT_T 
HNIrrigationZone::getNextSchedulingPeriod( uint dayIndex, uint cycleIndex, HNIZScheduleState &schState, HNISPeriod &tgtPeriod )
{
    std::cout << "getNextSchulingPeriod: " << dayIndex << "  " << cycleIndex << std::endl;

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
HNISDay::sort()
{
    m_periodList.sort( HNISPeriod::sortCompare );
}

void 
HNISDay::coalesce()
{
   // Cycle through all periods,
   // Combine exclusions that overlapp

}

HNIS_RESULT_T 
HNISDay::addPeriod( HNISPeriod value )
{
    std::cout << "addPeriod: " << value.getID() << std::endl;

    m_periodList.push_back( value );

    sort();
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


HNIrrigationSchedule::HNIrrigationSchedule()
{
    std::cout << "HNIrrigationSchedule -- create" << std::endl;

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

    m_eventMap.clear();
    m_zoneMap.clear();
}

bool 
HNIrrigationSchedule::hasEvent( std::string eventID )
{
    std::map< std::string, HNScheduleStaticEvent >::iterator it = m_eventMap.find( eventID );

    if( it == m_eventMap.end() )
        return false;

    return true;
}

HNScheduleStaticEvent*
HNIrrigationSchedule::updateEvent( std::string id )
{
    std::map< std::string, HNScheduleStaticEvent >::iterator it = m_eventMap.find( id );

    if( it == m_eventMap.end() )
    {
        HNScheduleStaticEvent nSpec;
        nSpec.setID( id );
        m_eventMap.insert( std::pair< std::string, HNScheduleStaticEvent >( id, nSpec ) );\
        return &( m_eventMap[ id ] );
    }

    return &(it->second);
}

void 
HNIrrigationSchedule::deleteEvent( std::string eventID )
{
    // Find the referenced zone
    std::map< std::string, HNScheduleStaticEvent >::iterator it = m_eventMap.find( eventID );

    // If already no existant than nothing to do.
    if( it == m_eventMap.end() )
        return;

    // Get rid of the zone record
    m_eventMap.erase( it );
}

void 
HNIrrigationSchedule::getEventList( std::vector< HNScheduleStaticEvent > &eventList )
{
    for( std::map< std::string, HNScheduleStaticEvent >::iterator it = m_eventMap.begin(); it != m_eventMap.end(); it++ )
    {
        eventList.push_back( it->second );
    }
}

HNIS_RESULT_T 
HNIrrigationSchedule::getEvent( std::string eventID, HNScheduleStaticEvent &event )
{
    std::map< std::string, HNScheduleStaticEvent >::iterator it = m_eventMap.find( eventID );

    if( it == m_eventMap.end() )
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
HNIrrigationSchedule::initStaticEventListSection( HNodeConfig &cfg )
{
    HNCSection *secPtr;

    cfg.updateSection( "irrStaticEventInfo", &secPtr );
    
    HNCObjList *listPtr;
    secPtr->updateList( "eventList", &listPtr );

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationSchedule::initConfigSections( HNodeConfig &cfg )
{
    HNIS_RESULT_T result;

    result = initZoneListSection( cfg );
    if( result != HNIS_RESULT_SUCCESS )
        return result;

    result = initStaticEventListSection( cfg );
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

        if( objPtr->getValueByName( "cyclesPerDay", rstStr ) == HNC_RESULT_SUCCESS )
        {
            zonePtr->setTargetCyclesPerDay( strtol( rstStr.c_str(), NULL, 0 ) );
        }

        if( objPtr->getValueByName( "secondsMinCycle", rstStr ) == HNC_RESULT_SUCCESS )
        {
            zonePtr->setMinimumCycleTimeSeconds( strtol( rstStr.c_str(), NULL, 0 ) );
        }

        if( objPtr->getValueByName( "swidList", rstStr ) == HNC_RESULT_SUCCESS )
        {
            zonePtr->setSWIDList( rstStr );
        }

    }
          
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationSchedule::readStaticEventListSection( HNodeConfig &cfg )
{
    HNCSection  *secPtr;

    // Aquire a pointer to the "device" section
    cfg.updateSection( "irrStaticEventInfo", &secPtr );

    // Get a list pointer
    HNCObjList *listPtr;
    secPtr->updateList( "eventList", &listPtr );

    for( uint indx = 0; indx < listPtr->size(); indx++ )
    {
        std::string eventID;
        std::string rstStr;
        HNCObj *objPtr;

        if( listPtr->getObjPtr( indx, &objPtr ) != HNC_RESULT_SUCCESS )
            continue;

        // Get the zoneID first, if missing skip the record
        if( objPtr->getValueByName( "eventid", eventID ) != HNC_RESULT_SUCCESS )
        {
            continue;
        }

        // Get the internal reference to the zone.
        HNScheduleStaticEvent *eventPtr = updateEvent( eventID );

        if( objPtr->getValueByName( "type", rstStr ) == HNC_RESULT_SUCCESS )
        {
            eventPtr->setTypeFromStr( rstStr );
        }

        if( objPtr->getValueByName( "startTime", rstStr ) == HNC_RESULT_SUCCESS )
        {
            eventPtr->setStartTime( rstStr );
        }

        if( objPtr->getValueByName( "endTime", rstStr ) == HNC_RESULT_SUCCESS )
        {
            eventPtr->setEndTime( rstStr );
        }

        if( objPtr->getValueByName( "dayIndex", rstStr ) == HNC_RESULT_SUCCESS )
        {
            eventPtr->setDayIndexFromNameStr( rstStr );
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

    result = readStaticEventListSection( cfg );
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

        sprintf( tmpStr, "%d", it->second.getTargetCyclesPerDay() );
        objPtr->updateValue( "cyclesPerDay", tmpStr );

        sprintf( tmpStr, "%d", it->second.getMinimumCycleTimeSeconds() );
        objPtr->updateValue( "secondsMinCycle", tmpStr );

        objPtr->updateValue( "swidList", it->second.getSWIDListStr() );    
    }

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationSchedule::updateStaticEventListSection( HNodeConfig &cfg )
{
    char tmpStr[256];
    HNCSection *secPtr;
    cfg.updateSection( "irrStaticEventInfo", &secPtr );
    //secPtr->updateValue( "test1", "value1" );

    HNCObjList *listPtr;
    secPtr->updateList( "eventList", &listPtr );

    for( std::map< std::string, HNScheduleStaticEvent >::iterator it = m_eventMap.begin(); it != m_eventMap.end(); it++ )
    { 
        HNCObj *objPtr;

        // Aquire a new list entry
        listPtr->appendObj( &objPtr );

        // Fill the entry with the static event info
        objPtr->updateValue( "eventid", it->second.getID() );

        objPtr->updateValue( "type", it->second.getTypeStr() );
        objPtr->updateValue( "startTime", it->second.getStartTime().getHMSStr() );
        objPtr->updateValue( "endTime", it->second.getEndTime().getHMSStr() );
        objPtr->updateValue( "dayName", it->second.getDayName() );
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

    result = updateStaticEventListSection( cfg );
    if( result != HNIS_RESULT_SUCCESS )
        return result;

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationSchedule::buildSchedule()
{
    // Clear any schedule data
    for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
        m_dayArr[ indx ].clear();

    // Create Periods for the exclusion specs.
    for( std::map< std::string, HNScheduleStaticEvent >::iterator eit = m_eventMap.begin(); eit != m_eventMap.end(); eit++ )
    {
        HNISPeriod period;

        HNScheduleStaticEvent *curSpec = &(eit->second);

        period.setID( curSpec->getID() );
        period.setStartTime( curSpec->getStartTime() );
        period.setEndTime( curSpec->getEndTime() );

        switch( curSpec->getType() )
        {
            case HNIS_SETYPE_EVERYDAY_KEEPOUT:
            {
                period.setType( HNIS_PERIOD_TYPE_EXCLUSION );

                for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
                    m_dayArr[ indx ].addPeriod( period );
            }
            break;

            case HNIS_SETYPE_SINGLE_KEEPOUT:
                period.setType( HNIS_PERIOD_TYPE_EXCLUSION );

                m_dayArr[ curSpec->getDayIndex() ].addPeriod( period );
            break;
        }
    }

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

std::string 
HNIrrigationSchedule::getSwitchDaemonJSON()
{
    // Open an output stream to the temporary file
    std::stringstream msg;

    // Create a json root object
    pjs::Object jsRoot;

    // Add the timezone name field
    jsRoot.set( "scheduleTimezone", "Americas/Denver" );

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

            jsSWAction.set( "swid", m_zoneMap[ it->getID() ].getSWIDListStr() );

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


