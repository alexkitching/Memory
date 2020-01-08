#pragma once

#include <map>
#include <string>
#include <Win.h>


class WindowsMessageMap
{
public:
	static std::string Read(DWORD msg, LPARAM lp, WPARAM wp);
private:
	WindowsMessageMap();
	~WindowsMessageMap() {}

	std::string operator()(DWORD msg, LPARAM lp, WPARAM wp) const noexcept;

	static WindowsMessageMap* s_pMessageMap;
	std::map<DWORD, std::string> m_MessageMap;
};