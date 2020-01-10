#pragma once
#include <new>

class Heap;

#define USE_MEM_SYS 0

#define MEM_END 0xFFC00FED

void* operator new(std::size_t a_size, const char* a_pFile, int a_Line, const char* a_pFunc);

void* operator new(std::size_t a_size, Heap* a_pHeap);


// Global Operator Overrides
void* operator new(std::size_t a_size);
void* operator new[](std::size_t a_size);

void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;


#define MM_NEW new(__FILE__, __LINE__, __FUNCTION__)