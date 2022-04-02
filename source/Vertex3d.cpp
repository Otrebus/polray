#include "vertex3d.h"

Vertex3d::Vertex3d()
{
	normal.x = 0;
	normal.y = 0;
	normal.z = 0;
}

Vertex3d::Vertex3d(float x, float y, float z)
{
	normal.x = 0;
	normal.y = 0;
	normal.z = 0;
	pos.x = x;
	pos.y = y;
	pos.z = z;
}

Vertex3d::Vertex3d(float x, float y, float z, float nx, float ny, float nz, float u, float v)
{
	pos.x = x; pos.y = y; pos.z = z; 
	normal.x = nx; normal.y = ny; normal.z = nz; 
	texpos.x = u; texpos.y = v;
}

Vertex3d::Vertex3d(const Vector3d& po, const Vector3d& norm, const Vector2d& tp) : pos(po), normal(norm), texpos(tp)
{
}

Vertex3d::Vertex3d(const Vector3d& po) : pos(po)
{
	normal.x = 0;
	normal.y = 0;
	normal.z = 0;
}

Vertex3d::~Vertex3d()
{
}