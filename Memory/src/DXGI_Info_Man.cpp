#if DEBUG
#include "DXGI_Info_Man.h"
#include "Window.h"
#include "D3D.h"



#pragma comment(lib, "dxguid.lib")

DXGIInfoManager* g_pInfoMan = nullptr;


DXGIInfoManager::DXGIInfoManager()
{
	// DXGI Interface Func Signature
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	// Load DLL containing the function
	const auto hModDXGIDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if(hModDXGIDebug == nullptr)
	{
		throw WND_Last_Except();
	}

	// Get the Address of the interface function
	const auto DXGIGetDebugInterfaceFunc = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDXGIDebug, "DXGIGetDebugInterface")));

	if(DXGIGetDebugInterfaceFunc == nullptr)
	{
		throw WND_Last_Except();
	}

	// Call the function to get the interface
#if DEBUG
	HRESULT hr;
#endif
	D3D_THROW_FAILED(DXGIGetDebugInterfaceFunc(__uuidof(IDXGIInfoQueue), (void**)&m_pDXGI_InfoQueue));
}

void DXGIInfoManager::Initialise()
{
	if (g_pInfoMan != nullptr)
		return;
	
	g_pInfoMan = new DXGIInfoManager();
}

DXGIInfoManager* DXGIInfoManager::Instance()
{
	return g_pInfoMan;
}

void DXGIInfoManager::Shutdown()
{
	if (g_pInfoMan == nullptr)
		return;
	
	delete g_pInfoMan;
}

void DXGIInfoManager::Set()
{
	// Set the index of next index
	m_next = m_pDXGI_InfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	m_msgs.clear();
}

std::vector<std::string>& DXGIInfoManager::GetMessages()
{
	const auto end = m_pDXGI_InfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for(unsigned long long i = m_next; i < end; ++i)
	{
		HRESULT hr;
		SIZE_T messageLen;

		// Get Size of Current Message
		D3D_THROW_FAILED_NOINFO(m_pDXGI_InfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLen))

		DXGI_INFO_QUEUE_MESSAGE* pMessage = (DXGI_INFO_QUEUE_MESSAGE*)new byte[messageLen];

		D3D_THROW_FAILED_NOINFO(m_pDXGI_InfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLen));

		m_msgs.emplace_back(pMessage->pDescription);

		delete[] pMessage;
	}

	return m_msgs;
}

#endif