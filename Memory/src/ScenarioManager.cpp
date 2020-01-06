#include "ScenarioManager.h"

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
		if(active == pScenarioToStart)
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

	m_ActiveScenarios.push_back(pScenarioToStart);
}

void ScenarioManager::StopScenario(ScenarioType a_type)
{
	IScenario* pScenarioToStart = GetScenario(a_type);

	for (size_t i = 0; i < m_ActiveScenarios.size(); ++i) // If Already Started, Ignore
	{
		IScenario* pActive = m_ActiveScenarios[i];
		if (pActive == pScenarioToStart)
		{
			m_ActiveScenarios.erase(m_ActiveScenarios.begin() + i);
			pActive->Reset();
			return;
		}
	}
}

void ScenarioManager::Update()
{
	for(size_t i = m_ActiveScenarios.size() - 1; i > 0; --i)
	{
		IScenario* pActive = m_ActiveScenarios[i];

		// TODO Time Scenario
		if(pActive->IsComplete() == false)
		{
			pActive->Run();
		}
		else
		{
			m_ActiveScenarios.erase(m_ActiveScenarios.begin() + i);
			pActive->Reset();
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

