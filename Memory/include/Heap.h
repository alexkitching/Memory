#pragma once

#include "Types.h"
#include "Platform.h"
#include "HeapBase.h"

//------------
// Description
//--------------
// Concrete Heap Class containing allocator base interface implementations
//------------


#define HEAP_ALLOC_SIG 0xFFC0FFEE // Heap Allocation Signature (Stored at the end of heap alloc data for ensuring no memory corruption)

class Heap : public HeapBase
{
public:
	Heap() {}
	virtual~Heap() {}

	// Allocator Base Interface
	virtual void* allocate(size_t a_size, uint8 a_alignment = DEFAULT_ALIGNMENT) override;
	virtual void deallocate(void* a_pBlock) override;

	// Static Deallocate allowing deallocation with no reference to original heap instance
	static void Deallocate(void* a_pBlock);
};
