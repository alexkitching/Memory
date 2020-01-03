#pragma once
#include "Win.h"
#include "Exception.h"
#if DEBUG
#include "DXGI_Info_Man.h"
#endif

#include <d3d11.h>


class D3DException : public Exception
{
public:
	D3DException(int a_line, const char* a_pfileName, HRESULT a_HR, std::vector<std::string> a_infoMessages = {});
	
	const char* what() const override;
	const char* GetType() const override;

	HRESULT GetErrorCode() const { return m_hr; }
	std::string GetErrorCodeString() const;
	std::string GetErrorCodeDescription() const;
	std::string GetErrorInfo() const { return m_info; }
private:
	HRESULT m_hr;
	std::string m_info;
};

class D3DDeviceRemovedException : public D3DException
{
public:
	D3DDeviceRemovedException(int a_line, const char* a_pfileName,
							HRESULT a_HR, std::vector<std::string> a_infoMessages = {})
		:
	D3DException(a_line, a_pfileName, a_HR, a_infoMessages ) {}
	const char* GetType() const override;
};

#if DEBUG
#define D3D_THROW_EXCEPT_WITHINFO(hr) throw D3DException(__LINE__, __FILE__, (hr), DXGIInfoManager::Instance()->GetMessages())
#define D3D_THROW_EXCEPT_NOINFO(hr) throw D3DException(__LINE__, __FILE__, (hr))
#define D3D_THROW_EXCEPT(hr) if(DXGIInfoManager::Instance() == nullptr) { D3D_THROW_EXCEPT_NOINFO(hr))} else { D3D_THROW_EXCEPT_WITHINFO(hr)}


#define D3D_THROW_FAILED_WITHINFO(hrcall) DXGIInfoManager::Instance()->Set(); if(FAILED(hr = (hrcall))) throw D3DException(__LINE__, __FILE__, hr, DXGIInfoManager::Instance()->GetMessages());
#define D3D_THROW_FAILED_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw D3DException(__LINE__, __FILE__, hr);
#define D3D_THROW_FAILED(hrcall) if(DXGIInfoManager::Instance() == nullptr) {D3D_THROW_FAILED_NOINFO(hrcall)} else {D3D_THROW_FAILED_WITHINFO(hrcall)}

#define D3D_THROW_DEVICE_REMOVED_EXCEPT_WITHINFO(hr) throw D3DDeviceRemovedException(__LINE__, __FILE__, (hr), DXGIInfoManager::Instance()->GetMessages());
#define D3D_THROW_DEVICE_REMOVED_EXCEPT_NOINFO(hr) throw D3DDeviceRemovedException(__LINE__, __FILE__, (hr));
#define D3D_THROW_DEVICE_REMOVED_EXCEPT(hr) if(DXGIInfoManager::Instance() == nullptr) { D3D_THROW_DEVICE_REMOVED_EXCEPT_NOINFO(hr)} else { D3D_THROW_DEVICE_REMOVED_EXCEPT_WITHINFO(hr)}
#else
#define D3D_EXCEPT(hr) D3DException(__LINE__, __FILE__, (hr))
#define D3D_THROW_FAILED(hrcall) if(FAILED(hr = (hrcall))) throw D3DException(__LINE__, __FILE__, hr);
#define D3D_DEVICE_REMOVED_EXCEPT(hr) D3DDeviceRemovedException(__LINE__, __FILE__, (hr));
#endif

