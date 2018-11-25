#ifndef CAMERA_H
#define CAMERA_H

#include "Matrix4.h"
#include "Vector3f.h"

class Camera
{
public:
	Camera(float viewportWidth, float viewportHeight);

	Camera& setSphereRadius(const float &sphereRadius);
	Camera& scaleSphereRadius(const float &scaleFactor);


	void rotate(const float &angle, const float &axisX, const float &axisY, const float &axisZ);
	void rotate(const Vector3f &axis, const float &angle);

	float aspect() const;
	float fov() const;
	float estimateMandelbulbDistance() const;


	// Update must be called after changing these fields
	Vector3f direction;
	Vector3f position;
	Vector3f up;
	
	float viewportWidth;
	float viewportHeight;
	float scale(const float &unit = 1.0f) const;
};


#endif /* CAMERA_H */