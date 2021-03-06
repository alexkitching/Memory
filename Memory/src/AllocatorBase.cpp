#include "AllocatorBase.h"
#include "Debug.h"

AllocatorBase::AllocatorBase()
	:
m_pStart(nullptr),
m_capacity(0u)
,
#if DEBUG
m_allocationCount(0u),
#endif
m_usedSize(0u)
{
}

AllocatorBase::AllocatorBase(size_t a_capacity, void* a_pStart)
	:
m_pStart(a_pStart),
m_capacity(a_capacity),
#if DEBUG
m_allocationCount(0u),
#endif
m_usedSize(0u)
{
	
}

AllocatorBase::~AllocatorBase()
{
	m_pStart = nullptr;
	m_capacity = 0;
	m_usedSize = 0;
#if DEBUG
	ASSERT(m_allocationCount == 0 && m_usedSize == 0 &&
		"Memory Leak Warning! Allocator Destroyed despite existing allocations!");
#endif
}

bool AllocatorBase::CapacityWouldExceed(size_t a_size) const
{
	if (m_usedSize + a_size > m_capacity)
	{
		LOG("Heap: %s Failed Attempted Allocation of Size %u REASON - Exceeds Capacity")
			return true;
	}
	return false;
}

