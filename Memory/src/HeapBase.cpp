#include "HeapBase.h"
#include "Debug.h"
#include "PointerMath.h"
#include <cstring>

#define HEAP_ALLOC_HEADER_FOOTER_SIZE (GetAllocHeaderSize() + sizeof(HeapBase::HeapSignature))

constexpr const char* kInvalidName = "Invalid";

HeapBase::HeapBase()
	:
	m_pHeadAlloc(nullptr),
	m_pTailAlloc(nullptr),
	m_pParent(nullptr),
	m_pChild(nullptr),
	m_pNextSibling(nullptr),
	m_pPreviousSibling(nullptr),
	m_Name{  },
	m_bActive(false),
	m_bFavourBestFit(true)
{
	strcpy_s(m_Name, HEAP_MAX_NAME_LEN, kInvalidName);
}

HeapBase::~HeapBase()
{
}

void HeapBase::Activate(Config& a_config)
{
	ASSERT(a_config.Name != nullptr && "Name cannot be Null!");
	ASSERT(a_config.pStartPtr != nullptr && "Start Pointer cannot be Null!");
	// Reset Vars Here
	m_bActive = true;
	m_pStart = a_config.pStartPtr;
	m_capacity = a_config.Capacity;
	strcpy_s(m_Name, HEAP_MAX_NAME_LEN, a_config.Name);
}

void HeapBase::Deactivate()
{
	m_bActive = false;
	m_pStart = nullptr;
	m_capacity = 0u;
	m_pChild = nullptr;
	m_pNextSibling = nullptr;
	m_pPreviousSibling = nullptr;
	m_pParent = nullptr;
	
	strcpy_s(m_Name, HEAP_MAX_NAME_LEN, kInvalidName);
}

void HeapBase::SetParent(HeapBase* a_pParent)
{
#if DEBUG
	if(a_pParent != nullptr)
	{
		ASSERT(a_pParent->IsMoveable() == false && "Can't Parent Heap under Moveable Parent");
	}
#endif
	
	if (m_pParent == a_pParent) // Avoid Reparenting
		return;

	// Detach from Siblings if any
	if (m_pPreviousSibling != nullptr)
	{
		m_pPreviousSibling->m_pNextSibling = m_pNextSibling;
	}

	if (m_pNextSibling != nullptr)
	{
		m_pNextSibling->m_pPreviousSibling = m_pPreviousSibling;
	}

	// Unparent
	if (m_pParent != nullptr && // We have a parent
		m_pParent->m_pChild == this) // Direct Child is us
	{
		m_pParent->m_pChild = m_pNextSibling; // Set to Next Sibling
	}

	// Now Parent to new parent
	m_pParent = a_pParent;
	m_pPreviousSibling = nullptr;
	// Insert as First Child
	if(m_pParent != nullptr)
	{
		m_pNextSibling = m_pParent->m_pChild;
		m_pParent->m_pChild = this;
	}
}

float HeapBase::CalculateFragmentation() const
{
	if (m_pHeadAlloc == nullptr || m_pTailAlloc == nullptr)
	{
		return 0.f;
	}

	// Identify Largest Free Block of Memory Size 
	size_t largestFreeBlock = 0u;
	size_t blockSize;

	char* pStart = (char*)m_pStart;
	char* pEnd = (char*)m_pHeadAlloc;
	if (m_pStart != m_pHeadAlloc) // Gap Between Start - Head Alloc
	{
		blockSize = pEnd - pStart;
		if (blockSize > largestFreeBlock)
		{
			largestFreeBlock = blockSize;
		}
	}

	BaseAllocationHeader* pCur = m_pHeadAlloc;
	while (pCur != nullptr && pCur->pNext != nullptr)
	{
		// Get Start and End Addresses Between Allocations
		pStart = (char*)pCur + HEAP_ALLOC_HEADER_FOOTER_SIZE + pCur->Size;
		pEnd = (char*)pCur->pNext;

		// Calculate Gap Size
		blockSize = pEnd - pStart;

		if (blockSize > largestFreeBlock)
		{
			largestFreeBlock = blockSize;
		}

		pCur = pCur->pNext;
	}

	pStart = (char*)m_pTailAlloc + HEAP_ALLOC_HEADER_FOOTER_SIZE + m_pTailAlloc->Size; // Tail Alloc End
	pEnd = (char*)m_pStart + m_capacity;

	if (pStart != pEnd)
	{
		blockSize = pEnd - pStart;
		if (blockSize > largestFreeBlock)
		{
			largestFreeBlock = blockSize;
		}
	}

	const size_t freeSize = m_capacity - m_usedSize;
	const size_t freeMinLargest = freeSize - largestFreeBlock;

	return ((float)freeMinLargest / (float)freeSize) * 100;
}

HeapBase::BaseAllocationHeader* HeapBase::TryAllocate(size_t a_size, uint8 a_alignment)
{
	union
	{
		void* as_voidPtr;
		uintptr as_intptr;
		BaseAllocationHeader* as_header;
	};

	if (CapacityWouldExceed(a_size + HEAP_ALLOC_HEADER_FOOTER_SIZE)) // Ensure Capacity
	{
		return nullptr;
	}

	if (m_pHeadAlloc == nullptr)
	{
		// Set Start
		as_voidPtr = m_pStart;
		as_intptr += GetAllocHeaderSize(); // Get Block Start
		const uint8 adjustment = PointerMath::AlignForwardAdjustment(as_voidPtr, a_alignment);  // Align Memory Block
		as_intptr -= GetAllocHeaderSize() - adjustment; // Get Header

		as_header->pHeap = this;
		as_header->Size = a_size;
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
		as_intptr += GetAllocHeaderSize(); // Get Block Start
		const uint8 adjustment = PointerMath::AlignForwardAdjustment(as_voidPtr, a_alignment); // Align Memory Block
		as_intptr -= (GetAllocHeaderSize() - adjustment); // Get Header

		as_header->pHeap = this;
		as_header->Size = a_size;
		as_header->pNext = nullptr;
		as_header->pPrev = m_pHeadAlloc;

		m_pTailAlloc = as_header; // This is the Tail
		m_pHeadAlloc->pNext = m_pTailAlloc; // Link the Two

		return as_header;
	}

	BaseAllocationHeader* pPrevClosestAlloc = nullptr;
	if (m_bFavourBestFit)
	{
		as_header = (BaseAllocationHeader*)TryAllocateBestFit(a_size, a_alignment, pPrevClosestAlloc);
	}
	else
	{
		as_header = (BaseAllocationHeader*)TryAllocateFirstFit(a_size, a_alignment, pPrevClosestAlloc);
	}

	if (as_header == nullptr)
	{
		// Failed to Fit Allocation
#if DEBUG
		const size_t FreeSpace = m_capacity - m_usedSize;
		if(FreeSpace >= a_size)
		{
			LOG("Failed to Allocate on Heap: %s due to fragmentation \n", m_Name);
		}
		else
		{
			LOG("Failed to Allocate on Heap: %s, capacity exceeded \n", m_Name);
		}
		
#endif
		return nullptr;
	}

	as_header->Size = a_size;
	as_header->pHeap = this;

	as_header->pPrev = pPrevClosestAlloc;
	as_header->pNext = nullptr;

	if (pPrevClosestAlloc != nullptr) // We are allocating in front of a previous allocation
	{
		ASSERT((char*)pPrevClosestAlloc + HEAP_ALLOC_HEADER_FOOTER_SIZE + (uint8)pPrevClosestAlloc->Size <= (char*)as_header);
		if (pPrevClosestAlloc->pNext == nullptr) // Should Be Tail Alloc
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

HeapBase::BaseAllocationHeader* HeapBase::TryAllocateBestFit(size_t a_size, uint8 a_alignment,
	BaseAllocationHeader*& a_pPrevClosestAlloc) const
{
	// Walk the Heap
	BaseAllocationHeader* pLast = nullptr;
	void* pBestFit = nullptr;
	size_t bestSize = ~0;
	size_t GapSize = 0u;

	char* pStart = nullptr;
	char* pEnd = nullptr;
	const size_t headerSize = GetAllocHeaderSize(); // Cache to reduce pesky v func calls

	if (m_pStart != m_pHeadAlloc) // Gap Between Start - Head Check First
	{
		pStart = (char*)m_pStart;
		pEnd = (char*)m_pHeadAlloc;
		// Align Start
		pStart += headerSize; // Get Block Start
		const uint8 adjustment = PointerMath::AlignForwardAdjustment(pStart, a_alignment);  // Align Memory Block
		pStart -= headerSize - adjustment; // Get Header

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

	BaseAllocationHeader* pCur = m_pHeadAlloc;
	while (pCur != nullptr && pCur->pNext != nullptr) // We have at least 2 Allocations
	{
		// Get Start and End Addresses Between Allocations
		pStart = (char*)pCur + HEAP_ALLOC_HEADER_FOOTER_SIZE + pCur->Size;
		pEnd = (char*)pCur->pNext;

		// Align Start
		pStart += headerSize; // Get Block Start
		const uint8 adjustment = PointerMath::AlignForwardAdjustment(pStart, a_alignment);  // Align Memory Block
		pStart -= headerSize - adjustment; // Get Header

		if (pStart < pEnd)
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

	pEnd = (char*)m_pStart + m_capacity - 1;
	if (pStart != pEnd) // End Point
	{
		// Align Start
		pStart += headerSize; // Get Block Start
		const uint8 adjustment = PointerMath::AlignForwardAdjustment(pStart, a_alignment);  // Align Memory Block
		pStart -= (headerSize - adjustment); // Get Header

		if (pStart < pEnd)
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
	return (BaseAllocationHeader*)pBestFit;
}

HeapBase::BaseAllocationHeader* HeapBase::TryAllocateFirstFit(size_t a_size, uint8 a_alignment,
	BaseAllocationHeader*& a_pPrevClosestAlloc)
{
	char* pStart = nullptr;
	char* pEnd = nullptr;
	size_t GapSize;
	const size_t headerSize = GetAllocHeaderSize(); // Cache to reduce pesky v func calls
	
	if (m_pStart != m_pHeadAlloc) // Gap Between Start - Head Check First
	{
		pStart = (char*)m_pStart;
		pEnd = (char*)m_pHeadAlloc;
		// Align Start
		pStart += headerSize; // Get Block Start
		const uint8 adjustment = PointerMath::AlignForwardAdjustment(pStart, a_alignment);  // Align Memory Block
		pStart -= (headerSize - adjustment); // Get Header

		if (pStart < pEnd)
		{
			GapSize = pEnd - pStart;

			if (GapSize >= a_size + HEAP_ALLOC_HEADER_FOOTER_SIZE) // Tail - Capacity Can fit, Set as Best for now
			{
				return (BaseAllocationHeader*)pStart;
			}
		}
	}

	BaseAllocationHeader* pCur = m_pHeadAlloc;
	while (pCur != nullptr && pCur->pNext != nullptr) // We have at least 2 Allocations
	{
		// Get Start and End Addresses Between Allocations
		pStart = (char*)pCur + HEAP_ALLOC_HEADER_FOOTER_SIZE + pCur->Size;
		pEnd = (char*)pCur->pNext;

		// Align Start
		pStart += headerSize; // Get Block Start
		const uint8 adjustment = PointerMath::AlignForwardAdjustment(pStart, a_alignment);  // Align Memory Block
		pStart -= headerSize - adjustment; // Get Header

		if (pStart < pEnd)
		{
			// Calculate Gap Size
			GapSize = pEnd - pStart;

			if (GapSize >= a_size + HEAP_ALLOC_HEADER_FOOTER_SIZE) // Better than Best Size
			{
				a_pPrevClosestAlloc = pCur;
				return (BaseAllocationHeader*)pStart;
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
		pStart += headerSize; // Get Block Start
		const uint8 adjustment = PointerMath::AlignForwardAdjustment(pStart, a_alignment);  // Align Memory Block
		pStart -= headerSize - adjustment; // Get Header

		if (pStart < pEnd)
		{
			GapSize = pEnd - pStart;

			if (GapSize >= a_size + HEAP_ALLOC_HEADER_FOOTER_SIZE)
			{
				a_pPrevClosestAlloc = m_pTailAlloc;
				return (BaseAllocationHeader*)pStart;
			}
		}
	}

	return nullptr;
}
