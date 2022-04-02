#include "Matrix3d.h"
#include "assert.h"

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
Matrix3d::Matrix3d()
{

}

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
Matrix3d::Matrix3d(float m00, float m01, float m02, float m03,
             float m10, float m11, float m12, float m13,
             float m20, float m21, float m22, float m23,
             float m30, float m31, float m32, float m33)
{
    m_val[0][0] = m00; m_val[0][1] = m01; m_val[0][2] = m02; m_val[0][3] = m03;
    m_val[1][0] = m10; m_val[1][1] = m11; m_val[1][2] = m12; m_val[1][3] = m13;
    m_val[2][0] = m20; m_val[2][1] = m21; m_val[2][2] = m22; m_val[2][3] = m23;
    m_val[3][0] = m30; m_val[3][1] = m31; m_val[3][2] = m32; m_val[3][3] = m33;
}

//-----------------------------------------------------------------------------
// Returns a reference to the value stored in matrix position (row,col)
//-----------------------------------------------------------------------------
float& Matrix3d::operator() (unsigned char i, unsigned char j)
{
    assert(i >= 0 && j <= 3 && j >= 0 && j <= 3);
    return m_val[i][j];
}

//-----------------------------------------------------------------------------
// Returns a reference to the value stored in matrix position (row,col)
//-----------------------------------------------------------------------------
float Matrix3d::operator() (unsigned char i, unsigned char j) const
{
    assert(i >= 0 && j <= 3 && j >= 0 && j <= 3);
    return m_val[i][j];
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
Matrix3d::~Matrix3d()
{
}

//------------------------------------------------------------------------------
// Matrix-vector multiplication
//------------------------------------------------------------------------------
Vector3d Matrix3d::operator *(const Vector3d& v) const
{
    return Vector3d(m_val[0][0]*v.x + m_val[0][1]*v.y + m_val[0][2]*v.z + m_val[0][3],
                    m_val[1][0]*v.x + m_val[1][1]*v.y + m_val[1][2]*v.z + m_val[1][3],
                    m_val[2][0]*v.x + m_val[2][1]*v.y + m_val[2][2]*v.z + m_val[2][3]);
}

//------------------------------------------------------------------------------
// Matrix-matrix multiplication
//------------------------------------------------------------------------------
Matrix3d Matrix3d::operator *(const Matrix3d& m) const
{
    Matrix3d result;
    for(int i = 0; i < 4; i++)
    {
        for(int j = 0; j < 4; j++)
        {
            result(i,j) = 0;
            for(int k = 0; k < 4; k++)
                result(i,j) += (*this)(i,k)*m(k,j);
        }
    }
    return result;
}