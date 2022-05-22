#include <iostream>

#include "HNIrrigationZone.h"
#include "HNIrrigationPlacement.h"
#include "HNIrrigationSchedule.h"

class HNIrrigationTest
{
    public:
        HNIrrigationPlacementSet m_placements;
        HNIrrigationZoneSet      m_zones;
        HNIrrigationModifierSet  m_modifiers;
        HNIrrigationSchedule     m_schedule;

        void init();
        
        void test1();
};

void
HNIrrigationTest::init()
{
    m_schedule.init( &m_placements, &m_zones, &m_modifiers );
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

    // Create a placement record
    HNIrrigationPlacement *place = m_placements.updatePlacement( "e1" );

    // Fill in zone record fields.
    place->setName( "Daily1" );
    place->setDesc( "" );
    place->setStartTime( "19:00:00" );
    place->setEndTime( "22:00:00" );
    place->setRank( 2 );
    place->setDayBits(( HNSC_DBITS_SUNDAY |
                       HNSC_DBITS_MONDAY |
                       HNSC_DBITS_TUESDAY |
                       HNSC_DBITS_WEDNESDAY |
                       HNSC_DBITS_THURSDAY |
                       HNSC_DBITS_FRIDAY |
                       HNSC_DBITS_SATURDAY ));

    place->clearZones();
    place->addZone( "z1" );
    place->addZone( "z2" );
    place->addZone( "z3" );
    
    // Create a placement record
    place = m_placements.updatePlacement( "e2" );

    // Fill in zone record fields.
    place->setName( "Daily2" );
    place->setDesc( "" );
    place->setStartTime( "08:00:00" );
    place->setEndTime( "09:00:00" );
    place->setRank( 2 );
    place->setDayBits(( HNSC_DBITS_SUNDAY |
                       HNSC_DBITS_MONDAY |
                       HNSC_DBITS_TUESDAY |
                       HNSC_DBITS_WEDNESDAY |
                       HNSC_DBITS_THURSDAY |
                       HNSC_DBITS_FRIDAY ));

    place->clearZones();
    place->addZone( "z1" );
    place->addZone( "z2" );
    place->addZone( "z3" );
    
    // Create a placement record
    place = m_placements.updatePlacement( "e3" );

    // Fill in zone record fields.
    place->setName( "Daily3" );
    place->setDesc( "" );
    place->setStartTime( "11:00:00" );
    place->setEndTime( "13:00:00" );
    place->setRank( 3 );
    place->setDayBits(( HNSC_DBITS_SUNDAY |
                       HNSC_DBITS_MONDAY |
                       HNSC_DBITS_TUESDAY |
                       HNSC_DBITS_WEDNESDAY |
                       HNSC_DBITS_THURSDAY |
                       HNSC_DBITS_FRIDAY ));

    place->clearZones();
    place->addZone( "z1" );
    place->addZone( "z2" );
    place->addZone( "z3" );

    // Create a modifier record
    HNIrrigationModifier *mod = m_modifiers.updateModifier( "m1" );

    // Fill in modifier record fields.
    mod->setName( "Modifier1" );
    mod->setDesc( "" );
    mod->setTypeFromStr( "local.duration" );
    mod->setValue( "1500" );
    mod->setZoneID( "z1" );

    // Create a modifier record
    mod = m_modifiers.updateModifier( "m2" );

    // Fill in modifier record fields.
    mod->setName( "Modifier2" );
    mod->setDesc( "" );
    mod->setTypeFromStr( "local.duration" );
    mod->setValue( "-500" );
    mod->setZoneID( "z2" );

    // Create a modifier record
    mod = m_modifiers.updateModifier( "m3" );

    // Fill in modifier record fields.
    mod->setName( "Modifier3" );
    mod->setDesc( "" );
    mod->setTypeFromStr( "local.percent" );
    mod->setValue( "-50" );
    mod->setZoneID( "z3" );

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
