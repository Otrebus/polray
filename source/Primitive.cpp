#include "Primitive.h"

/**
 * Constructor.
 */
Primitive::Primitive() : material(nullptr)
{
}

/**
 * Destructor.
 */
Primitive::~Primitive()
{
}

/**
 * Sets the material of the primitive.
 * @mat The material that constitutes the primitive.
 */
void Primitive::SetMaterial(Material* mat)
{
    material = mat;
}

/**
 * Returns the material of the primitive.
 */
Material* Primitive::GetMaterial() const
{
    return material;
}
