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

class HNISPlacementTracker
{
    public:
      HNISPlacementTracker();
     ~HNISPlacementTracker();
      
      void setStartSec( uint startSec );
      void setDuration( uint durSec );
      void setRank( uint rank );
      void setRoundRobinIndex( uint rri );
      void setMaxCycleDuration( uint durSec );
      
      uint getStartSec();
      uint getDuration();
      uint getRank();
      uint getRoundRobinIndex();
      uint getMaxCycleDuration();
      
      void addZoneSet( std::set<std::string> &srcSet );
      std::set< std::string >& getZoneSetRef();
      uint getZoneCnt();
      bool hasZone( std::string zoneID );

      static bool compareTrackerStartTimes( HNISPlacementTracker t1, HNISPlacementTracker t2 );    
    
    private:
      uint m_startSec;
      uint m_duration;
      uint m_rank;
      uint m_rri;
      uint m_maxCycleDuration;
      
      std::set< std::string > m_zoneSet;

};

HNISPlacementTracker::HNISPlacementTracker()
{

}

HNISPlacementTracker::~HNISPlacementTracker()
{

}

void 
HNISPlacementTracker::setStartSec( uint startSec )
{
    m_startSec = startSec;
}

void 
HNISPlacementTracker::setDuration( uint durSec )
{
    m_duration = durSec;
}

void 
HNISPlacementTracker::setRank( uint rank )
{
    m_rank = rank;
}

void 
HNISPlacementTracker::setRoundRobinIndex( uint rri )
{
    m_rri = rri;
}
   
void 
HNISPlacementTracker::setMaxCycleDuration( uint durSec )
{
    m_maxCycleDuration = durSec;
}
  
uint 
HNISPlacementTracker::getStartSec()
{
    return m_startSec;
}

uint 
HNISPlacementTracker::getDuration()
{
    return m_duration;
}

uint 
HNISPlacementTracker::getRank()
{
    return m_rank;
}

uint 
HNISPlacementTracker::getRoundRobinIndex()
{
    return m_rri;
}
     
uint 
HNISPlacementTracker::getMaxCycleDuration()
{
    return m_maxCycleDuration;
}
 
void 
HNISPlacementTracker::addZoneSet( std::set<std::string> &srcSet )
{
    for( std::set<std::string>::iterator it = srcSet.begin(); it != srcSet.end(); it++ )
    {
        printf( "Tracker zone add: %s\n", it->c_str());
        m_zoneSet.insert( *it );
    }
}

std::set< std::string >& 
HNISPlacementTracker::getZoneSetRef()
{
    return m_zoneSet;
}

uint 
HNISPlacementTracker::getZoneCnt()
{
    return m_zoneSet.size();
}

bool 
HNISPlacementTracker::hasZone( std::string zoneID )
{
    std::set<std::string>::iterator it = m_zoneSet.find( zoneID );
    
    if( it == m_zoneSet.end() )
        return false;
        
    return true;
}

bool 
HNISPlacementTracker::compareTrackerStartTimes( HNISPlacementTracker t1, HNISPlacementTracker t2 )
{
    return (t1.m_startSec < t2.m_startSec);
}

class HNISTimeSlot
{
    public:
      HNISTimeSlot();
     ~HNISTimeSlot();
     
      void setStartSec( uint startSec );
      void setDuration( uint durSec );
      
      uint getStartSec();
      uint getDuration();
    
      void setParent( HNISPlacementTracker *ptPtr );      
      HNISPlacementTracker* getParent();
     
      bool hasZone( std::string zoneID );

    private:
      uint m_startSec;
      uint m_duration;
      
      HNISPlacementTracker* m_srcPtr;      
};

HNISTimeSlot::HNISTimeSlot()
{

}

HNISTimeSlot::~HNISTimeSlot()
{

}
     
void 
HNISTimeSlot::setStartSec( uint startSec )
{
    m_startSec = startSec;
}

void 
HNISTimeSlot::setDuration( uint durSec )
{
    m_duration = durSec;
}
      
uint 
HNISTimeSlot::getStartSec()
{
    return m_startSec;
}

uint 
HNISTimeSlot::getDuration()
{
    return m_duration;
}

void 
HNISTimeSlot::setParent( HNISPlacementTracker *ptPtr )
{
    m_srcPtr = ptPtr;
}

HNISPlacementTracker* 
HNISTimeSlot::getParent()
{
    return m_srcPtr;
}

bool 
HNISTimeSlot::hasZone( std::string zoneID )
{
    return m_srcPtr->hasZone( zoneID );
}

class HNISZoneTracker
{
    public:
      HNISZoneTracker();
     ~HNISZoneTracker();
     
      void setDuration( uint durSec );
      uint getDuration();
      
      void setParent( HNIrrigationZone *zonePtr );
      HNIrrigationZone* getParent();
      
    private:
      uint        m_duration;
      
      HNIrrigationZone* m_srcPtr;
};

HNISZoneTracker::HNISZoneTracker()
{

}

HNISZoneTracker::~HNISZoneTracker()
{

}
     
void 
HNISZoneTracker::setDuration( uint durSec )
{
    m_duration = durSec;
}

uint 
HNISZoneTracker::getDuration()
{
    return m_duration;
}
      
void 
HNISZoneTracker::setParent( HNIrrigationZone *zonePtr )
{
    m_srcPtr = zonePtr;
}

HNIrrigationZone* 
HNISZoneTracker::getParent()
{
    return m_srcPtr;
}

class HNISPlacerDay
{
    public:
      HNISPlacerDay();
     ~HNISPlacerDay();
    
      void setDayIndex( HNIS_DAY_INDX_T dayIndx );
      uint getDayIndex();
      
      HNISPlacementTracker* findAvail( uint rank, uint rrIndx );
    
      HNIS_RESULT_T mapPlacementSpecs( std::vector< HNIrrigationCriteria > &placementList );
      HNIS_RESULT_T calculateZoneMaxCycleDurations( HNIrrigationZoneSet *zones );
      HNIS_RESULT_T generateTimeSlots();
      HNIS_RESULT_T placeZones( std::vector< HNISZoneTracker > &zoneTrackers, HNISchedule &tgtSched );
      
    private:
      HNIS_DAY_INDX_T m_dayIndx;
      
      std::list<HNISTimeSlot> m_slotList;
      std::vector<HNISPlacementTracker> m_availArray;
      std::map<uint, uint> m_rankRRIMap;
      
      uint m_maxrri;
      uint m_maxRank;
      
};

HNISPlacerDay::HNISPlacerDay()
{

}

HNISPlacerDay::~HNISPlacerDay()
{

}

void 
HNISPlacerDay::setDayIndex( HNIS_DAY_INDX_T dayIndx )
{
    m_dayIndx = dayIndx;
}

uint 
HNISPlacerDay::getDayIndex()
{
    return m_dayIndx;
}

HNISPlacementTracker* 
HNISPlacerDay::findAvail( uint rank, uint rrIndx )
{
    for( std::vector<HNISPlacementTracker>::iterator it = m_availArray.begin(); it != m_availArray.end(); it++ )
    {
        if( (rank == it->getRank()) && (rrIndx == it->getRoundRobinIndex()) && (it->getDuration() > 0) )
            return &(*it);
    }
    
    // No match
    return NULL;
}

HNIS_RESULT_T
HNISPlacerDay::mapPlacementSpecs( std::vector< HNIrrigationCriteria > &placementList )
{
    uint rank = 0;
    uint maxRank = 0;

    while(rank < 100)
    {
        // Start Round Robin at zero for each rank level
        uint roundRobinIndex = 0;
        
        // Accumulate the available time blocks from criteria with current rank.
        for( std::vector< HNIrrigationCriteria >::iterator pit = placementList.begin(); pit != placementList.end(); pit++ )
        {
            if( ( pit->isForDay(m_dayIndx) == true ) && ( pit->getRank() == rank ) )
            {
                    HNISPlacementTracker track;
                    track.setStartSec( pit->getStartTime().getSeconds() );
                    track.setDuration( (pit->getEndTime().getSeconds() - pit->getStartTime().getSeconds()) );
                    track.addZoneSet( pit->getZoneSetRef() );
                    track.setRank( pit->getRank() );
                    track.setRoundRobinIndex( roundRobinIndex );  
                    
                    m_availArray.push_back( track );
                    
                    if( m_rankRRIMap.count(rank) )
                        m_rankRRIMap[rank] = (roundRobinIndex+1);
                    else
                        m_rankRRIMap.insert( std::pair<uint,uint>(rank, roundRobinIndex+1) );
                    
                    printf("tins - rank: %d  rri: %d\n", rank, roundRobinIndex+1);
                                       
                    roundRobinIndex += 1;
                    
                    maxRank = rank;
            }
        }

        // Record the maximum Round Robin Index at this rank level
        m_maxrri = roundRobinIndex; 

        // Scan the next rank layer
        rank += 1;
    }
    
    for(std::map<uint, uint>::iterator mip = m_rankRRIMap.begin(); mip != m_rankRRIMap.end(); mip++)
    {
        printf("map - %d : %d\n", mip->first, mip->second);
    }
        
    // Record the maximum rank with actual data
    m_maxRank = maxRank;
        
    // Now sort the m_dayArr
    std::sort( m_availArray.begin(), m_availArray.end(), HNISPlacementTracker::compareTrackerStartTimes );
        
    printf("=== day index: %d  max_rank: %d  max_rri: %d ===\n", m_dayIndx, m_maxRank, m_maxrri); 
    uint idx = 0;
    for( std::vector< HNISPlacementTracker >::iterator it = m_availArray.begin(); it != m_availArray.end(); it++ )
    {
        printf("%d: start: %d  dur: %d  rank: %d  rri: %d\n", idx, it->getStartSec(), it->getDuration(), it->getRank(), it->getRoundRobinIndex());
        idx += 1;
    }
    printf("\n\n");
         
    for( std::map< uint, uint >::iterator it = m_rankRRIMap.begin(); it != m_rankRRIMap.end(); it++ )
    {
        printf("rmap: %d  %d\n", it->first, it->second);
    }
                
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T
HNISPlacerDay::calculateZoneMaxCycleDurations( HNIrrigationZoneSet *zones )
{
    for( std::vector<HNISPlacementTracker>::iterator it = m_availArray.begin(); it != m_availArray.end(); it++ )
    {
        uint tgtSlotSeconds = zones->getMaxCycleTimeForZoneSet( it->getZoneSetRef() );
        it->setMaxCycleDuration( tgtSlotSeconds );
    }
    
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T
HNISPlacerDay::generateTimeSlots()
{
    // Start carving the available space into slots
    uint rank = 0;
    uint rrIndx = 0;
    for( uint rank = 0; rank <= m_maxRank; rank++ )
    {
        uint availCnt = m_rankRRIMap[rank];

        printf("start carve -- rank: %d  availCnt: %d\n", rank, availCnt);
            
        while( availCnt )
        {   
            printf("start carve rr -- rrindx: %d\n", rrIndx);

            // Check if space is available for this rank and rrIndx. 
            HNISPlacementTracker *curAvail = findAvail( rank, rrIndx );
                
            // If no space found, dec the available count, and try again
            if( curAvail == NULL )
            {
                availCnt -= 1;
                  
                rrIndx += 1;
                if( rrIndx >= m_rankRRIMap[rank] )
                    rrIndx = 0;
                continue;  
            }
              
            printf("found %d:%d - start: %d  dur: %d  rank: %d  rri: %d\n", rank, rrIndx, curAvail->getStartSec(), curAvail->getDuration(), curAvail->getRank(), curAvail->getRoundRobinIndex());
              
            // Get the slot size for this space
            uint tgtSlotSeconds = curAvail->getMaxCycleDuration();
            uint avzCnt = curAvail->getZoneCnt();
                
            printf("  tgtSlotSize: %d  avzCnt: %d\n", tgtSlotSeconds, avzCnt);
                
            // Allocate slots for all applicable zones
            for( uint i = 0; i < avzCnt; i++ )
            {
                HNISTimeSlot slot;
                   
                if( curAvail->getDuration() == 0 )
                {
                    availCnt -= 1;
                    break;
                }
                else if( curAvail->getDuration() >= tgtSlotSeconds )
                {
                    slot.setStartSec( curAvail->getStartSec() );
                    slot.setDuration( tgtSlotSeconds );
                    slot.setParent( curAvail );
                    
                    m_slotList.push_back(slot);
                    
                    curAvail->setStartSec( curAvail->getStartSec() + tgtSlotSeconds );
                    curAvail->setDuration( curAvail->getDuration() - tgtSlotSeconds );
                }
                else
                {
                    slot.setStartSec( curAvail->getStartSec() );
                    slot.setDuration( curAvail->getDuration() );
                    slot.setParent( curAvail );
                    
                    m_slotList.push_back(slot);
                    
                    curAvail->setDuration( 0 );
                    availCnt -= 1;
                    break;
                }
            }
                
            // Update Round Robin
            rrIndx += 1;
            if( rrIndx >= m_rankRRIMap[rank] )
              rrIndx = 0;
        }
    }
       
    return HNIS_RESULT_SUCCESS;    
}
   
HNIS_RESULT_T
HNISPlacerDay::placeZones( std::vector< HNISZoneTracker > &zoneTrackers, HNISchedule &tgtSched )
{
    bool progress = false;

    printf( "dayIndx %d\n", m_dayIndx );
            
    // Try to allocate a slot for each zone.
    for( std::vector<HNISZoneTracker>::iterator zit = zoneTrackers.begin(); zit != zoneTrackers.end(); zit++ )
    {
        printf( "check zone %s\n", zit->getParent()->getID().c_str());
                
        // Skip zones that have been fully placed.
        if( zit->getDuration() == 0 )
            continue;
                    
        uint maxCycle = zit->getParent()->getMaximumCycleTimeSeconds();
        uint minCycle = zit->getParent()->getMinimumCycleTimeSeconds();
        uint secNeeded = (zit->getDuration() > maxCycle) ? maxCycle : zit->getDuration();
        std::string zid = zit->getParent()->getID();
                    
        // Attempt to find next slot for this zone
        for( std::list<HNISTimeSlot>::iterator it = m_slotList.begin(); it != m_slotList.end(); it++)
        {
            // Skip used up slots.
            if( it->getDuration() == 0 )
                continue;
                        
            printf( "slot - hasZone %d  secSupply: %d\n", it->hasZone( zid ), it->getDuration() );
            if( (it->hasZone( zid ) == true) && (it->getDuration() >= minCycle) )
            {                        
                printf( "Found slot for %s - totalNeeded: %d secNeeded: %d  secAvail: %d  maxCycle: %d\n", zid.c_str(), zit->getDuration(), secNeeded, it->getDuration(), maxCycle );

                // Check if timeslot is big enough for max placement.
                if( secNeeded <= it->getDuration() )
                {
                    // This will consume all of the time slot exactly.
                    // Add the new slot to the schedule.
                    tgtSched.addPeriodZoneOn( m_dayIndx, zit->getParent()->getID(), it->getStartSec(), secNeeded );
                            
                    // Account for allocated time
                    zit->setDuration( zit->getDuration() - secNeeded );
                    it->setStartSec( it->getStartSec() + secNeeded );
                    it->setDuration( it->getDuration() - secNeeded );
                }
                else if( secNeeded > it->getDuration() )
                {
                    // This will consume all of the time slot exactly.
                    // Add the new slot to the schedule.
                    tgtSched.addPeriodZoneOn( m_dayIndx, zit->getParent()->getID(), it->getStartSec(), it->getDuration() );
                            
                    // Account for allocated time
                    zit->setDuration( zit->getDuration() - it->getDuration() );
                    it->setStartSec( it->getStartSec() + it->getDuration() );
                    it->setDuration( 0 );                        
                }
                                                
                progress = true;
                        
                break;
            }
        }
    }

    // Keep going if we successfully placed a zone
    if( progress == true )
        return HNIS_RESULT_SCH_CONTINUE;

    // Done
    return HNIS_RESULT_SUCCESS;
}
     
// Now generate an ordered list across all days by traversing the individual day list in a specific
// pattern. Order the slots so that they are spread temporally, so that as the 
// amount of watering time goes up/down the watering is still distributed
// roughly evenly over the whole available time period.
static const uint g_insOrdArr[] = { HNIS_DINDX_TUESDAY, HNIS_DINDX_THURSDAY, HNIS_DINDX_MONDAY, HNIS_DINDX_WEDNESDAY, HNIS_DINDX_FRIDAY, HNIS_DINDX_SUNDAY, HNIS_DINDX_SATURDAY};
       
class HNISPlacer
{
    public:
      HNISPlacer();
     ~HNISPlacer();    
    
      HNIS_RESULT_T mapPlacementSpecs( HNIS_DAY_INDX_T dayIndex, std::vector< HNIrrigationCriteria > &placementList );
      HNIS_RESULT_T calculateZoneMaxCycleDurations( HNIS_DAY_INDX_T dayIndex, HNIrrigationZoneSet *zones );
      HNIS_RESULT_T generateTimeSlots( HNIS_DAY_INDX_T dayIndex );

      HNIS_RESULT_T initZoneTracking( std::vector< HNIrrigationZone > &zoneList );
      
      HNIS_RESULT_T placeZones( HNISchedule &tgtSched );
      
    private:
      HNISPlacerDay  m_dayArr[ HNIS_DINDX_NOTSET ];
      
      std::vector< HNISZoneTracker > m_zoneTrackers;
      

};

HNISPlacer::HNISPlacer()
{
    for( uint dayIndx = 0; dayIndx < HNIS_DINDX_NOTSET; dayIndx++ )
    {
        m_dayArr[dayIndx].setDayIndex( (HNIS_DAY_INDX_T)dayIndx );
    }
}

HNISPlacer::~HNISPlacer()
{

}

HNIS_RESULT_T 
HNISPlacer::mapPlacementSpecs( HNIS_DAY_INDX_T dayIndex, std::vector< HNIrrigationCriteria > &placementList )
{
    return m_dayArr[ dayIndex ].mapPlacementSpecs( placementList );
}

HNIS_RESULT_T
HNISPlacer::calculateZoneMaxCycleDurations( HNIS_DAY_INDX_T dayIndex, HNIrrigationZoneSet *zones )
{
    return m_dayArr[ dayIndex ].calculateZoneMaxCycleDurations( zones );
}

HNIS_RESULT_T 
HNISPlacer::generateTimeSlots( HNIS_DAY_INDX_T dayIndex )
{
    return m_dayArr[ dayIndex ].generateTimeSlots();
}

HNIS_RESULT_T
HNISPlacer::initZoneTracking( std::vector< HNIrrigationZone > &zoneList )
{
    // Create a tracker for each zone
    for( std::vector< HNIrrigationZone >::iterator zit = zoneList.begin(); zit != zoneList.end(); zit++ )
    {
        HNISZoneTracker ztrack;
        uint duration = zit->getWeeklySeconds();
        
        ztrack.setDuration( duration );
        ztrack.setParent( &(*zit) );
        
        m_zoneTrackers.push_back(ztrack);
    }  
    
    return HNIS_RESULT_SUCCESS;   
}


HNIS_RESULT_T
HNISPlacer::placeZones( HNISchedule &tgtSched )
{
    HNIS_RESULT_T result;
    
    // Attempt to schedule each zone
    bool progress = true;
    while( progress )
    {
        progress = false;
        
        // Cycle through each day
        for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
        {
            uint dayIndx = g_insOrdArr[ indx ];

            printf( "dayIndx %d\n", dayIndx );
     
            result = m_dayArr[(HNIS_DAY_INDX_T)dayIndx].placeZones( m_zoneTrackers, tgtSched );
            
            if( result == HNIS_RESULT_SCH_CONTINUE )
                progress = true;
            else if( result != HNIS_RESULT_SUCCESS )
                return result;
        }
    }

    return HNIS_RESULT_SUCCESS;
}

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

HNIS_RESULT_T
HNISDay::addPeriodZoneOn( std::string zoneID, uint startSec, uint durationSec )
{
    // Create a new period for the zone on time
    char newID[64];
    HNISPeriod period;

    sprintf( newID, "p%ld", m_periodList.size() );

    // Add a period to represent the overlap region
    period.setID( newID );
    period.setType( HNIS_PERIOD_TYPE_ZONE_ON );
    period.setDayIndex( m_dayIndex );
    period.setStartTimeSeconds( startSec );
    period.setEndTimeSeconds( (startSec + durationSec) );

    period.clearZones();
    period.addZone( zoneID );

    m_periodList.push_back( period );
    
    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNISDay::addPeriod( HNISPeriod value )
{
    std::cout << "addPeriod: " << value.getID() << std::endl;

    m_periodList.push_back( value );
    
    return HNIS_RESULT_SUCCESS;
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

HNISchedule::HNISchedule()
{
    m_schCRC32 = 0;

    for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
        m_dayArr[ indx ].setIndex( (HNIS_DAY_INDX_T) indx );
}

HNISchedule::~HNISchedule()
{

}

void 
HNISchedule::clear()
{
    // Clear any existing data
    for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
        m_dayArr[ indx ].clear();

    m_timezone.clear();
    m_schCRC32 = 0;
}

void 
HNISchedule::setTimezoneStr( std::string tzs )
{
    m_timezone = tzs;
}

std::string 
HNISchedule::getTimezoneStr()
{
    return m_timezone;
}

uint 
HNISchedule::getSMCRC32()
{
    return m_schCRC32;
}

HNIS_RESULT_T
HNISchedule::addPeriodZoneOn( HNIS_DAY_INDX_T dayIndex, std::string zoneID, uint startSec, uint durationSec )
{
    return m_dayArr[ dayIndex ].addPeriodZoneOn( zoneID, startSec, durationSec );
}

HNIS_RESULT_T 
HNISchedule::finalize()
{
    return HNIS_RESULT_SUCCESS;
}
        
std::string 
HNISchedule::getDayName( HNIS_DAY_INDX_T dayIndex )
{
    return m_dayArr[ dayIndex ].getDayName();
}

void 
HNISchedule::getPeriodList( HNIS_DAY_INDX_T dayIndex, std::vector< HNISPeriod > &periodList )
{
    m_dayArr[ dayIndex ].getPeriodList( periodList );
}

void
HNISchedule::calculateSMCRC32()
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
            digest.update( it->getZoneSetAsStr() );
        }
        
    }

    std::cout << "Calculate SMCRC32: " << digest.checksum() << std::endl;

    m_schCRC32 = digest.checksum();
}

void
HNISchedule::debugPrint()
{
    for( int indx = 0; indx < HNIS_DINDX_NOTSET; indx++ )
        m_dayArr[ indx ].debugPrint();
}

HNIrrigationSchedule::HNIrrigationSchedule()
{
    std::cout << "HNIrrigationSchedule -- create" << std::endl;

    m_schedule.setTimezoneStr( "Americas/Denver" );
}

HNIrrigationSchedule::~HNIrrigationSchedule()
{
    std::cout << "HNIrrigationSchedule -- destroy" << std::endl;

}

void
HNIrrigationSchedule::init( HNIrrigationCriteriaSet *criteria, HNIrrigationZoneSet *zones )
{
    std::cout << "HNIrrigationSchedule -- init" << std::endl;

    m_criteria = criteria;
    m_zones = zones;
}

void 
HNIrrigationSchedule::clear()
{
    // Clear any existing data
    m_schedule.clear();
}

std::string
HNIrrigationSchedule::getTimezoneStr()
{
    return m_schedule.getTimezoneStr();
}

uint 
HNIrrigationSchedule::getSMCRC32()
{
    return m_schedule.getSMCRC32();
}

std::string
HNIrrigationSchedule::getSMCRC32Str()
{
    char tmpStr[64];
    sprintf( tmpStr, "0x%x", m_schedule.getSMCRC32() );
    return tmpStr;
}

HNIS_RESULT_T
HNIrrigationSchedule::buildSchedule()
{
    uint rank = 0;
    uint roundRobinIndex = 0;
    uint maxRank = 0;
    bool processCriteria = true;

    HNISPlacer src;
        
    std::cout << "buildSchedule - start" << std::endl;

    // Get criteriaList for later usage.
    std::vector< HNIrrigationCriteria > criteriaList;
    m_criteria->getCriteriaList( criteriaList );

    for( int dayIndx = 0; dayIndx < HNIS_DINDX_NOTSET; dayIndx++ )
    {
        // Accumulate the available time blocks from criteria with current rank.
        src.mapPlacementSpecs( (HNIS_DAY_INDX_T) dayIndx, criteriaList );
        
        // Handle any overlapping placement segments
        //FIXME
        
        src.calculateZoneMaxCycleDurations( (HNIS_DAY_INDX_T) dayIndx, m_zones );
        src.generateTimeSlots( (HNIS_DAY_INDX_T) dayIndx );
    }

    // Create a tracker for each zone
    std::vector< HNIrrigationZone > zoneList;
    m_zones->getZoneList( zoneList );
    
    src.initZoneTracking( zoneList );

    src.placeZones( m_schedule );
    
    m_schedule.debugPrint();

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
        m_schedule.getPeriodList( (HNIS_DAY_INDX_T) indx, periodList );
        //m_dayArr[ indx ].getPeriodList( periodList );

        for( std::vector< HNISPeriod >::iterator it = periodList.begin(); it != periodList.end(); it++ )
        {
            pjs::Object jsSWAction;

            if( it->getType() != HNIS_PERIOD_TYPE_ZONE_ON )
            {
                std::cout << "js continue" << std::endl;                
                continue;
            }
            
            for( std::set< std::string >::iterator zsit = it->getZoneSetRef().begin(); zsit != it->getZoneSetRef().end(); zsit++ )
            {
                std::string zoneid = *zsit;
                
                jsSWAction.set( "action", "on" );
                jsSWAction.set( "startTime", it->getStartTimeStr() );
                jsSWAction.set( "endTime", it->getEndTimeStr() );
                jsSWAction.set( "zoneid", zoneid );

                std::string zName;
                m_zones->getZoneName( zoneid, zName );
                jsSWAction.set( "name", zName );

                jsActions.add( jsSWAction );
            }
        }
        
        jsDays.set( m_schedule.getDayName( (HNIS_DAY_INDX_T) indx ), jsActions );
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
        m_schedule.getPeriodList( (HNIS_DAY_INDX_T) indx, periodList );       
        //m_dayArr[ indx ].getPeriodList( periodList );

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

            HNIrrigationZone zone;
            m_zones->getZone( it->getZoneSetAsStr(), zone );
            jsSWAction.set( "swid", zone.getSWIDListStr() );

            jsActions.add( jsSWAction );
        }
        
        jsDays.set( m_schedule.getDayName( (HNIS_DAY_INDX_T) indx ), jsActions );
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


