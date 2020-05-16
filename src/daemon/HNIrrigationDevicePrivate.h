#ifndef __HN_SWITCH_DAEMON_H__
#define __HN_SWITCH_DAEMON_H__

#include <string>
#include <vector>

#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/OptionSet.h"

#include <hnode2/HNodeDevice.h>

//#include "HNIrrigationZone.h"
#include "HNIrrigationSchedule.h"

typedef enum HNIrrigationDeviceResultEnum
{
  HNID_RESULT_SUCCESS,
  HNID_RESULT_FAILURE
}HNID_RESULT_T;

class HNIrrigationDevice : public Poco::Util::ServerApplication, public HNDEPDispatchInf
{
    private:
        bool _helpRequested   = false;
        bool _debugLogging    = false;
        bool _instancePresent = false;

        std::string _instance; 

        std::string instanceName;

        //std::vector< HNIrrigationZone > zoneList;

        //std::vector< HNExclusionSpec > exclusionList;

        HNIrrigationSchedule schedule;

        void displayHelp();

    protected:
        // HNDevice REST callback
        virtual void dispatchEP( HNodeDevice *parent, HNOperationData *opData );

        // Poco funcions
        void defineOptions( Poco::Util::OptionSet& options );
        void handleOption( const std::string& name, const std::string& value );
        int main( const std::vector<std::string>& args );

};

#endif // __HN_SWITCH_DAEMON_H__
