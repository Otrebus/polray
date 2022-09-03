#include "IntersectionInfo.h"

IntersectionInfo::IntersectionInfo(Vector3d& pos, Vector3d& nor, Vector3d& raypos) : position(pos), normal(nor), rayposition(raypos)
{
}

IntersectionInfo::IntersectionInfo()
{
}

IntersectionInfo::~IntersectionInfo()
{
}