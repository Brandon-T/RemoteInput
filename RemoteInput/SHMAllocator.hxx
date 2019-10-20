//
//  SHMAllocator.hxx
//  RemoteInput
//
//  Created by Brandon on 2019-09-14.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef SHMALLOCATOR_HXX_INCLUDED
#define SHMALLOCATOR_HXX_INCLUDED

#include <cmath>
#include <memory>

template<typename T>
struct SharedMemoryAllocator
{
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

    void* memory;
    std::size_t size;


    SharedMemoryAllocator(void* memory, std::size_t size) noexcept : memory(memory), size(size) {};
    SharedMemoryAllocator(const SharedMemoryAllocator& other) noexcept : memory(other.memory), size(other.size) {};

    template<typename U>
    SharedMemoryAllocator(const SharedMemoryAllocator<U>& other) noexcept : memory(other.memory), size(other.size) {};

    template<typename U>
    SharedMemoryAllocator& operator = (const SharedMemoryAllocator<U>& other) { return *this; }
    SharedMemoryAllocator<T>& operator = (const SharedMemoryAllocator& other) { return *this; }
    ~SharedMemoryAllocator() {}

    pointer address(reference value) const {return memory;}
    const_pointer address(const_reference value) const {return value;}

    pointer allocate(size_type n, const void* hint = 0) {return static_cast<T*>(memory);}
    void deallocate(T* ptr, size_type n) {}

    template<typename U, typename... Args>
    void construct(U* ptr, Args&&... args) {::new(static_cast<void*>(ptr)) U(std::forward<Args>(args)...);}
    void construct(pointer ptr, const T& val) {new(static_cast<T*>(ptr)) T(val);}

    template<typename U>
    void destroy(U* ptr) {ptr->~U();}
    void destroy(pointer ptr) {ptr->~T();}

    size_type max_size() const {return size / sizeof(T);}

    template<typename U>
    struct rebind {typedef SharedMemoryAllocator<U> other;};
};

template <typename T, typename U>
inline bool operator == (const SharedMemoryAllocator<T>& a, const SharedMemoryAllocator<U>& b)
{
    return (a == b);
}

template <typename T, typename U>
inline bool operator != (const SharedMemoryAllocator<T>& a, const SharedMemoryAllocator<U>& b)
{
    return !(a == b);
}

template<typename T>
class offset_ptr
{
private:
    std::ptrdiff_t offset;

public:
    offset_ptr() : offset(0) {}
    offset_ptr(T* ptr) : offset(ptr ? reinterpret_cast<char*>(ptr) - reinterpret_cast<char*>(this) : 0) {}

    T* get() {return reinterpret_cast<T*>(reinterpret_cast<char*>(this) + offset);}
    const T* get() const {return reinterpret_cast<T*>(reinterpret_cast<char*>(this) + offset);}

    T* operator->() {return get();}
    const T* operator->() const {return get();}

    T& operator*() {return *get();}
    const T& operator*() const {return *get();}

    operator T*() {return get();}
    operator T*() const {return get();}

    offset_ptr<T>& operator = (T* ptr) {offset = ptr ? reinterpret_cast<char*>(ptr) - reinterpret_cast<char*>(this) : 0; return *this;}
};

#endif /* SHMALLOCATOR_HXX_INCLUDED */
