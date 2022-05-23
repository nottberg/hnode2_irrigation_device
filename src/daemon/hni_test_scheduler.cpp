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
        
        void preTestInit( std::string testName );
        
        void addZone( std::string id, std::string name, std::string desc, uint weekSec, uint maxCycleSec, uint minCycleSec, std::string swid );
        void addModifier( std::string id, std::string name, std::string desc, std::string type, std::string value, std::string zoneid );
        
        void test1();
        void test2();
        
        void testUnplacedZone();
};

void
HNIrrigationTest::init()
{
    m_schedule.init( &m_placements, &m_zones, &m_modifiers );
}

void 
HNIrrigationTest::preTestInit( std::string testName )
{
    m_zones.clear();
    m_placements.clear();
    m_modifiers.clear();
    
    std::cout << std::endl << "============== " << testName << " ==============" << std::endl << std::endl;
}

void 
HNIrrigationTest::addZone( std::string id, std::string name, std::string desc, uint weekSec, uint maxCycleSec, uint minCycleSec, std::string swid )
{
    HNIrrigationZone *zone = m_zones.updateZone( id );
    zone->setName( name );
    zone->setDesc( desc );
    zone->setWeeklySeconds( weekSec );
    zone->setMaximumCycleTimeSeconds( maxCycleSec );
    zone->setMinimumCycleTimeSeconds( minCycleSec );
    zone->clearSWIDSet();
    zone->addSWID( swid );
}

void 
HNIrrigationTest::addModifier( std::string id, std::string name, std::string desc, std::string type, std::string value, std::string zoneid )
{
    HNIrrigationModifier  *mod = m_modifiers.updateModifier( id );
    mod->setName( name );
    mod->setDesc( desc );
    mod->setTypeFromStr( type );
    mod->setValue( value );
    mod->setZoneID( zoneid );
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

void
HNIrrigationTest::test2()
{
    // Local pointers
    HNIrrigationPlacement *place;

    
    // Create a zone records
    addZone( "z1", "Front Grass East", "", 2520, 120, 20, "s11" );
    addZone( "z2", "Front Grass West", "", 3360, 240, 20, "s10" );
    addZone( "z3", "Dog Paddock 1", "", 1680, 120, 20, "s17" );
    addZone( "z4", "Dog Paddock 2", "", 1680, 120, 20, "s18" );
    addZone( "z5", "Dog Paddock 3", "", 1680, 120, 20, "s19" );
    addZone( "z6", "Dog Paddock 4", "", 1680, 120, 20, "s20" );
    addZone( "z7", "Back Fence West", "", 1680, 120, 20, "s22" );
    addZone( "z8", "Back Fence East", "", 1680, 120, 20, "s21" );
    addZone( "z9", "Back Pathway West", "", 2520, 120, 20, "s3" );
    addZone( "z10", "Back Pathway East", "", 2520, 120, 20, "s2" );
    addZone( "z11", "North Planter", "", 2520, 120, 20, "s13" );
    addZone( "z12", "East Planter", "", 2520, 120, 20, "s12" );
    addZone( "z13", "East Shrubs", "", 2520, 120, 20, "s16" );
    addZone( "z14", "Southeast Berm", "", 2520, 120, 20, "s14" );
    addZone( "z15", "Southwest Berm", "", 2520, 120, 20, "s15" );
    addZone( "z16", "Garden", "", 2520, 120, 20, "s1" );
    addZone( "z17", "Front Planter", "", 2520, 120, 20, "s9" );

    // Add local modifiers
    addModifier( "m1", "Z1 Manual Adj", "", "local.duration", "0", "z1" );
    addModifier( "m2", "Z2 Manual Adj", "", "local.duration", "0", "z2" );
    addModifier( "m3", "Z3 Manual Adj", "", "local.duration", "0", "z3" );
    addModifier( "m4", "Z4 Manual Adj", "", "local.duration", "0", "z4" );
    addModifier( "m5", "Z5 Manual Adj", "", "local.duration", "0", "z5" );
    addModifier( "m6", "Z6 Manual Adj", "", "local.duration", "0", "z6" );
    addModifier( "m7", "Z7 Manual Adj", "", "local.duration", "0", "z7" );
    addModifier( "m8", "Z8 Manual Adj", "", "local.duration", "0", "z8" );
    addModifier( "m9", "Z9 Manual Adj", "", "local.duration", "0", "z9" );
    addModifier( "m10", "Z10 Manual Adj", "", "local.duration", "0", "z10" );
    addModifier( "m11", "Z11 Manual Adj", "", "local.duration", "0", "z11" );
    addModifier( "m12", "Z12 Manual Adj", "", "local.duration", "0", "z12" );
    addModifier( "m13", "Z13 Manual Adj", "", "local.duration", "0", "z13" );
    addModifier( "m14", "Z14 Manual Adj", "", "local.duration", "0", "z14" );
    addModifier( "m15", "Z15 Manual Adj", "", "local.duration", "0", "z15" );
    addModifier( "m16", "Z16 Manual Adj", "", "local.duration", "0", "z16" );
    addModifier( "m17", "Z17 Manual Adj", "", "local.duration", "0", "z17" );

    // Create a placement record
    place = m_placements.updatePlacement( "e1" );
    place->setName( "Evening" );
    place->setDesc( "" );
    place->setStartTime( "20:00:00" );
    place->setEndTime( "23:00:00" );
    place->setRank( 1 );
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
    place->addZone( "z4" );
    place->addZone( "z5" );
    place->addZone( "z6" );
    place->addZone( "z7" );
    place->addZone( "z8" );
    place->addZone( "z9" );
    place->addZone( "z10" );
    place->addZone( "z11" );
    place->addZone( "z12" );
    place->addZone( "z13" );
    place->addZone( "z14" );
    place->addZone( "z15" );
    place->addZone( "z16" );
    place->addZone( "z17" );
                                                        
    // Create a placement record
    place = m_placements.updatePlacement( "e2" );

    // Fill in zone record fields.
    place->setName( "Morning" );
    place->setDesc( "" );
    place->setStartTime( "03:00:00" );
    place->setEndTime( "06:00:00" );
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
    place->addZone( "z4" );
    place->addZone( "z5" );
    place->addZone( "z6" );
    place->addZone( "z7" );
    place->addZone( "z8" );
    place->addZone( "z9" );
    place->addZone( "z10" );
    place->addZone( "z11" );
    place->addZone( "z12" );
    place->addZone( "z13" );
    place->addZone( "z14" );
    place->addZone( "z15" );
    place->addZone( "z16" );
    place->addZone( "z17" );
      
    // Calculate the new schedule
    HNIS_RESULT_T result = m_schedule.buildSchedule();
}

void
HNIrrigationTest::testUnplacedZone()
{
    // Local pointers
    HNIrrigationPlacement *place;

    // Create a zone records
    addZone( "z1", "Front Grass East", "", 5000, 300, 20, "s11" );
                                                        
    // Create a placement record
    place = m_placements.updatePlacement( "e2" );

    // Fill in zone record fields.
    place->setName( "Test" );
    place->setDesc( "" );
    place->setStartTime( "03:00:00" );
    place->setEndTime( "04:00:00" );
    place->setRank( 1 );
    place->setDayBits(( HNSC_DBITS_TUESDAY | HNSC_DBITS_THURSDAY ));

    place->clearZones();
    place->addZone( "z1" );
      
    // Calculate the new schedule
    HNIS_RESULT_T result = m_schedule.buildSchedule();
}



int 
main( int argc, char* argv[] )
{
    HNIrrigationTest testObj;
    
    std::cout << "=== Start hni_test_scheduler ===" << std::endl;

    testObj.init();
    
    testObj.preTestInit( "test1" );
    testObj.test1();

    testObj.preTestInit( "test2" );
    testObj.test2();

    testObj.preTestInit( "Unplaced Zone Test" );
    testObj.testUnplacedZone();

    std::cout << "=== End hni_test_scheduler ===" << std::endl;
    
    return 0;
    //return swd.run( argc, argv );
}
