#include "Quaternion.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include "Vector3f.h"

const float M_PI2 = 2.0f * (float)M_PI;

Quaternion::Quaternion()
	: x(0), y(0), z(0), w(1)
{}

Quaternion::Quaternion(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w)
{}

Quaternion::Quaternion(const Quaternion& other)
	: x(other.x)
	, y(other.y)
	, z(other.z)
	, w(other.w)
{
}

Quaternion::Quaternion(Quaternion&& other)
	: x(other.x)
	, y(other.y)
	, z(other.z)
	, w(other.w)
{}


Quaternion& Quaternion::set(const float &x, const float &y, const float &z, const float &w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
	return *this;
}

Quaternion& Quaternion::set(const Vector3f &axis, const float &degrees)
{
	return setFromAxis(axis.x, axis.y, axis.z, degrees);
}

Quaternion& Quaternion::idt()
{
	return set(0.0f, 0.0f, 0.0f, 1.0f);
}

float Quaternion::len2()
{
	return x * x + y * y + z * z + w * w;
}

Quaternion& Quaternion::nor()
{
	float len = len2();
	if (len != 0.0f) {
		len = sqrt(len);
		w /= len;
		x /= len;
		y /= len;
		z /= len;
	}

	return *this;
}

Quaternion& Quaternion::setFromAxis(const float axisX, const float axisY, const float axisZ, const float degrees)
{
	return setFromAxisRad(axisX, axisY, axisZ, degrees * (float)M_PI / 180.0f);
}

Quaternion& Quaternion::setFromAxisRad(const float x, const float y, const float z, const float radians)
{
	Vector3f axis = Vector3f(x,y,z);
	float d = axis.length();
	if (d == 0.0f) return idt();
	
	d = 1.0f / d;
	float l_ang = radians < 0 ? M_PI2 - fmod(abs(radians), M_PI2) : fmod(radians, M_PI2);
	float l_sin = (float)sin(l_ang / 2.0f);
	float l_cos = (float)cos(l_ang / 2.0f);
	
	return set(d * x * l_sin, d * y * l_sin, d * z * l_sin, l_cos).nor();
}
