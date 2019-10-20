#ifndef DETOUR_HXX_INCLUDED
#define DETOUR_HXX_INCLUDED

#include <cstdint>
#include <windows.h>

class Detour
{
    private:
        std::uint8_t* OrigFunc;
        std::uint8_t* HookFunc;
        std::uint8_t* HookJump;
        std::uint32_t* VTable;
        int JumpLength;

        void InsertJmp(std::uint8_t* jmp, std::uint8_t* src, std::uint8_t* dest, const int len);

    public:
        template<typename T>
        Detour(T OriginalFunction, T HookFunction, int JumpLength = 5);

        void Patch();
        void Remove();
        void PatchEx();
        void RemoveEx();
        bool IsPatched();

        template<typename T>
        T OriginalFunction();
};

template<typename T>
Detour::Detour(T OriginalFunction, T HookFunction, int JumpLength) : OrigFunc(reinterpret_cast<std::uint8_t*>(OriginalFunction)), HookFunc(reinterpret_cast<std::uint8_t*>(HookFunction)), JumpLength(JumpLength) {}

template<typename T>
T Detour::OriginalFunction()
{
    return reinterpret_cast<T>(this->HookJump);
}

#endif // DETOUR_HXX_INCLUDED
