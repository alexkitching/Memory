#include "ScenarioManager.h"
#include "Debug.h"

#define SMLOG(x, ...) LOG("ScenarioManager::" x, __VA_ARGS__)

ScenarioManager::ScenarioManager()
{
	// Setup Scenarios
	ResourceLoadingScenario::Config config = {};
	config.Bootup.LoadInterval = 0.25f;
	config.Bootup.MaxResourceSize = 200 * MB;
	config.Bootup.MinResourceSize = 20 * MB;
	config.Bootup.m_NumResourcesToLoad = 20;

	Scenarios.ResourceLoading = ResourceLoadingScenario(config);
}

ScenarioManager::~ScenarioManager()
{
}

void ScenarioManager::StartScenario(ScenarioType a_type)
{
	IScenario* pScenarioToStart = GetScenario(a_type);

	for(const auto& active : m_ActiveScenarios ) // If Already Started, Ignore
	{
		if(active.pScenario == pScenarioToStart)
		{
			return;
		}
	}

	// Extra Setup for Resource Loading
	switch (a_type)
	{
	case ScenarioType::ResourceLoadingBootup:
		Scenarios.ResourceLoading.SetType(ResourceLoadingScenario::Type::Bootup);
		break;
	case ScenarioType::ResourceLoadingGameplay:
		Scenarios.ResourceLoading.SetType(ResourceLoadingScenario::Type::Gameplay);
		break;
	default:
		break;
	}


	const char* pName = ScenarioTypeNames[(int)a_type].c_str();
	SMLOG("Starting Memory Scenario - %s \n", pName);
	
	m_ActiveScenarios.emplace_back(pScenarioToStart, a_type);
	
	OnScenarioStarted(a_type); // Raise Event
}

void ScenarioManager::StopScenario(ScenarioType a_type)
{
	IScenario* pScenarioToStart = GetScenario(a_type);

	for (size_t i = 0; i < m_ActiveScenarios.size(); ++i) // If Already Started, Ignore
	{
		ActiveScenario& active = m_ActiveScenarios[i];
		if (active.pScenario == pScenarioToStart)
		{
			active.pScenario->Reset();
			SMLOG("Scenario Stopped - %s \n", ScenarioTypeNames[(int)a_type].c_str());
			m_ActiveScenarios.erase(m_ActiveScenarios.begin() + i);

			OnScenarioStopped(a_type); // Raise Event
			return;
		}
	}
}

void ScenarioManager::Update()
{
	for(int i = (int)m_ActiveScenarios.size() - 1; i >= 0; --i)
	{
		ActiveScenario& active = m_ActiveScenarios[i];

		// TODO Time Scenario
		if(active.pScenario->IsComplete() == false)
		{
			active.pScenario->Run();
		}
		else
		{
			ScenarioType type = active.Type;
			active.pScenario->Reset();
			const float fTimeTaken = active.Timer.GetTime();

			m_ActiveScenarios.erase(m_ActiveScenarios.begin() + i);

			SMLOG("Scenario Completed - %s - Time Taken - %f s \n", ScenarioTypeNames[(int)type].c_str(), fTimeTaken);
			OnScenarioComplete(type); // Raise Event
		}
	}
}

IScenario* ScenarioManager::GetScenario(ScenarioType a_type)
{
	switch (a_type)
	{
	case ScenarioType::ResourceLoadingBootup:
	case ScenarioType::ResourceLoadingGameplay:
		return &Scenarios.ResourceLoading;
		break;
	case ScenarioType::ParticleSystem:
	case ScenarioType::VertexDataProcessing:
	default:
		break;
	}

	return nullptr;
}

