#ifndef TMEMORYMANAGER_HXX_INCLUDED
#define TMEMORYMANAGER_HXX_INCLUDED

#define DELPHI_CALLING_CONVENTION __attribute__((regparm(3)))

typedef struct
{
    std::uint32_t TotalAddressSpace;
    std::uint32_t TotalUncommited;
    std::uint32_t TotalCommited;
    std::uint32_t TotalAllocated;
    std::uint32_t TotalFree;
    std::uint32_t FreeSmall;
    std::uint32_t FreeBig;
    std::uint32_t Unused;
    std::uint32_t Overhead;
    std::uint32_t HeapErrorCode;
} THeapStatus;

typedef struct
{
    std::uintptr_t MaxHeapSize;
    std::uintptr_t MaxHeapUsed;
    std::uintptr_t CurrHeapSize;
    std::uintptr_t CurrHeapUsed;
    std::uintptr_t CurrHeapFree;
} TFPCHeapStatus;

typedef struct
{
    bool NeedLock;
    void* (*GetMem)(std::intptr_t size) DELPHI_CALLING_CONVENTION;
    std::intptr_t (*FreeMem)(void* &ptr) DELPHI_CALLING_CONVENTION;
    std::intptr_t (*FreeMemSize)(void* &ptr, std::intptr_t size) DELPHI_CALLING_CONVENTION;
    void* (*AllocMem)(std::intptr_t size) DELPHI_CALLING_CONVENTION;
    void* (*ReAllocMem)(void* &ptr, std::intptr_t size) DELPHI_CALLING_CONVENTION;
    std::intptr_t (*MemSize)(void* ptr) DELPHI_CALLING_CONVENTION;

    void (*InitThread)() DELPHI_CALLING_CONVENTION;
    void (*DoneThread)() DELPHI_CALLING_CONVENTION;
    void (*RelocateHeap)() DELPHI_CALLING_CONVENTION;
    THeapStatus (*GetHeapStatus)() DELPHI_CALLING_CONVENTION;
    TFPCHeapStatus (*GetFPCHeapStatus)() DELPHI_CALLING_CONVENTION;
} TMemoryManager;


typedef struct
{
    bool NeedLock;
    void* (*GetMem)(std::intptr_t size) DELPHI_CALLING_CONVENTION;
    std::intptr_t (*FreeMem)(void* &ptr) DELPHI_CALLING_CONVENTION;
    std::intptr_t (*FreeMemSize)(void* &ptr, std::intptr_t size) DELPHI_CALLING_CONVENTION;
    void* (*AllocMem)(std::intptr_t size) DELPHI_CALLING_CONVENTION;
    void* (*ReAllocMem)(void* &ptr, std::intptr_t size) DELPHI_CALLING_CONVENTION;
    std::intptr_t (*MemSize)(void* ptr) DELPHI_CALLING_CONVENTION;

    void (*InitThread)() DELPHI_CALLING_CONVENTION;
    void (*DoneThread)() DELPHI_CALLING_CONVENTION;
    void (*RelocateHeap)() DELPHI_CALLING_CONVENTION;
    THeapStatus (*GetHeapStatus)() DELPHI_CALLING_CONVENTION;
    TFPCHeapStatus (*GetFPCHeapStatus)() DELPHI_CALLING_CONVENTION;
} __attribute__((__packed__)) TCMemoryManager;

#endif // TMEMORYMANAGER_HXX_INCLUDED
