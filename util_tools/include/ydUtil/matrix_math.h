/*
 * file : matrix_math.h
 * auth : geng_bl
 * time : 2014-12-18 19:46:32
 * describe : angle pos matrix change tool
 * */
#ifndef __SE_UTIL_MATRIX_MATH_H_FILE__
#define __SE_UTIL_MATRIX_MATH_H_FILE__
#include <ydUtil/export.h>
#include <osg/Vec3>
#include <osg/Matrix>

namespace ydUtil
{
YDUTIL_API   osg::Matrix& ComputeModelToWorldMatrix(osg::Node* node, osg::Matrix& mat);
/*
	本地坐标系方向向量转换成方向角
	at			(in)	物体前向
	up			(in)	物体向上的方向
	yaw			(out)	偏航角,单位：度
	pitch		(out)	俯仰角,单位：度
	roll		(out)	滚转角,单位：度
*/
YDUTIL_API void ConvertPoseVecToAngle(const osg::Vec3& at, const osg::Vec3& up, float& yaw, float& pitch, float& roll);

/*
	本地坐标系方向角转换成方向向量
	yaw			(in)	偏航角,单位：度
	pitch		(in)	俯仰角,单位：度
	roll		(in)	滚转角,单位：度
	at			(out)	物体前向
	up			(out)	物体向上的方向
*/
YDUTIL_API void ConvertPoseAngleToVecf(float yaw, float pitch, float roll, osg::Vec3f& at, osg::Vec3f& up);
YDUTIL_API void ConvertPoseAngleToVecd(float yaw, float pitch, float roll, osg::Vec3d& at, osg::Vec3d& up);

YDUTIL_API void ComputePoseVecFromTransformf(const osg::Matrixf& mat, osg::Vec3f& at, osg::Vec3f& up);
YDUTIL_API void ComputePoseVecFromTransformd(const osg::Matrixd& mat, osg::Vec3d& at, osg::Vec3d& up);


/*
	由变换矩阵计算位置、姿态向量、缩放
	mat			(in)	变换矩阵
	pos			(out)	位置坐标
	at/up		(out)	姿态向量
	scale		(out)	三个轴向缩放系数
*/
YDUTIL_API void ComputePosPoseScaleFromTransformf(const osg::Matrixf& mat, osg::Vec3f& pos, osg::Vec3f& at, osg::Vec3f& up, osg::Vec3f& scale);
YDUTIL_API void ComputePosPoseScaleFromTransformd(const osg::Matrixd& mat, osg::Vec3d& pos, osg::Vec3d& at, osg::Vec3d& up, osg::Vec3d& scale);


/*
	由位置、姿态向量、缩放计算变换矩阵(组合顺序SRT)
	pos			(in)	位置坐标
	at/up		(in)	姿态向量,已单位化
	scale		(in)	三个轴向缩放系数
	mat			(out)	变换矩阵
	return:	返回计算得到的变换矩阵
*/
YDUTIL_API osg::Matrixf& ComputeMatrixf(const osg::Vec3f& pos, const osg::Vec3f& at, const osg::Vec3f& up, const osg::Vec3& scale, osg::Matrixf& mat);
YDUTIL_API osg::Matrixd& ComputeMatrixd(const osg::Vec3d& pos, const osg::Vec3d& at, const osg::Vec3d& up, const osg::Vec3d& scale, osg::Matrixd& mat);

YDUTIL_API osg::Matrixf &ComputeMatrixFromPosAndRotatef( osg::Vec3f const &pos, osg::Vec3f const &rotate, osg::Matrixf& mat );
YDUTIL_API osg::Matrixd &ComputeMatrixFromPosAndRotated( osg::Vec3d const &pos, osg::Vec3d const &rotate, osg::Matrixd& mat );
}


#endif // __SE_UTIL_MATRIX_MATH_H_FILE__
