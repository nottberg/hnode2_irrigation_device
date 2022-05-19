#ifndef __HN_IRRIGATION_DEVICE_PRIVATE_H__
#define __HN_IRRIGATION_DEVICE_PRIVATE_H__

#include <string>
#include <vector>

#include "Poco/Util/ServerApplication.h"
#include "Poco/Util/OptionSet.h"

#include <hnode2/HNodeDevice.h>
#include <hnode2/HNodeConfig.h>
#include <hnode2/HNEPLoop.h>
#include <hnode2/HNReqWaitQueue.h>

#include "HNSWDPacketClient.h"
#include "HNSWDStatus.h"

//#include "HNIrrigationZone.h"
#include "HNIDActionRequest.h"
#include "HNIrrigationSchedule.h"

#define HNODE_IRRIGATION_DEVTYPE   "hnode2-irrigation-device"

typedef enum HNIrrigationDeviceProcessStateEnum
{
  HNID_STATE_NOINIT,
  HNID_STATE_INITIALIZED,
  HNID_STATE_READY,
  HNID_STATE_CONNECT_RECOVER,
  HNID_STATE_SEND_SET_SCHEDULE,
  HNID_STATE_WAIT_SET_SCHEDULE,
  HNID_STATE_WAIT_SWINFO,
  HNID_STATE_WAIT_SCHCTL,
  HNID_STATE_WAIT_ZONECTL
}HNID_STATE_T;

typedef enum HNIrrigationDeviceResultEnum
{
  HNID_RESULT_SUCCESS,
  HNID_RESULT_FAILURE,
  HNID_RESULT_BAD_REQUEST,
  HNID_RESULT_SERVER_ERROR
}HNID_RESULT_T;

class HNIrrigationDevice : public Poco::Util::ServerApplication, public HNDEPDispatchInf, public HNEPLoopCallbacks
{
    private:
        bool _helpRequested   = false;
        bool _debugLogging    = false;
        bool _instancePresent = false;

        std::string _instance; 

        HNID_STATE_T m_state;

        std::string m_instanceName;

        uint        m_swdFD;
        HNSWDStatus m_swdStatus;
        bool        m_getSWDHealthDetail;

        HNReqWaitQueue    m_actionQueue;
        HNIDActionRequest *m_curAction;

        HNEPLoop       m_evLoop;

        HNodeDevice m_hnodeDev;

        HNIrrigationPlacementSet m_placements;
        HNIrrigationZoneSet     m_zones;
        HNIrrigationSchedule    m_schedule;

        bool m_sendSchedule;

        void displayHelp();

        void setState( HNID_STATE_T value );
        HNID_STATE_T getState();

        bool configExists();
        HNID_RESULT_T initConfig();
        HNID_RESULT_T readConfig();
        HNID_RESULT_T updateConfig();

        bool getUniqueZoneID( HNIDActionRequest *action );
        bool getUniquePlacementID( HNIDActionRequest *action );

        bool openSWDSocket();

        bool handleSWDPacket();

        void startAction();

        void sendScheduleUpdate();

        void handleSWDStatus( HNSWDPacketClient &packet );
        void handleSWDEvent( HNSWDPacketClient &packet );
        void handleSWDScheduleUpdateRsp( HNSWDPacketClient &packet );
        void handleSWDSwitchInfoRsp( HNSWDPacketClient &packet );
        void handleScheduleStateRsp( HNSWDPacketClient &packet );
        void handleZoneCtrlRsp( HNSWDPacketClient &packet );

        //bool getIrrigationStatusJSON( std::ostream &ostr );

    protected:
        // HNDevice REST callback
        virtual void dispatchEP( HNodeDevice *parent, HNOperationData *opData );

        // Event loop callbacks
        virtual void loopIteration();
        virtual void timeoutEvent();
        virtual void fdEvent( int sfd );
        virtual void fdError( int sfd );

        // Poco funcions
        void defineOptions( Poco::Util::OptionSet& options );
        void handleOption( const std::string& name, const std::string& value );
        int main( const std::vector<std::string>& args );

};

#endif // __HN_IRRIGATION_DEVICE_PRIVATE_H__
