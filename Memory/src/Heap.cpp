#include "Heap.h"
#include "Debug.h"

Heap::Heap()
	:
m_Name{"INVALID"},
m_bActive(false),
m_pParent(nullptr),
m_pChild(nullptr),
m_pNextSibling(nullptr),
m_pPreviousSibling(nullptr),
m_TotalAllocatedSize(0u)
{
}

void Heap::Activate(const char* a_pName)
{
	ASSERT(a_pName != nullptr && "Name cannot be Null!");
	
	// Reset Vars Here
	m_bActive = true;
	strcpy_s(m_Name, MAX_HEAP_NAME_LEN, a_pName);
}

void Heap::SetParent(Heap* a_pParent)
{
	ASSERT(a_pParent != nullptr && "Parent Heap is null!");
	
	if (m_pParent == a_pParent) // Avoid Reparenting
		return;

	// Detach from Siblings if any
	if(m_pPreviousSibling != nullptr)
	{
		m_pPreviousSibling->m_pNextSibling = m_pNextSibling;
	}

	if(m_pNextSibling != nullptr)
	{
		m_pNextSibling->m_pPreviousSibling = m_pPreviousSibling;
	}

	// Unparent
	if(m_pParent != nullptr && // We have a parent
		m_pParent->m_pChild == this) // Direct Child is us
	{
		m_pParent->m_pChild = m_pNextSibling; // Set to Next Sibling
	}

	// Now Parent to new parent
	m_pParent = a_pParent;
	m_pPreviousSibling = nullptr;
	// Insert as First Child
	m_pNextSibling = m_pParent->m_pChild;
	m_pParent->m_pChild = this;
}

void Heap::AddAllocation(size_t a_size)
{
	m_TotalAllocatedSize += a_size;
}

void Heap::RemoveAllocation(size_t a_size)
{
	m_TotalAllocatedSize -= a_size;
}
