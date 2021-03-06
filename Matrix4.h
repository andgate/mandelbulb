#ifndef MATRIX_4_H
#define MATRIX_4_H

class Vector3f;
class Quaternion;

class Matrix4
{
public:
	Matrix4();
	Matrix4(const Matrix4 &other);
	Matrix4(Matrix4 &&other);

	Matrix4& set(const Matrix4& other);
	const float* asArray() const;

	Matrix4& identity();

	Matrix4& set (const Quaternion &quat);
	Matrix4& set (const float &quaternionX, const float &quaternionY, const float &quaternionZ, const float &quaternionW);
	Matrix4& set ( const float &translationX, const float &translationY, const float &translationZ
		         , const float &quaternionX, const float &quaternionY, const float &quaternionZ, const float &quaternionW);

	Matrix4& setToProjection(float near, float far, float fovy, float aspectRatio);
	
	Matrix4& setToTranslation (const Vector3f &v);
	Matrix4& setToTranslation (float x, float y, float z);

	Matrix4& setToRotation (const float &axisX, const float &axisY, const float &axisZ, const float &degrees);
    Matrix4& setToRotation (const Vector3f &axis, const float &degrees);


	Matrix4& setToLookAt (Vector3f direction, Vector3f up);
	Matrix4& setToLookAt (Vector3f position, Vector3f target, Vector3f up);

	Matrix4& inv();

	Matrix4& add(const Matrix4 &other);
	Matrix4& mul(const Matrix4 &other);

	Matrix4& operator+=(const Matrix4 &other);
	Matrix4& operator*=(const Matrix4 &other);

private:
	float val[16];

public:
	static const int M00 = 0;
	static const int M01 = 4;
	static const int M02 = 8;
	static const int M03 = 12;
	static const int M10 = 1;
	static const int M11 = 5;
	static const int M12 = 9;
	static const int M13 = 13;
	static const int M20 = 2;
	static const int M21 = 6;
	static const int M22 = 10;
	static const int M23 = 14;
	static const int M30 = 3;
	static const int M31 = 7;
	static const int M32 = 11;
	static const int M33 = 15;
};

#endif /* MATRIX_4_H */