#include "ScenarioWindow.h"
#include <imgui.h>
#include "Delegate.h"

ScenarioWindow::ScenarioWindow(ScenarioManager* a_pScenarioManager)
	:
IMGUIWindow("Scenario Window", false),
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


void ScenarioWindow::OnGUIWindow(const IMGUIInterface& a_gui)
{
	ImGui::Text("Scenarios");
	ImGui::Separator();

	// Bootup Resource Loading Scenario 
	if(a_gui.Button("Bootup Resource Loading", m_bScenarioActive[(int)ScenarioType::ResourceLoadingBootup] == false))
	{
		m_pScenarioManager->StartScenario(ScenarioType::ResourceLoadingBootup);
	}

	if (a_gui.Button("Gameplay Resource Loading", m_bScenarioActive[(int)ScenarioType::ResourceLoadingGameplay] == false))
	{
		m_pScenarioManager->StartScenario(ScenarioType::ResourceLoadingGameplay);
	}
	

	if(a_gui.Button("Particle System", m_bScenarioActive[(int)ScenarioType::ParticleSystem] == false))
	{
		m_pScenarioManager->StartScenario(ScenarioType::ParticleSystem);
	}

	if(a_gui.Button("Vertex Data Processing", m_bScenarioActive[(int)ScenarioType::VertexDataProcessing] == false))
	{
		m_pScenarioManager->StartScenario(ScenarioType::VertexDataProcessing);
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

