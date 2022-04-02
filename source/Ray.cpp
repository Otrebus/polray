#include "Ray.h"

Ray::Ray(const Vector3d& org, const Vector3d& dir) : origin(org), direction(dir)
{
	//previousintersect = 0;
}

Ray::Ray(const Vector3d& org, const Vector3d& dir, IntersectionInfo ii) : origin(org), direction(dir)
{
//	previousintersect = new IntersectionInfo(ii);	
}

Ray& Ray::operator=(const Ray& r)
{
//	if(r.previousintersect)
//		previousintersect = new IntersectionInfo(*r.previousintersect);
//	else
	//	previousintersect = 0;
	origin = r.origin;
	direction = r.direction;
	return *this;
}

Ray::Ray()
{
//	previousintersect = 0;
}

Ray::Ray(const Ray& r)
{
	direction = r.direction;
	origin = r.origin;
	//refractiveindex = r.refractiveindex;
//	if(r.previousintersect)
	//	previousintersect = new IntersectionInfo(*r.previousintersect);
	//else
	//	previousintersect = 0;
}

Vector3d Ray::GetDirection() const
{
	return direction;
}

float Ray::GetRefractiveIndex() const
{
	return 0.0f;
//	return refractiveindex;
}

Ray::~Ray()
{
	//if(previousintersect)
//		delete previousintersect;
}

LineSegment::LineSegment(Vector3d& a, Vector3d& b) : v0(a), v1(b)
{
}

LineSegment::~LineSegment()
{
}

bool LineSegment::CutXYPlane(float z)
{
	if(v1.z > v0.z)
	{
		if(v0.z < z && v1.z > z)
		{
			float t = (z - v0.z)/(v1.z - v0.z);
			v0 = Vector3d(v0.x + (v1.x - v0.x)*t, v0.y + (v1.y - v0.y)*t, z);
		}
		else
			return false;
	}
	else
	{
		if(v1.z < z && v0.z > z)
		{
			float t = (z - v1.z)/(v0.z - v1.z);
			v1 = Vector3d(v1.x + (v0.x - v1.x)*t, v1.y + (v0.y - v1.y)*t, z);
		}
		else
			return false;
	}
}

bool LineSegment::CutYZPlane(float x)
{
	if(v1.x > v0.x)
	{
		if(v0.x < x && v1.x > x)
		{
			float t = (x - v0.x)/(v1.x - v0.x);
			v0 = Vector3d(x, v0.y + (v1.y - v0.y)*t, v0.z + (v1.z - v0.z)*t);
		}
		else
			return false;
	}
	else
	{
		if(v1.x < x && v0.x > x)
		{
			float t = (x - v1.x)/(v0.x - v1.x);
			v1 = Vector3d(x, v1.y + (v0.y - v1.y)*t, v1.z + (v0.z - v1.z)*t);
		}
		else
			return false;
	}
}

bool LineSegment::CutXZPlane(float y)
{
	if(v1.y > v0.y)
	{
		if(v0.y < y && v1.y > y)
		{
			float t = (y - v0.y)/(v1.y - v0.y);
			v0 = Vector3d(v0.x + (v1.x - v0.x)*t, y, v0.z + (v1.z - v0.z)*t);
		}
		else
			return false;
	}
	else
	{
		if(v1.y < y && v0.y > y)
		{
			float t = (y - v1.y)/(v0.y - v1.y);
			v1 = Vector3d(v1.y + (v0.y - v1.y)*t, y, v1.z + (v0.z - v1.z)*t);
		}
		else
			return false;
	}
}

