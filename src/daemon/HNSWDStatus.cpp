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

void
HNSWDStatus::setFromSwitchDaemonJSON( std::string jsonStr )
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
        m_schCRC32 = strtol( m_schCRC32Str.c_str(), NULL, 0 );

        pjs::Object::Ptr jsOHealth = jsRoot->getObject( "overallHealth" );
                            
        m_ohstat = jsOHealth->optValue( "status", empty );
        m_ohmsg = jsOHealth->optValue( "msg", empty ); 

        // Map switches to zones
        const std::regex ws_re("\\s+"); // whitespace
        std::string swONStr = jsRoot->optValue( "swOnList", empty );

        m_swON.clear();

        // Walk the switch List string
        std::sregex_token_iterator it( swONStr.begin(), swONStr.end(), ws_re, -1 );
        const std::sregex_token_iterator end;
        while( it != end )
        {
            // Add a new switch id.
            m_swON.insert( *it );
            it++;
        }

    }
    catch( Poco::Exception ex )
    {
        std::cout << "  ERROR: Status JSON not parsable: " << jsonStr << std::endl;
    }

}

bool
HNSWDStatus::getAsRESTJSON( std::ostream &ostr )
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

#if 0
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
#endif

    try
    {
        // Write out the generated json
        pjs::Stringifier::stringify( jsRoot, ostr, 1 );
    }
    catch( ... )
    {
        return true;
    }

    // Success
    return false;
}

