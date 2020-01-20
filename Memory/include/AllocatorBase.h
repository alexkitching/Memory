#pragma once

#include "Types.h"
#include "Platform.h"

//------------
// Description
//--------------
// Base Allocator Class containing main allocate/deallocate interface for all allocators.
//------------

class AllocatorBase
{
public:
	AllocatorBase();
	AllocatorBase(size_t a_capacity, void* a_pStart);
	virtual ~AllocatorBase();

	// Prevent Copies
	AllocatorBase(const AllocatorBase&) = delete;
	AllocatorBase& operator=(const AllocatorBase&) = delete;

	// Primary Allocator Interface
	virtual void* allocate(size_t a_size, uint8 a_alignment = DEFAULT_ALIGNMENT) = 0;
	virtual void deallocate(void* a_pBlock) = 0;

	// Properties
	void* GetStartAddress() const { return m_pStart;  }
	size_t GetCapacity() const { return m_capacity;  }

	size_t GetUsedMemory() const { return m_usedSize; }
#if DEBUG
	size_t GetAllocationCount() const { return m_allocationCount; }
#endif
protected:

	virtual inline bool CapacityWouldExceed(size_t a_size) const final;
	
	void* m_pStart;
	size_t m_capacity;
	
#if DEBUG
	size_t m_allocationCount;
#endif
	size_t m_usedSize;
};
