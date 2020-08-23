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

        bool _getScheduleRequested = false;

        bool _zoneListRequested    = false;
        bool _createZoneRequested  = false;
        bool _zoneInfoRequested    = false;
        bool _updateZoneRequested  = false;
        bool _deleteZoneRequested  = false;

        bool _staticEventListRequested   = false;
        bool _createStaticEventRequested = false;
        bool _staticEventInfoRequested   = false;
        bool _updateStaticEventRequested = false;
        bool _deleteStaticEventRequested = false;

        bool _switchListRequested = false;

        bool _hostPresent         = false;
        bool _namePresent         = false;
        bool _descPresent         = false;
        bool _spwPresent          = false;
        bool _cpdPresent          = false;
        bool _smcPresent          = false;
        bool _swidPresent         = false;
        bool _idPresent           = false;

        bool _typePresent         = false;
        bool _stPresent           = false;
        bool _etPresent           = false;
        bool _dayPresent          = false;

        std::string _hostStr;
        std::string _nameStr;
        std::string _descStr;
        std::string _swidStr;
        std::string _idStr;
        std::string _typeStr;
        std::string _startTimeStr;
        std::string _endTimeStr;
        std::string _dayStr;
        uint _spwInt;
        uint _cpdInt;
        uint _smcInt;

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

		    options.addOption( Option("help", "h", "display help information on command line arguments").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleHelp)));

            options.addOption( Option("device-info", "i", "Request Hnode2 Device Info").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("switch-list", "", "Request the switch list").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("schedule", "s", "Request the current schedule").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("zone-list", "", "Get a list of defined zones.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("create-zone", "", "Create a new zone").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("zone-info", "", "Get info for a single zone.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("update-zone", "", "Update an existing zone").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("delete-zone", "", "Delete an existing zone").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("event-list", "", "Get a list of defined Static Schedule Events.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("create-event", "", "Create a new Static Schedule Event. Types: everyday-keepout, single-keepout, everyday-zone, single-zone").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("event-info", "", "Get info for a single Static Schedule Event.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("update-event", "", "Update an existing Static Schedule Event.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("delete-event", "", "Delete an existing Static Schedule Event.").required(false).repeatable(false).callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("host", "u", "Host URL").required(false).repeatable(false).argument("<host>:<port>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("name", "", "The name parameter.").required(false).repeatable(false).argument("<name>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("desc", "", "The description parameter").required(false).repeatable(false).argument("<description>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("sec-per-week", "", "The seconds per week parameter").required(false).repeatable(false).argument("<value>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("cycle-per-day", "", "The cycles per day parameter").required(false).repeatable(false).argument("<value>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("sec-min-cycle", "", "The minimum seconds per cycle parameter").required(false).repeatable(false).argument("<value>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("swid-list", "", "A space seperated list of switch IDs").required(false).repeatable(false).argument("<value>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("id", "", "Specify an object identifier").required(false).repeatable(false).argument("<value>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("type", "", "Specify a type parameter").required(false).repeatable(false).argument("<value>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("start-time", "", "Specify a start time").required(false).repeatable(false).argument("<HH:MM:SS>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("end-time", "", "Specify an end time").required(false).repeatable(false).argument("<HH:MM:SS>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));

            options.addOption( Option("day", "", "Specify a day name parameter").required(false).repeatable(false).argument("<value>").callback(OptionCallback<HNIrrigationClient>(this, &HNIrrigationClient::handleOptions)));
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
            else if( "event-list" == name )
                _staticEventListRequested = true;
            else if( "create-event" == name )
                _createStaticEventRequested = true;
            else if( "event-info" == name )
                _staticEventInfoRequested = true;
            else if( "update-event" == name )
                _updateStaticEventRequested = true;
            else if( "delete-event" == name )
                _deleteStaticEventRequested = true;
            else if( "switch-list" == name )
                _switchListRequested = true;
            else if( "schedule" == name )
                _getScheduleRequested = true;
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
            else if( "cycle-per-day" == name )
            {
                _cpdPresent = true;
                _cpdInt     = strtol( value.c_str(), NULL, 0 );
            }
            else if( "sec-min-cycle" == name )
            {
                _smcPresent = true;
                _smcInt     = strtol( value.c_str(), NULL, 0 );
            }
            else if( "swid-list" == name )
            {
                _swidPresent = true;
                _swidStr     = value;
            }
            else if( "id" == name )
            {
                _idPresent = true;
                _idStr     = value;
            }
            else if( "type" == name )
            {
                _typePresent = true;
                _typeStr     = value;
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
            else if( "day" == name )
            {
                _dayPresent = true;
                _dayStr     = value;
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

            // Add request data fields
            if( _namePresent )
                jsRoot.set( "name", _nameStr );

            if( _descPresent )
                jsRoot.set( "description", _descStr );

            if( _spwPresent )
                jsRoot.set( "secondsPerWeek", _spwInt );

            if( _cpdPresent )
                jsRoot.set( "cyclesPerDay", _cpdInt );

            if( _smcPresent )
                jsRoot.set( "secondsMinCycle", _smcInt );

            if( _swidPresent )
                jsRoot.set( "swidList", _swidStr );

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

            // Add request data fields
            if( _namePresent )
                jsRoot.set( "name", _nameStr );

            if( _descPresent )
                jsRoot.set( "description", _descStr );

            if( _spwPresent )
                jsRoot.set( "secondsPerWeek", _spwInt );

            if( _cpdPresent )
                jsRoot.set( "cyclesPerDay", _cpdInt );

            if( _smcPresent )
                jsRoot.set( "secondsMinCycle", _smcInt );

            if( _swidPresent )
                jsRoot.set( "swidList", _swidStr );

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

        void getStaticEventList()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );
            uri.setPath( "/hnode2/irrigation/schedule/static-events" );

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


        void createStaticEventRequest()
        {
            Poco::URI uri;

            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );
            uri.setPath( "/hnode2/irrigation/schedule/static-events" );

            pn::HTTPClientSession session( uri.getHost(), uri.getPort() );
            pn::HTTPRequest request( pn::HTTPRequest::HTTP_POST, uri.getPathAndQuery(), pn::HTTPMessage::HTTP_1_1 );
            pn::HTTPResponse response;

            request.setContentType( "application/json" );

            std::ostream& os = session.sendRequest( request );

            // Build the payload message
            // Create a json root object
            pjs::Object jsRoot;

            // Add request data fields
            if( _typePresent )
                jsRoot.set( "type", _typeStr );

            if( _stPresent )
                jsRoot.set( "startTime", _startTimeStr );

            if( _etPresent )
                jsRoot.set( "endTime", _endTimeStr );

            if( _dayPresent )
                jsRoot.set( "dayName", _dayStr );

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

        void getStaticEventInfo()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );

            std::string path( "/hnode2/irrigation/schedule/static-events/" );
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

        void updateStaticEventRequest()
        {
            Poco::URI uri;

            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );

            std::string path( "/hnode2/irrigation/schedule/static-events/" );
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

            // Add request data fields
            if( _typePresent )
                jsRoot.set( "type", _typeStr );

            if( _stPresent )
                jsRoot.set( "startTime", _startTimeStr );

            if( _etPresent )
                jsRoot.set( "endTime", _endTimeStr );

            if( _dayPresent )
                jsRoot.set( "dayName", _dayStr );

            // Wait for the response
            std::istream& rs = session.receiveResponse( response );
            std::cout << response.getStatus() << " " << response.getReason() << " " << response.getContentLength() << std::endl;
        }

        void deleteStaticEventRequest()
        {
            Poco::URI uri;
            uri.setScheme( "http" );
            uri.setHost( m_host );
            uri.setPort( m_port );

            std::string path( "/hnode2/irrigation/schedule/static-events/" );
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

        int main( const ArgVec& args )
        {
            uint sockfd = 0;

            // Default the host
            m_host = "localhost";
            m_port = 8080;

            // Bailout if help was requested.
            if( _helpRequested == true )
                return Application::EXIT_OK;
 
            if( _devInfoRequested == true )
            {
                getHNodeDeviceInfo();
            }
            else if( _getScheduleRequested == true )
            {
                getScheduleInfo();
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
            else if( _staticEventListRequested == true )
            {
                getStaticEventList();
            }
            else if( _createStaticEventRequested == true )
            {
                createStaticEventRequest();
            }
            else if( _staticEventInfoRequested == true )
            {
                getStaticEventInfo();
            }
            else if( _updateStaticEventRequested == true )
            {
                updateStaticEventRequest();
            }
            else if( _deleteStaticEventRequested == true )
            {
                deleteStaticEventRequest();
            }
            else if( _switchListRequested == true )
            {
                getSwitchList();
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

