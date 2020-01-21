#include "MemoryManager.h"
#include "Debug.h"
#include "Common.h"
#include "Profiler.h"

#define MM_ASSERT_INIT() ASSERT(s_bInitialised && "Memory Manager not Initialised!")

constexpr const char* GLOBAL_HEAP_NAME = "Global";
constexpr const char* DEFAULT_HEAP_NAME = "Default";

constexpr size_t DEFAULT_HEAP_SIZE = 512 * MB;

bool MemoryManager::s_bInitialised = false;
Heap* MemoryManager::s_pGlobalHeap = nullptr;
Heap* MemoryManager::s_pDefaultHeap = nullptr;
Heap MemoryManager::s_Heaps[MAX_HEAPS] = { };
MoveableHeap MemoryManager::s_MoveableHeaps[MAX_MOVEABLE_HEAPS] = {};

void MemoryManager::Initialise(size_t a_maxGlobalMem)
{
	ASSERT(s_bInitialised == false && "Trying to Init Twice!");

	// Allocate Global Memory
	void* pGlobalMem = malloc(a_maxGlobalMem);
	if(pGlobalMem == nullptr)
	{
		UNREACHABLE("Failed to Allocate Global Memory");
		return;
	}

	// Set All Memory to 0
	memset(pGlobalMem, 0, a_maxGlobalMem);

	// Activate Global Heap
	Heap::Config GlobalConfig
	{
		GLOBAL_HEAP_NAME,
		pGlobalMem,
		a_maxGlobalMem
	};
	
	s_pGlobalHeap = ActivateEmptyHeap(GlobalConfig);

	// Activate Default Heap
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
		if(pHeap->IsActive() == false) // Heap is not active
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
	PROFILER_BEGIN_SAMPLE(MemoryManager::DefragmentHeaps);
	RecursiveDefragHeap(s_pGlobalHeap); // Runs Defrag across all moveable heaps
	PROFILER_END_SAMPLE();
}

void MemoryManager::RecursiveDefragHeap(HeapBase* a_pHeap)
{
	if(a_pHeap->IsMoveable())
	{
		MoveableHeap* pMoveableHeap = (MoveableHeap*)a_pHeap;
		pMoveableHeap->Defragment();
	}

	if(a_pHeap->m_pChild != nullptr)
	{
		RecursiveDefragHeap(a_pHeap->m_pChild);
	}

	if(a_pHeap->m_pNextSibling)
	{
		RecursiveDefragHeap(a_pHeap->m_pNextSibling);
	}
}

Heap* MemoryManager::CreateHeap(Heap::Config& a_config, const char* a_pParentName)
{
	// Try Find Parent
	Heap* pParent = FindActiveHeap(a_pParentName);
	ASSERT(pParent != nullptr && "No Parent Heap Found!");
	ASSERT(pParent->IsMoveable() == false && "Parent Heap cannot be moveable!");
	ASSERT(FindActiveHeap(a_config.Name) == nullptr && "Heap with identical name already exists!");

	// Create/Parent Heap
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

MoveableHeap* MemoryManager::CreateMoveableHeapFromGlobal(Heap::Config& a_config)
{
	return CreateMoveableHeap(a_config, s_pGlobalHeap);
}

MoveableHeap* MemoryManager::CreateMoveableHeap(MoveableHeap::Config& a_config, const char* a_pParentName)
{
	// Try find Parent
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

void MemoryManager::ReleaseHeap(HeapBase* a_pHeap)
{
	ASSERT(a_pHeap->m_pChild == nullptr && "Child Heap Should be released first");
	ASSERT(a_pHeap->GetAllocationCount() == 0 && "Existing Allocations still live!");

	HeapBase* pParent = a_pHeap->m_pParent;
	pParent->deallocate(a_pHeap->m_pStart); // Deallocate Heap from Parent
	
	// Unparent Heap
	a_pHeap->SetParent(nullptr);

	// Deactivate Heap
	a_pHeap->Deactivate();
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

		if(_strcmpi(pHeap->GetName(), a_pName) == 0) // Name Matches
		{
			return pHeap;
		}
	}
	return nullptr;
}
