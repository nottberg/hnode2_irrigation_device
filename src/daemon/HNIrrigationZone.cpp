#include "HNIrrigationZone.h"

HNI24HTime::HNI24HTime()
{
    secOfDay = 0;
}

HNI24HTime::~HNI24HTime()
{

}

uint
HNI24HTime::getSeconds() const
{
    return secOfDay;
}

void
HNI24HTime::getHMS( uint &hour, uint &minute, uint &second )
{
    hour    = (secOfDay / (60 * 60));
    minute  = (secOfDay - (hour * 60 * 60))/60;
    second  = secOfDay - ((hour * 60 * 60) + (minute * 60));
}

std::string
HNI24HTime::getHMSStr()
{
    char tmpBuf[128];
    uint hour, minute, second;

    getHMS( hour, minute, second );
    sprintf( tmpBuf, "%2.2d:%2.2d:%2.2d", hour, minute, second );

    std::string result( tmpBuf );
    return result;
}

HNIZ_RESULT_T
HNI24HTime::setFromHMS( uint hour, uint min, uint sec )
{
    secOfDay = (hour * 60 * 60) + (min *60) + sec;

    return HNIZ_RESULT_SUCCESS;
}

HNIZ_RESULT_T 
HNI24HTime::setFromSeconds( uint seconds )
{
    // Set to provided value, cieling 24 hours.
    secOfDay = seconds;
    if( secOfDay > (24 * 60 * 60) )
        secOfDay = (24 * 60 * 60);

    return HNIZ_RESULT_SUCCESS;
}

HNIZ_RESULT_T 
HNI24HTime::parseTime( std::string value )
{
    uint hour;
    uint min;
    uint sec;

    sscanf( value.c_str(), "%d:%d:%d", &hour, &min, &sec );

    return setFromHMS( hour, min, sec );
}

void
HNI24HTime::addSeconds( uint seconds )
{
    // Add the two times, cap 24 hours
    secOfDay = (secOfDay + seconds); 
    if( secOfDay > (24 * 60 * 60) )
        secOfDay = (24 * 60 * 60);

}

HNIrrigationZone::HNIrrigationZone()
{

}

HNIrrigationZone::~HNIrrigationZone()
{

}

