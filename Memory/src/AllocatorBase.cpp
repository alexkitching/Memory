#include "AllocatorBase.h"
#include "Debug.h"

AllocatorBase::AllocatorBase(size_t a_size, void* a_pStart)
	:
m_pStart(a_pStart),
m_maxSize(a_size),
m_usedSize(0)
{
	
}

AllocatorBase::~AllocatorBase()
{
	m_pStart = nullptr;
	m_maxSize = 0;
	m_usedSize = 0;
}

void AllocatorBase::PreDispose()
{
#if DEBUG
	ASSERT(m_allocationCount == 0 && m_usedSize == 0 &&
		"Memory Leak Warning! Allocator Destroyed despite existing allocations!");
#endif
}
