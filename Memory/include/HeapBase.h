#pragma once

#include "AllocatorBase.h"

#define HEAP_MAX_NAME_LEN 64

class HeapBase : public AllocatorBase
{
public:
	typedef uint32 HeapSignature;
	struct Config
	{
		const char* Name;
		void* pStartPtr;
		size_t Capacity;
	};

	struct BaseAllocationHeader
	{
		HeapBase* pHeap; // 8 
		size_t Size;
		BaseAllocationHeader* pNext;
		BaseAllocationHeader* pPrev; // 32 Bytes
#ifdef x64
		uint32 padding;
#endif
		HeapSignature Sig;
	}; // 24 Bytes

	HeapBase();
	virtual ~HeapBase();

	virtual void Activate(Config& a_config);

	const char* GetName() const { return m_Name; }
	bool IsActive() const { return m_bActive; }
	virtual bool IsMoveable() { return false; }

	void SetParent(HeapBase* a_pParent);
	const HeapBase* GetParent() const { return m_pParent; }
	const HeapBase* GetNextSibling() const { return m_pNextSibling; }
	const HeapBase* GetPreviousSibling() const { return m_pPreviousSibling; }
	const HeapBase* GetChild() const { return m_pChild; }

	virtual size_t GetAllocHeaderSize() const { return sizeof(BaseAllocationHeader); }

	
	float CalculateFragmentation() const;

protected:
	BaseAllocationHeader* TryAllocate(size_t a_size, uint8 a_alignment);
	BaseAllocationHeader* TryAllocateBestFit(size_t a_size, uint8 a_alignment, BaseAllocationHeader*& a_pPrevClosestAlloc) const;
	BaseAllocationHeader* TryAllocateFirstFit(size_t a_size, uint8 a_alignment, BaseAllocationHeader*& a_pPrevClosestAlloc);
	
	// List
	HeapBase* m_pParent;
	HeapBase* m_pChild;
	HeapBase* m_pNextSibling;
	HeapBase* m_pPreviousSibling;

	char m_Name[HEAP_MAX_NAME_LEN];
	bool m_bActive : 1;
	bool m_bFavourBestFit : 1;

	BaseAllocationHeader* m_pHeadAlloc;
	BaseAllocationHeader* m_pTailAlloc;
};