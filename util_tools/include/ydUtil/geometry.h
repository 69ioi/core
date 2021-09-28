#ifndef __EXPANSION_GEOMETRY_H__
#define __EXPANSION_GEOMETRY_H__
#include <ydUtil/export.h>
#include <map>
#include <vector>
#include <fstream>
#include <osg/Vec3f>
#include <osg/Vec3d>
#include <osg/BoundingBox>
#include <string.h>
typedef std::pair<int/*lon idx*/,int/*lat idx*/> CellXY;
namespace ydUtil
{
    typedef unsigned short ushort;
    struct CameraData{
        unsigned char datatype[256];
        ushort  width;
        ushort height;
        float   PTZ_x;
        float   PTZ_y;
        float  fovWidth;
        float  fovHight;
    };
    YDUTIL_API float deg2pix(float deg,int pixAll);
    YDUTIL_API float deg2TexCoord(float deg);
    YDUTIL_API float deg2pix_opencv(float deg,int pixAll);
	YDUTIL_API osg::BoundingBox getUnion(const osg::BoundingBox& rectThis,const osg::BoundingBox& rect) ;
	YDUTIL_API CellXY calcCell(int lon,int lat,int lonspan,int latspan);
	YDUTIL_API int16_t float2char(float v);
	YDUTIL_API float char2float(int16_t v);
}

#endif // __EXPANSION_GEOMETRY_H__
