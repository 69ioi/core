#include "ydUtil/FindGeometryVisitor.h"

namespace ydUtil
{
    void FindGeometryVisitor::apply(osg::Geode& node)
    {
        for (unsigned int i = 0; i < node.getNumDrawables(); ++i)
        {
            geometry = node.getDrawable(i)->asGeometry();
        }
        traverse(node);
    }

    FindGeometryVisitor::FindGeometryVisitor()
        : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    {
    }

    //   virtual void apply(osg::Group &node);
    void FindGeometryListVisitor::apply(osg::Geode& node)
    {
        osg::Matrix matrix;// = _matrixStack.back();
        if ( _matrixStack.size() > 0 )
        {
            matrix = _matrixStack.back();
        }
        for (unsigned int i = 0; i < node.getNumDrawables(); ++i)
        {
            osg::ref_ptr<osg::Geometry> geometry = node.getDrawable(i)->asGeometry();
            if (geometry)
            {
                _geometryList.push_back(std::make_pair(matrix, geometry));
            }

            return;
        }
        traverse(node);

    }
    void FindGeometryListVisitor::apply(osg::Transform& transform)
    {
        osg::Matrix matrix;
        if (!_matrixStack.empty()) matrix = _matrixStack.back();

        transform.computeLocalToWorldMatrix(matrix,this);

        pushMatrix(matrix);

        traverse(transform);

        popMatrix();
    }

    FindGeometryListVisitor::FindGeometryListVisitor()
        : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
    {
    }
}

