#include "Heap.h"
#include "Debug.h"
#include "MemSys.h"

// Min 1 Byte Heap Allocation
#define MIN_HEAP_ALLOC_SIZE (sizeof(Heap::AllocationHeader) + sizeof(char) + sizeof(unsigned int))
#define HEAP_ALLOC_HEADER_FOOTER_SIZE (sizeof(Heap::AllocationHeader) + sizeof(unsigned int))


Heap::Heap()
	:
AllocatorBase(),
m_Name{"INVALID"},
m_bActive(false),
m_bFavourBestFit(true),
m_pParent(nullptr),
m_pChild(nullptr),
m_pNextSibling(nullptr),
m_pPreviousSibling(nullptr),
m_pHeadAlloc(nullptr),
m_pTailAlloc(nullptr)
{
}

Heap::~Heap()
{
}

void Heap::Activate(const Config& a_config)
{
	ASSERT(a_config.Name != nullptr && "Name cannot be Null!");
	ASSERT(a_config.pStartPtr != nullptr && "Start Pointer cannot be Null!");
	ASSERT(a_config.Capacity >= MIN_HEAP_ALLOC_SIZE && "Capacity must be greater than min heap alloc size!");
	// Reset Vars Here
	m_bActive = true;
	m_pStart = a_config.pStartPtr;
	m_capacity = a_config.Capacity;
	strcpy_s(m_Name, MAX_HEAP_NAME_LEN, a_config.Name);
}

void* Heap::allocate(size_t a_size, uint8 a_alignment)
{
	AllocationHeader* pHeader = TryAllocate(a_size);

	if (pHeader == nullptr)
		return nullptr;

	char* pBlockStart = (char*)pHeader + sizeof(AllocationHeader);

	// Stamp End
	uint32* pEndMark = (uint32*)(pBlockStart + a_size);
	*pEndMark = MEM_END;

	// Increase Total Allocation Size
	m_usedSize += HEAP_ALLOC_HEADER_FOOTER_SIZE + pHeader->Size;
#if DEBUG
	m_allocationCount++;
#endif
	
	return pBlockStart;
}

void Heap::Deallocate(void* a_pBlock)
{
	auto pHeader = (AllocationHeader*)((char*)a_pBlock - sizeof(AllocationHeader));
	ASSERT(pHeader->Sig == MEM_HEAP_SIG && "Expected Heap Signature?");
	pHeader->pHeap->deallocate(a_pBlock); // Perform Deallocation
}

void Heap::deallocate(void* a_pBlock)
{
	auto pHeader = (AllocationHeader*)((char*)a_pBlock - sizeof(AllocationHeader));
	uint32* pEndMark = (uint32*)((char*)a_pBlock + pHeader->Size);
	ASSERT(*pEndMark == MEM_END && "Missing End Mark, Corrupt Allocation?");

	// Head------->Tail
	if (pHeader->pPrev == nullptr) // Must be Head 
	{
		ASSERT(pHeader == m_pHeadAlloc && "Expected Header to be Head Allocation");
		m_pHeadAlloc = pHeader->pNext; // Update New Head
		m_pHeadAlloc->pPrev = nullptr;
	}
	else if (pHeader->pNext == nullptr)
	{
		ASSERT(pHeader == m_pTailAlloc && "Expected Header to be Head Allocation");
		m_pTailAlloc = pHeader->pPrev;
		m_pTailAlloc->pNext = nullptr;
	}
	else // Not Head
	{
		pHeader->pPrev->pNext = pHeader->pNext;
		pHeader->pNext->pPrev = pHeader->pPrev;
	}

	m_usedSize -= HEAP_ALLOC_HEADER_FOOTER_SIZE + pHeader->Size;
#if DEBUG
	--m_allocationCount;
#endif
	// Free
	memset(pHeader, 0, HEAP_ALLOC_HEADER_FOOTER_SIZE + pHeader->Size);
}

void Heap::SetParent(Heap* a_pParent)
{
	ASSERT(a_pParent != nullptr && "Parent Heap is null!");
	
	if (m_pParent == a_pParent) // Avoid Reparenting
		return;

	// Detach from Siblings if any
	if(m_pPreviousSibling != nullptr)
	{
		m_pPreviousSibling->m_pNextSibling = m_pNextSibling;
	}

	if(m_pNextSibling != nullptr)
	{
		m_pNextSibling->m_pPreviousSibling = m_pPreviousSibling;
	}

	// Unparent
	if(m_pParent != nullptr && // We have a parent
		m_pParent->m_pChild == this) // Direct Child is us
	{
		m_pParent->m_pChild = m_pNextSibling; // Set to Next Sibling
	}

	// Now Parent to new parent
	m_pParent = a_pParent;
	m_pPreviousSibling = nullptr;
	// Insert as First Child
	m_pNextSibling = m_pParent->m_pChild;
	m_pParent->m_pChild = this;
}



Heap::AllocationHeader* Heap::TryAllocate(size_t a_size)
{
#if DEBUG
	if(m_usedSize + a_size > m_capacity)
	{
		LOG("Heap: %s Failed Attempted Allocation of Size %u REASON - Exceeds Capacity")
		return nullptr;
	}
#endif

	if(m_pHeadAlloc == nullptr)
	{
		AllocationHeader* pHeader = (AllocationHeader*)m_pStart;
		pHeader->Sig = MEM_HEAP_SIG;
		pHeader->pHeap = this;
		pHeader->Size = a_size;
		pHeader->pNext = nullptr;
		pHeader->pPrev = nullptr;

		m_pHeadAlloc = pHeader;
		
		return pHeader;
	}
	
	if (m_pTailAlloc == nullptr)
	{
		AllocationHeader* pHeader = (AllocationHeader*)((char*)m_pHeadAlloc + HEAP_ALLOC_HEADER_FOOTER_SIZE + m_pHeadAlloc->Size);
		pHeader->Sig = MEM_HEAP_SIG;
		pHeader->pHeap = this;
		pHeader->Size = a_size;
		pHeader->pNext = nullptr;
		pHeader->pPrev = m_pHeadAlloc;
		
		m_pTailAlloc = pHeader; // This is the Tail
		m_pHeadAlloc->pNext = m_pTailAlloc; // Link the Two
		
		return pHeader;
	}

	AllocationHeader* pPrevClosestAlloc = nullptr;
	AllocationHeader* pAllocHeader = nullptr;

	if(m_bFavourBestFit)
	{
		pAllocHeader = (AllocationHeader*)TryAllocateBestFit(a_size, pPrevClosestAlloc);
	}
	else
	{
		pAllocHeader = (AllocationHeader*)TryAllocateFirstFit(a_size, pPrevClosestAlloc);
	}

	if(pAllocHeader == nullptr)
	{
		// Failed to Fit Allocation
		return nullptr;
	}


	pAllocHeader->Sig = MEM_HEAP_SIG;
	pAllocHeader->Size = a_size;
	pAllocHeader->pHeap = this;
	
	pAllocHeader->pPrev = pPrevClosestAlloc;
	pAllocHeader->pNext = nullptr;

	if(pPrevClosestAlloc != nullptr)
	{
		if(pPrevClosestAlloc->pNext == nullptr) // Should Be Tail Alloc
		{
			ASSERT(pPrevClosestAlloc == m_pTailAlloc && "Expected Tail Alloc");
			m_pTailAlloc->pNext = pAllocHeader; // Old Tail next to this
			m_pTailAlloc = pAllocHeader; // Set This to Tail
		}
		else // Not Tail
		{
			pAllocHeader->pNext = pPrevClosestAlloc->pNext; // Set Next
			pPrevClosestAlloc->pNext->pPrev = pAllocHeader; // Set Nexts Prev to This
			pPrevClosestAlloc->pNext = pAllocHeader; // Set Prev to This
		}
	}
	else // This should be the new head
	{
		ASSERT(pAllocHeader < m_pHeadAlloc && "Expected Address Before Current Head!");
		m_pHeadAlloc->pPrev = pAllocHeader; // Set Old Heads Prev to this
		pAllocHeader->pNext = m_pHeadAlloc; // Set this Next to Old Head
		m_pHeadAlloc = pAllocHeader; // Set this as new head
	}

	return pAllocHeader;
}

void* Heap::TryAllocateBestFit(size_t a_size, AllocationHeader*& a_pPrevClosestAlloc) const
{
	// Walk the Heap
	AllocationHeader* pLast = nullptr;
	void* pBestFit = nullptr;
	size_t bestSize = ~0;
	size_t GapSize = 0u;

	char* pStart = nullptr;
	char* pEnd = nullptr;

	if(m_pStart != m_pHeadAlloc) // Gap Between Start - Head Check First
	{
		pStart = (char*)m_pStart;
		pEnd = (char*)m_pHeadAlloc;
		GapSize = pEnd - pStart;

		if (GapSize >= a_size + HEAP_ALLOC_HEADER_FOOTER_SIZE) // Tail - Capacity Can fit, Set as Best for now
		{
			pBestFit = m_pStart;
			bestSize = GapSize;
		}
	}
	
	AllocationHeader* pCur = m_pHeadAlloc;
	while(pCur != nullptr && pCur->pNext != nullptr) // We have at least 2 Allocations
	{
		// Get Start and End Addresses Between Allocations
		pStart = (char*)pCur + HEAP_ALLOC_HEADER_FOOTER_SIZE + pCur->Size;
		pEnd = (char*)pCur->pNext;

		// Calculate Gap Size
		GapSize = pEnd - pStart;

		if(GapSize >= a_size + HEAP_ALLOC_HEADER_FOOTER_SIZE && // Will Fit
			GapSize < bestSize) // Better than Best Size
		{
			pBestFit = pStart;
			bestSize = GapSize;
			pLast = pCur;
		}

		pCur = pCur->pNext;
	}

	// Check for Gap between final alloc and capacity
	pStart = (char*)m_pTailAlloc + HEAP_ALLOC_HEADER_FOOTER_SIZE + m_pTailAlloc->Size; // Tail Alloc End
	pEnd = (char*)m_pStart + m_capacity;
	if(pStart != pEnd) // End Point
	{
		GapSize = pEnd - pStart;
		if(GapSize >= a_size + HEAP_ALLOC_HEADER_FOOTER_SIZE &&
			GapSize < bestSize)
		{
			pBestFit = pStart;
			pLast = m_pTailAlloc;
		}
	}
	
	a_pPrevClosestAlloc = pLast;
	return pBestFit;
}

void* Heap::TryAllocateFirstFit(size_t a_size, AllocationHeader*& a_pPrevClosestAlloc)
{
	ASSERT(false);
	return nullptr;
}
