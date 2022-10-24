/**
 * Copyright (c) 2022 Peter Otrebus-Larsson (otrebus@gmail.com)
 * Distributed under GNU GPL v3. For full terms see the LICENSE file.
 * 
 * @file Bytestream.cpp
 * 
 * Definition of the Bytestream class used to serialize and deserialize data.
 */

#include "Bytestream.h"
#include <fstream>

/**
 * Constructor.
 */
Bytestream::Bytestream() : first(0), fail(false)
{
}

/**
 * Destructor.
 */
Bytestream::~Bytestream()
{
}

/**
 * Dumps the bytestream into a file.
 * 
 * @param fileName The name of the file to write to.
 */
void Bytestream::SaveToFile(std::string fileName)
{
    std::ofstream file;
    file.open(fileName, std::ios::out | std::ios::trunc | std::ios::binary);
    for(auto byte : data)
        file.write(&byte, sizeof(char));
}

/**
 * Loads the contents of a file into the bytestream.
 * 
 * @param fileName The name of the file to read from.
 */
void Bytestream::LoadFromFile(std::string fileName)
{
    std::ifstream file;
    file.open(fileName, std::ios::in | std::ios::binary);

    for(char byte; !file.eof(); data.push_back(byte))
        file.read(&byte, sizeof(char));
}
