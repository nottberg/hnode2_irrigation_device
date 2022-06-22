#ifndef __HN_IRRIGATION_INHIBIT_H__
#define __HN_IRRIGATION_INHIBIT_H__

#include <stdint.h>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <mutex>

#include <hnode2/HNodeConfig.h>

#include "HNIrrigationTypes.h"

typedef enum HNIrrigationInhibitTypeEnum
{
    HNII_TYPE_NOTSET          = 0x00,
    HNII_TYPE_LOCAL_DURATION  = 0x01,
    HNII_TYPE_LOCAL_PERCENT   = 0x02    
}HNII_TYPE_T;

class HNIrrigationInhibit
{
    private:
        std::string  m_id;
        std::string  m_name;
        std::string  m_desc;

        HNII_TYPE_T  m_type;
        std::string  m_value;
        std::string  m_zoneid;

    public:
        HNIrrigationInhibit();
       ~HNIrrigationInhibit();

        void setID( std::string id );
        void setName( std::string value );
        void setDesc( std::string value );

        void setType( HNII_TYPE_T type );
        HNIS_RESULT_T setTypeFromStr( std::string typeStr );
        
        void setValue( std::string value );         
        void setZoneID( std::string zone );

        std::string getID();
        std::string getName();
        std::string getDesc();

        HNII_TYPE_T getType();
        std::string getTypeAsStr();
        
        std::string getValue();        
        std::string getZoneID();
        
        HNIS_RESULT_T validateSettings();
        
        double calculateDelta( uint baseDuration, std::string &appliedValue );
};

class HNIrrigationInhibitSet
{
    private:
        // Protect access to the data members
        std::mutex m_accessMutex;

        std::map< std::string, HNIrrigationInhibit > m_inhibitsMap;

        HNIrrigationInhibit* internalUpdateInhibit( std::string id );

    public:
        HNIrrigationInhibitSet();
       ~HNIrrigationInhibitSet();

        bool hasID( std::string id );

        void clear();

        HNIrrigationInhibit* updateInhibit( std::string id );
        void deleteInhibit( std::string inhibitID );
        void getInhibitsList( std::vector< HNIrrigationInhibit > &inhibitsList );
        HNIS_RESULT_T getInhibit( std::string inhibitID, HNIrrigationInhibit &inhibit );
        HNIS_RESULT_T getInhibitName( std::string id, std::string &name );

        void getInhibitsForZone( std::string zoneID, std::vector< HNIrrigationInhibit > &inhibitsList );

        HNIS_RESULT_T initInhibitsListSection( HNodeConfig &cfg );
        HNIS_RESULT_T readInhibitsListSection( HNodeConfig &cfg );
        HNIS_RESULT_T updateInhibitsListSection( HNodeConfig &cfg );
};

#endif // __HN_IRRIGATION_INHIBIT_H__
