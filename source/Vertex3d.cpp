#include "vertex3d.h"

Vertex3d::Vertex3d() : normal(0, 0, 0)
{
}

Vertex3d::Vertex3d(double x, double y, double z) : pos(x, y, z), normal(0, 0, 0)
{
}

Vertex3d::Vertex3d(double x, double y, double z, double nx, double ny, double nz, double u, double v) : pos(x, y, z), normal(nx, ny, nz), texpos(u, v)
{
}

Vertex3d::Vertex3d(const Vector3d& po, const Vector3d& norm, const Vector2d& tp) : pos(po), normal(norm), texpos(tp)
{
}

Vertex3d::Vertex3d(const Vector3d& po) : pos(po), normal(0, 0, 0)
{
}

Vertex3d::~Vertex3d()
{
}