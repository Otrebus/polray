#include "Model.h"
#include "Triangle.h"
#include "TriangleMesh.h"
#include "Sphere.h"
#include "Bytestream.h"

/**
 * Constructor.
 */
Model::Model()
{
}

/**
 * Destructor.
 */
Model::~Model()
{
}

/**
 * Creates a model given an id (see Bytestream.h).
 */
Model* Model::Create(unsigned char id)
{
    switch(id)
    {
    case ID_TRIANGLEMESH:
        return new TriangleMesh;
        break;
    case ID_TRIANGLE:
        return new Triangle;
        break;
    case ID_SPHERE:
        return new Sphere;
        break;
    default:
        return 0;
    }
}
