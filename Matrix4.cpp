#include "Matrix4.h"

// Enable constants like PI
#define _USE_MATH_DEFINES
#include <cmath>

#include <cstring>

Matrix4::Matrix4()
{
	val[M00] = 1.f; val[M01] = 0.f; val[M02]  = 0.f; val[M03] = 0.f;
    val[M10] = 0.f; val[M11] = 1.f; val[M12]  = 0.f; val[M13] = 0.f;
    val[M20] = 0.f; val[M21] = 0.f; val[M22]  = 1.f; val[M23] = 0.f;
    val[M30] = 0.f; val[M31] = 0.f; val[M32]  = 0.f; val[M33] = 1.f;
}


const float* Matrix4::getMatrix() const
{
	return val;
}


Matrix4& Matrix4::identity()
{
	val[M00] = 1.0f;
	val[M01] = 0.0f;
	val[M02] = 0.0f;
	val[M03] = 0.0f;
	val[M10] = 0.0f;
	val[M11] = 1.0f;
	val[M12] = 0.0f;
	val[M13] = 0.0f;
	val[M20] = 0.0f;
	val[M21] = 0.0f;
	val[M22] = 1.0f;
	val[M23] = 0.0f;
	val[M30] = 0.0f;
	val[M31] = 0.0f;
	val[M32] = 0.0f;
	val[M33] = 1.0f;
	return *this;
}

Matrix4& Matrix4::setToProjection(float near, float far, float fovy, float aspectRatio)
{
	identity();
	float l_fd = (float)(1.0f / tan((fovy * (M_PI / 180.0f)) / 2.0f));
	float l_a1 = (far + near) / (near - far);
	float l_a2 = (2.0f * far * near) / (near - far);
	
	val[M00] = l_fd / aspectRatio;
	val[M10] = 0.0f;
	val[M20] = 0.0f;
	val[M30] = 0.0f;
	val[M01] = 0.0f;
	val[M11] = l_fd;
	val[M21] = 0.0f;
	val[M31] = 0.0f;
	val[M02] = 0.0f;
	val[M12] = 0.0f;
	val[M22] = l_a1;
	val[M32] = -1.0f;
	val[M03] = 0.0f;
	val[M13] = 0.0f;
	val[M23] = l_a2;
	val[M33] = 0.0f;

	return *this;
}

Matrix4& Matrix4::setToTranslation (const Vector3f &v)
{
	identity();
	val[M03] = v.getX();
	val[M13] = v.getY();
	val[M23] = v.getZ();
	return *this;
}

Matrix4& Matrix4::setToTranslation (float x, float y, float z)
{
	identity();
	val[M03] = x;
	val[M13] = y;
	val[M23] = z;
	return *this;
}

Matrix4& Matrix4::setToLookAt(Vector3f direction, Vector3f up)
{
	Vector3f l_vez(direction);
	l_vez.normalize();
	Vector3f l_vex(direction);
	l_vex.normalize().cross(up).normalize();
	Vector3f l_vey(l_vex);
	l_vey.cross(l_vez).normalize();
	
	identity();
	val[M00] = l_vex.x();
	val[M01] = l_vex.y();
	val[M02] = l_vex.z();
	val[M10] = l_vey.x();
	val[M11] = l_vey.y();
	val[M12] = l_vey.z();
	val[M20] = -l_vez.x();
	val[M21] = -l_vez.y();
	val[M22] = -l_vez.z();

	return *this;
}

Matrix4& Matrix4::setToLookAt (Vector3f position, Vector3f target, Vector3f up)
{
	Vector3f tmpVec(target);
	tmpVec.sub(position);
	setToLookAt(tmpVec, up);

	Matrix4 tmpMat;
	tmpMat.setToTranslation(-position.x(), -position.y(), -position.z());
	this->mul(tmpMat);

	return *this;
}




Matrix4& Matrix4::add(const Matrix4 &other)
{
	*this += other;
	return *this;
}

Matrix4& Matrix4::mul(const Matrix4 &other)
{
	*this *= other;
	return *this;
}

Matrix4& Matrix4::operator+=(const Matrix4 &other)
{
	const float *otherVal = other.val;

	val[M00] += otherVal[M00];
	val[M01] += otherVal[M01];
	val[M02] += otherVal[M02];
	val[M03] += otherVal[M03];
	val[M10] += otherVal[M10];
	val[M11] += otherVal[M11];
	val[M12] += otherVal[M12];
	val[M13] += otherVal[M13];
	val[M20] += otherVal[M20];
	val[M21] += otherVal[M21];
	val[M22] += otherVal[M22];
	val[M23] += otherVal[M23];
	val[M30] += otherVal[M30];
	val[M31] += otherVal[M31];
	val[M32] += otherVal[M32];
	val[M33] += otherVal[M33];
	
	return *this;
}


Matrix4& Matrix4::operator*=(const Matrix4 &other)
{
	float tmpVal[16];

	const float *otherVal = other.val;

	memcpy(tmpVal, val, 16*sizeof(float));

	val[M00] = tmpVal[M00]*otherVal[M00] + tmpVal[M01]*otherVal[M10] + tmpVal[M02]*otherVal[M20]  + tmpVal[M03]*otherVal[M30];
	val[M01] = tmpVal[M00]*otherVal[M01] + tmpVal[M01]*otherVal[M11] + tmpVal[M02]*otherVal[M21]  + tmpVal[M03]*otherVal[M31];
	val[M02] = tmpVal[M00]*otherVal[M02] + tmpVal[M01]*otherVal[M12] + tmpVal[M02]*otherVal[M22]  + tmpVal[M03]*otherVal[M32];
	val[M03] = tmpVal[M00]*otherVal[M03] + tmpVal[M01]*otherVal[M13] + tmpVal[M02]*otherVal[M23]  + tmpVal[M03]*otherVal[M33];
	val[M10] = tmpVal[M10]*otherVal[M00] + tmpVal[M11]*otherVal[M10] + tmpVal[M12]*otherVal[M20]  + tmpVal[M13]*otherVal[M30];
	val[M11] = tmpVal[M10]*otherVal[M01] + tmpVal[M11]*otherVal[M11] + tmpVal[M12]*otherVal[M21]  + tmpVal[M13]*otherVal[M31];
	val[M12] = tmpVal[M10]*otherVal[M02] + tmpVal[M11]*otherVal[M12] + tmpVal[M12]*otherVal[M22]  + tmpVal[M13]*otherVal[M32];
	val[M13] = tmpVal[M10]*otherVal[M03] + tmpVal[M11]*otherVal[M13] + tmpVal[M12]*otherVal[M23]  + tmpVal[M13]*otherVal[M33];
	val[M20] = tmpVal[M20]*otherVal[M00] + tmpVal[M21]*otherVal[M10] + tmpVal[M22]*otherVal[M20]  + tmpVal[M23]*otherVal[M30];
	val[M21] = tmpVal[M20]*otherVal[M01] + tmpVal[M21]*otherVal[M11] + tmpVal[M22]*otherVal[M21]  + tmpVal[M23]*otherVal[M31];
	val[M22] = tmpVal[M20]*otherVal[M02] + tmpVal[M21]*otherVal[M12] + tmpVal[M22]*otherVal[M22]  + tmpVal[M23]*otherVal[M32];
	val[M23] = tmpVal[M20]*otherVal[M03] + tmpVal[M21]*otherVal[M13] + tmpVal[M22]*otherVal[M23]  + tmpVal[M23]*otherVal[M33];
	val[M30] = tmpVal[M30]*otherVal[M00] + tmpVal[M31]*otherVal[M10] + tmpVal[M32]*otherVal[M20]  + tmpVal[M33]*otherVal[M30];
	val[M31] = tmpVal[M30]*otherVal[M01] + tmpVal[M31]*otherVal[M11] + tmpVal[M32]*otherVal[M21]  + tmpVal[M33]*otherVal[M31];
	val[M32] = tmpVal[M30]*otherVal[M02] + tmpVal[M31]*otherVal[M12] + tmpVal[M32]*otherVal[M22]  + tmpVal[M33]*otherVal[M32];
	val[M33] = tmpVal[M30]*otherVal[M03] + tmpVal[M31]*otherVal[M13] + tmpVal[M32]*otherVal[M23]  + tmpVal[M33]*otherVal[M33];
	
	return *this;
}
