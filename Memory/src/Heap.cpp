#include "Heap.h"
#include "Debug.h"
#include "MemSys.h"
#include "PointerMath.h"
#include "PerformanceCounter.h"
#include "GlobalTime.h"

// Min 1 Byte Heap Allocation
#define MIN_HEAP_ALLOC_SIZE (sizeof(Heap::AllocationHeader) + sizeof(char) + sizeof(unsigned int))


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
	AllocationHeader* pHeader = TryAllocate(a_size, a_alignment);

	if (pHeader == nullptr)
		return nullptr;

	if(pHeader->pNext != nullptr)
	{
		bool isValid = ((char*)pHeader + HEAP_ALLOC_HEADER_FOOTER_SIZE + pHeader->Size) <= (char*)pHeader->pNext;
		ASSERT(isValid);
	}

	if(pHeader->pPrev != nullptr)
	{
		bool isValid = ((char*)pHeader->pPrev + HEAP_ALLOC_HEADER_FOOTER_SIZE + pHeader->pPrev->Size) <= (char*)pHeader;
		ASSERT(isValid);
	}

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

float Heap::CalculateFragmentation() const
{
	size_t largestFreeBlock = 0u;
	size_t blockSize;

	char* pStart = (char*)m_pStart;
	char* pEnd = (char*)m_pHeadAlloc;
	if (m_pStart != m_pHeadAlloc)
	{
		blockSize = pEnd - pStart;
		if(blockSize > largestFreeBlock)
		{
			largestFreeBlock = blockSize;
		}
	}

	AllocationHeader* pCur = m_pHeadAlloc;
	while (pCur != nullptr && pCur->pNext != nullptr)
	{
		// Get Start and End Addresses Between Allocations
		pStart = (char*)pCur + HEAP_ALLOC_HEADER_FOOTER_SIZE + pCur->Size;
		pEnd = (char*)pCur->pNext;

		// Calculate Gap Size
		blockSize = pEnd - pStart;

		uintptr startIntPtr = (uintptr)pStart;
		uintptr endIntPtr = (uintptr)pEnd;

		if(pEnd < pStart)
		{
			LOG("Oops");
		}
		

		if(blockSize > largestFreeBlock)
		{
			largestFreeBlock = blockSize;
		}

		pCur = pCur->pNext;
	}

	pStart = (char*)m_pTailAlloc + HEAP_ALLOC_HEADER_FOOTER_SIZE + m_pTailAlloc->Size; // Tail Alloc End
	pEnd = (char*)m_pStart + m_capacity;

	if(pStart != pEnd)
	{
		blockSize = pEnd - pStart;
		if(blockSize > largestFreeBlock)
		{
			largestFreeBlock = blockSize;
		}
	}
	
	const size_t freeSize = m_capacity - m_usedSize;

	size_t freeMinLargest = freeSize - largestFreeBlock;

	float div = (float)freeMinLargest / (float)freeSize;

	div *= 100;
	
	return div;
	
}

void Heap::Defragment()
{

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



Heap::AllocationHeader* Heap::TryAllocate(size_t a_size, uint8 a_alignment)
{
	union
	{
		void* as_voidPtr;
		uintptr as_intptr;
		AllocationHeader* as_header;
	};

	if(m_pHeadAlloc == nullptr)
	{
		// Set Start
		as_voidPtr = m_pStart;
		as_intptr += sizeof(AllocationHeader); // Get Block Start
		const uint8 adjustment = PointerMath::AlignForwardAdjustment(as_voidPtr, a_alignment);  // Align Memory Block
		as_intptr -= (sizeof(AllocationHeader) - adjustment); // Get Header
		
		if(CapacityWouldExceed(a_size + HEAP_ALLOC_HEADER_FOOTER_SIZE)) // Ensure Capacity
		{
			return nullptr;
		}
		as_header->Sig = MEM_HEAP_SIG;
		as_header->pHeap = this;
		as_header->Size = a_size;
		as_header->Alignment = a_alignment;
		as_header->pNext = nullptr;
		as_header->pPrev = nullptr;

		m_pHeadAlloc = as_header;
		
		return as_header;
	}
	
	if (m_pTailAlloc == nullptr)
	{
		as_voidPtr = m_pHeadAlloc;
		as_intptr += HEAP_ALLOC_HEADER_FOOTER_SIZE + m_pHeadAlloc->Size;
		// Header Start
		as_intptr += sizeof(AllocationHeader); // Get Block Start
		const uint8 adjustment = PointerMath::AlignForwardAdjustment(as_voidPtr, a_alignment); // Align Memory Block
		as_intptr -= (sizeof(AllocationHeader) - adjustment ); // Get Header
		
		if (CapacityWouldExceed(a_size + HEAP_ALLOC_HEADER_FOOTER_SIZE)) // Ensure Capacity
		{
			return nullptr;
		}

		as_header->Sig = MEM_HEAP_SIG;
		as_header->pHeap = this;
		as_header->Size = a_size;
		as_header->Alignment = a_alignment;
		as_header->pNext = nullptr;
		as_header->pPrev = m_pHeadAlloc;
		
		m_pTailAlloc = as_header; // This is the Tail
		m_pHeadAlloc->pNext = m_pTailAlloc; // Link the Two
		
		return as_header;
	}

	AllocationHeader* pPrevClosestAlloc = nullptr;
	if(m_bFavourBestFit)
	{
		as_header = (AllocationHeader*)TryAllocateBestFit(a_size, a_alignment, pPrevClosestAlloc);
	}
	else
	{
		as_header = (AllocationHeader*)TryAllocateFirstFit(a_size, a_alignment, pPrevClosestAlloc);
	}

	if(as_header == nullptr)
	{
		// Failed to Fit Allocation
		return nullptr;
	}
	// Returned Header has Alignment Set


	as_header->Sig = MEM_HEAP_SIG;
	as_header->Size = a_size;
	as_header->Alignment = a_alignment;
	as_header->pHeap = this;
	
	as_header->pPrev = pPrevClosestAlloc;
	as_header->pNext = nullptr;

	if(pPrevClosestAlloc != nullptr)
	{
		ASSERT((char*)pPrevClosestAlloc + HEAP_ALLOC_HEADER_FOOTER_SIZE + (uint8)pPrevClosestAlloc->Size <= (char*)as_header);
		if(pPrevClosestAlloc->pNext == nullptr) // Should Be Tail Alloc
		{
			ASSERT(pPrevClosestAlloc == m_pTailAlloc && "Expected Tail Alloc");
			m_pTailAlloc->pNext = as_header; // Old Tail next to this
			m_pTailAlloc = as_header; // Set This to Tail
		}
		else // Not Tail
		{
			as_header->pNext = pPrevClosestAlloc->pNext; // Set Next
			pPrevClosestAlloc->pNext->pPrev = as_header; // Set Nexts Prev to This
			pPrevClosestAlloc->pNext = as_header; // Set Prev to This
		}
	}
	else // This should be the new head
	{
		ASSERT(as_header < m_pHeadAlloc && "Expected Address Before Current Head!");
		m_pHeadAlloc->pPrev = as_header; // Set Old Heads Prev to this
		as_header->pNext = m_pHeadAlloc; // Set this Next to Old Head
		m_pHeadAlloc = as_header; // Set this as new head
	}

	return as_header;
}

Heap::AllocationHeader* Heap::TryAllocateBestFit(size_t a_size, uint8 a_alignment, AllocationHeader*& a_pPrevClosestAlloc) const
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
		// Align Start
		pStart += sizeof(AllocationHeader); // Get Block Start
		const uint8 adjustment = PointerMath::AlignForwardAdjustment(pStart, a_alignment);  // Align Memory Block
		pStart -= (sizeof(AllocationHeader) - adjustment); // Get Header

		if (pStart < pEnd)
		{
			GapSize = pEnd - pStart;

			if (GapSize >= a_size + HEAP_ALLOC_HEADER_FOOTER_SIZE) // Tail - Capacity Can fit, Set as Best for now
			{
				pBestFit = pStart;
				bestSize = GapSize;
			}
		}
	}
	
	AllocationHeader* pCur = m_pHeadAlloc;
	while(pCur != nullptr && pCur->pNext != nullptr) // We have at least 2 Allocations
	{
		// Get Start and End Addresses Between Allocations
		pStart = (char*)pCur + HEAP_ALLOC_HEADER_FOOTER_SIZE + pCur->Size;
		pEnd = (char*)pCur->pNext;

		// Align Start
		pStart += sizeof(AllocationHeader); // Get Block Start
		const uint8 adjustment = PointerMath::AlignForwardAdjustment(pStart, a_alignment);  // Align Memory Block
		pStart -= (sizeof(AllocationHeader) - adjustment); // Get Header

		if(pStart <= pEnd)
		{
			// Calculate Gap Size
			GapSize = pEnd - pStart;

			if (GapSize >= a_size + HEAP_ALLOC_HEADER_FOOTER_SIZE && // Will Fit
				GapSize < bestSize) // Better than Best Size
			{
				pBestFit = pStart;
				bestSize = GapSize;
				pLast = pCur;
			}
		}
		
		pCur = pCur->pNext;
	}

	// Check for Gap between final alloc and capacity
	pStart = (char*)m_pTailAlloc + HEAP_ALLOC_HEADER_FOOTER_SIZE + m_pTailAlloc->Size; // Tail Alloc End

	pEnd = (char*)m_pStart + m_capacity;
	if(pStart != pEnd) // End Point
	{
		// Align Start
		pStart += sizeof(AllocationHeader); // Get Block Start
		const uint8 adjustment = PointerMath::AlignForwardAdjustment(pStart, a_alignment);  // Align Memory Block
		pStart -= (sizeof(AllocationHeader) - adjustment); // Get Header

		if(pStart < pEnd)
		{
			GapSize = pEnd - pStart;

			if (GapSize >= a_size + HEAP_ALLOC_HEADER_FOOTER_SIZE &&
				GapSize < bestSize)
			{
				pBestFit = pStart;
				pLast = m_pTailAlloc;
			}
		}
	}
	
	a_pPrevClosestAlloc = pLast;
	return (AllocationHeader*)pBestFit;
}

Heap::AllocationHeader* Heap::TryAllocateFirstFit(size_t a_size, uint8 a_alignment, AllocationHeader*& a_pPrevClosestAlloc)
{
	char* pStart = nullptr;
	char* pEnd = nullptr;
	size_t GapSize;
	if (m_pStart != m_pHeadAlloc) // Gap Between Start - Head Check First
	{
		pStart = (char*)m_pStart;
		pEnd = (char*)m_pHeadAlloc;
		// Align Start
		pStart += sizeof(AllocationHeader); // Get Block Start
		const uint8 adjustment = PointerMath::AlignForwardAdjustment(pStart, a_alignment);  // Align Memory Block
		pStart -= (sizeof(AllocationHeader) - adjustment); // Get Header

		if (pStart < pEnd)
		{
			GapSize = pEnd - pStart;

			if (GapSize >= a_size + HEAP_ALLOC_HEADER_FOOTER_SIZE) // Tail - Capacity Can fit, Set as Best for now
			{
				return (AllocationHeader*)pStart;
			}
		}
	}

	AllocationHeader* pCur = m_pHeadAlloc;
	while (pCur != nullptr && pCur->pNext != nullptr) // We have at least 2 Allocations
	{
		// Get Start and End Addresses Between Allocations
		pStart = (char*)pCur + HEAP_ALLOC_HEADER_FOOTER_SIZE + pCur->Size;
		pEnd = (char*)pCur->pNext;

		// Align Start
		pStart += sizeof(AllocationHeader); // Get Block Start
		const uint8 adjustment = PointerMath::AlignForwardAdjustment(pStart, a_alignment);  // Align Memory Block
		pStart -= (sizeof(AllocationHeader) - adjustment); // Get Header

		if (pStart <= pEnd)
		{
			// Calculate Gap Size
			GapSize = pEnd - pStart;

			if (GapSize >= a_size + HEAP_ALLOC_HEADER_FOOTER_SIZE) // Better than Best Size
			{
				a_pPrevClosestAlloc = pCur;
				return (AllocationHeader*)pStart;
			}
		}
		pCur = pCur->pNext;
	}

	// Check for Gap between final alloc and capacity
	pStart = (char*)m_pTailAlloc + HEAP_ALLOC_HEADER_FOOTER_SIZE + m_pTailAlloc->Size; // Tail Alloc End

	pEnd = (char*)m_pStart + m_capacity;
	if (pStart != pEnd) // End Point
	{
		// Align Start
		pStart += sizeof(AllocationHeader); // Get Block Start
		const uint8 adjustment = PointerMath::AlignForwardAdjustment(pStart, a_alignment);  // Align Memory Block
		pStart -= (sizeof(AllocationHeader) - adjustment); // Get Header

		if (pStart < pEnd)
		{
			GapSize = pEnd - pStart;

			if (GapSize >= a_size + HEAP_ALLOC_HEADER_FOOTER_SIZE)
			{
				a_pPrevClosestAlloc = m_pTailAlloc;
				return (AllocationHeader*)pStart;
			}
		}
	}
	
	return nullptr;
}

bool Heap::CapacityWouldExceed(size_t a_size) const
{
	if (m_usedSize + a_size > m_capacity)
	{
		LOG("Heap: %s Failed Attempted Allocation of Size %u REASON - Exceeds Capacity")
			return true;
	}
	return false;
}
