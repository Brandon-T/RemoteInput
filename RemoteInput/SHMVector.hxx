//
//  SHMVector.hxx
//  RemoteInput
//
//  Created by Brandon on 2019-09-14.
//  Copyright © 2019 XIO. All rights reserved.
//

#ifndef SHMVECTOR_HXX_INCLUDED
#define SHMVECTOR_HXX_INCLUDED

#include <memory>
#include "SHMAllocator.hxx"

template<typename T, typename Allocator = std::allocator<T>>
class CVector
{
private:
    offset_ptr<T> pMemory; //reinterpret_cast<char*>(this) + sizeof(*this))
    std::size_t pSize;
    std::size_t pCapacity;
    allocator pAlloc;

public:
    CVector() : pMemory(0), pSize(0), pCapacity(0), pAlloc(Allocator()) {}
    CVector(const allocator &alloc) : pMemory(0), pSize(0), pCapacity(0), pAlloc(alloc) {}
    ~CVector() {}

    std::size_t size() {return pSize;}
    std::size_t size() const {return pSize;}
    std::size_t capacity() {return pCapacity;}
    std::size_t capacity() const {return pCapacity;}

	void reserve(std::size_t size);
	void push_back(T&& value);
	T pop_back();
	T& operator[](std::size_t index);
	T& operator[](std::size_t index) const;
};

template<typename T, typename Allocator>
void CVector<T, Allocator>::reserve(std::size_t size)
{
	if(pCapacity < size)
	{
		pCapacity = size;
		std::uint64_t offsize = (sizeof(T) <= sizeof(*this) ? std::ceil(sizeof(*this) * 1.0f / sizeof(T)) : std::ceil(sizeof(T) * 1.0f / sizeof(*this)));

		T* ptr = reinterpret_cast<T*>(reinterpret_cast<char*>(pAlloc.allocate(pCapacity + offsize)) + offsize);

		if (pMemory && ptr)
		{
			for (std::size_t i = 0; i < this->pSize; ++i)
			{
				pAlloc.construct(ptr + i, *(pMemory + i));
				pAlloc.destroy(pMemory + i);
			}
		}

		pMemory = ptr;
	}
}

template<typename T, typename Allocator>
void CVector<T, Allocator>::push_back(T&& value)
{
	if(pCapacity == 0)
	{
		reserve(1);
	}

	if(pSize >= pCapacity)
	{
		reserve(pCapacity * 2);
	}

	pAlloc.construct(pMemory + pSize++, value);
}

template<typename T, typename Allocator>
T& CVector<T, Allocator>::operator[](std::size_t index)
{
	return *(pMemory + index);
}

template<typename T, typename Allocator>
T& CVector<T, Allocator>::operator[](std::size_t index) const
{
	return *(pMemory + index);
}

#endif /* SHMVECTOR_HXX_INCLUDED */
