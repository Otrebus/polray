#include "Bytestream.h"
#include <fstream>

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
size_t Bytestream::GetSize() const
{
    return data.size() - first;
}

void Bytestream::SaveToFile(std::string fileName)
{
    std::ofstream file;
    file.open(fileName, std::ios::out | std::ios::trunc | std::ios::binary);
    for(auto byte : data)
        file.write(&byte, sizeof(char));
}

void Bytestream::LoadFromFile(std::string fileName)
{
    std::ifstream file;
    file.open(fileName, std::ios::in | std::ios::binary);

    for(char byte; !file.eof(); data.push_back(byte))
        file.read(&byte, sizeof(char));
}
