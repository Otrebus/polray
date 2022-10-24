/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Primitive.cpp
 * 
 * Implementation of the Primitive base class.
 */

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
