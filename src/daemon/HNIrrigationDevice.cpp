#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/un.h>
#include <sys/socket.h>

#include <syslog.h>

#include <iostream>
#include <sstream>

#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/Option.h"
#include "Poco/Util/OptionSet.h"
#include "Poco/Util/HelpFormatter.h"
#include "Poco/Checksum.h"
#include <Poco/JSON/Object.h>
#include <Poco/JSON/Parser.h>

#include <hnode2/HNodeDevice.h>

//#include "HNSWDPacketDaemon.h"
#include "HNIrrigationDevicePrivate.h"

using namespace Poco::Util;

namespace pjs = Poco::JSON;
namespace pdy = Poco::Dynamic;

const std::string g_HNode2IrrigationRest = R"(
{
  "openapi": "3.0.0",
  "info": {
    "description": "",
    "version": "1.0.0",
    "title": ""
  },
  "paths": {
      "/hnode2/irrigation/switches": {
        "get": {
          "summary": "Get a list of controllable switches.",
          "operationId": "getSwitchList",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "array"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        }
      },

      "/hnode2/irrigation/zones": {
        "get": {
          "summary": "Get information about controlled zones.",
          "operationId": "getZoneList",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        },

        "post": {
          "summary": "Create a new zone association.",
          "operationId": "createZone",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        }
      },

      "/hnode2/irrigation/zones/{zoneid}": {
        "get": {
          "summary": "Get information about a specific zone.",
          "operationId": "getZoneInfo",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        },
        "put": {
          "summary": "Update existing zone settings.",
          "operationId": "updateZone",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        },
        "delete": {
          "summary": "Delete an existing zone.",
          "operationId": "deleteZone",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        }
      },


      "/hnode2/irrigation/schedule": {
        "get": {
          "summary": "Get information about the current zone schedule.",
          "operationId": "getScheduleInfo",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        }
      },


      "/hnode2/irrigation/schedule/static-events": {
        "get": {
          "summary": "Get list of schedule static-events.",
          "operationId": "getStaticEventList",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        },

        "post": {
          "summary": "Create a new schedule static event.",
          "operationId": "createStaticEvent",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        }
      },


      "/hnode2/irrigation/schedule/static-events/{eventid}": {
        "get": {
          "summary": "Get information about a specific static event.",
          "operationId": "getStaticEvent",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        },
        "put": {
          "summary": "Update an existing static event.",
          "operationId": "updateStaticEvent",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        },
        "delete": {
          "summary": "Delete an existing static event.",
          "operationId": "deleteStaticEvent",
          "responses": {
            "200": {
              "description": "successful operation",
              "content": {
                "application/json": {
                  "schema": {
                    "type": "object"
                  }
                }
              }
            },
            "400": {
              "description": "Invalid status value"
            }
          }
        }
      }

    }
}
)";

void 
HNIrrigationDevice::defineOptions( OptionSet& options )
{
    ServerApplication::defineOptions( options );

    options.addOption(
              Option("help", "h", "display help").required(false).repeatable(false));

    options.addOption(
              Option("debug","d", "Enable debug logging").required(false).repeatable(false));

    options.addOption(
              Option("instance", "", "Specify the instance name of this daemon.").required(false).repeatable(false).argument("name"));

}

void 
HNIrrigationDevice::handleOption( const std::string& name, const std::string& value )
{
    ServerApplication::handleOption( name, value );
    if( "help" == name )
        _helpRequested = true;
    else if( "debug" == name )
        _debugLogging = true;
    else if( "instance" == name )
    {
         _instancePresent = true;
         _instance = value;
    }
}

void 
HNIrrigationDevice::displayHelp()
{
    HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("[options]");
    helpFormatter.setHeader("HNode2 Switch Daemon.");
    helpFormatter.format(std::cout);
}

//m_hnodeDev.setName("sp1");

int 
HNIrrigationDevice::main( const std::vector<std::string>& args )
{
    m_instanceName = "default";
    if( _instancePresent == true )
        m_instanceName = _instance;

    m_hnodeDev.setDeviceType( HNODE_IRRIGATION_DEVTYPE );
    m_hnodeDev.setInstance( m_instanceName );

    HNDEndpoint hndEP;

    hndEP.setDispatch( "hnode2Irrigation", this );
    hndEP.setOpenAPIJson( g_HNode2IrrigationRest ); 

    m_hnodeDev.addEndpoint( hndEP );

    std::cout << "Looking for config file" << std::endl;
    
    if( configExists() == false )
    {
        initConfig();
    }

    readConfig();

    // Calculate an initial schedule
    m_schedule.buildSchedule();

    m_hnodeDev.start();

    waitForTerminationRequest();

    return Application::EXIT_OK;
}

bool 
HNIrrigationDevice::configExists()
{
    HNodeConfigFile cfgFile;

    return cfgFile.configExists( HNODE_IRRIGATION_DEVTYPE, m_instanceName );
}

HNID_RESULT_T
HNIrrigationDevice::initConfig()
{
    HNodeConfigFile cfgFile;
    HNodeConfig     cfg;

    m_hnodeDev.initConfigSections( cfg );

    m_schedule.initConfigSections( cfg );

    cfg.debugPrint(2);
    
    std::cout << "Saving config..." << std::endl;
    if( cfgFile.saveConfig( HNODE_IRRIGATION_DEVTYPE, m_instanceName, cfg ) != HNC_RESULT_SUCCESS )
    {
        std::cout << "ERROR: Could not save initial configuration." << std::endl;
        return HNID_RESULT_FAILURE;
    }

    return HNID_RESULT_SUCCESS;
}

HNID_RESULT_T
HNIrrigationDevice::readConfig()
{
    HNodeConfigFile cfgFile;
    HNodeConfig     cfg;

    if( configExists() == false )
        return HNID_RESULT_FAILURE;

    std::cout << "Loading config..." << std::endl;

    if( cfgFile.loadConfig( HNODE_IRRIGATION_DEVTYPE, m_instanceName, cfg ) != HNC_RESULT_SUCCESS )
    {
        std::cout << "ERROR: Could not load saved configuration." << std::endl;
        return HNID_RESULT_FAILURE;
    }
  
    m_hnodeDev.readConfigSections( cfg );

    m_schedule.readConfigSections( cfg );

    return HNID_RESULT_SUCCESS;
}

HNID_RESULT_T
HNIrrigationDevice::updateConfig()
{
    HNodeConfigFile cfgFile;
    HNodeConfig     cfg;

    m_hnodeDev.updateConfigSections( cfg );

    m_schedule.updateConfigSections( cfg );

    cfg.debugPrint(2);
    
    std::cout << "Saving config..." << std::endl;
    if( cfgFile.saveConfig( HNODE_IRRIGATION_DEVTYPE, m_instanceName, cfg ) != HNC_RESULT_SUCCESS )
    {
        std::cout << "ERROR: Could not save configuration." << std::endl;
        return HNID_RESULT_FAILURE;
    }

    return HNID_RESULT_SUCCESS;
}

HNID_RESULT_T
HNIrrigationDevice::updateZone( std::string zoneID, std::istream& bodyStream )
{
    // Parse the json body of the request
    try
    {
        // Attempt to parse the json    
        pjs::Parser parser;
        pdy::Var varRoot = parser.parse( bodyStream );

        // Get a pointer to the root object
        pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

        HNIrrigationZone *zone = m_schedule.updateZone( zoneID );

        if( jsRoot->has( "name" ) )
        {
            zone->setName( jsRoot->getValue<std::string>( "name" ) );
        }

        if( jsRoot->has( "description" ) )
        {
            zone->setDesc( jsRoot->getValue<std::string>( "description" ) );
        }

        if( jsRoot->has( "secondsPerWeek" ) )
        {
            zone->setWeeklySeconds( jsRoot->getValue<uint>( "secondsPerWeek" ) );
        }

        if( jsRoot->has( "cyclesPerDay" ) )
        {
            zone->setTargetCyclesPerDay( jsRoot->getValue<uint>( "cyclePerDay" ) );
        }

        if( jsRoot->has( "secondsMinCycle" ) )
        {
            zone->setMinimumCycleTimeSeconds( jsRoot->getValue<uint>( "secondsMinCycle" ) );
        }

        if( jsRoot->has( "swidList" ) )
        {
            zone->setSWIDList( jsRoot->getValue<std::string>( "swidList" ) );
        }
        
        if( zone->validateSettings() != HNIS_RESULT_SUCCESS )
        {
            std::cout << "updateZone validate failed" << std::endl;
            // zoneid parameter is required
            return HNID_RESULT_BAD_REQUEST;
        }        
    }
    catch( Poco::Exception ex )
    {
        std::cout << "updateZone exception: " << ex.displayText() << std::endl;
        // Request body was not understood
        return HNID_RESULT_BAD_REQUEST;
    }

    // Write any update to the config file
    updateConfig();

    // Calculate the new schedule
    HNIS_RESULT_T result = m_schedule.buildSchedule();
    if( result != HNIS_RESULT_SUCCESS )
    {
        return HNID_RESULT_SERVER_ERROR;        
    }

    return HNID_RESULT_SUCCESS;
}

bool
HNIrrigationDevice::getUniqueZoneID( std::string &id )
{
    char tmpID[ 64 ];
    uint idNum = 1;

    id.clear();

    do
    {
        sprintf( tmpID, "z%d", idNum );

        if( m_schedule.hasZone( tmpID ) == false )
        {
            id = tmpID;
            return true;
        }

        idNum += 1;

    }while( idNum < 2000 );

    return false;    
}

HNID_RESULT_T
HNIrrigationDevice::updateEvent( std::string eventID, std::istream& bodyStream )
{
    std::string rstStr;

    // Parse the json body of the request
    try
    {
        // Attempt to parse the json    
        pjs::Parser parser;
        pdy::Var varRoot = parser.parse( bodyStream );

        // Get a pointer to the root object
        pjs::Object::Ptr jsRoot = varRoot.extract< pjs::Object::Ptr >();

        HNScheduleStaticEvent *event = m_schedule.updateEvent( eventID );

        if( jsRoot->has( "type" ) )
        {
            event->setTypeFromStr( jsRoot->getValue<std::string>( "type" ) );
        }

        if( jsRoot->has( "startTime" ) )
        {
            event->setStartTime( jsRoot->getValue<std::string>( "startTime" ) );
        }

        if( jsRoot->has( "endTime" ) )
        {
            event->setEndTime( jsRoot->getValue<std::string>( "endTime" ) );
        }

        if( jsRoot->has( "dayName" ) )
        {
            event->setDayIndexFromNameStr( jsRoot->getValue<std::string>( "dayName" ) );
        }
        
        if( event->validateSettings() != HNIS_RESULT_SUCCESS )
        {
            std::cout << "updateEvent validate failed" << std::endl;
            // zoneid parameter is required
            return HNID_RESULT_BAD_REQUEST;
        }        
    }
    catch( Poco::Exception ex )
    {
        std::cout << "updateEvent exception: " << ex.displayText() << std::endl;
        // Request body was not understood
        return HNID_RESULT_BAD_REQUEST;
    }

    // Write any update to the config file
    updateConfig();

    // Calculate the new schedule
    HNIS_RESULT_T result = m_schedule.buildSchedule();
    if( result != HNIS_RESULT_SUCCESS )
    {
        return HNID_RESULT_SERVER_ERROR;        
    }

    return HNID_RESULT_SUCCESS;
}

bool
HNIrrigationDevice::getUniqueEventID( std::string &id )
{
    char tmpID[ 64 ];
    uint idNum = 1;

    id.clear();

    do
    {
        sprintf( tmpID, "e%d", idNum );

        if( m_schedule.hasEvent( tmpID ) == false )
        {
            id = tmpID;
            return true;
        }

        idNum += 1;

    }while( idNum < 2000 );

    return false;    
}

void 
HNIrrigationDevice::dispatchEP( HNodeDevice *parent, HNOperationData *opData )
{
    std::cout << "HNIrrigationDevice::dispatchEP() - entry" << std::endl;
    std::cout << "  dispatchID: " << opData->getDispatchID() << std::endl;
    std::cout << "  opID: " << opData->getOpID() << std::endl;

    std::string opID = opData->getOpID();

    // GET "/hnode2/irrigation/switches"
    if( "getSwitchList" == opID )
    {
        // Set response content type
        opData->responseSetChunkedTransferEncoding(true);
        opData->responseSetContentType("application/json");

        // Create a json root object
        pjs::Array jsRoot;

        pjs::Object swObj;

        swObj.set( "swid", "s1" );
        swObj.set( "description", "Garden Drip" );
 
        jsRoot.add( swObj );

        swObj.set( "swid", "s2" );
        swObj.set( "description", "West Front" );
 
        jsRoot.add( swObj );

        // Render the response
        std::ostream& ostr = opData->responseSend();
        try
        {
            // Write out the generated json
            pjs::Stringifier::stringify( jsRoot, ostr, 1 );
        }
        catch( ... )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            return;
        }

        opData->responseSetStatusAndReason( HNR_HTTP_OK );
    }
    // GET "/hnode2/irrigation/zones"
    else if( "getZoneList" == opID )
    {
        // Set response content type
        opData->responseSetChunkedTransferEncoding(true);
        opData->responseSetContentType("application/json");

        // Create a json root object
        pjs::Array jsRoot;

        std::vector< HNIrrigationZone > zoneList;
        m_schedule.getZoneList( zoneList );

        for( std::vector< HNIrrigationZone >::iterator zit = zoneList.begin(); zit != zoneList.end(); zit++ )
        { 
           pjs::Object znObj;

           znObj.set( "zoneid", zit->getID() );
           znObj.set( "name", zit->getName() );
           znObj.set( "description", zit->getDesc() );
           znObj.set( "secondsPerWeek", zit->getWeeklySeconds() );
           znObj.set( "cyclesPerDay", zit->getTargetCyclesPerDay() );
           znObj.set( "secondsMinCycle", zit->getMinimumCycleTimeSeconds() );
           znObj.set( "swidList", zit->getSWIDListStr() );

           jsRoot.add( znObj );
        }
 
        // Render the response
        std::ostream& ostr = opData->responseSend();
        try
        {
            // Write out the generated json
            pjs::Stringifier::stringify( jsRoot, ostr, 1 );
        }
        catch( ... )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            return;
        }

        opData->responseSetStatusAndReason( HNR_HTTP_OK );
    }
    // GET "/hnode2/irrigation/zones/{zoneid}"
    else if( "getZoneInfo" == opID )
    {
        std::string zoneID;
        HNIrrigationZone zone;

        if( opData->getParam( "zoneid", zoneID ) == true )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            opData->responseSend();
            return; 
        }

        if( m_schedule.getZone( zoneID, zone ) != HNIS_RESULT_SUCCESS )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
            opData->responseSend();
            return; 
        }

        // Set response content type
        opData->responseSetChunkedTransferEncoding(true);
        opData->responseSetContentType("application/json");

        // Create a json root object
        pjs::Object jsRoot;

        jsRoot.set( "zoneid", zone.getID() );
        jsRoot.set( "name", zone.getName() );
        jsRoot.set( "description", zone.getDesc() );
        jsRoot.set( "secondsPerWeek", zone.getWeeklySeconds() );
        jsRoot.set( "cyclesPerDay", zone.getTargetCyclesPerDay() );
        jsRoot.set( "secondsMinCycle", zone.getMinimumCycleTimeSeconds() );
        jsRoot.set( "swidList", zone.getSWIDListStr() );

        // Render the response
        std::ostream& ostr = opData->responseSend();
        try
        {
            // Write out the generated json
            pjs::Stringifier::stringify( jsRoot, ostr, 1 );
        }
        catch( ... )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            return;
        }

        opData->responseSetStatusAndReason( HNR_HTTP_OK );

    }
    // POST "/hnode2/irrigation/zones"
    else if( "createZone" == opID )
    {
        std::string zoneID;
 
        // Allocate a unique zone identifier
        if( getUniqueZoneID( zoneID ) == false )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            opData->responseSend();
            return; 
        }

        std::cout << "CreateZone 1" << std::endl;

        std::istream& bodyStream = opData->requestBody();
        HNID_RESULT_T result = updateZone( zoneID, bodyStream );

        std::cout << "CreateZone 2" << std::endl;

        switch( result )
        {
            case HNID_RESULT_SUCCESS:
                opData->responseSetCreated( zoneID );
                opData->responseSetStatusAndReason( HNR_HTTP_CREATED );
            break;

            case HNID_RESULT_BAD_REQUEST:
                std::cout << "CreateZone 2.1" << std::endl;
                opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            break;

            default:
            case HNID_RESULT_SERVER_ERROR:
            case HNID_RESULT_FAILURE:
                std::cout << "CreateZone 2.2" << std::endl;
                opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            break;
        }

        std::cout << "CreateZone 3" << std::endl;

        // Send the response
        opData->responseSend();
    }
    // PUT "/hnode2/irrigation/zones/{zoneid}"
    else if( "updateZone" == opID )
    {
        std::string zoneID;

        // Make sure zoneid was provided
        if( opData->getParam( "zoneid", zoneID ) == true )
        {
            // zoneid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            opData->responseSend();
            return; 
        }

        // Make sure zone does exist
        if( m_schedule.hasZone( zoneID ) == false )
        {
            // Zone already exists, return error
            opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
            opData->responseSend();
            return; 
        }

        std::istream& bodyStream = opData->requestBody();
        HNID_RESULT_T result = updateZone( zoneID, bodyStream );

        switch( result )
        {
            case HNID_RESULT_SUCCESS:
                opData->responseSetStatusAndReason( HNR_HTTP_OK );
            break;

            case HNID_RESULT_BAD_REQUEST:
                opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            break;

            default:
            case HNID_RESULT_SERVER_ERROR:
            case HNID_RESULT_FAILURE:
                opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            break;

        }

        // Send the response
        opData->responseSend();
    }
    // DELETE "/hnode2/irrigation/zones/{zoneid}"
    else if( "deleteZone" == opID )
    {
        std::string zoneID;

        // Make sure zoneid was provided
        if( opData->getParam( "zoneid", zoneID ) == true )
        {
            // zoneid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            return; 
        }

        // Remove the zone record
        m_schedule.deleteZone( zoneID );

        // Write the delete to the config file
        updateConfig();

        opData->responseSetStatusAndReason( HNR_HTTP_OK );

        // Send the response
        opData->responseSend();
    }
    else if( "getScheduleInfo" == opID )
    {
        // Set response content type
        opData->responseSetChunkedTransferEncoding(true);
        opData->responseSetContentType("application/json");

        // Send the response
        std::ostream& ostr = opData->responseSend();
        if( m_schedule.getScheduleInfoJSON( ostr ) != HNIS_RESULT_SUCCESS )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            opData->responseSend();
            return;
        }

        opData->responseSetStatusAndReason( HNR_HTTP_OK );
    }
    else if( "getStaticEventList" == opID )
    {
        // Set response content type
        opData->responseSetChunkedTransferEncoding(true);
        opData->responseSetContentType("application/json");

        // Create a json root object
        pjs::Array jsRoot;

        std::vector< HNScheduleStaticEvent > eventList;
        m_schedule.getEventList( eventList );

        for( std::vector< HNScheduleStaticEvent >::iterator eit = eventList.begin(); eit != eventList.end(); eit++ )
        { 
           pjs::Object evObj;

           evObj.set( "eventid", eit->getID() );
           evObj.set( "type", eit->getTypeStr() );
           evObj.set( "startTime", eit->getStartTime().getHMSStr() );
           evObj.set( "endTime", eit->getEndTime().getHMSStr() );
           evObj.set( "dayName", eit->getDayName() );

           jsRoot.add( evObj );
        }
 
        // Render the response
        std::ostream& ostr = opData->responseSend();
        try
        {
            // Write out the generated json
            pjs::Stringifier::stringify( jsRoot, ostr, 1 );
        }
        catch( ... )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            return;
        }

        opData->responseSetStatusAndReason( HNR_HTTP_OK );
    }
    else if( "createStaticEvent" == opID )
    {
        std::string eventID;
 
        // Allocate a unique zone identifier
        if( getUniqueEventID( eventID ) == false )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            opData->responseSend();
            return; 
        }

        std::cout << "CreateEvent 1" << std::endl;

        std::istream& bodyStream = opData->requestBody();
        HNID_RESULT_T result = updateEvent( eventID, bodyStream );

        std::cout << "CreateEvent 2" << std::endl;

        switch( result )
        {
            case HNID_RESULT_SUCCESS:
                opData->responseSetCreated( eventID );
                opData->responseSetStatusAndReason( HNR_HTTP_CREATED );
            break;

            case HNID_RESULT_BAD_REQUEST:
                std::cout << "CreateEvent 2.1" << std::endl;
                opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            break;

            default:
            case HNID_RESULT_SERVER_ERROR:
            case HNID_RESULT_FAILURE:
                std::cout << "CreateEvent 2.2" << std::endl;
                opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            break;
        }

        std::cout << "CreateEvent 3" << std::endl;

        // Send the response
        opData->responseSend();
    }
    else if( "getStaticEvent" == opID )
    {
        std::string eventID;
        HNScheduleStaticEvent event;

        if( opData->getParam( "eventid", eventID ) == true )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            opData->responseSend();
            return; 
        }

        if( m_schedule.getEvent( eventID, event ) != HNIS_RESULT_SUCCESS )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
            opData->responseSend();
            return; 
        }

        // Set response content type
        opData->responseSetChunkedTransferEncoding(true);
        opData->responseSetContentType("application/json");

        // Create a json root object
        pjs::Object jsRoot;

        jsRoot.set( "eventid", event.getID() );
        jsRoot.set( "type", event.getTypeStr() );
        jsRoot.set( "startTime", event.getStartTime().getHMSStr() );
        jsRoot.set( "endTime", event.getEndTime().getHMSStr() );
        jsRoot.set( "dayName", event.getDayName() );

        // Render the response
        std::ostream& ostr = opData->responseSend();
        try
        {
            // Write out the generated json
            pjs::Stringifier::stringify( jsRoot, ostr, 1 );
        }
        catch( ... )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            return;
        }

        opData->responseSetStatusAndReason( HNR_HTTP_OK );

    }
    else if( "updateStaticEvent" == opID )
    {
        std::string eventID;

        // Make sure eventid was provided
        if( opData->getParam( "eventid", eventID ) == true )
        {
            // zoneid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            opData->responseSend();
            return; 
        }

        // Make sure event does exist
        if( m_schedule.hasZone( eventID ) == false )
        {
            // Zone already exists, return error
            opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
            opData->responseSend();
            return; 
        }

        std::istream& bodyStream = opData->requestBody();
        HNID_RESULT_T result = updateEvent( eventID, bodyStream );

        switch( result )
        {
            case HNID_RESULT_SUCCESS:
                opData->responseSetStatusAndReason( HNR_HTTP_OK );
            break;

            case HNID_RESULT_BAD_REQUEST:
                opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            break;

            default:
            case HNID_RESULT_SERVER_ERROR:
            case HNID_RESULT_FAILURE:
                opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            break;

        }

        // Send the response
        opData->responseSend();
    }
    else if( "deleteStaticEvent" == opID )
    {
        std::string eventID;

        // Make sure eventid was provided
        if( opData->getParam( "eventid", eventID ) == true )
        {
            // eventid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            return; 
        }

        // Remove the zone record
        m_schedule.deleteZone( eventID );

        // Write the delete to the config file
        updateConfig();

        opData->responseSetStatusAndReason( HNR_HTTP_OK );

        // Send the response
        opData->responseSend();

    }
    else
    {
        // Send back not implemented
        opData->responseSetStatusAndReason( HNR_HTTP_NOT_IMPLEMENTED );
        opData->responseSend();
    }

}





