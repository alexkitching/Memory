#include "IMGUIWindow.h"
#include <imgui.h>

void IMGUIWindow::OnGUI(const IMGUIInterface& a_interface)
{
	if (ImGui::Begin(m_Name.c_str(), nullptr) == false)
	{
		ImGui::End();
		return;
	}

	OnGUIWindow(a_interface);

	ImGui::End();
}
