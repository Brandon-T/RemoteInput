//
// Created by Brandon on 2023-03-19.
//

#include "MemoryMapStream.hxx"
#include "Platform.hxx"
#include "EIOS.hxx"

MemoryMapStream::open_mode operator | (MemoryMapStream::open_mode a, MemoryMapStream::open_mode b)
{
    return static_cast<MemoryMapStream::open_mode>(
            static_cast<std::underlying_type<MemoryMapStream::open_mode>::type>(a) |
            static_cast<std::underlying_type<MemoryMapStream::open_mode>::type>(b));
}

inline MemoryMapStream::open_mode operator & (MemoryMapStream::open_mode a, MemoryMapStream::open_mode b)
{
    return static_cast<MemoryMapStream::open_mode>(
            static_cast<std::underlying_type<MemoryMapStream::open_mode>::type>(a) &
            static_cast<std::underlying_type<MemoryMapStream::open_mode>::type>(b));
}

inline bool operator ! (MemoryMapStream::open_mode mode)
{
    return !static_cast<std::underlying_type<MemoryMapStream::open_mode>::type>(mode);
}

#if defined(_WIN32) || defined(_WIN64)
MemoryMapStream::MemoryMapStream(const std::string &path, std::size_t size, MemoryMapStream::open_mode mode) : owner(false), path(path), pData(nullptr), pSize(size), mode(mode), data_stream(nullptr), hFile(nullptr)
#else
MemoryMapStream::MemoryMapStream(const std::string &path, std::size_t size, MemoryMapStream::open_mode mode) : owner(false), path(path), pData(MAP_FAILED), pSize(size), mode(mode), data_stream(nullptr), hFile(-1)
#endif
{
    open();
    map();
}

MemoryMapStream::MemoryMapStream(MemoryMapStream&& other) : owner(other.owner), path(other.path), pData(other.pData), pSize(other.pSize), mode(other.mode), data_stream(std::move(other.data_stream)), hFile(other.hFile)
{
    other.owner = false;
    other.path = std::string();
    other.pSize = 0;
    other.mode = static_cast<MemoryMapStream::open_mode>(0);
    other.data_stream = nullptr;

    #if defined(_WIN32) || defined(_WIN64)
    other.pData = nullptr;
    other.hFile = nullptr;
    #else
    other.pData = nullptr;
    other.hFile = -1;
    #endif
}

MemoryMapStream::~MemoryMapStream()
{
    data_stream.reset();
    close();
}

MemoryMapStream& MemoryMapStream::operator = (MemoryMapStream&& other)
{
    data_stream.reset();
    close();
    std::swap(owner, other.owner);
    std::swap(path, other.path);
    std::swap(pData, other.pData);
    std::swap(pSize, other.pSize);
    std::swap(mode, other.mode);
    std::swap(data_stream, other.data_stream);
    std::swap(hFile, other.hFile);
    return *this;
}

bool MemoryMapStream::open()
{
    #if defined(_WIN32) || defined(_WIN64)
    if (hFile != nullptr)
    {
        return true;
    }
    #else
    if (hFile != -1)
    {
        return true;
    }
    #endif

    bool read_only = !(mode & MemoryMapStream::open_mode::write);
    bool open_only = !(mode & MemoryMapStream::open_mode::create);

    #if defined(_WIN32) || defined(_WIN64)
    DWORD dwAccess = read_only ? PAGE_READONLY : PAGE_READWRITE;
    if (open_only)
    {
        owner = false;
        hFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, false, path.c_str());
        return hFile != nullptr;
    }

    owner = true;
    hFile = CreateFileMappingA(INVALID_HANDLE_VALUE, nullptr, dwAccess, 0, pSize, path.c_str());  // GetLastError() == ERROR_ALREADY_EXISTS
    return hFile != nullptr;
    #else
    int dwFlags = read_only ? O_RDONLY : O_RDWR;

    if (open_only)
    {
        owner = false;
        hFile = shm_open(path.c_str(), dwFlags, S_IRWXU);
    }
    else
    {
        owner = true;
        hFile = shm_open(path.c_str(), dwFlags | O_CREAT | O_EXCL | O_TRUNC, S_IRWXU);
        if (errno == EEXIST)
        {
            owner = false;
            hFile = shm_open(path.c_str(), dwFlags, S_IRWXU);
        }
    }

    if (hFile != -1)
    {
        if (pSize == 0)
        {
            struct stat info = {0};
            if (fstat(hFile, &info) != -1)
            {
                pSize = info.st_size;
            }
        }

        if (!read_only && owner)
        {
            if (ftruncate(hFile, pSize) != -1)
            {
                struct stat info = {0};
                return fstat(hFile, &info) != -1 ? info.st_size >= static_cast<std::int64_t>(pSize) : false;
            }
            return false;
        }
        return true;
    }
    return false;
    #endif
}

bool MemoryMapStream::close()
{
    bool result = unmap();
    #if defined(_WIN32) || defined(_WIN64)
    result = CloseHandle(hFile) && result;
    hFile = nullptr;
    owner = false;
    #else
    if (owner)
    {
        result = !shm_unlink(path.c_str()) && result;
    }
    else
    {
        result = ::close(hFile) != -1 && result;
    }

    hFile = -1;
    owner = false;
    #endif
    return result;
}

bool MemoryMapStream::map()
{
    #if defined(_WIN32) || defined(_WIN64)
    if (pData != nullptr)
    {
        return true;
    }
    #else
    if (pData != MAP_FAILED)
    {
        return true;
    }
    #endif

    bool read_only = !(mode & MemoryMapStream::open_mode::write);
    #if defined(_WIN32) || defined(_WIN64)
    DWORD dwAccess = read_only ? FILE_MAP_READ : FILE_MAP_READ | FILE_MAP_WRITE;
    pData = MapViewOfFile(hFile, dwAccess, 0, 0, pSize);
    return pData != nullptr;
    #else
    int dwAccess = read_only ? PROT_READ : (PROT_READ | PROT_WRITE);
    pData = mmap(nullptr, pSize, dwAccess, MAP_SHARED, hFile, 0);
    return pData != MAP_FAILED;
    #endif
}

bool MemoryMapStream::unmap()
{
    bool result = true;
    if (pData)
    {
        #if defined(_WIN32) || defined(_WIN64)
        result = UnmapViewOfFile(pData);
        pData = nullptr;
        #else
        result = !munmap(pData, pSize);
        pData = nullptr;
        #endif
    }
    return result;
}

ImageData& MemoryMapStream::image_data()
{
    if (!open() && !map())
    {
        throw std::runtime_error("MemoryStreamMap: Cannot create image_data from closed map");
    }

    if (!data_stream)
    {
        data_stream = std::make_unique<ImageData>(pData, pSize);
    }

    return *data_stream;
}

bool MemoryMapStream::is_mapped() const noexcept
{
    #if defined(_WIN32) || defined(_WIN64)
    return pData != nullptr;
    #else
    return pData != MAP_FAILED;
    #endif
}