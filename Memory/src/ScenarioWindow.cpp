#include "ScenarioWindow.h"
#include <imgui.h>
#include <functional>
#include "Delegate.h"
#include "imgui_internal.h"

ScenarioWindow::ScenarioWindow(ScenarioManager* a_pScenarioManager)
	:
IMGUIWindow("Scenario Window"),
m_bScenarioActive{false},
m_pScenarioManager(a_pScenarioManager)
{
	m_pScenarioManager->OnScenarioStarted.AddListener(ScenarioEventDelegate::Bind<ScenarioWindow, &ScenarioWindow::OnScenarioStarted>(this));
	m_pScenarioManager->OnScenarioStopped.AddListener(ScenarioEventDelegate::Bind<ScenarioWindow, &ScenarioWindow::OnScenarioFinished>(this));
	m_pScenarioManager->OnScenarioComplete.AddListener(ScenarioEventDelegate::Bind<ScenarioWindow, &ScenarioWindow::OnScenarioFinished>(this));
}

ScenarioWindow::~ScenarioWindow()
{
}


void ScenarioWindow::OnGUIWindow(const IMGUIInterface& a_interface)
{
	ImGui::Text("Scenarios");
	ImGui::Separator();

	const bool bScenarioActive = m_bScenarioActive[(int)ScenarioType::ResourceLoadingBootup];
	if(bScenarioActive)
	{
		 ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		 ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if(ImGui::Button("Bootup Resource Loading" ))
	{
		m_pScenarioManager->StartScenario(ScenarioType::ResourceLoadingBootup);
	}

	if (bScenarioActive)
	{
		ImGui::PopStyleVar();
		ImGui::PopItemFlag();
	}

	if(ImGui::Button("Gameplay Resource Loading"))
	{
		
	}

	if(ImGui::Button("Particle System"))
	{
		
	}

	if(ImGui::Button("Vertex Data Processing"))
	{
		
	}
}

void ScenarioWindow::OnScenarioStarted(ScenarioType a_type)
{
	m_bScenarioActive[(int)a_type] = true;
}

void ScenarioWindow::OnScenarioFinished(ScenarioType a_type)
{
	m_bScenarioActive[(int)a_type] = false;
}
