#pragma once

#include <new>
#define MAX_HEAPS 10
class Heap;

class MemoryManager
{
public:
	static void Initialise();
	
	static void Shutdown();


	static Heap* GetDefaultHeap();

	static Heap* CreateHeapFromGlobal(const char* a_pName);
private:

	static Heap* ActivateEmptyHeap(const char* a_pName);
	
	static Heap* CreateHeap(const char* a_pName, const char* a_pParentName);
	static Heap* CreateHeap(const char* a_pName, Heap* a_pParent);
	static Heap* FindActiveHeap(const char* a_pName);
	
	static bool s_bInitialised;
	static Heap* s_pGlobalHeap;
	static Heap* s_pDefaultHeap;
};

void* operator new(std::size_t a_size, const char* a_pFile, int a_Line, const char* a_pFunc);

void* operator new(std::size_t a_size, Heap* a_pHeap);

void* operator new(std::size_t a_size);

void operator delete(void* ptr) noexcept;


#define MM_NEW new(__FILE__, __LINE__, __FUNCTION__)