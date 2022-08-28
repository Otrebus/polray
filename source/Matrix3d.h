#pragma once

#include "Vector3d.h"

class Matrix3d
{
public:
    Matrix3d();
    Matrix3d(double m00, double m01, double m02, double m03,
             double m10, double m11, double m12, double m13,
             double m20, double m21, double m22, double m23,
             double m30, double m31, double m32, double m33);
    ~Matrix3d();

    Vector3d operator *(const Vector3d&) const;
    Matrix3d operator *(const Matrix3d& v) const;
    double& operator() (int i, int j);
    double operator() (int i, int j) const;

private:
    double m_val[4][4];
};
