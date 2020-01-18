#pragma once
#include "HeapBase.h"
#include "Debug.h"
#include "Platform.h"

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
		MoveableAllocationHeader* pPrev; //32 
		void** BridgePtr; // 40
		uint32 Alignment;
		uint32 Sig;
	}; // 48 Bytes
	
	MoveableHeap();
	virtual ~MoveableHeap() {}

	virtual void Activate(Config& a_config) override;

	virtual bool IsMoveable() override { return true; }
	virtual size_t GetAllocHeaderSize() const override { return sizeof(MoveableAllocationHeader); }

	template<typename T>
	MoveablePointer<T> allocate(size_t a_size, uint8 a_alignment = DEFAULT_ALIGNMENT);
	template<typename T>
	static void Deallocate(MoveablePointer<T> a_p);
	
	void deallocate(MoveableAllocationHeader* a_pHeader);

private:
	void* allocate(size_t a_size, uint8 a_alignment) override;
	void deallocate(void* a_pBlock) override;

	PoolAllocator<void*, DEFAULT_ALIGNMENT>* m_pBridgePointerAllocator;
};

template <typename T>
void MoveablePointer<T>::Release()
{
	if(m_p != nullptr)
	{
		MoveableHeap::Deallocate(*this);
	}
}

template <typename T>
MoveablePointer<T> MoveableHeap::allocate(size_t a_size, uint8 a_alignment)
{
	void* pBlock = allocate(a_size, a_alignment);

	if (pBlock == nullptr)
	{
		return nullptr;
	}
		

	MoveableAllocationHeader* pHeader = (MoveableAllocationHeader*)((char*)pBlock - sizeof(MoveableAllocationHeader));
	ASSERT(pHeader != nullptr && "Header is null");

	
	return MoveablePointer<T>((T**)pHeader->BridgePtr);
}

template <typename T>
void MoveableHeap::Deallocate(MoveablePointer<T> a_p)
{
	char* pBlock = a_p.Get();

	MoveableAllocationHeader* pHeader = (MoveableAllocationHeader*)((char*)pBlock - sizeof(MoveableAllocationHeader));
	ASSERT(pHeader != nullptr && "Header is null");
	ASSERT(pHeader->Sig == HEAP_MOVEABLE_ALLOC_SIG && "Expected Moveable Heap Signature?");

	pHeader->pHeap->deallocate(pHeader);
}


