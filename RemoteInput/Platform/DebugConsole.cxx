#include "DebugConsole.hxx"

#if defined(DEBUG)
#include <iostream>

DebugConsole::DebugConsole( noexcept
{
    inbuffer = std::cin.rdbuf();
    outbuffer = std::cout.rdbuf();
    errbuffer = std::cerr.rdbuf();
    input.open("CONIN$", std::ios::in);
    output.open("CONOUT$", std::ios::out);
    error.open("CONOUT$", std::ios::out);
    std::cin.rdbuf(input.rdbuf());
    std::cout.rdbuf(output.rdbuf());
    std::cerr.rdbuf(error.rdbuf());
}

DebugConsole::~DebugConsole() noexcept
{
    input.close();
    output.close();
    error.close();
    std::cin.rdbuf(inbuffer);
    std::cout.rdbuf(outbuffer);
    std::cerr.rdbuf(errbuffer);
    inbuffer = nullptr;
    outbuffer = nullptr;
    errbuffer = nullptr;
}
#endif