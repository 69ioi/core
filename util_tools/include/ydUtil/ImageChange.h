#ifndef __YD_COMMON_IMAGE_CHANGE_H_FILE__
#define __YD_COMMON_IMAGE_CHANGE_H_FILE__
#include <ydUtil/export.h>
#include <osg/Image>

#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include<memory>
namespace ydUtil
{
    YDUTIL_API cv::Mat osgImageToMat( osg::Image *lpImage );
    YDUTIL_API osg::Image *matToOsgImage( cv::Mat const& image );

    YDUTIL_API osg::Image* colorMap( osg::Image *lpImage );
    YDUTIL_API osg::Image* colorMap( std::vector<float> const& temperatureList, int row, int col );
    YDUTIL_API void colorMapCV( std::vector<float> const& temperatureList, int row, int col ,cv::Mat& mat);
    YDUTIL_API osg::Vec4 getColor(float temperature);
}



#endif // __YD_COMMON_IMAGE_CHANGE_H_FILE__
