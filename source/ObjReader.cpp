#include "ObjReader.h"
#include "TriangleMesh.h"
#include "PhongMaterial.h"
#include "LambertianMaterial.h"
#include "MirrorMaterial.h"
#include "DielectricMaterial.h"
#include "AshikhminShirley.h"
#include "Utils.h"
#include <stack>
#include <tuple>
#include <set>

bool ReadMaterialFile(string matfilestr, map<string, Material*>& materials)
{
	ifstream matfile;

	matfile.open(matfilestr.c_str(), ios::out);

	if(matfile.fail())
	{
		matfile.close();
		return false;
	}

	TriangleMesh mesh;

	Material* curmat = 0;
	PhongMaterial* phongmat = 0;
	std::string matName;
	bool phong = false, emissive = false;
	while(!matfile.eof())
	{
		string line, a;
		getline(matfile, line);
		stringstream ss(line); // Read one line at a time into a stringstream
		ss >> a;               // and parse it

		transform(a.begin(), a.end(), a.begin(), [](char a) { return (char) tolower(a); });

		if(a.length() && a[0] == '#')
			continue;
		if(a == "newmtl")
		{
			string b;
			ss >> matName;
			if(!ss)
				__debugbreak();
			ss >> b;
			if(b == ":") // We expect a special material definition to follow
			{
				stringstream arg;
				ss >> b;
				if(!ss)
					__debugbreak();
				transform(b.begin(), b.end(), b.begin(), [](char b) { return (char) tolower(b); });
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
                    getline(matfile, s);
					s += "\n";
					if(s[0] == '}')
						break;
					arg << s;
				}
				materials[matName] = curmat;
				curmat->ReadProperties(arg);
			}
			else
			{
				curmat = new PhongMaterial;
				materials[matName] = curmat;
				phong = true;
			}
		}

		else if(a == "ka")
		{
			double dummy;
			ss >> dummy >> dummy >> dummy;
			if(!curmat)
				__debugbreak();
			if(phong)
			{
				phongmat = (PhongMaterial*) curmat;
				ss >> phongmat->Ka.r >> phongmat->Ka.g >> phongmat->Ka.b;
			} else if(!emissive)
				__debugbreak();
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
			else if(!emissive)
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
			} else if(!emissive)
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
			else if(!emissive)
				__debugbreak();
		}
		else if(a == "ke") 
		{
			Color intensity;
			ss >> intensity.r >> intensity.g >> intensity.b;
			if(intensity) {
				phong = false;
				emissive = true;
				delete curmat;
				MeshLight* ml = new MeshLight(intensity);
				curmat = ml->material;
				materials[matName] = curmat;
			}
		}
	}
	return true;
}

std::tuple<bool, TriangleMesh*, std::vector<MeshLight*>> ReadFromFile(std::string file, Material* meshMat)
{
	Material* curmat = 0;
	map<string, Material*> materials;
	ifstream myfile;
	string line;
	myfile.open(file.c_str(), ios::out);

	std::set<MeshLight*> meshLights;

	TriangleMesh* mesh = new TriangleMesh();
	TriangleMesh* currentMesh = mesh;

	if(myfile.fail())
	{
		myfile.close();
		__debugbreak();
		return { false, {}, {} };
	}

	string a;
	double x, y, z;
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

	bool isLightMesh = false;
	std::unordered_map<MeshVertex*, Vertex3d*> replacement;

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
			std::vector<MeshVertex*> face;
			std::vector<int> ns;
			std::vector<int> ts;

			string s1, s2, s3;

			while(!ins.eof()) {
				std::string st; // The "v/t/n" string
				ins >> st;
				if(!st.size())
					break;

				auto s = split(st, '/');
				int v = 0, t = 0, n = 0;

				if(s.size() == 1) { // No normals or coordinates
					v = std::stoi(s[0]);
				} else if(s.size() == 2) { // Texture coordinates only
					v = std::stoi(s[0]);
					t = std::stoi(s[1]);
				} else if(s.size() == 3 && s[1].size() == 0) { // Normals only
					v = std::stoi(s[0]);
					n = std::stoi(s[2]);
				} else { // Texture coordinates and normals
					v = std::stoi(s[0]);
					t = std::stoi(s[1]);
					n = std::stoi(s[2]);
				}

				MeshVertex* mv;

				if (v < 0)
					v = vectors.size() + v + 1;
				auto it = groupVertices.find(v - 1);
				if (it == groupVertices.end()) { // We have not seen this vertex before in this group so create a new one
					mv = groupVertices[v-1] = new MeshVertex(*vectors[v-1]);
					if(n) {
						normalInterp = false; // A normal was submitted so let's trust that one in accordance with .obj standards
						mv->normal = normals[n-1];
					}
					currentMesh->points.push_back(mv);
				} else { // This vertex is already among the parsed vertices in this group so use that particular one
					mv = it->second;
					if(n)
					{
						normalInterp = false;
						if(mv->normal != normals[n-1])  // A different normal was given though, so we still need
						{                                 // to create an entirely new vertex
							mv = new MeshVertex(*mv); 
							currentMesh->points.push_back(mv);
						}
						mv->normal = normals[n-1];
					}  
				}
				face.push_back(mv);
				ns.push_back(n);
				ts.push_back(t);
			}

			for(int i = 0; i < (int) face.size()-2; i++) {
				auto pv0 = face[0], pv1 = face[i+1], pv2 = face[i+2];

				MeshTriangle* tri = new MeshTriangle(pv0, pv1, pv2);
				
				currentMesh->triangles.push_back(tri);

				pv0->triangles.push_back(tri);
				pv1->triangles.push_back(tri);
				pv2->triangles.push_back(tri);

				tri->v0 = pv0;
				tri->v1 = pv1;
				tri->v2 = pv2;

				if (!ns[0]) {
					tri->v0->normal = tri->GetNormal();
					//if(tri->v0->normal.x != tri->v0->normal.x)
					//	__debugbreak();
					tri->v1->normal = tri->v0->normal;
					tri->v2->normal = tri->v0->normal;

					if (!tri->v0->normal.IsNull())
						tri->v0->normal.Normalize();
					if (!tri->v1->normal.IsNull())
						tri->v1->normal.Normalize();
					if (!tri->v2->normal.IsNull())
						tri->v2->normal.Normalize();
				}

				// No material defined, set to diffuse
				if (!curmat) {
					LambertianMaterial* mat = new LambertianMaterial();
					mat->Kd = Color(0.7f, 0.7f, 0.7f);
					tri->SetMaterial(mat);
					currentMesh->materials.push_back(mat);
				}
				if (curmat && !meshMat)
					tri->SetMaterial(curmat);
				else if (meshMat)
					tri->SetMaterial(meshMat);
			}
		}
        if(a == "g" || myfile.eof())
		{   
			map<int, MeshVertex*>::iterator it;
			std::stack<MeshVertex*> vs;

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
					for(auto& t1 : v->triangles)
					{
						for(auto& t2 : v->triangles)
						{
							if(t1->GetNormal() * t2->GetNormal() < 0.7f)
							{
								// Create two new vertices
								//MeshVertex* v1 = new MeshVertex(*v);
								//v1->triangles.clear();
                                MeshVertex*& v1 = v;
                                MeshVertex* vold(v);
                                v1->triangles.clear();
								MeshVertex* v2 = new MeshVertex(*v);
                                currentMesh->points.push_back(v2);
								v2->triangles.clear();
								v1->normal = t1->GetNormal(); // Just the geometric normal for now, maybe averaged is better
								v2->normal = t2->GetNormal();
								// Reassign the triangles that belonged to v to either v1 or v2 depending on their normal
								for(auto& t3 : vold->triangles)
								{
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
			auto it = materials.find(mtl);
			if(it == materials.end())
				curmat = 0;
			else {
				if(materials[mtl]->light) {
					isLightMesh = true;
					meshLights.emplace((MeshLight*) materials[mtl]->light);
					currentMesh = ((MeshLight*) materials[mtl]->light)->mesh;
				} else {
					isLightMesh = false;
					currentMesh = mesh;
				}
				curmat = materials[mtl];
			}
		}
	}

	for(auto& t : mesh->triangles) {
		for(auto vv : { &t->v0, &t->v1, &t->v2 }) {
			auto& v = (*((MeshVertex**) vv));
			if(replacement.find(v) == replacement.end()) {
				auto oldv = v;
				v = (MeshVertex*) new Vertex3d(v->pos, v->normal, v->texpos);
				replacement[oldv] = (Vertex3d*) v;
				mesh->points.push_back(v);
				replacement[v] = v;
			}
			else
				v = (MeshVertex*) replacement[v];
		}
	}
	for(auto& p : mesh->points)
		if(replacement.find((MeshVertex*)p) != replacement.end())
			p = replacement[(MeshVertex*)p];

	for(auto& m : meshLights) {
		for(auto& t : m->mesh->triangles) {
			for(auto vv : { &t->v0, &t->v1, &t->v2 }) {
				auto& v = (*((MeshVertex**) vv));
				if(replacement.find(v) == replacement.end()) {
					auto oldv = v;
					v = (MeshVertex*) new Vertex3d(v->pos, v->normal, v->texpos);
					replacement[oldv] = (Vertex3d*) v;
					m->mesh->points.push_back(v);
					replacement[v] = v;
				}
				else
					v = (MeshVertex*) replacement[v];
			}
		}
		for(auto& p : m->mesh->points)
			if(replacement.find((MeshVertex*)p) != replacement.end())
				p = replacement[(MeshVertex*)p];
	}

    for(auto it = materials.begin(); it != materials.end(); it++)
        mesh->materials.push_back((*it).second);

	myfile.close();
	auto meshLightVector = std::vector<MeshLight*>(meshLights.begin(), meshLights.end());
	return { true, mesh, meshLightVector };
}
