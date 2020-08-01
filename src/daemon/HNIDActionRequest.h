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
    HNID_AR_TYPE_NOTSET   = 0,
    HNID_AR_TYPE_SWLIST   = 1,
    HNID_AR_TYPE_ZONELIST = 2,
    HNID_AR_TYPE_ZONEINFO = 3
}HNID_AR_TYPE_T;

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

    public:
        HNIDActionRequest();
       ~HNIDActionRequest();

        void setType( HNID_AR_TYPE_T type );
        void setZoneID( std::string value );

        HNID_AR_TYPE_T getType();
        std::string getZoneID();

        std::vector< HNIrrigationZone > &refZoneList();
};

#endif // __HNID_ACTION_REQUEST_H__
