#include "ScenarioManager.h"
#include "ResourceLoadingScenario.h"
#include "ParticleSystemScenario.h"
#include "Debug.h"
#include "MemoryManager.h"

#define SMLOG(x, ...) LOG("ScenarioManager::" x, __VA_ARGS__)

ScenarioManager::ScenarioManager()
	:
Scenarios({nullptr, nullptr})
{
	// Setup Scenarios
	ResourceLoadingScenario::Config config = {};
	config.Bootup.LoadInterval = 0.25f;
	config.Bootup.MaxResourceSize = 200 * MB;
	config.Bootup.MinResourceSize = 20 * MB;
	config.Bootup.m_NumResourcesToLoad = 20;

	config.Gameplay.LoadInterval = 0.5f;
	config.Gameplay.MaxResourceSize = 60 * MB;
	config.Gameplay.MinResourceSize = MB;
	config.Gameplay.MaxAllocatedResourceSize = 400 * MB;
	config.Gameplay.RunLength = 20.f;

	Scenarios.pResourceLoading = new ResourceLoadingScenario(config);

	
	ParticleSystemScenario::Config psConfig = {};
	psConfig.ParticleSystem.MaxParticles = 2000;
	psConfig.ParticleSystem.ParticleEmissionRate = 50.f;
	psConfig.ParticleSystem.ParticleStartCount = 50;
	psConfig.ParticleSystem.ParticleLifeTime = 3.f;
	psConfig.ParticleSystemsCount = 50;
	psConfig.RunLength = 20.f;

	Scenarios.pParticleSystem = new ParticleSystemScenario(psConfig);


}

ScenarioManager::~ScenarioManager()
{
	delete Scenarios.pResourceLoading;
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
		Scenarios.pResourceLoading->SetType(ResourceLoadingScenario::Type::Bootup);
		break;
	case ScenarioType::ResourceLoadingGameplay:
		Scenarios.pResourceLoading->SetType(ResourceLoadingScenario::Type::Gameplay);
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

void ScenarioManager::OnRender(IRenderer* a_pRenderer)
{
	for(auto& active : m_ActiveScenarios)
	{
		active.pScenario->OnRender(a_pRenderer);
	}
}

IScenario* ScenarioManager::GetScenario(ScenarioType a_type)
{
	switch (a_type)
	{
	case ScenarioType::ResourceLoadingBootup:
	case ScenarioType::ResourceLoadingGameplay:
		return Scenarios.pResourceLoading;
	case ScenarioType::ParticleSystem:
		return Scenarios.pParticleSystem;
	case ScenarioType::VertexDataProcessing:
	default:
		break;
	}

	return nullptr;
}

