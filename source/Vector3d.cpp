#include "Matrix3d.h"
#include "Vector3d.h"
#include <limits>

Vector3d::Vector3d(double a, double b, double c) : x(a), y(b), z(c)
{
}

Vector3d::Vector3d()
{
}

Vector3d::~Vector3d()
{
}

Vector3d& Vector3d::operator=(const Vector3d& v)
{
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
}

Vector3d Vector3d::operator+=(const Vector3d& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return Vector3d(x, y, z);
}

Vector3d Vector3d::operator-=(const Vector3d& v)
{
    x += v.x;
    y += v.y;
    z += v.z;
    return Vector3d(x, y, z);
}

Vector3d Vector3d::operator/(const double f) const
{
    return Vector3d(x/f, y/f, z/f);
}

Vector3d Vector3d::operator+(const Vector3d& v) const
{
    return Vector3d(x+v.x, y+v.y, z+v.z);
}

Vector3d Vector3d::operator-(const Vector3d& v) const
{
    return Vector3d(x-v.x, y-v.y, z-v.z); 
}

double Vector3d::operator*(const Vector3d& v) const
{
    return x*v.x + y*v.y + z*v.z;
}
Vector3d Vector3d::operator^(const Vector3d& v) const
{
    return Vector3d(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x);
}

double Vector3d::GetLength() const
{
    return sqrt(x*x+y*y+z*z);
}

void Vector3d::Normalize()
{
    double l = GetLength();

    x /= l;
    y /= l;
    z /= l;
}

Vector3d Vector3d::Normalized() const
{
    double l = GetLength();
    return { x/l, y/l, z/l };
}

Vector3d Vector3d::operator*(double t) const
{
    return Vector3d(t*x,t*y,t*z);
}

Vector3d Vector3d::operator/=(double t)
{
    x/= t;
    y/= t;
    z/= t;
    return *this;
}

Vector3d Vector3d::operator*=(double t)
{
    x*= t;
    y*= t;
    z*= t;
    return *this;
}

////////////////////////////

Vector2d::Vector2d(double a, double b) : x(a), y(b)
{
}

Vector2d::Vector2d()
{
}

Vector2d::~Vector2d()
{
}

Vector2d& Vector2d::operator=(const Vector2d& v)
{
    x = v.x;
    y = v.y;
    return *this;
}

Vector2d Vector2d::operator+(const Vector2d& v) const
{
    return Vector2d(x+v.x, y+v.y);
}

Vector2d Vector2d::operator-(const Vector2d& v) const
{
    return Vector2d(x-v.x, y-v.y); 
}

double Vector2d::operator*(const Vector2d& v) const
{
    return x*v.x + y*v.y;
}

double Vector2d::GetLength() const
{
    return sqrt(x*x+y*y);
}

double Vector2d::operator^(const Vector2d& v) const {
    return x*v.y - y*v.x;
}

void Vector2d::Normalize()
{
    double l = GetLength();

    x /= l;
    y /= l;
}

Vector2d Vector2d::Normalized() const
{
    double l = GetLength();
    return { x/l, y/l };
}

Vector2d Vector2d::operator*(double t) const
{
    return Vector2d(t*x,t*y);
}

Vector3d operator*(double t, const Vector3d& v)
{
    return Vector3d(t*v.x, t*v.y, t*v.z);
}

Vector3d Vector3d::operator-() const
{
    return Vector3d(-x, -y, -z);
}

bool Vector3d::IsNull() const
{
    return x == 0 && y == 0 && z == 0;
}

double& Vector3d::operator[](int t)
{
    return (&x)[t];
}

double Vector3d::operator[](int t) const
{
    return (&x)[t];
}

Vector2d operator*(double t, const Vector2d& v)
{
    return Vector2d(t*v.x, t*v.y);
}

std::ostream& operator << (std::ostream& s , const Vector3d& v)
{
    return(s << "(" << v.x << "," << v.y << "," << v.z << ")");
}

bool Vector3d::IsValid() const
{
    return x < std::numeric_limits<double>::infinity() && x > -std::numeric_limits<double>::infinity() && x == x
    && y < std::numeric_limits<double>::infinity() && y > -std::numeric_limits<double>::infinity() && y == y
    && z < std::numeric_limits<double>::infinity() && z > -std::numeric_limits<double>::infinity() && z == z;
}

bool Vector3d::operator==(const Vector3d& v) const
{
    return v.x == x && v.y == y && v.z == z;
}

bool Vector3d::operator!=(const Vector3d& v) const
{
    return v.x != x || v.y != y || v.z != z;
}

double Vector3d::GetLengthSquared() const
{
    return x*x + y*y + z*z;
}

Vector3d Vector3d::operator*=(const Matrix3d& m)
{
    *this = m**this;
    return *this;
}
