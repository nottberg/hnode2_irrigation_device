#include <iostream>

#include "HNIrrigationZone.h"
#include "HNIrrigationPlacement.h"
#include "HNIrrigationSchedule.h"

class HNIrrigationTest
{
    public:
        HNIrrigationCriteriaSet m_criteria;
        HNIrrigationZoneSet     m_zones;
        HNIrrigationSchedule    m_schedule;

        void init();
        
        void test1();
};

void
HNIrrigationTest::init()
{
    m_schedule.init( &m_criteria, &m_zones );
}

void
HNIrrigationTest::test1()
{
    // Create a zone record
    HNIrrigationZone *zone = m_zones.updateZone( "z1" );

    // Fill in zone record fields.
    zone->setName( "Garden" );
    zone->setDesc( "Garden box drip irrigation" );
    zone->setWeeklySeconds( 5040 );
    zone->setMaximumCycleTimeSeconds( 320 );
    zone->setMinimumCycleTimeSeconds( 20 );

    zone->clearSWIDSet();
    zone->addSWID( "s1" );

    // Create a zone record
    zone = m_zones.updateZone( "z2" );

    // Fill in zone record fields.
    zone->setName( "Front East" );
    zone->setDesc( "Front Grass East" );
    zone->setWeeklySeconds( 2100 );
    zone->setMaximumCycleTimeSeconds( 320 );
    zone->setMinimumCycleTimeSeconds( 20 );

    zone->clearSWIDSet();
    zone->addSWID( "s2" );

    // Create a zone record
    zone = m_zones.updateZone( "z3" );

    // Fill in zone record fields.
    zone->setName( "Front West" );
    zone->setDesc( "Front Grass West" );
    zone->setWeeklySeconds( 6300 );
    zone->setMaximumCycleTimeSeconds( 320 );
    zone->setMinimumCycleTimeSeconds( 20 );

    zone->clearSWIDSet();
    zone->addSWID( "s3" );

    // Create a criteria record
    HNIrrigationCriteria *crit = m_criteria.updateCriteria( "e1" );

    // Fill in zone record fields.
    crit->setName( "Daily1" );
    crit->setDesc( "" );
    crit->setStartTime( "19:00:00" );
    crit->setEndTime( "22:00:00" );
    crit->setRank( 2 );
    crit->setDayBits(( HNSC_DBITS_SUNDAY |
                       HNSC_DBITS_MONDAY |
                       HNSC_DBITS_TUESDAY |
                       HNSC_DBITS_WEDNESDAY |
                       HNSC_DBITS_THURSDAY |
                       HNSC_DBITS_FRIDAY |
                       HNSC_DBITS_SATURDAY ));

    crit->clearZones();
    crit->addZone( "z1" );
    crit->addZone( "z2" );
    crit->addZone( "z3" );
    
    // Create a criteria record
    crit = m_criteria.updateCriteria( "e2" );

    // Fill in zone record fields.
    crit->setName( "Daily2" );
    crit->setDesc( "" );
    crit->setStartTime( "08:00:00" );
    crit->setEndTime( "09:00:00" );
    crit->setRank( 2 );
    crit->setDayBits(( HNSC_DBITS_SUNDAY |
                       HNSC_DBITS_MONDAY |
                       HNSC_DBITS_TUESDAY |
                       HNSC_DBITS_WEDNESDAY |
                       HNSC_DBITS_THURSDAY |
                       HNSC_DBITS_FRIDAY ));

    crit->clearZones();
    crit->addZone( "z1" );
    crit->addZone( "z2" );
    crit->addZone( "z3" );
    
    // Create a criteria record
    crit = m_criteria.updateCriteria( "e3" );

    // Fill in zone record fields.
    crit->setName( "Daily3" );
    crit->setDesc( "" );
    crit->setStartTime( "11:00:00" );
    crit->setEndTime( "13:00:00" );
    crit->setRank( 3 );
    crit->setDayBits(( HNSC_DBITS_SUNDAY |
                       HNSC_DBITS_MONDAY |
                       HNSC_DBITS_TUESDAY |
                       HNSC_DBITS_WEDNESDAY |
                       HNSC_DBITS_THURSDAY |
                       HNSC_DBITS_FRIDAY ));

    crit->clearZones();
    crit->addZone( "z1" );
    crit->addZone( "z2" );
    crit->addZone( "z3" );

    // Calculate the new schedule
    HNIS_RESULT_T result = m_schedule.buildSchedule();

}

int 
main( int argc, char* argv[] )
{
    HNIrrigationTest testObj;
    
    std::cout << "=== Start hni_test_scheduler ===" << std::endl;

    testObj.init();
    
    testObj.test1();

    std::cout << "=== End hni_test_scheduler ===" << std::endl;
    
    return 0;
    //return swd.run( argc, argv );
}
