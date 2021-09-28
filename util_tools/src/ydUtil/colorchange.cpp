#include <ydUtil/colorchange.h>
#include <stdlib.h>
#include <stdio.h>

namespace ydUtil 
{
    float G_MAX_TEMPERATURE = 80;
    float G_MIN_TEMPERATURE = 6;
    void  setMinMaxTemperature( float minv, float maxv )
    {
        G_MAX_TEMPERATURE = maxv;
        G_MIN_TEMPERATURE = minv;
    }
    void getMinMaxTemperature( float &minv, float &maxv )
    {
        minv = G_MIN_TEMPERATURE;
        maxv = G_MAX_TEMPERATURE;
    }
    float colorToTemperature( osg::Vec4 c )
    {
        if ( c.a() < 0.5 )
        {
            return INVALID_TEMP;
        }

        double t = 0.0f;
        if ( c.a() < 1.0 && c.a() > 0.5 )
        {
            t = -( c.g() * 255 + c.b() * 255 / 100.0f );
        }else
        {
            t = c.g() * 255 + c.b() * 255 / 100.0f;
        }
        return t;
    }
    osg::Vec4 temperatureToColor( float t )
    {
        if ( t == INVALID_TEMP )
        {
            return osg::Vec4( 0.0, 0.0, 0.0, 0.0 );
        }
        osg::Vec4 c;
        const float maxt = G_MAX_TEMPERATURE;
        const float mint = G_MIN_TEMPERATURE;
        if ( t < mint )
        {
            t = mint;
        }
        if ( t > maxt )
        {
            t = maxt;
        }
        c.r() = ( t - mint ) / ( maxt - mint ) * 255;
        if ( c.r() > 255 )
        {
            c.r() = 255;
        }
        //c.r() = 255;
        int it = t;
        c.g() = abs(it);
        c.b() = ( t - it ) * 100;
        c.a() = 255;
        if ( it < 0 )
        {
            c.a() = 250;
        }
        return c;

    }
    float decodeTemperature1( char ct[] )
    {
        if ( ct )
        {
            unsigned char ht = ct[0];
            unsigned char lt = ct[1];
            /*
              高位表示整数，低位表示小数，基数是-100.00，0.00~255.99对应-100.99~155.99
              高位表示整数，低位表示小数部分，最后的结果减去100就是实际值

              -10.8 表示为   89.2
              10.8  表示为   110.8

            */

            float ft = ht + ( lt / 100.0f ) - 100;
            return ft;
        }
        return 0.0;
    }

}
