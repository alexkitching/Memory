#include "IMGUIWindow.h"
#include <imgui.h>

void IMGUIWindow::OnGUI(const IMGUIInterface& a_interface)
{
	if(m_bOpen == false)
	{
		return;
	}
	
	if (ImGui::Begin(m_Name.c_str(), &m_bOpen) == false)
	{
		ImGui::End();
		return;
	}

	OnGUIWindow(a_interface);

	ImGui::End();
}
