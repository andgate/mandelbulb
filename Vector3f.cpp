#include "Vector3f.h"

#include <cstring>
#include <cmath>

#include "Matrix4.h"

Vector3f::Vector3f()
	: x(0.0f), y(0.0f), z(0.0f)
{}


Vector3f::Vector3f(float x, float y, float z)
	: x(x), y(y), z(z)
{}

Vector3f::Vector3f(const Vector3f &other)
	: x(other.x), y(other.y), z(other.z)
{}

Vector3f::Vector3f(Vector3f &&other)
	: x(other.x), y(other.y), z(other.z)
{}


Vector3f& Vector3f::operator=(const Vector3f &b)
{
	this->x = b.x;
	this->y = b.y;
	this->z = b.z;
	return *this;
}

Vector3f& Vector3f::operator=(Vector3f &&b)
{
	this->x = b.x;
	this->y = b.y;
	this->z = b.z;
	return *this;
}



sf::Vector3f Vector3f::asSFML() const
{
	return sf::Vector3f(x, y, z);
}


Vector3f& Vector3f::set(const float &x, const float &y, const float &z)
{
	this->x = x;
	this->y = y;
	this->z = z;
	return *this;
}

Vector3f& Vector3f::set(const Vector3f &u)
{
	return this->set(u.x, u.y, u.z);
}


float Vector3f::length() const
{
	return sqrt((x * x) + (y * y) + (z * z));
}


Vector3f& Vector3f::sub(const Vector3f &b)
{
	return *this -= b;
}

Vector3f& Vector3f::add(const Vector3f &b)
{
	return *this += b;
}


Vector3f& Vector3f::mul(const Matrix4& m)
{
	const float* l_mat = m.asArray();
	return this->set( x * l_mat[Matrix4::M00] + y * l_mat[Matrix4::M01] + z * l_mat[Matrix4::M02] + l_mat[Matrix4::M03]
	                , x * l_mat[Matrix4::M10] + y * l_mat[Matrix4::M11] + z * l_mat[Matrix4::M12] + l_mat[Matrix4::M13]
				    , x * l_mat[Matrix4::M20] + y * l_mat[Matrix4::M21] + z * l_mat[Matrix4::M22] + l_mat[Matrix4::M23]);
}


Vector3f& Vector3f::normalize()
{
	return *this /= length();
}

Vector3f Vector3f::normalize(const Vector3f &v)
{
	return Vector3f(v) /= v.length();
}

float Vector3f::dot(const Vector3f &u) const
{
	return x * u.x + y * u.y + z * u.z;
}

Vector3f& Vector3f::cross(const Vector3f &u)
{
	Vector3f tmp(*this);
	tmp.x = y * u.z - z * u.y;
	tmp.y = z * u.x - x * u.z;
	tmp.z = x * u.y - y * u.x;

	return this->set(tmp);
}

Vector3f Vector3f::cross(const Vector3f &a, const Vector3f &b)
{
	return Vector3f( a.y * b.z - a.z * b.y
		           , a.z * b.x - a.x * b.z
				   , a.x * b.y - a.y * b.x
		           );
}

Vector3f& Vector3f::rot(const Matrix4& matrix)
{
	const float* l_mat = matrix.asArray();
	this->set( x * l_mat[Matrix4::M00] + y * l_mat[Matrix4::M01] + z * l_mat[Matrix4::M02]
	         , x * l_mat[Matrix4::M10] + y * l_mat[Matrix4::M11] + z * l_mat[Matrix4::M12]
			 , x * l_mat[Matrix4::M20] + y * l_mat[Matrix4::M21] + z * l_mat[Matrix4::M22]);

	return *this;
}


Vector3f& Vector3f::rotate(const float &degrees, const float &axisX, const float &axisY, const float &axisZ)
{
	Matrix4 tmpMat;
	tmpMat.setToRotation(axisX, axisY, axisZ, degrees);
	return this->mul(tmpMat);
}

Vector3f& Vector3f::rotate(const Vector3f& axis, const float &degrees)
{
	Matrix4 tmpMat;
	tmpMat.setToRotation(axis, degrees);
	return this->mul(tmpMat);
}

Vector3f Vector3f::operator+(const Vector3f &u) const
{
	return Vector3f( x + u.x
				   , y + u.y
				   , z + u.z
	               );
}


Vector3f Vector3f::operator-(const Vector3f &u) const
{
	return Vector3f( x - u.x
				   , y - u.y
				   , z - u.z
	               );
}

Vector3f Vector3f::operator*(const float &s) const
{
	return Vector3f( x * s
				   , y * s
				   , z * s
	               );
}

Vector3f Vector3f::operator/(const float &s) const
{
	if (s != 0.0f)
	{
		return Vector3f( x / s
					   , y / s
				       , z / s
					   );
	}

	return *this;
}

Vector3f& Vector3f::operator+=(const Vector3f &u)
{
	x += u.x;
	y += u.y;
	z += u.z;
	return *this;
}


Vector3f& Vector3f::operator-=(const Vector3f &u)
{
	x -= u.x;
	y -= u.y;
	z -= u.z;
	return *this;
}

Vector3f& Vector3f::operator*=(const float &s)
{
	x *= s;
	y *= s;
	z *= s;
	return *this;
}

Vector3f& Vector3f::operator/=(const float &s)
{
	if (s != 0.0f)
	{
		x /= s;
		y /= s;
		z /= s;
	}

	return *this;
}