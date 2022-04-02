#include "Bytestream.h"
#include "Color.h"
#include "PreethamSky.h"

#include <cmath>

PreethamSky::PreethamSky(float phi, float theta, float T, float divisor)
    : phi_(phi*M_PI/180.0f), theta_(theta*M_PI/180.0f), T_(T), divisor_(divisor)
{
    Init();
}

PreethamSky::PreethamSky()
{
}

void PreethamSky::Init()
{
    float t = theta_;
    float t2 = theta_*theta_;
    float t3 = t2*theta_;

    float xz0, xz1, xz2, yz0, yz1, yz2;

    dir_sun_ = Vector3d(cos(phi_), cos(theta_), sin(phi_));
    dir_sun_.Normalize();

    Y_z_ = (4.0453f*T_-4.9710f)*tan((4.0f/9.0f-T_/120.0f)*(M_PI-2*theta_))
           - 0.2155f*T_ + 2.4192;

    xz0 = t3*0.00166f  + t2*-0.00375f + t*0.00209f;
    xz1 = t3*-0.02903f + t2*0.06377f  + t*-0.03203f + 0.00394f;
    xz2 = t3*0.11693f  + t2*-0.21196f + t*0.06052f  + 0.25886f;
    x_z_ = xz0*T_*T_ + xz1*T_ + xz2;

    yz0 = t3*0.00275f  + t2*-0.00610f + t*0.00317f;
    yz1 = t3*-0.04214f + t2*0.08970f  + t*-0.04153f + 0.00516f;
    yz2 = t3*0.15346f  + t2*-0.26756  + t*0.06670f  + 0.26688f;
    y_z_ = yz0*T_*T_ + yz1*T_ + yz2;

    A_Y_ = T_*0.1787f  - 1.4630f;
    B_Y_ = T_*-0.3554f + 0.4275f;
    C_Y_ = T_*-0.0227f + 5.3251f;
    D_Y_ = T_*0.1206f  - 2.5771f;
    E_Y_ = T_*-0.0670f + 0.3703f;

    A_x_ = T_*-0.0193f - 0.2592f;
    B_x_ = T_*-0.0665f + 0.0008f;
    C_x_ = T_*-0.0004f + 0.2125f;
    D_x_ = T_*-0.0641f - 0.8989f;
    E_x_ = T_*-0.0033f + 0.0452f;

    A_y_ = T_*-0.0167f - 0.2608f;
    B_y_ = T_*-0.0950f + 0.0092f;
    C_y_ = T_*-0.0079f + 0.2102f;
    D_y_ = T_*-0.0441f - 1.6537f;
    E_y_ = T_*-0.0109f + 0.0529f;

    YDenom_ = Perez(A_Y_, B_Y_, C_Y_, D_Y_, E_Y_, 0, theta_);
    xDenom_ = Perez(A_x_, B_x_, C_x_, D_x_, E_x_, 0, theta_);
    yDenom_ = Perez(A_y_, B_y_, C_y_, D_y_, E_y_, 0, theta_);
}

float PreethamSky::Perez(float A, float B, float C, float D, float E, 
                   float theta, float gamma)
{
    float cosgamma2 = cos(gamma)*cos(gamma);
    float sdf = (1 + A*exp(B/cos(theta)))*(1 + C*exp(D*gamma) + E*cosgamma2);
    return sdf;
}

PreethamSky::~PreethamSky()
{
}

Color PreethamSky::GetRadiance(const Vector3d& viewDir) const
{
    Vector3d dir = viewDir;
    dir.Normalize();
    
    //float phi_dir = atan2(dir.z, dir.x);
    //float theta_dir = acos(dir.z);
    float sdf = dir*dir_sun_;
    float gamma = acos(dir*dir_sun_);

    float Y_Yxy = Y_z_*Perez(A_Y_, B_Y_, C_Y_, D_Y_, E_Y_, theta_, gamma)/YDenom_;
    float y_Yxy = y_z_*Perez(A_y_, B_y_, C_y_, D_y_, E_y_, theta_, gamma)/yDenom_;
    float x_Yxy = x_z_*Perez(A_x_, B_x_, C_x_, D_x_, E_x_, theta_, gamma)/xDenom_;

    float X_XYZ = x_Yxy*Y_Yxy/y_Yxy;
    float Y_XYZ = Y_Yxy;
    float Z_XYZ = (1 - x_Yxy - y_Yxy)*Y_Yxy/y_Yxy;

    float R = 3.240479f*X_XYZ  - 1.53715*Y_XYZ  - 0.49853*Z_XYZ;
    float G = -0.969256f*X_XYZ + 1.875991*Y_XYZ + 0.041556*Z_XYZ;
    float B = 0.055647f*X_XYZ  - 0.204043*Y_XYZ + 1.057311*Z_XYZ;

    return Color(R, G, B)/divisor_;
}

void PreethamSky::Save(Bytestream& b) const
{
    b << ID_PREETHAMSKY << phi_ << theta_ << T_;
}

void PreethamSky::Load(Bytestream& b)
{
    b >> phi_ >> theta_ >> T_;
    Init();
}

