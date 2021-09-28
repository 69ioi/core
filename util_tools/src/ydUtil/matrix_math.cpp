#include <ydUtil/matrix_math.h>
#include <osg/NodeVisitor>
#include <osg/Transform>


namespace ydUtil
{

    // 收集父节点的访问器,一个节点可能有多个父节点路径
    class  CollectParentPaths : public osg::NodeVisitor
    {
    public:
        CollectParentPaths(const osg::Node* haltTraversalAtNode = 0)
            : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_PARENTS),
              _haltTraversalAtNode(haltTraversalAtNode)
            {
            }

        virtual void apply(osg::Node& node);
        void reset(const osg::Node* haltTraversalAtNode = 0);

        const osg::Node*	_haltTraversalAtNode;
        osg::NodePathList	_nodePaths;
    };
    void CollectParentPaths::apply(osg::Node& node)
    {
        if (node.getNumParents() == 0 || &node == _haltTraversalAtNode)
        {
            _nodePaths.push_back(getNodePath());
        }
        else
        {
            traverse(node);
        }
    }

    void CollectParentPaths::reset(const osg::Node* haltTraversalAtNode)
    {
        _haltTraversalAtNode = haltTraversalAtNode;
        _nodePath.clear();
        _nodePaths.clear();
    }


    osg::Matrix& ComputeModelToWorldMatrix(osg::Node* node, osg::Matrix& mat)
    {
        mat.makeIdentity();

        if (!node)
            return mat;

        CollectParentPaths cpp;
        node->accept(cpp);
        if (!cpp._nodePaths.empty())
            mat = osg::computeLocalToWorld(cpp._nodePaths.front());

        return mat;
    }



    void ConvertPoseVecToAngle(const osg::Vec3& at, const osg::Vec3& up, float& yaw, float& pitch, float& roll)
    {
        osg::Vec3 _at(at), _up(up);
        _at.normalize();
        _up.normalize();

        if ( _at.z() > 1 )
            _at.z() = 1;
        if ( _at.z() < -1 )
            _at.z() = -1;

        pitch = asinf( _at.z() );

        _at.z() = 0;
        if ( _at.length2() <= 0.01 )
        {
            roll = 0;

            _up.z() = 0;
            _up.normalize();

            if ( _up.y() < -1 )
                _up.y() = -1;
            if ( _up.y() > 1 )
                _up.y() = 1;

            yaw = acosf( _up.y() );
            if ( _up.x() < 0 )
                yaw = -yaw;

            pitch = osg::RadiansToDegrees(pitch);
            yaw = osg::RadiansToDegrees(yaw);

            if ( at.z() > 0 )
                yaw = yaw-180.0f;

            return;
        }
        else
        {
            _at.normalize();

            if ( _at.y() < -1 )
                _at.y() = -1;
            if ( _at.y() > 1 )
                _at.y() = 1;

            yaw = acosf( _at.y() );
            if ( _at.x() <= 0 )
                yaw = -yaw;
        }

        osg::Vec3d right1 = at ^ osg::Vec3d(0, 0, 1);
        osg::Vec3d right2 = at ^ up;
        right1.normalize();
        right2.normalize();

        float aaa = right1 * right2;

        if ( aaa > 1 )
            aaa = 1;
        if ( aaa < -1 )
            aaa = -1;

        roll = -acosf( aaa );

        osg::Vec3d aa = right2 ^ right1;
        aa.normalize();

        if ( aa * at < 0 )
        {
            roll = -roll;
        }

        pitch = osg::RadiansToDegrees(pitch);
        yaw = osg::RadiansToDegrees(yaw);
        roll = osg::RadiansToDegrees(roll);
    }

    void ConvertPoseAngleToVecf(float yaw, float pitch, float roll, osg::Vec3f& at, osg::Vec3f& up)
    {
        at.set(0, 1, 0);
        up.set(0, 0, 1);
        if (abs(yaw) < 0.00001)
        {
            yaw = 0.0;
        }
        if (abs(pitch) < 0.00001)
        {
            pitch = 0.0;
        }
        if (abs(roll) < 0.00001)
        {
            roll = 0.0;
        }
        osg::Matrix mt = osg::Matrix::rotate(osg::DegreesToRadians(roll), osg::Vec3(0,1,0),
                                             osg::DegreesToRadians(pitch), osg::Vec3(1,0,0),
                                             osg::DegreesToRadians(-yaw), osg::Vec3(0,0,1));
        at = mt.preMult(at);
        up = mt.preMult(up);
    }

    void ConvertPoseAngleToVecd(float yaw, float pitch, float roll, osg::Vec3d& at, osg::Vec3d& up)
    {
        at.set(0, 1, 0);
        up.set(0, 0, 1);

        osg::Matrix mt = osg::Matrix::rotate(osg::DegreesToRadians(roll), osg::Vec3(0,1,0),
                                             osg::DegreesToRadians(pitch), osg::Vec3(1,0,0),
                                             osg::DegreesToRadians(-yaw), osg::Vec3(0,0,1));
        at = mt.preMult(at);
        up = mt.preMult(up);
    }

    void ComputePoseVecFromTransformf(const osg::Matrixf& mat, osg::Vec3f& at, osg::Vec3f& up)
    {
        at = osg::Matrix::transform3x3(osg::Vec3f(0,1,0), mat);
        up = osg::Matrix::transform3x3(osg::Vec3f(0,0,1), mat);
        at.normalize();
        up.normalize();
    }

    void ComputePoseVecFromTransformd(const osg::Matrixd& mat, osg::Vec3d& at, osg::Vec3d& up)
    {
        at = osg::Matrix::transform3x3(osg::Vec3d(0,1,0), mat);
        up = osg::Matrix::transform3x3(osg::Vec3d(0,0,1), mat);
        at.normalize();
        up.normalize();
    }

    void ComputePosPoseScaleFromTransformf(const osg::Matrixf& mat, osg::Vec3f& pos, osg::Vec3f& at, osg::Vec3f& up, osg::Vec3f& scale)
    {
        pos = mat.getTrans();
            
        //scale = mat.getScale(); 这种方法获得的结果不对
        // 暂时使用这种笨方法
        osg::Vec3f temp = osg::Matrix::transform3x3(osg::Vec3f(1, 0, 0), mat);
        scale[0] = temp.length();
        temp = osg::Matrix::transform3x3(osg::Vec3f(0, 1, 0), mat);
        scale[1] = temp.length();
        temp = osg::Matrix::transform3x3(osg::Vec3f(0, 0, 1), mat);
        scale[2] = temp.length();

        at = osg::Matrix::transform3x3(osg::Vec3f(0,1,0), mat);
        up = osg::Matrix::transform3x3(osg::Vec3f(0,0,1), mat);
        at.normalize();
        up.normalize();
    }

    void ComputePosPoseScaleFromTransformd(const osg::Matrixd& mat, osg::Vec3d& pos, osg::Vec3d& at, osg::Vec3d& up, osg::Vec3d& scale)
    {
        pos = mat.getTrans();

        //scale = mat.getScale(); 这种方法获得的结果不对
        // 暂时使用这种笨方法
        osg::Vec3d temp = osg::Matrix::transform3x3(osg::Vec3d(1, 0, 0), mat);
        scale[0] = temp.length();
        temp = osg::Matrix::transform3x3(osg::Vec3d(0, 1, 0), mat);
        scale[1] = temp.length();
        temp = osg::Matrix::transform3x3(osg::Vec3d(0, 0, 1), mat);
        scale[2] = temp.length();

        at = osg::Matrix::transform3x3(osg::Vec3d(0,1,0), mat);
        up = osg::Matrix::transform3x3(osg::Vec3d(0,0,1), mat);
        at.normalize();
        up.normalize();
    }

    osg::Matrixf& ComputeMatrixf(const osg::Vec3f& pos, const osg::Vec3f& at, const osg::Vec3f& up, const osg::Vec3f& scale, osg::Matrixf& mat)
    {
        osg::Vec3f right = at ^ up;

        mat.set(right.x(),	right.y(), right.z(),	0.0,
                at.x(),		at.y(),		at.z(),		0.0,
                up.x(),		up.y(),		up.z(),		0.0,
                pos.x(),	pos.y(),	pos.z(),	1.0);
        mat = osg::Matrixf::scale(scale) * mat;

        return mat;
    }

    osg::Matrixd& ComputeMatrixd(const osg::Vec3d& pos, const osg::Vec3d& at, const osg::Vec3d& up, const osg::Vec3d& scale, osg::Matrixd& mat)
    {
        osg::Vec3d right = at ^ up;

        mat.set(right.x(),	right.y(), right.z(),	0.0,
                at.x(),		at.y(),		at.z(),		0.0,
                up.x(),		up.y(),		up.z(),		0.0,
                pos.x(),	pos.y(),	pos.z(),	1.0);
        mat = osg::Matrixd::scale(scale) * mat;

        return mat;
    }

    osg::Matrixf &ComputeMatrixFromPosAndRotatef( osg::Vec3f const &pos, osg::Vec3f const &rotate, osg::Matrixf& mat )
    {
        osg::Vec3f at, up, scale;
        osg::Vec3f q = rotate;
        scale.set( 1.0, 1.0, 1.0 );
        osg::Matrixf m;
        ConvertPoseAngleToVecf(q.x(), q.y(), q.z(), at, up);
        ComputeMatrixf( pos, at, up, scale, mat);
        return mat;
    }
    osg::Matrixd &ComputeMatrixFromPosAndRotated( osg::Vec3d const &pos, osg::Vec3d const &rotate, osg::Matrixd& mat )
    {
        osg::Vec3d at, up, scale;
        osg::Vec3d q = rotate;
        scale.set( 1.0, 1.0, 1.0 );
        ConvertPoseAngleToVecd(q.x(), q.y(), q.z(), at, up);
        ComputeMatrixd( pos, at, up, scale, mat);
        return mat;
    }


}
