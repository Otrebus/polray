//------------------------------------------------------------------------------
// File: Logger.cpp
//  
// Handles debug and error messages through logging and other means.
//------------------------------------------------------------------------------

#include "Logger.h"

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
Logger::Logger()
{
}

//------------------------------------------------------------------------------
// Constructor
//------------------------------------------------------------------------------
Logger::Logger(const string& fileName)
{
    m_fileName = fileName;
}

//------------------------------------------------------------------------------
// Destructor
//------------------------------------------------------------------------------
Logger::~Logger()
{
}

//------------------------------------------------------------------------------
// Shows a message box with a text message.
//------------------------------------------------------------------------------
void Logger::Box(const string& msg)
{
    std::wstring ws(msg.begin(), msg.end());
    MessageBox(0, ws.c_str(), (LPCWSTR) L"Error", MB_OK | MB_ICONERROR);
}

//------------------------------------------------------------------------------
// Appends a string with the timestamp and the given message to the file with
// name given by m_fileName.
//------------------------------------------------------------------------------
void Logger::File(const string& msg)
{
    ofstream file;
    time_t currentTime;
    time(&currentTime);
    char timestamp[ 100 ];

    file.open (m_fileName.c_str(), ofstream::app);
    if(file.fail())
    {
        Box("Could not open debug log file.");
        exit(1);
    }
    
    sprintf( timestamp, "%s", ctime( &currentTime ) );
    timestamp[ strlen( timestamp ) - 1 ] = 32;

    file << timestamp << ":: " << msg << endl;
    file.close();
}