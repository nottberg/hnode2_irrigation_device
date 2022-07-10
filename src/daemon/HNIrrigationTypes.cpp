#include <regex>

#include "HNIrrigationTypes.h"

HNI24HTime::HNI24HTime()
{
    m_secOfDay = 0;
}

HNI24HTime::~HNI24HTime()
{

}

uint
HNI24HTime::getSeconds() const
{
    return m_secOfDay;
}

void
HNI24HTime::getHMS( uint &hour, uint &minute, uint &second )
{
    hour    = (m_secOfDay / (60 * 60));
    minute  = (m_secOfDay - (hour * 60 * 60))/60;
    second  = m_secOfDay - ((hour * 60 * 60) + (minute * 60));
}

//#include <string.h>

std::string
HNI24HTime::getHMSStr()
{
    std::string result;
    char tmpBuf[256];
    uint hour, minute, second;

    getHMS( hour, minute, second );

//    std::cout << "hms: " << hour << " " << minute << " " << second << std::endl;

    sprintf( tmpBuf, "%2.2d:%2.2d:%2.2d", hour, minute, second );

    //std::cout << "sl: " << strlen(tmpBuf) << "<str>" << tmpBuf << "<eos>" << std::endl;

    result.assign( tmpBuf );

    return result;
}

HNIS_RESULT_T
HNI24HTime::setFromHMS( uint hour, uint min, uint sec )
{
    m_secOfDay = (hour * 60 * 60) + (min * 60) + sec;

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNI24HTime::setFromSeconds( uint seconds )
{
    // Set to provided value, cieling 24 hours.
    m_secOfDay = seconds;
    if( m_secOfDay > HNIS_SECONDS_IN_24H )
        m_secOfDay = HNIS_SECONDS_IN_24H;

    return HNIS_RESULT_SUCCESS;
}

HNIS_RESULT_T 
HNI24HTime::parseTime( std::string value )
{
    const std::regex secRegEx("^[0-9]+$");
    const std::regex durRegEx("^([0-9]+):([0-9]{2}):([0-9]{2})$");
    std::smatch matchRec;

    if( std::regex_match( value, matchRec, secRegEx ) == true )
    {
        m_secOfDay = strtol( matchRec[1].str().c_str(), NULL, 10 );
        return HNIS_RESULT_SUCCESS;
    }
    else if( std::regex_match( value, matchRec, durRegEx ) == true )
    {
        uint hour = strtol( matchRec[3].str().c_str(), NULL, 10 );
        uint min = strtol( matchRec[2].str().c_str(), NULL, 10 );
        uint sec = strtol( matchRec[1].str().c_str(), NULL, 10 );
        return setFromHMS( hour, min, sec );
    }

    return HNIS_RESULT_SUCCESS;
}

void
HNI24HTime::addSeconds( uint seconds )
{
    // Add the two times, cap 24 hours
    m_secOfDay = (m_secOfDay + seconds); 
    if( m_secOfDay > HNIS_SECONDS_IN_24H )
        m_secOfDay = HNIS_SECONDS_IN_24H;
}

void
HNI24HTime::subtractSeconds( uint seconds )
{
    // Subtract seconds, cap at 0 hour

    if( m_secOfDay <= seconds )
        m_secOfDay = 0;

    m_secOfDay = (m_secOfDay - seconds); 
}

