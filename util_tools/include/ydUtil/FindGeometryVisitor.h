#ifndef __YD_UTIL_FIND_GEOMETRY_VISITOR_H_FILE__
#define __YD_UTIL_FIND_GEOMETRY_VISITOR_H_FILE__
#include <ydUtil/export.h>
#include <osg/NodeVisitor>
#include <vector>
#include <osg/ref_ptr>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Transform>


namespace ydUtil
{
	class YDUTIL_API FindGeometryVisitor : public osg::NodeVisitor
	{
	public:

		osg::observer_ptr<osg::Geometry> geometry;

		void apply(osg::Geode& node);
		FindGeometryVisitor();

	};

    class YDUTIL_API FindGeometryListVisitor : public osg::NodeVisitor
    {
    public:
        void apply(osg::Transform& transform);
        void apply(osg::Geode& node);

        FindGeometryListVisitor();
    
        inline void pushMatrix(osg::Matrix& matrix) { _matrixStack.push_back(matrix); }

        inline void popMatrix() { _matrixStack.pop_back(); }
    public:
        typedef std::vector<osg::Matrix> MatrixStack;
        typedef std::vector<std::pair< osg::Matrix, osg::ref_ptr<osg::Geometry> > > GeometryList;
        GeometryList _geometryList;
        MatrixStack  _matrixStack;
	};

}



#endif // __YD_COMMON_FIND_GEOMETRY_VISITOR_H_FILE__
