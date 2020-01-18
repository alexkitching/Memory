#pragma once

#include "Heap.h"
#include "MoveableHeap.h"
#define MAX_HEAPS 10
#define MAX_MOVEABLE_HEAPS 10

class MemoryManager
{
public:
	struct GlobalAllocationHeader
	{
		uint32 AllocTypeSig;
	};
	
	static void Initialise(size_t a_maxGlobalMem);
	
	static void Shutdown();

	static Heap* GetDefaultHeap();

	static Heap* CreateHeapFromGlobal(Heap::Config& a_config);
	static Heap* CreateHeap(Heap::Config& a_config, const char* a_pParentName);
	static Heap* CreateHeap(Heap::Config& a_config, Heap* a_pParent);

	static MoveableHeap* CreateMoveableHeap(MoveableHeap::Config& a_config, const char* a_pParentName);
	static MoveableHeap* CreateMoveableHeap(MoveableHeap::Config& a_config, Heap* a_pParent);
	
	static void* New(size_t a_size);
	static void Delete(void* a_pPtr);

	static void DefragmentHeaps();
	
private:

	static Heap* ActivateEmptyHeap(Heap::Config& a_config);
	static MoveableHeap* ActivateEmptyMoveableHeap(Heap::Config& a_config);

	static Heap* FindActiveHeap(const char* a_pName);
	
	static bool s_bInitialised;
	static Heap* s_pGlobalHeap;
	static Heap* s_pDefaultHeap;
	static Heap s_Heaps[MAX_HEAPS];
	static MoveableHeap s_MoveableHeaps[MAX_MOVEABLE_HEAPS];
};

#define MM_NEW(size) MemoryManager::New(size)
#define MM_DELETE(ptr) MemoryManager::Delete(ptr)