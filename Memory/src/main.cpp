#include "MemoryApp.h"
#include "MemoryManager.h"
#include "Common.h"
#include "MemSys.h"
#include "Platform.h"

#ifdef x64
constexpr size_t MM_GLOBAL_MEM_SIZE = (size_t)3 * GB;
#else
constexpr size_t MM_GLOBAL_MEM_SIZE = (size_t) 1.6 * GB;
#endif


int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR lpCmdLine, int nCmdShow)
{
	try
	{
#if USE_MEM_SYS
		MemoryManager::Initialise(MM_GLOBAL_MEM_SIZE);
#endif
	}
	catch(std::exception e)
	{
		return 0;
	}
	
	MemoryApp App;
	return App.Run();
}
