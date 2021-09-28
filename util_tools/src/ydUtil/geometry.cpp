#include "ydUtil/common.h"

#ifdef _WIN32
#include <regex>
#include <atlstr.h>
#include <direct.h>
#include <corecrt_io.h>
#else
#include <unistd.h>
#include <sys/stat.h>
#include <ydUtil/console.h>
#endif
#include <ydUtil/log.h>
#include <osg/BoundingBox>
#include "ydUtil/geometry.h"
namespace ydUtil
{
    float deg2pix(float deg,int pixAll)
    {
//        float yOffset = ((sin(osg::DegreesToRadians((deg-90)))+1)*0.5)*pixAll;
        float yOffset = ((((deg/180.0))))*pixAll;
        return yOffset;
    }
    float deg2TexCoord(float deg)
    {
        float yOffset = ((sin(osg::DegreesToRadians((deg-90)))+1)*0.5);
        return yOffset;
    }
    float deg2pix_opencv(float deg,int pixAll)
    {
//        float yOffset = (1-(sin(osg::DegreesToRadians((deg-90)))+1)*0.5)*pixAll;
        float yOffset = (1-deg/180.0)*pixAll;
        return yOffset;
    }
	osg::BoundingBox getUnion(const osg::BoundingBox& rectThis,const osg::BoundingBox& rect) 
    {
        bool con1 = (rectThis.xMax() <= rect.xMin()) ||
            (rectThis.xMin() >= rect.xMax()) ||
            (rectThis.yMin() >= rect.yMax())  ||
            (rectThis.yMax() <= rect.yMin());
        bool con2 = (rect.xMax() <= rectThis.xMin())  ||
            (rect.xMin() >= rectThis.xMax())  ||
            (rect.yMin() >= rectThis.yMax())  ||
            (rect.yMax() <= rectThis.yMin());
        // check for total exclusion
        if (con1||con2)
        {
            return osg::BoundingBox(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        }
        else
        {
            osg::BoundingBox temp;

            // fill in temp with the intersection
            temp.xMin() = (rectThis.xMin() > rect.xMin()) ? rectThis.xMin() : rect.xMin();
            temp.yMin() = (rectThis.yMin() > rect.yMin()) ? rectThis.yMin() : rect.yMin();
            temp.xMax() = (rectThis.xMax() < rect.xMax()) ? rectThis.xMax()  : rect.xMax();
            temp.yMax() = (rectThis.yMax() < rect.yMax()) ? rectThis.yMax() : rect.yMax();

            return temp;

        }
    }
    CellXY calcCell(int lon,int lat,int lonspan,int latspan)
    {
        CellXY cellXY(lon/lonspan,lat/latspan);

        return cellXY;
    }
    int16_t float2char(float v)
    {
        #if 1
        return (int16_t) v;
        #else
        return v * 100;
        #endif
    }
    float char2float(int16_t v)
    {
    #if 1
     return (float)v;
     #else
     return (float)v*0.01;
     #endif
    }
    }
