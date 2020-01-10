#pragma once
#include "AllocatorBase.h"
#include "Debug.h"
#include "PointerMath.h"

class StackAllocator : public AllocatorBase
{
public:
	StackAllocator(size_t a_capacity,
		void* a_pStart);
	virtual ~StackAllocator();

	StackAllocator(const StackAllocator&) = delete;
	StackAllocator& operator=(const StackAllocator&) = delete;

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
