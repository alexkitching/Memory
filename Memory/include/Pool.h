#pragma once

#include "AllocatorBase.h"
#include "PointerMath.h"
#include "Debug.h"

template<typename ObjectT, uint8 Alignment>
class Pool : public AllocatorBase
{
public:
	Pool(size_t a_capacity,
		void* a_pStart);
	virtual~Pool()
	{
		m_pListHead = nullptr;
	}

	
	Pool(const Pool&);
	Pool& operator=(const Pool&) = delete;

	virtual void* allocate(size_t a_size, uint8 a_alignment) override;
	void deallocate(void* a_pBlock) override;

	typedef ObjectT value_type;
private:
	void Initialise();
	
	struct FreeList
	{
		FreeList* m_pNext;
	};

	FreeList* m_pListHead;
};

template <typename ObjectT, uint8 Alignment>
Pool<ObjectT, Alignment>::Pool(size_t a_capacity, void* a_pStart)
	:
AllocatorBase(a_capacity, a_pStart),
m_pListHead(nullptr)
{
	Initialise();
}

template <typename ObjectT, uint8 Alignment>
Pool<ObjectT, Alignment>::Pool(const Pool& a_other)
	:
AllocatorBase(a_other.m_capacity, a_other.m_pStart),
m_pListHead(nullptr)
{
	Initialise();
}

template <typename ObjectT, uint8 Alignment>
void* Pool<ObjectT, Alignment>::allocate(size_t a_size, uint8 a_alignment)
{
	ASSERT(a_size == sizeof(ObjectT) && a_alignment == Alignment && "Incorrect Allocation Params for Pool");

	if (m_pListHead == nullptr)
	{
		return nullptr;
	}

	void* p = m_pListHead;
	m_pListHead = m_pListHead->m_pNext; // Move Head

	m_usedSize += a_size;
	m_allocationCount++;

	return p;
}

template <typename ObjectT, uint8 Alignment>
void Pool<ObjectT, Alignment>::deallocate(void* a_pBlock)
{
	FreeList* pNewHead = (FreeList*)a_pBlock;
	pNewHead->m_pNext = m_pListHead;
	m_pListHead = pNewHead;

	m_usedSize -= sizeof(ObjectT);
	m_allocationCount--;
}

template <typename ObjectT, uint8 Alignment>
void Pool<ObjectT, Alignment>::Initialise()
{
	const uint8 adjustment = PointerMath::AlignForwardAdjustment(m_pStart, Alignment);

	union
	{
		uintptr as_intptr;
		FreeList* as_list;
	};

	// Setup Head
	as_intptr = (uintptr)m_pStart + adjustment;
	m_pListHead = as_list;

	// Offset to next elem
	as_intptr += sizeof(ObjectT);

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
