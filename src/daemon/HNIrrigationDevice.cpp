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
        }
      },

      "/hnode2/irrigation/placement": {
        "get": {
          "summary": "Get list of schedule placement.",
          "operationId": "getPlacementList",
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
          "summary": "Create a new schedule placement.",
          "operationId": "createPlacement",
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

      "/hnode2/irrigation/placement/{placementid}": {
        "get": {
          "summary": "Get information about a specific schedule placement.",
          "operationId": "getPlacement",
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
          "summary": "Update an existing placement.",
          "operationId": "updatePlacement",
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
          "summary": "Delete an existing placement.",
          "operationId": "deletePlacement",
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

      "/hnode2/irrigation/modifier": {
        "get": {
          "summary": "Get list of zone modifiers.",
          "operationId": "getModifiersList",
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
          "summary": "Create a new zone modifier.",
          "operationId": "createModifier",
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

      "/hnode2/irrigation/modifier/{modifierid}": {
        "get": {
          "summary": "Get information about a specific zone modifier.",
          "operationId": "getModifier",
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
          "summary": "Update an existing modifier.",
          "operationId": "updateModifier",
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
          "summary": "Delete an existing modifier.",
          "operationId": "deleteModifier",
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

      "/hnode2/irrigation/sequence": {
        "get": {
          "summary": "Get list of zone sequences.",
          "operationId": "getSequencesList",
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
          "summary": "Create a new zone sequence.",
          "operationId": "createSequence",
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

      "/hnode2/irrigation/sequence/{sequenceid}": {
        "get": {
          "summary": "Get information about a specific zone sequence.",
          "operationId": "getSequence",
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
          "summary": "Update an existing sequence.",
          "operationId": "updateSequence",
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
          "summary": "Delete an existing sequence.",
          "operationId": "deleteSequence",
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

      "/hnode2/irrigation/inhibit": {
        "get": {
          "summary": "Get list of zone inhibits.",
          "operationId": "getInhibitsList",
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
          "summary": "Create a new zone inhibit.",
          "operationId": "createInhibit",
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

      "/hnode2/irrigation/inhibit/{inhibitid}": {
        "get": {
          "summary": "Get information about a specific zone inhibit.",
          "operationId": "getInhibit",
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
          "summary": "Delete an existing inhibit.",
          "operationId": "deleteInhibit",
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




      "/hnode2/irrigation/operation": {
        "get": {
          "summary": "Get list of active and queued operations.",
          "operationId": "getOperationsList",
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
          "summary": "Queue a new operation.",
          "operationId": "createOperation",
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

      "/hnode2/irrigation/operation/{operationid}": {
        "get": {
          "summary": "Get information about a specific operation.",
          "operationId": "getOperation",
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
          "summary": "Cancel an active operation.",
          "operationId": "cancelOperation",
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
    m_sendSchedule       = false;
    m_sendSchedulerState = false;

    m_nextInhibitID      = 1;
    m_nextOpID           = 1;

    m_pendingActiveSequence = NULL;
    m_currentActiveSequence = NULL;

    m_targetSchedulerEnabled = false;

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

    m_schedule.init( &m_placements, &m_zones, &m_modifiers );

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
HNIrrigationDevice::sendSchedulerStateUpdate()
{
    HNSWDPacketClient packet;
    pjs::Object       jsRoot;
    std::stringstream msg;

    // Build the payload message
    // Add the new requested state
    if( m_targetSchedulerEnabled == true )
    {
        jsRoot.set( "state", "enable" );
        jsRoot.set( "inhibitDuration", "00:00:00" );
    }
    else
    {
        jsRoot.set( "state", "disable" );
        jsRoot.set( "inhibitDuration", "00:00:00" );
    }

    // Render into a json string.
    try { pjs::Stringifier::stringify( jsRoot, msg ); } catch( ... ) { return; }

    packet.setType( HNSWD_PTYPE_SCH_STATE_REQ );
    packet.setMsg( msg.str() );
    packet.sendAll( m_swdFD );

    std::cout << "Scheduler State Update Sent" << std::endl;
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

    // If the scheduler state is not as desired then setup
    // to correct that.
    if( (m_targetSchedulerEnabled == true) && (m_swdStatus.getSchedulerState() != "enabled") )
    {
        std::cout << "=== Daemon Status schedulerState - " << m_swdStatus.getSchedulerState() << " : " << m_targetSchedulerEnabled << std::endl;
        m_sendSchedulerState = true;
    }
    else if( (m_targetSchedulerEnabled == false) && (m_swdStatus.getSchedulerState() != "disabled") )
    {
        std::cout << "=== Daemon Status schedulerState - " << m_swdStatus.getSchedulerState() << " : " << m_targetSchedulerEnabled << std::endl;
        m_sendSchedulerState = true;
    }

    // If there is a pending sequence watch for the switch daemon to acknowledge it is running.
    if( m_pendingActiveSequence != NULL )
    {
        if( m_swdStatus.hasActiveSequence() && (m_swdStatus.getActiveSequenceID() == m_pendingActiveSequence->getSeqReqID()) )
        {
            std::cout << "=== Moving pending sequence to active sequence - " << m_pendingActiveSequence->getSeqReqID() << std::endl;
            m_currentActiveSequence = m_pendingActiveSequence;
            m_pendingActiveSequence = NULL;
        }
    }

    // If there is a running sequence operation watch for its completion
    if( m_currentActiveSequence != NULL )
    {
        if( (m_swdStatus.hasActiveSequence() == false) || (m_swdStatus.getActiveSequenceID() != m_currentActiveSequence->getSeqReqID()) )
        {
            std::cout << "=== Daemon Status Active Sequence Completed: " << m_currentActiveSequence->getSeqReqID() << std::endl;

            // The switch daemon is no longer running our active sequence so,
            // cleanup as it has completed.
            std::string oid = m_currentActiveSequence->getID();
            m_opQueue.deleteOperation( oid );
            m_currentActiveSequence = NULL;
        }
        else
        {
            std::cout << "=== Daemon Status Active Sequence: " << m_swdStatus.getActiveSequenceID() << std::endl;
        }
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
HNIrrigationDevice::handleScheduleStateRsp( HNSWDPacketClient &packet )
{
    std::string msg;

    std::cout << "Response RX: " << m_state << std::endl;
    // If response was spurious then ignore it
    if( m_state != HNID_STATE_WAIT_SCHCTL )
    {
        return;
    }

    std::cout << "Received Schedule State Response" << std::endl;

    // TODO error handling

    // Finish the request
    m_sendSchedulerState = false;

    // Since we know our request succeeded update the status block
    // with the expected value, without waiting for a status update
    m_swdStatus.setSchedulerState( (m_targetSchedulerEnabled == true) ? "enabled" : "disabled" );

    // Retire the request
    setState( HNID_STATE_READY );

    return;
}

void
HNIrrigationDevice::handleSequenceStartRsp( HNSWDPacketClient &packet )
{
    std::string msg;

    std::cout << "Response RX: " << m_state << std::endl;
    // If response was spurious then ignore it
    if( m_state != HNID_STATE_WAIT_SEQSTART )
    {
        return;
    }

    // TODO error handling

    // Finish the request
    m_curAction->complete( true );
    m_curAction = NULL;
    setState( HNID_STATE_READY );

    return;
}

void
HNIrrigationDevice::handleSequenceCancelRsp( HNSWDPacketClient &packet )
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
        {
            handleSequenceStartRsp( packet );
        }
        break;

        case HNSWD_PTYPE_SEQ_CANCEL_RSP:
        {
            handleSequenceCancelRsp( packet );
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

bool
HNIrrigationDevice::checkForInhibitChanges( time_t curTime )
{
    std::string inhibitID;
    std::vector< std::string > inhibitDeleteList;
    bool rtnValue = false;

    // Get the current time to check for inhibit expirations
    switch( m_inhibits.checkSchedulerAction( curTime, inhibitID ) )
    {
        case HNII_INHIBIT_ACTION_NONE:
            if( m_targetSchedulerEnabled == false )
            {
                std::cout << "== Inhibit - no-action - scheduler change to enabled" << std::endl;
                m_targetSchedulerEnabled = true;
                sendSchedulerStateUpdate();
                rtnValue = true;
            } 
        break;

        case HNII_INHIBIT_ACTION_ACTIVE:
            if( m_targetSchedulerEnabled == true )
            {
                std::cout << "== Inhibit - active - scheduler change to disabled: " << inhibitID << std::endl;
                m_targetSchedulerEnabled = false;
                sendSchedulerStateUpdate();
                rtnValue = true;
            }
        break;

        case HNII_INHIBIT_ACTION_EXPIRED:
            if( m_targetSchedulerEnabled == false )
            {
                std::cout << "== Inhibit - expired - scheduler change to enabled: " << inhibitID << std::endl;
                m_targetSchedulerEnabled = true;
                sendSchedulerStateUpdate();
                rtnValue = true;
            }
            inhibitDeleteList.push_back( inhibitID );
        break;
    }

    // Scan through each zone and check for an inhibit
    std::vector< HNIrrigationZone > zoneList;
    m_zones.getZoneList( zoneList );
    for( std::vector< HNIrrigationZone >::iterator zit = zoneList.begin(); zit != zoneList.end(); zit++ )
    {    
        switch( m_inhibits.checkZoneAction( curTime, zit->getID(), inhibitID ) )
        {
            case HNII_INHIBIT_ACTION_NONE:
                if( zit->isInhibited() == true )
                {
                    std::cout << "== Inhibit - none - zone " << zit->getID() << " inhibit cleared" << std::endl;
                    m_zones.clearInhibited( zit->getID() );
                    rtnValue = true;
                }
            break;

            case HNII_INHIBIT_ACTION_ACTIVE:
                if( zit->isInhibited() == false )
                {
                    std::cout << "== Inhibit - active - zone " << zit->getID() << " inhibit set : " << inhibitID << std::endl;
                    m_zones.setInhibited( zit->getID(), inhibitID );
                    rtnValue = true;
                }
            break;

            case HNII_INHIBIT_ACTION_EXPIRED:
                if( zit->isInhibited() == true )
                {
                    std::cout << "== Inhibit - expired - zone " << zit->getID() << " inhibit set : " << inhibitID << std::endl;
                    m_zones.clearInhibited( zit->getID() );
                    rtnValue = true;
                }
                inhibitDeleteList.push_back( inhibitID );
            break;
        }
    }

    // Cleanup expired inhibits
    for( std::vector< std::string >::iterator it = inhibitDeleteList.begin(); it != inhibitDeleteList.end(); it++ )
    {
        std::cout << "Deleting inhibit: " << *it << std::endl;
        m_inhibits.deleteInhibit( *it );
        rtnValue = true;
    }

    // No update needed.
    return rtnValue;
}

void 
HNIrrigationDevice::loopIteration()
{
    //struct tm newtime;
    time_t ltime;

    // Get current timestamp
    ltime = time(NULL);
    //localtime_r( &ltime, &newtime );

//    std::cout << "EV Loop Iteration Handler" << std::endl;
    switch( getState() )
    {
        // Finish with the startup
        case HNID_STATE_INITIALIZED:
            // Calculate an initial schedule
            m_schedule.buildSchedule( m_targetSchedulerEnabled );

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

            // Periodic scan of the inhibits to look for changes that would effect
            // zone scheduling.
            if( checkForInhibitChanges( ltime ) == true )
            {
                std::cout << "Update config and rebuilding schedule due to inhibit change." << std::endl;

                // Commit config update since inhibit state changed.
                updateConfig();

                // Rebuild the schedule
                m_schedule.buildSchedule( m_targetSchedulerEnabled );
            }

            // Schedule Update?
            if( m_sendSchedule == true )
            {
                // Wait for reply before further action
                setState( HNID_STATE_WAIT_SET_SCHEDULE );

                // Send schedule update request
                sendScheduleUpdate();

                return;
            }

            // Schedule State Update?
            if( m_sendSchedulerState == true )
            {
                // Wait for reply before further action
                setState( HNID_STATE_WAIT_SCHCTL );

                // Send schedule update request
                sendSchedulerStateUpdate();

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
    //std::cout << "EV Timeout Handler" << std::endl;
    //std::cout << "Posted Count: " << m_actionQueue.getPostedCnt() << std::endl;
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
    HNodeConfigFile  cfgFile;
    HNodeConfig      cfg;
    HNCSection      *secPtr;

    cfg.updateSection( "irrDeviceInfo", &secPtr );

    secPtr->updateValue("targetSchedulerState", (m_targetSchedulerEnabled == true) ? "enabled" : "disabled" );

    m_hnodeDev.initConfigSections( cfg );

    m_zones.initZoneListSection( cfg );

    m_placements.initPlacementsListSection( cfg );

    m_modifiers.initModifiersListSection( cfg );

    m_sequences.initSequencesListSection( cfg );

    m_inhibits.initInhibitsListSection( cfg );

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
    HNCSection      *secPtr;
    std::string     targetSchedulerState;

    if( configExists() == false )
        return HNID_RESULT_FAILURE;

    std::cout << "Loading config..." << std::endl;

    if( cfgFile.loadConfig( HNODE_IRRIGATION_DEVTYPE, m_instanceName, cfg ) != HNC_RESULT_SUCCESS )
    {
        std::cout << "ERROR: Could not load saved configuration." << std::endl;
        return HNID_RESULT_FAILURE;
    }
  
    // Aquire a pointer to the "device" section
    cfg.updateSection( "irrDeviceInfo", &secPtr );

    // Get a list pointer
    secPtr->getValueByName( "targetSchedulerState", targetSchedulerState );
    if( targetSchedulerState == "enabled" )
        m_targetSchedulerEnabled = true;
    else
        m_targetSchedulerEnabled = false;

    std::cout << "cl1" << std::endl;
    m_hnodeDev.readConfigSections( cfg );

    std::cout << "cl2" << std::endl;
    m_zones.readZoneListSection( cfg );

    std::cout << "cl3" << std::endl;
    m_placements.readPlacementsListSection( cfg );

    std::cout << "cl4" << std::endl;
    m_modifiers.readModifiersListSection( cfg );

    std::cout << "cl5" << std::endl;
    m_sequences.readSequencesListSection( cfg );

    std::cout << "cl6" << std::endl;
    m_inhibits.readInhibitsListSection( cfg );

    std::cout << "Config loaded" << std::endl;

    return HNID_RESULT_SUCCESS;
}

HNID_RESULT_T
HNIrrigationDevice::updateConfig()
{
    HNodeConfigFile  cfgFile;
    HNodeConfig      cfg;
    HNCSection      *secPtr;

    // Aquire a pointer to the "device" section
    cfg.updateSection( "irrDeviceInfo", &secPtr );

    // Get a list pointer
    secPtr->updateValue( "targetSchedulerState", (m_targetSchedulerEnabled == true) ? "enabled" : "disabled" );

    m_hnodeDev.updateConfigSections( cfg );

    m_zones.updateZoneListSection( cfg );

    m_placements.updatePlacementsListSection( cfg );

    m_modifiers.updateModifiersListSection( cfg );

    m_sequences.updateSequencesListSection( cfg );

    m_inhibits.updateInhibitsListSection( cfg );

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
HNIrrigationDevice::getUniquePlacementID( HNIDActionRequest *action )
{
    char tmpID[ 64 ];
    uint idNum = 1;

    do
    {
        sprintf( tmpID, "p%d", idNum );

        if( m_placements.hasID( tmpID ) == false )
        {
            action->setPlacementID( tmpID );
            return true;
        }

        idNum += 1;

    }while( idNum < 2000 );

    return false;    
}

bool
HNIrrigationDevice::getUniqueModifierID( HNIDActionRequest *action )
{
    char tmpID[ 64 ];
    uint idNum = 1;

    do
    {
        sprintf( tmpID, "m%d", idNum );

        if( m_modifiers.hasID( tmpID ) == false )
        {
            action->setModifierID( tmpID );
            return true;
        }

        idNum += 1;

    }while( idNum < 2000 );

    return false;    
}

bool
HNIrrigationDevice::getUniqueSequenceID( HNIDActionRequest *action )
{
    char tmpID[ 64 ];
    uint idNum = 1;

    do
    {
        sprintf( tmpID, "sq%d", idNum );

        if( m_sequences.hasID( tmpID ) == false )
        {
            action->setSequenceID( tmpID );
            return true;
        }

        idNum += 1;

    }while( idNum < 2000 );

    return false;    
}

bool
HNIrrigationDevice::getUniqueInhibitID( HNIDActionRequest *action )
{
    char tmpID[ 64 ];

    sprintf( tmpID, "in%d", m_nextInhibitID );
    m_nextInhibitID += 1;
    action->setInhibitID( tmpID );

    return true;
}

bool
HNIrrigationDevice::getUniqueOperationID( HNIDActionRequest *action )
{
    char tmpID[ 64 ];

    sprintf( tmpID, "op%d", m_nextOpID );
    m_nextOpID += 1;
    action->setOperationID( tmpID );

    return true;    
}

HNID_RESULT_T 
HNIrrigationDevice::buildStoredSequenceJSON( HNIrrigationOperation *opObj, std::ostream &ostr )
{
    // Create a json root object
    pjs::Object jsRoot;

    HNIrrigationSequence seqObj;
    if( m_sequences.getSequence( opObj->getFirstObjID(), seqObj ) != HNIS_RESULT_SUCCESS ) 
    {
        return HNID_RESULT_FAILURE;
    }
            
    switch( seqObj.getType() )
    {
        case HNISQ_TYPE_UNIFORM:
        {
            jsRoot.set( "requestID", opObj->getSeqReqID() );
            jsRoot.set( "seqType", "uniform" );
            jsRoot.set( "onDuration", seqObj.getOnDurationAsStr() );
            jsRoot.set( "offDuration", seqObj.getOffDurationAsStr() );

            // Convert the zone references into switch references.   
            std::string swidStr;
            bool first = true;
            for( std::list< std::string >::iterator oit = seqObj.getObjIDListRef().begin(); oit != seqObj.getObjIDListRef().end(); oit++ )
            {
                HNIrrigationZone zone;

                if( m_zones.getZone( *oit, zone ) != HNIS_RESULT_SUCCESS )
                    continue;

                if( first == false )
                    swidStr += " ";
                swidStr += zone.getSWIDListStr();
                first = false;
            }

            jsRoot.set( "swidList", swidStr );          
        }
        break;

        case HNISQ_TYPE_CHAIN:
        {
            return HNID_RESULT_FAILURE;
        }
        break;
    }

    try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return HNID_RESULT_FAILURE; }

    return HNID_RESULT_SUCCESS;
}

HNID_RESULT_T 
HNIrrigationDevice::buildOneTimeSequenceJSON( HNIrrigationOperation *opObj, std::ostream &ostr )
{
    // Create a json root object
    pjs::Object jsRoot;

    jsRoot.set( "requestID", opObj->getSeqReqID() );
    jsRoot.set( "seqType", "uniform" );
    jsRoot.set( "onDuration", opObj->getOnDurationAsStr() );
    jsRoot.set( "offDuration", opObj->getOffDurationAsStr() );

    // Convert the zone references into switch references. 
    std::string swidStr;
    bool first = true;
    for( std::vector< std::string >::iterator oit = opObj->getObjIDListRef().begin(); oit != opObj->getObjIDListRef().end(); oit++ )
    {
        HNIrrigationZone zone;

        if( m_zones.getZone( *oit, zone ) != HNIS_RESULT_SUCCESS )
            continue;

        if( first == false )
            swidStr += " ";
        swidStr += zone.getSWIDListStr();
        first = false;
    }

    jsRoot.set( "swidList", swidStr );

    try { pjs::Stringifier::stringify( jsRoot, ostr, 1 ); } catch( ... ) { return HNID_RESULT_FAILURE; }

    return HNID_RESULT_SUCCESS;
}

HNID_ACTBIT_T
HNIrrigationDevice::executeOperation( HNIrrigationOperation *opReq, HNSWDPacketClient &packet )
{
    std::cout << "Start Execute Operation - type: " << opReq->getType() << std::endl;

    switch( opReq->getType() )
    {
#if 0      
        // Enable/Disable the scheduler
        case HNOP_TYPE_SCHEDULER_STATE:
        {
            // Add the new requested state
            if( opReq->getEnable() == true )
                m_targetSchedulerState = "enabled";
            else
                m_targetSchedulerState = "disabled";
       
            // Check if we think the scheduler state needs to be updated
            if( m_targetSchedulerState != m_swdStatus.getSchedulerState() )
            {
                std::cout << "=== Operation Check schedulerState - " << m_swdStatus.getSchedulerState() << " : " << m_targetSchedulerState << std::endl;
                m_sendSchedulerState = true;
            }

            // Get rid of operation record
            std::string opID = opReq->getID();
            m_opQueue.deleteOperation( opID );

            return (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_COMPLETE);
        }
        break;
#endif

        // Execute a stored sequence
        case HNOP_TYPE_EXEC_SEQUENCE:
        {
            std::stringstream msg;
            pjs::Object       jsRoot;

            // If there is already an active sequence then reject
            // this new sequence?
            if( (m_pendingActiveSequence != NULL) || (m_currentActiveSequence != NULL) )
            {
                return HNID_ACTBIT_ERROR;  
            }
             
            // Build the payload message
            HNID_RESULT_T result = buildStoredSequenceJSON( opReq, msg );
            if( result != HNID_RESULT_SUCCESS )
            {
                return HNID_ACTBIT_ERROR;
            }

            // Wait for sequence start response
            setState( HNID_STATE_WAIT_SEQSTART );

            std::cout << "Sending a Sequence Start request..." << std::endl;
            packet.setType( HNSWD_PTYPE_USEQ_ADD_REQ );
            packet.setMsg( msg.str() );
       
            // Keep track of this current sequence request
            m_pendingActiveSequence = opReq;

            std::cout << "=== Starting Active Sequence: " << m_pendingActiveSequence->getFirstObjID() << std::endl;

            return (HNID_ACTBIT_T)(HNID_ACTBIT_SENDREQ);
        }
        break;

        // Execute a onetime sequence
        case HNOP_TYPE_EXEC_ONETIMESEQ:
        {
            std::stringstream msg;
            pjs::Object       jsRoot;

            // If there is already an active sequence then reject
            // this new sequence?
            if( (m_pendingActiveSequence != NULL) || (m_currentActiveSequence != NULL) )
            {
                return HNID_ACTBIT_ERROR;  
            }
             
            // Build the payload message
            HNID_RESULT_T result = buildOneTimeSequenceJSON( opReq, msg );
            if( result != HNID_RESULT_SUCCESS )
            {
                return HNID_ACTBIT_ERROR;
            }

            // Wait for sequence start response
            setState( HNID_STATE_WAIT_SEQSTART );

            std::cout << "Sending a One Time Sequence Start request..." << std::endl;
            packet.setType( HNSWD_PTYPE_USEQ_ADD_REQ );
            packet.setMsg( msg.str() );
       
            // Keep track of this current sequence request
            m_pendingActiveSequence = opReq;

            std::cout << "=== Starting One Time Sequence: " << m_pendingActiveSequence->getID() << std::endl;

            return (HNID_ACTBIT_T)(HNID_ACTBIT_SENDREQ);
        }        
        break;
    }

    // Unrecognized operation
    std::cout << "ERROR: Unrecognized operation." << std::endl;

    return HNID_ACTBIT_ERROR;

#if 0
            else if( _seqcancelRequested == true )
            {
                HNSWDPacketClient packet;

                packet.setType( HNSWD_PTYPE_SEQ_CANCEL_REQ );

                std::cout << "Sending a SEQUENCE CANCEL request..." << std::endl;

                packet.sendAll( sockfd );
            }
#endif    
}

HNIS_RESULT_T
HNIrrigationDevice::buildIrrigationStatusResponse( std::ostream &ostr )
{
    std::string seqIDStr;
    std::string seqNameStr;
    pjs::Object jsRoot;

    // Create a json root object
    jsRoot.set( "date", m_swdStatus.getDateStr() );
    jsRoot.set( "time", m_swdStatus.getTimeStr() );
    jsRoot.set( "timezone", m_swdStatus.getTimezoneStr() );

    jsRoot.set( "schedulerState", (m_targetSchedulerEnabled == true) ? "enabled" : "disabled" );
    jsRoot.set( "inhibitUntil", "" );

    pjs::Object ovHealth;

    ovHealth.set( "status", m_swdStatus.getOverallHealthStatus() );
    ovHealth.set( "msg", m_swdStatus.getOverallHealthMessage() );

    jsRoot.set( "overallHealth", ovHealth );

    if( (m_pendingActiveSequence != NULL) || (m_currentActiveSequence != NULL) )
    {
        HNIrrigationOperation *curSeq = (m_pendingActiveSequence) ? m_pendingActiveSequence : m_currentActiveSequence;
        if( curSeq->getType() == HNOP_TYPE_EXEC_ONETIMESEQ )
        {
            seqNameStr = "One-Time Sequence (" + curSeq->getSeqReqID() + ")";
        }
        else
        {
            if( m_sequences.getSequenceName( curSeq->getSeqReqID(), seqNameStr ) == HNIS_RESULT_SUCCESS )
                seqIDStr = curSeq->getSeqReqID();
            else
                seqNameStr = "Unknown Sequence";
        }
    }

    jsRoot.set( "activeSequenceName", seqNameStr );
    jsRoot.set( "activeSequenceID", seqIDStr );

    pjs::Array activeZones;
    std::vector< HNIrrigationZone > azoneList;
    m_zones.getActiveZones( azoneList );
    for( std::vector< HNIrrigationZone >::iterator it = azoneList.begin(); it != azoneList.end(); it++ )
    {
        pjs::Object azone;
        azone.set( "id", it->getID() );
        azone.set( "name", it->getName() );
        activeZones.add( azone );
    }
    jsRoot.set( "activeZones", activeZones );

    pjs::Array inhibitedZones;
    std::vector< HNIrrigationZone > izoneList;
    m_zones.getInhibitedZones( izoneList );
    for( std::vector< HNIrrigationZone >::iterator it = izoneList.begin(); it != izoneList.end(); it++ )
    {
        pjs::Object izone;
        izone.set( "id", it->getID() );
        izone.set( "name", it->getName() );
        izone.set( "inhibitByID", it->getInhibitedByID() );
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
            if( buildIrrigationStatusResponse( m_curAction->refRspStream() ) != HNIS_RESULT_SUCCESS )
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

        case HNID_AR_TYPE_PLACELIST:
            // Populate the event list in the action
            m_placements.getPlacementsList( m_curAction->refPlacementsList() );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        break;

        case HNID_AR_TYPE_PLACEINFO:
        {
            HNIrrigationPlacement event;

            if( m_placements.getPlacement( m_curAction->getPlacementID(), event ) != HNIS_RESULT_SUCCESS )
            {
                //opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Populate the zone list in the action
            m_curAction->refPlacementsList().push_back( event );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        }
        break;

        case HNID_AR_TYPE_PLACECREATE:
        {
            // Allocate a unique zone identifier
            if( getUniquePlacementID( m_curAction ) == false )
            {
                // opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Create the zone record
            HNIrrigationPlacement *event = m_placements.updatePlacement( m_curAction->getPlacementID() );

            // Update the fields of the zone record.
            m_curAction->applyPlacementUpdate( event );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_PLACEUPDATE:
        {
            if( m_placements.hasID( m_curAction->getPlacementID() ) == false )
            {
                // Zone doesn't exist, return error
                // opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Get a point to zone record
            HNIrrigationPlacement *event = m_placements.updatePlacement( m_curAction->getPlacementID() );

            // Update the fields of the zone record.
            m_curAction->applyPlacementUpdate( event );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_PLACEDELETE:
        {
            // Remove the zone record
            m_placements.deletePlacement( m_curAction->getPlacementID() );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_MODIFIERSLIST:
            // Populate the event list in the action
            m_modifiers.getModifiersList( m_curAction->refModifiersList() );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        break;

        case HNID_AR_TYPE_MODIFIERINFO:
        {
            HNIrrigationModifier event;

            if( m_modifiers.getModifier( m_curAction->getModifierID(), event ) != HNIS_RESULT_SUCCESS )
            {
                //opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Populate the zone list in the action
            m_curAction->refModifiersList().push_back( event );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        }
        break;

        case HNID_AR_TYPE_MODIFIERCREATE:
        {
            // Allocate a unique zone identifier
            if( getUniqueModifierID( m_curAction ) == false )
            {
                // opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Create the zone record
            HNIrrigationModifier *event = m_modifiers.updateModifier( m_curAction->getModifierID() );

            // Update the fields of the zone record.
            m_curAction->applyModifierUpdate( event );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_MODIFIERUPDATE:
        {
            if( m_modifiers.hasID( m_curAction->getModifierID() ) == false )
            {
                // Zone doesn't exist, return error
                // opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Get a point to zone record
            HNIrrigationModifier *event = m_modifiers.updateModifier( m_curAction->getModifierID() );

            // Update the fields of the zone record.
            m_curAction->applyModifierUpdate( event );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_MODIFIERDELETE:
        {
            // Remove the zone record
            m_modifiers.deleteModifier( m_curAction->getModifierID() );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;
        
        case HNID_AR_TYPE_SEQUENCESLIST:
            // Populate the event list in the action
            m_sequences.getSequencesList( m_curAction->refSequencesList() );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        break;

        case HNID_AR_TYPE_SEQUENCEINFO:
        {
            HNIrrigationSequence event;

            if( m_sequences.getSequence( m_curAction->getSequenceID(), event ) != HNIS_RESULT_SUCCESS )
            {
                //opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Populate the zone list in the action
            m_curAction->refSequencesList().push_back( event );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        }
        break;

        case HNID_AR_TYPE_SEQUENCECREATE:
        {
            // Allocate a unique zone identifier
            if( getUniqueSequenceID( m_curAction ) == false )
            {
                // opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Create the zone record
            HNIrrigationSequence *event = m_sequences.updateSequence( m_curAction->getSequenceID() );

            // Update the fields of the zone record.
            m_curAction->applySequenceUpdate( event );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_SEQUENCEUPDATE:
        {
            if( m_sequences.hasID( m_curAction->getSequenceID() ) == false )
            {
                // Zone doesn't exist, return error
                // opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Get a point to zone record
            HNIrrigationSequence *event = m_sequences.updateSequence( m_curAction->getSequenceID() );

            // Update the fields of the zone record.
            m_curAction->applySequenceUpdate( event );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_SEQUENCEDELETE:
        {
            // Remove the zone record
            m_sequences.deleteSequence( m_curAction->getSequenceID() );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_INHIBITSLIST:
            // Populate the event list in the action
            m_inhibits.getInhibitsList( m_curAction->refInhibitsList() );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        break;

        case HNID_AR_TYPE_INHIBITINFO:
        {
            HNIrrigationInhibit event;

            if( m_inhibits.getInhibit( m_curAction->getInhibitID(), event ) != HNIS_RESULT_SUCCESS )
            {
                //opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Populate the zone list in the action
            m_curAction->refInhibitsList().push_back( event );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        }
        break;

        case HNID_AR_TYPE_INHIBITCREATE:
        {
            // Allocate a unique inhibit identifier
            if( getUniqueInhibitID( m_curAction ) == false )
            {
                // opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Create the inhibit record
            HNIrrigationInhibit *event = m_inhibits.updateInhibit( m_curAction->getInhibitID() );

            // Update the fields of the inhibit record.
            m_curAction->applyInhibitUpdate( event );

            // Special processing to update the inhibit maps.
            m_inhibits.reconcileNewObject( m_curAction->getInhibitID() );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_INHIBITDELETE:
        {
            // Remove the zone record
            m_inhibits.deleteInhibit( m_curAction->getInhibitID() );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_UPDATE | HNID_ACTBIT_RECALCSCH | HNID_ACTBIT_COMPLETE);
        }
        break;

        case HNID_AR_TYPE_OPERATIONSLIST:
            // Populate the event list in the action
            m_opQueue.getOperationsList( m_curAction->refOperationsList() );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        break;

        case HNID_AR_TYPE_OPERATIONINFO:
        {
            HNIrrigationOperation event;

            if( m_opQueue.getOperation( m_curAction->getOperationID(), event ) != HNIS_RESULT_SUCCESS )
            {
                //opData->responseSetStatusAndReason( HNR_HTTP_NOT_FOUND );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Populate the operation list in the action
            m_curAction->refOperationsList().push_back( event );

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
        }
        break;

        case HNID_AR_TYPE_OPERATIONCREATE:
        {
            // Allocate a unique operation identifier
            if( getUniqueOperationID( m_curAction ) == false )
            {
                // opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
                actBits = HNID_ACTBIT_ERROR;
                break;
            }

            // Create the operation record
            HNIrrigationOperation *event = m_opQueue.addOperation( m_curAction->getOperationID() );

            // Update the fields of the operation record.
            m_curAction->applyOperationUpdate( event );

            // Execute the requested local operation
            actBits = executeOperation( event, packet );

        }
        break;

        case HNID_AR_TYPE_OPERATIONCANCEL:
        {
            // Cancel any ongoing operation
            m_opQueue.deleteOperation( m_curAction->getOperationID() );

            actBits = (HNID_ACTBIT_T)(HNID_ACTBIT_COMPLETE);
        }
        break; 


        // Get detailed health information
        //HNSWD_PTYPE_HEALTH_REQ,
        //HNSWD_PTYPE_HEALTH_RSP,

        // Implement Me
        case HNID_AR_TYPE_GETSCHSTATE:
        {
            // Build the payload message
            // Create a json root object          
            std::stringstream msg;
            pjs::Object jsRoot;

            jsRoot.set( "state", m_swdStatus.getSchedulerState() );
            jsRoot.set( "inhibitDuration", "00:00:00" );

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

            // Done with this request
            actBits = HNID_ACTBIT_COMPLETE;
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
        HNIS_RESULT_T result = m_schedule.buildSchedule( m_targetSchedulerEnabled );
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
    else if( "getPlacementList" == opID )
    {
        action.setType( HNID_AR_TYPE_PLACELIST );
    }
    else if( "createPlacement" == opID )
    {
        action.setType( HNID_AR_TYPE_PLACECREATE );

        std::istream& bodyStream = opData->requestBody();
        action.decodePlacementUpdate( bodyStream );
    }
    else if( "getPlacement" == opID )
    {
        std::string placementID;

        if( opData->getParam( "placementid", placementID ) == true )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_PLACEINFO );
        action.setPlacementID( placementID );
    }
    else if( "updatePlacement" == opID )
    {
        std::string placementID;

        // Make sure zoneid was provided
        if( opData->getParam( "placementid", placementID ) == true )
        {
            // zoneid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_PLACEUPDATE );
        action.setPlacementID( placementID );

        std::istream& bodyStream = opData->requestBody();
        action.decodePlacementUpdate( bodyStream );
    }
    else if( "deletePlacement" == opID )
    {
        std::string placementID;

        // Make sure zoneid was provided
        if( opData->getParam( "placementid", placementID ) == true )
        {
            // eventid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_PLACEDELETE );
        action.setPlacementID( placementID );
    }
    else if( "getModifiersList" == opID )
    {
        action.setType( HNID_AR_TYPE_MODIFIERSLIST );
    }
    else if( "createModifier" == opID )
    {
        action.setType( HNID_AR_TYPE_MODIFIERCREATE );

        std::istream& bodyStream = opData->requestBody();
        action.decodeModifierUpdate( bodyStream );
    }
    else if( "getModifier" == opID )
    {
        std::string modifierID;

        if( opData->getParam( "modifierid", modifierID ) == true )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_MODIFIERINFO );
        action.setModifierID( modifierID );
    }
    else if( "updateModifier" == opID )
    {
        std::string modifierID;

        // Make sure zoneid was provided
        if( opData->getParam( "modifierid", modifierID ) == true )
        {
            // zoneid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_MODIFIERUPDATE );
        action.setModifierID( modifierID );

        std::istream& bodyStream = opData->requestBody();
        action.decodeModifierUpdate( bodyStream );
    }
    else if( "deleteModifier" == opID )
    {
        std::string modifierID;

        // Make sure zoneid was provided
        if( opData->getParam( "modifierid", modifierID ) == true )
        {
            // eventid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_MODIFIERDELETE );
        action.setModifierID( modifierID );
    }
    else if( "getSequencesList" == opID )
    {
        action.setType( HNID_AR_TYPE_SEQUENCESLIST );
    }
    else if( "createSequence" == opID )
    {
        action.setType( HNID_AR_TYPE_SEQUENCECREATE );

        std::istream& bodyStream = opData->requestBody();
        action.decodeSequenceUpdate( bodyStream );
    }
    else if( "getSequence" == opID )
    {
        std::string sequenceID;

        if( opData->getParam( "sequenceid", sequenceID ) == true )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_SEQUENCEINFO );
        action.setSequenceID( sequenceID );
    }
    else if( "updateSequence" == opID )
    {
        std::string sequenceID;

        // Make sure zoneid was provided
        if( opData->getParam( "sequenceid", sequenceID ) == true )
        {
            // zoneid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_SEQUENCEUPDATE );
        action.setSequenceID( sequenceID );

        std::istream& bodyStream = opData->requestBody();
        action.decodeSequenceUpdate( bodyStream );
    }
    else if( "deleteSequence" == opID )
    {
        std::string sequenceID;

        // Make sure zoneid was provided
        if( opData->getParam( "sequenceid", sequenceID ) == true )
        {
            // eventid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_SEQUENCEDELETE );
        action.setSequenceID( sequenceID );
    }
    else if( "getInhibitsList" == opID )
    {
        action.setType( HNID_AR_TYPE_INHIBITSLIST );
    }
    else if( "createInhibit" == opID )
    {
        action.setType( HNID_AR_TYPE_INHIBITCREATE );

        std::istream& bodyStream = opData->requestBody();
        action.decodeInhibitUpdate( bodyStream );
    }
    else if( "getInhibit" == opID )
    {
        std::string inhibitID;

        if( opData->getParam( "inhibitid", inhibitID ) == true )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_INHIBITINFO );
        action.setInhibitID( inhibitID );
    }
    else if( "deleteInhibit" == opID )
    {
        std::string inhibitID;

        // Make sure zoneid was provided
        if( opData->getParam( "inhibitid", inhibitID ) == true )
        {
            // eventid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_INHIBITDELETE );
        action.setInhibitID( inhibitID );
    }    
    else if( "getOperationsList" == opID )
    {
        action.setType( HNID_AR_TYPE_OPERATIONSLIST );
    }
    else if( "createOperation" == opID )
    {
        action.setType( HNID_AR_TYPE_OPERATIONCREATE );

        std::istream& bodyStream = opData->requestBody();
        action.decodeOperationUpdate( bodyStream );
    }
    else if( "getOperation" == opID )
    {
        std::string operationID;

        if( opData->getParam( "operationid", operationID ) == true )
        {
            opData->responseSetStatusAndReason( HNR_HTTP_INTERNAL_SERVER_ERROR );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_OPERATIONINFO );
        action.setOperationID( operationID );
    }
    else if( "cancelOperation" == opID )
    {
        std::string operationID;

        // Make sure zoneid was provided
        if( opData->getParam( "operationid", operationID ) == true )
        {
            // eventid parameter is required
            opData->responseSetStatusAndReason( HNR_HTTP_BAD_REQUEST );
            opData->responseSend();
            return; 
        }

        action.setType( HNID_AR_TYPE_OPERATIONCANCEL );
        action.setOperationID( operationID );
    }    
    else if( "getScheduleInfo" == opID )
    {
        action.setType( HNID_AR_TYPE_SCHINFO );
    }
    else if( "getSchedulerState" == opID )
    {
        action.setType( HNID_AR_TYPE_GETSCHSTATE );
    }
#if 0    
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
#endif
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


