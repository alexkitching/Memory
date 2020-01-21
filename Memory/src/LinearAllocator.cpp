#include "LinearAllocator.h"
#include "Debug.h"
#include "PointerMath.h"

LinearAllocator::LinearAllocator(size_t a_capacity, void* a_pStart)
	:
	AllocatorBase(a_capacity, a_pStart),
	m_pCurrentPos(a_pStart)
{

}

void* LinearAllocator::allocate(size_t a_size, uint8 a_alignment)
{
	// Calculate Adjustment Required for Alignment
	const uint8 adjustment = PointerMath::AlignForwardAdjustment(m_pCurrentPos, a_alignment);

	// Ensure Capacity not exceeded
	if(CapacityWouldExceed(a_size + adjustment))
	{
		return nullptr;
	}

	union
	{
		void* as_voidPtr;
		uintptr as_intptr;
	};

	// Set Current Position
	as_voidPtr = m_pCurrentPos;
	// Offset by adjustment to get aligned address
	as_intptr += adjustment;

	// Set new current
	m_pCurrentPos = reinterpret_cast<void*>(as_intptr + a_size);

	m_usedSize += a_size + adjustment;
#if DEBUG
	m_allocationCount++;
#endif
	
	return as_voidPtr;
}

void LinearAllocator::deallocate(void* a_pBlock)
{
	ASSERT(false && "Undefined Use of Linear Allocator, Deallocation can only be performed via Clear()");
}

void LinearAllocator::Clear()
{
	m_pCurrentPos = nullptr;
#if DEBUG
	m_allocationCount = 0;
#endif
	m_usedSize = 0;
}
