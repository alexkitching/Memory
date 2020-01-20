#pragma once
#include "AllocatorBase.h"
#include "Debug.h"
#include "PointerMath.h"

//------------
// Description
//--------------
// Stack Allocator, was used originally with Vertex Data Processing Scenario but later replaced with faster linear.
// Can only deallocate in reverse order of allocation.
//------------

class StackAllocator : public AllocatorBase
{
public:
	StackAllocator(size_t a_capacity,
		void* a_pStart);
	virtual ~StackAllocator();

	StackAllocator(const StackAllocator&) = delete;
	StackAllocator& operator=(const StackAllocator&) = delete;

	// Base Allocation Interface
	virtual void* allocate(size_t a_size, uint8 a_alignment) override;
	virtual void deallocate(void* a_pBlock) override;

private:
	struct AllocationHeader
	{
		size_t Adjustment;
#if DEBUG
		void* pPrev;
#endif
	};

	void* m_pLastPos;
	void* m_pCurrentPos;
};
