#ifndef VECTOR_3F_H
#define VECTOR_3F_H

#include <SFML/System/Vector3.hpp>

class Matrix4;

class Vector3f
{
public:
	float x;
	float y;
	float z;

	Vector3f();
	Vector3f(float x, float y, float z);
	Vector3f(const Vector3f &other);
	Vector3f(Vector3f &&other);
	
	Vector3f& operator=(const Vector3f &b);
	Vector3f& operator=(Vector3f &&b);

	sf::Vector3f asSFML() const;

	Vector3f& set(const float &x, const float &y, const float &z);
	Vector3f& set(const Vector3f &u);

	float length() const;
	float dot(const Vector3f &b) const;

	Vector3f& add(const Vector3f &b);
	Vector3f& sub(const Vector3f &b);

	Vector3f& mul(const Matrix4& m);
	
	Vector3f& normalize();
	static Vector3f normalize(const Vector3f &v);

	Vector3f& cross(const Vector3f &b);
	static Vector3f cross(const Vector3f &a, const Vector3f &b);

	Vector3f& rot(const Matrix4& matrix);
	Vector3f& rotate(const float &degrees, const float &axisX, const float &axisY, const float &axisZ);
	Vector3f& rotate(const Vector3f& axis, const float &degrees);

	Vector3f operator+(const Vector3f &u) const;
	Vector3f operator-(const Vector3f &u) const;
	Vector3f operator*(const float &s) const;
	Vector3f operator/(const float &s) const;

	Vector3f& operator+=(const Vector3f &u);
	Vector3f& operator-=(const Vector3f &u);
	Vector3f& operator*=(const float &s);
	Vector3f& operator/=(const float &s);
};

#endif // VECTOR_3F_H