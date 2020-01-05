#pragma once

#include "Types.h"

class AllocatorBase
{
public:
	AllocatorBase(size_t a_size, void* a_pStart);
	virtual ~AllocatorBase();

	// Throws Exceptions in Debug 
	void PreDispose();

	// Prevent Copies
	AllocatorBase(const AllocatorBase&) = delete;
	AllocatorBase& operator=(const AllocatorBase&) = delete;

	virtual void* allocate(size_t a_size, uint8 a_alignment = 4u) = 0;
	virtual void deallocate(void* a_p) = 0;

	void* GetStartAddress() const { return m_pStart;  }
	size_t GetMaxSize() const { return m_maxSize;  }
	
#if DEBUG
	size_t GetUsedMemory() const { return m_usedSize; }
	size_t GetAllocationCount() const { return m_allocationCount; }
#endif
protected:
	void* m_pStart;
	size_t m_maxSize;
	
#if DEBUG
	size_t m_allocationCount;
	size_t m_usedSize;
#endif
};