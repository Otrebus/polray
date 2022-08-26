#pragma once

#include "EnvironmentLight.h"
#include "Vector3d.h"

class Bytestream;
class Color;

class PreethamSky : public EnvironmentLight
{
public:
    PreethamSky(double phi, double theta, double T, double divisor);
    PreethamSky();
    virtual ~PreethamSky();

    void Init();

    Color GetRadiance(const Vector3d& dir) const;

    virtual void Save(Bytestream& stream) const;
    virtual void Load(Bytestream& stream);

private:
    static double Perez(double A, double B, double C, double D, double E, 
                       double theta, double gamma);

    Vector3d dir_sun_;

    double divisor_;

    double theta_, phi_, T_, Y_z_;
    double x_z_, y_z_;

    double A_Y_, B_Y_, C_Y_, D_Y_, E_Y_;
    double A_y_, B_y_, C_y_, D_y_, E_y_;
    double A_x_, B_x_, C_x_, D_x_, E_x_;

    double YDenom_, yDenom_, xDenom_;
};
