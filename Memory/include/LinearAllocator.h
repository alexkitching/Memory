#pragma once

#include "AllocatorBase.h"

//------------
// Description
//--------------
// Linear Allocator class, most basic form of allocator, works by bumping a pointer after each allocation.
//------------

class LinearAllocator : public AllocatorBase
{
public:

	LinearAllocator() = default;
	LinearAllocator(size_t a_capacity, void* a_pStart);

	LinearAllocator& operator=(LinearAllocator&& other) noexcept
	{
		m_pStart = other.m_pStart;
		m_capacity = other.m_capacity;
		m_allocationCount = other.m_capacity;
		m_usedSize = other.m_usedSize;
		m_pCurrentPos = other.m_pCurrentPos;
		return *this;
	}

	// Allocator Interface
	virtual void* allocate(size_t a_size, uint8 a_alignment) override;
	virtual void deallocate(void* a_pBlock) override;

	// Resets the Current Position Ptr
	void Clear();

private:

	void* m_pCurrentPos;

};