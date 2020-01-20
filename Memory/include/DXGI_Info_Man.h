#pragma once

#if DEBUG

#include "Win.h"
#include "Exception.h"
#include <vector>
#include <wrl.h>
#include <dxgidebug.h>

//------------
// Description
//--------------
// Class Responsible for Reading DirectX Debug Device Infomation Messages
//------------

class DXGIInfoManager
{
public:
	
	static void Initialise();
	static DXGIInfoManager* Instance();
	static void Shutdown();
	
	void Set(); // Clear Messages and Bump Next Message Idx
	std::vector<std::string>& GetMessages();
	
private:
	DXGIInfoManager();
	~DXGIInfoManager() {}
	
	DXGIInfoManager(const DXGIInfoManager&) = delete;
	DXGIInfoManager& operator=(const DXGIInfoManager&) = delete;
	
	unsigned long long m_next = 0u; // Next Message Idx
	Microsoft::WRL::ComPtr<IDXGIInfoQueue> m_pDXGI_InfoQueue; // Info Msg Queue
	std::vector<std::string> m_msgs; // Read Messages
};

#endif