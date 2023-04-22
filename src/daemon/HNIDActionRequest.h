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
#include "HNIrrigationSequence.h"
#include "HNIrrigationInhibit.h"
#include "HNIrrigationOperation.h"
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
    HNID_AR_TYPE_MODIFIERSLIST  = 16,
    HNID_AR_TYPE_MODIFIERINFO   = 17,
    HNID_AR_TYPE_MODIFIERCREATE = 18,
    HNID_AR_TYPE_MODIFIERUPDATE = 19,
    HNID_AR_TYPE_MODIFIERDELETE = 20,
    HNID_AR_TYPE_SEQUENCESLIST  = 21,
    HNID_AR_TYPE_SEQUENCEINFO   = 22,
    HNID_AR_TYPE_SEQUENCECREATE = 23,
    HNID_AR_TYPE_SEQUENCEUPDATE = 24,
    HNID_AR_TYPE_SEQUENCEDELETE = 25,
    HNID_AR_TYPE_INHIBITSLIST   = 26,
    HNID_AR_TYPE_INHIBITINFO    = 27,
    HNID_AR_TYPE_INHIBITCREATE  = 28,
    HNID_AR_TYPE_INHIBITDELETE  = 29,
    HNID_AR_TYPE_OPERATIONSLIST  = 30,
    HNID_AR_TYPE_OPERATIONINFO   = 31,
    HNID_AR_TYPE_OPERATIONCREATE = 32,
    HNID_AR_TYPE_OPERATIONCANCEL = 33    
}HNID_AR_TYPE_T;

#if 0
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
#endif

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

typedef enum HNIDActionSequenceUpdateMaskEnum
{
    HNID_SQU_FLDMASK_CLEAR      = 0x00000000,
    HNID_SQU_FLDMASK_NAME       = 0x00000001,
    HNID_SQU_FLDMASK_DESC       = 0x00000002,
    HNID_SQU_FLDMASK_TYPE       = 0x00000004,
    HNID_SQU_FLDMASK_ONDUR      = 0x00000008,
    HNID_SQU_FLDMASK_OFFDUR     = 0x00000010,
    HNID_SQU_FLDMASK_OBJIDLIST  = 0x00000020
}HNID_SQU_FLDMASK_T;

typedef enum HNIDActionInhibitUpdateMaskEnum
{
    HNID_INU_FLDMASK_CLEAR      = 0x00000000,
    HNID_INU_FLDMASK_NAME       = 0x00000001,
    HNID_INU_FLDMASK_TYPE       = 0x00000002,
    HNID_INU_FLDMASK_EXPIRATION = 0x00000004,
    HNID_INU_FLDMASK_ZONEID     = 0x00000008
}HNID_INU_FLDMASK_T;

typedef enum HNIDActionOperationUpdateMaskEnum
{
    HNID_OPU_FLDMASK_CLEAR           = 0x00000000,
    HNID_OPU_FLDMASK_TYPE            = 0x00000002,
    HNID_OPU_FLDMASK_ONDUR           = 0x00000004,
    HNID_OPU_FLDMASK_OFFDUR          = 0x00000008,
    HNID_OPU_FLDMASK_OBJLIST         = 0x00000010,
    HNID_OPU_FLDMASK_SCHEDULER_STATE = 0x00000020
}HNID_OPU_FLDMASK_T;

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
        std::string m_sequenceID;
        std::string m_inhibitID;
        std::string m_operationID;

        std::vector< HNIrrigationZone > m_zoneList;
        std::vector< HNIrrigationPlacement > m_placementsList;
        std::vector< HNIrrigationModifier > m_modifiersList;
        std::vector< HNIrrigationSequence > m_sequencesList;
        std::vector< HNIrrigationInhibit > m_inhibitsList;
        std::vector< HNIrrigationOperation > m_operationsList;
        std::vector< HNSWDSwitchInfo > m_swList;

        uint m_zoneUpdateMask;
        uint m_placementUpdateMask;
        uint m_modifierUpdateMask;
        uint m_sequenceUpdateMask;
        uint m_inhibitUpdateMask;
        uint m_operationUpdateMask;

        std::stringstream m_rspStream;

    public:
        HNIDActionRequest();
       ~HNIDActionRequest();

        void setType( HNID_AR_TYPE_T type );

        void setZoneID( std::string value );
        void setPlacementID( std::string value );
        void setModifierID( std::string value );
        void setSequenceID( std::string value );
        void setInhibitID( std::string value );
        void setOperationID( std::string value );

        bool decodeZoneUpdate( std::istream& bodyStream );
        bool decodePlacementUpdate( std::istream& bodyStream );
        bool decodeModifierUpdate( std::istream& bodyStream );
        bool decodeSequenceUpdate( std::istream& bodyStream );
        bool decodeInhibitUpdate( std::istream& bodyStream );
        bool decodeOperationUpdate( std::istream& bodyStream );

        HNID_AR_TYPE_T getType();

        std::string getZoneID();
        std::string getPlacementID();
        std::string getModifierID();
        std::string getSequenceID();
        std::string getOperationID();
        std::string getInhibitID();

        void applyZoneUpdate( HNIrrigationZone *tgtZone );
        void applyPlacementUpdate( HNIrrigationPlacement *tgtPlacement );
        void applyModifierUpdate( HNIrrigationModifier *tgtModifier );
        void applySequenceUpdate( HNIrrigationSequence *tgtSequence );
        void applyInhibitUpdate( HNIrrigationInhibit *tgtInhibit );
        void applyOperationUpdate( HNIrrigationOperation *tgtOperation );

        std::vector< HNIrrigationZone > &refZoneList();
        std::vector< HNIrrigationPlacement > &refPlacementsList();
        std::vector< HNIrrigationModifier > &refModifiersList();
        std::vector< HNIrrigationSequence > &refSequencesList();
        std::vector< HNIrrigationInhibit > &refInhibitsList();
        std::vector< HNIrrigationOperation > &refOperationsList();

        std::vector< HNSWDSwitchInfo > &refSwitchList();

        std::stringstream &refRspStream();

        bool hasNewObject( std::string &newID );
        bool hasRspContent( std::string &contentType );
        bool generateRspContent( std::ostream &ostr );
};

#endif // __HNID_ACTION_REQUEST_H__
