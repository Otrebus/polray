#include "Bytestream.h"
#include "Model.h"
#include <fstream>

//template Bytestream& Bytestream::operator<<(const int& t);
//template Bytestream& Bytestream::operator>>(int& t);
//template Bytestream& Bytestream::operator<<(const char& t);
//template Bytestream& Bytestream::operator>>(char& t);
//template Bytestream& Bytestream::operator<<(const float& t);
//template Bytestream& Bytestream::operator>>(float& t);

//------------------------------------------------------------------------------
// Constructor.
//------------------------------------------------------------------------------
Bytestream::Bytestream() : first(0), fail(false)
{
}

//------------------------------------------------------------------------------
// Destructor.
//------------------------------------------------------------------------------
Bytestream::~Bytestream()
{
}

//------------------------------------------------------------------------------
// Returns true if a previous operation has failed and the error flag has not
// been cleared.
//------------------------------------------------------------------------------
bool Bytestream::HasFailed() const
{
    return fail;
}

//------------------------------------------------------------------------------
// Clears the error flag.
//------------------------------------------------------------------------------
void Bytestream::ClearFail()
{
    fail = false;
}

//------------------------------------------------------------------------------
// Returns the size, in bytes, of the data currently in the stream. This is the
// amount of data put into the stream minus the amount of data streamed from it.
//------------------------------------------------------------------------------
unsigned int Bytestream::GetSize() const
{
    return data.size() - first;
}

void Bytestream::SaveToFile(std::string fileName)
{
    std::ofstream file;
    file.open(fileName, ios::out | ios::trunc | ios::binary);

    char byte;
    for(auto i = data.begin() + first; i < data.end(); i++)
    {
        byte = *i;
        file.write(&byte, sizeof(char));
    }
}

void Bytestream::LoadFromFile(std::string fileName)
{
    std::ifstream file;
    file.open(fileName, ios::in | ios::binary);

    char byte;
    while(!file.eof())
    {
        char byte;
        file.read(&byte, sizeof(char));
        data.push_back(byte);
    }
}