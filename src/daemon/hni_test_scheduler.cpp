#include <iostream>

#include "HNIrrigationZone.h"
#include "HNIrrigationCriteria.h"
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
    zone->setWeeklySeconds( 4200 );
    zone->setMaximumCycleTimeSeconds( 300 );
    zone->setMinimumCycleTimeSeconds( 20 );

    zone->clearSWIDSet();
    zone->addSWID( "s1" );

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
