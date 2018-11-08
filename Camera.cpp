#include "Camera.h"

#include <cmath>
#include "Vector3f.h"

Camera::Camera()
  : fieldOfView(67)
  , position(0.0f, 0.0f, -3.0f)
  , direction(0.0f, 0.0f, 1.0f)
  , up(0.0f, 1.0f, 0.0f)
  , projection()
  , view()
  , combined()
  , invProjectionView()
  , near(1.0f)
  , far(100.0)
  , viewportWidth(0.0f)
  , viewportHeight(0.0f)
{}

const float* Camera::getMatrix() const
{
	return combined.getMatrix();
}

void Camera::update()
{
	float aspect = viewportWidth / viewportHeight;
	projection.setToProjection(abs(near), abs(far), fieldOfView, aspect);
	view.setToLookAt(position, Vector3f(position).add(direction), up);
	combined = projection;
	combined *= view;
}