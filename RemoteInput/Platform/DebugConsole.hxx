#ifndef DEBUGCONSOLE_HXX_INCLUDED
#define DEBUGCONSOLE_HXX_INCLUDED

#if defined(_WIN32) || defined(_WIN64)
#include <fstream>
#include <streambuf>

class DebugConsole final
{
private:
    std::streambuf *inbuffer, *outbuffer, *errbuffer;
    std::fstream input, output, error;

public:
    DebugConsole();
    ~DebugConsole();
};
#endif

#endif // DEBUGCONSOLE_HXX_INCLUDED
