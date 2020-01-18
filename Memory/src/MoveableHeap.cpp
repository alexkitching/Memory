#include "MoveableHeap.h"
#include "PointerMath.h"
#include "MemSys.h"
#include "PoolAllocator.h"

#define MOVEABLE_HEAP_ALLOC_HEADER_FOOTER_SIZE (sizeof(MoveableHeap::MoveableAllocationHeader) + sizeof(unsigned int))

#define MOVEABLE_BRIDGE_POINTER_BUFFER_SIZE 5 * MB

MoveableHeap::MoveableHeap()
{
}

void MoveableHeap::Activate(Config& a_config)
{
	// Reserve Bridge Buffer Space
	char* pEnd = (char*)a_config.pStartPtr + a_config.Capacity - 1;
	a_config.Capacity -= MOVEABLE_BRIDGE_POINTER_BUFFER_SIZE;

	// Create Bridge Pointer Allocator
	m_pBridgePointerAllocator = new PoolAllocator<void*, DEFAULT_ALIGNMENT>(MOVEABLE_BRIDGE_POINTER_BUFFER_SIZE, pEnd - MOVEABLE_BRIDGE_POINTER_BUFFER_SIZE);
	
	HeapBase::Activate(a_config);
}

void MoveableHeap::deallocate(MoveableAllocationHeader* a_pHeader)
{
	// Deallocate Bridge Pointer
	m_pBridgePointerAllocator->deallocate(a_pHeader->BridgePtr);
	// Deallocate Memory Block
	deallocate((char*)a_pHeader + sizeof(MoveableAllocationHeader));
}

void* MoveableHeap::allocate(size_t a_size, uint8 a_alignment)
{
	MoveableAllocationHeader* pHeader = (MoveableAllocationHeader*)TryAllocate(a_size, a_alignment);

	int breakHere = 0;
	if(pHeader == (MoveableAllocationHeader*)m_pTailAlloc)
	{
		breakHere++;
	}

	int breakHERE = breakHere;

	if (pHeader == nullptr)
		return nullptr;

#if DEBUG // Additional Safety Checks
	if (pHeader->pNext != nullptr)
	{
		bool isValid = ((char*)pHeader + MOVEABLE_HEAP_ALLOC_HEADER_FOOTER_SIZE + pHeader->Size) <= (char*)pHeader->pNext;
		ASSERT(isValid);
	}

	if (pHeader->pPrev != nullptr)
	{
		bool isValid = ((char*)pHeader->pPrev + MOVEABLE_HEAP_ALLOC_HEADER_FOOTER_SIZE + pHeader->pPrev->Size) <= (char*)pHeader;
		ASSERT(isValid);
	}
#endif

	pHeader->Alignment = a_alignment;
	pHeader->Sig = HEAP_MOVEABLE_ALLOC_SIG;

	char* pBlockStart = (char*)pHeader + sizeof(MoveableAllocationHeader);

	// Allocate Bridge Pointer
	void** pBridge = (void**)m_pBridgePointerAllocator->allocate(sizeof(void**), DEFAULT_ALIGNMENT);

	*pBridge = pBlockStart;
	pHeader->BridgePtr = pBridge;

	// Stamp End
	uint32* pEndMark = (uint32*)(pBlockStart + a_size);
	*pEndMark = MEM_END;
	
	// Increase Total Allocation Size
	m_usedSize += MOVEABLE_HEAP_ALLOC_HEADER_FOOTER_SIZE + pHeader->Size;
#if DEBUG
	m_allocationCount++;
#endif

	return pBlockStart;
}

void MoveableHeap::deallocate(void* a_pBlock)
{
	auto pHeader = (BaseAllocationHeader*)((char*)a_pBlock - GetAllocHeaderSize());
	uint32* pEndMark = (uint32*)((char*)a_pBlock + pHeader->Size);
	ASSERT(*pEndMark == MEM_END && "Missing End Mark, Corrupt Allocation?");

	// Head------->Tail
	if(pHeader->pPrev == nullptr &&
		pHeader->pNext == nullptr)
	{
		m_pHeadAlloc = nullptr;
		m_pTailAlloc = nullptr;
	}
	else if (pHeader->pPrev == nullptr) // Must be Head 
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

	m_usedSize -= sizeof(MoveableAllocationHeader) + sizeof(HeapSignature) + pHeader->Size;
#if DEBUG
	--m_allocationCount;
#endif
	// Free
	memset(pHeader, 0, sizeof(MoveableAllocationHeader) + sizeof(HeapSignature) + pHeader->Size);
}
