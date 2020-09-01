#ifndef __HNID_ACTION_REQUEST_H__
#define __HNID_ACTION_REQUEST_H__

#include <stdint.h>

#include <string>
#include <mutex>
#include <thread>
#include <sstream>

#include <hnode2/HNReqWaitQueue.h>

#include "HNIrrigationSchedule.h"
#include "HNSWDPacketClient.h"

typedef enum HNIDActionRequestType 
{
    HNID_AR_TYPE_NOTSET     = 0,
    HNID_AR_TYPE_SWLIST     = 1,
    HNID_AR_TYPE_ZONELIST   = 2,
    HNID_AR_TYPE_ZONEINFO   = 3,
    HNID_AR_TYPE_ZONECREATE = 4,
    HNID_AR_TYPE_ZONEUPDATE = 5,
    HNID_AR_TYPE_ZONEDELETE = 6,
    HNID_AR_TYPE_SCHINFO    = 7,
    HNID_AR_TYPE_CRITLIST   = 8,
    HNID_AR_TYPE_CRITINFO   = 9,
    HNID_AR_TYPE_CRITCREATE = 10,
    HNID_AR_TYPE_CRITUPDATE = 11,
    HNID_AR_TYPE_CRITDELETE = 12
}HNID_AR_TYPE_T;

typedef enum HNIDActionZoneUpdateMaskEnum
{
    HNID_ZU_FLDMASK_CLEAR = 0x00000000,
    HNID_ZU_FLDMASK_NAME  = 0x00000001,
    HNID_ZU_FLDMASK_DESC  = 0x00000002,
    HNID_ZU_FLDMASK_SPW   = 0x00000004,
    HNID_ZU_FLDMASK_CPD   = 0x00000008,
    HNID_ZU_FLDMASK_SMC   = 0x00000010,
    HNID_ZU_FLDMASK_SWLST = 0x00000020
}HNID_ZU_FLDMASK_T;

typedef enum HNIDActionCriteriaUpdateMaskEnum
{
    HNID_CU_FLDMASK_CLEAR   = 0x00000000,
    HNID_CU_FLDMASK_TYPE    = 0x00000001,
    HNID_CU_FLDMASK_START   = 0x00000002,
    HNID_CU_FLDMASK_END     = 0x00000004,
    HNID_CU_FLDMASK_DAYNAME = 0x00000008
}HNID_EU_FLDMASK_T;

typedef enum HNIDActionRequestResult
{
    HNID_AR_RESULT_SUCCESS,
    HNID_AR_RESULT_FAILURE
}HNID_AR_RESULT_T;

class HNIDActionRequest : public HNReqWaitAction
{
    private:
        HNID_AR_TYPE_T  m_type;

        std::string m_zoneID;

        std::string m_eventID;

        std::vector< HNIrrigationZone > m_zoneList;
        std::vector< HNScheduleCriteria > m_eventList;
        std::vector< HNSWDSwitchInfo > m_swList;

        uint m_zoneUpdateMask;
        uint m_eventUpdateMask;

        std::stringstream m_rspStream;

    public:
        HNIDActionRequest();
       ~HNIDActionRequest();

        void setType( HNID_AR_TYPE_T type );
        void setZoneID( std::string value );
        void setCriteriaID( std::string value );

        bool setZoneUpdate( std::istream& bodyStream );
        bool setCriteriaUpdate( std::istream& bodyStream );

        HNID_AR_TYPE_T getType();
        std::string getZoneID();
        std::string getCriteriaID();

        void applyZoneUpdate( HNIrrigationZone *tgtZone );
        void applyCriteriaUpdate( HNScheduleCriteria *tgtCriteria );

        std::vector< HNIrrigationZone > &refZoneList();
        std::vector< HNScheduleCriteria > &refCriteriaList();
        std::vector< HNSWDSwitchInfo > &refSwitchList();

        std::stringstream &refRspStream();

        bool hasNewObject( std::string &newID );
        bool hasRspContent( std::string &contentType );
        bool generateRspContent( std::ostream &ostr );
};

#endif // __HNID_ACTION_REQUEST_H__
