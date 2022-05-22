#ifndef __HNID_ACTION_REQUEST_H__
#define __HNID_ACTION_REQUEST_H__

#include <stdint.h>

#include <string>
#include <mutex>
#include <thread>
#include <sstream>

#include <hnode2/HNReqWaitQueue.h>

#include "HNIrrigationZone.h"
#include "HNIrrigationPlacement.h"
#include "HNIrrigationModifier.h"
#include "HNIrrigationSchedule.h"
#include "HNSWDPacketClient.h"

typedef enum HNIDActionRequestType 
{
    HNID_AR_TYPE_NOTSET         = 0,
    HNID_AR_TYPE_SWLIST         = 1,
    HNID_AR_TYPE_ZONELIST       = 2,
    HNID_AR_TYPE_ZONEINFO       = 3,
    HNID_AR_TYPE_ZONECREATE     = 4,
    HNID_AR_TYPE_ZONEUPDATE     = 5,
    HNID_AR_TYPE_ZONEDELETE     = 6,
    HNID_AR_TYPE_SCHINFO        = 7,
    HNID_AR_TYPE_GETSCHSTATE    = 8,
    HNID_AR_TYPE_SETSCHSTATE    = 9,
    HNID_AR_TYPE_PLACELIST      = 10,
    HNID_AR_TYPE_PLACEINFO      = 11,
    HNID_AR_TYPE_PLACECREATE    = 12,
    HNID_AR_TYPE_PLACEUPDATE    = 13,
    HNID_AR_TYPE_PLACEDELETE    = 14,
    HNID_AR_TYPE_IRRSTATUS      = 15,
    HNID_AR_TYPE_ZONECTL        = 16,
    HNID_AR_TYPE_MODIFIERSLIST  = 17,
    HNID_AR_TYPE_MODIFIERINFO   = 18,
    HNID_AR_TYPE_MODIFIERCREATE = 19,
    HNID_AR_TYPE_MODIFIERUPDATE = 20,
    HNID_AR_TYPE_MODIFIERDELETE = 21
}HNID_AR_TYPE_T;

// Change scheduling state. Enable/Disable/Inhibit
typedef enum HNIDScheduleStateRequestTypeEnum
{
    HNID_SSR_NOTSET,
    HNID_SSR_ENABLE,    
    HNID_SSR_DISABLE,
    HNID_SSR_INHIBIT
}HNID_SSR_T;

// Control requests for a single zone
typedef enum HNIDZoneControlRequestTypeEnum
{
    HNID_ZCR_NOTSET,
    HNID_ZCR_SEQADD,    
    HNID_ZCR_SEQCANCEL, 
    HNID_ZCR_ENABLE,    
    HNID_ZCR_DISABLE,   
    HNID_ZCR_INHIBIT    
}HNID_ZCR_T;

typedef enum HNIDActionZoneUpdateMaskEnum
{
    HNID_ZU_FLDMASK_CLEAR  = 0x00000000,
    HNID_ZU_FLDMASK_NAME   = 0x00000001,
    HNID_ZU_FLDMASK_DESC   = 0x00000002,
    HNID_ZU_FLDMASK_SPW    = 0x00000004,
    HNID_ZU_FLDMASK_MAXSPC = 0x00000008,
    HNID_ZU_FLDMASK_MINSPC = 0x00000010,
    HNID_ZU_FLDMASK_SWLST  = 0x00000020
}HNID_ZU_FLDMASK_T;

typedef enum HNIDActionPlacementUpdateMaskEnum
{
    HNID_PU_FLDMASK_CLEAR    = 0x00000000,
    HNID_PU_FLDMASK_NAME     = 0x00000001,
    HNID_PU_FLDMASK_DESC     = 0x00000002,
    HNID_PU_FLDMASK_TYPE     = 0x00000004,
    HNID_PU_FLDMASK_START    = 0x00000008,
    HNID_PU_FLDMASK_END      = 0x00000010,
    HNID_PU_FLDMASK_RANK     = 0x00000020,
    HNID_PU_FLDMASK_DAYBITS  = 0x00000040,
    HNID_PU_FLDMASK_ZONELIST = 0x00000080
}HNID_PU_FLDMASK_T;

typedef enum HNIDActionModifierUpdateMaskEnum
{
    HNID_MU_FLDMASK_CLEAR    = 0x00000000,
    HNID_MU_FLDMASK_NAME     = 0x00000001,
    HNID_MU_FLDMASK_DESC     = 0x00000002,
    HNID_MU_FLDMASK_TYPE     = 0x00000004,
    HNID_MU_FLDMASK_VALUE    = 0x00000008,
    HNID_MU_FLDMASK_ZONEID   = 0x00000010
}HNID_MU_FLDMASK_T;

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
        std::string m_placementID;
        std::string m_modifierID;

        std::vector< HNIrrigationZone > m_zoneList;
        std::vector< HNIrrigationPlacement > m_placementsList;
        std::vector< HNIrrigationModifier > m_modifiersList;        
        std::vector< HNSWDSwitchInfo > m_swList;

        uint m_zoneUpdateMask;
        uint m_placementUpdateMask;
        uint m_modifierUpdateMask;       

        std::stringstream m_rspStream;

        HNID_SSR_T m_schReqType;
        HNID_ZCR_T m_zoneReqType;

        std::string m_inhibitDuration;

        std::string m_onDuration;
        std::string m_offDuration;

    public:
        HNIDActionRequest();
       ~HNIDActionRequest();

        void setType( HNID_AR_TYPE_T type );
        void setZoneID( std::string value );
        void setPlacementID( std::string value );
        void setModifierID( std::string value );

        bool decodeZoneUpdate( std::istream& bodyStream );
        bool decodePlacementUpdate( std::istream& bodyStream );
        bool decodeModifierUpdate( std::istream& bodyStream );       
        bool decodeSchedulerState( std::istream& bodyStream );
        bool decodeZoneCtrl( std::istream& bodyStream );

        void setScheduleStateRequestType( HNID_SSR_T value );
        void setZoneControlRequestType( HNID_ZCR_T value );

        void setInhibitDuration( std::string value );
        void setOnDuration( std::string value );
        void setOffDuration( std::string value );

        HNID_AR_TYPE_T getType();
        std::string getZoneID();
        std::string getPlacementID();
        std::string getModifierID();        

        HNID_SSR_T getScheduleStateRequestType();
        HNID_ZCR_T getZoneControlRequestType();

        std::string getInhibitDuration();
        std::string getOnDuration();
        std::string getOffDuration();

        void applyZoneUpdate( HNIrrigationZone *tgtZone );
        void applyPlacementUpdate( HNIrrigationPlacement *tgtPlacement );
        void applyModifierUpdate( HNIrrigationModifier *tgtModifier );

        std::vector< HNIrrigationZone > &refZoneList();
        std::vector< HNIrrigationPlacement > &refPlacementsList();
        std::vector< HNIrrigationModifier > &refModifiersList();       
        std::vector< HNSWDSwitchInfo > &refSwitchList();

        std::stringstream &refRspStream();

        bool hasNewObject( std::string &newID );
        bool hasRspContent( std::string &contentType );
        bool generateRspContent( std::ostream &ostr );
};

#endif // __HNID_ACTION_REQUEST_H__
