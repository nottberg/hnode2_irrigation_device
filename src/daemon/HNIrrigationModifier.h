#ifndef __HN_IRRIGATION_MODIFIER_H__
#define __HN_IRRIGATION_MODIFIER_H__

#include <stdint.h>

#include <string>
#include <vector>
#include <map>
#include <set>
#include <mutex>

#include <hnode2/HNodeConfig.h>

#include "HNIrrigationTypes.h"

typedef enum HNIrrigationModifierTypeEnum
{
    HNIM_TYPE_NOTSET          = 0x00,
    HNIM_TYPE_LOCAL_DURATION  = 0x01,
    HNIM_TYPE_LOCAL_PERCENT   = 0x02    
}HNIM_TYPE_T;

class HNIrrigationModifier
{
    private:
        std::string  m_id;
        std::string  m_name;
        std::string  m_desc;

        HNIM_TYPE_T  m_type;
        std::string  m_value;
        std::string  m_zoneid;

    public:
        HNIrrigationModifier();
       ~HNIrrigationModifier();

        void setID( std::string id );
        void setName( std::string value );
        void setDesc( std::string value );

        void setType( HNIM_TYPE_T type );
        HNIS_RESULT_T setTypeFromStr( std::string typeStr );
        
        void setValue( std::string value );         
        void setZoneID( std::string zone );

        std::string getID();
        std::string getName();
        std::string getDesc();

        HNIM_TYPE_T getType();
        std::string getTypeAsStr();
        
        std::string getValue();        
        std::string getZoneID();
        
        HNIS_RESULT_T validateSettings();
        
        double calculateDelta( uint baseDuration, std::string &appliedValue );
};

class HNIrrigationModifierSet
{
    private:
        // Protect access to the data members
        std::mutex m_accessMutex;

        std::map< std::string, HNIrrigationModifier > m_modifiersMap;

        HNIrrigationModifier* internalUpdateModifier( std::string id );

    public:
        HNIrrigationModifierSet();
       ~HNIrrigationModifierSet();

        bool hasID( std::string id );

        void clear();

        HNIrrigationModifier* updateModifier( std::string id );
        void deleteModifier( std::string modifierID );
        void getModifiersList( std::vector< HNIrrigationModifier > &modifiersList );
        HNIS_RESULT_T getModifier( std::string modifierID, HNIrrigationModifier &modifier );
        HNIS_RESULT_T getModifierName( std::string id, std::string &name );

        void getModifiersForZone( std::string zoneID, std::vector< HNIrrigationModifier > &modifiersList );

        HNIS_RESULT_T initModifiersListSection( HNodeConfig &cfg );
        HNIS_RESULT_T readModifiersListSection( HNodeConfig &cfg );
        HNIS_RESULT_T updateModifiersListSection( HNodeConfig &cfg );
};

#endif // __HN_IRRIGATION_MODIFIER_H__
