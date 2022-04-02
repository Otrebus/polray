#ifndef PREETHAMSKY_H
#define PREETHAMSKY_H

#include "EnvironmentLight.h"
#include "Vector3d.h"

class Bytestream;
class Color;

#define M_PI 3.141592654f

class PreethamSky : public EnvironmentLight
{
public:
    PreethamSky(float phi, float theta, float T, float divisor);
    PreethamSky();
    virtual ~PreethamSky();

    void Init();

    Color GetRadiance(const Vector3d& dir) const;

    virtual void Save(Bytestream& stream) const;
    virtual void Load(Bytestream& stream);

private:
    static float Perez(float A, float B, float C, float D, float E, 
                       float theta, float gamma);

    Vector3d dir_sun_;

    float divisor_;

    float theta_, phi_, T_, Y_z_;
    float x_z_, y_z_;

    float A_Y_, B_Y_, C_Y_, D_Y_, E_Y_;
    float A_y_, B_y_, C_y_, D_y_, E_y_;
    float A_x_, B_x_, C_x_, D_x_, E_x_;

    float YDenom_, yDenom_, xDenom_;
};

#endif