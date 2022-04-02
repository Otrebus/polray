#ifndef VECTOR3D_H
#define VECTOR3D_H

class Matrix3d;

#include <cmath>
#include <sstream>

class Vector3d
{
public:
	Vector3d(float x, float y, float z);
	~Vector3d();
	Vector3d();

	Vector3d& operator=(const Vector3d&);
	Vector3d operator+(const Vector3d&) const;
	Vector3d operator-(const Vector3d&) const;
	Vector3d operator-() const;

	Vector3d operator+=(const Vector3d& v);
	Vector3d operator-=(const Vector3d& v);

    Vector3d operator*=(const Matrix3d& m);
    bool operator!=(const Vector3d&) const;
    bool operator==(const Vector3d&) const;
	float operator*(const Vector3d&) const;
	Vector3d operator^(const Vector3d&)	const;
	Vector3d operator/(const float) const;
	Vector3d operator/=(float);
	Vector3d operator*=(float);
	Vector3d operator*(const float) const;
	float& operator[](int);
	float operator[](int) const;
	
	float GetLength() const;
    float GetLengthSquared() const;
    void Normalize();
	bool IsNull() const;
	bool IsValid() const;

	float x, y, z;
};

class Vector2d
{
public:
	Vector2d(float x, float y);
	~Vector2d();
	Vector2d();

	Vector2d& operator=(const Vector2d&);
	Vector2d operator+(const Vector2d&) const;
	Vector2d operator-(const Vector2d&) const;

	float operator*(const Vector2d&) const;
	Vector2d operator*(const float) const;

	float GetLength() const;
	void Normalize();

	float x, y;
};

Vector3d operator*(float t, const Vector3d& v);
std::ostream& operator<<(std::ostream& str, const Vector3d& v);

#endif