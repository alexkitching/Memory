#pragma once
#include "HeapBase.h"
#include "Debug.h"
#include "Platform.h"

//------------
// Description
//--------------
// Moveable Heap & Moveable Pointer Classes, Enhances Default Heap with Defragmentation Functionality.
// Moveable Pointer works as a Pointer to Pointer. Each Allocation also allocates a "Bridge Pointer"
// at the end of the heap via Pool Allocator which the returned Moveable Pointer Points to.
// This Bridge pointer is updated to the new location of the moved memory during defragmentation maintain valid pointers.
//------------

#define HEAP_MOVEABLE_ALLOC_SIG 0xFEC0FFEE

template<typename T>
class MoveablePointer
{
	friend class MoveableAllocator;
public:
	MoveablePointer()
		:
		m_p(nullptr)
	{

	}

	MoveablePointer(T** a_p)
		:
		m_p(a_p)
	{

	}

	T* Get()
	{
		return  *m_p;
	}

	T* Get() const
	{
		return *m_p;
	}

	T* operator->()
	{
		return *m_p;
	}

	T* operator->() const
	{
		return *m_p;
	}

	T** GetAddress() const
	{
		return m_p;
	}

	void Release();

	bool IsNull() const { return m_p == nullptr; }

private:
	T** m_p;
};

// Forward Declartions
template<typename T, uint8 Alignment>
class PoolAllocator;

class MoveableHeap : public HeapBase
{
public:
	struct MoveableAllocationHeader
	{
		MoveableHeap* pHeap;
		size_t Size;
		MoveableAllocationHeader* pNext;
		MoveableAllocationHeader* pPrev; 
		void** BridgePtr; // Bridge Pointer Pointing to this Allocation
		uint32 Alignment; // Alignment Should be Stored to ensure alignment is maintained on defrags
		uint32 Sig;
	}; // 48 Bytes
	
	MoveableHeap() : m_pBridgePointerAllocator(nullptr) {}
	virtual ~MoveableHeap() {}

	virtual void Activate(Config& a_config) override;

	virtual bool IsMoveable() override { return true; }
	virtual size_t GetAllocHeaderSize() const override { return sizeof(MoveableAllocationHeader); }

	template<typename T>
	MoveablePointer<T> allocate(size_t a_size, uint8 a_alignment = DEFAULT_ALIGNMENT);
	template<typename T>
	static void Deallocate(MoveablePointer<T> a_p);
	
	void deallocate(MoveableAllocationHeader* a_pHeader);

	// Defragmentation Functions
	void Defragment();
	static inline void MoveAllocation(MoveableAllocationHeader*& a_pHeader, void* pDestination);

private:
	// Base Allocator Interface (Private due to replacement by Moveable Pointer versions, still utilised privately
	void* allocate(size_t a_size, uint8 a_alignment) override;
	void deallocate(void* a_pBlock) override;

	// Allocator for Bridge Pointers
	PoolAllocator<void*, DEFAULT_ALIGNMENT>* m_pBridgePointerAllocator;
};

template <typename T>
void MoveablePointer<T>::Release()
{
	if(m_p != nullptr)
	{
		MoveableHeap::Deallocate(*this); // Deallocate this Pointer
	}
}

template <typename T>
MoveablePointer<T> MoveableHeap::allocate(size_t a_size, uint8 a_alignment)
{
	// Allocate Header/Block
	void* pBlock = allocate(a_size, a_alignment);

	if (pBlock == nullptr)
	{
		return nullptr;
	}

	// Get The Header
	MoveableAllocationHeader* pHeader = reinterpret_cast<MoveableAllocationHeader*>(static_cast<char*>(pBlock) - sizeof(MoveableAllocationHeader));
	ASSERT(pHeader != nullptr && "Header is null");

	// Return Moveable Pointer Pointing at the Bridge Pointer
	return MoveablePointer<T>(reinterpret_cast<T**>(pHeader->BridgePtr));
}

template <typename T>
void MoveableHeap::Deallocate(MoveablePointer<T> a_p)
{
	char* pBlock = a_p.Get();

	// Get The Header
	MoveableAllocationHeader* pHeader = reinterpret_cast<MoveableAllocationHeader*>(static_cast<char*>(pBlock) - sizeof(MoveableAllocationHeader));
	ASSERT(pHeader != nullptr && "Header is null");
	ASSERT(pHeader->Sig == HEAP_MOVEABLE_ALLOC_SIG && "Expected Moveable Heap Signature?");

	// Deallocate Pointer on Heap Instance
	pHeader->pHeap->deallocate(pHeader);
}


