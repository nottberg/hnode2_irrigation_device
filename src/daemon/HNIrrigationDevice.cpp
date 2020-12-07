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
      "/hnode2/irrigation/status": {
        "get": {
          "summary": "Get irrigation device status.",
          "operationId": "getStatus",
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

      "/hnode2/irrigation/schedule/state": {
        "get": {
          "summary": "Get information about scheduler state.",
          "operationId": "getSchedulerState",
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
          "summary": "Set scheduler to a specific state.",
          "operationId": "setSchedulerState",
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


      "/hnode2/irrigation/criteria": {
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

      "/hnode2/irrigation/criteria/{criteriaid}": {
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
      },

      "/hnode2/irrigation/zonectl": {
        "put": {
          "summary": "Send manual control request for one or more zones.",
          "operationId": "putZoneControlRequest",
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

    m_schedule.init( &m_criteria, &m_zones );

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
HNIrrigationDevice::sendScheduleUpdate()
{
    HNSWDPacketClient packet;

    packet.setType( HNSWD_PTYPE_SCHEDULE_UPDATE_REQ );
    packet.setMsg( m_schedule.getSwitchDaemonJSON() );
    packet.sendAll( m_swdFD );

    std::cout << "Schedule Sent" << std::endl;
}

void
HNIrrigationDevice::handleSWDStatus( HNSWDPacketClient &packet )
{
    std::string msg;

    packet.getMsg( msg );
    //std::cout << "=== Daemon Status Recieved - result code: " << packet.getResult() << " ===" << std::endl;

    m_swdStatus.setFromSwitchDaemonJSON( msg, &m_zones );

    // Check if the schedule on the switch daemon
    // needs to be updated to match our current
    // schedule.
    if( m_swdStatus.getSMCRC32() != m_schedule.getSMCRC32() )
    {
        std::cout << "=== Daemon Status schCRC - " << m_swdStatus.getSMCRC32() << " : " << m_schedule.getSMCRC32() << std::endl;
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
HNIrrigationDevice::handleSWDEvent( HNSWDPacketClient &packet )
{
    std::string msg;

    packet.getMsg( msg );
    std::cout << "=== Daemon Event Recieved - result code: " << packet.getResult() << " ===" << std::endl;
}

void
HNIrrigationDevice::handleSWDScheduleUpdateRsp( HNSWDPacketClient &packet )
{
    if( m_state != HNID_STATE_WAIT_SET_SCHEDULE )
    {
        return;
    }

    // Clear the action
    m_sendSchedule = false;

    setState( HNID_STATE_READY );

    return;
}

void
HNIrrigationDevice::handleSWDSwitchInfoRsp( HNSWDPacketClient &packet )
{
    std::string msg;

    std::cout << "Response RX: " << m_state << std::endl;
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

void
HNIrrigationDevice::handleScheduleStateRsp( HNSWDPacketClient &packet )
{
    std::string msg;

    std::cout << "Response RX: " << m_state << std::endl;
    // If response was spurious then ignore it
    if( m_state != HNID_STATE_WAIT_SCHCTL )
    {
        return;
    }

    // TODO error handling

    // Finish the request
    m_curAction->complete( true );

    // Retire the request
    m_curAction = NULL;
    setState( HNID_STATE_READY );

    return;
}

void
HNIrrigationDevice::handleZoneCtrlRsp( HNSWDPacketClient &packet )
{
    std::string msg;

    std::cout << "Response RX: " << m_state << std::endl;
    // If response was spurious then ignore it
    if( m_state != HNID_STATE_WAIT_ZONECTL )
    {
        return;
    }

    // TODO error handling

    // Finish the request
    m_curAction->complete( true );

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

    //printf( "Waiting for packet reception...\n" );

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

        case HNSWD_PTYPE_DAEMON_EVENT:
        {
            handleSWDEvent( packet );
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

        case HNSWD_PTYPE_SCH_STATE_RSP:
        {
            handleScheduleStateRsp( packet );
        }
        break;

        case HNSWD_PTYPE_USEQ_ADD_RSP:
        case HNSWD_PTYPE_SEQ_CANCEL_RSP:
        {
            handleZoneCtrlRsp( packet );
        }
        break;
    }

    return false;
}

void
HNIrrigationDevice::fdEvent( int sfd )
{
    //std::cout << "FD Event Handler: " << sfd << std::endl;

    if( sfd == m_swdFD )
    {
        handleSWDPacket();
    }
    else if( sfd == m_actionQueue.getEventFD() )
    {
        // Verify that we can handle a new action,
        // otherwise just spin.
        std::cout << "Current State: " << getState() << std::endl;
        if( getState() != HNID_STATE_READY )
            return;

        // Start the new action
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
                // Wait for reply before further action
                setState( HNID_STATE_WAIT_SET_SCHEDULE );

                // Send schedule update request
                sendScheduleUpdate();

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

    m_zones.initZoneListSection( cfg );

    m_criteria.initCriteriaListSection( cfg );

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
  
    std::cout << "cl1" << std::endl;
    m_hnodeDev.readConfigSections( cfg );

    std::cout << "cl2" << std::endl;
    m_zones.readZoneListSection( cfg );

    std::cout << "cl3" << std::endl;
    m_criteria.readCriteriaListSection( cfg );

    std::cout << "cl4" << std::endl;
    m_schedule.readConfigSections( cfg );

    std::cout << "Config loaded" << std::endl;

    return HNID_RESULT_SUCCESS;
}

HNID_RESULT_T
HNIrrigationDevice::updateConfig()
{
    HNodeConfigFile cfgFile;
    HNodeConfig     cfg;

    m_hnodeDev.updateConfigSections( cfg );

    m_zones.updateZoneListSection( cfg );

    m_criteria.updateCriteriaListSection( cfg );

    m_schedule.updateConfigSections( cfg );

    cfg.debugPrint(2);
    
    std::cout << "Saving config..." << std::endl;
    if( cfgFile.saveConfig( HNODE_IRRIGATION_DEVTYPE, m_instanceName, cfg ) != HNC_RESULT_SUCCESS )
    {
        std::cout << "ERROR: Could not save configuration." << std::endl;
        return HNID_RESULT_FAILURE;
    }
    std::cout << "Config saved" << std::endl;

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

        if( m_zones.hasID( tmpID ) == false )
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

        if( m_criteria.hasID( tmpID ) == false )
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

    std::cout << "Action aquired - type: " << m_curAction->getType()  << "  thread: " << std::this_thread::get_id() << std::endl;

    switch( m_curAction->getType() )
    {
        case HNID_AR_TYPE_IRRSTATUS:
        {
            // Get current device status as JSON
            if( m_swdStatus.getAsIrrigationJSON( m_curAction->refRspStream(), &m_zones ) != HNIS_RESULT_SUCCESS )
            {
                //opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        }
        break;

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
            m_zones.getZoneList( m_curAction->refZoneList() );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        break;

        case HNID_AR_TYPE_ZONEINFO:
        {
            HNIrrigationZone zone;

            if( m_zones.getZone( m_curAction->getZoneID(), zone ) != HNIS_RESULT_SUCCESS )
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
            HNIrrigationZone *zone = m_zones.updateZone( m_curAction->getZoneID() );

            // Update the fields of the zone record.
            m_curAction->applyZoneUpdate( zone );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_ZONEUPDATE:
        {
            if( m_zones.hasID( m_curAction->getZoneID() ) == false )
            {
                // Zone doesn't exist, return error
                // opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Get a point to zone record
            HNIrrigationZone *zone = m_zones.updateZone( m_curAction->getZoneID() );

            // Update the fields of the zone record.
            m_curAction->applyZoneUpdate( zone );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_ZONEDELETE:
        {
            // Remove the zone record
            m_zones.deleteZone( m_curAction->getZoneID() );

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
            m_criteria.getCriteriaList( m_curAction->refCriteriaList() );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        break;

        case HNID_AR_TYPE_CRITINFO:
        {
            HNIrrigationCriteria event;

            if( m_criteria.getCriteria( m_curAction->getCriteriaID(), event ) != HNIS_RESULT_SUCCESS )
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
            HNIrrigationCriteria *event = m_criteria.updateCriteria( m_curAction->getCriteriaID() );

            // Update the fields of the zone record.
            m_curAction->applyCriteriaUpdate( event );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_CRITUPDATE:
        {
            if( m_criteria.hasID( m_curAction->getCriteriaID() ) == false )
            {
                // Zone doesn't exist, return error
                // opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Get a point to zone record
            HNIrrigationCriteria *event = m_criteria.updateCriteria( m_curAction->getCriteriaID() );

            // Update the fields of the zone record.
            m_curAction->applyCriteriaUpdate( event );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_CRITDELETE:
        {
            // Remove the zone record
            m_criteria.deleteCriteria( m_curAction->getCriteriaID() );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        // Get detailed health information
        //HNSWD_PTYPE_HEALTH_REQ,
        //HNSWD_PTYPE_HEALTH_RSP,

        // Implement Me
        case HNID_AR_TYPE_GETSCHSTATE:
        {
            actBits = HNID_ACTBIT_ERROR;
            break;
        }

        case HNID_AR_TYPE_SETSCHSTATE:
        {
            std::stringstream msg;

            // Wait for scheduling state change response
            setState( HNID_STATE_WAIT_SCHCTL );
          
            // Build the payload message
            // Create a json root object
            pjs::Object jsRoot;

            // Add the new requested state
            switch( m_curAction->getScheduleStateRequestType() )
            {
                case HNID_SSR_ENABLE:
                    jsRoot.set( "state", "enable" );
                    jsRoot.set( "inhibitDuration", "00:00:00" );
                break;
 
                case HNID_SSR_DISABLE:
                    jsRoot.set( "state", "disable" );
                    jsRoot.set( "inhibitDuration", "00:00:00" );
                break;

                case HNID_SSR_INHIBIT:
                    jsRoot.set( "state", "inhibit" );
                    jsRoot.set( "inhibitDuration", "00:10:00" );
                break;
            }

            // Render into a json string.
            try
            {
                pjs::Stringifier::stringify( jsRoot, msg );
            }
            catch( ... )
            {
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            std::cout << "Sending a SCHEDULING STATE request..." << std::endl;
            packet.setType( HNSWD_PTYPE_SCH_STATE_REQ );
            packet.setMsg( msg.str() );
            actBits = HNID_ACTBIT_SENDREQ;
        }

        case HNID_AR_TYPE_ZONECTL:
        {
            // Wait for zone control response
            setState( HNID_STATE_WAIT_ZONECTL );

            // Switch Daemon Requests
            // HNSWD_PTYPE_RESET_REQ, // Reset switch daemon      
            // HNSWD_PTYPE_USEQ_ADD_REQ,   // Add a one time sequence of switch actions.
            // HNSWD_PTYPE_SEQ_CANCEL_REQ, // Cancel all future sequence actions.
            // HNSWD_PTYPE_SCH_STATE_REQ,  // Change scheduling state. Enable/Disable/Inhibit

#if 0
                HNSWDPacketClient packet;
                uint32_t length;

                packet.setType( HNSWD_PTYPE_RESET_REQ );

                std::cout << "Sending a RESET request..." << std::endl;

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
#endif

            packet.setType( HNSWD_PTYPE_SWINFO_REQ );
            actBits = HNID_ACTBIT_SENDREQ;
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
        std::cout << "Sending a switch deamon request..." << "  thread: " << std::this_thread::get_id() << std::endl;
        packet.sendAll( m_swdFD );
    }

    // There was an error, complete with error
    if( actBits & HNID_ACTBIT_ERROR )
    {
        std::cout << "Failing action: " << m_curAction->getType() << "  thread: " << std::this_thread::get_id() << std::endl;

        // Signal failure
        m_curAction->complete( false );
        m_curAction = NULL;
        return;
    }

    // Request has been completed successfully
    if( actBits & HNID_ACTBIT_COMPLETE )
    {
        std::cout << "Completing action: " << m_curAction->getType() << "  thread: " << std::this_thread::get_id() << std::endl;

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
    std::cout << "  thread: " << std::this_thread::get_id() << std::endl;

    std::string opID = opData->getOpID();

    // GET "/hnode2/irrigation/status"
    if( "getStatus" == opID )
    {
        action.setType( HNID_AR_TYPE_IRRSTATUS );
    }
    // GET "/hnode2/irrigation/switches"
    else if( "getSwitchList" == opID )
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
        action.decodeZoneUpdate( bodyStream );
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
        action.decodeZoneUpdate( bodyStream );
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
        action.decodeCriteriaUpdate( bodyStream );
    }
    else if( "getCriteria" == opID )
    {
        std::string criteriaID;

        if( opData->getParam( "criteriaid", criteriaID ) == true )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_CRITINFO );
        action.setCriteriaID( criteriaID );
    }
    else if( "updateCriteria" == opID )
    {
        std::string criteriaID;

        // Make sure zoneid was provided
        if( opData->getParam( "criteriaid", criteriaID ) == true )
        {
            // zoneid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_CRITUPDATE );
        action.setCriteriaID( criteriaID );

        std::istream& bodyStream = opData->requestBody();
        action.decodeCriteriaUpdate( bodyStream );
    }
    else if( "deleteCriteria" == opID )
    {
        std::string criteriaID;

        // Make sure zoneid was provided
        if( opData->getParam( "criteriaid", criteriaID ) == true )
        {
            // eventid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_CRITDELETE );
        action.setCriteriaID( criteriaID );
    }
    else if( "getScheduleInfo" == opID )
    {
        action.setType( HNID_AR_TYPE_SCHINFO );
    }
    else if( "getSchedulerState" == opID )
    {
        action.setType( HNID_AR_TYPE_GETSCHSTATE );
    }
    else if( "setSchedulerState" == opID )
    {
        action.setType( HNID_AR_TYPE_SETSCHSTATE );

        std::istream& bodyStream = opData->requestBody();
        action.decodeSchedulerState( bodyStream );
    }
    else if( "putZoneControlRequest" == opID )
    {
        action.setType( HNID_AR_TYPE_ZONECTL );

        std::istream& bodyStream = opData->requestBody();
        action.decodeZoneCtrl( bodyStream );
    }
    else
    {
        // Send back not implemented
        opData->responseSetStatusAndReason( HNR_HTTP_NOT_IMPLEMENTED );
        opData->responseSend();
        return;
    }

    std::cout << "Start Action - client: " << action.getType() << "  thread: " << std::this_thread::get_id() << std::endl;

    // Submit the action and block for response
    m_actionQueue.postAndWait( &action );

    std::cout << "Finish Action - client" << "  thread: " << std::this_thread::get_id() << std::endl;

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


