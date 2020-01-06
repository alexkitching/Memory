#pragma once

#include "IMGUIInterface.h"

class IMGUIWindow
{
public:
	IMGUIWindow(const char* a_pName) : m_Name(a_pName) {};
	virtual ~IMGUIWindow() {}

	void OnGUI(const IMGUIInterface& a_interface);

protected:
	virtual void OnGUIWindow(const IMGUIInterface& a_interface) = 0;
private:
	std::string m_Name;
};