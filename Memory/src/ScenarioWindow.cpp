#include "ScenarioWindow.h"
#include <imgui.h>
#include "ResourceLoadingScenario.h"
#include "ParticleSystemScenario.h"
#include "VertexDataProcessingScenario.h"

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
	ImGui::Separator();
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

	// Draw Start/Stop Buttons
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

	ImGui::SameLine();

	if (a_gui.Button("Configuration##BRL",
		m_bScenarioActive[(int)scenario] == false))
	{
		ImGui::OpenPopup("Bootup Resource Loading Configuration");
	}

	// Configuration Popup
	if(ImGui::BeginPopupModal("Bootup Resource Loading Configuration"))
	{
		// Draw Config Settings
		ResourceLoadingScenario::Config& config = ResourceLoadingScenario::Configuration;

		int BootupTotalSizeMBs = (int)(config.Bootup.TotalSizeToLoad / MB);

		if (a_gui.IntSlider("Total Mbs to Load", m_bScenarioActive[(int)scenario] == false,
			&BootupTotalSizeMBs, 1.f, 0, 4000))
		{
			config.Bootup.TotalSizeToLoad = BootupTotalSizeMBs * MB;
		}

		float LoadInterval = config.Bootup.LoadInterval;
		if (a_gui.FloatSlider("Load Interval", m_bScenarioActive[(int)scenario] == false,
			&LoadInterval, 1.f, 0.05f, 5.f, "%.2f"))
		{
			config.Bootup.LoadInterval = LoadInterval;
		}

		int MaxResourceSizeMB = (int)(config.Bootup.MaxResourceSize / MB);
		int MinResourceSizeMB = (int)(config.Bootup.MinResourceSize / MB);

		if (a_gui.IntSlider("Min Resource Size Mbs", m_bScenarioActive[(int)scenario] == false,
			&MinResourceSizeMB, 1.f, 1, MaxResourceSizeMB - 1))
		{
			config.Bootup.MinResourceSize = MinResourceSizeMB * MB;
		}

		if (a_gui.IntSlider("Max Resource Size Mbs", m_bScenarioActive[(int)scenario] == false,
			&MaxResourceSizeMB, 1.f, MinResourceSizeMB + 1, 250))
		{
			config.Bootup.MaxResourceSize = MaxResourceSizeMB * MB;
		}

		if (a_gui.Button("Close ##BRL",
			m_bScenarioActive[(int)scenario] == false))
		{
			ImGui::CloseCurrentPopup();
		}
		
		ImGui::EndPopup();
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

	ImGui::SameLine();

	if (a_gui.Button("Configuration##GRL",
		m_bScenarioActive[(int)scenario] == false))
	{
		ImGui::OpenPopup("Gameplay Resource Loading Configuration");
	}

	if (ImGui::BeginPopupModal("Gameplay Resource Loading Configuration"))
	{
		// Draw Config Settings
		ResourceLoadingScenario::Config& config = ResourceLoadingScenario::Configuration;

		int ResourceSizeCap = (int)(config.Gameplay.AllocatedResourceCap / MB);
		if(a_gui.IntSlider("Allocated Resource Cap", m_bScenarioActive[(int)scenario] == false, 
			&ResourceSizeCap, 1.f, 100, 800))
		{
			config.Gameplay.AllocatedResourceCap = ResourceSizeCap * MB;
		}

		float LoadInterval = config.Gameplay.LoadInterval;
		if (a_gui.FloatSlider("Load Interval", m_bScenarioActive[(int)scenario] == false,
			&LoadInterval, 1.f, 0.05f, 5.f, "%.2f"))
		{
			config.Gameplay.LoadInterval = LoadInterval;
		}

		int MaxResourceSizeMB = (int)(config.Gameplay.MaxResourceSize / MB);
		int MinResourceSizeMB = (int)(config.Gameplay.MinResourceSize / MB);

		if (a_gui.IntSlider("Min Resource Size Mbs", m_bScenarioActive[(int)scenario] == false,
			&MinResourceSizeMB, 1.f, 1, MaxResourceSizeMB - 1))
		{
			config.Gameplay.MinResourceSize = MinResourceSizeMB * MB;
		}

		if (a_gui.IntSlider("Max Resource Size Mbs", m_bScenarioActive[(int)scenario] == false,
			&MaxResourceSizeMB, 1.f, MinResourceSizeMB + 1, ResourceSizeCap))
		{
			config.Gameplay.MaxResourceSize = MaxResourceSizeMB * MB;
		}

		
		
		if (a_gui.Button("Close ##GRL",
			m_bScenarioActive[(int)scenario] == false))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
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

	ImGui::SameLine();

	if (a_gui.Button("Configuration##PS",
		m_bScenarioActive[(int)scenario] == false))
	{
		ImGui::OpenPopup("Particle System Configuration");
	}

	if (ImGui::BeginPopupModal("Particle System Configuration"))
	{
		// Draw Config Settings
		ParticleSystemScenario::Config& config = ParticleSystemScenario::Configuration;

		int ParticleSystemNum = config.ParticleSystemsCount;
		if (a_gui.IntSlider("No. of Particle Systems", m_bScenarioActive[(int)scenario] == false,
			&ParticleSystemNum, 1.f, 1, 100))
		{
			config.ParticleSystemsCount = ParticleSystemNum;
		}
		
		ImGui::Separator();
		ImGui::Text("Particle System Settings");

		int MaxParticles = config.ParticleSystem.MaxParticles;
		if (a_gui.IntSlider("Max Particles", m_bScenarioActive[(int)scenario] == false,
			&MaxParticles, 1.f, 100, 8000))
		{
			config.ParticleSystem.MaxParticles = MaxParticles;
		}

		int ParticlesPerInterval = config.ParticleSystem.ParticlesPerInterval;
		if (a_gui.IntSlider("Particles Per Interval", m_bScenarioActive[(int)scenario] == false,
			&ParticlesPerInterval, 1.f, 1, 50))
		{
			config.ParticleSystem.ParticlesPerInterval = ParticlesPerInterval;
		}

		int StartParticleCount = config.ParticleSystem.ParticleStartCount;
		if (a_gui.IntSlider("Start Particle Count", m_bScenarioActive[(int)scenario] == false,
			&StartParticleCount, 1.f, 1, MaxParticles - 1))
		{
			config.ParticleSystem.ParticleStartCount = StartParticleCount;
		}

		float MinParticleLifetime = config.ParticleSystem.ParticleLifeTimeMin;
		float MaxParticleLifetime = config.ParticleSystem.ParticleLifeTimeMax;

		if (a_gui.FloatSlider("Min Particle Lifetime", m_bScenarioActive[(int)scenario] == false,
			&MinParticleLifetime, 1.f, 0.5f, MaxParticleLifetime - 0.5f, "%.2f"))
		{
			config.ParticleSystem.ParticleLifeTimeMin = MinParticleLifetime;
		}

		if (a_gui.FloatSlider("Max Particle Lifetime", m_bScenarioActive[(int)scenario] == false,
			&MaxParticleLifetime, 1.f, MinParticleLifetime + 0.5f, 20.f, "%.2f"))
		{
			config.ParticleSystem.ParticleLifeTimeMax = MaxParticleLifetime;
		}
		

		if (a_gui.Button("Close ##PS",
			m_bScenarioActive[(int)scenario] == false))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void ScenarioWindow::DrawVertexDataProcessingScenario(const IMGUIInterface& a_gui)
{
	ImGui::Text("Vertex Data Processing");

	const ScenarioType scenario = ScenarioType::VertexDataProcessing;

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

	ImGui::SameLine();

	if (a_gui.Button("Configuration##VDP",
		m_bScenarioActive[(int)scenario] == false))
	{
		ImGui::OpenPopup("Vertex Data Processing Configuration");
	}


	if (ImGui::BeginPopupModal("Vertex Data Processing Configuration"))
	{
		// Draw Config Settings
		VertexDataProcessingScenario::Config& config = VertexDataProcessingScenario::Configuration;
		

		int MaxVertsPerSub = config.MaxVertsPerSub;
		int MinVertsPerSub = config.MinVertsPerSub;

		if (a_gui.IntSlider("Min Verts Per Vertex Submission", m_bScenarioActive[(int)scenario] == false,
			&MinVertsPerSub, 1.f, 1, MaxVertsPerSub - 1))
		{
			config.MinVertsPerSub = MinVertsPerSub;
		}

		if (a_gui.IntSlider("Max Verts Per Vertex Submission", m_bScenarioActive[(int)scenario] == false,
			&MaxVertsPerSub, 1.f, MinVertsPerSub + 1, 32000))
		{
			config.MaxVertsPerSub = MaxVertsPerSub;
		}

		int PerFrameDataSize = (int)(config.PerFrameTotalData / MB);
		if(a_gui.IntSlider("Per Frame Total Data Mbs", m_bScenarioActive[(int)scenario] == false,
			&PerFrameDataSize, 1.f, 1, 300))
		{
			config.PerFrameTotalData = PerFrameDataSize * MB;
		}
		
		if (a_gui.Button("Close ##VDP",
			m_bScenarioActive[(int)scenario] == false))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
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

