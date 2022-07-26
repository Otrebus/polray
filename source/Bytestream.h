#ifndef BYTESTREAM_H
#define BYTESTREAM_H

#include <vector>
#include <string>
#include "Model.h"

using namespace std;


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