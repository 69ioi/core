
#include <opencv/cv.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <ydUtil/colorchange.h>
#include <ydUtil/ImageChange.h>
#include <osg/Vec4b>
#include <mutex>
namespace ydUtil
{

    float clamp (float value)
    {
        return value > 255 ? 255 : (value < 0 ? 0 : value);
    }


    osg::Vec4 getColor(float temperature)
    {
        float red = 0, green = 0, blue = 0;
        float percentKelvin = temperature;;
        osg::Vec4 c;

        float x = temperature;
        if ( x < 66 )   // b_1
        {
            blue = pow(x,2.5)/125 + 70;
        }else if ( x < 200 ) // b_2
        {
            blue = log(x-50) *-120 + 280 + 320;
        }else
        {
            blue = 0;
        }
        blue = clamp(blue);

        if ( x <= 130 ) // g_1
        {
            green = pow(x,2)/60 + 30;
        }else if( x > 130 && x <= 255)  // g_2
        {
            green=log(x-100) * -100 + 340 + 250;
        }else
        {
            green = 173 * pow((x-100),-0.2) / 2 + 25;
        }

        green = clamp(green);

        if ( x <= 44 )
        {
            red = 0;
        }
        if ( x > 44 && x < 200 )
        {
            red = pow((x-44),1.5)/3;
        }
        if ( x >= 200 )
        {
            red = cos((x-200)/(255-200)* 3.1415926) * 0.5 * 127 + 190;
            //red = 255;
        }

        red = clamp(red);

        //red = clamp(percentKelvin <= 66 ? 255 : (329.698727446 * pow(percentKelvin - 60, -0.1332047592)));
        //green = clamp(percentKelvin <= 66 ? (99.4708025861 * log(percentKelvin) - 161.1195681661) : 288.1221695283 * pow(percentKelvin, -0.0755148492));
        //blue = clamp(percentKelvin >= 66 ? 255 : (percentKelvin <= 19 ? 0 : 138.5177312231 * log(percentKelvin - 10) - 305.0447927307));

        //green = 0;
        //red = 0;

        c  = osg::Vec4(red / 255, green / 255, blue / 255, 1.0);
        return c;

    }
    osg::Image *matToOsgImage( cv::Mat const& src )
    {
        osg::Image *lpImage = new osg::Image;

        int y = src.rows;
        int x = src.cols;

        lpImage->allocateImage(x, y, 1, GL_RGBA, GL_UNSIGNED_BYTE);

#pragma omp parallel for
        for ( int i = 0; i < y; i ++ )
        {
            for ( int j = 0; j < x; j ++ )
            {
                osg::Vec4 c;
                c.b() = src.at<cv::Vec3b>(y - 1 - i,j)[0] / 255.0f;
                c.g() = src.at<cv::Vec3b>(y - 1 - i,j)[1] / 255.0f;
                c.r() = src.at<cv::Vec3b>(y - 1 - i,j)[2] / 255.0f;
                c.a() = 1.0f;

                lpImage->setColor(c, j, i );
            }
        }

        return lpImage;
    }
    cv::Mat osgImageToMat( osg::Image *lpImage )
    {
        int y = lpImage->t();
        int x = lpImage->s();

        cv::Mat src(y, x, CV_8UC3);
#pragma omp parallel for
        for ( int i = 0; i < y; i ++ )
        {
            for ( int j = 0; j < x; j ++ )
            {
                osg::Vec4 c = lpImage->getColor( j, i );
                src.at<cv::Vec3b>(y - 1 - i,j)[0] = c.b() * 255;
                src.at<cv::Vec3b>(y - 1 - i,j)[1] = c.g() * 255;
                src.at<cv::Vec3b>(y - 1 - i,j)[2] = c.r() * 255;
            }
        }

        return src;
    }

#define LinearValue(vmin, vmax, pmin, pmax, cp) ((float)(cp - pmin) / ( pmax - pmin ) * ( vmax - vmin ) + vmin)
    //int getRangeValue( unsigned char *lut, osg::Vec4b minv, osg::Vec4b maxv, int minp, int maxp )
    void getRangeValue( cv::Mat &lut, osg::Vec4f minv, osg::Vec4f maxv, int minp, int maxp )
    {
        for (int i = minp; i < maxp; i ++)
        {
            lut.at<cv::Vec3b>(i)[ 2 ] = LinearValue( minv.r(), maxv.r(), minp, maxp, i  );
            lut.at<cv::Vec3b>(i)[ 1 ] = LinearValue( minv.g(), maxv.g(), minp, maxp, i  );
            lut.at<cv::Vec3b>(i)[ 0 ] = LinearValue( minv.b(), maxv.b(), minp, maxp, i  );
        }
    }

    /*
    osg::Image *matToOsgImage( cv::Mat const& src, cv::Mat const& colorMat )
    {
        osg::Image *lpImage = new osg::Image;

        int y = src.rows;
        int x = src.cols;

        lpImage->allocateImage(x, y, 1, GL_RGBA, GL_UNSIGNED_BYTE);

//#pragma omp parallel for
        for ( int i = 0; i < y; i ++ )
        {
            for ( int j = 0; j < x; j ++ )
            {
                osg::Vec4 c;

                c.b() = (src.at<cv::Vec3b>(y - 1 - i,j)[0] / 255.0f * 0.8 + colorMat.at<cv::Vec3b>(y - 1 - i,j)[0] / 255.0f ) / 2 * 0.2;
                c.g() = (src.at<cv::Vec3b>(y - 1 - i,j)[1] / 255.0f * 0.8 + colorMat.at<cv::Vec3b>(y - 1 - i,j)[1] / 255.0f) / 2 * 0.2;
                c.r() = (src.at<cv::Vec3b>(y - 1 - i,j)[2] / 255.0f * 0.8 + colorMat.at<cv::Vec3b>(y - 1 - i,j)[2] / 255.0f ) / 2 * 0.2;

                c.a() = 1.0f;

                lpImage->setColor(c * 2, j, i );
            }
        }

        return lpImage;
    }

    osg::Image* colorMap( osg::Image *lpImage)
    {
        cv::Mat m;

        int y = lpImage->t();
        int x = lpImage->s();

        cv::Mat tempMat(y, x, CV_32FC1);
        cv::Mat src(y, x, CV_8UC3);
        tempMat.setTo(0);

        float maxv = FLT_MIN;
        float minv = FLT_MAX;
        for ( int i = 0; i < y; i ++ )
        {
            for ( int j = 0; j < x; j ++ )
            {
                osg::Vec4 c = lpImage->getColor( j, i );
                float t = ydCommon::colorToTemperature( c );
                t = log( t );
                if ( maxv < t )
                {
                    maxv = t;
                }

                if ( minv > t )
                {
                    minv = t;
                }
            }
        }

        float r = maxv - minv;
        if ( r == 0.0f )
        {
            r = 1.0f;
        }

        const int arraySize = 20000;
        std::vector<int> cc(arraySize);
        memset( &cc[0], 0, arraySize * sizeof( int ) );
        std::mutex mutex;
#pragma omp parallel for
        for ( int i = 0; i < y; i ++ )
        {
            for ( int j = 0; j < x; j ++ )
            {
                osg::Vec4 c = lpImage->getColor( j, i );
                float t = ydCommon::colorToTemperature( c );
                t = log ( t );
                float v = ( t - minv ) / r;
                if ( v > 1.0f )
                {
                    v = 1.0f;
                }
                if ( v < 0 )
                {
                    v = 0.0f;
                }
                tempMat.at<float>(y - 1 - i,j) = v * ( arraySize - 1 );
                {
                    std::lock_guard<std::mutex> l(mutex);
                    cc[(int)(v * (arraySize - 1))] += 1;
                }
                src.at<cv::Vec3b>(y - 1 - i,j)[0] = v * 255;
                src.at<cv::Vec3b>(y - 1 - i,j)[1] = v * 255;
                src.at<cv::Vec3b>(y - 1 - i,j)[2] = v * 255;
            }
        }
        static int index = 0;
        std::vector<int>compression_params;
        compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
        compression_params.push_back(100);

        //cv::Mat oriImg;
        //cv::applyColorMap( src, oriImg, cv::COLORMAP_JET );
        //cv::imwrite(("/home/fosky/data/img/test" + std::to_string( index ) + "_3.jpg").c_str(), src, compression_params);

        //imageList.push_back( matToOsgImage( src, oriImg ) );

        int validCount = 0;
        std::map<int, int> tempToIndex;
        int sumCount = 0;
        for ( unsigned int i = 0; i < cc.size(); i ++ )
        {
            if ( cc[i] > 0 )
            {
                if ( cc[i] > 0 )
                {
                    sumCount += cc[i];
                    tempToIndex[i] = validCount;
                    validCount ++;
                }
                validCount ++;
            }
        }
        const int colorRange = 255;
        std::vector<int> cc2(colorRange+1);
        memset( &cc2[0], 0, (colorRange+1) * sizeof( int ) );
        int sumCount2 = 0;
        for ( int i = 0; i < y; i ++ )
        {
            for ( int j = 0; j < x; j ++ )
            {
                float v = tempMat.at<float>(y - 1 - i,j);

                if ( tempToIndex.find((int)v) != tempToIndex.end() )
                {
                    v = ((float)(tempToIndex[(int)v])) / (tempToIndex.size() * 1.2);// * 255;
                    if ( v > 1.0 )
                    {
                        v = 1.0;
                    }
                    if ( v < 0.0 )
                    {
                        v = 0.0;
                    }
                    v *= 255;
                    //v *= 0.85;

                    cc2[(int)(v)] += 1;

                    unsigned char r,g,b;
                    src.at<cv::Vec3b>(y - 1 - i,j)[0] = v;
                    src.at<cv::Vec3b>(y - 1 - i,j)[1] = v;
                    src.at<cv::Vec3b>(y - 1 - i,j)[2] = v;

                    sumCount2 ++;
                }
            }
        }

        for ( auto tt : cc2 )
        {
            printf("%d ", tt);
        }
        printf("\n============================%d,%d\n", sumCount, sumCount2);

        static cv::Mat lut(1, colorRange + 1, CV_8UC3);


        // for ( int i = 1; i < 9; i ++ )
        // {
        //     getRangeValue( lut, colors[i-1], colors[i], position[i-1], position[i] );
        // }

        static bool init = false;

        if ( !init )
        {
            osg::Vec4f c1(0, 0, 127, 255);
            osg::Vec4f c2(0, 0, 255, 255);
            osg::Vec4f c3(0, 127, 255, 255);
            osg::Vec4f c4(0, 255, 255, 255);
            osg::Vec4f c5(127, 255, 127, 255);
            osg::Vec4f c6(255, 255, 0, 255);
            osg::Vec4f c7(255, 0, 0, 255);
            osg::Vec4f c8(127, 0, 0, 255);
            osg::Vec4f c9(255, 255, 255, 255);

            {
                getRangeValue( lut, c1, c2, 0, 40 );
                getRangeValue( lut, c2, c3, 40, 90 );
                getRangeValue( lut, c3, c4, 90, 115 );
                getRangeValue( lut, c4, c5, 115, 135 );
                getRangeValue( lut, c5, c6, 135, 175 );
                getRangeValue( lut, c6, c7, 175, 225 );
                getRangeValue( lut, c7, c8, 225, 246 );
                getRangeValue( lut, c8, c9, 245, 256 );
            }

            init = true;
        }

        cv::Mat colorMat;
        cv::LUT( src, lut, colorMat );
        osg::Image *lpResultImage = matToOsgImage ( src, colorMat );
        //imageList.push_back( matToOsgImage ( src, colorMat ) );
        cv::imwrite(("/home/fosky/data/img/test" + std::to_string( index ) + "_3.jpg").c_str(), colorMat, compression_params);

        //cv::applyColorMap( src, colorMat, cv::COLORMAP_JET);
        //imageList.push_back( matToOsgImage ( src, colorMat ) );
        //cv::imwrite(("/home/fosky/data/img/test" + std::to_string( index ) + "_2.jpg").c_str(), colorMat, compression_params);


        index ++;
        return lpResultImage;
    }
    */
    osg::Image* colorMap( std::vector<float> const& temperatureList, int row, int col )
    {
        int y = row;
        int x = col;

        osg::Image *lpColorImage = new osg::Image;

        lpColorImage->allocateImage(x, y, 1, GL_RGBA, GL_UNSIGNED_BYTE);
        std::vector<osg::Vec4> colorList( temperatureList.size() );

        float maxv = FLT_MIN;
        float minv = FLT_MAX;

        for ( int i = 0; i < temperatureList.size(); i ++ )
        {
            float t = temperatureList[i];
            if ( maxv < t )
            {
                maxv = t;
            }

            if ( minv > t )
            {
                minv = t;
            }
        }

#pragma omp parallel for
        for ( int i = 0; i < row; i ++ )
        {
            for ( int j = 0; j < col; j ++ )
            {
                float t = temperatureList[ i * col + j ];

                float p = ( t - minv ) / ( maxv - minv ) * 255;
                osg::Vec4 c = getColor( p );
                lpColorImage->setColor(c, j, i);
            }
        }

        return lpColorImage;
    }
void colorMapCV( std::vector<float> const& temperatureList, int row, int col ,cv::Mat& mat)
    {
        int y = row;
        int x = col;

        mat.create( y,x,CV_8UC3);
        float maxv = FLT_MIN;
        float minv = FLT_MAX;

        for ( int i = 0; i < temperatureList.size(); i ++ )
        {
            float t = temperatureList[i];
            if ( maxv < t )
            {
                maxv = t;
            }

            if ( minv > t )
            {
                minv = t;
            }
        }
#pragma omp parallel for
        for ( int i = 0; i < row; i ++ )
        {
            for ( int j = 0; j < col; j ++ )
            {
                float t = temperatureList[ i * col + j ];

                float p = ( t - minv ) / ( maxv - minv ) * 255;
                osg::Vec4 c = getColor( p );
                mat.at<cv::Vec3b>(i,j)[0] = c.b() * 255;
                mat.at<cv::Vec3b>(i,j)[1] = c.g() * 255;
                mat.at<cv::Vec3b>(i,j)[2] = c.r() * 255;
            }
        }
        //cv::imwrite("/home/ywy/osg/OpenSceneGraph-Data/Images/part_websocket_infrared2.png", mat);

        //return lpColorImage;
    }
    osg::Image* colorMap( osg::Image *lpImage )
    {
        int y = lpImage->t();
        int x = lpImage->s();

        osg::Image *lpColorImage = new osg::Image;

        lpColorImage->allocateImage(x, y, 1, GL_RGBA, GL_UNSIGNED_BYTE);

        float maxv = FLT_MIN;
        float minv = FLT_MAX;

        for ( int i = 0; i < y; i ++ )
        {
            for ( int j = 0; j < x; j ++ )
            {
                osg::Vec4 c = lpImage->getColor( j, i );
                float t = ydUtil::colorToTemperature( c );
                if ( maxv < t )
                {
                    maxv = t;
                }

                if ( minv > t )
                {
                    minv = t;
                }
            }
        }
#pragma omp parallel for
        for ( int i = 0; i < y; i ++ )
        {
            for ( int j = 0; j < x; j ++ )
            {
                osg::Vec4 c = lpImage->getColor( j, i );
                float t = ydUtil::colorToTemperature( c );
                float p = ( t - minv ) / ( maxv - minv ) * 255;
                c = getColor( p );
                lpColorImage->setColor(c, j,i);
            }
        }

        return lpColorImage;

    }
	/*
    osg::Image* colorMap2( osg::Image *lpImage )
    {
        cv::Mat m;

        int y = lpImage->t();
        int x = lpImage->s();

        cv::Mat tempMat(y, x, CV_32FC3);
        cv::Mat src(y, x, CV_8UC3);
        tempMat.setTo(0);

        float maxv = FLT_MIN;
        float minv = FLT_MAX;
        for ( int i = 0; i < y; i ++ )
        {
            for ( int j = 0; j < x; j ++ )
            {
                osg::Vec4 c = lpImage->getColor( j, i );
                float t = colorToTemperature( c );
                if ( maxv < t )
                {
                    maxv = t;
                }

                if ( minv > t )
                {
                    minv = t;
                }

                float v = t / 30.0f;
                src.at<cv::Vec3b>(y - 1 - i,j)[2] = v * 255;
                src.at<cv::Vec3b>(y - 1 - i,j)[1] = v * 255;
                src.at<cv::Vec3b>(y - 1 - i,j)[0] = v * 255;

            }
        }

        float r = maxv - minv;
        if ( r == 0.0f )
        {
            r = 1.0f;
        }

    
        cv::Mat colorMat;
        cv::applyColorMap( src, colorMat, cv::COLORMAP_JET);
        #if 0
        {
            cv::Mat colorMat2;
            cv::LUT( src, lut, colorMat2 );
            {
                std::vector<int>compression_params;
                compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
                compression_params.push_back(100);
                static int index = 0;
                cv::imwrite(("/home/fosky/data/img/test" + std::to_string( index ) + "_3.jpg").c_str(), colorMat, compression_params);
                index ++;
            }
            {
                std::vector<int>compression_params;
                compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
                compression_params.push_back(100);
                static int index = 0;
                cv::imwrite(("/home/fosky/data/img/test" + std::to_string( index ) + "_4.jpg").c_str(), colorMat2, compression_params);
                index ++;
            }
        }
        #endif

        osg::Image *lpColorImage = new osg::Image;

        lpColorImage->allocateImage(x, y, 1, GL_RGBA, GL_UNSIGNED_BYTE);

#pragma omp parallel for
        for ( int i = 0; i < y; i ++ )
        {
            for ( int j = 0; j < x; j ++ )
            {
                osg::Vec4 c;;
                c.r() = colorMat.at<cv::Vec3b>(y - 1 - i,j)[2];
                c.g() = colorMat.at<cv::Vec3b>(y - 1 - i,j)[1];
                c.b() = colorMat.at<cv::Vec3b>(y - 1 - i,j)[0];
                c.a() = 255;
                lpColorImage->setColor( c / 255.0, j, i );
            }
        }
        return lpColorImage;
    }
	*/
}
