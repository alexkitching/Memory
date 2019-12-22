#pragma once

#include <map>
#include <string>
#include <Win.h>


class WindowsMessageMap
{
public:
	WindowsMessageMap();
	~WindowsMessageMap() {}

	std::string operator()( DWORD msg,LPARAM lp,WPARAM wp ) const noexcept;

private:
	std::map<DWORD, std::string> m_MessageMap;
};