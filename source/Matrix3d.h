#ifndef MATRIX3D_H
#define MATRIX3D_H

#include "Vector3d.h"

class Matrix3d
{
public:
    Matrix3d();
    Matrix3d(float m00, float m01, float m02, float m03,
             float m10, float m11, float m12, float m13,
             float m20, float m21, float m22, float m23,
             float m30, float m31, float m32, float m33);
    ~Matrix3d();

    Vector3d operator *(const Vector3d&) const;
    Matrix3d operator *(const Matrix3d& v) const;
    float& operator() (unsigned char i, unsigned char j);
    float operator() (unsigned char i, unsigned char j) const;

private:
    float m_val[4][4];
};

#endif