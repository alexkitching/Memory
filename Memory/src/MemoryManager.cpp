#include "MemoryManager.h"
#include "Heap.h"
#include "Debug.h"


#define OVERRIDE_NEW 1

bool MemoryManager::s_bInitialised = false;
Heap* MemoryManager::s_pGlobalHeap = nullptr;
Heap* MemoryManager::s_pDefaultHeap = nullptr;

#define MM_ASSERT_INIT() ASSERT(s_bInitialised && "Memory Manager not Initialised!")

constexpr const char* GLOBAL_HEAP_NAME = "Global";
constexpr const char* DEFAULT_HEAP_NAME = "Default";

static Heap s_Heaps[MAX_HEAPS];

#if OVERRIDE_NEW // Overloaded Operators
void* operator new(std::size_t a_size, const char* a_pFile, int a_Line, const char* a_pFunc)
{
	void* pPtr = operator new(a_size, MemoryManager::GetDefaultHeap());
	if(pPtr != nullptr)
	{
		return pPtr;
	}
	throw std::bad_alloc();
}

void* operator new(size_t a_size, Heap* a_pHeap)
{
	const size_t ReqBytes = sizeof(Heap::AllocationHeader) + a_size;
	char* pMem = (char*)malloc(ReqBytes);

	Heap::AllocationHeader* pHeader = (Heap::AllocationHeader*)pMem;

	if(pHeader != nullptr)
	{
		pHeader->pHeap = a_pHeap;
		pHeader->Size = a_size;
	}
	

	a_pHeap->AddAllocation(a_size);

	void* pStartBlock = pMem + sizeof(Heap::AllocationHeader);
	return pStartBlock;
}

// Default Globals
void* operator new(std::size_t a_size)
{
	void* pPtr = operator new(a_size, MemoryManager::GetDefaultHeap());

	if (pPtr)
		return pPtr;

	throw std::bad_alloc();
}

void operator delete(void* a_pPtr)
{
	char* pPtr = (char*)a_pPtr;
	pPtr -= sizeof(Heap::AllocationHeader);

	Heap::AllocationHeader* pHeader = (Heap::AllocationHeader*)(pPtr);

	pHeader->pHeap->RemoveAllocation(pHeader->Size);
	
	std::free(pPtr);
}


#else // STANDARD OPERATORS
void* operator new(size_t a_size)
{

	void* pPtr = malloc(a_size);
	if(pPtr)
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

void MemoryManager::Initialise()
{
	ASSERT(s_bInitialised == false && "Trying to Init Twice!");

	s_pGlobalHeap = ActivateEmptyHeap(GLOBAL_HEAP_NAME);
	s_pDefaultHeap = CreateHeapFromGlobal(DEFAULT_HEAP_NAME);

	s_bInitialised = true;
}

void MemoryManager::Shutdown()
{
}

Heap* MemoryManager::GetDefaultHeap()
{
	MM_ASSERT_INIT();

	return s_pDefaultHeap;
}

Heap* MemoryManager::ActivateEmptyHeap(const char* a_pName)
{
	for(int i = 0; i < MAX_HEAPS; ++i)
	{
		Heap* pHeap = &s_Heaps[i];
		if(pHeap->IsActive() == false)
		{
			pHeap->Activate(a_pName);
			return pHeap;
		}
	}
	
	return nullptr;
}

Heap* MemoryManager::CreateHeapFromGlobal(const char* a_pName)
{
	return CreateHeap(a_pName, s_pGlobalHeap);
}

Heap* MemoryManager::CreateHeap(const char* a_pName, const char* a_pParentName)
{
	Heap* pParent = FindActiveHeap(a_pParentName);
	ASSERT(pParent != nullptr && "No Parent Heap Found!");
	ASSERT(FindActiveHeap(a_pName) == nullptr && "Heap with identical name already exists!");

	Heap* pChildHeap = CreateHeap(a_pName, pParent);
	pChildHeap->SetParent(pParent);

	return pChildHeap;
}

Heap* MemoryManager::CreateHeap(const char* a_pName, Heap* a_pParent)
{
	ASSERT(a_pParent != nullptr && "Parent Heap is null!");
	ASSERT(FindActiveHeap(a_pName) == nullptr && "Heap with identical name already exists!");

	Heap* pHeap = ActivateEmptyHeap(a_pName);
	pHeap->SetParent(a_pParent);
	return pHeap;
}

Heap* MemoryManager::FindActiveHeap(const char* a_pName)
{
	for(int i = 0; i < MAX_HEAPS; ++i)
	{
		Heap* pHeap = &s_Heaps[i];
		if(pHeap->IsActive() == false)
		{
			continue;
		}

		if(_strcmpi(pHeap->GetName(), a_pName) == 0)
		{
			return pHeap;
		}
	}
	return nullptr;
}
