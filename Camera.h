#ifndef CAMERA_H
#define CAMERA_H

#include "Matrix4.h"
#include "Vector3f.h"

class Camera
{
public:
	Camera();

	void update();

	const float* getMatrix() const;

	// Update must be called after changing these fields
	Vector3f direction;
	Vector3f position;
	Vector3f up;

private:
	float fieldOfView;

	Matrix4 projection;
	Matrix4 view;
	Matrix4 combined;
	Matrix4 invProjectionView;

	float near;
	float far;

	float viewportWidth;
	float viewportHeight;
};


#endif /* CAMERA_H */