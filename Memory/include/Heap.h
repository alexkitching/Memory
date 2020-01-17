#pragma once

#include "Types.h"
#include "Platform.h"
#include "AllocatorBase.h"

#define HEAP_MAX_NAME_LEN 64

#define HEAP_ALLOC_SIG 0xFFC0FFEE

#define HEAP_ALLOC_HEADER_FOOTER_SIZE (sizeof(Heap::AllocationHeader) + sizeof(unsigned int))

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
		Heap* pHeap;
		size_t Size; 
		AllocationHeader* pNext;
		AllocationHeader* pPrev;
		uint32 Alignment;
		uint32 Sig; 
	}; // 24 Bytes x64, 20 x32

	Heap();
	virtual~Heap();

	// Prevent Copies (TODO Remove?)
	Heap(const Heap&) = delete;
	Heap& operator=(const Heap&) = delete;

	void Activate(const Config& a_config);
	//void Deactivate();
	//
	virtual void* allocate(size_t a_size, uint8 a_alignment = DEFAULT_ALIGNMENT) override;
	static void Deallocate(void* a_pBlock);
	virtual void deallocate(void* a_pBlock) override;

	// Fragmentation/Defragmentation
	float CalculateFragmentation() const;
	void Defragment();

	const char* GetName() const { return m_Name; }
	bool IsActive() const { return m_bActive; }

	void SetParent(Heap* a_pParent);

	AllocationHeader* GetHeadAllocation() const { return m_pHeadAlloc; }
	AllocationHeader* GetTailAllocation() const { return m_pTailAlloc; }
private:

	AllocationHeader* TryAllocate(size_t a_size, uint8 a_alignment);
	AllocationHeader* TryAllocateBestFit(size_t a_size, uint8 a_alignment, AllocationHeader*& a_pPrevClosestAlloc) const;
	AllocationHeader* TryAllocateFirstFit(size_t a_size, uint8 a_alignment, AllocationHeader*& a_pPrevClosestAlloc);

	inline bool CapacityWouldExceed(size_t a_size) const;
	
	char m_Name[HEAP_MAX_NAME_LEN];
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
