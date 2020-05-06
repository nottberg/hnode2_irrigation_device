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

    hnDevice.start();

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

    waitForTerminationRequest();

    return Application::EXIT_OK;
}



