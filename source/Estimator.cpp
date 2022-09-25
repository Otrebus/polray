#include "Estimator.h"
#include "MonEstimator.h"
#include "MeanEstimator.h"
#include "Bytestream.h"

Estimator::~Estimator()
{
}

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

int Estimator::GetWidth() const
{
    return width;
}

int Estimator::GetHeight() const
{
    return height;
}