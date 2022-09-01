#include "ObjReader.h"
#include "TriangleMesh.h"
#include "PhongMaterial.h"
#include "LambertianMaterial.h"
#include "MeshLight.h"
#include "MirrorMaterial.h"
#include "EmissiveMaterial.h"
#include "DielectricMaterial.h"
#include "AshikhminShirley.h"
#include "Utils.h"
#include <stack>
#include <tuple>
#include <set>
#include <unordered_map>

bool ReadMaterialFile(std::string matfilestr, std::map<std::string, Material*>& materials)
{
    std::ifstream matfile;

    matfile.open(matfilestr.c_str(), std::ios::out);

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
        std::string line, a;
        std::getline(matfile, line);
        std::stringstream ss(line); // Read one line at a time into a stringstream
        ss >> a;               // and parse it

        std::transform(a.begin(), a.end(), a.begin(), [](char a) { return (char) tolower(a); });

        if(a.length() && a[0] == '#')
            continue;
        if(a == "newmtl")
        {
            std::string b;
            ss >> matName;
            if(!ss)
                __debugbreak();
            ss >> b;
            if(b == ":") // We expect a special material definition to follow
            {
                std::stringstream arg;
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
                    std::string line2, s;
                    std::getline(matfile, line2);
                    std::stringstream ss2(line2);
                    ss2 >> s;
                    if(s == "{")
                        break;
                    else if(s == "")
                        continue;
                    else
                        __debugbreak();
                }
                while(!matfile.eof())
                {
                    std::string s;
                    std::getline(matfile, s);
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

class Token
{
public:
    enum Type { String, Number, Operator, Newline, Eof };

    Token(Type t, std::string s="") : type(t), str(s)
    {
    }

    bool operator==(const Token& token)
    {
        return type == token.type && str == token.str;
    }

    bool operator!=(const Token& token)
    {
        return !(*this == token);
    }

    Type type;
    std::string str;
};


std::vector<Token> tokenize(std::string str)
{
    int p = 0;

    std::vector<Token> v;

    auto skipspace = [&p, &str] ()
    {
        while(p < str.length() && str[p] == ' ' || str[p] == '\t')
            p++;
    };

    auto peek = [&p, &str, &skipspace] () -> char
    {
        return p < str.size() ? str[p] : 0;
    };

    auto next = [&p, &str, &skipspace] () -> char
    {
        return p < str.size() ? str[p++] : 0;
    };

    auto accept = [&p, &str, &skipspace] (char c) -> bool
    {
        return (p < str.size() && str[p] == c) ? ++ p : false;
    };

    while(p < str.size())
    {
        skipspace();
        if(accept('/'))
            v.push_back(Token(Token::Operator, "/"));
        if(peek() == '-' || peek() == '.' || std::isdigit(peek()))
        {
            std::string s;
            while(std::isdigit(peek()) || peek() == '.' || peek() == 'e' || peek() == 'E' || peek() == '+' || peek() == '-')
                s += next();

            std::stringstream ss(s);
            double d;
            ss >> d;
            if(ss.fail())
            {
                // TODO: fail the tokenizer somehow
                __debugbreak();
            }
            v.push_back(Token(Token::Number, s));
        }
        if(std::isalpha(peek()))
        {
            std::string s;
            while(!std::isspace(peek()))
                s += next();
            v.push_back(Token(Token::String, s));
        }
        if(peek() == '\n' || peek() == '\r')
        {
            while(accept('\n') || accept('\r'));
            v.push_back(Token(Token::Newline, "\n"));
        }
    }
    v.push_back(Token(Token::Eof, ""));
    return v;
}

class Parser
{
public:

    Parser(std::vector<Token> tokens) : tokens(tokens)
    {
    }

    Token peek()
    {
        return p < tokens.size() ? tokens[p] : Token(Token::Eof);
    }

    Token next()
    {
        return p < tokens.size() ? tokens[p++] : Token(Token::Eof);
    }

    bool accept(std::string s)
    {
        if(p >= tokens.size())
            return false;
        
        return tokens[p].str == s ? ++p : false;
    }

    bool accept(const Token& token)
    {
        return tokens[p] == token ? ++p : false;
    }

    bool accept(Token::Type t)
    {
        return tokens[p].type == t ? ++p : false;
    }

    bool eof()
    {
        return p == tokens.size();
    }

    std::vector<Token> tokens;
    int p;
};

struct MeshData
{
    std::map<std::string, Material*> materials;
    std::set<MeshLight*> meshLights;

    TriangleMesh* mesh;
    bool normalInterp;
    bool isLightMesh;

    std::vector<MeshVertex*> vectors;
    std::vector<Vector3d> normals;
    std::map<int, MeshVertex*> groupVertices; // The vertices that have been added to the current group so far

    MeshData()
    {
        normalInterp = true;
        isLightMesh = false;
    }
};

std::tuple<bool, std::string> parseStr(Parser& parser)
{
    if(parser.peek().type != Token::String)
        return { false, "" };

    return { true, parser.next().str };
}

std::tuple<bool, int> parseInt(Parser& parser)
{
    if(parser.peek().type != Token::Number)
        return { false, 0 };

    int d = 0;
    std::stringstream ss(parser.next().str);
    ss >> d;

    return { true, d };
}

std::tuple<bool, double> parseDouble(Parser& parser)
{
    if(parser.peek().type != Token::Number)
        return { false, 0 };

    double d = 0;
    std::stringstream ss(parser.next().str);
    ss >> d;

    return { true, d };
}

std::tuple<bool, int, int, int> parseVertex(Parser& parser)
{
    int n1, n2, n3;
    auto [success, x] = parseInt(parser);
    n1 = x;
    if(!success)
        return { false, 0, 0, 0 };
    
    if(!parser.accept("/")) // Format is x
        return { true, n1, 0, 0 };

    if(parser.accept("/")) { // Format is x//y
        auto [success, x] = parseInt(parser);
        n2 = x;
        if(!success)
            return { false, 0, 0, 0 };
        return { true, n1, 0, n2 };
    }

    auto [success2, y] = parseInt(parser);
    n2 = y;
    if(!success2)
        return { false, n1, 0, 0 };

    if(!parser.accept("/"))
        return { true, n1, n2, 0 };

    auto [success3, z] = parseInt(parser);
    n3 = z;
    if(success3)
        return { true, n1, n2, n3 };

    return { false, n1, n2, 0 };
}

std::tuple<bool, Vector3d> parseCoordinate3d(Parser& parser)
{
    double arr[3];
    for(int i = 0; i < 3; i++) {
        auto [success, d] = parseDouble(parser);
        if(!success)
            return { false, { 0, 0, 0 } };
        arr[i] = d;
    }
    return { true, Vector3d(arr[0], arr[1], arr[2]) };
}

std::tuple<bool, Vector3d> parseVtCoordinate(Parser& parser)
{
    double arr[3] = { 0, 0, 0 };
    for(int i = 0; i < 3; i++) {
        auto [success, d] = parseDouble(parser);
        if(success)
            arr[i] = d;
    }
    return { true, Vector3d(arr[0], arr[1], arr[2] ) };
}

std::tuple<bool, TriangleMesh*, std::vector<MeshLight*>> _ReadFromFile(std::string file, Material* meshMat)
{
    Material* curmat = 0;
    std::ifstream myfile;
    myfile.open(file.c_str(), std::ios::out);

    if(myfile.fail())
    {
        myfile.close();
        __debugbreak();
        return { false, {}, {} };
    }

    std::string str;

    while(!myfile.eof())
    {
        std::istringstream ins;
        std::string line;
        std::getline(myfile, line);
        auto strs = split(line, '#');
        str += strs[0] + '\n';
    }


    MeshData meshData;
    meshData.mesh = new TriangleMesh();

    TriangleMesh* currentMesh = meshData.mesh;

    std::string a;
    double x, y, z;

    int index = 0;
    bool normalInterp = true;

    bool isLightMesh = false;
    std::unordered_map<MeshVertex*, Vertex3d*> replacement;

    Parser parser(tokenize(str));

    while(true)
    {
        while(parser.accept(Token::Newline));

        if(parser.accept("f"))
        {
            std::vector<MeshVertex*> face;
            std::vector<int> ns;
            std::vector<int> ts;

            while(true)
            {
                auto p = parser.p;
                auto [success, v, t, n] = parseVertex(parser);
                if(!success) {
                    parser.p = p; // TODO: Not the best way of dealing with a failed parse
                    break;
                }

                MeshVertex* mv;

                if (v < 0)
                    v = (int) meshData.vectors.size() + v + 1;
                auto it = meshData.groupVertices.find(v - 1);
                if (it == meshData.groupVertices.end()) { // We have not seen this vertex before in this group so create a new one
                    mv = meshData.groupVertices[v-1] = new MeshVertex(*meshData.vectors[v-1]);
                    if(n) {
                        normalInterp = false; // A normal was submitted so let's trust that one in accordance with .obj standards
                        mv->normal = meshData.normals[n-1];
                    }
                    currentMesh->points.push_back(mv);
                } else { // This vertex is already among the parsed vertices in this group so use that particular one
                    mv = it->second;
                    if(n)
                    {
                        normalInterp = false;
                        if(mv->normal != meshData.normals[n-1])  // A different normal was given though, so we still need
                        {                                 // to create an entirely new vertex
                            mv = new MeshVertex(*mv); 
                            currentMesh->points.push_back(mv);
                        }
                        mv->normal = meshData.normals[n-1];
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
        else if(parser.accept("g"))
        {   
            int names = 0;
            while(true) {
                auto [success, dummyName] = parseStr(parser);
                if(success) {
                    names++;
                } else
                    break;
            }
            if(!names) {
                __debugbreak();
                    return { false, nullptr, {} };
            }

            std::map<int, MeshVertex*>::iterator it;
            std::stack<MeshVertex*> vs;

            for(it = meshData.groupVertices.begin(); it != meshData.groupVertices.end(); it++)
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
            meshData.groupVertices.clear();
        } // if(a == "g" ..
        else if(parser.accept("mtllib"))
        {
            // Check if there's an associated materials file, and parse it
            auto [success, matfilestr] = parseStr(parser);
            if(!success) {
                __debugbreak();
                return { false, nullptr, {} };
            }

            if(!meshMat)
                ReadMaterialFile(matfilestr, meshData.materials);

            continue;
        }
        else if(parser.accept("vn"))
        {
            auto [success, normal] = parseCoordinate3d(parser);
            if(!success) {
                __debugbreak();
                return { false, nullptr, {} };
            }
            meshData.normals.push_back(normal);
        }
        else if(parser.accept("o")) {
            auto [success, str] = parseStr(parser);
            if(!success) {
                __debugbreak();
                return { false, nullptr, {} };
            }
        }
        else if(parser.accept("vt"))
        {
            auto [success, texturecoords] = parseVtCoordinate(parser);
            if(!success) {
                __debugbreak();
                return { false, nullptr, {} };
            }
            //meshData.normals.push_back(normal);
        }
        else if(parser.accept("s")) // Smoothing group ending or starting
        {
            auto [success, s1] = parseStr(parser);
            if(success) {
                normalInterp = s1 != "off";
            } else {
                auto [success, s1] = parseInt(parser);
                if(!success)
                    __debugbreak();
                normalInterp = s1 != 0;
            }
            
        }
        else if(parser.accept("v"))
        {
            auto [success, coord] = parseCoordinate3d(parser);
            meshData.vectors.push_back(new MeshVertex((coord)));
            index++;
        }
        else if(parser.accept("usemtl"))
        {
            auto [success, mtl] = parseStr(parser);
            if(!success) {
                __debugbreak();
                return { false, nullptr, {} };
            }
            auto it = meshData.materials.find(mtl);
            if(it == meshData.materials.end())
                curmat = 0;
            else {
                if(meshData.materials[mtl]->light) {
                    isLightMesh = true;
                    meshData.meshLights.emplace((MeshLight*) meshData.materials[mtl]->light);
                    currentMesh = ((MeshLight*) meshData.materials[mtl]->light)->mesh;
                } else {
                    isLightMesh = false;
                    currentMesh = meshData.mesh;
                }
                curmat = meshData.materials[mtl];
            }
        }
        else if(parser.accept(Token::Newline));
        else if(parser.accept(Token::Eof))
            break;
        else {
            __debugbreak();
            return { false, nullptr, {} };
        }
    }

    for(auto& t : meshData.mesh->triangles) {
        for(auto vv : { &t->v0, &t->v1, &t->v2 }) {
            auto& v = (*((MeshVertex**) vv));
            if(replacement.find(v) == replacement.end()) {
                auto oldv = v;
                v = (MeshVertex*) new Vertex3d(v->pos, v->normal, v->texpos);
                replacement[oldv] = (Vertex3d*) v;
                meshData.mesh->points.push_back(v);
                replacement[v] = v;
            }
            else
                v = (MeshVertex*) replacement[v];
        }
    }
    for(auto& p : meshData.mesh->points)
        if(replacement.find((MeshVertex*)p) != replacement.end())
            p = replacement[(MeshVertex*)p];

    for(auto& m : meshData.meshLights) {
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

    for(auto it = meshData.materials.begin(); it != meshData.materials.end(); it++)
        meshData.mesh->materials.push_back((*it).second);

    myfile.close();
    auto meshLightVector = std::vector<MeshLight*>(meshData.meshLights.begin(), meshData.meshLights.end());
    return { true, meshData.mesh, meshLightVector };
}

std::tuple<bool, TriangleMesh*, std::vector<MeshLight*>> ReadFromFile(std::string file, Material* meshMat)
{
    return _ReadFromFile(file, meshMat);

    Material* curmat = 0;
    std::map<std::string, Material*> materials;
    std::ifstream myfile;
    std::string line;
    myfile.open(file.c_str(), std::ios::out);

    std::set<MeshLight*> meshLights;

    TriangleMesh* mesh = new TriangleMesh();
    TriangleMesh* currentMesh = mesh;

    if(myfile.fail())
    {
        myfile.close();
        __debugbreak();
        return { false, {}, {} };
    }

    std::string a;
    double x, y, z;

    int index = 0;
    bool normalInterp = true;
    std::vector<MeshVertex*> vectors;
    std::vector<Vector3d> normals;
    std::map<int, MeshVertex*> groupVertices; // The vertices that have been added to the current group so far

    bool isLightMesh = false;
    std::unordered_map<MeshVertex*, Vertex3d*> replacement;

    while(!myfile.eof())
    {
        std::istringstream ins;
        std::getline(myfile, line);
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

            std::string s1, s2, s3;

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
                    v = (int) vectors.size() + v + 1;
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
            std::map<int, MeshVertex*>::iterator it;
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
            std::ifstream matfile;
            std::string matfilestr;
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
            std::string s1;
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
            std::string mtl;
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
