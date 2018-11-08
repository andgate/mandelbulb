#include "Vector3f.h"

#include <cstring>
#include <cmath>


Vector3f::Vector3f()
{
	val[X] = 0.0f;
	val[Y] = 0.0f;
	val[Z] = 0.0f;
}


Vector3f::Vector3f(float x, float y, float z)
{
	val[X] = x;
	val[Y] = y;
	val[Z] = z;
}

Vector3f::Vector3f(const Vector3f &other)
{
	memcpy(val, other.val, 3*sizeof(float));
}

Vector3f::Vector3f(Vector3f &&other)
{
	memcpy(val, other.val, 3);
	// zeroing isn't necessary for the move
	//memset(other.val, 0, 3);
}


Vector3f& Vector3f::operator=(const Vector3f &b)
{
	memcpy(val, b.val, 3);
	return *this;
}

Vector3f& Vector3f::operator=(Vector3f &&b)
{
	memcpy(val, b.val, 3);
	return *this;
}



sf::Vector3f Vector3f::asSFML() const
{
	return sf::Vector3f(val[X], val[Y], val[Z]);
}


float* Vector3f::getValues()
{
	return val;
}

float Vector3f::getX() const
{
	return val[X];
}

float Vector3f::getY() const
{
	return val[Y];
}

float Vector3f::getZ() const
{
	return val[Z];
}

float& Vector3f::x() 
{
	return val[X];
}

float& Vector3f::y()
{
	return val[Y];
}

float& Vector3f::z()
{
	return val[Z];
}

float Vector3f::length() const
{
	return sqrt((val[X] * val[X]) + (val[Y] * val[Y]) + (val[Z] * val[Z]));
}


Vector3f& Vector3f::sub(const Vector3f &b)
{
	return *this -= b;
}

Vector3f& Vector3f::add(const Vector3f &b)
{
	return *this += b;
}



Vector3f& Vector3f::normalize()
{
	return *this /= length();
}

Vector3f Vector3f::normalize(const Vector3f &v)
{
	return Vector3f(v) /= v.length();
}

float Vector3f::dot(const Vector3f &b) const
{
	const float *v = val;
	const float *u = b.val;
	return v[X] * u[X] + v[Y] * u[Y] + v[Z] * u[Z];
}

Vector3f& Vector3f::cross(const Vector3f &b)
{
	Vector3f tmp = Vector3f(*this);
	const float *v = tmp.val;
	const float *u = b.val;

	val[X] = v[Y] * u[Z] - v[Z] * u[Y];
	val[Y] = v[Z] * u[X] - v[X] * u[Z];
	val[Z] = v[X] * u[Y] - v[Y] * u[X];

	return *this;
}

Vector3f Vector3f::cross(const Vector3f &a, const Vector3f &b)
{
	const float *v = a.val;
	const float *u = b.val;
	return Vector3f( v[Y] * u[Z] - v[Z] * u[Y]
		           , v[X] * u[Z] - v[Z] * u[X]
				   , v[X] * u[Y] - v[Y] * u[X]
		           );
}


Vector3f Vector3f::operator+(const Vector3f &u) const
{
	const float *uVal = u.val;
	return Vector3f( val[X] + uVal[X]
				   , val[Y] + uVal[Y]
				   , val[Z] + uVal[Z]
	               );
}


Vector3f Vector3f::operator-(const Vector3f &u) const
{
	const float *uVal = u.val;
	return Vector3f( val[X] - uVal[X]
				   , val[Y] - uVal[Y]
				   , val[Z] - uVal[Z]
	               );
}

Vector3f Vector3f::operator*(const float &s) const
{
	return Vector3f( val[X] * s
				   , val[Y] * s
				   , val[Z] * s
	               );
}

Vector3f Vector3f::operator/(const float &s) const
{
	if (s != 0.0f)
	{
		return Vector3f( val[X] / s
					   , val[Y] / s
					   , val[Z] / s
					   );
	}

}

Vector3f& Vector3f::operator+=(const Vector3f &u)
{
	const float *uVal = u.val;
	val[X] += uVal[X];
	val[Y] += uVal[Y];
	val[Z] += uVal[Z];

	return *this;
}


Vector3f& Vector3f::operator-=(const Vector3f &u)
{
	const float *uVal = u.val;
	val[X] -= uVal[X];
	val[Y] -= uVal[Y];
	val[Z] -= uVal[Z];

	return *this;
}

Vector3f& Vector3f::operator*=(const float &s)
{
	val[X] *= s;
	val[Y] *= s;
	val[Z] *= s;

	return *this;
}

Vector3f& Vector3f::operator/=(const float &s)
{
	if (s != 0.0f)
	{
		val[X] /= s;
		val[Y] /= s;
		val[Z] /= s;
	}

	return *this;
}