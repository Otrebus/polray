#include "Triangle.h"
#include "Scene.h"
#include "Material.h"

Triangle::Triangle(float v1x, float v1y, float v1z, float v2x, float v2y, float v2z, float v3x, float v3y, float v3z)
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
/*
// Möller-Trumbore triangle-ray intersection
float Triangle::Intersect(const Ray& ray)
{
	float u, v, t;
	Vector3d D;
	D.x = ray.direction.x;
	D.y = ray.direction.y;
	D.z = ray.direction.z;

	Vector3d E1 = v1.pos-v0.pos;
	Vector3d E2 = v2.pos-v0.pos;
	Vector3d T = ray.origin - v0.pos;

	Vector3d P = E2^T;
	Vector3d Q = E1^D;

	float det = E2*Q;
	if(det < 0.00001f && det > -0.00001f) // Ray in (almost) the same plane as the triangle
		return -1.0f;

	u = D*P/det;

	if(u > 1 || u < 0)
		return -1.0f;

	v = T*Q/det;

	if(u+v > 1 || u < 0 || v < 0)
		return -1.0f;

	t = E1*P/det;
	if(t < 0.00001f)
		return -1.0f;


	return t;
}*/

float Triangle::Intersect(const Ray& ray) const
{
	float u, v, t;
	Vector3d D;
	D.x = ray.direction.x;
	D.y = ray.direction.y;
	D.z = ray.direction.z;

	// THIS IS HORRIBLE FOR MULTITHREADING
	//numintersects++;

	Vector3d E1 = v1.pos-v0.pos;
	Vector3d E2 = v2.pos-v0.pos;
	Vector3d T = ray.origin - v0.pos;

	Vector3d P = E2^T;
	Vector3d Q = E1^D;

	float det = E2*Q;
	if(det < 0.0000000001f && det > -0.0000000001f)	// Ray in (almost) the same plane as the triangle
		return -1.0f;							// NOTE: depending on this epsilon value, extremely
	u = D*P/det;								// small triangles could be missed

	if(u > 1 || u < 0)
		return -1.0f;

	v = T*Q/det;

	if(u+v > 1 || u < 0 || v < 0)
		return -1.0f;

	t = E1*P/det;
	if(t < 0.0000000001f)
		return -1.0f;

	return t;
}

bool Triangle::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
{
	float u, v, t;
	Vector3d D;

	info.direction = ray.direction;

	D.x = ray.direction.x;
	D.y = ray.direction.y;
	D.z = ray.direction.z;

	Vector3d E1 = v1.pos-v0.pos;
	Vector3d E2 = v2.pos-v0.pos;
	Vector3d T = ray.origin - v0.pos;

	Vector3d P = E2^T;
	Vector3d Q = E1^D;

	float det = E2*Q;
	if(det < 0.0000000001f && det > -0.0000000001f) // Ray in (almost) the same plane as the triangle
		return false;

	u = D*P/det;

	if(u > 1 || u < 0)
		return false;

	v = T*Q/det;

	if(u+v > 1 || u < 0 || v < 0)
		return false;

	t = E1*P/det;
	if(t < 0.0000000001f)
		return false;

/*	if(material && material->normalmap)
	{
		Vector2d tex1(u * (v1->texpos.x - v0->texpos.x),
			          u * (v1->texpos.y - v0->texpos.y));
		Vector2d tex2(v * (v2->texpos.x - v0->texpos.x),
			          v * (v2->texpos.y - v0->texpos.y));

		Vector2d txc = Vector2d(v0->texpos.x, v0->texpos.y) + tex1+tex2;
		// If there's a problem with normal mapping not looking right, try normalizing this vector.
		Color nc = material->normalmap->GetTexelBLInterp(txc.x, txc.y);
		float nx = (nc.r - 0.5f)*2.0f;
		float ny = (nc.g - 0.5f)*2.0f;
		float nz = nc.b;

		info.normal = (*tangent)*nx + (*binormal)*ny + (*normal)*nz;
	}*/
	//else
	{
		/*Vector3d n0 = v0->normal*GetNormal() > 0.8 ? v0->normal : GetNormal();
		Vector3d n1 = v1->normal*GetNormal() > 0.8 ? v1->normal : GetNormal();
		Vector3d n2 = v2->normal*GetNormal() > 0.8 ? v2->normal : GetNormal();*/

		Vector3d n0 = v0.normal;
		Vector3d n1 = v1.normal;
		Vector3d n2 = v2.normal;

		info.normal = u*(v1.normal-v0.normal) + v*(v2.normal-v0.normal) + v0.normal;
        //info.normal = GetNormal();
		//info.normal = GetNormal();
		/*
		if(info.normal.x == 0 && info.normal.y == 0 && info.normal.z == 0)
		{
			stringstream sstr33;
			std::string str33 = sstr33.str();
			sstr33 << "sdf " << v0->normal << " " << v1->normal << " " << v2->normal;
			logger.Box(sstr33.str());
			//_asm int 3;
		}
		if(info.normal.x != info.normal.x || info.normal.y != info.normal.y || info.normal.z != info.normal.z)
			
		{
			stringstream sstr33;
			std::string str33 = sstr33.str();
			sstr33 << "sdf " << v0->normal << " " << v1->normal << " " << v2->normal;
			logger.Box(sstr33.str());
			//_asm int 3;
		}

			if(info.normal.GetLength() < 0.00001f)
		{
			stringstream sstr33;
			std::string str33 = sstr33.str();
			sstr33 << "sdf " << v0->normal << " " << v1->normal << " " << v2->normal << "  " << info.normal ;
			logger.Box(sstr33.str());
			_asm int 3;
		}
		//_asm int 3;
		if(info.normal.GetLength() > 2)
		
		{
			stringstream sstr33;
			std::string str33 = sstr33.str();
			sstr33 << "sdf " << v0->normal << " " << v1->normal << " " << v2->normal;
			logger.Box(sstr33.str());
			//_asm int 3;
		}*/
	//	info.normal = GetNormal();
	//	info.normal = v0->normal;
		//info.normal = E1^E2;
	}
    //info.normal = info.geometricnormal = E1^E2;
    info.normal.Normalize();
    info.geometricnormal = E1^E2;
    info.geometricnormal.Normalize();
	//info.geometricnormal = GetNormal();
	//info.normal = GetNormal();
	//info.normal = GetNormal(); // temp

	info.normal.Normalize();
	/*
	if(info.normal.GetLength() < 0.00001f)
		{
			stringstream sstr33;
			std::string str33 = sstr33.str();
			sstr33 << "sdf " << v0->normal << " " << v1->normal << " " << v2->normal << "  " << info.normal ;
			logger.Box(sstr33.str());
			_asm int 3;
		}

	if(info.normal.x == 0 && info.normal.y == 0 && info.normal.z == 0)
		{
			stringstream sstr33;
			std::string str33 = sstr33.str();
			sstr33 << "sdf " << v0->normal << " " << v1->normal << " " << v2->normal;
			logger.Box(sstr33.str());
			//_asm int 3;
		}
		if(info.normal.x != info.normal.x || info.normal.y != info.normal.y || info.normal.z != info.normal.z)
			
		{
			stringstream sstr33;
			std::string str33 = sstr33.str();
			sstr33 << "sdf " << v0->normal << " " << v1->normal << " " << v2->normal;
			logger.Box(sstr33.str());
			//_asm int 3;
		}

			if(info.normal.GetLength() < 0.00001f)
		{
			stringstream sstr33;
			std::string str33 = sstr33.str();
			sstr33 << "sdf " << v0->normal << " " << v1->normal << " " << v2->normal << "  " << info.normal ;
			logger.Box(sstr33.str());
			_asm int 3;
		}
		//_asm int 3;
		if(info.normal.GetLength() > 2)
		
		{
			stringstream sstr33;
			std::string str33 = sstr33.str();
			sstr33 << "sdf " << v0->normal << " " << v1->normal << " " << v2->normal;
			logger.Box(sstr33.str());
			//_asm int 3;
		}*/
    

	//info.position = v0->pos + E1*u + E2 * v;
	info.position = ray.origin + ray.direction*(t-0.0001f);
	info.texpos.x = u;
	info.texpos.y = v;
	info.material = material;

	return true;
}

/*
bool Triangle::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info)
{
	float u, v, t;
	Vector3d D;

	D.x = ray.direction.x;
	D.y = ray.direction.y;
	D.z = ray.direction.z;

	Vector3d E1 = v1.pos-v0.pos;
	Vector3d E2 = v2.pos-v0.pos;
	Vector3d T = ray.origin - v0.pos;

	Vector3d P = E2^T;
	Vector3d Q = E1^D;

	float det = E2*Q;
	if(det < 0.00001f && det > -0.00001f) // Ray in (almost) the same plane as the triangle
		return false;

	u = D*P/det;

	if(u > 1 || u < 0)
		return false;

	v = T*Q/det;

	if(u+v > 1 || u < 0 || v < 0)
		return false;

	t = E1*P/det;
	if(t < 0.00001f)
		return false;

	/*if(material && material->normalmap)
	{
		Vector2d tex1(u * (v1.texpos.x - v0.texpos.x),
			          u * (v1.texpos.y - v0.texpos.y));
		Vector2d tex2(v * (v2.texpos.x - v0.texpos.x),
			          v * (v2.texpos.y - v0.texpos.y));

		Vector2d txc = Vector2d(v0.texpos.x, v0.texpos.y) + tex1+tex2;
		// If there's a problem with normal mapping not looking right, try normalizing this vector.
		Color nc = material->normalmap->GetTexelBLInterp(txc.x, txc.y);
		float nx = (nc.r - 0.5f)*2.0f;
		float ny = (nc.g - 0.5f)*2.0f;
		float nz = nc.b;

		info.normal = (*tangent)*nx + (*binormal)*ny + (*normal)*nz;
	}*/
	//else
//	{
	/*	info.normal = E1^E2;
	//}

	info.normal.Normalize();
	info.position = v0.pos + E1*u + E2 * v;
	info.texpos.x = u;
	info.texpos.y = v;

	return true;
}*/
/*
int Triangle::GetType()
{
	return type_triangle;
}*/
/*
Color Triangle::GetTextureColor(float u, float v) const
{
	assert(material);
	Vector2d tv0 = (v1.texpos - v0.texpos) * u;
	Vector2d tv1 = (v2.texpos - v0.texpos) * v;
	Vector2d final = v0.texpos + tv0+tv1;
	return material->texture->GetTexel((int)final.x, (int)final.y);
}*/

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
/*
void Triangle::ComputeTangentSpaceVectors()
{
	Vector3d Q1 = v1.pos-v0.pos;
	Vector3d Q2 = v2.pos-v0.pos;

	float s1 = v1.normal.x - v0.normal.x;
	float s2 = v2.normal.x - v0.normal.x;

	float t1 = v1.normal.y - v0.normal.y;
	float t2 = v2.normal.y - v0.normal.y;

	float det = 1.0f/(s1*t2-t1*s2);

	float tx, ty, tz, bx, by, bz;

	tx = t2*Q1.x - t1*Q2.x;
	ty = t2*Q1.y - t1*Q2.y;
	tz = t2*Q1.z - t1*Q2.z;

	bx = s1*Q2.x - s2*Q1.x;
	by = s1*Q2.y - s2*Q1.y;
	bz = s1*Q2.z - s2*Q1.z;

	tangent = new Vector3d(tx, ty, tz);
	tangent->Normalize();
	binormal = new Vector3d(bx, by, bz);
	binormal->Normalize();
	normal = new Vector3d((*binormal)^(*tangent));
	normal->Normalize();
}*/

bool Triangle::GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox) const
{
	resultbox = GetBoundingBox();
	return true;
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