#include <stdint.h>
#include <iostream>
#include <cstddef>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#include "Poco/Util/Application.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include <Poco/Path.h>
#include <Poco/File.h>
#include <Poco/StreamCopier.h>
#include <Poco/String.h>
#include <Poco/StringTokenizer.h>

#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>

#include "Poco/Net/HTTPClientSession.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/Net/HTTPResponse.h"
#include "Poco/URI.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <iterator>
#include <regex>

using Poco::Util::Application;
using Poco::Util::Option;
using Poco::Util::OptionSet;
using Poco::Util::HelpFormatter;
using Poco::Util::OptionCallback;

namespace pjs = Poco::JSON;
namespace pdy = Poco::Dynamic;
namespace pn  = Poco::Net;

class HNIrrigationClient: public Application
{
    private:
        bool _helpRequested       = false;
        bool _devInfoRequested    = false;

        bool _statusRequested     = false;

        bool _getScheduleRequested = false;
        bool _schstateRequested    = false;

        bool _zoneListRequested    = false;
        bool _createZoneRequested  = false;
        bool _zoneInfoRequested    = false;
        bool _updateZoneRequested  = false;
        bool _deleteZoneRequested  = false;

        bool _listPlacementsRequested   = false;
        bool _createPlacementRequested = false;
        bool _infoPlacementRequested   = false;
        bool _updatePlacementRequested = false;
        bool _deletePlacementRequested = false;

        bool _listModifiersRequested   = false;
        bool _createModifierRequested = false;
        bool _infoModifierRequested   = false;
        bool _updateModifierRequested = false;
        bool _deleteModifierRequested = false;

        bool _switchListRequested = false;

        bool _zonectlRequested    = false;

        bool _hostPresent         = false;
        bool _namePresent         = false;
        bool _descPresent         = false;
        bool _spwPresent          = false;
        bool _sxcPresent          = false;
        bool _smcPresent          = false;
        bool _idPresent           = false;

        bool _stPresent           = false;
        bool _etPresent           = false;
        bool _rankPresent         = false;

        bool _inhibitDurationPresent = false;
        bool _onDurationPresent      = false;
        bool _offDurationPresent     = false;
        
        bool _swidPresent         = false;        
        bool _zoneidPresent       = false;
        bool _dayListPresent      = false;

        bool _modtypeDurationPresent = false;
        bool _modtypePercentPresent  = false;

        std::string _hostStr;
        std::string _nameStr;
        std::string _descStr;
        std::string _idStr;
        std::string _startTimeStr;
        std::string _endTimeStr;
        std::string _inhibitDurationStr;
        std::string _schstateNewState;
        std::string _zonectlCmd;
        std::string _onDurationStr;
        std::string _offDurationStr;
        
        std::string _swidStr;        
        std::string _zoneidStr;
        std::string _dayListStr;
        
        std::string _modtypeValueStr;
        
        uint _spwInt;
        uint _sxcInt;
        uint _smcInt;
        uint _rankInt;

        std::string m_host;
        uint16_t    m_port;

    public:
	    HNIrrigationClient()
	    {
        }

    protected:	
	    void initialize( Application& self )
	    {
		    //loadConfiguration(); 
		    Application::initialize( self );

		    // add your own initialization code here
	    }
	
        void uninitialize()
        {
		    // add your own uninitialization code here

            Application::uninitialize();
        }
	
        void reinitialize( Application& self )
        {
            Application::reinitialize( self );
            
            // add your own reinitialization code here
        }
	
        void defineOptions( OptionSet& options )
        {
            Application::defineOptions( options );

            options.addOption( Option("status", "", "Query the current irrigation device status.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

		    options.addOption( Option("help", "h", "display help information on command line arguments").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleHelp)));

            options.addOption( Option("device-info", "i", "Request Hnode2 Device Info").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("switch-list", "", "Request the switch list").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("schedule", "s", "Request the current schedule").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("schstate", "", "Change the scheduler state. Possible states: enabled|disabled|inhibit. For inhibit the duration parameter is also required.").required(false).repeatable(false).argument("newstate").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("zone-list", "", "Get a list of defined zones.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("create-zone", "", "Create a new zone").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("zone-info", "", "Get info for a single zone.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("update-zone", "", "Update an existing zone").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("delete-zone", "", "Delete an existing zone").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("placements-list", "", "Get a list of defined scheduling placements.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("create-placement", "", "Create a new scheduling placement.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("placement-info", "", "Get info for a single scheduling placement.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("update-placement", "", "Update an existing scheduling placement.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("delete-placement", "", "Delete an existing scheduling placement.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("modifiers-list", "", "Get a list of defined zone modifiers.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("create-modifier", "", "Create a new zone modifier.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("modifier-info", "", "Get info for a single zone modifier.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("update-modifier", "", "Update an existing zone modifier.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("delete-modifier", "", "Delete an existing scheduling placement.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("zonectl", "", "Send a zone control command. Possible commands: .").required(false).repeatable(false).argument("command").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("host", "u", "Host URL").required(false).repeatable(false).argument("<host>:<port>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("name", "", "The name parameter.").required(false).repeatable(false).argument("<name>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("desc", "", "The description parameter").required(false).repeatable(false).argument("<description>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("sec-per-week", "", "The seconds per week parameter").required(false).repeatable(false).argument("<value>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("sec-max-cycle", "", "The maximum seconds per cycle parameter").required(false).repeatable(false).argument("<value>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("sec-min-cycle", "", "The minimum seconds per cycle parameter").required(false).repeatable(false).argument("<value>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("id", "", "Specify an object identifier").required(false).repeatable(false).argument("<value>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("start-time", "", "Specify a start time").required(false).repeatable(false).argument("<HH:MM:SS>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("end-time", "", "Specify an end time").required(false).repeatable(false).argument("<HH:MM:SS>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("rank", "", "Specify the rank integer for a placement.  Placement with the lowest rank will be utilized first.").required(false).repeatable(false).argument("<value>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("inhibitDuration", "", "Duration in HH:MM:SS format.").required(false).repeatable(false).argument("00:00:00").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("onDuration", "", "Duration in HH:MM:SS format.").required(false).repeatable(false).argument("00:00:00").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("offDuration", "", "Duration in HH:MM:SS format.").required(false).repeatable(false).argument("00:00:00").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("swid-list", "", "A list of switch IDs. Multi-entry lists must be space seperated and in quotes.").required(false).repeatable(false).argument("<value>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("zoneid-list", "", "A list of one or more zone ids.  Multi-entry lists must be space seperated and in quotes.").required(false).repeatable(false).argument("z1").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("day-list", "", "A list of day names.  Multi-entry lists must be space seperated and in quotes.").required(false).repeatable(false).argument("<value>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("modtype-duration", "", "Zone modifier type which increments/decrements zone duration by a fixed number of seconds.").required(false).repeatable(false).argument("<value>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("modtype-percent", "", "Zone modifier type which increments/decrements zone duration based on a percentage of zone base duration.").required(false).repeatable(false).argument("<value>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

        }
	
        void handleHelp(const std::string& name, const std::string& value)
        {
            _helpRequested = true;
            displayHelp();
            stopOptionsProcessing();
        }
			
        void handleOptions( const std::string& name, const std::string& value )
        {
            if( "device-info" == name )
                _devInfoRequested = true;
            else if( "status" == name )
                _statusRequested = true;
            else if( "zone-list" == name )
                _zoneListRequested = true;
            else if( "create-zone" == name )
                _createZoneRequested = true;
            else if( "zone-info" == name )
                _zoneInfoRequested = true;
            else if( "update-zone" == name )
                _updateZoneRequested = true;
            else if( "delete-zone" == name )
                _deleteZoneRequested = true;
            else if( "placements-list" == name )
                _listPlacementsRequested = true;
            else if( "create-placement" == name )
                _createPlacementRequested = true;
            else if( "placement-info" == name )
                _infoPlacementRequested = true;
            else if( "update-placement" == name )
                _updatePlacementRequested = true;
            else if( "delete-placement" == name )
                _deletePlacementRequested = true;     
            else if( "modifiers-list" == name )
                _listModifiersRequested = true;
            else if( "create-modifier" == name )
                _createModifierRequested = true;
            else if( "modifier-info" == name )
                _infoModifierRequested = true;
            else if( "update-modifier" == name )
                _updateModifierRequested = true;
            else if( "delete-modifier" == name )
                _deleteModifierRequested = true;          
            else if( "switch-list" == name )
                _switchListRequested = true;
            else if( "schedule" == name )
                _getScheduleRequested = true;
            else if( "schstate" == name )
            {
                _schstateRequested = true;
                _schstateNewState  = value;
            }
            else if( "zonectl" == name )
            {
                _zonectlRequested = true;
                _zonectlCmd       = value;
            }
            else if( "host" == name )
            {
                _hostPresent = true;
                _hostStr     = value;
            }
            else if( "name" == name )
            {
                _namePresent = true;
                _nameStr     = value;
            }
            else if( "desc" == name )
            {
                _descPresent = true;
                _descStr     = value;
            }
            else if( "sec-per-week" == name )
            {
                _spwPresent = true;
                _spwInt     = strtol( value.c_str(), NULL, 0 );
            }
            else if( "sec-max-cycle" == name )
            {
                _sxcPresent = true;
                _sxcInt     = strtol( value.c_str(), NULL, 0 );
            }
            else if( "sec-min-cycle" == name )
            {
                _smcPresent = true;
                _smcInt     = strtol( value.c_str(), NULL, 0 );
            }
            else if( "id" == name )
            {
                _idPresent = true;
                _idStr     = value;
            }
            else if( "start-time" == name )
            {
                _stPresent    = true;
                _startTimeStr = value;
            }
            else if( "end-time" == name )
            {
                _etPresent  = true;
                _endTimeStr = value;
            }
            else if( "rank" == name )
            {
                _rankPresent = true;
                _rankInt     = strtol( value.c_str(), NULL, 0 );
            }
            else if( "inhibitDuration" == name )
            {
                _inhibitDurationPresent = true;
                _inhibitDurationStr     = value;
            }
            else if( "onDuration" == name )
            {
                _onDurationPresent = true;
                _onDurationStr     = value;
            }
            else if( "offDuration" == name )
            {
                _offDurationPresent = true;
                _offDurationStr     = value;
            }
            else if( "swid-list" == name )
            {
                _swidPresent = true;
                _swidStr     = value;
            }            
            else if( "zoneid-list" == name )
            {
                _zoneidPresent = true;
                _zoneidStr     = value;
            }
            else if( "day-list" == name )
            {
                _dayListPresent = true;
                _dayListStr     = value;
            }
            else if( "modtype-duration" == name )
            {
                _modtypeDurationPresent = true;
                _modtypeValueStr        = value;
            }
            else if( "modtype-percent" == name )
            {
                _modtypePercentPresent = true;
                _modtypeValueStr       = value;
            }

        }

        void displayHelp()
        {
            HelpFormatter helpFormatter(options());
            helpFormatter.setCommand(commandName());
            helpFormatter.setUsage("OPTIONS");
            helpFormatter.setHeader("A simple command line client for the hnswitchd daemon.");
            helpFormatter.format(std::cout);
        }

#if 0
        bool openClientSocket( std::string deviceName, std::string instanceName, uint &sockfd )
        {
            struct sockaddr_un addr;
            char str[512];

            // Clear address structure - UNIX domain addressing
            // addr.sun_path[0] cleared to 0 by memset() 
            memset( &addr, 0, sizeof(struct sockaddr_un) );  
            addr.sun_family = AF_UNIX;                     

            // Abstract socket with name @<deviceName>-<instanceName>
            sprintf( str, "hnode2-%s-%s", deviceName.c_str(), instanceName.c_str() );
            strncpy( &addr.sun_path[1], str, strlen(str) );

            // Register the socket
            sockfd = socket( AF_UNIX, SOCK_SEQPACKET, 0 );

            // Establish the connection.
            if( connect( sockfd, (struct sockaddr *) &addr, ( sizeof( sa_family_t ) + strlen( str ) + 1 ) ) == 0 )
            {
                // Success
                printf( "Successfully opened client socket on file descriptor: %d\n", sockfd );
                return false;
            }

            // Failure
            return true;
        }
#endif

        void getHNodeDeviceInfo()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );
            uri.setPath( "/hnode2/device/info" );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            session.sendRequest( request );
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;

            if( response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK )
            {
                return;
            }

            std::string body;
            Poco::StreamCopier::copyToString( rs, body );
            std::cout << body << std::endl;
        }

        void getIrrigationStatus()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );
            uri.setPath( "/hnode2/irrigation/status" );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            session.sendRequest( request );
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;

            if( response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK )
            {
                return;
            }

            std::string body;
            Poco::StreamCopier::copyToString( rs, body );
            std::cout << "Response:" << std::endl << body << std::endl;
        }

        void getScheduleInfo()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );
            uri.setPath( "/hnode2/irrigation/schedule" );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            session.sendRequest( request );
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;

            if( response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK )
            {
                return;
            }

            // Parse the response
            try
            {
                std::string empty;
                std::string body;
                pjs::Parser parser;

                Poco::StreamCopier::copyToString( rs, body );
                std::cout << "== Body ==" << std::endl << body << std::endl;

                // Attempt to parse the json
                pdy::Var varRoot = parser.parse( body );

                // Get a pointer to the root object
                pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

                // Get the schedule matrix element
                pjs::Object::Ptr jsSchedule = jsRoot->getObject( "scheduleMatrix" );

                pjs::Array::Ptr jsSunday    = jsSchedule->getArray( "Sunday" );
                pjs::Array::Ptr jsMonday    = jsSchedule->getArray( "Monday" );
                pjs::Array::Ptr jsTuesday   = jsSchedule->getArray( "Tuesday" );
                pjs::Array::Ptr jsWednesday = jsSchedule->getArray( "Wednesday" );
                pjs::Array::Ptr jsThursday  = jsSchedule->getArray( "Thursday" );
                pjs::Array::Ptr jsFriday    = jsSchedule->getArray( "Friday" );
                pjs::Array::Ptr jsSaturday  = jsSchedule->getArray( "Saturday" );

#define DAYCOL_WIDTH   30
#define TIME_WIDTH     ((2*(8))+1)
#define NAME_WIDTH     (DAYCOL_WIDTH - TIME_WIDTH - 2)

                char tmpBuf[256];

                printf( "%-*.*s|%-*.*s|%-*.*s|%-*.*s\n",
                        DAYCOL_WIDTH, DAYCOL_WIDTH, "Sunday",
                        DAYCOL_WIDTH, DAYCOL_WIDTH, "Monday",
                        DAYCOL_WIDTH, DAYCOL_WIDTH, "Tuesday",
                        DAYCOL_WIDTH, DAYCOL_WIDTH, "Wednesday");

                memset( tmpBuf, '-', sizeof(tmpBuf) );
                tmpBuf[ ((DAYCOL_WIDTH+1) * 4) ] = '\0';
                printf( "%s\n", tmpBuf );

                uint index = 0;
                bool quit = false;
                while( quit == false && index < 100 )
                {
                    quit = true;

                    if( jsSunday && ( index < jsSunday->size() ) )
                    {
                        pjs::Object::Ptr jsCol = jsSunday->getObject( index );
                        sprintf( tmpBuf, "%-*.*s %s-%s ", NAME_WIDTH, NAME_WIDTH, jsCol->optValue( "name", empty ).c_str(), 
                                 jsCol->optValue( "startTime", empty ).c_str(), jsCol->optValue( "endTime", empty ).c_str() );
                        printf( "%-*.*s ", DAYCOL_WIDTH, DAYCOL_WIDTH, tmpBuf );
                        quit = false;
                    }
                    else
                    {
                        printf( "%-*.*s", DAYCOL_WIDTH, DAYCOL_WIDTH, " " );
                    } 

                    if( jsMonday && ( index < jsMonday->size() ) )
                    {
                        pjs::Object::Ptr jsCol = jsMonday->getObject( index );
                        sprintf( tmpBuf, "%-*.*s %s-%s ", NAME_WIDTH, NAME_WIDTH, jsCol->optValue( "name", empty ).c_str(), 
                                 jsCol->optValue( "startTime", empty ).c_str(), jsCol->optValue( "endTime", empty ).c_str() );
                        printf( "%-*.*s ", DAYCOL_WIDTH, DAYCOL_WIDTH, tmpBuf );
                        quit = false;
                    }
                    else
                    {
                        printf( "%-*.*s", DAYCOL_WIDTH, DAYCOL_WIDTH, " " );
                    } 

                    if( jsTuesday && ( index < jsTuesday->size() ) )
                    {
                        pjs::Object::Ptr jsCol = jsTuesday->getObject( index );
                        sprintf( tmpBuf, "%-*.*s %s-%s ", NAME_WIDTH, NAME_WIDTH, jsCol->optValue( "name", empty ).c_str(), 
                                 jsCol->optValue( "startTime", empty ).c_str(), jsCol->optValue( "endTime", empty ).c_str() );
                        printf( "%-*.*s ", DAYCOL_WIDTH, DAYCOL_WIDTH, tmpBuf );
                        quit = false;
                    }
                    else
                    {
                        printf( "%-*.*s", DAYCOL_WIDTH, DAYCOL_WIDTH, " " );
                    } 

                    if( jsWednesday && ( index < jsWednesday->size() ) )
                    {
                        pjs::Object::Ptr jsCol = jsWednesday->getObject( index );
                        sprintf( tmpBuf, "%-*.*s %s-%s ", NAME_WIDTH, NAME_WIDTH, jsCol->optValue( "name", empty ).c_str(), 
                                 jsCol->optValue( "startTime", empty ).c_str(), jsCol->optValue( "endTime", empty ).c_str() );
                        printf( "%-*.*s ", DAYCOL_WIDTH, DAYCOL_WIDTH, tmpBuf );
                        quit = false;
                    }
                    else
                    {
                        printf( "%-*.*s", DAYCOL_WIDTH, DAYCOL_WIDTH, " " );
                    } 
                    
                    index += 1;

                    printf( "\n" );
                    //printf( "%d %d\n", quit, index );
                }


                printf( "\n%-*.*s|%-*.*s|%-*.*s\n",
                        DAYCOL_WIDTH, DAYCOL_WIDTH, "Thursday",
                        DAYCOL_WIDTH, DAYCOL_WIDTH, "Friday",
                        DAYCOL_WIDTH, DAYCOL_WIDTH, "Saturday" );

                memset( tmpBuf, '-', sizeof(tmpBuf) );
                tmpBuf[ ((DAYCOL_WIDTH+1) * 3) ] = '\0';
                printf( "%s\n", tmpBuf );

                index = 0;
                quit = false;
                while( quit == false && index < 100 )
                {
                    quit = true;

                    if( jsThursday && ( index < jsThursday->size() ) )
                    {
                        pjs::Object::Ptr jsCol = jsThursday->getObject( index );
                        sprintf( tmpBuf, "%-*.*s %s-%s ", NAME_WIDTH, NAME_WIDTH, jsCol->optValue( "name", empty ).c_str(), 
                                 jsCol->optValue( "startTime", empty ).c_str(), jsCol->optValue( "endTime", empty ).c_str() );
                        printf( "%-*.*s ", DAYCOL_WIDTH, DAYCOL_WIDTH, tmpBuf );
                        quit = false;
                    }
                    else
                    {
                        printf( "%-*.*s", DAYCOL_WIDTH, DAYCOL_WIDTH, " " );
                    } 

                    if( jsFriday && ( index < jsFriday->size() ) )
                    {
                        pjs::Object::Ptr jsCol = jsFriday->getObject( index );
                        sprintf( tmpBuf, "%-*.*s %s-%s ", NAME_WIDTH, NAME_WIDTH, jsCol->optValue( "name", empty ).c_str(), 
                                 jsCol->optValue( "startTime", empty ).c_str(), jsCol->optValue( "endTime", empty ).c_str() );
                        printf( "%-*.*s ", DAYCOL_WIDTH, DAYCOL_WIDTH, tmpBuf );
                        quit = false;
                    }
                    else
                    {
                        printf( "%-*.*s", DAYCOL_WIDTH, DAYCOL_WIDTH, " " );
                    } 
     
                    if( jsSaturday && ( index < jsSaturday->size() ) )
                    {
                        pjs::Object::Ptr jsCol = jsSaturday->getObject( index );
                        sprintf( tmpBuf, "%-*.*s %s-%s ", NAME_WIDTH, NAME_WIDTH, jsCol->optValue( "name", empty ).c_str(), 
                                 jsCol->optValue( "startTime", empty ).c_str(), jsCol->optValue( "endTime", empty ).c_str() );
                        printf( "%-*.*s ", DAYCOL_WIDTH, DAYCOL_WIDTH, tmpBuf );
                        quit = false;
                    }
                    else
                    {
                        printf( "%-*.*s", DAYCOL_WIDTH, DAYCOL_WIDTH, " " );
                    } 
               
                    index += 1;

                    printf( "\n" );
                    //printf( "%d %d\n", quit, index );
                }
#if 0
{
 "scheduleMatrix" : {
  "Friday" : [
   {
    "action" : "on",
    "endTime" : "00:05:00",
    "startTime" : "00:00:00",
    "zoneid" : "z1"
   },
   {
    "action" : "on",
    "endTime" : "00:10:00",
    "startTime" : "00:05:00",
    "zoneid" : "z2"
   },
   {
    "action" : "on",
    "endTime" : "23:55:00",
    "startTime" : "23:50:00",
    "zoneid" : "z2"
   },
   {
    "action" : "on",
    "endTime" : "24:00:00",
    "startTime" : "23:55:00",
    "zoneid" : "z1"
   }
  ],
  "Monday" : [
   {
    "action" : "on",
    "endTime" : "00:05:00",
    "startTime" : "00:00:00",
    "zoneid" : "z1"
   },
   {
    "action" : "on",
    "endTime" : "00:10:00",
    "startTime" : "00:05:00",
    "zoneid" : "z2"
   },
   {
    "action" : "on",
    "endTime" : "23:55:00",
    "startTime" : "23:50:00",
    "zoneid" : "z2"
   },
   {
    "action" : "on",
    "endTime" : "24:00:00",
    "startTime" : "23:55:00",
    "zoneid" : "z1"
   }
  ],
  "Saturday" : [
   {
    "action" : "on",
    "endTime" : "00:05:00",
    "startTime" : "00:00:00",
    "zoneid" : "z1"
   },
   {
    "action" : "on",
    "endTime" : "00:10:00",
    "startTime" : "00:05:00",
    "zoneid" : "z2"
   },
   {
    "action" : "on",
    "endTime" : "23:55:00",
    "startTime" : "23:50:00",
    "zoneid" : "z2"
   },
   {
    "action" : "on",
    "endTime" : "24:00:00",
    "startTime" : "23:55:00",
    "zoneid" : "z1"
   }
  ],
  "Sunday" : [
   {
    "action" : "on",
    "endTime" : "00:05:00",
    "startTime" : "00:00:00",
    "zoneid" : "z1"
   },
   {
    "action" : "on",
    "endTime" : "00:10:00",
    "startTime" : "00:05:00",
    "zoneid" : "z2"
   },
   {
    "action" : "on",
    "endTime" : "23:55:00",
    "startTime" : "23:50:00",
    "zoneid" : "z2"
   },
   {
    "action" : "on",
    "endTime" : "24:00:00",
    "startTime" : "23:55:00",
    "zoneid" : "z1"
   }
  ],
  "Thursday" : [
   {
    "action" : "on",
    "endTime" : "00:05:00",
    "startTime" : "00:00:00",
    "zoneid" : "z1"
   },
   {
    "action" : "on",
    "endTime" : "00:10:00",
    "startTime" : "00:05:00",
    "zoneid" : "z2"
   },
   {
    "action" : "on",
    "endTime" : "23:55:00",
    "startTime" : "23:50:00",
    "zoneid" : "z2"
   },
   {
    "action" : "on",
    "endTime" : "24:00:00",
    "startTime" : "23:55:00",
    "zoneid" : "z1"
   }
  ],
  "Tuesday" : [
   {
    "action" : "on",
    "endTime" : "00:05:00",
    "startTime" : "00:00:00",
    "zoneid" : "z1"
   },
   {
    "action" : "on",
    "endTime" : "00:10:00",
    "startTime" : "00:05:00",
    "zoneid" : "z2"
   },
   {
    "action" : "on",
    "endTime" : "23:55:00",
    "startTime" : "23:50:00",
    "zoneid" : "z2"
   },
   {
    "action" : "on",
    "endTime" : "24:00:00",
    "startTime" : "23:55:00",
    "zoneid" : "z1"
   }
  ],
  "Wednesday" : [
   {
    "action" : "on",
    "endTime" : "00:05:00",
    "startTime" : "00:00:00",
    "zoneid" : "z1"
   },
   {
    "action" : "on",
    "endTime" : "00:10:00",
    "startTime" : "00:05:00",
    "zoneid" : "z2"
   },
   {
    "action" : "on",
    "endTime" : "23:55:00",
    "startTime" : "23:50:00",
    "zoneid" : "z2"
   },
   {
    "action" : "on",
    "endTime" : "24:00:00",
    "startTime" : "23:55:00",
    "zoneid" : "z1"
   }
  ]
 },
 "scheduleTimezone" : "Americas\/Denver"
}

#endif

#if 0
                std::string date = jsRoot->optValue( "date", empty );
                std::string time = jsRoot->optValue( "time", empty );
                std::string tz   = jsRoot->optValue( "timezone", empty );
                std::string swON = jsRoot->optValue( "swOnList", empty );

                std::string schState = jsRoot->optValue( "schedulerState", empty );
                std::string inhUntil = jsRoot->optValue( "inhibitUntil", empty );

                pjs::Object::Ptr jsOHealth = jsRoot->getObject( "overallHealth" );
                            
                std::string ohstat = jsOHealth->optValue( "status", empty );
                std::string ohmsg = jsOHealth->optValue( "msg", empty ); 

                printf( "       Date: %s\n", date.c_str() );
                printf( "       Time: %s\n", time.c_str() );
                printf( "   Timezone: %s\n\n", tz.c_str() );
                printf( "   Schduler State: %s\n", schState.c_str() );
                printf( "    Inhibit Until: %s\n\n", inhUntil.c_str() );
                printf( "  Switch On: %s\n", swON.c_str() );
                printf( "     Health: %s (%s)\n", ohstat.c_str(), ohmsg.c_str() );
#endif
            }
            catch( Poco::Exception ex )
            {
                std::cout << "  ERROR: Response message not parsable: " << ex.displayText() << std::endl;
            }
        }

        void getZoneList()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );
            uri.setPath( "/hnode2/irrigation/zones" );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            session.sendRequest( request );
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;

            if( response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK )
            {
                return;
            }

            std::string body;
            Poco::StreamCopier::copyToString( rs, body );
            std::cout << body << std::endl;
        }


        void createZoneRequest()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );
            uri.setPath( "/hnode2/irrigation/zones" );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_POST, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            request.setContentType( "application/json" );

            std::ostream& os = session.sendRequest( request );

            // Build the payload message
            // Create a json root object
            pjs::Object jsRoot;
            pjs::Array  jsSwList;

            // Add request data fields
            if( _namePresent )
                jsRoot.set( "name", _nameStr );

            if( _descPresent )
                jsRoot.set( "description", _descStr );

            if( _spwPresent )
                jsRoot.set( "secondsPerWeek", _spwInt );

            if( _sxcPresent )
                jsRoot.set( "secondsMaxCycle", _sxcInt );

            if( _smcPresent )
                jsRoot.set( "secondsMinCycle", _smcInt );

            if( _swidPresent )
            {
                const std::regex ws_re("\\s+"); // whitespace

                // Walk the switch List string
                std::sregex_token_iterator it( _swidStr.begin(), _swidStr.end(), ws_re, -1 );
                const std::sregex_token_iterator end;
                while( it != end )
                {
                    // Add a new switch id.
                    std::string swid = *it;
                    std::cout << "SwitchID: " << swid << std::endl;
                    jsSwList.add( swid );
                    it++;
                }
                jsRoot.set( "swidList", jsSwList );
            }

            // Render into a json string.
            try
            {
                pjs::Stringifier::stringify( jsRoot, os );
            }
            catch( Poco::Exception& ex )
            {
                std::cerr << ex.displayText() << std::endl;
            }
            catch( ... )
            {
                std::cerr << "Unknown exception" << std::endl;
                return;
            }

            // Wait for the response
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;
        }

        void getZoneInfo()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );

            std::string path( "/hnode2/irrigation/zones/" );
            path += _idStr;

            uri.setPath( path );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            session.sendRequest( request );
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;

            if( response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK )
            {
                return;
            }

            std::string body;
            Poco::StreamCopier::copyToString( rs, body );
            std::cout << body << std::endl;
        }

        void updateZoneRequest()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );

            std::string path( "/hnode2/irrigation/zones/" );
            path += _idStr;

            uri.setPath( path );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_PUT, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            request.setContentType( "application/json" );

            std::ostream& os = session.sendRequest( request );

            // Build the payload message
            // Create a json root object
            pjs::Object jsRoot;
            pjs::Array  jsSwList;

            // Add request data fields
            if( _namePresent )
                jsRoot.set( "name", _nameStr );

            if( _descPresent )
                jsRoot.set( "description", _descStr );

            if( _spwPresent )
                jsRoot.set( "secondsPerWeek", _spwInt );

            if( _sxcPresent )
                jsRoot.set( "secondsMaxCycle", _sxcInt );

            if( _smcPresent )
                jsRoot.set( "secondsMinCycle", _smcInt );

            if( _swidPresent )
            {
                const std::regex ws_re("\\s+"); // whitespace

                // Walk the switch List string
                std::sregex_token_iterator it( _swidStr.begin(), _swidStr.end(), ws_re, -1 );
                const std::sregex_token_iterator end;
                while( it != end )
                {
                    // Add a new switch id.
                    std::string swid = *it;
                    std::cout << "SwitchID: " << swid << std::endl;
                    jsSwList.add( swid );
                    it++;
                }
                jsRoot.set( "swidList", jsSwList );
            }

            // Render into a json string.
            try
            {
                pjs::Stringifier::stringify( jsRoot, os );
            }
            catch( Poco::Exception& ex )
            {
                std::cerr << ex.displayText() << std::endl;
            }
            catch( ... )
            {
                std::cout << "Stringfy exception" << std::endl;
                return;
            }

            // Wait for the response
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;
        }

        void deleteZoneRequest()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );

            std::string path( "/hnode2/irrigation/zones/" );
            path += _idStr;

            uri.setPath( path );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_DELETE, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            session.sendRequest( request );

            // Wait for the response
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;
        }

        void getPlacementsList()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );
            uri.setPath( "/hnode2/irrigation/placement" );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            session.sendRequest( request );
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;

            if( response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK )
            {
                return;
            }

            std::string body;
            Poco::StreamCopier::copyToString( rs, body );
            std::cout << body << std::endl;
        }


        void createPlacementRequest()
        {
            Poco::URI uri;

            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );
            uri.setPath( "/hnode2/irrigation/placement" );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_POST, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            request.setContentType( "application/json" );

            std::ostream& os = session.sendRequest( request );

            // Build the payload message
            // Create a json root object
            pjs::Object jsRoot;
            pjs::Array  jsZoneList;
            pjs::Array  jsDayList;
            
            // Add request data fields
            if( _namePresent )
                jsRoot.set( "name", _nameStr );

            if( _descPresent )
                jsRoot.set( "description", _descStr );

            if( _stPresent )
                jsRoot.set( "startTime", _startTimeStr );

            if( _etPresent )
                jsRoot.set( "endTime", _endTimeStr );

            if( _rankPresent )
                jsRoot.set( "rank", _rankInt );

            if( _zoneidPresent )
            {
                const std::regex ws_re("\\s+"); // whitespace

                // Walk the zone id List string
                std::sregex_token_iterator it( _zoneidStr.begin(), _zoneidStr.end(), ws_re, -1 );
                const std::sregex_token_iterator end;
                while( it != end )
                {
                    // Add a new switch id.
                    std::string zid = *it;
                    std::cout << "ZoneID: " << zid << std::endl;
                    jsZoneList.add( zid );
                    it++;
                }
                jsRoot.set( "zoneList", jsZoneList );
            }
            
            if( _dayListPresent )
            {
                const std::regex ws_re("\\s+"); // whitespace

                // Walk the zone id List string
                std::sregex_token_iterator it( _dayListStr.begin(), _dayListStr.end(), ws_re, -1 );
                const std::sregex_token_iterator end;
                while( it != end )
                {
                    // Add a new switch id.
                    std::string dayName = *it;
                    std::cout << "Day: " << dayName << std::endl;
                    jsDayList.add( dayName );
                    it++;
                }
                jsRoot.set( "dayList", jsDayList );
            }

            // Render into a json string.
            try
            {
                pjs::Stringifier::stringify( jsRoot, os );
            }
            catch( Poco::Exception& ex )
            {
                std::cerr << ex.displayText() << std::endl;
            }
            catch( ... )
            {
                std::cerr << "Unknown exception" << std::endl;
                return;
            }

            // Wait for the response
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;
        }

        void getPlacementInfo()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );

            std::string path( "/hnode2/irrigation/placement/" );
            path += _idStr;

            uri.setPath( path );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            session.sendRequest( request );
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;

            if( response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK )
            {
                return;
            }

            std::string body;
            Poco::StreamCopier::copyToString( rs, body );
            std::cout << body << std::endl;
        }

        void updatePlacementRequest()
        {
            Poco::URI uri;

            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );

            std::string path( "/hnode2/irrigation/placement/" );
            path += _idStr;

            uri.setPath( path );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_PUT, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            request.setContentType( "application/json" );

            std::ostream& os = session.sendRequest( request );

            // Build the payload message
            // Create a json root object
            pjs::Object jsRoot;
            pjs::Array  jsZoneList;
            pjs::Array  jsDayList;
            
            // Add request data fields
            if( _namePresent )
                jsRoot.set( "name", _nameStr );

            if( _descPresent )
                jsRoot.set( "description", _descStr );
            
            if( _stPresent )
                jsRoot.set( "startTime", _startTimeStr );

            if( _etPresent )
                jsRoot.set( "endTime", _endTimeStr );

            if( _rankPresent )
                jsRoot.set( "rank", _rankInt );

            if( _zoneidPresent )
            {
                const std::regex ws_re("\\s+"); // whitespace

                // Walk the zone id List string
                std::sregex_token_iterator it( _zoneidStr.begin(), _zoneidStr.end(), ws_re, -1 );
                const std::sregex_token_iterator end;
                while( it != end )
                {
                    // Add a new switch id.
                    std::string zid = *it;
                    std::cout << "ZoneID: " << zid << std::endl;
                    jsZoneList.add( zid );
                    it++;
                }
                jsRoot.set( "zoneList", jsZoneList );
            }
            
            if( _dayListPresent )
            {
                const std::regex ws_re("\\s+"); // whitespace

                // Walk the zone id List string
                std::sregex_token_iterator it( _dayListStr.begin(), _dayListStr.end(), ws_re, -1 );
                const std::sregex_token_iterator end;
                while( it != end )
                {
                    // Add a new switch id.
                    std::string dayName = *it;
                    std::cout << "Day: " << dayName << std::endl;
                    jsDayList.add( dayName );
                    it++;
                }
                jsRoot.set( "dayList", jsDayList );
            }

            // Render into a json string.
            try
            {
                pjs::Stringifier::stringify( jsRoot, os );
            }
            catch( Poco::Exception& ex )
            {
                std::cerr << ex.displayText() << std::endl;
                return;
            }
            catch( ... )
            {
                std::cerr << "Unknown exception" << std::endl;
                return;
            }

            // Wait for the response
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;
        }

        void deletePlacementRequest()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );

            std::string path( "/hnode2/irrigation/placement/" );
            path += _idStr;

            uri.setPath( path );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_DELETE, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            session.sendRequest( request );

            // Wait for the response
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;
        }

        void getModifiersList()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );
            uri.setPath( "/hnode2/irrigation/modifier" );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            session.sendRequest( request );
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;

            if( response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK )
            {
                return;
            }

            std::string body;
            Poco::StreamCopier::copyToString( rs, body );
            std::cout << body << std::endl;
        }

        void createModifierRequest()
        {
            Poco::URI uri;

            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );
            uri.setPath( "/hnode2/irrigation/modifier" );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_POST, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            request.setContentType( "application/json" );

            std::ostream& os = session.sendRequest( request );

            // Build the payload message
            // Create a json root object
            pjs::Object jsRoot;
            pjs::Array  jsZoneList;
            pjs::Array  jsDayList;
            
            // Add request data fields
            if( _namePresent )
                jsRoot.set( "name", _nameStr );

            if( _descPresent )
                jsRoot.set( "description", _descStr );

            if( _zoneidPresent )
                jsRoot.set( "zoneid", _zoneidStr );
            
            if( _modtypeDurationPresent )
            {
                jsRoot.set( "type", "local.duration" );
                jsRoot.set( "value", _modtypeValueStr );            
            }
            
            if( _modtypePercentPresent )
            {
                jsRoot.set( "type", "local.percent" );
                jsRoot.set( "value", _modtypeValueStr );            
            }
            
            // Render into a json string.
            try
            {
                pjs::Stringifier::stringify( jsRoot, os );
            }
            catch( Poco::Exception& ex )
            {
                std::cerr << ex.displayText() << std::endl;
            }
            catch( ... )
            {
                std::cerr << "Unknown exception" << std::endl;
                return;
            }

            // Wait for the response
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;
        }

        void getModifierInfo()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );

            std::string path( "/hnode2/irrigation/modifier/" );
            path += _idStr;

            uri.setPath( path );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            session.sendRequest( request );
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;

            if( response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK )
            {
                return;
            }

            std::string body;
            Poco::StreamCopier::copyToString( rs, body );
            std::cout << body << std::endl;
        }

        void updateModifierRequest()
        {
            Poco::URI uri;

            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );

            std::string path( "/hnode2/irrigation/modifier/" );
            path += _idStr;

            uri.setPath( path );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_PUT, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            request.setContentType( "application/json" );

            std::ostream& os = session.sendRequest( request );

            // Build the payload message
            // Create a json root object
            pjs::Object jsRoot;
            pjs::Array  jsZoneList;
            pjs::Array  jsDayList;
            
            // Add request data fields
            if( _namePresent )
                jsRoot.set( "name", _nameStr );

            if( _descPresent )
                jsRoot.set( "description", _descStr );

            if( _zoneidPresent )
                jsRoot.set( "zoneid", _zoneidStr );

            if( _modtypeDurationPresent )
            {
                jsRoot.set( "type", "local.duration" );
                jsRoot.set( "value", _modtypeValueStr );            
            }
            
            if( _modtypePercentPresent )
            {
                jsRoot.set( "type", "local.percent" );
                jsRoot.set( "value", _modtypeValueStr );            
            }
            
            // Render into a json string.
            try
            {
                pjs::Stringifier::stringify( jsRoot, os );
            }
            catch( Poco::Exception& ex )
            {
                std::cerr << ex.displayText() << std::endl;
                return;
            }
            catch( ... )
            {
                std::cerr << "Unknown exception" << std::endl;
                return;
            }

            // Wait for the response
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;
        }

        void deleteModifierRequest()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );

            std::string path( "/hnode2/irrigation/modifier/" );
            path += _idStr;

            uri.setPath( path );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_DELETE, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            session.sendRequest( request );

            // Wait for the response
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;
        }

        void getSwitchList()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );
            uri.setPath( "/hnode2/irrigation/switches" );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_GET, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            session.sendRequest( request );
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;

            if( response.getStatus() != Poco::Net::HTTPResponse::HTTP_OK )
            {
                return;
            }

            std::string body;
            Poco::StreamCopier::copyToString( rs, body );
            std::cout << body << std::endl;
        }

        void setSchedulerState()
        {
            std::stringstream msg;

            // Error check the provided parameters
            if(   ( _schstateNewState != "enable" )
               && ( _schstateNewState != "disable" )
               && ( _schstateNewState != "inhibit" ) )
            {
                std::cout << "ERROR: Request scheduling state is not supported: " << _schstateNewState << std::endl;
                return;
            }

            if( ( _schstateNewState == "inhibit" ) && ( _inhibitDurationPresent == false ) )
            {
                std::cout << "ERROR: When requesting the inhibit state a duration must be provided: " << _inhibitDurationStr << std::endl;
                return;
            }
          
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );

            std::string path( "/hnode2/irrigation/schedule/state" );

            uri.setPath( path );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_PUT, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            request.setContentType( "application/json" );

            std::ostream& os = session.sendRequest( request );

            // Build the payload message
            // Create a json root object
            pjs::Object jsRoot;

            // Add the timezone setting
            jsRoot.set( "state", _schstateNewState );

            // Add the current date
            if( _inhibitDurationPresent )
                jsRoot.set( "inhibitDuration", _inhibitDurationStr );
            else
                jsRoot.set( "inhibitDuration", "00:00:00" );

            // Render into a json string.
            try
            {
                pjs::Stringifier::stringify( jsRoot, os );
            }
            catch( ... )
            {
                return;
            }

            // Wait for the response
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;
        }

        void sendZoneControl()
        {
            std::string cmdStr;
            std::stringstream msg;

            // Error check the provided parameters
            if(   ( _zonectlCmd != "start-sequence" )
               && ( _zonectlCmd != "cancel-sequence" ))
            {
                std::cout << "ERROR: Zone Control Request command is not supported: " << _zonectlCmd << std::endl;
                return;
            }

            // Allow massage of command string in future
            cmdStr = _zonectlCmd;

#if 0
            if( ( _schstateNewState == "inhibit" ) && ( _durationPresent == false ) )
            {
                std::cout << "ERROR: When requesting the inhibit state a duration must be provided: " << _durationStr << std::endl;
                return;
            }
#endif          
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );

            std::string path( "/hnode2/irrigation/zonectl" );
            uri.setPath( path );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_PUT, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            request.setContentType( "application/json" );

            std::ostream& os = session.sendRequest( request );

            // Build the payload message
            // Create a json root object
            pjs::Object jsRoot;
            pjs::Array  jsSeqList;

            // Add the timezone setting
            jsRoot.set( "command", cmdStr );

            if( "start-sequence" == cmdStr )
            {
                if( ( _onDurationPresent == false ) || ( _offDurationPresent == false ) || ( _zoneidPresent == false ) )
                {
                    std::cout << "ERROR: Starting a zone sequence requires onDuration, offDuration, and zone-list parameters" << std::endl;
                    return;
                }

                // Add the on duration
                jsRoot.set( "onDuration", _onDurationStr );

                // Add the off duration
                jsRoot.set( "offDuration", _offDurationStr );

                // Add sequence zone array
                const std::regex ws_re("\\s+"); // whitespace

                // Walk the zone id List string
                std::sregex_token_iterator it( _zoneidStr.begin(), _zoneidStr.end(), ws_re, -1 );
                const std::sregex_token_iterator end;
                while( it != end )
                {
                    // Add a new zone id.
                    std::string zid = *it;
                    std::cout << "ZoneID: " << zid << std::endl;
                    jsSeqList.add( zid );
                    it++;
                }

                // Add Switch List field
                jsRoot.set( "zoneSequence", jsSeqList );
            }
#if 0
            // Add the current date
            if( _durationPresent )
                jsRoot.set( "inhibitDuration", _durationStr );
            else
                jsRoot.set( "inhibitDuration", "00:00:00" );
#endif
            // Render into a json string.
            try
            {
                pjs::Stringifier::stringify( jsRoot, os );
            }
            catch( ... )
            {
                return;
            }

            // Wait for the response
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;
        }

        int main( const ArgVec& args )
        {
            uint sockfd = 0;

            // Check if non-default host
            if( _hostPresent == true )
            {
                // Host string provided, break into constiuent parts.
               Poco::StringTokenizer tk(_hostStr, ":", Poco::StringTokenizer::TOK_TRIM | Poco::StringTokenizer::TOK_IGNORE_EMPTY);
                switch( tk.count() )
                {
                   case 0:
                       m_host = "localhost";
                       m_port = 8080;
                   break;

                   case 1:
                       m_host = tk[0];
                       m_port = 8080;
                   break;

                   default:
                       m_host = tk[0];
                       m_port = strtol(tk[1].c_str(), NULL, 0);
                   break;
                }
            }
            else
            {
               // Default the host
               m_host = "localhost";
               m_port = 8080;
            }

            // Bailout if help was requested.
            if( _helpRequested == true )
                return Application::EXIT_OK;
 
            if( _devInfoRequested == true )
            {
                getHNodeDeviceInfo();
            }
            else if( _statusRequested == true )
            {
                getIrrigationStatus();
            }
            else if( _getScheduleRequested == true )
            {
                getScheduleInfo();
            }
            else if( _schstateRequested == true )
            {
                setSchedulerState();
            }
            else if( _zoneListRequested == true )
            {
                getZoneList();
            }
            else if( _createZoneRequested == true )
            {
                createZoneRequest();
            }
            else if( _zoneInfoRequested == true )
            {
                getZoneInfo();
            }
            else if( _updateZoneRequested == true )
            {
                updateZoneRequest();
            }
            else if( _deleteZoneRequested == true )
            {
                deleteZoneRequest();
            }
            else if( _listPlacementsRequested == true )
            {
                getPlacementsList();
            }
            else if( _createPlacementRequested == true )
            {
                createPlacementRequest();
            }
            else if( _infoPlacementRequested == true )
            {
                getPlacementInfo();
            }
            else if( _updatePlacementRequested == true )
            {
                updatePlacementRequest();
            }
            else if( _deletePlacementRequested == true )
            {
                deletePlacementRequest();
            }
            
            else if( _listModifiersRequested == true )
            {
                getModifiersList();
            }
            else if( _createModifierRequested == true )
            {
                createModifierRequest();
            }
            else if( _infoModifierRequested == true )
            {
                getModifierInfo();
            }
            else if( _updateModifierRequested == true )
            {
                updateModifierRequest();
            }
            else if( _deleteModifierRequested == true )
            {
                deleteModifierRequest();
            }
            
            
            else if( _switchListRequested == true )
            {
                getSwitchList();
            }
            else if( _zonectlRequested == true )
            {
                sendZoneControl();
            }


#if 0
            // Establish the connection.
            if( openClientSocket( HN_SWDAEMON_DEVICE_NAME, _instanceStr, sockfd ) == true )
            {
                std::cerr << "ERROR: Could not establish connection to daemon. Exiting..." << std::endl;
                return Application::EXIT_SOFTWARE;
            }

            //           
            if( _resetRequested == true )
            {
                HNSWDPacketClient packet;
                uint32_t length;

                packet.setType( HNSWD_PTYPE_RESET_REQ );

                std::cout << "Sending a RESET request..." << std::endl;

                packet.sendAll( sockfd );

            }
            else if( _statusRequested == true )
            {
                HNSWDPacketClient packet;
                uint32_t length;

                packet.setType( HNSWD_PTYPE_STATUS_REQ );

                std::cout << "Sending a STATUS request..." << std::endl;

                packet.sendAll( sockfd );

            }
            else if( _healthRequested == true )
            {
                HNSWDPacketClient packet;
                uint32_t length;

                packet.setType( HNSWD_PTYPE_HEALTH_REQ );

                std::cout << "Sending a HEALTH request..." << std::endl;

                packet.sendAll( sockfd );
            }
            else if( _swinfoRequested == true )
            {
                HNSWDPacketClient packet;

                packet.setType( HNSWD_PTYPE_SWINFO_REQ );

                std::cout << "Sending a SWITCH INFO request..." << std::endl;

                packet.sendAll( sockfd );
            }
            else if( _seqaddRequested == true )
            {
                std::stringstream msg;

                Poco::Path path( _seqaddFilePath );
                Poco::File file( path );

                if( file.exists() == false || file.isFile() == false )
                {
                    std::cerr << "ERROR: Sequence definition file does not exist: " << path.toString() << std::endl;
                    return Application::EXIT_SOFTWARE;
                }
            
                // Open a stream for reading
                std::ifstream its;
                its.open( path.toString() );

                if( its.is_open() == false )
                {
                    std::cerr << "ERROR: Sequence definition file could not be opened: " << path.toString() << std::endl;
                    return Application::EXIT_SOFTWARE;
                }

                // Invoke the json parser
                try
                {
                    // Attempt to parse the json    
                    pjs::Parser parser;
                    pdy::Var varRoot = parser.parse( its );
                    its.close();

                    // Get a pointer to the root object
                    pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

                    // Write out the generated json
                    pjs::Stringifier::stringify( jsRoot, msg );
                }
                catch( Poco::Exception ex )
                {
                    its.close();
                    std::cerr << "ERROR: Sequence definition file json parsing failure: " << ex.displayText().c_str() << std::endl;
                    return Application::EXIT_SOFTWARE;
                }

                HNSWDPacketClient packet( HNSWD_PTYPE_USEQ_ADD_REQ, HNSWD_RCODE_NOTSET, msg.str() );

                std::cout << "Sending a Uniform Sequence Add request..." << std::endl;

                packet.sendAll( sockfd );
            }
            else if( _seqcancelRequested == true )
            {
                HNSWDPacketClient packet;

                packet.setType( HNSWD_PTYPE_SEQ_CANCEL_REQ );

                std::cout << "Sending a SEQUENCE CANCEL request..." << std::endl;

                packet.sendAll( sockfd );
            }
            else if( _schstateRequested == true )
            {
                std::stringstream msg;

                // Error check the provided parameters
                if(   ( _schstateNewState != "enable" )
                   && ( _schstateNewState != "disable" )
                   && ( _schstateNewState != "inhibit" ) )
                {
                    std::cout << "ERROR: Request scheduling state is not supported: " << _schstateNewState << std::endl;
                    return Application::EXIT_SOFTWARE;
                }

                if( ( _schstateNewState == "inhibit" ) && ( _durationPresent == false ) )
                {
                    std::cout << "ERROR: When requesting the inhibit state a duration must be provided: " << _durationStr << std::endl;
                    return Application::EXIT_SOFTWARE;
                }
          
                // Build the payload message
                // Create a json root object
                pjs::Object jsRoot;

                // Add the timezone setting
                jsRoot.set( "state", _schstateNewState );

                // Add the current date
                if( _durationPresent )
                    jsRoot.set( "inhibitDuration", _durationStr );
                else
                    jsRoot.set( "inhibitDuration", "00:00:00" );

                // Render into a json string.
                try
                {
                    pjs::Stringifier::stringify( jsRoot, msg );
                }
                catch( ... )
                {
                    return Application::EXIT_SOFTWARE;
                }

                // Build the request packet.
                HNSWDPacketClient packet( HNSWD_PTYPE_SCH_STATE_REQ, HNSWD_RCODE_NOTSET, msg.str() );

                std::cout << "Sending a SCHEDULING STATE request..." << std::endl;

                packet.sendAll( sockfd );
            }
            else if( _singleRequested == true )
            {
                std::stringstream msg;

                // Make sure a duration was present
                if( _durationPresent == false )
                {
                    std::cout << "ERROR: When requesting single switch turn-on a duration must be provided" << std::endl;
                    return Application::EXIT_SOFTWARE;
                }

                // Build the json request
                msg << "{";
                msg << " \"seqType\":\"uniform\"";
                msg << ", \"onDuration\":\"" << _durationStr << "\"";
                msg << ", \"offDuration\":\"00:00:00\"";
                msg << ", \"swidList\":\"" << _swID << "\"";
                msg << "}";
            
                HNSWDPacketClient packet( HNSWD_PTYPE_USEQ_ADD_REQ, HNSWD_RCODE_NOTSET, msg.str() );

                std::cout << "Sending a Single Switch Add request..." << std::endl;

                packet.sendAll( sockfd );
            }

            // Listen for packets
            bool quit = false;
            while( quit == false )
            {
                HNSWDPacketClient packet;
                HNSWDP_RESULT_T   result;

                printf( "Waiting for packet reception...\n" );

                // Read the header portion of the packet
                result = packet.rcvHeader( sockfd );
                if( result != HNSWDP_RESULT_SUCCESS )
                {
                    printf( "ERROR: Failed while receiving packet header." );
                    return Application::EXIT_SOFTWARE;
                } 

                // Read any payload portion of the packet
                result = packet.rcvPayload( sockfd );
                if( result != HNSWDP_RESULT_SUCCESS )
                {
                    printf( "ERROR: Failed while receiving packet payload." );
                    return Application::EXIT_SOFTWARE;
                } 

                switch( packet.getType() )
                {
                    case HNSWD_PTYPE_DAEMON_STATUS:
                    {
                        std::string msg;
                        packet.getMsg( msg );
                        std::cout << "=== Daemon Status Recieved - result code: " << packet.getResult() << " ===" << std::endl;

                        // Parse the response
                        try
                        {
                            std::string empty;
                            pjs::Parser parser;

                            // Attempt to parse the json
                            pdy::Var varRoot = parser.parse( msg );

                            // Get a pointer to the root object
                            pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

                            std::string date = jsRoot->optValue( "date", empty );
                            std::string time = jsRoot->optValue( "time", empty );
                            std::string tz   = jsRoot->optValue( "timezone", empty );
                            std::string swON = jsRoot->optValue( "swOnList", empty );

                            std::string schState = jsRoot->optValue( "schedulerState", empty );
                            std::string inhUntil = jsRoot->optValue( "inhibitUntil", empty );

                            pjs::Object::Ptr jsOHealth = jsRoot->getObject( "overallHealth" );
                            
                            std::string ohstat = jsOHealth->optValue( "status", empty );
                            std::string ohmsg = jsOHealth->optValue( "msg", empty ); 

                            printf( "       Date: %s\n", date.c_str() );
                            printf( "       Time: %s\n", time.c_str() );
                            printf( "   Timezone: %s\n\n", tz.c_str() );
                            printf( "   Schduler State: %s\n", schState.c_str() );
                            printf( "    Inhibit Until: %s\n\n", inhUntil.c_str() );
                            printf( "  Switch On: %s\n", swON.c_str() );
                            printf( "     Health: %s (%s)\n", ohstat.c_str(), ohmsg.c_str() );
                        }
                        catch( Poco::Exception ex )
                        {
                            std::cout << "  ERROR: Response message not parsable: " << msg << std::endl;
                        }

                        // Exit if we received the expected response and monitoring wasn't requested.
                        if( _monitorRequested == false )
                            quit = true;
                    }
                    break;

                    case HNSWD_PTYPE_HEALTH_RSP:
                    {
                        std::string msg;

                        // Get the json response string.
                        packet.getMsg( msg );
                        std::cout << "=== Component Health Response Recieved - result code: " << packet.getResult() << " ===" << std::endl;

                        // Parse and format the response
                        try
                        {
                            std::string empty;
                            pjs::Parser parser;

                            // Attempt to parse the json
                            pdy::Var varRoot = parser.parse( msg );

                            // Get a pointer to the root object
                            pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

                            pjs::Object::Ptr jsOHealth = jsRoot->getObject( "overallHealth" );
                            
                            std::string ohstat = jsOHealth->optValue( "status", empty );
                            std::string ohmsg = jsOHealth->optValue( "msg", empty ); 

                            printf( "  Component                    |   Status   |   (Error Code) Message\n" );
                            printf( "  ------------------------------------------------------------------\n" );
                            printf( "  %-29.29s %-12.12s (%s) %s\n", "overall", ohstat.c_str(), "0", ohmsg.c_str() );

                            pjs::Array::Ptr jsCHealth = jsRoot->getArray( "componentHealth" );

                            for( uint index = 0; index < jsCHealth->size(); index++ )
                            {
                                if( jsCHealth->isObject( index ) == false )
                                    continue;
                                
                                pjs::Object::Ptr jsCHObject = jsCHealth->getObject( index );

                                std::string component = jsCHObject->optValue( "component", empty );
                                std::string status    = jsCHObject->optValue( "status", empty );
                                std::string errCode   = jsCHObject->optValue( "errCode", empty );
                                std::string msg       = jsCHObject->optValue( "msg", empty );

                                printf( "  %-29.29s %-12.12s (%s) %s\n", component.c_str(), status.c_str(), errCode.c_str(), msg.c_str() );
                            }
                        }
                        catch( Poco::Exception ex )
                        {
                            std::cout << "  ERROR: Response message not parsable: " << msg << std::endl;
                        }

                        // Exit if we received the expected response and monitoring wasn't requested.
                        if( (_healthRequested == true) && (_monitorRequested == false ) )
                            quit = true;

                    }
                    break;

                    case HNSWD_PTYPE_SWINFO_RSP:
                    {
                        std::string msg;

                        // Get the json response string.
                        packet.getMsg( msg );
                        std::cout << "=== Switch Info Response Recieved - result code: " << packet.getResult() << " ===" << std::endl;

                        // Parse and format the response
                        try
                        {
                            std::string empty;
                            pjs::Parser parser;

                            // Attempt to parse the json
                            pdy::Var varRoot = parser.parse( msg );

                            // Get a pointer to the root object
                            pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

                            pjs::Array::Ptr jsSWList = jsRoot->getArray( "swList" );

                            printf( "  swid                |   Description  \n" );
                            printf( "  -------------------------------------\n" );

                            for( uint index = 0; index < jsSWList->size(); index++ )
                            {
                                if( jsSWList->isObject( index ) == false )
                                    continue;
                                
                                pjs::Object::Ptr jsSWInfo = jsSWList->getObject( index );

                                std::string swid = jsSWInfo->optValue( "swid", empty );
                                std::string desc = jsSWInfo->optValue( "description", empty );

                                printf( "  %-20.20s %s\n", swid.c_str(), desc.c_str() );
                            }
                        }
                        catch( Poco::Exception ex )
                        {
                            std::cout << "  ERROR: Response message not parsable: " << msg << std::endl;
                        }

                        // Exit if we received the expected response and monitoring wasn't requested.
                        if( (_swinfoRequested == true) && (_monitorRequested == false ) )
                            quit = true;

                    }
                    break;

                    case HNSWD_PTYPE_USEQ_ADD_RSP:
                    {
                        std::string msg;
                        packet.getMsg( msg );
                        std::cout << "=== Uniform Sequence Add Response Recieved - result code: " << packet.getResult() << " ===" << std::endl;

                        // Exit if we received the expected response and monitoring wasn't requested.
                        if( ( (_seqaddRequested == true) || (_singleRequested == true) ) && (_monitorRequested == false ) )
                            quit = true;

                    }
                    break;

                    case HNSWD_PTYPE_SEQ_CANCEL_RSP:
                    {
                        std::string msg;
                        packet.getMsg( msg );
                        std::cout << "=== Sequence Cancel Response Recieved - result code: " << packet.getResult() << " ===" << std::endl;

                        // Exit if we received the expected response and monitoring wasn't requested.
                        if( (_seqcancelRequested == true) && (_monitorRequested == false ) )
                            quit = true;

                    }
                    break;

                    case HNSWD_PTYPE_SCH_STATE_RSP:
                    {
                        std::string msg;
                        packet.getMsg( msg );
                        std::cout << "=== Schedule State Response Recieved - result code: " << packet.getResult() << " ===" << std::endl;

                        // Exit if we received the expected response and monitoring wasn't requested.
                        if( (_schstateRequested == true) && (_monitorRequested == false ) )
                            quit = true;

                    }
                    break;

                    default:
                    {
                        std::cout << "Unknown Packet Type -  type: " << packet.getType() << std::endl;
                    }
                    break;
                }

            }

            close( sockfd );
#endif
            return Application::EXIT_OK;
        }

};

POCO_APP_MAIN( HNIrrigationClient )

