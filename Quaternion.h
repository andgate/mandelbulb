#ifndef QUATERNION_H
#define QUATERNION_H

class Vector3f;

class Quaternion
{
public:
	float x, y, z, w;

	Quaternion();
	Quaternion(float x, float y, float z, float w);

	Quaternion(const Quaternion& other);
	Quaternion(Quaternion&& other);

	Quaternion& set(const float &x, const float &y, const float &z, const float &w);
	Quaternion& set(const Vector3f &axis, const float &degrees);

	Quaternion& idt();
	float len2();
	Quaternion& nor();

	Quaternion& setFromAxis(const float axisX, const float axisY, const float axisZ, const float degrees);
	Quaternion& setFromAxisRad(const float axisX, const float axisY, const float axisZ, const float radians);
};

#endif /* QUATERNION_H */