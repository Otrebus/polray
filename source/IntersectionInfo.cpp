#include "intersectioninfo.h"

IntersectionInfo::IntersectionInfo(Vector3d& pos, Vector3d& nor, Vector3d& raypos) : position(pos), normal(nor), rayposition(raypos)
{
}

IntersectionInfo::IntersectionInfo()
{
}

IntersectionInfo::~IntersectionInfo()
{
}

Vector3d IntersectionInfo::GetNormal() const
{
	return normal;
}

Vector3d IntersectionInfo::GetPosition() const
{
	return position;
}

Vector3d IntersectionInfo::GetRayPosition() const
{
	return rayposition;
}

Vector2d IntersectionInfo::GetTexturePosition() const
{
	return texpos;
}

Material* IntersectionInfo::GetMaterial() const
{
	return material;
}

Vector3d IntersectionInfo::GetDirection() const
{
	return direction;
}

Vector3d IntersectionInfo::GetGeometricNormal() const
{
	return geometricnormal;
}