//
// Created by Brandon on 2023-03-19.
//

#ifndef MEMORYMAPSTREAM_HXX_INCLUDED
#define MEMORYMAPSTREAM_HXX_INCLUDED

#if defined(_WIN32) || defined(_WIN64)
#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#else
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#endif

#include <istream>
#include "ImageData.hxx"

class MemoryMapStream
{
public:
    /********************************************//**
     * @brief A set of flags that determine how the file is memory
     *
     * @param read open_mode - Determine if the memory map should be opened for reading
     * @param write open_mode - Determine if the memory map should be opened for writing
     * @param create open_mode - Determine if the memory map should be created if it doesn't already exist
     *
     ***********************************************/
    enum class open_mode: std::uint32_t
    {
        read = 0x01,
        write = 0x02,
        create = 0x04
    };

    MemoryMapStream(const std::string &path, std::size_t size, MemoryMapStream::open_mode mode);
    MemoryMapStream(MemoryMapStream&& other);
    MemoryMapStream(const MemoryMapStream& other) = delete;
    ~MemoryMapStream();

    ImageData& image_data();

    bool is_mapped() const noexcept;

    MemoryMapStream& operator = (MemoryMapStream&& other);
    MemoryMapStream& operator = (const MemoryMapStream& other) = delete;

private:
    bool owner;
    std::string path;
    void* pData;
    std::size_t pSize;
    MemoryMapStream::open_mode mode;
    std::unique_ptr<ImageData> data_stream;

    #if defined(_WIN32) || defined(_WIN64)
    HANDLE hFile;
    #else
    int hFile;
    #endif

    bool open();
    bool close();
    bool map();
    bool unmap();
};

MemoryMapStream::open_mode operator | (MemoryMapStream::open_mode a, MemoryMapStream::open_mode b);

#endif /* MEMORYMAPSTREAM_HXX_INCLUDED */
