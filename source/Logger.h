#pragma once

#pragma warning( disable : 4996 )

#define LOG_FILENAME "debuglog"

#include <iostream>
#include <string>
#include <windows.h>
#include <fstream>
#include <ctime>

using namespace std;

typedef unsigned char uchar;

class Logger
{    
public:
    Logger(const string&);
    Logger();
    ~Logger();

    static void Box(const string&);
    void File(const string&);
private:
    string m_fileName;
};

extern Logger logger;
