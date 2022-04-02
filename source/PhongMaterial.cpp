#include <sstream>
#include "Bytestream.h"
#include "PhongMaterial.h"

using namespace std;

PhongMaterial::PhongMaterial()
{
	Kd = Color(1, 1, 1);
	Ks = Color(0, 0, 0);
	Ka = Color(0, 0, 0);
	normalmap = 0;
	alpha = 0;
#ifdef DETERMINISTIC
	rnd.Seed(0);
#else
	rnd.Seed(GetTickCount() + int(this));
#endif
}

PhongMaterial::~PhongMaterial()
{
}

Color PhongMaterial::GetSampleE(const IntersectionInfo& info, Ray& out, float& pdf, float& rpdf, unsigned char& component, bool adjoint) const
{
	float df = Kd.GetMax();
	float sp = Ks.GetMax();
	
	float r = rnd.GetFloat(0, df + sp);
	if(r <= df) // Diffuse bounce
	{
        component = 1;

        float r1 = rnd.GetFloat(0.0001f, 2*M_PI);
        float r2 = rnd.GetFloat(0.0001f, 0.9999f);

	    Vector3d N_g = info.GetGeometricNormal();
        Vector3d N_s = info.GetNormal();
	
        const Vector3d& w_i = -info.GetDirection();
    
        if(w_i*N_g < 0)
		    N_g = -N_g;

        if(N_g*N_s < 0)
            N_s = -N_s;

        Vector3d N = adjoint ? N_g : N_s;
	    Vector3d adjN = adjoint ? N_s : N_g;

	    Vector3d dir;
        SampleHemisphereCos(r1, r2, N, dir);

        const Vector3d& w_o = dir;

        pdf = w_o*N/F_PI;
        rpdf = w_i*adjN/F_PI;
        if(rpdf < 0)
            rpdf = 0;
        if(pdf < 0)
            pdf = 0;

        out.origin = info.GetPosition();
	    out.direction = dir;

        if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
            return Color(0, 0, 0);

        Color ret = adjoint ? Kd*abs(w_i*N_s)/abs(w_i*N_g) : Kd;
        return ret/(df/(df+sp));       
	}
	else // Specular bounce
	{
		component = 2;
		float r1 = rnd.GetFloat(0.0f, 2*F_PI);
		float r2 = acos(pow(rnd.GetFloat(0.0001f, 0.9999f), 1/(alpha+1)));

   	    Vector3d N_g = info.GetGeometricNormal();
        Vector3d N_s = info.GetNormal();
        Vector3d w_i = -info.GetDirection();

		if(w_i*N_g < 0)
		    N_g = -N_g;

        if(N_g*N_s < 0)
            N_s = -N_s;

        Vector3d N = adjoint ? N_g : N_s;
	    Vector3d adjN = adjoint ? N_s : N_g;

		Vector3d up = Reflect(info.GetDirection(), N_s);
		Vector3d right, forward;

		MakeBasis(up, right, forward);
        Vector3d base = forward*cos(r1) + right*sin(r1);

		out = Ray(info.GetPosition(), up + base*tanf(r2));
		out.direction.Normalize();
        Vector3d w_o = out.direction;

        if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
        {
            pdf = rpdf = 0;
            return Color(0, 0, 0);
        }

        // FIXME these should probably be equal
		pdf = rpdf = pow(out.direction*up, alpha)*(alpha + 1)/(2*F_PI);
        //rpdf = pow(-info.GetDirection()*Reflect(-out.direction, N_s), alpha)*(alpha+1)/(2*F_PI);
    	Color mod = abs(out.direction*N)*Ks*float(alpha + 2)/float(alpha + 1);

		return (adjoint ? abs((N_s*w_i)/(N_g*w_i)) : 1)*mod/(sp/(df+sp));
	}
}

Color PhongMaterial::GetSample(const IntersectionInfo& info, Ray& outRay, bool adjoint) const
{
	float df = Kd.GetMax();
	float sp = Ks.GetMax();
	float r = rnd.GetFloat(0, df + sp);

	if(r <= df) // Diffuse bounce
	{
        float r1 = rnd.GetFloat(0.0001f, 2*M_PI);
        float r2 = rnd.GetFloat(0.0001f, 0.9999f);

	    Vector3d N_g = info.GetGeometricNormal();
        Vector3d N_s = info.GetNormal();
	
        const Vector3d& w_i = -info.GetDirection();
    
        if(w_i*N_g < 0)
		    N_g = -N_g;

        if(N_g*N_s < 0)
            N_s = -N_s;

        Vector3d N = adjoint ? N_g : N_s;
	    Vector3d adjN = adjoint ? N_s : N_g;

	    Vector3d dir;
        SampleHemisphereCos(r1, r2, N, dir);

        const Vector3d& w_o = dir;
        outRay.origin = info.GetPosition() + N_g*0.0001f;
	    outRay.direction = dir;

        if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
            return Color(0, 0, 0);

        Color ret = adjoint ? Kd*abs(w_i*N_s)/abs(w_i*N_g) : Kd;
        return ret/(df/(df+sp));
	}
	else // Specular bounce
	{
        Vector3d N_g = info.GetGeometricNormal();
        Vector3d N_s = info.GetNormal();

        const Vector3d in = -info.GetDirection();

        if(in*N_g < 0)
		    N_g = -N_g;

        if(N_g*N_s < 0)
            N_s = -N_s;

		float r1 = rnd.GetFloat(0.0f, 2*F_PI);
		float r2 = acos(pow(rnd.GetFloat(0.0001f, 0.9999f), 1/(alpha+1)));

        Vector3d normal = adjoint ? N_g : N_s;

		Vector3d up = Reflect(info.GetDirection(), N_s);
        Vector3d right, forward;
        MakeBasis(up, right, forward);

        Vector3d base = forward*cos(r1) + right*sin(r1);
		outRay = Ray(info.GetPosition() + N_g*0.0001f, up + base*tanf(r2));
		outRay.direction.Normalize();
        Vector3d& out = outRay.direction;
        
        if(in*N_s < 0 || out*N_s < 0 || out*N_g < 0 || in*N_g < 0)
            return Color(0, 0, 0);

        Color mod = abs(outRay.direction*normal)*Ks*float(alpha + 2)/float(alpha + 1);
		return (adjoint ? abs(in*N_s)/abs(in*N_g) : 1.0f)*mod/(sp/(df+sp));
	}
}

Color PhongMaterial::BRDF(const IntersectionInfo& info, const Vector3d& out) const
{
    Vector3d N_s = info.GetNormal();
    Vector3d N_g = info.GetGeometricNormal();
    const Vector3d& in = -info.GetDirection();

    if(in*N_g < 0)
		N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0)
        return 0;

	if(out*Reflect(info.GetDirection(), N_s) < 0)
		return Kd/F_PI;

	return Kd/F_PI + Ks*((alpha + 2)/(2*F_PI))*pow(out*Reflect(-in, N_s), alpha);
}

Color PhongMaterial::ComponentBRDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component) const
{
	assert(component == 1 || component == 2);

	float df = Kd.GetMax();
	float sp = Ks.GetMax();

    Vector3d N_s = info.GetNormal();
    Vector3d N_g = info.GetGeometricNormal();
    const Vector3d& in = -info.GetDirection();

    if(in*N_g < 0)
		N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0) // FIXME: redundant checks
        return 0;

	if(component == 1)
	{
		//pdf = out*N_s/F_PI;
		return Kd/F_PI/(df/(df+sp));
	}
	else
	{
		Vector3d reflection = Reflect(info.GetDirection(), N_s);

		if(reflection*out < 0)
			return Color(0, 0, 0);

		return Ks*((alpha + 2)/(2*F_PI))*pow(out*reflection, alpha)/(sp/(df+sp));
	}
}

Light* PhongMaterial::GetLight() const
{
	return light;
}

bool PhongMaterial::IsSpecular(unsigned char component) const
{
	assert(component == 1 || component == 2);
	return false;
}

void PhongMaterial::ReadProperties(stringstream& ss)
{
    while(!ss.eof())
	{
		string line;
        string a;
		getline(ss, line);
		stringstream ss2(line);
		ss2 >> a;
        transform(a.begin(), a.end(), a.begin(), tolower);
        if(a == "kd")
            ss2 >> Kd.r >> Kd.g >> Kd.b;
        else if(a == "ks")
            ss2 >> Ks.r >> Ks.g >> Ks.b;
        else if(a == "alpha")
            ss2 >> alpha;
	}
}

float PhongMaterial::PDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component, bool adjoint) const
{
	assert(component == 1 || component == 2);

    Vector3d N_s = info.GetNormal();
    Vector3d N_g = info.GetGeometricNormal();
    const Vector3d& in = -info.GetDirection();

    if(in*N_g < 0)
		N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0)
        return 0;

    Vector3d normal = adjoint ? N_g : N_s;

	if(component == 1)
		return out*normal/F_PI;
	else
	{
		Vector3d up = Reflect(info.GetDirection(), N_s);
        float asdf = out*up;
		return out*up > 0 ? pow(out*up, alpha)*float(alpha + 1)/(2*F_PI) : 0;
	}
}

void PhongMaterial::Save(Bytestream& stream) const
{
    stream << (unsigned char) 101;
    stream << Kd << Ks << alpha;
}

void PhongMaterial::Load(Bytestream& stream)
{
    stream >> Kd >> Ks >> alpha;
}