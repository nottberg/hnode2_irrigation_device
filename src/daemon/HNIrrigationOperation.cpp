#include <iostream>
#include <regex>

#include "HNIrrigationOperation.h"

HNIrrigationOperation::HNIrrigationOperation()
{
    m_type   = HNOP_TYPE_NOTSET;
    m_enable = false;
}

HNIrrigationOperation::~HNIrrigationOperation()
{

}

void 
HNIrrigationOperation::setID( std::string id )
{
    m_id = id;
}

void 
HNIrrigationOperation::setType( HNOP_TYPE_T type )
{
    m_type = type;
}

HNIS_RESULT_T 
HNIrrigationOperation::setTypeFromStr( std::string typeStr )
{
    m_type = HNOP_TYPE_NOTSET;
    
    if( typeStr == "scheduler_state" )
        m_type = HNOP_TYPE_SCHEDULER_STATE;
    else if( typeStr == "exec_sequence" )
        m_type = HNOP_TYPE_EXEC_SEQUENCE;
    else if( typeStr == "exec_onetimeseq" )
        m_type = HNOP_TYPE_EXEC_ONETIMESEQ;
    else
        return HNIS_RESULT_FAILURE;
    
    return HNIS_RESULT_SUCCESS;
}

void 
HNIrrigationOperation::setEnable( bool value )
{
    m_enable = value;
}

void 
HNIrrigationOperation::setOnDuration( uint seconds )
{
    m_onDuration.setFromSeconds( seconds );
}

HNIS_RESULT_T 
HNIrrigationOperation::setOnDurationFromStr( std::string value )
{
    return m_onDuration.parseTime( value );
}

void 
HNIrrigationOperation::setOffDuration( uint seconds )
{
    m_offDuration.setFromSeconds( seconds );
}

HNIS_RESULT_T 
HNIrrigationOperation::setOffDurationFromStr( std::string value )
{
    return m_offDuration.parseTime( value );
}

void 
HNIrrigationOperation::clearObjIDList()
{
    m_objIDList.clear();
}

void 
HNIrrigationOperation::addObjID( std::string id )
{
    m_objIDList.push_back( id );
}
 
void 
HNIrrigationOperation::addObjIDList( std::vector<std::string> &srcList )
{
    for( std::vector<std::string>::iterator it = srcList.begin(); it != srcList.end(); it++ )
    {
        m_objIDList.push_back( *it );
    }
}
        
std::string 
HNIrrigationOperation::getID()
{
    return m_id;
}

HNOP_TYPE_T 
HNIrrigationOperation::getType()
{
    return m_type;
}

std::string 
HNIrrigationOperation::getTypeAsStr()
{
    switch( m_type )
    {
        case HNOP_TYPE_SCHEDULER_STATE:
            return "scheduler_state";
        break;
        
        case HNOP_TYPE_EXEC_SEQUENCE:
            return "exec_sequence";
        break;

        case HNOP_TYPE_EXEC_ONETIMESEQ:
            return "exec_onetimeseq";
        break;

        default:
        break;
    }
    
    return "notset";
}

bool 
HNIrrigationOperation::getEnable()
{
    return m_enable;
}

std::string 
HNIrrigationOperation::getSeqReqID()
{
    if( m_type == HNOP_TYPE_EXEC_SEQUENCE )
        return getFirstObjID();

    return getID();
}

uint
HNIrrigationOperation::getOnDuration()
{
    return m_onDuration.getSeconds();
}

std::string
HNIrrigationOperation::getOnDurationAsStr()
{
    return m_onDuration.getHMSStr();
}

uint
HNIrrigationOperation::getOffDuration()
{
    return m_offDuration.getSeconds();
}

std::string
HNIrrigationOperation::getOffDurationAsStr()
{
    return m_offDuration.getHMSStr();
}

bool 
HNIrrigationOperation::hasObjID( std::string objID )
{
    for( std::vector<std::string>::iterator it = m_objIDList.begin(); it != m_objIDList.end(); it++ )
    {
        if( *it == objID )
            return true;
    }

    return false;
}

std::string 
HNIrrigationOperation::getFirstObjID()
{
    if( m_objIDList.empty() )
        return "";

    return m_objIDList.front();
}

std::string 
HNIrrigationOperation::getObjIDListAsStr()
{
    std::string rspStr;

    bool first = true;
    for( std::vector<std::string>::iterator it = m_objIDList.begin(); it != m_objIDList.end(); it++ )
    {
        if( first == false )
            rspStr += " ";
        rspStr += *it;
        first = false;
    }

    return rspStr;
}

std::vector< std::string >& 
HNIrrigationOperation::getObjIDListRef()
{
    return m_objIDList;
}

HNIS_RESULT_T 
HNIrrigationOperation::validateSettings()
{
    // Add validation checking here
    return HNIS_RESULT_SUCCESS;
}

HNIrrigationOperationQueue::HNIrrigationOperationQueue()
{

}

HNIrrigationOperationQueue::~HNIrrigationOperationQueue()
{

}

void
HNIrrigationOperationQueue::clear()
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    m_opsQueue.clear();
}

bool 
HNIrrigationOperationQueue::hasID( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);
/*
    std::map< std::string, HNIrrigationOperation >::iterator it = m_operationsMap.find( id );

    if( it == m_operationsMap.end() )
        return false;
*/
    return true;
}

HNIrrigationOperation*
HNIrrigationOperationQueue::addOperation( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    HNIrrigationOperation newOp;
    newOp.setID( id );
    m_opsQueue.push_back( newOp );

    return &(m_opsQueue.back());
}

void 
HNIrrigationOperationQueue::deleteOperation( std::string id )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Find the referenced operation
    for( std::list< HNIrrigationOperation >::iterator it = m_opsQueue.begin(); it != m_opsQueue.end(); it++ )
    {
        if( it->getID() == id )
        {
            m_opsQueue.erase( it );
            return;
        }
    }
}

void 
HNIrrigationOperationQueue::getOperationsList( std::vector< HNIrrigationOperation > &operationsList )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    for( std::list< HNIrrigationOperation >::iterator it = m_opsQueue.begin(); it != m_opsQueue.end(); it++ )
    {
        operationsList.push_back( *it );
    }
}

HNIS_RESULT_T 
HNIrrigationOperationQueue::getOperation( std::string id, HNIrrigationOperation &event )
{
    // Scope lock
    const std::lock_guard<std::mutex> lock(m_accessMutex);

    // Find the referenced operation
    for( std::list< HNIrrigationOperation >::iterator it = m_opsQueue.begin(); it != m_opsQueue.end(); it++ )
    {
        if( it->getID() == id )
        {
            event = *it;
            return HNIS_RESULT_SUCCESS;
        }
    }

    return HNIS_RESULT_FAILURE;
}
