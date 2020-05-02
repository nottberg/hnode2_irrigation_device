#include <iostream>

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

void 
HNIrrigationZone::setID( std::string id )
{
    m_zoneID = id;
}

void 
HNIrrigationZone::setName( std::string name )
{
    m_zoneName = name;
}

void 
HNIrrigationZone::setDesc( std::string desc )
{
    m_zoneDesc = desc;
}
       
void 
HNIrrigationZone::setSWIDList( std::string swidList )
{
    m_swidList = swidList;
}

std::string 
HNIrrigationZone::getID()
{
    return m_zoneID;
}

std::string 
HNIrrigationZone::getName()
{
    return m_zoneName;
}

std::string 
HNIrrigationZone::getDesc()
{
    return m_zoneDesc;
}

std::string 
HNIrrigationZone::getSWIDListStr()
{
    return m_swidList;
}

uint 
HNIrrigationZone::getWeeklySeconds()
{
    return (((5 * 60) * 2) * 7);
}

uint 
HNIrrigationZone::getTargetCyclesPerDay()
{
    return 2;
}

uint 
HNIrrigationZone::getMinimumCycleTimeSeconds()
{
    return (2 * 60);
}

