/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Vector3d.cpp
 * 
 * Implementation of the Vector3d class for vector arithmetic.
 */

#include "Matrix3d.h"
#include "Vector3d.h"

/**
 * Constructor.
 * 
 * @param x x
 * @param y y
 * @param z z
 */
Vector3d::Vector3d(double x, double y, double z) : x(x), y(y), z(z)
{
}

/**
 * Constructor.
 */
Vector3d::Vector3d()
{
}

/**
 * Destructor.
 */
Vector3d::~Vector3d()
{
}

/**
 * Assignment.
 * 
 * @param v The vector whose value we assign to this vector.
 * @returns A reference to this vector.
 */
Vector3d& Vector3d::operator=(const Vector3d& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

/**
 * Adds a given vector to the vector.
 * 
 * @param v The vector to subtract.
 * @returns The vector difference.
 */
Vector3d Vector3d::operator+=(const Vector3d& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return Vector3d(x, y, z);
}

/**
 * Subtracts the vector by the given vector.
 * 
 * @param v The vector to subtract.
 * @returns The vector difference.
 */
Vector3d Vector3d::operator-=(const Vector3d& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return Vector3d(x, y, z);
}

/**
 * Vector/scalar division.
 * 
 * @param v The vector to add.
 * @returns The vector sum.
 */
Vector3d Vector3d::operator/(double f) const
{
    return Vector3d(x/f, y/f, z/f);
}

/**
 * Vector addition.
 * 
 * @param v The vector to add.
 * @returns The vector sum.
 */
Vector3d Vector3d::operator+(const Vector3d& v) const
{
    return Vector3d(x+v.x, y+v.y, z+v.z);
}

/**
 * Vector subtraction.
 * 
 * @param v The vector to subtract.
 * @returns The vector difference.
 */
Vector3d Vector3d::operator-(const Vector3d& v) const
{
    return Vector3d(x-v.x, y-v.y, z-v.z); 
}

/**
 * Scalar product.
 * 
 * @param v The vector to form the scalar product with.
 * @returns The scalar product.
 */
double Vector3d::operator*(const Vector3d& v) const
{
    return x*v.x + y*v.y + z*v.z;
}

/**
 * Vector product.
 * 
 * @param v The vector to form the product with.
 * @returns The vector sum.
 */
Vector3d Vector3d::operator^(const Vector3d& v) const
{
    return Vector3d(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);
}

/**
 * Returns the norm of the vector.
 */
double Vector3d::Length() const
{
    return sqrt(x*x + y*y + z*z);
}

/**
 * Normalizes the vector.
 */
void Vector3d::Normalize()
{
    double l = Length();

    x /= l;
    y /= l;
    z /= l;
}

/**
 * Returns the normalized vector.
 * 
 * @returns The normalized vector.
 */
Vector3d Vector3d::Normalized() const
{
    double l = Length();
    return { x/l, y/l, z/l };
}

/**
 * Scalar/vector multiplication.
 * 
 * @param t The scalar to multiply by.
 * @returns The product.
 */
Vector3d Vector3d::operator*(double t) const
{
    return Vector3d(t*x, t*y, t*z);
}

/**
 * Vector addition.
 * 
 * @param v The vector to add.
 * @returns The vector sum.
 */
Vector3d Vector3d::operator/=(double t)
{
    x/= t;
    y/= t;
    z/= t;
    return *this;
}

/**
 * Multiplies the vector with a scalar.
 * 
 * @param t The scalar to multiply by.
 * @returns The product.
 */
Vector3d Vector3d::operator*=(double t)
{
    x*= t;
    y*= t;
    z*= t;
    return *this;
}

/**
 * Constructor.
 * 
 * @param x x
 * @param y y
 */
Vector2d::Vector2d(double x, double y) : x(x), y(y)
{
}

/**
 * Constructor.
 */
Vector2d::Vector2d()
{
}

/**
 * Destructor.
 */
Vector2d::~Vector2d()
{
}

/**
 * Checks for equality.
 * 
 * @param v The vector to test against.
 * @returns True if the vectors are equal.
 */
Vector2d& Vector2d::operator=(const Vector2d& v)
{
    x = v.x;
    y = v.y;
    return *this;
}

/**
 * Vector addition.
 * 
 * @param v The vector to add.
 * @returns The vector sum.
 */
Vector2d Vector2d::operator+(const Vector2d& v) const
{
    return Vector2d(x+v.x, y+v.y);
}

/**
 * Vector subtraction.
 * 
 * @param v The vector to subtract.
 * @returns The vector difference.
 */
Vector2d Vector2d::operator-(const Vector2d& v) const
{
    return Vector2d(x-v.x, y-v.y); 
}

/**
 * Scalar product.
 * 
 * @param v The other vector to form the scalar product with.
 * @returns The scalar product.
 */
double Vector2d::operator*(const Vector2d& v) const
{
    return x*v.x + y*v.y;
}

/**
 * Returns the length of the vector.
 * 
 * @returns The euclidean norm of the vector.
 */
double Vector2d::Length() const
{
    return sqrt(x*x + y*y);
}

/**
 * Returns the 2d "vector product" of the two vectors, same as the 3d vector product if we let
 * z = 0 and then return the value of the z component of the vector product.
 * 
 * @param v The vector on the right side of the vector product.
 * @returns The vector product.
 */
double Vector2d::operator^(const Vector2d& v) const
{
    return x*v.y - y*v.x;
}

/**
 * Compares two vectors component-wise, with the x component as the primary and y as the
 * secondary comparison.
 * 
 * @returns True if this vector is smaller than the other.
 */
bool Vector2d::operator<(const Vector2d& v) const
{
    return std::make_pair(x, y) < std::make_pair(v.x, v.y);
}

/**
 * Normalizes the vector.
 */
void Vector2d::Normalize()
{
    double l = Length();

    x /= l;
    y /= l;
}

/**
 * Returns the normalized vector.
 * 
 * @returns The normalized vector.
 */
Vector2d Vector2d::Normalized() const
{
    double l = Length();
    return { x/l, y/l };
}

/**
 * Scalar multiplication of a vector.
 * 
 * @param t The scalar to multiply with.
 * @returns The product.
 */
Vector2d Vector2d::operator*(double t) const
{
    return Vector2d(t*x, t*y);
}

/**
 * Scalar multiplication of a vector.
 * 
 * @param t The real number to scale with.
 * @param v The vector to scale.
 * @returns The value of the vector multiplied with the scalar.
 */
Vector3d operator*(double t, const Vector3d& v)
{
    return Vector3d(t*v.x, t*v.y, t*v.z);
}

/**
 * Vector negation.
 * 
 * @returns The opposite vector.
 */
Vector3d Vector3d::operator-() const
{
    return Vector3d(-x, -y, -z);
}

/**
 * Returns true if the vector is the null vector.
 * 
 * @returns True if the vector is all zeroes.
 */
bool Vector3d::operator!() const
{
    return x == 0 && y == 0 && z == 0;
}


/**
 * Returns a reference to a component of the vector.
 * 
 * @param t The component to return (x=0, y=1, z=2).
 * @returns A reference to the result.
 */
double& Vector3d::operator[](int t)
{
    return (&x)[t];
}

/**
 * Returns a reference to a component of the vector.
 * 
 * @param t The component to return (x=0, y=1, z=2).
 * @returns A reference to the result.
 */
double Vector3d::operator[](int t) const
{
    return (&x)[t];
}

/**
 * Scalar multiplication of a vector.
 * 
 * @param t The scalar to multiply with.
 * @param v The vector to multiply with.
 * @returns The result.
 */
Vector2d operator*(double t, const Vector2d& v)
{
    return Vector2d(t*v.x, t*v.y);
}

/**
 * Outputs the vector to an ostream.
 * 
 * @param s The ostream to stream to.
 * @param s The vector to output.
 * @returns A reference to the ostream.
 */
std::ostream& operator<<(std::ostream& s, const Vector3d& v)
{
    return(s << "(" << v.x << "," << v.y << "," << v.z << ")");
}

/**
 * Sanity checking.
 * 
 * @returns True if all components are finite.
 */
bool Vector3d::IsValid() const
{
    return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
}

/**
 * Equality testing.
 * 
 * @param v The vector to test for equality.
 * @returns True if at all vector components are equal.
 */
bool Vector3d::operator==(const Vector3d& v) const
{
    return v.x == x && v.y == y && v.z == z;
}

/**
 * Inequality testing.
 * 
 * @param v The vector to test for inequality.
 * @returns True if at least one vector component is unequal.
 */
bool Vector3d::operator!=(const Vector3d& v) const
{
    return v.x != x || v.y != y || v.z != z;
}

/**
 * Length squared.
 * 
 * @returns The squared length of the vector.
 */
double Vector3d::Length2() const
{
    return x*x + y*y + z*z;
}

/**
 * Matrix multiplication.
 * 
 * @param m The matrix to multiply the vector with.
 * @returns The resulting vector.
 */
Vector3d Vector3d::operator*=(const Matrix3d& m)
{
    *this = m**this;
    return *this;
}
