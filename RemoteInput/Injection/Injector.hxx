//
// Created by Brandon on 2020-05-02.
//

#ifndef REMOTEINPUT_INJECTOR_HXX
#define REMOTEINPUT_INJECTOR_HXX

#include <utility>
#include <cstdint>
#include <string>
#include <sys/types.h>

class Injector
{
public:
    static bool Inject(std::string module_path, std::int32_t pid, void* bootstrap) noexcept;
};

#endif //REMOTEINPUT_INJECTOR_HXX
