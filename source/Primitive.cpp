#include "Primitive.h"

Primitive::Primitive() : material(nullptr)
{
}

Primitive::~Primitive()
{
}

void Primitive::SetMaterial(Material* mat)
{
    material = mat;
}

Material* Primitive::GetMaterial() const
{
    return material;
}