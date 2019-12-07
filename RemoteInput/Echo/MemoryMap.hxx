//
//  MemoryMap.hxx
//  Echo
//
//  Created by Brandon on 2019-09-14.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef MEMORYMAP_HXX_INCLUDED
#define MEMORYMAP_HXX_INCLUDED

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <istream>

template<typename char_type>
class MemoryMap
{
private:
    #if defined(_WIN32) || defined(_WIN64)
    HANDLE hFile;
    HANDLE hMap;
    #else
    int hFile;
    bool physical;
    #endif
	bool owner;
    std::basic_string<char_type> path;
    void* pData;
    std::size_t pSize;
    std::ios_base::openmode mode;

public:
    explicit MemoryMap(const char_type* path, std::ios_base::openmode mode = std::ios::in | std::ios::out);
    explicit MemoryMap(const char_type* path, std::size_t size, std::ios_base::openmode mode = std::ios::in | std::ios::out);
    ~MemoryMap();

    bool open();
    bool open_file();
    bool map();
    bool unmap();
    bool close();
    bool is_open() const;
    bool is_mapped() const;
    std::size_t size() const;
    void* data() const;
	void flush() const;
    std::size_t granularity() const;
};

#if defined(_WIN32) || defined(_WIN64)
template<typename char_type>
MemoryMap<char_type>::MemoryMap(const char_type* path, std::ios_base::openmode mode) : hFile(INVALID_HANDLE_VALUE), hMap(nullptr), owner(false), path(path), pData(nullptr), pSize(0), mode(mode) {}

template<typename char_type>
MemoryMap<char_type>::MemoryMap(const char_type* path, std::size_t size, std::ios_base::openmode mode) : hFile(INVALID_HANDLE_VALUE), hMap(nullptr), owner(false), path(path), pData(nullptr), pSize(size), mode(mode) {}
#else
template<typename char_type>
MemoryMap<char_type>::MemoryMap(const char_type* path, std::ios_base::openmode mode) : hFile(0), physical(false), owner(false), path(path), pData(nullptr), pSize(0), mode(mode) {}

template<typename char_type>
MemoryMap<char_type>::MemoryMap(const char_type* path, std::size_t size, std::ios_base::openmode mode) : hFile(0), physical(false), owner(false), path(path), pData(nullptr), pSize(size), mode(mode) {}
#endif

template<typename char_type>
MemoryMap<char_type>::~MemoryMap()
{
    close();
}

template<typename char_type>
bool MemoryMap<char_type>::open()
{
    bool read_only = !(mode & std::ios::out);
    #if defined(_WIN32) || defined(_WIN64)
    DWORD dwCreation = (!read_only && pSize > 0) ? CREATE_ALWAYS : OPEN_EXISTING;
    DWORD dwAccess = read_only ? PAGE_READONLY : PAGE_READWRITE;

    if(dwCreation == CREATE_ALWAYS)
    {
		owner = true;
        hMap = std::is_same<char_type, wchar_t>::value ? CreateFileMappingW(hFile, nullptr, dwAccess, 0, pSize, reinterpret_cast<const wchar_t*>(path.c_str())) : CreateFileMappingA(hFile, nullptr, dwAccess, 0, pSize, reinterpret_cast<const char*>(path.c_str()));
        return hMap != nullptr;
    }

	owner = false;
    hMap = std::is_same<char_type, wchar_t>::value ? OpenFileMappingW(FILE_MAP_ALL_ACCESS, false, reinterpret_cast<const wchar_t*>(path.c_str())) : OpenFileMappingA(FILE_MAP_ALL_ACCESS, false, reinterpret_cast<const char*>(path.c_str()));
    return hMap != nullptr;
    #else
	owner = (!read_only && pSize > 0);
    int dwFlags = read_only ? O_RDONLY : O_RDWR;
    dwFlags |= (!read_only && pSize > 0) ? (O_CREAT | O_EXCL | O_TRUNC) : 0;
    hFile = shm_open(path.c_str(), dwFlags, S_IRWXU);
	if(hFile == -1)
	{
		if (errno == EEXIST)
		{
			owner = false;
			hFile = shm_open(path.c_str(), read_only ? O_RDONLY : O_RDWR, S_IRWXU);
		}
	}

    if(hFile != -1)
    {
        if(pSize <= 0)
        {
            struct stat info = {0};
            if (fstat(hFile, &info) != -1)
            {
                pSize = info.st_size;
            }
        }

        if(!read_only && pSize > 0)
        {
			if (owner)
			{
				if (ftruncate(hFile, pSize) != -1)
				{
					struct stat info = {0};
					return fstat(hFile, &info) != -1 ? info.st_size >= pSize : false;
				}
				return false;
			}
        }
		return true;
    }
    #endif
    return false;
}

template<typename char_type>
bool MemoryMap<char_type>::open_file()
{
    bool read_only = !(mode & std::ios::out);
    #if defined(_WIN32) || defined(_WIN64)
    DWORD dwAccess = read_only ? GENERIC_READ : (GENERIC_READ | GENERIC_WRITE);
    DWORD dwCreation = (!read_only && pSize > 0) ? CREATE_ALWAYS : OPEN_EXISTING;
    DWORD dwAttributes = read_only ? FILE_ATTRIBUTE_READONLY : FILE_ATTRIBUTE_TEMPORARY;
	owner = dwCreation != OPEN_EXISTING;

    hFile = std::is_same<char_type, wchar_t>::value ? CreateFileW(reinterpret_cast<const wchar_t*>(path.c_str()), dwAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, dwCreation, dwAttributes, nullptr) : CreateFileA(reinterpret_cast<const char*>(path.c_str()), dwAccess, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, dwCreation, dwAttributes, nullptr);

    if(hFile != INVALID_HANDLE_VALUE)
    {
        std::uint32_t high = (static_cast<std::uint64_t>(pSize) >> 32);
        std::uint32_t low = (static_cast<std::uint64_t>(pSize) & 0xFFFFFFFF);
        if((SetFilePointer(hFile, static_cast<long>(low), reinterpret_cast<long*>(&high), FILE_BEGIN) != INVALID_SET_FILE_POINTER))
        {
            if(!read_only && SetEndOfFile(hFile))
            {
                low = GetFileSize(hFile, reinterpret_cast<LPDWORD>(&high));
                if (low != INVALID_FILE_SIZE && ((static_cast<std::uint64_t>(high) << 32) | low) == pSize)
                {
                    DWORD dwCreation = (!read_only && pSize > 0) ? CREATE_ALWAYS : OPEN_EXISTING;
                    DWORD dwAccess = read_only ? PAGE_READONLY : PAGE_READWRITE;

                    if(dwCreation == CREATE_ALWAYS)
                    {
                        hMap = std::is_same<char_type, wchar_t>::value ? CreateFileMappingW(hFile, nullptr, dwAccess, 0, pSize, reinterpret_cast<const wchar_t*>(path.c_str())) : CreateFileMappingA(hFile, nullptr, dwAccess, 0, pSize, reinterpret_cast<const char*>(path.c_str()));
                        return hMap != nullptr;
                    }

                    hMap = std::is_same<char_type, wchar_t>::value ? OpenFileMappingW(FILE_MAP_ALL_ACCESS, false, reinterpret_cast<const wchar_t*>(path.c_str())) : OpenFileMappingA(FILE_MAP_ALL_ACCESS, false, reinterpret_cast<const char*>(path.c_str()));
                    return hMap != nullptr;
                }
                return false;
            }
            low = GetFileSize(hFile, reinterpret_cast<LPDWORD>(&high));
            pSize = low != INVALID_FILE_SIZE ? (static_cast<std::uint64_t>(high) << 32) | low : 0;
            if (low != INVALID_FILE_SIZE)
            {
                DWORD dwCreation = (!read_only && pSize > 0) ? CREATE_ALWAYS : OPEN_EXISTING;
                DWORD dwAccess = read_only ? PAGE_READONLY : PAGE_READWRITE;

                if(dwCreation == CREATE_ALWAYS)
                {
                    hMap = std::is_same<char_type, wchar_t>::value ? CreateFileMappingW(hFile, nullptr, dwAccess, 0, pSize, reinterpret_cast<const wchar_t*>(path.c_str())) : CreateFileMappingA(hFile, nullptr, dwAccess, 0, pSize, reinterpret_cast<const char*>(path.c_str()));
                    return hMap != nullptr;
                }

                hMap = std::is_same<char_type, wchar_t>::value ? OpenFileMappingW(FILE_MAP_ALL_ACCESS, false, reinterpret_cast<const wchar_t*>(path.c_str())) : OpenFileMappingA(FILE_MAP_ALL_ACCESS, false, reinterpret_cast<const char*>(path.c_str()));
                return hMap != nullptr;
            }
            return false;
        }
    }
    #else
    physical = true;
	owner = true;
    int dwFlags = read_only ? O_RDONLY : O_RDWR;
    dwFlags |= (!read_only && pSize > 0) ? (O_CREAT | O_EXCL | O_TRUNC) : 0;
    hFile = ::open(path.c_str(), dwFlags, S_IRWXU);
	if(hFile == -1 && errno == EEXIST)
	{
		owner = false;
		hFile = ::open(path.c_str(), read_only ? O_RDONLY : O_RDWR, S_IRWXU);
	}

    if(hFile != -1)
    {
        if(!read_only && pSize > 0 && ftruncate(hFile, pSize) != -1)
        {
            struct stat info = {0};
            return fstat(hFile, &info) != -1 ? pSize == info.st_size : false;
        }

        pSize = 0;
        struct stat info = {0};
        if(fstat(hFile, &info) != -1)
        {
            pSize = info.st_size;
            return true;
        }
    }
    #endif
    return false;
}

template<typename char_type>
bool MemoryMap<char_type>::map()
{
    bool read_only = !(mode & std::ios::out);
    #if defined(_WIN32) || defined(_WIN64)
    DWORD dwAccess = read_only ? FILE_MAP_READ : FILE_MAP_READ | FILE_MAP_WRITE;
    pData = MapViewOfFile(hMap, dwAccess, 0, 0, pSize);
    return pData != nullptr;
    #else
    int dwAccess = read_only ? PROT_READ : (PROT_READ | PROT_WRITE);
    pData = mmap(nullptr, pSize, dwAccess, MAP_SHARED, hFile, 0);
    return pData != MAP_FAILED;
    #endif
}

template<typename char_type>
bool MemoryMap<char_type>::unmap()
{
    bool result = true;
    #if defined(_WIN32) || defined(_WIN64)
    result = UnmapViewOfFile(pData);
    pData = nullptr;
    #else
    result = !munmap(pData, pSize);
    pData = nullptr;
    #endif
    return result;
}

template<typename char_type>
bool MemoryMap<char_type>::close()
{
    bool result = unmap();
    #if defined(_WIN32) || defined(_WIN64)
    result = CloseHandle(hMap) && result;
    hMap = nullptr;

    if (hFile != INVALID_HANDLE_VALUE)
    {
        result = CloseHandle(hFile) && result;
        hFile = INVALID_HANDLE_VALUE;
    }
    #else
	if (physical || !owner)
	{
		result = ::close(hFile) != -1 && result;
	}

	if (!physical && owner)
	{
		result = !shm_unlink(path.c_str()) && result;
		result = ::close(hFile) != -1 && result;
	}
    physical = false;
	owner = false;
    #endif
    return result;
}

template<typename char_type>
bool MemoryMap<char_type>::is_open() const
{
    #if defined(_WIN32) || defined(_WIN64)
    return hMap || (hFile != INVALID_HANDLE_VALUE);
    #else
    return hFile != nullptr;
    #endif
}

template<typename char_type>
bool MemoryMap<char_type>::is_mapped() const
{
    return pData;
}

template<typename char_type>
std::size_t MemoryMap<char_type>::size() const
{
    return pSize;
}

template<typename char_type>
void* MemoryMap<char_type>::data() const
{
    return pData;
}

template<typename char_type>
void MemoryMap<char_type>::flush() const
{
    #if defined(_WIN32) || defined(_WIN64)
    FlushViewOfFile(pData, pSize);
    #else
	msync(pData, pSize, MS_SYNC);
	#endif
}

template<typename char_type>
std::size_t MemoryMap<char_type>::granularity() const
{
    #if defined(_WIN32) || defined(_WIN64)
    SYSTEM_INFO info = {0};
    GetSystemInfo(&info);
    return info.dwAllocationGranularity;
    #else
    return sysconf(_SC_PAGESIZE);
    #endif
}

#endif /* MEMORYMAP_HXX_INCLUDED */
