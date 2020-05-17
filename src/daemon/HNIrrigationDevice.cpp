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

int 
HNIrrigationDevice::main( const std::vector<std::string>& args )
{
    std::string instance = "default";
    HNodeDevice hnDevice( "hnode2-irrigation-device", instance );

    hnDevice.setName("sp1");


    HNDEndpoint hndEP;

    hndEP.setDispatch( "hnode2Irrigation", this );
    hndEP.setOpenAPIJson( g_HNode2IrrigationRest ); 

    hnDevice.addEndpoint( hndEP );


    hnDevice.start();



#if 0
    HNIrrigationSchedule schedule;

    HNExclusionSpec *testExclude = schedule.updateExclusion( "early" );
    testExclude->setType( HNIS_EXCLUDE_TYPE_EVERYDAY );
    testExclude->setTimesFromStr( "00:00:00", "08:00:00" ); 

    testExclude = schedule.updateExclusion( "mid" );
    testExclude->setType( HNIS_EXCLUDE_TYPE_EVERYDAY );
    testExclude->setTimesFromStr( "11:00:00", "13:00:00" ); 

    testExclude = schedule.updateExclusion( "late" );
    testExclude->setType( HNIS_EXCLUDE_TYPE_EVERYDAY );
    testExclude->setTimesFromStr( "22:00:00", "23:59:59" ); 

    HNIrrigationZone *testZone = schedule.updateZone( "z1" );
    testZone->setName("Test Zone");
    testZone->setDesc("test zone desc");
    testZone->setSWIDList("s1");

    HNIS_RESULT_T result = schedule.buildSchedule();
    if( schedule.buildSchedule() != HNIS_RESULT_SUCCESS )
    {
        return Application::EXIT_SOFTWARE;
    }

    std::cout << "=== Schedule Matrix ===" << std::endl << schedule.getSwitchDaemonJSON() << std::endl;
#endif

    waitForTerminationRequest();

    return Application::EXIT_OK;
}

HNID_RESULT_T
HNIrrigationDevice::commitConfig()
{
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

        HNIrrigationZone *zone = schedule.updateZone( zoneID );

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
            // zoneid parameter is required
            return HNID_RESULT_BAD_REQUEST;
        }        
    }
    catch( Poco::Exception ex )
    {
        // Request body was not understood
        return HNID_RESULT_BAD_REQUEST;
    }

    // Write any update to the config file
    commitConfig();

    // Calculate the new schedule
    HNIS_RESULT_T result = schedule.buildSchedule();
    if( result != HNIS_RESULT_SUCCESS )
    {
        return HNID_RESULT_SERVER_ERROR;        
    }

    return HNID_RESULT_SUCCESS;
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
        schedule.getZoneList( zoneList );

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
            opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
            return; 
        }

        if( schedule.getZone( zoneID, zone ) != HNIS_RESULT_SUCCESS )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            return; 
        }

        // Set response content type
        opData->responseSetChunkedTransferEncoding(true);
        opData->responseSetContentType("application/json");

        HNIS_RESULT_T updateZone( std::string zoneID, HNOperationData *opData );

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
    // POST "/hnode2/irrigation/zones/{zoneid}"
    else if( "createZone" == opID )
    {
        std::string zoneID;

        // Make sure zoneid was provided
        if( opData->getParam( "zoneid", zoneID ) == true )
        {
            // zoneid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            return; 
        }

        // Make sure zone doesn't already exist
        if( schedule.hasZone( zoneID ) == true )
        {
            // Zone already exists, return error
            opData->responseSetStatusAndReason( HNR_HTTP_CONFLICT );
            return; 
        }

        std::istream& bodyStream = opData->requestBody();
        HNID_RESULT_T result = updateZone( zoneID, bodyStream );

        switch( result )
        {
            case HNID_RESULT_SUCCESS:
            break;

            case HNID_RESULT_BAD_REQUEST:
                opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
                return;
            break;

            default:
            case HNID_RESULT_SERVER_ERROR:
            case HNID_RESULT_FAILURE:
                opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
                return;
            break;
        }

        opData->responseSetStatusAndReason( HNR_HTTP_OK );
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
            return; 
        }

        // Make sure zone does exist
        if( schedule.hasZone( zoneID ) == false )
        {
            // Zone already exists, return error
            opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
            return; 
        }

        std::istream& bodyStream = opData->requestBody();
        HNID_RESULT_T result = updateZone( zoneID, bodyStream );

        switch( result )
        {
            case HNID_RESULT_SUCCESS:
            break;

            case HNID_RESULT_BAD_REQUEST:
                opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
                return;
            break;

            default:
            case HNID_RESULT_SERVER_ERROR:
            case HNID_RESULT_FAILURE:
                opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
                return;
            break;

        }

        opData->responseSetStatusAndReason( HNR_HTTP_OK );

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
        schedule.deleteZone( zoneID );

        opData->responseSetStatusAndReason( HNR_HTTP_OK );
    }
    else
    {
        // Send back not implemented
        opData->responseSetStatusAndReason( HNR_HTTP_NOT_IMPLEMENTED );
    }

#if 0
    if( "getDeviceInfo" == opID )
    {
        // Create a json root object
        pjs::Object jsRoot;

        opData->responseSetChunkedTransferEncoding(true);
        opData->responseSetContentType("application/json");

        jsRoot.set( "hnodeID", getHNodeIDStr() );
        jsRoot.set( "crc32ID", getHNodeIDCRC32Str() );
 
        jsRoot.set( "name", getName() );

        jsRoot.set( "instance", getInstance() );

        jsRoot.set( "deviceType", getDeviceType() );
        jsRoot.set( "version", getVersionStr() );

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
    }
    else if( "getDeviceOwner" == opID )
    {
        // Create a json root object
        pjs::Object jsRoot;

        opData->responseSetChunkedTransferEncoding(true);
        opData->responseSetContentType("application/json");

        jsRoot.set( "state", getOwnerState() );
        jsRoot.set( "hnodeID", getOwnerHNodeIDStr() );

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
    }
    else
    {
        // Send back not implemented
        opData->responseSetStatusAndReason( HNR_HTTP_NOT_IMPLEMENTED );
    }
#endif

}


