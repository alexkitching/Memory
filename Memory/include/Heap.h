#pragma once

#include <string>

#define MAX_HEAP_NAME_LEN 64

class Heap
{
public:
	struct AllocationHeader
	{
		Heap* pHeap; // Heap the Allocation is within
		size_t Size;
	};
	
	Heap();

	void Activate(const char* a_pName);
	//void Deactivate();
	//
	//void* Allocate(size_t a_size);
	//static void Deallocate(void* a_pMem);

	const char* GetName() const { return m_Name; }
	bool IsActive() const { return m_bActive; }

	void SetParent(Heap* a_pParent);

	void AddAllocation(size_t a_size);
	void RemoveAllocation(size_t a_size);

	size_t GetTotalAllocationSize() const { return m_TotalAllocatedSize; }
private:
	char m_Name[MAX_HEAP_NAME_LEN];
	bool m_bActive;

	// List
	Heap* m_pParent;
	Heap* m_pChild;
	Heap* m_pNextSibling;
	Heap* m_pPreviousSibling;

	size_t m_TotalAllocatedSize;
};
