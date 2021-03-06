#include "StackAllocator.h"


StackAllocator::StackAllocator(size_t a_capacity, void* a_pStart)
	:
	AllocatorBase(a_capacity, a_pStart),
	m_pLastPos(nullptr),
	m_pCurrentPos(a_pStart)
{
}


StackAllocator::~StackAllocator()
{
	ASSERT(m_pLastPos == nullptr && "Allocations still live - Memory Leak!");
}

void* StackAllocator::allocate(size_t a_size, uint8 a_alignment)
{
	union
	{
		void* as_voidPtr;
		uintptr as_intptr;
		AllocationHeader* as_header;
	};

	as_voidPtr = m_pCurrentPos;
	as_intptr += sizeof(AllocationHeader);
	const uint8 adjustment = PointerMath::AlignForwardAdjustment(as_voidPtr, a_alignment);
	as_intptr -= (sizeof(AllocationHeader) - adjustment);

	if(CapacityWouldExceed(a_size + adjustment + sizeof(AllocationHeader)))
	{
		return nullptr;
	}

	// Offset by header to get aligned address
	as_intptr += sizeof(AllocationHeader);

	void* pAlignedBlockAddr = as_voidPtr;

	// Get Header Address
	as_intptr -= sizeof(AllocationHeader);

	// Set Adjustment
	as_header->Adjustment = adjustment;

#if DEBUG
	as_header->pPrev = m_pLastPos;
	m_pLastPos = pAlignedBlockAddr;
#endif

	// Set new current
	m_pCurrentPos = (void*)((uintptr)pAlignedBlockAddr + a_size);

	m_usedSize += a_size + adjustment;
#if DEBUG
	m_allocationCount++;
#endif

	return pAlignedBlockAddr;
}

inline void StackAllocator::deallocate(void* a_pBlock)
{
	ASSERT(a_pBlock == m_pLastPos && "Attempting to Deallocate non head allocation!");

	union
	{
		void* as_voidPtr;
		uintptr as_intptr;
		AllocationHeader* as_header;
	};

	// Get Header
	as_intptr = (uintptr)a_pBlock - sizeof(AllocationHeader);

	// Remove Block + Adjustment size
	m_usedSize -= ((uintptr)m_pCurrentPos - (uintptr)a_pBlock) +
		as_header->Adjustment;

	// Adjust Current Position
	m_pCurrentPos = (void*)((uintptr)a_pBlock - as_header->Adjustment);

#if DEBUG
	m_pLastPos = as_header->pPrev;
	--m_allocationCount;
#endif
}