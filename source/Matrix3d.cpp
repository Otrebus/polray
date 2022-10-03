#include "Matrix3d.h"
#include "assert.h"

/**
 * Constructor.
 */
Matrix3d::Matrix3d()
{

}

/**
 * Constructor.
 * 
 * @param m00 The value top, left in the matrix.
 * @param m01 The value top, middle left in the matrix.
 * @param m02 The value top, middle right in the matrix.
 * @param m03 The value top, right in the matrix.
 * @param m10 The value top middle, left in the matrix.
 * @param m11 The value top middle, middle left in the matrix.
 * @param m12 The value top middle, middle right in the matrix.
 * @param m13 The value top middle, right in the matrix.
 * @param m20 The value bottom middle, left in the matrix.
 * @param m21 The value bottom middle, middle left in the matrix.
 * @param m22 The value bottom middle, middle right in the matrix.
 * @param m23 The value bottom middle, right in the matrix.
 * @param m30 The value bottom, left in the matrix.
 * @param m31 The value bottom, middle left in the matrix.
 * @param m32 The value bottom, middle right in the matrix.
 * @param m33 The value bottom, right in the matrix.
 */
Matrix3d::Matrix3d(double m00, double m01, double m02, double m03,
                   double m10, double m11, double m12, double m13,
                   double m20, double m21, double m22, double m23,
                   double m30, double m31, double m32, double m33)
{
    m_val[0][0] = m00; m_val[0][1] = m01; m_val[0][2] = m02; m_val[0][3] = m03;
    m_val[1][0] = m10; m_val[1][1] = m11; m_val[1][2] = m12; m_val[1][3] = m13;
    m_val[2][0] = m20; m_val[2][1] = m21; m_val[2][2] = m22; m_val[2][3] = m23;
    m_val[3][0] = m30; m_val[3][1] = m31; m_val[3][2] = m32; m_val[3][3] = m33;
}

/**
 * Returns a reference to the value stored in matrix position (row, col)
 * 
 * @param i The row.
 * @param j The column.
 * @returns A reference to the value M_ij
 */
double& Matrix3d::operator() (int i, int j)
{
    assert(i >= 0 && j <= 3 && j >= 0 && j <= 3);
    return m_val[i][j];
}

/**
 * Returns the value stored in matrix position (row, col)
 * 
 * @param i The row.
 * @param j The column.
 * @returns The value M_ij
 */
double Matrix3d::operator() (int i, int j) const
{
    assert(i >= 0 && j <= 3 && j >= 0 && j <= 3);
    return m_val[i][j];
}

/**
 * Destructor.
 */
Matrix3d::~Matrix3d()
{
}

/**
 * Matrix-vector multiplication.
 * 
 * @param v The vector to multiply with.
 * @returns The resulting vector.
 */
Vector3d Matrix3d::operator *(const Vector3d& v) const
{
    return Vector3d(m_val[0][0]*v.x + m_val[0][1]*v.y + m_val[0][2]*v.z + m_val[0][3],
                    m_val[1][0]*v.x + m_val[1][1]*v.y + m_val[1][2]*v.z + m_val[1][3],
                    m_val[2][0]*v.x + m_val[2][1]*v.y + m_val[2][2]*v.z + m_val[2][3]);
}

/**
 * Matrix-matrix multiplication.
 * 
 * @param m The matrix to multiply with.
 * @returns The resulting matrix.
 */
Matrix3d Matrix3d::operator*(const Matrix3d& m) const
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