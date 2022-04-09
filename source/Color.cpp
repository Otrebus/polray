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

int Color::GetInt() const
{
    return (int)(b*255) | (int)(g*255) << 8 | (int)(r*255) << 16;
}


Color::Color(float _r, float _g, float _b) : r(_r), g(_g), b(_b)
{
    assert(IsValid());
}

Color::~Color()
{
}

Color Color::operator+(const Color& v) const
{
    return Color(r+v.r, g+v.g, b+v.b);
}

Color Color::operator-(const Color& v) const
{
    return Color(r-v.r, g-v.g, b-v.b);
}

Color Color::operator*(float t) const
{
    return Color(r*t, g*t, b*t);
}

Color Color::operator*(int t) const
{
    return Color(r*float(t), g*float(t), b*float(t));
}

Color operator*(float t, const Color& c)
{
    return Color(c.r*t, c.g*t, c.b*t);
}

Color operator*(int t, const Color& c)
{
    return Color(c.r*float(t), c.g*float(t), c.b*float(t));
}

Color Color::operator/(float t) const
{
    return Color(r/t, g/t, b/t);
}

Color Color::operator/(int t) const
{
    return Color(r/float(t), g/float(t), b/float(t));
}

Color Color::operator*(const Color& v) const
{
    return Color(r*v.r, g*v.g, b*v.b);
}


Color Color::operator+=(const Color& c)
{
    r+=c.r;
    g+=c.g;
    b+=c.b;

    return Color(r, g, b);
}

Color Color::operator*=(const Color& c)
{
    r *= c.r; g *= c.g; b *= c.b;
    return Color(r, g, b);
}

Color Color::operator*=(float f)
{
    r *= f; g *= f;	b *= f;
    return Color(r, g, b);
}

Color Color::operator/=(float t)
{
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
    return std::isfinite(r) && std::isfinite(g) && std::isfinite(b) && r >= 0 && g >= 0 && b >= 0;
}

std::ostream& operator << (std::ostream& s , const Color& v)
{
    return(s << "(" << v.r << "," << v.g << "," << v.b << ")");
}

float& Color::operator[](int t)
{
    return (&r)[t];
}