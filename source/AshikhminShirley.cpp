#include "Bytestream.h"
#include "AshikhminShirley.h"

AshikhminShirley::AshikhminShirley()
{
    Rs = Color(1, 1, 1);
    Rd = Color(1, 1, 1);
    n = 1;
}

AshikhminShirley::~AshikhminShirley()
{
}

void AshikhminShirley::ReadProperties(stringstream& ss)
{
    while(!ss.eof())
	{
		string line;
        string a;
		getline(ss, line);
		stringstream ss2(line);
		ss2 >> a;
        transform(a.begin(), a.end(), a.begin(), tolower);
        if(a == "rd")
            ss2 >> Rd.r >> Rd.g >> Rd.b;
        else if(a == "rs")
            ss2 >> Rs.r >> Rs.g >> Rs.b;
        else if(a == "n")
            ss2 >> n;
	}
}

Color AshikhminShirley::GetSample(const IntersectionInfo& info, Ray& outRay, bool adjoint) const
{
    Vector3d wi = -info.GetDirection();
    Vector3d wo = outRay.GetDirection();

    Vector3d h = (wi + wo)/2;

        Vector3d Ng, Ns;
        const Vector3d in = info.GetDirection();

        Ng = in*info.GetGeometricNormal() < 0 ? info.GetGeometricNormal() 
            : -info.GetGeometricNormal();

        Ns = Ng*info.GetNormal() > 0 ? info.GetNormal() : -info.GetNormal();

		float r1 = rnd.GetFloat(0, 2*F_PI);
		float r2 = rnd.GetFloat(0, 0.9999f);

        Vector3d normal = adjoint ? Ng : Ns;
		Vector3d right = normal^Vector3d(1, 0, 0);

		if(right.GetLength() < 0.0001f)
			right = normal^Vector3d(0, 0, 1);
	
		right.Normalize();
		Vector3d forward = normal^right;
		forward.Normalize();

    float df = (Rd*(Color(1.0f, 1.0f, 1.0f)-Rs)).GetMax()*(1-pow(1-(abs(normal*wi))/2, 5));
    float sp = (Rs + (Color(1.0f, 1.0f, 1.0f) - Rs)*(pow(1.0f-abs(in*normal), 5.0f))).GetMax();
    //float df = Rd.GetMax();
    //float sp = Rs.GetMax();

    float r = rnd.GetFloat(0.0001f, df + sp);

	if(r <= df) // Diffuse bounce
	{
        Vector3d Ng, Ns;
        const Vector3d in = info.GetDirection();

        Ng = in*info.GetGeometricNormal() < 0 ? info.GetGeometricNormal() 
            : -info.GetGeometricNormal();

        Ns = Ng*info.GetNormal() > 0 ? info.GetNormal() : -info.GetNormal();

		float r1 = rnd.GetFloat(0, 2*F_PI);
		float r2 = rnd.GetFloat(0, 0.9999f);

        Vector3d normal = adjoint ? Ng : Ns;
		Vector3d right = normal^Vector3d(1, 0, 0);

		if(right.GetLength() < 0.0001f)
			right = normal^Vector3d(0, 0, 1);
	
		right.Normalize();
		Vector3d forward = normal^right;
		forward.Normalize();

		outRay = Ray(info.GetPosition() + normal*0.0001f, 
            forward*cos(r1)*sqrt(r2) + right*sin(r1)*sqrt(r2) 
            + normal * sqrt(1-r2));
		outRay.direction.Normalize();
        const Vector3d& out = outRay.direction;

        if(in*Ns > 0 || out*Ns < 0 || out*Ng < 0 || in*Ng > 0)
            return Color(0, 0, 0);

        Color mod = (28.0f/23.0f)*Rd*(Color(1.0f, 1.0f, 1.0f)-Rs)*(1-pow(1-abs(normal*wi)/2, 5.0f))*(1-pow(1-(normal*out)/2, 5.0f));

		return (adjoint ? abs(in*Ns)/abs(in*Ng) : 1.0f)*mod/(df/(df+sp));
	}
	else // Specular bounce
	{
        Vector3d Ng, Ns;
        const Vector3d in = info.GetDirection();
		float r1 = rnd.GetFloat(0.0f, 2*F_PI);
		float r2 = acos(pow(rnd.GetFloat(0.0001f, 0.9990f), 1/float(n+1)));

        Ng = in*info.GetGeometricNormal() < 0 ? info.GetGeometricNormal() 
            : -info.GetGeometricNormal();
        Ns = Ng*info.GetNormal() > 0 ? info.GetNormal() : -info.GetNormal();
        Vector3d normal = adjoint ? Ng : Ns;

        Vector3d right = normal^Vector3d(1, 0, 0);
        if(right.GetLength() < 0.0001f)
			right = normal^Vector3d(0, 0, 1);

        right.Normalize();
		Vector3d forward = normal^right;
		forward.Normalize(); 
        Vector3d hv = sin(r2)*(forward*cos(r1) + right*sin(r1)) + cos(r2)*normal;
        outRay = Ray(info.GetPosition() + normal*0.0001f, -wi + 2*(wi*hv)*hv);
		outRay.direction.Normalize();
        Vector3d& out = outRay.direction;

        //hv = (outRay.direction + wi)/2;
        
        if(in*Ns > 0 || out*Ns < 0 || out*Ng < 0 || in*Ng > 0)
            return Color(0, 0, 0);

        Color fresnel = Rs + (Color(1.0f, 1.0f, 1.0f) - Rs)*(pow(1-out*hv, 5.0f));
        Color mod = (normal*outRay.direction)*fresnel/(max(normal*wi, normal*outRay.direction));

        //Color mod = abs(outRay.direction*normal)*Rs*(n + 2)/(n + 1);
		return (adjoint ? abs(in*Ns)/abs(in*Ng) : 1.0f)*mod/(sp/(df+sp));
	}
return Color(0, 0, 0);
}

Color AshikhminShirley::GetSampleE(const IntersectionInfo& info, Ray& outRay, float& pdf, float& rpdf, unsigned char& component, bool adjoint) const
{
    float df = Rd.GetMax();
    float sp = Rs.GetMax();

    float r = rnd.GetFloat(0.0000f, df + sp);

	if(r <= df) // Diffuse bounce
	{
        component = 1;

        Vector3d N_g = info.GetGeometricNormal();
        Vector3d N_s = info.GetNormal();
        Vector3d w_i = -info.GetDirection();

		float r1 = rnd.GetFloat(0, 2*F_PI);
		float r2 = rnd.GetFloat(0, 0.9999f);

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

        outRay.origin = info.GetPosition();
	    outRay.direction = dir;

        if(w_i*N_g < 0 || w_o*N_g < 0 || w_i*N_s < 0 || w_o*N_s < 0)
        {
            pdf = rpdf = 0;
            return Color(0, 0, 0);
        }

        Color mod = (28.0f/23.0f)*Rd*(Color::Identity - Rs)*(1-pow(1-abs(N_s*w_i)/2.0f, 5.0f))*(1-pow(1-(N_s*w_o)/2.0f, 5.0f));

		return (adjoint ? abs(w_i*N_s)/abs(w_i*N_g) : 1.0f)*mod/(df/(df+sp));
	}
	else // Specular bounce
	{
        component = 2;
		float r1 = rnd.GetFloat(0.0f, 2*F_PI);
		float r2 = acos(pow(rnd.GetFloat(0.0001f, 0.9999f), 1/float(n+1)));

        Vector3d N_g = info.GetGeometricNormal();
        Vector3d N_s = info.GetNormal();
        Vector3d w_i = -info.GetDirection();

        if(w_i*N_g < 0)
		    N_g = -N_g;

        if(N_g*N_s < 0)
            N_s = -N_s;

        Vector3d N = adjoint ? N_g : N_s;
	    Vector3d adjN = adjoint ? N_s : N_g;

        Vector3d right, forward;
        MakeBasis(N_s, right, forward);
        Vector3d hv = sin(r2)*(forward*cos(r1) + right*sin(r1)) + cos(r2)*N_s;
        outRay = Ray(info.GetPosition(), -w_i + 2*(w_i*hv)*hv);
		outRay.direction.Normalize();
        Vector3d& w_o = outRay.direction;

		pdf = pow(N_s*hv, n)*(n + 1)/((w_i*hv)*8*F_PI);
        rpdf = pow(N_s*hv, n)*(n + 1)/((w_o*hv)*8*F_PI);
        if(rpdf < 0)
            rpdf = 0;
        if(pdf < 0)
            pdf = 0;
        
        if(w_i*N_s < 0 || w_o*N_s < 0 || w_o*N_g < 0 || w_i*N_g < 0) 
        {
            pdf = rpdf = 0;
            return Color(0, 0, 0);
        }

        Color fresnel = Rs + (Color::Identity - Rs)*(pow(1-w_o*hv, 5.0f));
        Color mod = abs(N*w_o)*fresnel/(max(N_s*w_i, N_s*w_o));
		return (adjoint ? abs(w_i*N_s)/abs(w_i*N_g) : 1.0f)*mod/(sp/(df+sp));
	}
}

Color AshikhminShirley::BRDF(const IntersectionInfo& info, const Vector3d& out) const
{
    Vector3d wi = -info.GetDirection();
    Vector3d h = (wi + out);
    h.Normalize();
    Vector3d normal = info.GetNormal();
    float b = h.GetLength();

    if(h*normal < 0 || normal*out < 0 || normal*wi < 0)
        return Color(0, 0, 0);

    Color specular = (Rs + (Color::Identity - Rs)*pow(1-wi*h, 5.0f))*pow(normal*h, float(n))*(float(n + 1)/(8*F_PI))/( (h*out)*max(normal*wi, normal*out) );
    Color diffuse = Rd*(28.0f/(23.0f*F_PI))*(Color::Identity - Rs)*(1-pow(1-(normal*out)/2, 5.0f))*(1-pow(1-abs(normal*wi)/2, 5.0f));
    return specular + diffuse;
}

Color AshikhminShirley::ComponentBRDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component) const
{
    float df = Rd.GetMax();
    float sp = Rs.GetMax();

  	assert(component == 1 || component == 2);

    Vector3d N_s = info.GetNormal();
    Vector3d N_g = info.GetGeometricNormal();
    Vector3d in = -info.GetDirection();

    if(in*N_g < 0)
		N_g = -N_g;

    if(N_g*N_s < 0)
        N_s = -N_s;

    if(in*N_g < 0 || out*N_g < 0 || in*N_s < 0 || out*N_s < 0) // FIXME: redundant checks
        return 0;

    Vector3d wi = -info.GetDirection();
    Vector3d h = (wi + out);
    h.Normalize();

    if(component == 1)
        return Rd*(28.0f/(23.0f*F_PI))*(Color::Identity-Rs)*(1-pow(1-abs(N_s*out)/2, 5.0f))*(1-pow(1-(abs(N_s*wi))/2, 5.0f))/(df/(df+sp));
    else
        return (Rs + (Color::Identity - Rs)*pow(1-out*h, 5.0f))*pow(N_s*h, float(n))*(float(n + 1)/(8*F_PI))/( (h*out)*max(N_s*wi, N_s*out) )/(sp/(df+sp));
}

Light* AshikhminShirley::GetLight() const
{
    return 0;
}

bool AshikhminShirley::IsSpecular(unsigned char component) const
{
    assert(component == 1 || component == 2);
    return false;
}

float AshikhminShirley::PDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component, bool adjoint) const
{
    assert(component == 1 || component == 2);
    Vector3d N_s = info.GetNormal();
    Vector3d N_g = info.GetGeometricNormal();
    Vector3d in = -info.GetDirection();
    Vector3d hv = (in + out);
    hv.Normalize();

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
		return pow(N_s*hv, n)*(n + 1)/((in*hv)*8*F_PI);
}

/*	Color GetSample(const IntersectionInfo& info, Ray& out, bool adjoint) const;
	Color GetSampleE(const IntersectionInfo& info, Ray& out, float& pdf, float& rpdf, unsigned char& component, bool adjoint) const;

	Color BRDF(const IntersectionInfo& info, const Vector3d& out) const;
	Color ComponentBRDF(const IntersectionInfo& info, const Vector3d& out, float& pdf, unsigned char component) const;

	Light* GetLight() const;
	bool IsSpecular(unsigned char component) const;	float PDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component) const;

	void ReadProperties(stringstream& ss);*/

void AshikhminShirley::Save(Bytestream& stream) const
{
    stream << (unsigned char) ID_ASHIKHMINSHIRLEY;
    stream << Rd << Rs << n;
}
void AshikhminShirley::Load(Bytestream& stream)
{
    stream >> Rd >> Rs >> n;
}


/*
#include "Bytestream.h"
#include "AshikhminShirley.h"

AshikhminShirley::AshikhminShirley()
{
    Rs = Color(1, 1, 1);
    Rd = Color(1, 1, 1);
    n = 1;
}

AshikhminShirley::~AshikhminShirley()
{
}

void AshikhminShirley::ReadProperties(stringstream& ss)
{
    while(!ss.eof())
	{
		string line;
        string a;
		getline(ss, line);
		stringstream ss2(line);
		ss2 >> a;
        transform(a.begin(), a.end(), a.begin(), ptr_fun(tolower));
        if(a == "rd")
            ss2 >> Rd.r >> Rd.g >> Rd.b;
        else if(a == "rs")
            ss2 >> Rs.r >> Rs.g >> Rs.b;
        else if(a == "n")
            ss2 >> n;
	}
}

Color AshikhminShirley::GetSample(const IntersectionInfo& info, Ray& outRay, bool adjoint) const
{
    Vector3d wi = -info.GetDirection();
    Vector3d wo = outRay.GetDirection();

    //Vector3d h = (wi + wo)/2;

    float df = Rd.GetMax();
    float sp = Rs.GetMax();

    float r = rnd.GetFloat(0.0001f, df + sp);

	if(r <= df) // Diffuse bounce
	{
        Vector3d Ng, Ns;
        const Vector3d in = info.GetDirection();

        Ng = in*info.GetGeometricNormal() < 0 ? info.GetGeometricNormal() 
            : -info.GetGeometricNormal();

        Ns = Ng*info.GetNormal() > 0 ? info.GetNormal() : -info.GetNormal();

		float r1 = rnd.GetFloat(0, 2*F_PI);
		float r2 = rnd.GetFloat(0, 0.9999f);

        Vector3d normal = adjoint ? Ng : Ns;
		Vector3d right = normal^Vector3d(1, 0, 0);

		if(right.GetLength() < 0.0001f)
			right = normal^Vector3d(0, 0, 1);
	
		right.Normalize();
		Vector3d forward = normal^right;
		forward.Normalize();

		outRay = Ray(info.GetPosition() + normal*0.0001f, 
            forward*cos(r1)*sqrt(r2) + right*sin(r1)*sqrt(r2) 
            + normal * sqrt(1-r2));
		outRay.direction.Normalize();
        const Vector3d& out = outRay.direction;

        if(in*Ns > 0 || out*Ns < 0 || out*Ng < 0 || in*Ng > 0)
            return Color(0, 0, 0);

        Color mod = (28.0f/23.0f)*Rd*(Color(1.0f, 1.0f, 1.0f)-Rs)*(1-pow(1-(normal*wi)/2, 5))*(1-pow(1-(normal*out)/2, 5));

		return (adjoint ? abs(in*Ns)/abs(in*Ng) : 1.0f)*mod/(df/(df+sp));
	}
	else // Specular bounce
	{
        Vector3d Ng, Ns;
        const Vector3d in = info.GetDirection();
		float r1 = rnd.GetFloat(0.0f, 2*F_PI);
		float r2 = acos(pow(rnd.GetFloat(0, 0.9990f), 1/float(n+1)));

        Ng = in*info.GetGeometricNormal() < 0 ? info.GetGeometricNormal() 
            : -info.GetGeometricNormal();
        Ns = Ng*info.GetNormal() > 0 ? info.GetNormal() : -info.GetNormal();
        Vector3d normal = adjoint ? Ng : Ns;

        Vector3d right = normal^Vector3d(1, 0, 0);
        if(right.GetLength() < 0.0001f)
			right = normal^Vector3d(0, 0, 1);

        right.Normalize();
		Vector3d forward = normal^right;
		forward.Normalize(); 
        Vector3d hv = sin(r2)*(forward*cos(r1) + right*sin(r1)) + cos(r2)*normal;
        outRay = Ray(info.GetPosition() + normal*0.0001f, -wi + 2*(wi*hv)*hv);
		outRay.direction.Normalize();
        Vector3d& out = outRay.direction;

        //hv = (outRay.direction + wi)/2;
        
        if(in*Ns > 0 || out*Ns < 0 || out*Ng < 0 || in*Ng > 0)
            return Color(0, 0, 0);

        Color fresnel = Rs + (Color(1.0f, 1.0f, 1.0f) - Rs)*(pow(1-out*hv, 5));
        Color mod = (normal*outRay.direction)*fresnel/(max(normal*wi, normal*outRay.direction));

        //Color mod = abs(outRay.direction*normal)*Rs*(n + 2)/(n + 1);
		return (adjoint ? abs(in*Ns)/abs(in*Ng) : 1.0f)*mod/(sp/(df+sp));
	}
}

Color AshikhminShirley::GetSampleE(const IntersectionInfo& info, Ray& out, float& pdf, float& rpdf, unsigned char& component, bool adjoint) const
{
    return Color(0, 0, 0);
}

Color AshikhminShirley::BRDF(const IntersectionInfo& info, const Vector3d& out) const
{
    Vector3d wi = -info.GetDirection();
    Vector3d h = (wi + out);
    h.Normalize();
    Vector3d normal = info.GetNormal();
    float b = h.GetLength();

    if(h*normal < 0 || normal*out < 0)
        return Color(0, 0, 0);

    Color specular = (Rs + (Color(1.0f, 1.0f, 1.0f) - Rs)*pow(1-wi*h, 5))*pow(normal*h, n)*(float(n + 1)/(8*F_PI))/( (h*out)*max(normal*wi, normal*out) );
    Color diffuse = Rd*(28.0f/(23.0f*F_PI))*(Color(1.0f, 1.0f, 1.0f)-Rs)*(1-pow(1-(normal*out)/2, 5))*(1-pow(1-(normal*wi)/2, 5));
    return specular + diffuse;
}

Color AshikhminShirley::ComponentBRDF(const IntersectionInfo& info, const Vector3d& out, float& pdf, unsigned char component) const
{
    Vector3d wi = -info.GetDirection();
    Vector3d h = (wi + out);
    h.Normalize();
    Vector3d normal = info.GetNormal();
    float b = h.GetLength();

    if(h*normal < 0 || normal*out < 0)
        return Color(0, 0, 0);
    if(component == 1)
        return Rd*(28.0f/(23.0f*F_PI))*(Color(1.0f, 1.0f, 1.0f)-Rs)*(1-pow(1-(normal*out)/2, 5))*(1-pow(1-(normal*wi)/2, 5));
    else
        return (Rs + (Color(1.0f, 1.0f, 1.0f) - Rs)*pow(1-wi*h, 5))*pow(normal*h, n)*(float(n + 1)/(8*F_PI))/( (h*out)*max(normal*wi, normal*out) );
    return Color(0, 0, 0);
}

Light* AshikhminShirley::GetLight() const
{
    return 0;
}

bool AshikhminShirley::IsSpecular(unsigned char component) const
{
    assert(component == 1 || component == 2);
    return false;
}

float AshikhminShirley::PDF(const IntersectionInfo& info, const Vector3d& out, unsigned char component) const
{
    return 1;
}


void AshikhminShirley::Save(Bytestream& stream) const
{
    stream << (unsigned char) ID_ASHIKHMINSHIRLEY;
    stream << Rd << Rs << n;
}
void AshikhminShirley::Load(Bytestream& stream)
{
    stream >> Rd >> Rs >> n;
}*/