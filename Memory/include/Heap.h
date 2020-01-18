#pragma once

#include "Types.h"
#include "Platform.h"
#include "HeapBase.h"


#define HEAP_ALLOC_SIG 0xFFC0FFEE

class Heap : public HeapBase
{
public:
	Heap();
	virtual~Heap();

	virtual void* allocate(size_t a_size, uint8 a_alignment = DEFAULT_ALIGNMENT) override;
	static void Deallocate(void* a_pBlock);
	virtual void deallocate(void* a_pBlock) override;

	
	BaseAllocationHeader* GetHeadAllocation() const { return m_pHeadAlloc; }
	BaseAllocationHeader* GetTailAllocation() const { return m_pTailAlloc; }	
};
