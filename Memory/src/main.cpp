#include "MemoryApp.h"
#include "MemoryManager.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPreviousInstance, LPSTR lpCmdLine, int nCmdShow)
{
	MemoryManager::Initialise();
	MemoryApp App;
	return App.Run();
}
