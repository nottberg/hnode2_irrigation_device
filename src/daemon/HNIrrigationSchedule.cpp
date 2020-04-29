#include <iostream>

#include "HNIrrigationSchedule.h"

HNExclusionSpec::HNExclusionSpec()
{

}

HNExclusionSpec::~HNExclusionSpec()
{

}

HNIS_EXCLUDE_TYPE_T 
HNExclusionSpec::getType()
{
     return type;
}

HNI24HTime&
HNExclusionSpec::getStartTime()
{
    return startTime;
}

HNI24HTime&
HNExclusionSpec::getEndTime()
{
    return endTime;
}

HNIS_DAY_INDX_T 
HNExclusionSpec::getDayIndex()
{
    return HNIS_DINDX_MONDAY;
}

HNISPeriod::HNISPeriod()
{

}

HNISPeriod::~HNISPeriod()
{

}

void 
HNISPeriod::setType( HNIS_PERIOD_TYPE_T value )
{
    type = value;
}

HNIS_RESULT_T
HNISPeriod::setStartTime( HNI24HTime &time )
{
    startTime = time;
}

HNIS_RESULT_T 
HNISPeriod::setEndTime( HNI24HTime &time )
{
    endTime = time;
}

HNI24HTime& 
HNISPeriod::getStartTime()
{
    return startTime;
}

HNI24HTime& 
HNISPeriod::getEndTime()
{
    return endTime;
}

bool 
HNISPeriod::sortCompare( const HNISPeriod& first, const HNISPeriod& second )
{
  return ( first.startTime.getSeconds() < second.startTime.getSeconds() );
}



HNISDay::HNISDay()
{

}

HNISDay::~HNISDay()
{

}

void 
HNISDay::clear()
{
    periodList.clear();
}

void 
HNISDay::sort()
{
    periodList.sort( HNISPeriod::sortCompare );
}

void 
HNISDay::coalesce()
{
   // Cycle through all periods,
   // Combine exclusions that overlapp

}

HNIS_RESULT_T 
HNISDay::addPeriod( HNISPeriod &value )
{
    periodList.push_back( value );
}

HNIS_CAR_T
HNISDay::assessCollision( HNISPeriod &value, bool moveLater, uint &boundary )
{
     uint insStartSec = value.getStartTime().getSeconds();
     uint insEndSec   = value.getEndTime().getSeconds();

     boundary = 0;

     for( std::list< HNISPeriod >::iterator pit = periodList.begin(); pit != periodList.end(); pit++ )
     {
         uint curStartSec = value.getStartTime().getSeconds();
         uint curEndSec   = value.getEndTime().getSeconds();
       
         if( (insStartSec >= curStartSec) && (insEndSec <= curEndSec) )
         {
             if( moveLater )
                 boundary = curEndSec;
             else
                 boundary = curStartSec;

             return HNIS_CAR_CONTAINED;
         }
         else if( (insStartSec >= curStartSec) && (insEndSec >= curEndSec) && (insStartSec < curEndSec) )
         {
             if( moveLater )
                 boundary = curEndSec;
             else
                 boundary = curStartSec;

             return HNIS_CAR_BEFORE;
         }
         else if( (insStartSec <= curStartSec) && (insEndSec <= curEndSec) && (insEndSec > curStartSec) )
         {
             if( moveLater )
                 boundary = curEndSec;
             else
                 boundary = curStartSec;

             return HNIS_CAR_AFTER;
         }
     }

     return HNIS_CAR_NONE;
}

HNIS_RESULT_T 
HNISDay::scheduleTimeSlots( uint totalSeconds, uint cycleCnt, HNIrrigationZone &zone )
{
    std::vector< uint > cycleMidTimes;

    std::cout << "scheduleTimeSlots - ts: " << totalSeconds << "  cc: " << cycleCnt << "  zone: " << zone.getName() << std::endl;

    uint secPerCycle = totalSeconds / cycleCnt;

    uint halfCycleWidth = (60 * 60 * 24) / (cycleCnt * 2 );

    uint midPoint = halfCycleWidth;
    for( uint indx = 0; indx < cycleCnt; indx++ )
    {
        cycleMidTimes.push_back( midPoint );
        midPoint += (2 * halfCycleWidth );
    }

    sort();

    for( std::vector< uint >::iterator it = cycleMidTimes.begin(); it != cycleMidTimes.end(); it++ )
    {
        bool moveForward = true;
        HNISPeriod zmPeriod;

        zmPeriod.getStartTime().setFromSeconds( *it );
        zmPeriod.getEndTime().setFromSeconds( (*it + secPerCycle) );  

        if( moveForward == true )
        {
            while( zmPeriod.getEndTime().getSeconds() <= (24 * 60 * 60) )
            {
                uint boundary;
                if( assessCollision( zmPeriod, true, boundary  ) == HNIS_CAR_NONE )
                {
                    // Found a spot, add the zone on period

                    // Done looking
                    break;
                }

                // Shift to the next boundary and try again.
                zmPeriod.getStartTime().setFromSeconds( boundary );
                zmPeriod.getEndTime().setFromSeconds( (boundary + secPerCycle) );                  
            }
       }


    }

}

HNIrrigationSchedule::HNIrrigationSchedule()
{

}

HNIrrigationSchedule::~HNIrrigationSchedule()
{

}

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
    for( std::vector< HNIrrigationZone >::iterator zit = zoneList.begin(); zit != zoneList.end(); zit++ )
    {
        // Get the total weekly amount of time
        uint weeklySec = zit->getWeeklySeconds();

        // Get the desired number of cycles per day
        uint cyclePerDay = zit->getTargetCyclesPerDay();

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
                    dayArr[ indx ].scheduleTimeSlots( perDaySec, cyclePerDay, *zit );
                }
            }
            break;
        }
    }

}

