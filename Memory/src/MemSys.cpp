#include "MemSys.h"
#include "MemoryManager.h"
#include "Heap.h"
#include "Debug.h"
#include "Types.h"



#define OVERRIDE_NEW USE_MEM_SYS

#if OVERRIDE_NEW // Overloaded Operators
void* operator new(std::size_t a_size, const char* a_pFile, int a_Line, const char* a_pFunc)
{
	void* pPtr = operator new(a_size, MemoryManager::GetDefaultHeap());
	if (pPtr != nullptr)
	{
		return pPtr;
	}
	throw std::bad_alloc();
}

// Heap Allocation
void* operator new(size_t a_size, Heap* a_pHeap)
{
	ASSERT(a_pHeap != nullptr && "Heap was nullptr!");
	return a_pHeap->Allocate(a_size);
}

// Default Globals
void* operator new(std::size_t a_size)
{
	void* pPtr = operator new(a_size, MemoryManager::GetDefaultHeap());

	if (pPtr)
		return pPtr;

	throw std::bad_alloc();
}

void* operator new [](size_t a_size)
{
	void* pPtr = operator new(a_size, MemoryManager::GetDefaultHeap());

	if (pPtr)
		return pPtr;

	throw std::bad_alloc();
}

void operator delete(void* a_pPtr)
{
	if(a_pPtr != nullptr)
	{
		//Sizes
		size_t heapSize = sizeof(Heap::AllocationHeader);
		size_t gSize = sizeof(MemoryManager::GlobalAllocationHeader);
		
		auto pHeader = (Heap::AllocationHeader*)((char*)a_pPtr - heapSize);
		
		
		const auto pGlobalHeader = (MemoryManager::GlobalAllocationHeader*)((char*)a_pPtr - gSize);
		
		if(pHeader->Sig == MEM_HEAP_SIG)
		{
			Heap::Deallocate(a_pPtr);
		}
		else
		{
			UNREACHABLE("Unknown Signature");
		}
		
	}
}

void operator delete [](void* _Block) noexcept
{
	if (_Block != nullptr)
	{
		const auto pGlobalHeader = (MemoryManager::GlobalAllocationHeader*)((char*)_Block -
			sizeof(MemoryManager::GlobalAllocationHeader));

		Heap::AllocationHeader* pHeader = (Heap::AllocationHeader*)((char*)_Block - sizeof(Heap::AllocationHeader));

		if (pHeader->Sig == MEM_HEAP_SIG)
		{
			Heap::Deallocate(_Block);
		}
		else
		{
			UNREACHABLE("Unknown Signature");
		}

	}
}


#else // STANDARD OPERATORS
void* operator new(size_t a_size)
{

	void* pPtr = malloc(a_size);
	if (pPtr)
	{
		return pPtr;
	}

	throw std::bad_alloc();
}

void operator delete(void* a_pPtr)
{
	std::free(a_pPtr);
}
#endif