#pragma once

#include "Win.h"
#include "Exception.h"
#include <vector>
#include <wrl.h>
#include <dxgidebug.h>

class DXGIInfoManager
{
public:
	
	static void Initialise();
	static DXGIInfoManager* Instance();
	static void Shutdown();
	
	void Set();
	std::vector<std::string> GetMessages();
	
private:
	DXGIInfoManager();
	~DXGIInfoManager() {}
	DXGIInfoManager(const DXGIInfoManager&) = delete;
	DXGIInfoManager& operator=(const DXGIInfoManager&) = delete;
	
	unsigned long long m_next = 0u;
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> m_pDXGI_InfoQueue;
};
