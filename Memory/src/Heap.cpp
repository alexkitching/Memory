#include "Heap.h"
#include "Debug.h"
#include "MemSys.h"
#include "PointerMath.h"
#include "GlobalTime.h"


Heap::Heap()
{
}

Heap::~Heap()
{
}

void* Heap::allocate(size_t a_size, uint8 a_alignment)
{
	BaseAllocationHeader* pHeader = TryAllocate(a_size, a_alignment);
	
	if (pHeader == nullptr)
		return nullptr;

	pHeader->Sig = HEAP_ALLOC_SIG; // Set Unique Sig

#if DEBUG // Additional Safety Checks
	if(pHeader->pNext != nullptr)
	{
		bool isValid = ((char*)pHeader + sizeof(BaseAllocationHeader) + sizeof(HeapSignature) + pHeader->Size) <= (char*)pHeader->pNext;
		ASSERT(isValid);
	}

	if(pHeader->pPrev != nullptr)
	{
		bool isValid = ((char*)pHeader->pPrev + sizeof(BaseAllocationHeader) + sizeof(HeapSignature) + pHeader->pPrev->Size) <= (char*)pHeader;
		ASSERT(isValid);
	}
#endif

	char* pBlockStart = (char*)pHeader + GetAllocHeaderSize();

	// Stamp End
	uint32* pEndMark = (uint32*)(pBlockStart + a_size);
	*pEndMark = MEM_END;

	// Increase Total Allocation Size
	m_usedSize += sizeof(BaseAllocationHeader) + sizeof(HeapSignature) + pHeader->Size;
#if DEBUG
	m_allocationCount++;
#endif
	
	return pBlockStart;
}

void Heap::Deallocate(void* a_pBlock)
{
	auto pHeader = (BaseAllocationHeader*)((char*)a_pBlock - sizeof(BaseAllocationHeader));
	ASSERT(pHeader->Sig == HEAP_ALLOC_SIG && "Expected Heap Signature?");
	pHeader->pHeap->deallocate(a_pBlock); // Perform Deallocation
}

void Heap::deallocate(void* a_pBlock)
{
	auto pHeader = (BaseAllocationHeader*)((char*)a_pBlock - GetAllocHeaderSize());
	uint32* pEndMark = (uint32*)((char*)a_pBlock + pHeader->Size);
	ASSERT(*pEndMark == MEM_END && "Missing End Mark, Corrupt Allocation?");

	// Head------->Tail
	if (pHeader->pPrev == nullptr &&
		pHeader->pNext == nullptr)
	{
		m_pHeadAlloc = nullptr;
		m_pTailAlloc = nullptr;
	}
	else if (pHeader->pPrev == nullptr) // Must be Head 
	{
		ASSERT(pHeader == m_pHeadAlloc && "Expected Header to be Head Allocation");
		m_pHeadAlloc = pHeader->pNext; // Update New Head
		if (m_pHeadAlloc == m_pTailAlloc) // Head is now Tail
		{
			m_pTailAlloc = nullptr;
		}
		m_pHeadAlloc->pPrev = nullptr;
	}
	else if (pHeader->pNext == nullptr)
	{
		ASSERT(pHeader == m_pTailAlloc && "Expected Header to be Tail Allocation");
		m_pTailAlloc = pHeader->pPrev;
		m_pTailAlloc->pNext = nullptr;
	}
	else // Not Head
	{
		pHeader->pPrev->pNext = pHeader->pNext;
		pHeader->pNext->pPrev = pHeader->pPrev;
	}

	m_usedSize -= sizeof(BaseAllocationHeader) + sizeof(HeapSignature) + pHeader->Size;
#if DEBUG
	--m_allocationCount;
#endif
	// Free
	memset(pHeader, 0, sizeof(BaseAllocationHeader) + sizeof(HeapSignature) + pHeader->Size);
}
