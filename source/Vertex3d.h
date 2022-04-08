#ifndef VERTEX3D_H
#define VERTEX3D_H

#include "Vector3d.h"

class Vertex3d
{
public:
    Vertex3d(float, float, float, float, float, float, float, float);
    Vertex3d(const Vector3d& pos, const Vector3d& norm, const Vector2d& tex);
    Vertex3d(const Vector3d&);
    Vertex3d(float x, float y, float z);
    Vertex3d();
    ~Vertex3d();
    //Vector3d padding[100];
    Vector3d pos;
    Vector3d normal;
    Vector2d texpos;
    //Vector3d padding2[100];
};

#endif
