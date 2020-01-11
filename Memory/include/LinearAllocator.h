#pragma once

#include "AllocatorBase.h"

class LinearAllocator : public AllocatorBase
{
public:

	LinearAllocator() = default;
	LinearAllocator(size_t a_capacity, void* a_pStart);

	virtual void* allocate(size_t a_size, uint8 a_alignment) override;

	virtual void deallocate(void* a_pBlock) override;

	void Clear();

private:

	void* m_pCurrentPos;

};