#pragma once

#include <map>
#include <string>
#include <Win.h>

//------------
// Description
//--------------
// Windows Message Map used for identifying and outputting recieved windows messages
//------------

class WindowsMessageMap
{
public:
	static std::string Read(DWORD msg, LPARAM lp, WPARAM wp);
private:
	WindowsMessageMap();
	~WindowsMessageMap() {}

	std::string operator()(DWORD msg, LPARAM lp, WPARAM wp) const;

	static WindowsMessageMap* s_pMessageMap;
	std::map<DWORD, std::string> m_MessageMap;
};