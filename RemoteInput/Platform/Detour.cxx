#include "Detour.hxx"

void Detour::InsertJmp(std::uint8_t* jmp, std::uint8_t* src, std::uint8_t* dest, const int len)
{
    *jmp = 0xE9;
    *reinterpret_cast<long*>(jmp + 1) = static_cast<long>(src + len - dest);
}

void Detour::Patch()
{
    DWORD dwProt = 0;
    std::uint8_t* jmp = new std::uint8_t[this->JumpLength + 5];
    VirtualProtect(this->OrigFunc, this->JumpLength, PAGE_READWRITE, &dwProt);
    memcpy(jmp, this->OrigFunc, this->JumpLength);

    jmp += this->JumpLength;
    jmp[0] = 0xE9;
    *reinterpret_cast<DWORD*>(jmp + 1) = static_cast<DWORD>(this->OrigFunc + this->JumpLength - jmp) - 5;
    memset(this->OrigFunc, 0x90, this->JumpLength);

    this->OrigFunc[0] = 0xE9;
    *reinterpret_cast<DWORD*>(this->OrigFunc + 1) = static_cast<DWORD>(this->HookFunc - this->OrigFunc) - 5;
    VirtualProtect(this->OrigFunc, this->JumpLength, dwProt, &dwProt);
    this->HookJump = (jmp - this->JumpLength);
}

void Detour::Remove()
{
    DWORD dwProt = 0;
    VirtualProtect(this->OrigFunc, this->JumpLength, PAGE_READWRITE, &dwProt);
    memcpy(this->OrigFunc, reinterpret_cast<std::uint8_t*>(this->HookJump), this->JumpLength);
    VirtualProtect(this->OrigFunc, this->JumpLength, dwProt, &dwProt);
}

void Detour::PatchEx()
{
    #ifndef __x86_64
    this->HookJump = new std::uint8_t[7];
    const static std::uint8_t jmp[] = {0xb8, 0x00, 0x00, 0x00, 0x00, 0xff, 0xe0}; /** movl $0x0, %eax ;;; jmp *%eax **/
    #else
    const static std::uint8_t jmp[] = {0x48, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xe0, 0x90, 0x90}; /** movq $0x0, %rax ;;; jmp *%rax **/
    this->HookJump = new std::uint8_t[15];
    #endif
    DWORD dwProtect = 0;
    const static std::int8_t jmp_size = sizeof(jmp) / sizeof(std::uint8_t);
    VirtualProtect(this->OrigFunc, jmp_size, PAGE_EXECUTE_READWRITE, &dwProtect);
    memcpy(this->HookJump, this->OrigFunc, jmp_size);
    memcpy(this->OrigFunc, jmp, jmp_size);
    #ifndef __x86_64
    memcpy(this->OrigFunc + 1, &this->HookFunc, sizeof(void*));
    #else
    memcpy(this->OrigFunc + 2, &this->HookFunc, sizeof(void*));
    #endif
    VirtualProtect(this->OrigFunc, jmp_size, dwProtect, &dwProtect);
}

void Detour::RemoveEx()
{
    #ifndef __x86_64
    const static std::int8_t jmp_size = 7;
    #else
    const static std::int8_t jmp_size = 14;
    #endif
    DWORD dwProtect = 0;
    VirtualProtect(this->OrigFunc, jmp_size, PAGE_EXECUTE_READWRITE, &dwProtect);
    memcpy(this->OrigFunc, this->HookJump, jmp_size);
    VirtualProtect(this->OrigFunc, jmp_size, dwProtect, &dwProtect);
    delete[] this->HookJump;
    this->HookJump = nullptr;
}

bool Detour::IsPatched()
{
    #ifndef __x86_64
    return this->OrigFunc[0] == 0xE9;
    #else
    return this->HookJump != nullptr && this->HookJump[0] == 0x48;
    #endif
}
