#include "cubemap.h"

Cubemap::Cubemap(Texture front, Texture back, Texture left, Texture right, Texture up, Texture down) : m_north(front), m_south(back), m_west(left), m_east(right), m_top(up), m_bottom(down)
{
}

int Cubemap::GetTexel(const Vector3d& v) const
{
	Vector3d w = v;
	w.Normalize();
	const Texture* face; 

	// These cases can be optimized a little more to just check for the largest part and its sign

	// South side (positive z)
	if(v.z >= fabsf(v.x) && v.z >= fabsf(v.y))
	{
		face = &m_south;
		w.x = -w.x;
		w.z = -w.z;
	}

	// North side
	else if(-v.z >= fabsf(v.x) && -v.z >= fabsf(v.y))
	{
		face = &m_north;
	}

	// West side
	else if(-v.x >= fabsf(v.z) && -v.x >= fabsf(v.y))
	{
		face = &m_west;
		w.x = -v.z;
		w.z = v.x;
	}

	// East side
	else if(v.x >= fabsf(v.z) && v.x >= fabsf(v.y))
	{
		face = &m_east;
		w.x = v.z;
		w.z = -v.x;
	}

	// Top side
	else if(v.y >= fabsf(v.x) && v.y >= fabsf(v.z))
	{
		face = &m_top;
		w.y = v.z;
		w.z = -v.y;
	}

	// Bottom side
	else
	{
		face = &m_bottom;
		w.y = -v.z;
		w.z = v.y;
	}
	
	// Project the vector onto the plane z = -0.5 and then read the coordinates directly from w.x and w.y
	float t = -0.5f/w.z;
	w = w*t;
	float X = 0.5f + w.x;
	float Y = 0.5f - w.y;
	//return face->GetTexel((int)(X*float(face->GetWidth())), (int)(Y*float(face->GetHeight()))).GetInt();
	Color c = face->GetTexelBLInterp((X*float(face->GetWidth() - 1)), (Y*float(face->GetHeight() - 1)));
	return c.GetInt();
}

Cubemap::~Cubemap()
{
}