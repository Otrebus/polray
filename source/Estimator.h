/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Estimator.h
 * 
 * Declaration of the Estimator base class.
 */

#pragma once

class Bytestream;
class Color;

class Estimator
{
public:
    virtual ~Estimator();

    virtual void AddSample(int, int, const Color& c) = 0;
    virtual Color GetEstimate(int, int) const = 0;

    static Estimator* Create(unsigned char n);

    int GetWidth() const;
    int GetHeight() const;

    virtual void Save(Bytestream& stream) const = 0;
    virtual void Load(Bytestream& stream) = 0;

protected:
    int height, width;
};
