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
#include "Logger.h"
#include <stack>
#include <tuple>
#include <set>
#include <unordered_map>
#include "Timer.h"
#include <charconv>


class Token
{
public:
    enum Type { String, Number, Operator, Newline, Eof };
    static inline std::string typeStrs[5] = { "string", "number", "operator", "newline", "eof" };

    Token(Type t, const char* a, const char* b) : type(t), str(a, b), line(0), column(0)
    {
    }

    Token(Type t, const std::string& a) : type(t), str(a), line(0), column(0)
    {
    }

    Token(Type t) : type(t), line(0), column(0)
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

    int line, column;
    Type type;
    std::string_view str;
};

struct ParseException
{
    ParseException(const std::string& str, int line, int col)
    {
        message = str + " at line " + std::to_string(line) + ", column " + std::to_string(col);
    }

    ParseException(const std::string& str, const Token& t)
    {
        message = str + " at line " + std::to_string(t.line) + ", column " + std::to_string(t.column);
    }

    ParseException(const std::string& str)
    {
        message = str;
    }

    std::string message;
};

class Parser
{
public:
    Parser(const std::vector<Token>& tokens) : tokens(tokens), p(0)
    {
    }

    Token peek()
    {
        return tokens[p];
    }

    Token next()
    {
        return tokens[p++];
    }

    bool accept(const char* s)
    {
        return tokens[p].str == s ? ++p : false;
    }

    bool accept(const Token& t)
    {
        return tokens[p] == t ? ++p : false;
    }

    bool accept(Token::Type t)
    {
        return tokens[p].type == t ? ++p : false;
    }

    bool expect(Token::Type t)
    {
        if(p >= tokens.size() || tokens[p].type != t)
            throw ParseException("Expected " + Token::typeStrs[t], tokens[p]);
        p++;
        return true;
    }

    bool expect(const Token& token)
    {
        if(p >= tokens.size() || tokens[p] != token)
            throw ParseException("Expected " + Token::typeStrs[token.type] + " " + std::string(token.str), tokens[p]);
        p++;
        return true;
    }

    bool eof()
    {
        return p == tokens.size();
    }

    std::vector<Token> tokens;
    int p;
};

std::tuple<bool, std::string_view> acceptStr(Parser& parser)
{
    if(parser.peek().type != Token::String)
        return { false, "" };

    return { true, parser.next().str };
}

bool acceptAnyCaseStr(Parser& parser, const std::string_view& str)
{
    if(parser.peek().type != Token::String)
        return false;

    const auto& a = parser.peek().str;
    if(!std::equal(a.begin(), a.end(), str.begin(), str.end(), [](char a, char b) { return tolower(a) == tolower(b); }))
        return false;

    parser.next();
    return true;
}

std::string_view expectStr(Parser& parser)
{
    if(parser.peek().type != Token::String)
        throw ParseException("Alphanumeric string expected", parser.peek().line, parser.peek().column);
    return parser.next().str;
}

int expectInt(Parser& parser)
{
    if(parser.peek().type != Token::Number)
        throw ParseException("Integer expected", parser.peek());

    int d = 0;
    const auto& st = parser.next().str;
    auto res = std::from_chars(st.data(), st.data() + st.size(), d);
    if(res.ec == std::errc::invalid_argument)
        throw ParseException("Integer expected", parser.peek());

    return d;
}

std::tuple<bool, int> acceptInt(Parser& parser)
{
    if(parser.peek().type != Token::Number)
        return { false, 0 };

    int d = 0;
    const auto& st = parser.next().str;
    auto res = std::from_chars(st.data(), st.data() + st.size(), d);

    return { true, d };
}

double expectDouble(Parser& parser)
{
    if(parser.peek().type != Token::Number)
        throw ParseException("Floating point expected", parser.peek());

    double d = 0;
    const auto& st = parser.next().str;
    auto res = std::from_chars(st.data(), st.data() + st.size(), d);

    return d;
}

std::tuple<bool, double> acceptDouble(Parser& parser)
{
    if(parser.peek().type != Token::Number)
        return { false, 0 };

    double d = 0;
    const auto& st = parser.next().str;
    auto res = std::from_chars(st.data(), st.data() + st.size(), d);

    return { true, d };
}

std::tuple<bool, int, int, int> acceptVertex(Parser& parser)
{
    auto [success, n1] = acceptInt(parser);
    if(!success)
        return { false, 0, 0, 0 };
    
    if(!parser.accept("/")) // Format is x
        return { true, n1, 0, 0 };

    if(parser.accept("/")) { // Format is x//y
        int n2 = expectInt(parser);
        return { true, n1, 0, n2 };
    }

    int n2 = expectInt(parser); // Format is x/y..

    if(!parser.accept("/"))
        return { true, n1, n2, 0 };

    int n3 = expectInt(parser); // Format is x/y/z

    return { true, n1, n2, n3 };
}

Vector3d expectVector3d(Parser& parser)
{
    double arr[3];
    for(int i = 0; i < 3; i++)
        arr[i] = expectDouble(parser);
    return Vector3d(arr[0], arr[1], arr[2]);
}

Vector3d expectVtCoordinate(Parser& parser)
{
    double arr[3] = { 0, 0, 0 };
    bool hadSuccess = false;
    for(int i = 0; i < 3; i++) {
        auto [success, d] = acceptDouble(parser);
        if(success) {
            arr[i] = d;
            hadSuccess = true;
        }
    }
    if(!hadSuccess)
        throw ParseException("Bad texture coordinate");
    return Vector3d(arr[0], arr[1], arr[2]);
}

std::vector<Token> tokenize(std::ifstream& file, std::string& str)
{
    //Timer t1;
    //t1.Reset();

    // Before tokenizing, remove any comments
    int i = 0;
    for(std::string ln; !file.eof(); str += '\n')
        for(std::getline(file, ln), i = 0; i < ln.size() && ln[i] != '#'; i++)
            str += ln[i];

    std::vector<Token> v;
    int line = 1, col = 1, p = 0;

    auto addToken = [&v, &line, &col, &str] (Token::Type t, int a, int b)
    {
        for(auto it = str.begin()+a; it < str.begin()+b; it++) {
            if(*it == '\n')
                line++, col = 1;
            else
                col++;
        }
        Token token(t, str.c_str()+a, str.c_str()+b);
        token.line = line;
        token.column = col;
        v.push_back(token);
    };

    auto skipspace = [&p, &str] ()
    {
        while(p < str.length() && str[p] == ' ' || str[p] == '\t')
            p++;
    };

    auto peek = [&p, &str] () -> char
    {
        return p < str.size() ? str[p] : 0;
    };

    auto next = [&p, &str] () -> char
    {
        return p < str.size() ? str[p++] : 0;
    };

    auto accept = [&p, &str] (char c) -> bool
    {
        return (p < str.size() && str[p] == c) ? ++ p : false;
    };

    while(p < str.size())
    {
        skipspace();
        auto c = peek();
        if(c == '/' || c == '{' || c == '}' || c == ':') { // Operator
            addToken(Token::Operator, p, p+1);
            p++;
        }

        else if(std::isdigit(c) || c == '-' || c == '.') // Number
        {
            auto c = peek();
            int d = p;
            // Not the best way of parsing a number ..
            for(auto c = peek(); std::isdigit(c) || c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-'; c = peek())
                p++;

            double D = 0;
            auto res = std::from_chars(str.c_str()+d, str.c_str() + p, D);
            if(res.ec == std::errc::invalid_argument)
                throw ParseException("Couldn't parse floating point number \"" + str.substr(d, p-d) + "\"", line, col);

            addToken(Token::Number, d, p);
        }
        else if(std::isalpha(c)) // Identifier
        {
            // Another way we could do this is to make a token resolve to a string type as a last fallback after
            // trying every other type. This would solve stuff like "mttllib ./blah.mtl" which tries to parse
            // ./blah.mtl as a floating point number, and then just throwing outright, instead of trying to
            // interpret it as a string. Of course, this wouldn't solve other types of contextual ambiguities
            int d = p;
            while(!std::isspace(peek()))
                p++;
            addToken(Token::String, d, p);
        }
        else if(c == '\n') // Endline
        {
            int d = p;
            addToken(Token::Newline, d, ++p);
        }
        else if(accept('\r'))
        {
            int d = p;
            addToken(Token::Newline, d, ++p);
        }
        else
            throw ParseException(std::string("Couldn't parse character: \"") + peek() + "\"");
    }
    addToken(Token::Eof, str.size(), str.size());
    //logger.Box(std::to_string(t1.GetTime()));
    return v;
}

bool ReadMaterialFile(const std::string& matfilestr, std::map<std::string, Material*>& materials)
{
    std::ifstream matfile;
    matfile.open(matfilestr.c_str(), std::ios::out);

    if(matfile.fail())
    {
        matfile.close();
        throw ParseException("Couldn't open the file \"" + matfilestr + "\"");
    }

    std::string str;
    auto parser = Parser(tokenize(matfile, str));

    Material* curmat = 0;
    PhongMaterial* phongmat = 0;
    std::string matName;
    bool phong = false, emissive = false;
    while(!parser.accept(Token::Eof))
    {
        if(parser.accept(Token::Newline))
            while(parser.accept(Token::Newline));

        else if(acceptAnyCaseStr(parser, "newmtl"))
        {
            matName = expectStr(parser);

            if(parser.accept(Token(Token::Operator, ":"))) // We expect a special material definition to follow
            {
                auto b = std::string(expectStr(parser));
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
                else
                    throw ParseException("Unknown material: " + b);
                
                while(parser.accept(Token::Newline));
                parser.expect(Token(Token::Operator, "{"));

                Token token(Token::Eof);
                std::string matArg;
                while((token = parser.next()) != Token(Token::Operator, "}")) {
                    if(token == Token::Eof)
                        throw ParseException("Unexpected end of file");
                    matArg += std::string(token.str) + " ";
                }

                materials[matName] = curmat;
                std::stringstream ss(matArg);
                curmat->ReadProperties(ss);
            }
            else
            {
                curmat = new PhongMaterial;
                materials[matName] = curmat;
                phong = true;
            }
        }

        else if(acceptAnyCaseStr(parser, "ka"))
        {
            // We ignore any ambient term
            for(int i = 0; i < 3; i++)
                expectDouble(parser);

            if(!curmat)
                throw ParseException("No current material specified"); // TODO: not really a parse exception
        }
        else if(acceptAnyCaseStr(parser, "tf"))
        {
            // We ignore any transmission filter
            for(int i = 0; i < 3; i++)
                expectDouble(parser);

            if(!curmat)
                throw ParseException("No current material specified"); // TODO: not really a parse exception
        }
        else if(acceptAnyCaseStr(parser, "kd"))
        {
            if(!curmat)
                throw ParseException("No current material specified"); // TODO: not really a parse exception
            if(phong)
            {
                phongmat = static_cast<PhongMaterial*>(curmat);
                phongmat->Kd = expectVector3d(parser);
            }
            else if(!emissive)
                throw ParseException("Kd specified for custom material");
        }
        else if(acceptAnyCaseStr(parser, "ks"))
        {
            if(!curmat)
                throw ParseException("No current material specified"); // TODO: not really a parse exception
            if(phong)
            {
                phongmat = static_cast<PhongMaterial*>(curmat);
                phongmat->Ks = expectVector3d(parser);
            }
            else if(!emissive)
                throw ParseException("Ks specified for custom material");
        }
        else if(acceptAnyCaseStr(parser, "ns"))
        {
            if(!curmat)
                throw ParseException("No current material specified"); // TODO: not really a parse exception
            if(phong)
            {
                phongmat = static_cast<PhongMaterial*>(curmat);
                phongmat->alpha = expectInt(parser);
            }
            else if(!emissive)
                throw ParseException("Ns specified for custom material");
        }
        else if(acceptAnyCaseStr(parser, "ke")) 
        {
            Color intensity;
            intensity = expectVector3d(parser);

            if(intensity) {
                phong = false;
                emissive = true;
                delete curmat;
                MeshLight* ml = new MeshLight(intensity);
                curmat = ml->material;
                materials[matName] = curmat;
            }
        }
        else if(acceptAnyCaseStr(parser, "ni")) 
            expectDouble(parser);
        else if(acceptAnyCaseStr(parser, "illum")) 
            expectInt(parser);
        else
        {
            auto token = parser.peek();
            throw ParseException("Unknown token \"" + std::string(token.str) + "\"", token);
        }
    }
    return true;
}

std::tuple<bool, TriangleMesh*, std::vector<MeshLight*>> ReadFromFile(const std::string& file, Material* meshMat)
{
    Material* curmat = nullptr;
    std::ifstream myfile;
    myfile.open(file.c_str(), std::ios::out);

    std::map<std::string, Material*> materials;
    std::set<MeshLight*> meshLights;

    TriangleMesh* mesh = new TriangleMesh();
    bool normalInterp;

    std::vector<MeshVertex*> vectors;
    std::vector<Vector3d> normals;
    std::map<int, MeshVertex*> groupVertices; // The vertices that have been added to the current group so far

    TriangleMesh* currentMesh = mesh;

    std::unordered_map<MeshVertex*, Vertex3d*> replacement;

    try {

        if(myfile.fail())
        {
            myfile.close();
            throw ParseException("Can't open the given .obj file \"" + file + "\"");
        }

        std::string str;
        Parser parser(tokenize(myfile, str));

        while(!parser.accept(Token::Eof))
        {
            if(parser.accept(Token::Newline))
                while(parser.accept(Token::Newline));

            if(parser.accept("f"))
            {
                std::vector<MeshVertex*> faceVertices;

                while(true)
                {
                    auto [success, v, t, n] = acceptVertex(parser);
                    if(!success)
                        break;

                    if (v < 0)
                        v = (int) vectors.size() + v + 1;

                    MeshVertex* mv;

                    auto it = groupVertices.find(v-1);
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
                            {                               // to create an entirely new vertex
                                mv = new MeshVertex(*mv); 
                                currentMesh->points.push_back(mv);
                            }
                            mv->normal = normals[n-1];
                        }  
                    }
                    faceVertices.push_back(mv);
                }

                for(int i = 0; i < (int) faceVertices.size()-2; i++) {
                    auto pv0 = faceVertices[0], pv1 = faceVertices[i+1], pv2 = faceVertices[i+2];

                    MeshTriangle* tri = new MeshTriangle(pv0, pv1, pv2);
                    currentMesh->triangles.push_back(tri);
                    for(auto& p : { pv0, pv1, pv2 })
                        p->triangles.push_back(tri);

                    if (pv0->normal.IsNull())
                        for(auto& v : { tri->v0, tri->v1, tri->v2 })
                            v->normal = tri->GetNormal();

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
            else if(parser.accept("g") || parser.peek() == Token::Eof)
            {   
                // We don't care about the name of the group
                for(auto p = acceptStr(parser); std::get<0>(p); p = acceptStr(parser));

                std::stack<MeshVertex*> vs;

                for(auto& v : groupVertices)
                    vs.push(v.second);

                // Create duplicate vertices for every vertex that is part of 
                // triangles that are above a certain angle threshold to each other
                if(true)
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
                                    MeshVertex*& v1 = v;

                                    v1->triangles.clear();
                                    MeshVertex* v2 = new MeshVertex(*v);
                                    currentMesh->points.push_back(v2);
                                    v2->triangles.clear();
                                    v1->normal = t1->GetNormal(); // Just the geometric normal for now, maybe averaged is better
                                    v2->normal = t2->GetNormal();
                                    // Reassign the triangles that belonged to v to either v1 or v2 depending on their normal
                                    for(auto& t3 : v->triangles)
                                    {
                                        if(t3->GetNormal() * v1->normal >= 0.7f)
                                        {
                                            // First reassign the correct vector in the triangle
                                            for(auto vv : { &t3->v0, &t3->v1, &t3->v2 })
                                                if(*vv == v)
                                                    *vv = v1;
                                            // Then reassign the triangle in the vector's triangle list
                                            v1->triangles.push_back(t3);
                                        }
                                        else
                                        {
                                            for(auto vv : { &t3->v0, &t3->v1, &t3->v2 })
                                                if(*vv == v)
                                                    *vv = v2;
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
            else if(parser.accept("mtllib"))
            {
                // Check if there's an associated materials file, and parse it
                auto matfilestr = expectStr(parser);
                if(!meshMat)
                    ReadMaterialFile(std::string(matfilestr), materials);
                continue;
            }
            else if(parser.accept("vn"))
            {
                auto normal = expectVector3d(parser);
                normals.push_back(normal);
            }
            else if(parser.accept("o")) {
                auto str = expectStr(parser);
            }
            else if(parser.accept("vt"))
            {
                auto texturecoords = expectVtCoordinate(parser);
            }
            else if(parser.accept("s")) // Smoothing group ending or starting
            {
                auto [success, s1] = acceptStr(parser);
                if(success) {
                    normalInterp = s1 != "off";
                } else {
                    int s1 = expectInt(parser);
                    normalInterp = s1 != 0;
                }
            }
            else if(parser.accept("v"))
            {
                auto coord = expectVector3d(parser);
                vectors.push_back(new MeshVertex((coord)));
            }
            else if(parser.accept("usemtl"))
            {
                auto mtl = std::string(expectStr(parser));
                auto it = materials.find(mtl);
                if(it == materials.end())
                    curmat = 0;
                else {
                    if(materials[mtl]->light) {
                        meshLights.emplace(static_cast<MeshLight*>(materials[mtl]->light));
                        currentMesh = (static_cast<MeshLight*>(materials[mtl]->light)->mesh);
                    } else
                        currentMesh = mesh;
                    curmat = materials[mtl];
                }
            }
            else if(parser.accept(Token::Newline));
            else if(parser.accept("l"))
            {
                while(parser.next().type != Token::Newline);
            }
            else {
                auto token = parser.peek();
                throw ParseException("Unknown token \"" + std::string(token.str) + "\"", token.line, token.column);
            }
        }
    }
    catch(const ParseException& p)
    {
        logger.Box(p.message);
        __debugbreak();
    }

    for(auto& t : mesh->triangles) {
        for(auto vv : { &t->v0, &t->v1, &t->v2 }) {
            auto& v = (*(reinterpret_cast<MeshVertex**>(vv)));
            if(replacement.find(v) == replacement.end()) {
                auto oldv = v;
                v = static_cast<MeshVertex*>(new Vertex3d(v->pos, v->normal, v->texpos));
                replacement[oldv] = (Vertex3d*) v;
                mesh->points.push_back(v);
                replacement[v] = v;
            }
            else
                v = static_cast<MeshVertex*>(replacement[v]);
        }
    }
    for(auto& p : mesh->points)
        if(replacement.find(static_cast<MeshVertex*>(p)) != replacement.end())
            p = replacement[static_cast<MeshVertex*>(p)];

    for(auto& m : meshLights) {
        for(auto& t : m->mesh->triangles) {
            for(auto vv : { &t->v0, &t->v1, &t->v2 }) {
                auto& v = (*((MeshVertex**) vv));
                if(replacement.find(v) == replacement.end()) {
                    auto oldv = v;
                    v = static_cast<MeshVertex*>(new Vertex3d(v->pos, v->normal, v->texpos));
                    replacement[oldv] = static_cast<Vertex3d*>(v);
                    m->mesh->points.push_back(v);
                    replacement[v] = v;
                }
                else
                    v = static_cast<MeshVertex*>(replacement[v]);
            }
        }
        for(auto& p : m->mesh->points)
            if(replacement.find(static_cast<MeshVertex*>(p)) != replacement.end())
                p = replacement[static_cast<MeshVertex*>(p)];
    }

    for(auto it = materials.begin(); it != materials.end(); it++)
        mesh->materials.push_back((*it).second);

    myfile.close();
    auto meshLightVector = std::vector<MeshLight*>(meshLights.begin(), meshLights.end());
    return { true, mesh, meshLightVector };
}
