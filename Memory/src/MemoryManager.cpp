#include "MemoryManager.h"
#include "Debug.h"
#include "Common.h"

bool MemoryManager::s_bInitialised = false;
Heap* MemoryManager::s_pGlobalHeap = nullptr;
Heap* MemoryManager::s_pDefaultHeap = nullptr;

#define MM_ASSERT_INIT() ASSERT(s_bInitialised && "Memory Manager not Initialised!")

constexpr const char* GLOBAL_HEAP_NAME = "Global";
constexpr const char* DEFAULT_HEAP_NAME = "Default";

constexpr size_t DEFAULT_HEAP_SIZE = 512 * MB;

Heap MemoryManager::s_Heaps[MAX_HEAPS] = { };
MoveableHeap MemoryManager::s_MoveableHeaps[MAX_MOVEABLE_HEAPS] = {};

void MemoryManager::Initialise(size_t a_maxGlobalMem)
{
	ASSERT(s_bInitialised == false && "Trying to Init Twice!");
	
	void* pGlobalMem = malloc(a_maxGlobalMem);
	if(pGlobalMem == nullptr)
	{
		UNREACHABLE("Failed to Allocate Global Memory");
		return;
	}

	// Set All Memory to 0
	memset(pGlobalMem, 0, a_maxGlobalMem);

	Heap::Config GlobalConfig
	{
		GLOBAL_HEAP_NAME,
		pGlobalMem,
		a_maxGlobalMem
	};
	
	s_pGlobalHeap = ActivateEmptyHeap(GlobalConfig);

	Heap::Config DefaultConfig
	{
		DEFAULT_HEAP_NAME,
		nullptr, // To be Allocated from Global
		a_maxGlobalMem - (sizeof(Heap::BaseAllocationHeader) + sizeof(HeapBase::HeapSignature))
	};
	
	s_pDefaultHeap = CreateHeapFromGlobal(DefaultConfig);

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

Heap* MemoryManager::ActivateEmptyHeap(Heap::Config& a_config)
{
	for(int i = 0; i < MAX_HEAPS; ++i)
	{
		Heap* pHeap = &s_Heaps[i];
		if(pHeap->IsActive() == false)
		{
			pHeap->Activate(a_config);
			return pHeap;
		}
	}
	
	return nullptr;
}

MoveableHeap* MemoryManager::ActivateEmptyMoveableHeap(Heap::Config& a_config)
{
	for (int i = 0; i < MAX_MOVEABLE_HEAPS; ++i)
	{
		MoveableHeap* pHeap = &s_MoveableHeaps[i];
		if (pHeap->IsActive() == false)
		{
			pHeap->Activate(a_config);
			return pHeap;
		}
	}

	return nullptr;
}

Heap* MemoryManager::CreateHeapFromGlobal(Heap::Config& a_config)
{
	return CreateHeap(a_config, s_pGlobalHeap);
}

void* MemoryManager::New(size_t a_size)
{
	void* pPtr = GetDefaultHeap()->allocate(a_size);

	if (pPtr)
		return pPtr;

	throw std::bad_alloc();
}

void MemoryManager::Delete(void* a_pPtr)
{
	if (a_pPtr != nullptr)
	{
		const auto pGlobalHeader = (MemoryManager::GlobalAllocationHeader*)((char*)a_pPtr -
			sizeof(MemoryManager::GlobalAllocationHeader));

		if (pGlobalHeader->AllocTypeSig == HEAP_ALLOC_SIG)
		{
			Heap::Deallocate(a_pPtr);
		}
		else
		{
			UNREACHABLE("Unknown Signature");
		}
	}
}

void MemoryManager::DefragmentHeaps()
{
	
}

Heap* MemoryManager::CreateHeap(Heap::Config& a_config, const char* a_pParentName)
{
	Heap* pParent = FindActiveHeap(a_pParentName);
	ASSERT(pParent != nullptr && "No Parent Heap Found!");
	ASSERT(pParent->IsMoveable() == false && "Parent Heap cannot be moveable!");
	ASSERT(FindActiveHeap(a_config.Name) == nullptr && "Heap with identical name already exists!");

	Heap* pChildHeap = CreateHeap(a_config, pParent);
	pChildHeap->SetParent(pParent);

	return pChildHeap;
}

Heap* MemoryManager::CreateHeap(Heap::Config& a_config, Heap* a_pParent)
{
	ASSERT(a_pParent != nullptr && "Parent Heap is null!");
	ASSERT(a_pParent->IsMoveable() == false && "Parent Heap cannot be moveable!");
	ASSERT(FindActiveHeap(a_config.Name) == nullptr && "Heap with identical name already exists!");

	void* pHeapStart = a_pParent->allocate(a_config.Capacity);

	if (pHeapStart == nullptr) // Failed to Allocate
		return nullptr;

	a_config.pStartPtr = pHeapStart;
	
	Heap* pHeap = ActivateEmptyHeap(a_config);
	pHeap->SetParent(a_pParent);
	return pHeap;
}

MoveableHeap* MemoryManager::CreateMoveableHeap(MoveableHeap::Config& a_config, const char* a_pParentName)
{
	Heap* pParent = FindActiveHeap(a_pParentName);
	ASSERT(pParent != nullptr && "No Parent Heap Found!");
	ASSERT(pParent->IsMoveable() == false && "Parent Heap cannot be moveable!");
	ASSERT(FindActiveHeap(a_config.Name) == nullptr && "Heap with identical name already exists!");

	MoveableHeap* pChildHeap = CreateMoveableHeap(a_config, pParent);
	pChildHeap->SetParent(pParent);

	return pChildHeap;
}

MoveableHeap* MemoryManager::CreateMoveableHeap(MoveableHeap::Config& a_config, Heap* a_pParent)
{
	ASSERT(a_pParent != nullptr && "Parent Heap is null!");
	ASSERT(a_pParent->IsMoveable() == false && "Parent Heap cannot be moveable!");
	ASSERT(FindActiveHeap(a_config.Name) == nullptr && "Heap with identical name already exists!");

	void* pHeapStart = a_pParent->allocate(a_config.Capacity);

	if (pHeapStart == nullptr) // Failed to Allocate
		return nullptr;

	a_config.pStartPtr = pHeapStart;

	MoveableHeap* pHeap = ActivateEmptyMoveableHeap(a_config);
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
