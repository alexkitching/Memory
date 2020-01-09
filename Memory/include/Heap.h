#pragma once

#include "Types.h"

#define MAX_HEAP_NAME_LEN 64

#define MEM_HEAP_SIG 0xFFC0FFEE

class Heap
{
public:
	struct Config
	{
		const char* Name;
		void* pStartPtr;
		size_t Capacity;
	};
	
	struct AllocationHeader
	{
		Heap* pHeap; // Heap the Allocation is within // 8 Bytes
		size_t Size; // 16 Bytes
		AllocationHeader* pNext; // 24 Bytes
		AllocationHeader* pPrev; // 32 Bytes
		//char padding;
		uint32 Sig; // 36 Bytes
	};
	
	Heap();

	void Activate(const Config& a_config);
	//void Deactivate();
	//
	void* Allocate(size_t a_size);
	static void Deallocate(void* a_pMem);
	void Deallocate(AllocationHeader* a_pHeader);

	const char* GetName() const { return m_Name; }
	bool IsActive() const { return m_bActive; }

	void SetParent(Heap* a_pParent);

	size_t GetTotalAllocationSize() const { return m_TotalAllocatedSize; }
private:

	AllocationHeader* TryAllocate(size_t a_size);
	void* TryAllocateBestFit(size_t a_size, AllocationHeader*& a_pPrevClosestAlloc) const;
	void* TryAllocateFirstFit(size_t a_size, AllocationHeader*& a_pPrevClosestAlloc);
	
	char m_Name[MAX_HEAP_NAME_LEN];
	bool m_bActive : 1;
	bool m_bFavourBestFit : 1;
	

	// List
	Heap* m_pParent;
	Heap* m_pChild;
	Heap* m_pNextSibling;
	Heap* m_pPreviousSibling;

	void* m_pStartPtr;
	size_t m_Capacity;

	// Allocations
	size_t m_TotalAllocatedSize;

	AllocationHeader* m_pHeadAlloc;
	AllocationHeader* m_pTailAlloc;
};
