#ifndef __HNID_ACTION_REQUEST_H__
#define __HNID_ACTION_REQUEST_H__

#include <stdint.h>

#include <string>
#include <mutex>
#include <thread>

#include <hnode2/HNReqWaitQueue.h>

typedef enum HNIDActionRequestType 
{
    HNID_AR_TYPE_NOTSET  = 0,
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

    public:
        HNIDActionRequest();
       ~HNIDActionRequest();

        void setType( HNID_AR_TYPE_T type );

        HNID_AR_TYPE_T getType();
};

#endif // __HNID_ACTION_REQUEST_H__
