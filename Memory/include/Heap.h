#pragma once

#include "Types.h"
#include "Platform.h"
#include "AllocatorBase.h"

#define MAX_HEAP_NAME_LEN 64

#define MEM_HEAP_SIG 0xFFC0FFEE

class Heap : public AllocatorBase
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
#ifdef x64
		uint32 padding;
#endif
		uint32 Sig; // 36 Bytes
	};

	Heap();
	virtual~Heap();

	// Prevent Copies
	Heap(const Heap&) = delete;
	Heap& operator=(const Heap&) = delete;

	void Activate(const Config& a_config);
	//void Deactivate();
	//
	virtual void* allocate(size_t a_size, uint8 a_alignment = 4u) override;
	static void Deallocate(void* a_pBlock);
	virtual void deallocate(void* a_pBlock) override;

	const char* GetName() const { return m_Name; }
	bool IsActive() const { return m_bActive; }

	void SetParent(Heap* a_pParent);
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

	AllocationHeader* m_pHeadAlloc;
	AllocationHeader* m_pTailAlloc;
};
