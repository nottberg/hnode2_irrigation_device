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

HNID_AR_TYPE_T
HNIDActionRequest::getType()
{
    return m_type;
}


