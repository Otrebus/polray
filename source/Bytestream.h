#ifndef BYTESTREAM_H
#define BYTESTREAM_H

#include <vector>
#include <string>
#include "Model.h"

using namespace std;

#define ID_MONESTIMATOR ((unsigned char) 70)
#define ID_MEANESTIMATOR ((unsigned char) 71)

#define ID_PREETHAMSKY ((unsigned char)50)

#define ID_PINHOLECAMERA ((unsigned char)199)
#define ID_THINLENSCAMERA ((unsigned char)198)

#define ID_AREALIGHT ((unsigned char)200)
#define ID_SPHERELIGHT ((unsigned char)201)
#define ID_LIGHTPORTAL ((unsigned char)202)
#define ID_UNIFORMENVIRONMENTLIGHT ((unsigned char)203)
#define ID_MESHLIGHT ((unsigned char)204)

#define ID_ASHIKHMINSHIRLEY ((unsigned char)104)
#define ID_DIELECTRICMATERIAL ((unsigned char)102)
#define ID_MIRRORMATERIAL ((unsigned char)103)
#define ID_PHONGMATERIAL ((unsigned char)101)
#define ID_LAMBERTIANMATERIAL ((unsigned char)100)
#define ID_EMISSIVEMATERIAL ((unsigned char)105)
#define ID_COOKTORRANCEMATERIAL ((unsigned char)106)

#define ID_TRIANGLEMESH ((unsigned char)1)
#define ID_TRIANGLE ((unsigned char)2)
#define ID_SPHERE ((unsigned char)3)

#define ID_PATHTRACER ((char) 50)
#define ID_LIGHTTRACER ((char) 51)
#define ID_BDPT ((char) 52)
#define ID_RAYTRACER ((char) 53)

class Bytestream
{
public:
    Bytestream();
    ~Bytestream();

    //--------------------------------------------------------------------------
    // Appends data to the end of the stream.
    //--------------------------------------------------------------------------
    template<typename T> Bytestream& operator<<(const T& t)
    {
        char* p = (char*)&t;

        for(int i = 0; i < sizeof(T); i++)
            data.push_back(p[i]);

        if(data.size() - first < first)
        {   // The empty space at the beginning of the vector has grown too
            // large so push the data to the beginning of the vector
            for(unsigned int i = 0; i < data.size() - first; i++)
                data[i] = data[first + i];
        }
        return *this;
    }

    //--------------------------------------------------------------------------
    // Extracts data from the stream, treated as the type of the variable 
    // specified.
    //--------------------------------------------------------------------------
    template<typename T> Bytestream& operator>>(T& t)
    {
        char* p = (char*)&t;

        if(first + sizeof(t) > data.size())
        {
            fail = true;
            return *this;
        }

        for(int i = 0; i < sizeof(t); i++)
            p[i] = data[first + i];
        first += sizeof(t);
        return *this;
    }

    size_t GetSize() const;
    void ClearFail();
    bool HasFailed() const;

    void SaveToFile(std::string fileName);
    void LoadFromFile(std::string fileName);
private:

    bool fail;
    unsigned int first;
    vector<char> data;
};

#endif