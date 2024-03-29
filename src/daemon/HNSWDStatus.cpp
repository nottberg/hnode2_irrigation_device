#include <regex>

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>

#include "HNSWDStatus.h"

namespace pjs = Poco::JSON;
namespace pdy = Poco::Dynamic;

HNSWDStatus::HNSWDStatus()
{

}

HNSWDStatus::~HNSWDStatus()
{

}

uint 
HNSWDStatus::getSMCRC32()
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    return m_schCRC32;
}

std::string
HNSWDStatus::getSMCRC32Str()
{
    char tmpStr[64];

    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    sprintf( tmpStr, "0x%x", m_schCRC32 );
    return tmpStr;
}

bool
HNSWDStatus::healthDegraded()
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    return false;
}

std::string
HNSWDStatus::getSchedulerState()
{
    return m_schState;
}

bool
HNSWDStatus::hasActiveSequence()
{
    return ( m_activeSequenceID.empty() == false );
}

std::string
HNSWDStatus::getActiveSequenceID()
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    return m_activeSequenceID;
}

std::string 
HNSWDStatus::getDateStr()
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    return m_date;
}

std::string 
HNSWDStatus::getTimeStr()
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    return m_time;
}

std::string 
HNSWDStatus::getTimezoneStr()
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    return m_tz;
}

std::string 
HNSWDStatus::getOverallHealthStatus()
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    return m_ohstat;
}

std::string 
HNSWDStatus::getOverallHealthMessage()
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    return m_ohmsg;
}

void 
HNSWDStatus::setSchedulerState( std::string value )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    m_schState = value;
}

void
HNSWDStatus::setFromSwitchDaemonJSON( std::string jsonStr, HNIrrigationZoneSet *zones )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Parse the response
    try
    {
        std::string empty;
        pjs::Parser parser;

        // Attempt to parse the json
        pdy::Var varRoot = parser.parse( jsonStr );

        // Get a pointer to the root object
        pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

        m_date = jsRoot->optValue( "date", empty );
        m_time = jsRoot->optValue( "time", empty );
        m_tz   = jsRoot->optValue( "timezone", empty );

        m_schState = jsRoot->optValue( "schedulerState", empty );
        m_inhUntil = jsRoot->optValue( "inhibitUntil", empty );

        m_schUIStr    = jsRoot->optValue( "scheduleUpdateIndex", empty );

        m_schCRC32Str = jsRoot->optValue( "scheduleCRC32", empty );
        m_schCRC32 = strtoul( m_schCRC32Str.c_str(), NULL, 0 );

        m_activeSequenceID = jsRoot->optValue( "activeSequenceID", empty );

        pjs::Object::Ptr jsOHealth = jsRoot->getObject( "overallHealth" );
                            
        m_ohstat = jsOHealth->optValue( "status", empty );
        m_ohmsg = jsOHealth->optValue( "msg", empty ); 

        // Map switches to zones
        const std::regex ws_re("\\s+"); // whitespace
        std::string swONStr = jsRoot->optValue( "swOnList", empty );

        // Set zone status
        zones->clearAllActive();
        std::sregex_token_iterator it( swONStr.begin(), swONStr.end(), ws_re, -1 );
        const std::sregex_token_iterator end;
        while( it != end )
        {
            zones->setActiveFromSWID( *it );
            it++;
        }

    }
    catch( Poco::Exception ex )
    {
        std::cout << "  ERROR: Status JSON not parsable: " << jsonStr << std::endl;
    }

}

#if 0
HNIS_RESULT_T
HNSWDStatus::getAsIrrigationJSON( std::ostream &ostr, HNIrrigationZoneSet *zones )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Create a json root object
    pjs::Object jsRoot;

    jsRoot.set( "date", m_date );
    jsRoot.set( "time", m_time );
    jsRoot.set( "timezone", m_tz );

    jsRoot.set( "schedulerState", m_schState );
    jsRoot.set( "inhibitUntil", m_inhUntil );

    pjs::Object ovHealth;

    ovHealth.set( "status", m_ohstat );
    ovHealth.set( "msg", m_ohmsg );

    jsRoot.set( "overallHealth", ovHealth );

    pjs::Array activeZones;
    std::vector< HNIrrigationZone > azoneList;
    zones->getActiveZones( azoneList );
    for( std::vector< HNIrrigationZone >::iterator it = azoneList.begin(); it != azoneList.end(); it++ )
    {
        pjs::Object azone;
        azone.set( "id", it->getID() );
        azone.set( "name", it->getName() );
        activeZones.add( azone );
    }
    jsRoot.set( "activeZones", activeZones );

    pjs::Array disabledZones;
    std::vector< HNIrrigationZone > dzoneList;
    zones->getDisabledZones( dzoneList );
    for( std::vector< HNIrrigationZone >::iterator it = dzoneList.begin(); it != dzoneList.end(); it++ )
    {
        pjs::Object dzone;
        dzone.set( "id", it->getID() );
        dzone.set( "name", it->getName() );
        disabledZones.add( dzone );
    }
    jsRoot.set( "disabledZones", disabledZones );

    pjs::Array inhibitedZones;
    std::vector< HNIrrigationZone > izoneList;
    zones->getInhibitedZones( izoneList );
    for( std::vector< HNIrrigationZone >::iterator it = izoneList.begin(); it != izoneList.end(); it++ )
    {
        pjs::Object izone;
        izone.set( "id", it->getID() );
        izone.set( "name", it->getName() );
        izone.set( "until", it->getInhibitedUntil() );
        inhibitedZones.add( izone );
    }
    jsRoot.set( "inhibitedZones", inhibitedZones );

    try
    {
        // Write out the generated json
        pjs::Stringifier::stringify( jsRoot, ostr, 1 );
    }
    catch( Poco::Exception& ex )
    {
        std::cerr << "Stringify Exception: " << ex.displayText() << std::endl;
        return HNIS_RESULT_FAILURE;
    }
    catch( std::exception& ex )
    {
        std::cerr << "Standard Exception: " << ex.what() << std::endl;
        return HNIS_RESULT_FAILURE;
    }
    catch( ... )
    {
        std::cerr << "Stringify Exception: Uncaught Type" << std::endl;
        return HNIS_RESULT_FAILURE;
    }

    // Success
    return HNIS_RESULT_SUCCESS;
}
#endif
