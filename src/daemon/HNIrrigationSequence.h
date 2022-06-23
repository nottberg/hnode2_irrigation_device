#ifndef __HN_IRRIGATION_SEQUENCE_H__
#define __HN_IRRIGATION_SEQUENCE_H__

#include <stdint.h>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <mutex>

#include <hnode2/HNodeConfig.h>

#include "HNIrrigationTypes.h"

typedef enum HNIrrigationSequenceTypeEnum
{
    HNISQ_TYPE_NOTSET   = 0x00,
    HNISQ_TYPE_UNIFORM  = 0x01,  // A sequence of zones
    HNISQ_TYPE_CHAIN    = 0x02   // A sequence of other Uniform sequences 
}HNISQ_TYPE_T;

class HNIrrigationSequence
{
    private:
        std::string  m_id;
        std::string  m_name;
        std::string  m_desc;

        HNISQ_TYPE_T  m_type;
        uint          m_onDuration;
        uint          m_offDuration;

        std::list< std::string > m_objList;
        
    public:
        HNIrrigationSequence();
       ~HNIrrigationSequence();

        void setID( std::string id );
        void setName( std::string value );
        void setDesc( std::string value );

        void setType( HNISQ_TYPE_T type );
        HNIS_RESULT_T setTypeFromStr( std::string typeStr );
        
        void setOnDuration( uint seconds );
        void setOffDuration( uint seconds );

        void clearObjList();
        void addObj( std::string objID );
        void setObjListFromStr( std::string objListStr );
        
        std::string getID();
        std::string getName();
        std::string getDesc();

        HNISQ_TYPE_T getType();
        std::string  getTypeAsStr();

        uint getOnDuration();
        uint getOffDuration();

        std::list< std::string >& getObjListRef();
        std::string getObjListAsStr();

        HNIS_RESULT_T validateSettings();
        
};

class HNIrrigationSequenceSet
{
    private:
        // Protect access to the data members
        std::mutex m_accessMutex;

        std::map< std::string, HNIrrigationSequence > m_sequencesMap;

        HNIrrigationSequence* internalUpdateSequence( std::string id );

    public:
        HNIrrigationSequenceSet();
       ~HNIrrigationSequenceSet();

        bool hasID( std::string id );

        void clear();

        HNIrrigationSequence* updateSequence( std::string id );
        void deleteSequence( std::string sequenceID );
        void getSequencesList( std::vector< HNIrrigationSequence > &sequencesList );
        HNIS_RESULT_T getSequence( std::string sequenceID, HNIrrigationSequence &sequence );
        HNIS_RESULT_T getSequenceName( std::string id, std::string &name );

        HNIS_RESULT_T initSequencesListSection( HNodeConfig &cfg );
        HNIS_RESULT_T readSequencesListSection( HNodeConfig &cfg );
        HNIS_RESULT_T updateSequencesListSection( HNodeConfig &cfg );
};

#endif // __HN_IRRIGATION_SEQUENCE_H__
