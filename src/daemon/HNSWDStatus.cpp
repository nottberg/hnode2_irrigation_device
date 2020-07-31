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

void
HNSWDStatus::setFromJSON( std::string jsonStr )
{
    // Parse the response
    try
    {
        std::string empty;
        pjs::Parser parser;

        // Attempt to parse the json
        pdy::Var varRoot = parser.parse( jsonStr );

        // Get a pointer to the root object
        pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

        std::string m_date = jsRoot->optValue( "date", empty );
        std::string m_time = jsRoot->optValue( "time", empty );
        std::string m_tz   = jsRoot->optValue( "timezone", empty );
        std::string m_swON = jsRoot->optValue( "swOnList", empty );

        std::string m_schState = jsRoot->optValue( "schedulerState", empty );
        std::string m_inhUntil = jsRoot->optValue( "inhibitUntil", empty );

        std::string m_schUIStr    = jsRoot->optValue( "scheduleUpdateIndex", empty );

        std::string m_schCRC32Str = jsRoot->optValue( "scheduleCRC32", empty );
        m_schCRC32 = strtol( m_schCRC32Str.c_str(), NULL, 0 );

        pjs::Object::Ptr jsOHealth = jsRoot->getObject( "overallHealth" );
                            
        std::string m_ohstat = jsOHealth->optValue( "status", empty );
        std::string m_ohmsg = jsOHealth->optValue( "msg", empty ); 

        printf( "       Date: %s\n", m_date.c_str() );
        printf( "       Time: %s\n", m_time.c_str() );
        printf( "   Timezone: %s\n\n", m_tz.c_str() );
        printf( "   Schduler State: %s\n", m_schState.c_str() );
        printf( "    Inhibit Until: %s\n", m_inhUntil.c_str() );
        printf( "Schedule Up Index: %s\n", m_schUIStr.c_str() );
        printf( "   Schedule CRC32: 0x%x\n\n", m_schCRC32 );

        printf( "  Switch On: %s\n", m_swON.c_str() );
        printf( "     Health: %s (%s)\n", m_ohstat.c_str(), m_ohmsg.c_str() );
    }
    catch( Poco::Exception ex )
    {
        std::cout << "  ERROR: Status JSON not parsable: " << jsonStr << std::endl;
    }

}

uint 
HNSWDStatus::getSMCRC32()
{
    return m_schCRC32;
}

std::string
HNSWDStatus::getSMCRC32Str()
{
    char tmpStr[64];
    sprintf( tmpStr, "0x%x", m_schCRC32 );
    return tmpStr;
}

bool
HNSWDStatus::healthDegraded()
{
    return false;
}

