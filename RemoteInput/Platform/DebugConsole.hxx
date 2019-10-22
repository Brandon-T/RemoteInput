#ifndef DEBUGCONSOLE_HXX_INCLUDED
#define DEBUGCONSOLE_HXX_INCLUDED

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

#endif // DEBUGCONSOLE_HXX_INCLUDED
