#pragma once
#include <new>

//------------
// Description
//--------------
// Memory System Header
// - Contains Primary Define for Toggling Usage of Custom Memory Management
// - Contains Overloaded Global allocation operators
//------------

class Heap;

#define USE_MEM_SYS 1

#define MEM_END 0xFFC00FED

void* operator new(std::size_t a_size, const char* a_pFile, int a_Line, const char* a_pFunc);
void* operator new(std::size_t a_size, Heap* a_pHeap);

// Global Operator Overrides
void* operator new(std::size_t a_size);
void* operator new[](std::size_t a_size);

void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;
