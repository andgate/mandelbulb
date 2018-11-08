#ifndef VECTOR_3F_H
#define VECTOR_3F_H

#include <SFML/System/Vector3.hpp>

class Vector3f
{
public:
	Vector3f();
	Vector3f(float x, float y, float z);
	Vector3f(const Vector3f &other);
	Vector3f(Vector3f &&other);
	
	Vector3f& operator=(const Vector3f &b);
	Vector3f& operator=(Vector3f &&b);

	sf::Vector3f asSFML() const;

	float* getValues();

	float getX() const;
	float getY() const;
	float getZ() const;

	float& x();
	float& y();
	float& z();

	float length() const;
	float dot(const Vector3f &b) const;

	Vector3f& add(const Vector3f &b);
	Vector3f& sub(const Vector3f &b);
	
	Vector3f& normalize();
	static Vector3f normalize(const Vector3f &v);

	Vector3f& cross(const Vector3f &b);
	static Vector3f cross(const Vector3f &a, const Vector3f &b);

	Vector3f operator+(const Vector3f &u) const;
	Vector3f operator-(const Vector3f &u) const;
	Vector3f operator*(const float &s) const;
	Vector3f operator/(const float &s) const;

	Vector3f& operator+=(const Vector3f &u);
	Vector3f& operator-=(const Vector3f &u);
	Vector3f& operator*=(const float &s);
	Vector3f& operator/=(const float &s);


private:
	float val[3];

	static const int X = 0;
	static const int Y = 1;
	static const int Z = 2;
};

#endif // VECTOR_3F_H