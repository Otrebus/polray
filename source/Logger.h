#pragma once

#pragma warning( disable : 4996 )

#define LOG_FILENAME "debuglog"
#define NOMINMAX

#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <ctime>

typedef unsigned char uchar;

class Logger
{    
public:
    Logger(const std::string&);
    Logger();
    ~Logger();

    static void Box(const std::string&);
    void File(const std::string&);
private:
    std::string m_fileName;
};

extern Logger logger;
