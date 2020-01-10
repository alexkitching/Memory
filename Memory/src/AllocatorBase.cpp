#include "AllocatorBase.h"
#include "Debug.h"

AllocatorBase::AllocatorBase()
	:
m_pStart(nullptr),
m_capacity(0u),
m_allocationCount(0u),
m_usedSize(0u)
{
}

AllocatorBase::AllocatorBase(size_t a_capacity, void* a_pStart)
	:
m_pStart(a_pStart),
m_capacity(a_capacity),
m_allocationCount(0u),
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

