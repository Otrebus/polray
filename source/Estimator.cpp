/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Estimator.cpp
 * 
 * Implementation of the Estimator base class.
 */

#include "Estimator.h"
#include "MonEstimator.h"
#include "MeanEstimator.h"
#include "Bytestream.h"

/**
 * Destructor.
 */
Estimator::~Estimator()
{
}

/**
 * Creates an estimator given an id (see ByteStream.h).
 * 
 * @returns The created estimator.
 */
Estimator* Estimator::Create(unsigned char id)
{
    switch(id)
    {
    case ID_MONESTIMATOR:
        return new MonEstimator;
        break;
    case ID_MEANESTIMATOR:
        return new MeanEstimator;
        break;
    default:
        __debugbreak();
        return nullptr;
    }
}

/**
 * Returns the width of the buffer.
 * 
 * @returns The horizontal width of the buffer in terms of pixels.
 */
int Estimator::GetWidth() const
{
    return width;
}

/**
 * Returns the height of the buffer.
 * 
 * @returns The vertical span of the buffer in terms of pixels.
 */
int Estimator::GetHeight() const
{
    return height;
}