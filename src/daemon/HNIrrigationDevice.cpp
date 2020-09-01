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
#include <Poco/StreamCopier.h>

#include <hnode2/HNodeDevice.h>
#include <hnode2/HNSwitchDaemon.h>

#include "HNIDActionRequest.h"
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


      "/hnode2/irrigation/schedule/criteria": {
        "get": {
          "summary": "Get list of schedule criteria.",
          "operationId": "getCriteriaList",
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
          "summary": "Create a new schedule criteria.",
          "operationId": "createCriteria",
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


      "/hnode2/irrigation/schedule/criteria/{criteriaid}": {
        "get": {
          "summary": "Get information about a specific schedule criteria.",
          "operationId": "getCriteria",
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
          "summary": "Update an existing criteria.",
          "operationId": "updateCriteria",
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
          "summary": "Delete an existing criteria.",
          "operationId": "deleteCriteria",
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

void 
HNIrrigationDevice::setState( HNID_STATE_T value )
{
    m_state = value;
}

HNID_STATE_T 
HNIrrigationDevice::getState()
{
    return m_state;
}

int 
HNIrrigationDevice::main( const std::vector<std::string>& args )
{
    setState( HNID_STATE_NOINIT );

    m_getSWDHealthDetail = false;
    m_sendSchedule = false;

    m_instanceName = "default";
    if( _instancePresent == true )
        m_instanceName = _instance;

    m_actionQueue.init();

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

    if( m_evLoop.setup( this ) != HNEP_RESULT_SUCCESS )
    {
        std::cerr << "ERROR: Failed to start event loop." << std::endl;
        return Application::EXIT_SOFTWARE;
    }

    // Hook the local action queue into the event loop
    if( m_evLoop.addFDToEPoll( m_actionQueue.getEventFD() ) != HNEP_RESULT_SUCCESS )
    {
        // Failed to add client socket.
        std::cerr << "ERROR: Failed to add local action queue to event loop." << std::endl;
        return Application::EXIT_SOFTWARE;
    }

    setState( HNID_STATE_INITIALIZED );

    // Start up the hnode device
    m_hnodeDev.start();

    // Start the event loop
    m_evLoop.run();  

    waitForTerminationRequest();

    return Application::EXIT_OK;
}

bool 
HNIrrigationDevice::openSWDSocket()
{
    struct sockaddr_un addr;
    char str[512];

    // Clear address structure - UNIX domain addressing
    // addr.sun_path[0] cleared to 0 by memset() 
    memset( &addr, 0, sizeof(struct sockaddr_un) );  
    addr.sun_family = AF_UNIX;                     

    // Abstract socket with name @<deviceName>-<instanceName>
    sprintf( str, "hnode2-%s-%s", HN_SWDAEMON_DEVICE_NAME, m_instanceName.c_str() );
    strncpy( &addr.sun_path[1], str, strlen(str) );

    // Register the socket
    m_swdFD = socket( AF_UNIX, SOCK_SEQPACKET, 0 );

    // Establish the connection.
    if( connect( m_swdFD, (struct sockaddr *) &addr, ( sizeof( sa_family_t ) + strlen( str ) + 1 ) ) == 0 )
    {
        // Success
        printf( "Successfully opened client socket on file descriptor: %d\n", m_swdFD );
        return false;
    }

    // Failure
    return true;
}

void
HNIrrigationDevice::handleSWDStatus( HNSWDPacketClient &packet )
{
    std::string msg;

    packet.getMsg( msg );
    std::cout << "=== Daemon Status Recieved - result code: " << packet.getResult() << " ===" << std::endl;

    m_swdStatus.setFromJSON( msg );

    // Check if the schedule on the switch daemon
    // needs to be updated to match our current
    // schedule.
    if( m_swdStatus.getSMCRC32() != m_schedule.getSMCRC32() )
    {
        m_sendSchedule = true;
    }

    // Check if the switch daemon is unhealthy,
    // if so collect additional health information
    if( m_swdStatus.healthDegraded() )
    {
        m_getSWDHealthDetail = true;
    }
}

void
HNIrrigationDevice::handleSWDScheduleUpdateRsp( HNSWDPacketClient &packet )
{
    if( m_state != HNID_STATE_WAIT_SET_SCHEDULE )
    {
        return;
    }

    setState( HNID_STATE_READY );

    return;
}

void
HNIrrigationDevice::handleSWDSwitchInfoRsp( HNSWDPacketClient &packet )
{
    std::string msg;

    // If response was spurious then ignore it
    if( m_state != HNID_STATE_WAIT_SWINFO )
    {
        return;
    }

    // Decode the response and complete the action

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

        // Clear rsp data
        m_curAction->refSwitchList().clear();

        // Walk through each switch list item
        for( uint index = 0; index < jsSWList->size(); index++ )
        {
            if( jsSWList->isObject( index ) == false )
                continue;
                                
            pjs::Object::Ptr jsSWInfo = jsSWList->getObject( index );

            // Add the switch info to the response data
            HNSWDSwitchInfo swInfo;
            swInfo.setID( jsSWInfo->optValue( "swid", empty ) );
            swInfo.setDesc( jsSWInfo->optValue( "description", empty ) );

            m_curAction->refSwitchList().push_back( swInfo );
        }

        // Finish the request
        m_curAction->complete( true );

    }
    catch( Poco::Exception ex )
    {
        // Error during response processing
        std::cout << "  ERROR: Response message not parsable: " << msg << std::endl;
        m_curAction->complete( false );
    }

    // Retire the request
    m_curAction = NULL;
    setState( HNID_STATE_READY );

    return;
}



bool
HNIrrigationDevice::handleSWDPacket()
{
    HNSWDPacketClient packet;
    HNSWDP_RESULT_T   result;

    printf( "Waiting for packet reception...\n" );

    // Read the header portion of the packet
    result = packet.rcvHeader( m_swdFD );
    if( result != HNSWDP_RESULT_SUCCESS )
    {
        printf( "ERROR: Failed while receiving packet header." );
        return true;
    } 

    // Read any payload portion of the packet
    result = packet.rcvPayload( m_swdFD );
    if( result != HNSWDP_RESULT_SUCCESS )
    {
        printf( "ERROR: Failed while receiving packet payload." );
        return true;
    } 

    switch( packet.getType() )
    {
        case HNSWD_PTYPE_DAEMON_STATUS:
        {
            handleSWDStatus( packet );
        }
        break;

        case HNSWD_PTYPE_SCHEDULE_UPDATE_RSP:
        {
            handleSWDScheduleUpdateRsp( packet );
        }
        break;

        case HNSWD_PTYPE_SWINFO_RSP:
        {
            handleSWDSwitchInfoRsp( packet );
        }
        break;
    }

    return false;
}

void
HNIrrigationDevice::fdEvent( int sfd )
{
    std::cout << "FD Event Handler: " << sfd << std::endl;

    if( sfd == m_swdFD )
    {
        handleSWDPacket();
    }
    else if( sfd == m_actionQueue.getEventFD() )
    {
        startAction();
    }
}

void
HNIrrigationDevice::fdError( int sfd )
{
    std::cout << "FD Error Handler: " << sfd << std::endl;

}

void 
HNIrrigationDevice::loopIteration()
{
    // Get current timestamp


//    std::cout << "EV Loop Iteration Handler" << std::endl;
    switch( getState() )
    {
        // Finish with the startup
        case HNID_STATE_INITIALIZED:
            // Calculate an initial schedule
            m_schedule.buildSchedule();

            // Temporary output of switch manager schedule
            std::cout << "===Switch Manager JSON===" << std::endl;
            std::cout << m_schedule.getSwitchDaemonJSON() << std::endl;

            if( openSWDSocket() ) //"", m_instanceName, swdFD ) )
            {
                // Failed to open client socket, set health status
                std::cerr << "ERROR: Failed to open switch daemon socket." << std::endl;
                setState( HNID_STATE_CONNECT_RECOVER );
                return;
            }
            else if( m_evLoop.addFDToEPoll( m_swdFD ) != HNEP_RESULT_SUCCESS )
            {
                // Failed to add client socket, set health status
                std::cerr << "ERROR: Failed to add switch daemon socket to event loop." << std::endl;
                setState( HNID_STATE_CONNECT_RECOVER );
                return;
            }
                 
            setState( HNID_STATE_READY );
        break;

        case HNID_STATE_CONNECT_RECOVER:

        break;

        case HNID_STATE_READY:

            // Schedule Update?
            if( m_sendSchedule == true )
            {
                // Send schedule update request

                // Clear the action
                m_sendSchedule = false;

                // Wait for reply before further action
                setState( HNID_STATE_WAIT_SET_SCHEDULE );

                return;
            }

            // Check if health state should be aquired

        break;

        case HNID_STATE_WAIT_SET_SCHEDULE:

        break;

    }
}

void 
HNIrrigationDevice::timeoutEvent()
{
//    std::cout << "EV Timeout Handler" << std::endl;

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

bool
HNIrrigationDevice::getUniqueZoneID( HNIDActionRequest *action )
{
    char tmpID[ 64 ];
    uint idNum = 1;

    do
    {
        sprintf( tmpID, "z%d", idNum );

        if( m_schedule.hasZone( tmpID ) == false )
        {
            action->setZoneID( tmpID );
            return true;
        }

        idNum += 1;

    }while( idNum < 2000 );

    return false;    
}

bool
HNIrrigationDevice::getUniqueCriteriaID( HNIDActionRequest *action )
{
    char tmpID[ 64 ];
    uint idNum = 1;

    do
    {
        sprintf( tmpID, "e%d", idNum );

        if( m_schedule.hasCriteria( tmpID ) == false )
        {
            action->setCriteriaID( tmpID );
            return true;
        }

        idNum += 1;

    }while( idNum < 2000 );

    return false;    
}

typedef enum HNIDStartActionBitsEnum
{
    HNID_ACTBIT_CLEAR     = 0x0000,
    HNID_ACTBIT_COMPLETE  = 0x0001,
    HNID_ACTBIT_UPDATE    = 0x0002,
    HNID_ACTBIT_RECALCSCH = 0x0004,
    HNID_ACTBIT_ERROR     = 0x0008,
    HNID_ACTBIT_SENDREQ   = 0x0010
} HNID_ACTBIT_T;

void
HNIrrigationDevice::startAction()
{
    HNSWDPacketClient packet;
    HNID_ACTBIT_T  actBits = HNID_ACTBIT_CLEAR;

    // Verify that we are in a state that will allow actions to start
//    if( getState() != HNID_STATE_READY )
//    {
//        return;
//    }

    // Pop the action from the queue
    m_curAction = ( HNIDActionRequest* ) m_actionQueue.aquireRecord();

    std::cout << "Action aquired - type: " << m_curAction->getType() << std::endl;

    switch( m_curAction->getType() )
    {
        case HNID_AR_TYPE_SWLIST:
        {
            // Request the switch list information 
            // from the switch list deamon.
            setState( HNID_STATE_WAIT_SWINFO );
            packet.setType( HNSWD_PTYPE_SWINFO_REQ );
            actBits = HNID_ACTBIT_SENDREQ;
        }
        break;

        case HNID_AR_TYPE_ZONELIST:
            // Populate the zone list in the action
            m_schedule.getZoneList( m_curAction->refZoneList() );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        break;

        case HNID_AR_TYPE_ZONEINFO:
        {
            HNIrrigationZone zone;

            if( m_schedule.getZone( m_curAction->getZoneID(), zone ) != HNIS_RESULT_SUCCESS )
            {
                //opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Populate the zone list in the action
            m_curAction->refZoneList().push_back( zone );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        }
        break;

        case HNID_AR_TYPE_ZONECREATE:
        {
            // Allocate a unique zone identifier
            if( getUniqueZoneID( m_curAction ) == false )
            {
                // opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Create the zone record
            HNIrrigationZone *zone = m_schedule.updateZone( m_curAction->getZoneID() );

            // Update the fields of the zone record.
            m_curAction->applyZoneUpdate( zone );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_ZONEUPDATE:
        {
            if( m_schedule.hasZone( m_curAction->getZoneID() ) == false )
            {
                // Zone doesn't exist, return error
                // opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Get a point to zone record
            HNIrrigationZone *zone = m_schedule.updateZone( m_curAction->getZoneID() );

            // Update the fields of the zone record.
            m_curAction->applyZoneUpdate( zone );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_ZONEDELETE:
        {
            // Remove the zone record
            m_schedule.deleteZone( m_curAction->getZoneID() );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;


        case HNID_AR_TYPE_SCHINFO:
        {
            // Get a copy of the schedule as JSON
            // std::ostream& ostr = opData->responseSend();
            if( m_schedule.getScheduleInfoJSON( m_curAction->refRspStream() ) != HNIS_RESULT_SUCCESS )
            {
                //opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        }
        break;

        case HNID_AR_TYPE_CRITLIST:
            // Populate the event list in the action
            m_schedule.getCriteriaList( m_curAction->refCriteriaList() );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        break;

        case HNID_AR_TYPE_CRITINFO:
        {
            HNScheduleCriteria event;

            if( m_schedule.getCriteria( m_curAction->getCriteriaID(), event ) != HNIS_RESULT_SUCCESS )
            {
                //opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Populate the zone list in the action
            m_curAction->refCriteriaList().push_back( event );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        }
        break;

        case HNID_AR_TYPE_CRITCREATE:
        {
            // Allocate a unique zone identifier
            if( getUniqueCriteriaID( m_curAction ) == false )
            {
                // opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Create the zone record
            HNScheduleCriteria *event = m_schedule.updateCriteria( m_curAction->getCriteriaID() );

            // Update the fields of the zone record.
            m_curAction->applyCriteriaUpdate( event );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_CRITUPDATE:
        {
            if( m_schedule.hasCriteria( m_curAction->getCriteriaID() ) == false )
            {
                // Zone doesn't exist, return error
                // opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Get a point to zone record
            HNScheduleCriteria *event = m_schedule.updateCriteria( m_curAction->getCriteriaID() );

            // Update the fields of the zone record.
            m_curAction->applyCriteriaUpdate( event );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_CRITDELETE:
        {
            // Remove the zone record
            m_schedule.deleteCriteria( m_curAction->getCriteriaID() );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        default:
            actBits = HNID_ACTBIT_ERROR;
        break;
    }

    // The configuration was changed so commit
    // it to persistent copy
    if( actBits & HNID_ACTBIT_UPDATE )
    {
        // Commit config update
        updateConfig();
    }

    // Configuration changed so recalculate the
    // schedule layout
    if( actBits & HNID_ACTBIT_RECALCSCH )
    {
        // Calculate the new schedule
        HNIS_RESULT_T result = m_schedule.buildSchedule();
        if( result != HNIS_RESULT_SUCCESS )
        {
            actBits = HNID_ACTBIT_ERROR;
        }
    }

    // Send a request down to the switch daemon
    if( actBits & HNID_ACTBIT_SENDREQ )
    {
        std::cout << "Sending a switch deamon request..." << std::endl;
        packet.sendAll( m_swdFD );
    }

    // There was an error, complete with error
    if( actBits & HNID_ACTBIT_ERROR )
    {
        // Signal failure
        m_curAction->complete( false );
        m_curAction = NULL;
        return;
    }

    // Request has been completed successfully
    if( actBits & HNID_ACTBIT_COMPLETE )
    {
        // Done with this request
        m_curAction->complete( true );
        m_curAction = NULL;
    }

}

void 
HNIrrigationDevice::dispatchEP( HNodeDevice *parent, HNOperationData *opData )
{
    HNIDActionRequest action;

    std::cout << "HNIrrigationDevice::dispatchEP() - entry" << std::endl;
    std::cout << "  dispatchID: " << opData->getDispatchID() << std::endl;
    std::cout << "  opID: " << opData->getOpID() << std::endl;

    std::string opID = opData->getOpID();

    // GET "/hnode2/irrigation/switches"
    if( "getSwitchList" == opID )
    {
        action.setType( HNID_AR_TYPE_SWLIST );
    }
    // GET "/hnode2/irrigation/zones"
    else if( "getZoneList" == opID )
    {
        action.setType( HNID_AR_TYPE_ZONELIST );
    }
    // GET "/hnode2/irrigation/zones/{zoneid}"
    else if( "getZoneInfo" == opID )
    {
        std::string zoneID;

        if( opData->getParam( "zoneid", zoneID ) == true )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_ZONEINFO );
        action.setZoneID( zoneID );
    }
    // POST "/hnode2/irrigation/zones"
    else if( "createZone" == opID )
    {
        action.setType( HNID_AR_TYPE_ZONECREATE );

        std::istream& bodyStream = opData->requestBody();
        action.setZoneUpdate( bodyStream );
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
        action.setType( HNID_AR_TYPE_ZONEUPDATE );
        action.setZoneID( zoneID );

        std::istream& bodyStream = opData->requestBody();
        action.setZoneUpdate( bodyStream );
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
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_ZONEDELETE );
        action.setZoneID( zoneID );
    }
    else if( "getCriteriaList" == opID )
    {
        action.setType( HNID_AR_TYPE_CRITLIST );
    }
    else if( "createCriteria" == opID )
    {
        action.setType( HNID_AR_TYPE_CRITCREATE );

        std::istream& bodyStream = opData->requestBody();
        action.setZoneUpdate( bodyStream );
    }
    else if( "getCriteria" == opID )
    {
        std::string eventID;

        if( opData->getParam( "eventid", eventID ) == true )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_CRITINFO );
        action.setZoneID( eventID );
    }
    else if( "updateCriteria" == opID )
    {
        std::string eventID;

        // Make sure zoneid was provided
        if( opData->getParam( "eventid", eventID ) == true )
        {
            // zoneid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_CRITUPDATE );
        action.setZoneID( eventID );

        std::istream& bodyStream = opData->requestBody();
        action.setCriteriaUpdate( bodyStream );
    }
    else if( "deleteCriteria" == opID )
    {
        std::string eventID;

        // Make sure zoneid was provided
        if( opData->getParam( "eventid", eventID ) == true )
        {
            // eventid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_CRITDELETE );
        action.setZoneID( eventID );
    }
    else if( "getScheduleInfo" == opID )
    {
        action.setType( HNID_AR_TYPE_SCHINFO );
    }
    else
    {
        // Send back not implemented
        opData->responseSetStatusAndReason( HNR_HTTP_NOT_IMPLEMENTED );
        opData->responseSend();
        return;
    }

    std::cout << "Start Action - client: " << action.getType() << std::endl;

    // Submit the action and block for response
    m_actionQueue.postAndWait( &action );

    std::cout << "Finish Action - client" << std::endl;

    // Determine what happened
    switch( action.getStatus() )
    {
        case HNRW_RESULT_SUCCESS:
        {
            std::string cType;
            std::string objID;


            // See if response content should be generated
            if( action.hasRspContent( cType ) )
            {
                // Set response content type
                opData->responseSetChunkedTransferEncoding( true );
                opData->responseSetContentType( cType );

                // Render any response content
                std::ostream& ostr = opData->responseSend();
            
                if( action.generateRspContent( ostr ) == true )
                {
                    opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
                    opData->responseSend();
                    return;
                }
            }

            // Check if a new object was created.
            if( action.hasNewObject( objID ) )
            {
                // Object was created return info
                opData->responseSetCreated( objID );
                opData->responseSetStatusAndReason( HNR_HTTP_CREATED );
            }
            else
            {
                // Request was successful
                opData->responseSetStatusAndReason( HNR_HTTP_OK );
            }
        }
        break;

        case HNRW_RESULT_FAILURE:
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
        break;

        case HNRW_RESULT_TIMEOUT:
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
        break;
    }

    // Return to caller
    opData->responseSend();
}


