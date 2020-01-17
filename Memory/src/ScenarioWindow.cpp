#include "ScenarioWindow.h"
#include <imgui.h>
#include "Delegate.h"

ScenarioWindow::ScenarioWindow(ScenarioManager* a_pScenarioManager)
	:
IMGUIWindow("Scenario Window", false),
m_bScenarioActive{false},
m_ActiveIsolatedScenario(ScenarioType::Invalid),
m_pScenarioManager(a_pScenarioManager)
{
	m_pScenarioManager->OnScenarioStarted.AddListener<ScenarioWindow, &ScenarioWindow::OnScenarioStarted>(this);
	m_pScenarioManager->OnScenarioStopped.AddListener<ScenarioWindow, &ScenarioWindow::OnScenarioFinished>(this);
	m_pScenarioManager->OnScenarioComplete.AddListener<ScenarioWindow, &ScenarioWindow::OnScenarioFinished>(this);
}

ScenarioWindow::~ScenarioWindow()
{

}


void ScenarioWindow::OnGUIWindow(const IMGUIInterface& a_gui)
{
	ImGui::Text("Scenarios");
	ImGui::Separator();

	// Bootup Resource Loading Scenario
	DrawBootupResourceLoadingScenario(a_gui);
	// Gameplay Resource Loading Scenario
	DrawGameplayResourceLoadingScenario(a_gui);
	// Particle System Scenario
	DrawParticleSystemScenario(a_gui);
	// Vertex Data Processing Scenario
	DrawVertexDataProcessingScenario(a_gui);
}

void ScenarioWindow::OnScenarioStarted(ScenarioType a_type)
{
	if(a_type == ScenarioType::ResourceLoadingBootup)
	{
		m_ActiveIsolatedScenario = a_type;
	}
	
	m_bScenarioActive[(int)a_type] = true;
}

void ScenarioWindow::OnScenarioFinished(ScenarioType a_type)
{
	if(m_ActiveIsolatedScenario == a_type)
	{
		m_ActiveIsolatedScenario = ScenarioType::Invalid;
	}
	
	m_bScenarioActive[(int)a_type] = false;
}

void ScenarioWindow::DrawBootupResourceLoadingScenario(const IMGUIInterface& a_gui)
{
	ImGui::Text("Bootup Resource Loading (Isolated)");

	const ScenarioType scenario = ScenarioType::ResourceLoadingBootup;

	if (a_gui.Button("Start##BRL",
			AnyOtherScenarioRunning(scenario) == false &&
		m_bScenarioActive[(int)scenario] == false))
	{
		m_pScenarioManager->StartScenario(scenario);
	}

	ImGui::SameLine();

	if (a_gui.Button("Stop##BRL",
		AnyOtherScenarioRunning(scenario) == false &&
		m_bScenarioActive[(int)scenario]))
	{
		m_pScenarioManager->StopScenario(scenario);
	}


}

void ScenarioWindow::DrawGameplayResourceLoadingScenario(const IMGUIInterface& a_gui)
{
	ImGui::Text("Gameplay Resource Loading");

	const ScenarioType scenario = ScenarioType::ResourceLoadingGameplay;

	if (a_gui.Button("Start##GRL",
		IsolatedScenarioRunning(scenario) == false &&
		m_bScenarioActive[(int)scenario] == false))
	{
		m_pScenarioManager->StartScenario(scenario);
	}

	ImGui::SameLine();

	if (a_gui.Button("Stop##GRL",
		IsolatedScenarioRunning(scenario) == false &&
		m_bScenarioActive[(int)scenario]))
	{
		m_pScenarioManager->StopScenario(scenario);
	}
}

void ScenarioWindow::DrawParticleSystemScenario(const IMGUIInterface& a_gui)
{
	ImGui::Text("Particle System");

	const ScenarioType scenario = ScenarioType::ParticleSystem;

	if (a_gui.Button("Start##PS",
		IsolatedScenarioRunning(scenario) == false &&
		m_bScenarioActive[(int)scenario] == false))
	{
		m_pScenarioManager->StartScenario(scenario);
	}

	ImGui::SameLine();

	if (a_gui.Button("Stop##PS",
		IsolatedScenarioRunning(scenario) == false &&
		m_bScenarioActive[(int)scenario]))
	{
		m_pScenarioManager->StopScenario(scenario);
	}
}

void ScenarioWindow::DrawVertexDataProcessingScenario(const IMGUIInterface& a_gui)
{
	ImGui::Text("Vertex Data Processing");

	const ScenarioType scenario = ScenarioType::ParticleSystem;

	if (a_gui.Button("Start##VDP",
		IsolatedScenarioRunning(scenario) == false &&
		m_bScenarioActive[(int)scenario] == false))
	{
		m_pScenarioManager->StartScenario(scenario);
	}

	ImGui::SameLine();

	if (a_gui.Button("Stop##VDP",
		IsolatedScenarioRunning(scenario) == false &&
		m_bScenarioActive[(int)scenario]))
	{
		m_pScenarioManager->StopScenario(scenario);
	}
}

bool ScenarioWindow::IsolatedScenarioRunning(ScenarioType a_checkingScenario) const
{
	if (a_checkingScenario == m_ActiveIsolatedScenario)
		return false;
	
	return m_ActiveIsolatedScenario != ScenarioType::Invalid;
}

bool ScenarioWindow::AnyOtherScenarioRunning(ScenarioType a_checkingScenario) const
{
	bool otherRunning = false;
	for(int i = 0; i < (int)ScenarioType::COUNT; ++i)
	{
		if (i == (int)a_checkingScenario)
			continue;

		otherRunning |= m_bScenarioActive[i];
	}

	return otherRunning;
}

