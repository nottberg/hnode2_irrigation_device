#include "HNIDActionRequest.h"

HNIDActionRequest::HNIDActionRequest()
{
    m_type = HNID_AR_TYPE_NOTSET;
}

HNIDActionRequest::~HNIDActionRequest()
{

}

void
HNIDActionRequest::setType( HNID_AR_TYPE_T type )
{
    m_type = type;
}

void 
HNIDActionRequest::setZoneID( std::string value )
{
    m_zoneID = value;
}

HNID_AR_TYPE_T
HNIDActionRequest::getType()
{
    return m_type;
}

std::string 
HNIDActionRequest::getZoneID()
{
    return m_zoneID;
}

std::vector< HNIrrigationZone >&
HNIDActionRequest::refZoneList()
{
    return m_zoneList;
}

