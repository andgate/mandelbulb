#include "Camera.h"

#include <iostream>
#include <cmath>
using namespace std;

#include "Vector3f.h"
#include "Constants.h"

Camera::Camera(float viewportWidth, float viewportHeight)
  : position(0.0f, 0.0f, -3.0f)
  , direction(0.0f, 0.0f, 1.0f)
  , up(0.0f, 1.0f, 0.0f)
  , viewportWidth(viewportWidth)
  , viewportHeight(viewportHeight)
{}

void Camera::rotate(const float &angle, const float &axisX, const float &axisY, const float &axisZ)
{
	direction.rotate(angle, axisX, axisY, axisZ);
	up.rotate(angle, axisX, axisY, axisZ);
}

void Camera::rotate(const Vector3f &axis, const float &angle)
{
	direction.rotate(axis, angle);
	up.rotate(axis, angle);
}

float Camera::aspect() const
{
	return viewportWidth / viewportHeight;
}

float inversesqrt(float n)
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = n * 0.5F;
	y  = n;
	i  = * ( long * ) &y;
	i  = 0x5f3759df - ( i >> 1 );
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );

	return y;
}

float sdfMandelbulb_fast(const Vector3f &p)
{
	Vector3f q = p;
	float m = q.dot(q);
	float dr = 1.0f;

	for (int i = 0; i < 4; ++i)
	{
		float m2 = m*m;
		float m4 = m2*m2;
		dr =  8.0f*sqrt(m4*m2*m)*dr + 1.0f;

		float x = q.x; float x2 = x*x; float x4 = x2*x2;
		float y = q.y; float y2 = y*y; float y4 = y2*y2;
		float z = q.z; float z2 = z*z; float z4 = z2*z2;

		float k3 = x2 + z2;
		float k2 = inversesqrt( k3*k3*k3*k3*k3*k3*k3 );
		float k1 = x4 + y4 + z4 - 6.0f*y2*z2 - 6.0f*x2*y2 + 2.0f*z2*x2;
        float k4 = x2 - y2 + z2;

        q.x = p.x +  64.0f*x*y*z*(x2-z2)*k4*(x4-6.0f*x2*z2+z4)*k1*k2;
        q.y = p.y + -16.0f*y2*k3*k4*k4 + k1*k1;
        q.z = p.z +  -8.0f*y*k4*(x4*x4 - 28.0f*x4*x2*z2 + 70.0f*x4*z4 - 28.0f*x2*z2*z4 + z4*z4)*k1*k2;

        m = q.dot(q);
		if( m > 256.0f )
            break;
	}

	return 0.25f*log(m)*sqrt(m)/dr;
}

float sdfMandelbulb(const Vector3f &p, const int &power)
{
	Vector3f q(p);
	float r = q.length();
	float dr = 1.0f;

	int i = MAX_ITER;
	while (r < DIVERGENCE && i-- > 0)
	{
		float ph = asinf( q.z/r );
		float th = atanf( q.y / q.x );
		float zr = powf( r, power - 1.0f );

		dr = zr * dr * power + 1.0f;
		zr *= r;

		float sph = sin(power*ph); float cph = cos(power*ph);
		float sth = sin(power*th); float cth = cos(power*th);

        q.x = zr * cph*cth + p.x;
		q.y = zr * cph*sth + p.y;
		q.z = zr * sph     + p.z;

		r = q.length();
	}

	return 0.5f*log(r)*r/dr;
}


float Camera::estimateMandelbulbDistance() const
{
	return sdfMandelbulb_fast(position);
}


float Camera::scale(const float &unit) const
{
	float mdist = estimateMandelbulbDistance();
	if (abs(mdist) >= unit) return unit;
	else return abs(mdist / unit);
}