#pragma once

#include "AllocatorBase.h"
#include "PointerMath.h"
#include "Debug.h"

//------------
// Description
//--------------
// Pool Allocator Class
//------------

template<typename ObjectT, uint8 Alignment>
class PoolAllocator : public AllocatorBase
{
public:
	PoolAllocator(size_t a_capacity,
		void* a_pStart);
	virtual~PoolAllocator() {}

	PoolAllocator(const PoolAllocator&);
	PoolAllocator& operator=(const PoolAllocator&) = delete;

	// Base Allocator Interface
	virtual void* allocate(size_t a_size, uint8 a_alignment) override;
	virtual void deallocate(void* a_pBlock) override;

private:
	// Initialise Called On Ctor and Copy Ctor
	void Initialise();
	
	struct FreeList // Simple List Struct for Tracking Blocks
	{
		FreeList* m_pNext;
	};

	FreeList* m_pListHead;
};

template <typename ObjectT, uint8 Alignment>
PoolAllocator<ObjectT, Alignment>::PoolAllocator(size_t a_capacity, void* a_pStart)
	:
AllocatorBase(a_capacity, a_pStart),
m_pListHead(nullptr)
{
	Initialise();
}

template <typename ObjectT, uint8 Alignment>
PoolAllocator<ObjectT, Alignment>::PoolAllocator(const PoolAllocator& a_other)
	:
AllocatorBase(a_other.m_capacity, a_other.m_pStart),
m_pListHead(nullptr)
{
	Initialise();
}

template <typename ObjectT, uint8 Alignment>
void* PoolAllocator<ObjectT, Alignment>::allocate(size_t a_size, uint8 a_alignment)
{
	ASSERT(a_size == sizeof(ObjectT) && a_alignment == Alignment && "Incorrect Allocation Params for Pool");

	if (m_pListHead == nullptr) // Pool is Empty
	{
		return nullptr;
	}

	void* p = m_pListHead; // Get Current Head
	m_pListHead = m_pListHead->m_pNext; // Move Head

	m_usedSize += a_size;
#if DEBUG
	m_allocationCount++;
#endif

	return p;
}

template <typename ObjectT, uint8 Alignment>
void PoolAllocator<ObjectT, Alignment>::deallocate(void* a_pBlock)
{
	// Cast to Free List Pointer to Set as new Head
	FreeList* pNewHead = static_cast<FreeList*>(a_pBlock);
	pNewHead->m_pNext = m_pListHead; // Set next to Previous Head
	m_pListHead = pNewHead; // This is the new head

	m_usedSize -= sizeof(ObjectT);
#if DEBUG
	m_allocationCount--;
#endif
}

template <typename ObjectT, uint8 Alignment>
void PoolAllocator<ObjectT, Alignment>::Initialise()
{
	// Ensure Start Address is aligned
	const uint8 adjustment = PointerMath::AlignForwardAdjustment(m_pStart, Alignment);

	union
	{
		uintptr as_intptr;
		FreeList* as_list;
	};

	// Setup Head
	as_intptr = reinterpret_cast<uintptr>(m_pStart) + adjustment;
	m_pListHead = as_list;

	// Offset to next elem
	as_intptr += sizeof(ObjectT);

	// Calculate Max Objects we can store
	const size_t maxObjects = (m_capacity - adjustment) / sizeof(ObjectT);

	// Init List
	FreeList* p = m_pListHead;
	for (size_t i = 0; i < maxObjects - 1; ++i)
	{
		p->m_pNext = as_list;
		p = as_list;
		as_intptr += sizeof(ObjectT);
	}

	p->m_pNext = nullptr;
}
