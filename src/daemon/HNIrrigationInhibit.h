#ifndef __HN_IRRIGATION_INHIBIT_H__
#define __HN_IRRIGATION_INHIBIT_H__

#include <stdint.h>

#include <string>
#include <list>
#include <map>
#include <set>
#include <mutex>

#include <hnode2/HNodeConfig.h>

#include "HNIrrigationTypes.h"

typedef enum HNIrrigationInhibitActionEnum
{
    HNII_INHIBIT_ACTION_NONE     = 1,
    HNII_INHIBIT_ACTION_ACTIVE   = 2,
    HNII_INHIBIT_ACTION_EXPIRED  = 3
}HNII_INHIBIT_ACTION_T;

typedef enum HNIrrigationInhibitTypeEnum
{
    HNII_TYPE_NOTSET              = 0x00,
    HNII_TYPE_SCHEDULER           = 0x01,
    HNII_TYPE_SCHEDULER_NOEXPIRE  = 0x02,    
    HNII_TYPE_ZONE                = 0x03, 
    HNII_TYPE_ZONE_NOEXPIRE       = 0x04       
}HNII_TYPE_T;

class HNIrrigationInhibit
{
    private:
        std::string  m_id;
        std::string  m_name;
        HNII_TYPE_T  m_type;
        std::string  m_zoneid;
        time_t       m_expiration;

    public:
        HNIrrigationInhibit();
       ~HNIrrigationInhibit();

        void setID( std::string id );
        void setName( std::string value );
    
        void setType( HNII_TYPE_T type );
        HNIS_RESULT_T setTypeFromStr( std::string typeStr );
              
        void setZoneID( std::string zone );
        void setExpiration( time_t timeVal );

        void setExpirationFromDurationStr( time_t curTime, std::string durationStr );

        std::string getID();
        std::string getName();
    
        HNII_TYPE_T getType();
        std::string getTypeAsStr();
            
        std::string getZoneID();
        time_t getExpiration();
        std::string getExpirationDateStr();
        
        HNIS_RESULT_T validateSettings();
};

class HNIrrigationInhibitSet
{
    private:
        // Protect access to the data members
        std::mutex m_accessMutex;

        std::map< std::string, HNIrrigationInhibit > m_inhibitsMap;

        std::string m_curSchInhibitID;

        std::map< std::string, std::string > m_zoneIDMap;

        HNIrrigationInhibit* internalUpdateInhibit( std::string id );

    public:
        HNIrrigationInhibitSet();
       ~HNIrrigationInhibitSet();

        bool hasID( std::string id );

        void clear();

        HNIrrigationInhibit* updateInhibit( std::string id );
        
        void reconcileNewObject( std::string newID );

        void deleteInhibit( std::string inhibitID );

        void getInhibitsList( std::vector< HNIrrigationInhibit > &inhibitsList );
        HNIS_RESULT_T getInhibit( std::string inhibitID, HNIrrigationInhibit &inhibit );
        HNIS_RESULT_T getInhibitName( std::string id, std::string &name );
        HNIS_RESULT_T getInhibitExpirationDateStr( std::string id, std::string &dateStr );

        HNIS_RESULT_T getSchedulerInhibitID( std::string &idStr );

        HNII_INHIBIT_ACTION_T checkSchedulerAction( time_t curTime, std::string &inhibitID );
        HNII_INHIBIT_ACTION_T checkZoneAction( time_t curTime, std::string zoneID, std::string &inhibitID );

        HNIS_RESULT_T initInhibitsListSection( HNodeConfig &cfg );
        HNIS_RESULT_T readInhibitsListSection( HNodeConfig &cfg );
        HNIS_RESULT_T updateInhibitsListSection( HNodeConfig &cfg );
};

#endif // __HN_IRRIGATION_INHIBIT_H__
