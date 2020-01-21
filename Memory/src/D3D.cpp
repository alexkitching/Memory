#include "D3D.h"
#include "dxerr.h"
#include <sstream>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

D3DException::D3DException(int a_line, const char* a_pfileName, HRESULT a_HR, std::vector<std::string> a_infoMessages)
	:
Exception(a_line, a_pfileName),
m_hr(a_HR)
{
	// Copy Messages into info
	for(const std::string& message : a_infoMessages)
	{
		m_info += message;
		m_info.push_back('\n'); // Format Line ending
	}

	if(m_info.empty() == false)
	{
		m_info.pop_back(); // Remove Final Line
	}
}

// Format info into Buffer
const char* D3DException::what() const
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Error String] " << GetErrorCodeString() << std::endl
		<< "[Description] " << GetErrorCodeDescription() << std::endl;

	if(m_info.empty() == false)
	{
		oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
	}
	
	oss	<< GetOriginalString();

	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

std::string D3DException::GetErrorCodeString() const
{
	return DXGetErrorString(m_hr);
}

std::string D3DException::GetErrorCodeDescription() const
{
	char buf[512];
	DXGetErrorDescription(m_hr, buf, sizeof(buf));
	return buf;
}

D3DInfoException::D3DInfoException(int a_line, const char* a_file, std::vector<std::string> a_info)
	:
Exception(a_line, a_file)
{
	for(const std::string& info : a_info)
	{
		m_info += info;
		m_info.push_back('\n');
	}

	if(m_info.empty() == false)
	{
		m_info.pop_back();
	}
}

// Format Info into Buffer
const char* D3DInfoException::what() const
{
	std::ostringstream oss;
	oss << GetType()	<< std::endl 
		<< "[Info] " << GetErrorInfo() << std::endl;

	oss << GetOriginalString();

	m_whatBuffer = oss.str();
	
	return m_whatBuffer.c_str();
}

const char* D3DDeviceRemovedException::GetType() const
{
	return "D3D Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
