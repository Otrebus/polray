#include "Color.h"

const Color Color::Identity = Color(1, 1, 1);
const Color Color::Black = Color(0, 0, 0);

Color::Color()
{
}

Color::Color(int c)
{
	r = (float)((c >> 16) & 0xFF) / 255.0f;
	g = (float)((c >> 8) & 0xFF) / 255.0f;
	b = (float)(c & 0xFF) / 255.0f;
	assert(r >= 0 && g >= 0 && b >= 0 && r == r && g == g && b == b);
}

Color::Color(float f)
{
}

/*Color::Color(const Vector3d& v)
{
	r = v.x;
	g = v.y;
	b = v.z;
	assert(r >= 0 && g >= 0 && b >= 0 && r == r && g == g && b == b);
}*/

int Color::GetInt() const
{
	return (int)(b*255) | (int)(g*255) << 8 | (int)(r*255) << 16;
}


Color::Color(float _r, float _g, float _b) : r(_r), g(_g), b(_b)
{
	assert(r >= 0 && g >= 0 && b >= 0 && r == r && g == g && b == b);
}

Color::~Color()
{
}

Color Color::operator+(const Color& v) const
{
	assert(v.r >= 0 && v.g >= 0 && v.b >= 0 && r == r && g == g && b == b);
	return Color(r+v.r, g+v.g, b+v.b);
}

Color Color::operator-(const Color& v) const
{
	assert(r-v.r >= 0 && g-v.g >= 0 && g-v.b >= 0 && r == r && g == g && b == b);
	return Color(r-v.r, g-v.g, b-v.b);
}

Color Color::operator*(float t) const
{
	assert(t >= 0 && r == r && g == g && b == b);
	return Color(r*t, g*t, b*t);
}

Color Color::operator*(int t) const
{
	assert(t >= 0 && r == r && g == g && b == b);
	return Color(r*float(t), g*float(t), b*float(t));
}

void Color::Normalize()
{
	float rl = 1.0f/sqrt(r*r + g*g + b*b);
	r*=rl;
	g*=rl;
	b*=rl;
}

Color operator*(float t, const Color& c)
{
	assert(t >= 0 && c.r >= 0 && c.g >= 0 && c.b >= 0 && c.r == c.r && c.g == c.g && c.b == c.b && t == t);
	return Color(c.r*t, c.g*t, c.b*t);
}

Color operator*(int t, const Color& c)
{
	assert(t >= 0 && c.r >= 0 && c.g >= 0 && c.b >= 0 && c.r == c.r && c.g == c.g && c.b == c.b && t == t);
	return Color(c.r*float(t), c.g*float(t), c.b*float(t));
}

Color Color::operator/(float t) const
{
	assert(t >= 0 && t == t);
	return Color(r/t, g/t, b/t);
}

Color Color::operator/(int t) const
{
	assert(t >= 0 && t == t);
	return Color(r/float(t), g/float(t), b/float(t));
}

Color Color::operator*(const Color& v) const
{
	assert(v.r >= 0 && v.g >= 0 && v.b >= 0 
		   && r == r && g == g && b == b);
	return Color(r*v.r, g*v.g, b*v.b);
}


Color Color::operator+=(const Color& c)
{
	assert(c.r >= 0 && c.g >= 0 && c.b >= 0 
		   && r == r && g == g && b == b);
	r+=c.r;
	g+=c.g;
	b+=c.b;

	return Color(r, g, b);
}

/**
 *  Multiplies each color component with the corresponding color component of
 *  a supplied color, analogous to a dot product. The function behaves as is 
 *  expected by a *= operation.
 *  @param c The color with which to do the color dot product with.
 */
Color Color::operator*=(const Color& c)
{
	assert(c.r >= 0 && c.g >= 0 && c.b >= 0 && r == r && g == g && b == b);
	r *= c.r; g *= c.g; b *= c.b;
	return Color(r, g, b);
}

/**
 *  Multiplies each color component with the given number. The function behaves
 *  as is expected by a *= operation.
 *  @param t The non-negative real number t with which to divide the color with.
 */
Color Color::operator*=(float f)
{
	assert(f >= 0.f);
	r *= f; g *= f;	b *= f;
	return Color(r, g, b);
}

/**
 *  Divides each color component with the given number. The function behaves
 *  as is expected by a /= operation.
 *  @param t The non-negative real number t with which to divide the color with.
 */
Color Color::operator/=(float t)
{
	assert(t >= 0 && t == t);
	r /= t;	g /= t;	b /= t;
	return Color(r, g, b);
}

bool Color::operator!() const
{
	return (r == 0 && g == 0 && b == 0);
}

float Color::GetIntensity() const
{
	return sqrt(r*r + g*g + b*b);
}

float Color::GetMax() const
{
	return __max(r, __max(g, b));
}

float Color::GetAverage() const
{
	return (r + g + b)/3.0f;
}

float Color::GetSum() const
{
	return r + g + b;
}

float Color::GetLuminance() const
{
    // Rec. 709 would be 0.2126f*r + 0.7152f*g + 0.0722f*b, below is Rec. 601
    return 0.2989f*r + 0.5866f*g + 0.1145f*b;
}

void Color::SetLuminance(float L)
{
    float x = L / (0.2989f*r + 0.5866f*g + 0.1145f*b);
    r = r*x; g = g*x; b = b*x;
}

bool Color::IsValid() const
{
	return r == r && g == g && b == b && r >= 0 && 
		   g >= 0 && b >= 0 && 
		   r < std::numeric_limits<float>::infinity() && 
		   g < std::numeric_limits<float>::infinity() && 
		   b < std::numeric_limits<float>::infinity() &&
		   r > -std::numeric_limits<float>::infinity() &&
		   g > -std::numeric_limits<float>::infinity() &&
		   b > -std::numeric_limits<float>::infinity();
}

std::ostream& operator << (std::ostream& s , const Color& v)
{
	return(s << "(" << v.r << "," << v.g << "," << v.b << ")");
}

float& Color::operator[](int t)
{
	return (&r)[t];
}