#ifndef __HN_IRRIGATION_OPERATION_H__
#define __HN_IRRIGATION_OPERATION_H__

#include <stdint.h>

#include <string>
#include <list>
#include <map>
#include <mutex>

#include <hnode2/HNodeConfig.h>

#include "HNIrrigationTypes.h"

typedef enum HNIrrigationOperationTypeEnum
{
    HNOP_TYPE_NOTSET          = 0x00,
    HNOP_TYPE_SCHEDULER_STATE = 0x01,  // Change the scheduler enable/disable state
    HNOP_TYPE_EXEC_SEQUENCE   = 0x02,  // Execute a stored sequence
    HNOP_TYPE_EXEC_ONETIMESEQ = 0x03   // Execute a onetime instantaneous sequence    
}HNOP_TYPE_T;

class HNIrrigationOperation
{
    private:
        // The operation object id
        std::string  m_id;

        // The requested operation type   
        HNOP_TYPE_T  m_type;   

        // Scheduler Request - enable/disable state
        bool         m_enable; 

        // One-Time Sequence request - Durations
        HNI24HTime    m_onDuration;  
        HNI24HTime    m_offDuration;

        // Sequence Request object IDs
        std::vector< std::string > m_objIDList; 

    public:
        HNIrrigationOperation();
       ~HNIrrigationOperation();

        void setID( std::string id );

        void setType( HNOP_TYPE_T type );
        HNIS_RESULT_T setTypeFromStr( std::string typeStr );
        
        void setEnable( bool value );   
    
        void setOnDuration( uint seconds );
        HNIS_RESULT_T setOnDurationFromStr( std::string value );

        void setOffDuration( uint seconds );
        HNIS_RESULT_T setOffDurationFromStr( std::string value );

        void clearObjIDList();
        void addObjID( std::string id );
        void addObjIDList( std::vector<std::string> &srcList );

        std::string getID();

        HNOP_TYPE_T getType();
        std::string getTypeAsStr();
        
        bool getEnable();  

        std::string getSeqReqID();

        uint getOnDuration();
        std::string getOnDurationAsStr();

        uint getOffDuration();
        std::string getOffDurationAsStr();

        bool hasObjID( std::string objID );
        std::string getFirstObjID();
        std::string getObjIDListAsStr();
        std::vector< std::string >& getObjIDListRef();

        HNIS_RESULT_T validateSettings();
};

class HNIrrigationOperationQueue
{
    private:
        // Protect access to the data members
        std::mutex m_accessMutex;

        std::list< HNIrrigationOperation > m_opsQueue;

    public:
        HNIrrigationOperationQueue();
       ~HNIrrigationOperationQueue();

        std::string getNextOpID();

        bool hasID( std::string id );

        void clear();

        HNIrrigationOperation* addOperation( std::string id );

        void deleteOperation( std::string opID );
        void getOperationsList( std::vector< HNIrrigationOperation > &opList );
        HNIS_RESULT_T getOperation( std::string opID, HNIrrigationOperation &operation );
};

#endif // __HN_IRRIGATION_OPERATION_H__
