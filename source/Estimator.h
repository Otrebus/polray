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
