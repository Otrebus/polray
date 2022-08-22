#include "Estimator.h"
#include "MonEstimator.h"

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
    default:
        __debugbreak();
        return nullptr;
    }
}

int Estimator::GetWidth() const {
    return width;
}

int Estimator::GetHeight() const {
    return height;
}