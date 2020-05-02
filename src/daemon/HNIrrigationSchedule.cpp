#include <iostream>
#include <sstream>

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>

#include "HNIrrigationSchedule.h"

namespace pjs = Poco::JSON;
namespace pdy = Poco::Dynamic;

// Must match the day index enumeration from 
// the header file.
static const char *gDayNames[] =
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

HNExclusionSpec::HNExclusionSpec()
{

}

HNExclusionSpec::~HNExclusionSpec()
{

}

void 
HNExclusionSpec::setType( HNIS_EXCLUDE_TYPE_T value )
{
    m_type = value;
}

HNIS_RESULT_T 
HNExclusionSpec::setTimesFromStr( std::string startTime, std::string endTime )
{
    m_startTime.parseTime( startTime );
 
    std::cout << "mst: " << m_startTime.getHMSStr() << std::endl;

    m_endTime.parseTime( endTime );
}

HNIS_EXCLUDE_TYPE_T 
HNExclusionSpec::getType()
{
     return m_type;
}

HNI24HTime&
HNExclusionSpec::getStartTime()
{
    return m_startTime;
}

HNI24HTime&
HNExclusionSpec::getEndTime()
{
    return m_endTime;
}

HNIS_DAY_INDX_T 
HNExclusionSpec::getDayIndex()
{
    return HNIS_DINDX_MONDAY;
}

HNISPeriod::HNISPeriod()
{
    m_type = HNIS_PERIOD_TYPE_NOTSET;
    m_zone = NULL;
}

HNISPeriod::~HNISPeriod()
{

}

void 
HNISPeriod::setType( HNIS_PERIOD_TYPE_T value )
{
    m_type = value;
}

void 
HNISPeriod::setZone( HNIrrigationZone *zone )
{
    m_zone = zone;
}

HNIS_RESULT_T
HNISPeriod::setStartTime( HNI24HTime &time )
{
    m_startTime.setFromSeconds( time.getSeconds() );
}

HNIS_RESULT_T 
HNISPeriod::setEndTime( HNI24HTime &time )
{
    m_endTime.setFromSeconds( time.getSeconds() );
}

HNIS_RESULT_T 
HNISPeriod::setTimesFromStr( std::string startTime, std::string endTime )
{
    m_startTime.parseTime( startTime );
    m_endTime.parseTime( endTime );
}

HNIS_PERIOD_TYPE_T 
HNISPeriod::getType()
{
    return m_type;
}

HNIrrigationZone*
HNISPeriod::getZone()
{
    return m_zone;
}

HNI24HTime& 
HNISPeriod::getStartTime()
{
    return m_startTime;
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

std::string 
HNISPeriod::getEndTimeStr()
{
    return m_endTime.getHMSStr();
}

bool 
HNISPeriod::sortCompare( const HNISPeriod& first, const HNISPeriod& second )
{
  return ( first.m_startTime.getSeconds() < second.m_startTime.getSeconds() );
}



HNISDay::HNISDay()
{
    m_dayIndex = HNIS_DAY_CNT;
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
    if( dayIndex > HNIS_DAY_CNT )
        m_dayIndex = HNIS_DAY_CNT;
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
    std::cout << "addPeriod" << std::endl;

    m_periodList.push_back( value );
}

HNIS_CAR_T
HNISDay::assessCollision( HNISPeriod &value, bool moveLater, uint &boundary )
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
             if( moveLater )
                 boundary = curEndSec;
             else
                 boundary = curStartSec;

             std::cout << "assessCol CONTAINED: " << boundary << std::endl;
             return HNIS_CAR_CONTAINED;
         }
         else if( (insStartSec >= curStartSec) && (insEndSec >= curEndSec) && (insStartSec < curEndSec) )
         {
             if( moveLater )
                 boundary = curEndSec;
             else
                 boundary = curStartSec;

             std::cout << "assessCol BEFORE: " << boundary << std::endl;
             return HNIS_CAR_BEFORE;
         }
         else if( (insStartSec <= curStartSec) && (insEndSec <= curEndSec) && (insEndSec > curStartSec) )
         {
             if( moveLater )
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
HNISDay::scheduleTimeSlots( uint totalSeconds, uint cycleCnt, HNIrrigationZone *zone )
{
    std::vector< uint > cycleMidTimes;

    std::cout << "scheduleTimeSlots - ts: " << totalSeconds << "  cc: " << cycleCnt << "  zone: " << zone->getName() << std::endl;

    uint secPerCycle = totalSeconds / cycleCnt;

    uint halfCycleWidth = (60 * 60 * 24) / (cycleCnt * 2 );

    uint midPoint = halfCycleWidth;
    for( uint indx = 0; indx < cycleCnt; indx++ )
    {
        std::cout << "scheduleTimeSlots - addMidpoint: " << midPoint << std::endl;

        cycleMidTimes.push_back( midPoint );
        midPoint += (2 * halfCycleWidth );
    }

    sort();

    for( std::vector< uint >::iterator it = cycleMidTimes.begin(); it != cycleMidTimes.end(); it++ )
    {
        bool moveForward = true;
        HNISPeriod zmPeriod;

        zmPeriod.setType( HNIS_PERIOD_TYPE_ZONE_ON );
        zmPeriod.setZone( zone );
        zmPeriod.getStartTime().setFromSeconds( *it );
        zmPeriod.getEndTime().setFromSeconds( (*it + secPerCycle) );  
        
        std::cout << "==scheduleTimeSlots - midpoint: " << zmPeriod.getStartTime().getSeconds() << "  " << zmPeriod.getEndTime().getSeconds() << "==" << std::endl;

        if( *it >= (12 * 60 * 60) )
        {
            while( zmPeriod.getEndTime().getSeconds() <= (24 * 60 * 60) )
            {
                uint boundary;

                std::cout << "scheduleTimeSlots - collision loop1: " << zmPeriod.getStartTime().getSeconds() << "  " << zmPeriod.getEndTime().getSeconds() << std::endl;

                if( assessCollision( zmPeriod, true, boundary  ) == HNIS_CAR_NONE )
                {
                    // Found a spot, add the zone on period
                    addPeriod( zmPeriod );

                    // Done looking
                    break;
                }

                // Shift to the next boundary and try again.
                zmPeriod.getStartTime().setFromSeconds( boundary );
                zmPeriod.getEndTime().setFromSeconds( (boundary + secPerCycle) );                  

            }
        }
        else 
        {
            while( zmPeriod.getStartTime().getSeconds() > 0 )
            {
                uint boundary;

                std::cout << "scheduleTimeSlots - collision loop2: " << zmPeriod.getStartTime().getSeconds() << "  " << zmPeriod.getEndTime().getSeconds() << std::endl;

                if( assessCollision( zmPeriod, false, boundary  ) == HNIS_CAR_NONE )
                {
                    // Found a spot, add the zone on period
                    addPeriod( zmPeriod );

                    // Done looking
                    break;
                }

                // Shift to the next boundary and try again.
                zmPeriod.getStartTime().setFromSeconds( (boundary - secPerCycle) );
                zmPeriod.getEndTime().setFromSeconds( boundary );                  

            }
       }

       std::cout << "== scheduleTimeSlots - midpoint done ==" << std::endl;


    }

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
    return gDayNames[ m_dayIndex ];
}


HNIrrigationSchedule::HNIrrigationSchedule()
{
    std::cout << "HNIrrigationSchedule -- create" << std::endl;

    for( int indx = 0; indx < HNIS_DAY_CNT; indx++ )
        dayArr[ indx ].setIndex( (HNIS_DAY_INDX_T) indx );

}

HNIrrigationSchedule::~HNIrrigationSchedule()
{
    std::cout << "HNIrrigationSchedule -- destroy" << std::endl;

}

void 
HNIrrigationSchedule::clear()
{
    // Clear any existing data
    for( int indx = 0; indx < HNIS_DAY_CNT; indx++ )
        dayArr[ indx ].clear();
}

HNIS_RESULT_T 
HNIrrigationSchedule::addExclusion( HNExclusionSpec *exclusion )
{
    HNISPeriod period;
    std::string st = period.getStartTimeStr();
    
    // Create a period for the exclusion spec
    switch( exclusion->getType() )
    {
        case HNIS_EXCLUDE_TYPE_EVERYDAY:
        {
            period.setType( HNIS_PERIOD_TYPE_EXCLUSION );
            period.setStartTime( exclusion->getStartTime() );
            period.setEndTime( exclusion->getEndTime() );

            for( int indx = 0; indx < HNIS_DAY_CNT; indx++ )
            {
                dayArr[ indx ].addPeriod( period );
                dayArr[ indx ].sort();
            }

            std::cout << "Add: " << period.getStartTime().getSeconds() << std::endl;
            std::string st = period.getStartTimeStr();
            std::cout << "Add2: " << st << std::endl;
        }
        break;

        case HNIS_EXCLUDE_TYPE_SINGLE:
        {
            period.setType( HNIS_PERIOD_TYPE_EXCLUSION );
            period.setStartTime( exclusion->getStartTime() );
            period.setEndTime( exclusion->getEndTime() );

            dayArr[ exclusion->getDayIndex() ].addPeriod( period );
            dayArr[ exclusion->getDayIndex() ].sort();
        }
        break;
    }

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNIrrigationSchedule::addZone( HNIrrigationZone *zone )
{
    // Schedule a zones time slots.
    // Get the total weekly amount of time
    uint weeklySec = zone->getWeeklySeconds();

    // Get the desired number of cycles per day
    uint cyclePerDay = zone->getTargetCyclesPerDay();

    // Get the minimum time per cycle
    uint minCycleDuration = zone->getMinimumCycleTimeSeconds();

    // Figure out how many days of the week can be supported
    uint perDayMin = minCycleDuration * cyclePerDay;

    uint dayCnt = weeklySec / perDayMin;

    std::cout << "weeklySec: " << weeklySec << std::endl;
    std::cout << "cyclePerDay: " << cyclePerDay << std::endl;
    std::cout << "minCycleDuration: " << minCycleDuration << std::endl;
    std::cout << "perDayMin: " << perDayMin << std::endl;
    std::cout << "dayCnt: " << dayCnt << std::endl;

    if( dayCnt > 7 )
        dayCnt = 7;

    switch( dayCnt )
    {
        // Less than one days worth of time
        case 0:
        break;

        case 1:
        break;

        case 2:
        break;

        case 3:
        break;

        case 4:
        break;

        case 5:
        break;

        case 6:
        break;
 
        // Schedule something on every week day.
        case 7:
        {
            uint perDaySec   = weeklySec / dayCnt;
            uint perCycleSec = perDaySec / cyclePerDay;

            std::cout << "perDaySec: " << perDaySec << std::endl;
            std::cout << "perCycleSec: " << perCycleSec << std::endl;

            for( int indx = 0; indx < HNIS_DAY_CNT; indx++ )
            {
                dayArr[ indx ].scheduleTimeSlots( perDaySec, cyclePerDay, zone );
            }
        }
        break;
    }

    return HNIS_RESULT_SUCCESS;

}

#if 0
HNIS_RESULT_T 
HNIrrigationSchedule::buildSchedule( std::vector< HNExclusionSpec > &excludeList, std::vector< HNIrrigationZone > &zoneList )
{
    // Clear any existing data
    for( int indx = 0; indx < HNIS_DAY_CNT; indx++ )
        dayArr[ indx ].clear();

    // Create Periods for the exclusion specs.
    for( std::vector< HNExclusionSpec >::iterator eit = excludeList.begin(); eit != excludeList.end(); eit++ )
    {
        HNISPeriod period;

        switch( eit->getType() )
        {
            case HNIS_EXCLUDE_TYPE_EVERYDAY:
            {
                period.setType( HNIS_PERIOD_TYPE_EXCLUSION );
                period.setStartTime( eit->getStartTime() );
                period.setEndTime( eit->getEndTime() );

                for( int indx = 0; indx < HNIS_DAY_CNT; indx++ )
                    dayArr[ indx ].addPeriod( period );

                std::cout << "Add: " << period.getStartTime().getSeconds() << std::endl;
                std::cout << "Add2: " << period.getStartTimeStr() << std::endl;

            }
            break;

            case HNIS_EXCLUDE_TYPE_SINGLE:
                period.setType( HNIS_PERIOD_TYPE_EXCLUSION );
                period.setStartTime( eit->getStartTime() );
                period.setEndTime( eit->getEndTime() );

                dayArr[ eit->getDayIndex() ].addPeriod( period );
            break;
        }
    }

    // Sort and Coalesce each day
    for( int indx = 0; indx < HNIS_DAY_CNT; indx++ )
    {
        dayArr[ indx ].sort();
        dayArr[ indx ].coalesce();
    }

    // Schedule a zones time slots.
    for( std::vector< HNIrrigationZone >::iterator zone = zoneList.begin(); zone != zoneList.end(); zone++ )
    {
        // Get the total weekly amount of time
        uint weeklySec = zone->getWeeklySeconds();

        // Get the desired number of cycles per day
        uint cyclePerDay = zone->getTargetCyclesPerDay();

        // Get the minimum time per cycle
        uint minCycleDuration = zit->getMinimumCycleTimeSeconds();

        // Figure out how many days of the week can be supported
        uint perDayMin = minCycleDuration * cyclePerDay;

        uint dayCnt = weeklySec / perDayMin;

        std::cout << "weeklySec: " << weeklySec << std::endl;
        std::cout << "cyclePerDay: " << cyclePerDay << std::endl;
        std::cout << "minCycleDuration: " << minCycleDuration << std::endl;
        std::cout << "perDayMin: " << perDayMin << std::endl;
        std::cout << "dayCnt: " << dayCnt << std::endl;

        if( dayCnt > 7 )
            dayCnt = 7;

        switch( dayCnt )
        {
            // Less than one days worth of time
            case 0:
            break;

            case 1:
            break;

            case 2:
            break;

            case 3:
            break;

            case 4:
            break;

            case 5:
            break;

            case 6:
            break;
 
            // Schedule something on every week day.
            case 7:
            {
                uint perDaySec   = weeklySec / dayCnt;
                uint perCycleSec = perDaySec / cyclePerDay;

                std::cout << "perDaySec: " << perDaySec << std::endl;
                std::cout << "perCycleSec: " << perCycleSec << std::endl;

                for( int indx = 0; indx < HNIS_DAY_CNT; indx++ )
                {
                    dayArr[ indx ].scheduleTimeSlots( perDaySec, cyclePerDay, &(*zit) );
                }
            }
            break;
        }
    }

}
#endif

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

    for( int indx = 0; indx < HNIS_DAY_CNT; indx++ )
    {
        pjs::Array jsActions;

        std::cout << "js chk day: " << indx << std::endl;

        std::vector< HNISPeriod > periodList;
        dayArr[ indx ].getPeriodList( periodList );

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
            jsSWAction.set( "swid", it->getZone()->getSWIDListStr() );

            jsActions.add( jsSWAction );
        }
        
        jsDays.set( dayArr[ indx ].getDayName(), jsActions );
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


