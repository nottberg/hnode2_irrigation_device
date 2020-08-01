#ifndef __HNID_ACTION_REQUEST_H__
#define __HNID_ACTION_REQUEST_H__

#include <stdint.h>

#include <string>
#include <mutex>
#include <thread>

#include <hnode2/HNReqWaitQueue.h>

#include "HNIrrigationSchedule.h"

typedef enum HNIDActionRequestType 
{
    HNID_AR_TYPE_NOTSET     = 0,
    HNID_AR_TYPE_SWLIST     = 1,
    HNID_AR_TYPE_ZONELIST   = 2,
    HNID_AR_TYPE_ZONEINFO   = 3,
    HNID_AR_TYPE_ZONECREATE = 4,
    HNID_AR_TYPE_ZONEUPDATE = 5,
    HNID_AR_TYPE_ZONEDELETE = 6
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

        std::vector< HNIrrigationZone > m_zoneList;

        uint m_zoneUpdateMask;

    public:
        HNIDActionRequest();
       ~HNIDActionRequest();

        void setType( HNID_AR_TYPE_T type );
        void setZoneID( std::string value );

        bool setZoneUpdate( std::istream& bodyStream );

        HNID_AR_TYPE_T getType();
        std::string getZoneID();

        void applyZoneUpdate( HNIrrigationZone *tgtZone );

        std::vector< HNIrrigationZone > &refZoneList();
};

#endif // __HNID_ACTION_REQUEST_H__
