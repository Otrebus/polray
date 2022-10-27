/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Vertex3d.cpp
 * 
 * Implementation of the Vertex3d class.
 */

#include "vertex3d.h"

/**
 * Constructor.
 */
Vertex3d::Vertex3d()
{
}

/**
 * Constructor.
 * 
 * @param pos The position of the vertex.
 * @param normal The normal of the vertex.
 * @param texpos The texture position of the vertex.
 */
Vertex3d::Vertex3d(const Vector3d& pos, const Vector3d& normal, const Vector2d& texpos) : pos(pos), normal(normal), texpos(texpos)
{
}

/**
 * Constructor, creates a vertex from a vector.
 * 
 * @parameter v The vector from which to copy the position from.
 */
Vertex3d::Vertex3d(const Vector3d& v) : pos(v), normal(0, 0, 0)
{
}

/**
 * Destructor.
 */
Vertex3d::~Vertex3d()
{
}
