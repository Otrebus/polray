#include "TriangleMesh.h"
//#include "triboxoverlap.h"
#include <stack>
#include <map>
#include "PhongMaterial.h"
#include "EmissiveMaterial.h"
#include "PhongMaterial.h"
#include "LambertianMaterial.h"
#include "MirrorMaterial.h"
#include "DielectricMaterial.h"
#include "AshikhminShirley.h"
#include "Scene.h"
#include <filesystem>

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new DEBUG_NEW

MeshTriangle::MeshTriangle(const Vector3d& a, const Vector3d& b, const Vector3d& c)
{
	v0 = new MeshVertex(a);
	v1 = new MeshVertex(b);
	v2 = new MeshVertex(c);
	v0->normal = v1->normal = v2->normal = (v1->pos-v0->pos)^(v2->pos-v0->pos);
}

Vector3d MeshTriangle::GetNormal() const
{
	Vector3d normal = (v1->pos-v0->pos)^(v2->pos-v0->pos);
	normal.Normalize();
	return normal;
}

float MeshTriangle::GetArea()
{
	return fabsf(((v1->pos-v0->pos)^(v2->pos-v0->pos)).GetLength())/2;
}

MeshVertex::MeshVertex(float a, float b, float c)
{
	pos.x = a;
	pos.y = b;
	pos.z = c;
    replacement = 0;
}

MeshVertex::MeshVertex(const Vector3d& vec)
{
	pos.x = vec.x;
	pos.y = vec.y;
	pos.z = vec.z;
    replacement = 0;
}


MeshTriangle::MeshTriangle(MeshVertex* _v0, MeshVertex* _v1, MeshVertex* _v2)// : mesh(t)
{
	v0 = _v0;
	v1 = _v1;
	v2 = _v2;
}
/*
MeshTriangle::MeshTriangle(TriangleMesh* t, MeshVertex* _v0, MeshVertex* _v1, MeshVertex* _v2, Material* mat)// : mesh(t)
{
//		id = highestid++;
	v0 = _v0;
	v1 = _v1;
	v2 = _v2;
	material = mat;
}*/

MeshTriangle::MeshTriangle()
{
//		id = highestid++;
}

MeshTriangle::~MeshTriangle()
{
}
/*
MeshTriangle::MeshTriangle(Material* mat)
{
	material = mat;
}*/

float MeshTriangle::Intersect(const Ray& ray) const
{
	float u, v, t;
	Vector3d D;
	D.x = ray.direction.x;
	D.y = ray.direction.y;
	D.z = ray.direction.z;

	// THIS IS HORRIBLE FOR MULTITHREADING
	//numintersects++;

	Vector3d E1 = v1->pos-v0->pos;
	Vector3d E2 = v2->pos-v0->pos;
	Vector3d T = ray.origin - v0->pos;

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
/*
bool MeshTriangle::Intersect(const BoundingBox& box)
{
	float boxhalfsize[3] = {(box.c2.x - box.c1.x)/2.0f, (box.c2.y - box.c1.y)/2.0f, (box.c2.z - box.c1.z)/2.0f};
	float boxcenter[3] = {box.c2.x - boxhalfsize[0], box.c2.y - boxhalfsize[1], box.c2.z - boxhalfsize[2]};
	float triverts[3][3] = {{v0->pos.x, v0->pos.y, v0->pos.z}, {v1->pos.x, v1->pos.y, v1->pos.z}, {v2->pos.x, v2->pos.y, v2->pos.z}};
	return triBoxOverlap(boxcenter, boxhalfsize, triverts) == 1 ? true : 0;
}*/
	

	

/*float MeshTriangle::Intersect(const Ray& ray)
{
	float u, v, t;
	float Dx, Dy, Dz;
	Dx = ray.direction.x;
	Dy = ray.direction.y;
	Dz = ray.direction.z;

	float E1x, E1y, E1z;
	float E2x, E2y, E2z;
	float Tx, Ty, Tz;
	float Px, Py, Pz;
	float Qx, Qy, Qz;

	E1x = v1->pos.x - v0->pos.x;
	E1y = v1->pos.y - v0->pos.y;
	E1z = v1->pos.z - v0->pos.z;

	E2x = v2->pos.x - v0->pos.x;
	E2y = v2->pos.y - v0->pos.y;
	E2z = v2->pos.z - v0->pos.z;

	Tx = ray.origin.x - v0->pos.x;
	Ty = ray.origin.y - v0->pos.y;
	Tz = ray.origin.z - v0->pos.z;


	

	//Vector3d E1 = v1->pos-v0->pos;
	//Vector3d E2 = v2->pos-v0->pos;
	//Vector3d T = ray.origin - v0->pos;
// y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x
	Px = E2y*Tz -E2z*Ty;
	Py = E2z*Tx -E2x*Tz;
	Pz = E2x*Ty-E2y*Tx;

	Qx = E1y*Dz -E1z*Dy;
	Qy = E1z*Dx -E1x*Dz;
	Qz = E1x*Dy-E1y*Dx;


	//Vector3d P = E2^T;
	//Vector3d Q = E1^D;

	float det;
	det = E2x*Qx + E2y*Qy + E2z * Qz;

	//float det = E2*Q;
	if(det < 0.00001f && det > -0.00001f) // Ray in (almost) the same plane as the triangle
		return -1.0f;

	u = Dx*Px + Dy*Py + Dz*Pz;
	u/= det;
	//u = D*P/det;

	if(u > 1 || u < 0)
		return -1.0f;

	v = Tx*Qx + Ty*Qy + Tz*Qz;
	v/= det;

	//v = T*Q/det;

	if(u+v > 1 || u < 0 || v < 0)
		return -1.0f;

	t = E1x*Px + E1y*Py + E1z*Pz;
	t/= det;
	//t = E1*P/det;
	if(t < 0.00001f)
		return -1.0f;

	return t;
}*/


bool MeshTriangle::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info) const
{
	float u, v, t;
	Vector3d D;

	info.direction = ray.direction;

	D.x = ray.direction.x;
	D.y = ray.direction.y;
	D.z = ray.direction.z;

	Vector3d E1 = v1->pos-v0->pos;
	Vector3d E2 = v2->pos-v0->pos;
	Vector3d T = ray.origin - v0->pos;

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
		Vector3d n0 = v0->normal;
		Vector3d n1 = v1->normal;
		Vector3d n2 = v2->normal;

		info.normal = u*(v1->normal-v0->normal) + v*(v2->normal-v0->normal) + v0->normal;
	}

	info.geometricnormal = GetNormal();
	info.normal.Normalize();

	info.position = v0->pos + u*E1 + v*E2 + (info.geometricnormal*info.direction < 0 ? info.geometricnormal*0.0001f : -info.geometricnormal*0.0001f);
	info.texpos.x = u;
	info.texpos.y = v;
	info.material = material;

	return true;
}

TriangleMesh::TriangleMesh(string file, Material* mat)
{
	normalsuptodate = false;
	logger.File("Reading from file");
	ReadFromFile(file, mat);
}

bool TriangleMesh::ReadMaterialFile(string matfilestr, map<string, Material*>& materials)
{
	ifstream matfile;

	matfile.open(matfilestr.c_str(), ios::out);

	if(matfile.fail())
	{
		matfile.close();
		return false;
	}

	Material* curmat = 0;
	PhongMaterial* phongmat = 0;
	bool phong = false;
	while(!matfile.eof())
	{
		string line, a;
		getline(matfile, line);
		stringstream ss(line); // Read one line at a time into a stringstream
		ss >> a;               // and parse it

		transform(a.begin(), a.end(), a.begin(), [](unsigned char c) { return std::tolower(c); });

		if(a.length() && a[0] == '#')
			continue;
		if(a == "newmtl")
		{
			string b;
			ss >> a;
			if(!ss)
				__debugbreak();
			ss >> b;
			if(b == ":") // We expect a special material definition to follow
			{
				stringstream arg;
				ss >> b;
				if(!ss)
					__debugbreak();
				transform(b.begin(), b.end(), b.begin(), [](unsigned char c) { return std::tolower(c); });
				if(b == "emissive")
					curmat = new EmissiveMaterial;
				else if(b == "lambertian")
					curmat = new LambertianMaterial;
				else if(b == "mirror")
					curmat = new MirrorMaterial;
				else if(b == "phong")
					curmat = new PhongMaterial;
                else if(b == "dielectric")
                    curmat = new DielectricMaterial;
                else if(b == "ashikhminshirley")
                    curmat = new AshikhminShirley;
				while(!matfile.eof())
				{
					string line, s;
					getline(matfile, line);
					stringstream ss(line);
					ss >> s;
					if(s == "{")
						break;
					else if(s == "")
						continue;
					else
						__debugbreak();
				}
				while(!matfile.eof())
				{
					string line, s;
                    //matfile.getline(blah, s);
                    getline(matfile, s);
					//char blah[10000];
					//matfile.getline(blah, 10000);
					//s = blah;
					s+="\n";
					if(s[0] == '}')
						break;
					arg << s;
				}
				materials[a] = curmat;
				curmat->ReadProperties(arg);
			}
			else
			{
				curmat = new PhongMaterial;
				materials[a] = curmat;
				phong = true;
			}
		}
		
		else if(a == "ka")
		{
			if(!curmat)
				__debugbreak();
			if(phong)
			{
				phongmat = (PhongMaterial*) curmat;
				ss >> phongmat->Ka.r >> phongmat->Ka.g >> phongmat->Ka.b;
			}
		}
		else if(a == "kd")
		{
			if(!curmat)
				__debugbreak();
			if(phong)
			{
				phongmat = (PhongMaterial*) curmat;
				ss >> phongmat->Kd.r >> phongmat->Kd.g >> phongmat->Kd.b;
			}
			else
				__debugbreak();
		}
		else if(a == "ks")
		{
			if(!curmat)
				__debugbreak();
			if(phong)
			{
				phongmat = (PhongMaterial*) curmat;
				ss >> phongmat->Ks.r >> phongmat->Ks.g >> phongmat->Ks.b;
			}
			else
				__debugbreak();
		}
		else if(a == "ns")
		{
			if(!curmat)
				__debugbreak();
			if(phong)
			{
				phongmat = (PhongMaterial*) curmat;
				ss >> phongmat->alpha;
			}
			else
				__debugbreak();
		}
	}
	return true;
}

bool TriangleMesh::ReadFromFile(string file, Material* meshMat)
{
	Material* curmat = 0;
	map<string, Material*> materials;
	ifstream myfile;
	string line;
	myfile.open(file.c_str(), ios::out);
	char tmp[256];

	auto s = std::filesystem::current_path();
	cout << "Current working directory: " << tmp << endl;
	if(myfile.fail())
	{
		myfile.close();
		__debugbreak();
		return false;
	}

	string a;
	float x, y, z;
	int v0, v1, v2;
	int t0, t1, t2;
	int n0, n1, n2;

	t0 = t1 = t2 = n0 = n1 = n2 = 0;

	char slash;
	int index = 0;
	bool b = false;
	bool normalInterp = true;
	vector<MeshVertex*> vectors;
	vector<Vector3d> normals;

	map<int, MeshVertex*> groupVertices; // The vertices that have been added to the current group so far

	while(!myfile.eof())
	{
		istringstream ins;
		getline(myfile, line);
		ins.str(line);
        a = "";
		ins >> a;
		if(ins.fail() && !myfile.eof())
		{
			ins.clear();
			continue;
		}
        if(a == "f")
		{
			string s1, s2, s3;
			string number;

			ins >> s1 >> s2 >> s3;
			stringstream ss(s1);

			ss >> v0;
			if(ss.fail())
				__debugbreak();
			ss >> slash;
			if(!ss.fail())
			{
				// We (possibly) have some texture coordinates
				if(slash != '/')
					__debugbreak();
				ss >> t0;
				if(ss.fail())
					// No texture coordinate
					ss.clear();
			}
			else
				ss.clear();

			ss >> slash;
			if(!ss.fail())
			{
				if(slash != '/')
					__debugbreak();
				// We have a normal
				normalsuptodate = true;
				ss >> n0;
				if(ss.fail())
					__debugbreak();
			}
			else
				ss.clear();

			ss.clear();
			ss.str(s2);
			ss >> v1;
			if(ss.fail())
				__debugbreak();
			ss >> slash;
			if(!ss.fail())
			{
				// We (possibly) have some texture coordinates
				if(slash != '/')
					__debugbreak();
				ss >> t1;
				if(ss.fail())
					// No texture coordinate
					ss.clear();
			}
			else
				ss.clear();

			ss >> slash;
			if(!ss.fail())
			{
				if(slash != '/')
					__debugbreak();
				// We have a normal
				normalsuptodate = true;
				ss >> n1;
				if(ss.fail())
					__debugbreak();
			}
			else
				ss.clear();

			ss.clear();
			ss.str(s3);
			ss >> v2;
			if(ss.fail())
				__debugbreak();
			ss >> slash;
			if(!ss.fail())
			{
				// We (possibly) have some texture coordinates
				if(slash != '/')
					__debugbreak();
				ss >> t2;
				if(ss.fail())
					// No texture coordinate
					ss.clear();
			}
			else
				ss.clear();

			ss >> slash;
			if(!ss.fail())
			{
				if(slash != '/')
					__debugbreak();
				// We have a normal
				normalsuptodate = true;
				ss >> n2;
				if(ss.fail())
					__debugbreak();
			}
			else
				ss.clear();

    		MeshVertex* pv0, *pv1, *pv2;

            auto it = groupVertices.find(v0-1);
			if(it == groupVertices.end()) // We have not seen this vertex before in this group
			{                             // so create a new one
				pv0 = groupVertices[v0-1] = new MeshVertex(*vectors[v0-1]);
                if(n0)
			    {
				    normalInterp = false; // A normal was submitted so let's trust that one in accordance with .obj standards
					pv0->normal = normals[n0-1];
                }
                points.push_back(pv0);
			}
			else
			{ // This vertex is already among the parsed vertices in this group so use that particular one
				pv0 = (*it).second;
                if(n0)
                {
                    normalInterp = false;
                    if(pv0->normal != normals[n0-1])  // A different normal was given though, so we still need
                    {                                 // to create an entirely new vertex
					    pv0 = new MeshVertex(*pv0); 
                        points.push_back(pv0);
                    }
                    pv0->normal = normals[n0-1];
                }  
			}

	    	it = groupVertices.find(v1-1);
            if(it == groupVertices.end())
			{
				pv1 = groupVertices[v1-1] = new MeshVertex(*vectors[v1-1]);
                if(n1)
			    {
				    normalInterp = false;
					pv1->normal = normals[n1-1];
                }
                points.push_back(pv1);
			}
			else
			{
				pv1 = (*it).second;
                if(n1)
                {
                    normalInterp = false;
                    if(pv1->normal != normals[n1-1])
                    {
					    pv1 = new MeshVertex(*pv1); 
                        points.push_back(pv1);
                    }
                    pv1->normal = normals[n1-1];
                }  
			}

	    	it = groupVertices.find(v2-1);
            if(it == groupVertices.end()) // We have not seen this vertex before in this group
			{                             // so create a new one
				pv2 = groupVertices[v2-1] = new MeshVertex(*vectors[v2-1]);
                if(n2)
			    {
				    normalInterp = false; // A normal was submitted so let's trust that one in accordance with .obj standards
					pv2->normal = normals[n2-1];
                }
                points.push_back(pv2);
            }
			else
			{ // This vertex is already among the parsed vertices in this group so use that particular one
				pv2 = (*it).second;
                if(n2)
                {
                    normalInterp = false;
                    if(pv2->normal != normals[n2-1])  // A different normal was given though, so we still need
                    {                                 // to create an entirely new vertex
					    pv2 = new MeshVertex(*pv2); 
                        points.push_back(pv2);
                    }
                    pv2->normal = normals[n2-1];
                }  
			}

			MeshTriangle* tri = new MeshTriangle(pv0, pv1, pv2);
			triangles.push_back(tri);

            pv0->triangles.push_back(tri);
            pv1->triangles.push_back(tri);
			pv2->triangles.push_back(tri);

			if((n0 || n1 || n2) && !(n0 && n1 && n2))
				__debugbreak();

			tri->v0 = pv0;
			tri->v1 = pv1;
			tri->v2 = pv2;

			if(!n0)
			{
				tri->v0->normal = tri->GetNormal();
				tri->v1->normal = tri->v0->normal;
				tri->v2->normal = tri->v0->normal;
				
				if(!tri->v0->normal.IsNull())
					tri->v0->normal.Normalize();
				if(!tri->v1->normal.IsNull())   
					tri->v1->normal.Normalize();
				if(!tri->v2->normal.IsNull())
					tri->v2->normal.Normalize();
			}

			// No material defined
			if(!curmat)
			{
                //PhongMaterial* mat = new PhongMaterial();
	            //mat->Kd = Color(0, 0, 0.8f);
	            //mat->Ks = Color(0.05f, 0.05f, 0.05f);
                //tri->material = mat;
                DielectricMaterial* mat = new DielectricMaterial();
                //MirrorMaterial* mat = new MirrorMaterial();
                mat->m_ior = 1.5f;
                tri->SetMaterial(mat);
				//PhongMaterial* mat = new PhongMaterial();
				
				//mat->Ka = Color(0.0f, 0.0f, 0.0f);
				//mat->Ks = Color(0.0f, 0.0f, 0.0f);
				//mat->Kd = Color(0.7f, 0.7f, 0.7f);
				//mat->alpha = 0;
				//tri->material = mat;
			}
			if(curmat && !meshMat)
				tri->SetMaterial(curmat);
            else if(meshMat)
                tri->SetMaterial(meshMat);

		//	triangles.push_back(tri);
		}
        if(a == "g" || myfile.eof())
		{   
			map<int, MeshVertex*>::iterator it;
			stack<MeshVertex*> vs;

			for(it = groupVertices.begin(); it != groupVertices.end(); it++)
			{
				vs.push((*it).second);
			}

			// Create duplicate vertices for every vertex that is part of 
			// triangles that are above a certain angle threshold to each other
			if(normalInterp)
			{
				while(!vs.empty())
				{
					MeshVertex* v = vs.top();
					vs.pop();
		
					// Check if we have two triangles at a large angle sharing this vertex
					for(vector<MeshTriangle*>::iterator it = v->triangles.begin(); it < v->triangles.end(); it++)
					{
						MeshTriangle* t1 = *it;
						for(vector<MeshTriangle*>::iterator it2 = it;it2 < v->triangles.end(); it2++)
						{
							MeshTriangle* t2 = *it2;
							if(t1->GetNormal() * t2->GetNormal() < 0.7f)
							{
								// Create two new vertices
								//MeshVertex* v1 = new MeshVertex(*v);
								//v1->triangles.clear();
                                MeshVertex*& v1 = v;
                                MeshVertex* vold(v);
                                v1->triangles.clear();
								MeshVertex* v2 = new MeshVertex(*v);
                                points.push_back(v2);
								v2->triangles.clear();
								v1->normal = t1->GetNormal(); // Just the geometric normal for now, maybe averaged is better
								v2->normal = t2->GetNormal();
								// Reassign the triangles that belonged to v to either v1 or v2 depending on their normal
								for(vector<MeshTriangle*>::iterator it3 = vold->triangles.begin(); it3 < vold->triangles.end(); it3++)
								{
									MeshTriangle* t3 = *it3;
									if(t3->GetNormal() * v1->normal >= 0.7f)
									{
										// First reassign the correct vector in the triangle
										if(t3->v0 == vold)
											t3->v0 = v1;
										if(t3->v1 == vold)
											t3->v1 = v1;
										if(t3->v2 == vold)
											t3->v2 = v1;
										// Then reassign the triangle in the vector's triangle list
										v1->triangles.push_back(t3);
									}
									else
									{
										if(t3->v0 == vold)
											t3->v0 = v2;
										if(t3->v1 == vold)
											t3->v1 = v2;
										if(t3->v2 == vold)
											t3->v2 = v2;
										v2->triangles.push_back(t3);
									}
								}
								vs.push(v1);
								vs.push(v2);
								goto nextwhile; // Yes, it's goto, since nested continues don't exist
							}
						}
					}
			nextwhile:;
				} // while(!vs.empty() ..
			} // if(normalInterp ..

			normalInterp = true;
			groupVertices.clear();
		} // if(a == "g" ..
        if(myfile.eof())
            break;
		if(a == "mtllib")
		{
			// Check if there's an associated materials file, and parse it
			ifstream matfile;
			string matfilestr;
			ins >> matfilestr;

			if(ins.fail())
				__debugbreak();

//			if(!ReadMaterialFile(matfilestr, materials))
//				__debugbreak();
            if(!meshMat)
			    ReadMaterialFile(matfilestr, materials);

			continue;
		}
			
		if(a == "#")
			continue;

		if(a == "v")
		{
			ins >> x >> y >> z;
			vectors.push_back(new MeshVertex(x, y, z));
			index++;
		}
		if(a == "s") // Smoothing group ending or starting
		{
			string s1;
			ins >> s1;
			if(s1 == "off" || s1 == "0")
				normalInterp = false;
			else
				normalInterp = true;
		}

		if(a == "vn")
		{
		    ins >> x >> y >> z;
			Vector3d normal = Vector3d(x, y, z);
			normals.push_back(normal);
		}
		if(a == "usemtl")
		{
			string mtl;
			ins >> mtl;
			map<string, Material*>::iterator it = materials.begin();
			it = materials.find(mtl);
			if(it == materials.end())
				curmat = 0;
			else
				curmat = materials[mtl];
		}
	}

	//vector<Shape*> shapes;

	for(vector<MeshTriangle*>::iterator it = triangles.begin(); it < triangles.end(); it++)
	{
        MeshTriangle* t = (*it);

        MeshVertex* t0v = (MeshVertex*)(t->v0);
        MeshVertex* t1v = (MeshVertex*)(t->v1);
        MeshVertex* t2v = (MeshVertex*)(t->v2);
        
        if(!t0v->replacement)
        {
            Vertex3d* newVertex = new Vertex3d(t0v->pos, t0v->normal, t0v->texpos);
            t0v->replacement = newVertex;
            t->v0 = newVertex;
        }
        else
            t->v0 = t0v->replacement;

        if(!t1v->replacement)
        {
            Vertex3d* newVertex = new Vertex3d(t1v->pos, t1v->normal, t1v->texpos);
            t1v->replacement = newVertex;
            t->v1 = newVertex;
        }
        else
            t->v1 = t1v->replacement;
        
        if(!t2v->replacement)
        {
            Vertex3d* newVertex = new Vertex3d(t2v->pos, t2v->normal, t2v->texpos);
            t2v->replacement = newVertex;
            t->v2 = newVertex;        
        }
        else
            t->v2 = t2v->replacement;

	//	shapes.push_back((Shape*)*it);
	}

    for(auto it = points.begin(); it < points.end(); it++)
    {
        if(((*it)->replacement))
            *it = (MeshVertex*)((*it)->replacement);
        else 
            (*it) = (MeshVertex*) (new Vertex3d((*it)->pos, (*it)->normal, (*it)->texpos));
    }



	//points = vectors;
	
	//vectors.clear();

    for(auto it = materials.begin(); it != materials.end(); it++)
    {
        this->materials.push_back((*it).second);
    }
	
	myfile.close();
	return true;
}

TriangleMesh::TriangleMesh()
{
}
/*
int TriangleMesh::GetType()
{
	return type_trianglemesh;
}

void MeshTriangle::ComputeTangentSpaceVectors()
{
}*/

BoundingBox MeshTriangle::GetBoundingBox() const
{
//	return bbox;
	Vector3d c1, c2;
	c1.x = min(v0->pos.x, min(v1->pos.x, v2->pos.x));
	c1.y = min(v0->pos.y, min(v1->pos.y, v2->pos.y));
	c1.z = min(v0->pos.z, min(v1->pos.z, v2->pos.z));

	c2.x = max(v0->pos.x, max(v1->pos.x, v2->pos.x));
	c2.y = max(v0->pos.y, max(v1->pos.y, v2->pos.y));
	c2.z = max(v0->pos.z, max(v1->pos.z, v2->pos.z));
	return BoundingBox(c1, c2);
}

/*
BoundingBox TriangleMesh::GetBoundingBox()
{
	if(boundingboxuptodate)
		return bbox;
	float maxz = -10e30f;
	float maxy = -10e30f;
	float maxx = -10e30f;
	float minz = 10e30f;
	float miny = 10e30f;
	float minx = 10e30f;

	vector<MeshTriangle*> blah = triangles;

	for(vector<MeshTriangle*>::iterator it = blah.begin(); it < blah.end(); it++)
	{
		MeshTriangle tri = **it;
		BoundingBox box = tri.GetBoundingBox();
		if(box.c2.z > maxz)
			maxz = box.c2.z;
		if(box.c2.y > maxy)
			maxy = box.c2.y;
		if(box.c2.x > maxx)
			maxx = box.c2.x;
		if(box.c1.z < minz)
			minz = box.c1.z;
		if(box.c1.y < miny)
			miny = box.c1.y;
		if(box.c1.x < minx)
			minx = box.c1.x;
	}

	boundingboxuptodate = true;
	bbox = BoundingBox(Vector3d(minx, miny, minz), Vector3d(maxx, maxy, maxz));
	return bbox;
}

int MeshTriangle::GetType()
{
	return type_meshtriangle;
}
*/
TriangleMesh::~TriangleMesh()
{
}
/*
float TriangleMesh::Intersect(const Ray& ray)
{
	Shape* dummy = 0;
    return -1.0f;
//	return tree.Intersect(Ray(ray), dummy);
}

bool TriangleMesh::GenerateIntersectionInfo(const Ray& ray, IntersectionInfo& info)
{
	Shape* shape = 0;
	/*if(tree.Intersect(Ray(ray), shape) > -1.0f)
	{
		return shape->GenerateIntersectionInfo(Ray(ray), info);
	}*//*
	return false;
}*/

void TriangleMesh::CalculateVertexNormals()
{
	Vector3d totalnormal(0,0,0);
	Vector3d normal(0, 0, 0);

	for(vector<MeshVertex*>::iterator it = points.begin(); it < points.end(); it++)
	{
		for(vector<MeshTriangle*>::iterator it2 = (*it)->triangles.begin(); it2 < (*it)->triangles.end(); it2++)
		{
			totalnormal += (*it2)->GetNormal();
		}
		totalnormal.Normalize();
		(*it)->normal = totalnormal;
		totalnormal = Vector3d(0,0,0);
	}
}
/*
void MeshTriangle::UpdateBoundingBox()
{
	Vector3d c1, c2;

	c1.x = min(v0->pos.x, min(v1->pos.x, v2->pos.x));
	c1.y = min(v0->pos.y, min(v1->pos.y, v2->pos.y));
	c1.z = min(v0->pos.z, min(v1->pos.z, v2->pos.z));

	c2.x = max(v0->pos.x, max(v1->pos.x, v2->pos.x));
	c2.y = max(v0->pos.y, max(v1->pos.y, v2->pos.y));
	c2.z = max(v0->pos.z, max(v1->pos.z, v2->pos.z));

//	bbox = BoundingBox(c1, c2);
}*/

						
/*
void TriangleMesh::ComputeTangentSpaceVectors()
{
}*/

void TriangleMesh::RemoveDuplicateVertices(float threshold)
{
	vector<MeshVertex*> points2 = points;
	// This whole thing is O(n^2) and can be optimized through the use of a KD-Tree
	for(vector<MeshVertex*>::iterator it1 = points.begin(); it1 < points.end(); it1++)
	{
		for(vector<MeshVertex*>::iterator it2 = points2.begin(); it2 < points2.end(); it2++)
		{
			MeshVertex* v1 = *it1;
			MeshVertex* v2 = *it2;

			if(abs(v1->pos.x - v2->pos.x) < threshold && abs(v1->pos.y - v2->pos.y) < threshold &&
			   abs(v1->pos.z - v2->pos.z) < threshold && v1 != v2)
			{
				points2.erase(it2);
				for(vector<MeshTriangle*>::iterator it3 = v2->triangles.begin(); it3 < v2->triangles.end(); it3++)
				{
					MeshTriangle* tri = *it3;
					if(tri->v0 == v2)
					{
						tri->v0 = v1;
						v1->triangles.push_back(tri);
					}

					else if(tri->v1 == v2)
					{
						tri->v1 = v1;
						v1->triangles.push_back(tri);
					}
					else if(tri->v2 == v2)
					{
						tri->v2 = v1;
						v1->triangles.push_back(tri);
					}
				}
			}
		}
	}
	points = points2;
}

bool MeshTriangle::GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox) const
{
	const int positive = 1;
	const int negative = -1;
	vector<Vector3d> points;
	points.push_back(v0->pos);	
	points.push_back(v1->pos);
	points.push_back(v2->pos);

	ClipPolygonToAAP(0, positive, clipbox.c1.x, points); // Left side of the bounding box
	ClipPolygonToAAP(0, negative, clipbox.c2.x, points); // Right
	ClipPolygonToAAP(1, positive, clipbox.c1.y, points); // Bottom
	ClipPolygonToAAP(1, negative, clipbox.c2.y, points); // Top
	ClipPolygonToAAP(2, positive, clipbox.c1.z, points); // Front
	ClipPolygonToAAP(2, negative, clipbox.c2.z, points); // Back

	resultbox.c1.x = numeric_limits<float>::infinity();
	resultbox.c2.x = -numeric_limits<float>::infinity();
	resultbox.c1.y = numeric_limits<float>::infinity();
	resultbox.c2.y = -numeric_limits<float>::infinity();
	resultbox.c1.z = numeric_limits<float>::infinity();
	resultbox.c2.z = -numeric_limits<float>::infinity();

	for(vector<Vector3d>::iterator it = points.begin(); it < points.end(); it++)
	{
		Vector3d v = *it;
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

/*
bool TriangleMesh::GetClippedBoundingBox(const BoundingBox& clipbox, BoundingBox& resultbox)
{
	resultbox = GetBoundingBox();
	return true;
}*/

void TriangleMesh::AddToScene(Scene& scene)
{
	for(vector<MeshTriangle*>::iterator it = triangles.begin(); it < triangles.end(); it++)
		Scene::PrimitiveAdder::AddPrimitive(scene, *it);
}


void TriangleMesh::Transform(const Matrix3d& m)
{
    // This is the transpose of the inverse used to calculate the transforms of the normals
    Matrix3d nm(
        m(1,1)*m(2,2)-m(1,2)*m(2,1),m(1,2)*m(2,0)-m(1,0)*m(2,2),m(1,0)*m(2,1)-m(1,1)*m(2,0),0,
        m(0,2)*m(2,1)-m(0,1)*m(2,2),m(0,0)*m(2,2)-m(0,2)*m(2,0),m(0,1)*m(2,0)-m(0,0)*m(2,1),0,
        m(0,1)*m(1,2)-m(0,2)*m(1,1),m(0,2)*m(1,0)-m(0,0)*m(1,2),m(0,0)*m(1,1)-m(0,1)*m(1,0),0,
        0,0,0,1);
                int i = 0;
    for(auto it = points.begin(); it < points.end(); it++)
    {
        i++;
        Vertex3d* v = *it;
        v->pos = m*v->pos;
        v->normal = nm*v->normal;
        v->normal.Normalize();
    }
}

void TriangleMesh::Save(Bytestream& stream) const
{
    // This could also be a hash table, for very large triangle meshes
    map<Vertex3d*, unsigned int> vertexMemToIndex;
    map<Material*, unsigned int> materialMemToIndex;

    stream << (unsigned char)ID_TRIANGLEMESH; 
    stream << (unsigned int)materials.size();
    stream << (unsigned int)points.size();
    stream << (unsigned int)triangles.size();

    for(unsigned int i = 0; i < materials.size(); i++)
    {
        materials[i]->Save(stream);
        materialMemToIndex[materials[i]] = i;
    }
    for(unsigned int i = 0; i < points.size(); i++)
    {
        Vertex3d* v = points[i];
        stream << v->pos.x << v->pos.y << v->pos.z
               << v->normal.x << v->normal.y << v->normal.z
               << v->texpos.x << v->texpos.y;
        vertexMemToIndex[v] = i;
    }
    for(auto it = triangles.begin(); it < triangles.end(); it++)
    {
        unsigned int a, b, c;
        MeshTriangle* v = *it;
        //a = vertexMemToIndex[v->v0];
        //b = vertexMemToIndex[v->v1];
        //c = vertexMemToIndex[v->v2];
        //stream << a << b << c;
        stream << vertexMemToIndex[v->v0] 
               << vertexMemToIndex[v->v1] << vertexMemToIndex[v->v2];
        stream << materialMemToIndex[v->GetMaterial()];
    }
}

void TriangleMesh::Load(Bytestream& stream)
{
    unsigned int nMats, nPoints, nTriangles;
    stream >> nMats >> nPoints >> nTriangles;

    materials.clear(); points.clear(); triangles.clear();

    for(unsigned int i = 0; i < nMats; i++)
    {
        unsigned char id;
        Material* mat;
        stream >> id;
        mat = Material::Create(id);
        mat->Load(stream);
        materials.push_back(mat);
    }
    for(unsigned int i = 0; i < nPoints; i++)
    {
        Vertex3d* v = new Vertex3d;
        stream >> v->pos.x >> v->pos.y >> v->pos.z 
               >> v->normal.x >> v->normal.y >> v->normal.z 
               >> v->texpos.x >> v->texpos.y;
        points.push_back((MeshVertex*)v);
    }
    for(unsigned int i = 0; i < nTriangles; i++)
    {
        MeshTriangle* v = new MeshTriangle;
        unsigned int n1, n2, n3, m;
        stream >> n1 >> n2 >> n3;
        v->v0 = points[n1]; v->v1 = points[n2]; v->v2 = points[n3];
        stream >> m;
        v->SetMaterial(materials[m]);
        triangles.push_back(v);
    }
}