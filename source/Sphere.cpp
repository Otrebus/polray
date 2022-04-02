#include "Sphere.h"
#include "Material.h"
#include "BoundingBox.h"
#include "Scene.h"

Sphere::Sphere(const Vector3d& v, float r) : position(v), radius(r)
{
}

Sphere::Sphere(const Vector3d& pos, const Vector3d& u, const Vector3d& ri, float r) : position(pos), up(u), right(ri), radius(r)
{
	up.Normalize();
	right = (up^right)^up;
	right.Normalize();
}

Sphere::Sphere()
{
    radius = 1.0f;
    position = Vector3d(0.0f, 0.0f, 0.0f);
    material = 0;
}

Sphere::~Sphere()
{
}

float Sphere::GetRadius() const
{
	return radius;
}

void Sphere::SetRadius(float r)
{
	radius = r;
}

Vector3d Sphere::GetPosition() const
{
	return position;
}

float Sphere::Intersect(const Ray& ray) const
{
		float t;
		Vector3d dir(ray.direction);
		Vector3d vec = ray.origin - position;

		float C = vec*vec - radius*radius;
		float B = 2*(vec*dir);
		float A = dir*dir;

		float D = (B*B/(4*A) - C)/A;

		t = -B/(2*A) - sqrt(D);
			
		if(D > 0)
		{
			if(t < 0.00001f)
			{
				return -B/(2*A) + sqrt(D) > 0 ? t = -B/(2*A) + sqrt(D) : -1.0f;
			}
			return t;
		}
		return -1.0f;
}

bool Sphere::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
{
	float t;
	Vector3d dir(ray.direction);
	Vector3d vec = ray.origin - position;

	info.direction = ray.direction;

	info.material = 0;

	float C = vec*vec - radius*radius;
	float B = 2*(vec*dir);
	float A = dir*dir;

	float D = (B*B/(4*A) - C)/A;

	t = -B/(2*A) - sqrt(D);

	if(D >= 0)
	{
		if(t < 0.00001f)
		{
			t = -B/(2*A) + sqrt(D);
			if(t < 0)
				return false;
		}
		info.normal = (ray.origin + ray.direction*t) - position;
		info.normal.Normalize();
		info.position = ray.origin + ray.direction*(t - 0.0001f);

		// Texture coordinates - there are probably better methods than this one
		Vector3d v = info.position - position;
		Vector3d w = (up^v)^up;
		Vector3d forward = up^right;
		v.Normalize();
		w.Normalize();
		forward.Normalize();

		float vcoord = acosf(up*v) / 3.14159265f;
		float ucoord;

		// Clamp the coordinates to prevent NaNs, which is one of the reasons this method is inferior
		float wright = w*right > 1 ? 1 : w*right < -1 ? -1 : w*right;

		if(w*forward >= 0)
			ucoord = acos(wright) / (2.0f*3.14159265f);
		else
			ucoord = 1.0f - acos(wright) / (2.0f*3.14159265f);
		
		info.texpos.x = ucoord;
		info.texpos.y = vcoord;

		info.material = material;

/*		if(material && material->normalmap)
		{
			Vector3d normal = v;
			Vector3d tangent = up^normal;
			Vector3d binormal = normal^tangent;
			normal.Normalize();
			binormal.Normalize();
			tangent.Normalize();

			Color nc = bl->GetTexelBLInterp(ucoord*(bl->GetWidth()-1), vcoord*(bl->GetHeight()-1));

			//nc.Normalize();
			float nx = (nc.r - 0.5f)*2.0f;
			float ny = (nc.g - 0.5f)*2.0f;
			float nz = nc.b;

			info.normal = tangent*nx + binormal*ny + normal*nz;
			info.normal.Normalize();
			//v.Normalize();
			//info.normal = v;
		}*/
		info.geometricnormal = info.normal;
		return true;
	}
	return false;
}

/*int Sphere::GetType()
{
	return type_sphere;
}

Color Sphere::GetTextureColor(float u, float v) const
{
	return 0;
}*/

BoundingBox Sphere::GetBoundingBox() const
{
	Vector3d c1 = Vector3d(position.x-radius, position.y-radius, position.z-radius);
	Vector3d c2 = Vector3d(position.x+radius, position.y+radius, position.z+radius);
	return BoundingBox(c1, c2);
}
/*
void Sphere::ComputeTangentSpaceVectors()
{
	// We already have these so no computation needed
}*/

bool Sphere::GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox) const
{
	// Just return the unclipped box for now
	resultbox = GetBoundingBox();
	return true;
}

void Sphere::AddToScene(Scene& scene)
{
	Scene::PrimitiveAdder::AddPrimitive(scene, this);
    Scene::MaterialAdder::AddMaterial(scene, material);
}

void Sphere::Save(Bytestream& stream) const
{
    stream << (unsigned char)ID_SPHERE << position << radius;
    material->Save(stream);
}

void Sphere::Load(Bytestream& stream)
{
    unsigned char matId;
    stream >> position >> radius;
    stream >> matId;
    material = Material::Create(matId);
    material->Load(stream);
}