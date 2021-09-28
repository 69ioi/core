#ifndef __COMMON_COLOR_CHANGE_H_FILE__
#define __COMMON_COLOR_CHANGE_H_FILE__

#include <ydUtil/export.h>
#include <osg/Vec4>
namespace ydUtil
{
    #define INVALID_TEMP 10000.0f
    #define ATTRIBUTE_INDEX 7
    #define INDEX_ATTRIBUTE ATTRIBUTE_INDEX
    #define INDEX_EMMISIVITY INDEX_ATTRIBUTE + 1
    void YDUTIL_API setMinMaxTemperature( float minv, float maxv );
    void YDUTIL_API getMinMaxTemperature( float &minv, float &maxv );
    float YDUTIL_API colorToTemperature( osg::Vec4 c );
    osg::Vec4 YDUTIL_API temperatureToColor( float t );
    float YDUTIL_API decodeTemperature1( char ct[] );
        
}

#endif // __COMMON_COLOR_CHANGE_H_FILE__
