#include "Triangle.h"
#include "Scene.h"
#include "Material.h"
#include "Utils.h"

Triangle::Triangle(double v1x, double v1y, double v1z, double v2x, double v2y, double v2z, double v3x, double v3y, double v3z)
{
    v0.pos.x = v1x; v0.pos.y = v1y; v0.pos.z = v1z;
    v1.pos.x = v2x; v1.pos.y = v2y; v1.pos.z = v2z;
    v2.pos.x = v3x; v2.pos.y = v3y; v2.pos.z = v3z;
}

Triangle::Triangle(const Vector3d& a, const Vector3d& b, const Vector3d& c) : v0(a), v1(b), v2(c)
{
    Vector3d normal = (b-a)^(c-a);
    normal.Normalize();
    v0.normal = v1.normal = v2.normal = normal;
}

Triangle::Triangle()
{
}

Triangle::Triangle(const Vertex3d& a, const Vertex3d& b, const Vertex3d& c) : v0(a), v1(b), v2(c)
{
}

Triangle::~Triangle()
{
}

double Triangle::Intersect(const Ray& ray) const
{
    return IntersectTriangle(v0.pos, v1.pos, v2.pos, ray);
}

bool Triangle::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
{
    Vector3d D = ray.direction;

    info.direction = ray.direction;

    Vector3d E1 = v1.pos-v0.pos;
    Vector3d E2 = v2.pos-v0.pos;
    Vector3d T = ray.origin - v0.pos;

    Vector3d P = E2^T;
    Vector3d Q = E1^D;

    double det = E2*Q;

    auto u = D*P/det, v = T*Q/det, t = E1*P/det;

    info.normal = u*(v1.normal-v0.normal) + v*(v2.normal-v0.normal) + v0.normal;
    info.normal.Normalize();

    info.geometricnormal = E1^E2;
    info.geometricnormal.Normalize();

    info.position = v0.pos + u*E1 + v*E2 + (info.geometricnormal*info.direction < 0 ? info.geometricnormal*eps : -info.geometricnormal*eps);
    //info.texpos.x = u;
    //info.texpos.y = v;
    info.material = material;

    return true;
}

BoundingBox Triangle::GetBoundingBox() const
{
    Vector3d c1, c2;
    c1.x = min(v0.pos.x, min(v1.pos.x, v2.pos.x));
    c1.y = min(v0.pos.y, min(v1.pos.y, v2.pos.y));
    c1.z = min(v0.pos.z, min(v1.pos.z, v2.pos.z));

    c2.x = max(v0.pos.x, max(v1.pos.x, v2.pos.x));
    c2.y = max(v0.pos.y, max(v1.pos.y, v2.pos.y));
    c2.z = max(v0.pos.z, max(v1.pos.z, v2.pos.z));
    return BoundingBox(c1, c2);
}

bool Triangle::GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox) const
{
    vector<Vector3d> points = { v0.pos, v1.pos, v2.pos };

	ClipPolygonToAAP(0, true, clipbox.c1.x, points); // Left side of the bounding box
	ClipPolygonToAAP(0, false, clipbox.c2.x, points); // Right
	ClipPolygonToAAP(1, true, clipbox.c1.y, points); // Bottom
	ClipPolygonToAAP(1, false, clipbox.c2.y, points); // Top
	ClipPolygonToAAP(2, true, clipbox.c1.z, points); // Front
	ClipPolygonToAAP(2, false, clipbox.c2.z, points); // Back

	resultbox.c1.x = numeric_limits<double>::infinity();
	resultbox.c2.x = -numeric_limits<double>::infinity();
	resultbox.c1.y = numeric_limits<double>::infinity();
	resultbox.c2.y = -numeric_limits<double>::infinity();
	resultbox.c1.z = numeric_limits<double>::infinity();
	resultbox.c2.z = -numeric_limits<double>::infinity();

	for(auto v : points)
	{
		resultbox.c1.x = v.x < resultbox.c1.x ? v.x : resultbox.c1.x;
		resultbox.c2.x = v.x > resultbox.c2.x ? v.x : resultbox.c2.x;
		resultbox.c1.y = v.y < resultbox.c1.y ? v.y : resultbox.c1.y;
		resultbox.c2.y = v.y > resultbox.c2.y ? v.y : resultbox.c2.y;
		resultbox.c1.z = v.z < resultbox.c1.z ? v.z : resultbox.c1.z;
		resultbox.c2.z = v.z > resultbox.c2.z ? v.z : resultbox.c2.z;
	}

	if(points.size() > 2)
		return true;
	else
		return false;
}

void Triangle::AddToScene(Scene& scene)
{
    Scene::PrimitiveAdder::AddPrimitive(scene, this);
    Scene::MaterialAdder::AddMaterial(scene, material);
}

void Triangle::Save(Bytestream& stream) const
{
    stream << ID_TRIANGLE << v0 << v1 << v2;
    material->Save(stream);
}

void Triangle::Load(Bytestream& stream)
{
    unsigned char matId;
    stream >> v0 >> v1 >> v2;
    stream >> matId;
    material = Material::Create(matId);
    material->Load(stream);
}