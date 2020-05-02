#ifndef DEBUGCONSOLE_HXX_INCLUDED
#define DEBUGCONSOLE_HXX_INCLUDED

#if defined(DEBUG)
#include <fstream>
#include <streambuf>

class DebugConsole final
{
private:
    std::streambuf *inbuffer, *outbuffer, *errbuffer;
    std::fstream input, output, error;

public:
    DebugConsole() noexcept;
    ~DebugConsole() noexcept;
};
#endif

#endif // DEBUGCONSOLE_HXX_INCLUDED
