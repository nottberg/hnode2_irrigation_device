#ifndef __HN_IRRIGATION_SEQUENCE_H__
#define __HN_IRRIGATION_SEQUENCE_H__

#include <stdint.h>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <mutex>

#include <hnode2/HNodeConfig.h>

#include "HNIrrigationTypes.h"

typedef enum HNIrrigationSequenceTypeEnum
{
    HNISQ_TYPE_NOTSET          = 0x00,
    HNISQ_TYPE_LOCAL_DURATION  = 0x01,
    HNISQ_TYPE_LOCAL_PERCENT   = 0x02    
}HNISQ_TYPE_T;

class HNIrrigationSequence
{
    private:
        std::string  m_id;
        std::string  m_name;
        std::string  m_desc;

        HNISQ_TYPE_T  m_type;
        std::string  m_value;
        std::string  m_zoneid;

    public:
        HNIrrigationSequence();
       ~HNIrrigationSequence();

        void setID( std::string id );
        void setName( std::string value );
        void setDesc( std::string value );

        void setType( HNISQ_TYPE_T type );
        HNIS_RESULT_T setTypeFromStr( std::string typeStr );
        
        void setValue( std::string value );         
        void setZoneID( std::string zone );

        std::string getID();
        std::string getName();
        std::string getDesc();

        HNISQ_TYPE_T getType();
        std::string getTypeAsStr();
        
        std::string getValue();        
        std::string getZoneID();
        
        HNIS_RESULT_T validateSettings();
        
        double calculateDelta( uint baseDuration, std::string &appliedValue );
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

        void getSequencesForZone( std::string zoneID, std::vector< HNIrrigationSequence > &sequencesList );

        HNIS_RESULT_T initSequencesListSection( HNodeConfig &cfg );
        HNIS_RESULT_T readSequencesListSection( HNodeConfig &cfg );
        HNIS_RESULT_T updateSequencesListSection( HNodeConfig &cfg );
};

#endif // __HN_IRRIGATION_SEQUENCE_H__
