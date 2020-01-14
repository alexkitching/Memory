#include "ScenarioManager.h"
#include "ResourceLoadingScenario.h"
#include "ParticleSystemScenario.h"
#include "VertexDataProcessingScenario.h"
#include "Debug.h"
#include "MemoryManager.h"
#include "PerformanceCounter.h"
#include "Profiler.h"

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
#ifdef x64
	config.Bootup.TotalSizeToLoad = 2 * GB;
#else
	config.Bootup.TotalSizeToLoad = 0.9 * GB;
#endif

	config.Gameplay.LoadInterval = 0.5f;
	config.Gameplay.MaxResourceSize = 60 * MB;
	config.Gameplay.MinResourceSize = MB;
	config.Gameplay.MaxAllocatedResourceSize = 400 * MB;
	config.Gameplay.RunLength = 20.f;

	Scenarios.pResourceLoading = new ResourceLoadingScenario(config);

	
	ParticleSystemScenario::Config psConfig = {};
	psConfig.ParticleSystem.MaxParticles = 2000;
	psConfig.ParticleSystem.FixedParticleSpawnInterval = 0.0001f;
	psConfig.ParticleSystem.ParticlesPerInterval = 10;
	psConfig.ParticleSystem.ParticleStartCount = 50;
	psConfig.ParticleSystem.ParticleLifeTimeMax = 5.f;
	psConfig.ParticleSystem.ParticleLifeTimeMin = 1.5f;
	psConfig.ParticleSystemsCount = 50;
	psConfig.RunLength = 20.f;

	Scenarios.pParticleSystem = new ParticleSystemScenario(psConfig);

	VertexDataProcessingScenario::Config vpConfig = {};
	vpConfig.MinVertsPerSub = 3;
	vpConfig.MaxVertsPerSub = 300;
	vpConfig.PerFrameTotalData = 5 * MB;
	vpConfig.RunLength = 20.f;

	Scenarios.pVertexProcessing = new VertexDataProcessingScenario(vpConfig);
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
	PROFILER_BEGIN_SAMPLE(ScenarioManager::Update);
	for(int i = (int)m_ActiveScenarios.size() - 1; i >= 0; --i)
	{
		ActiveScenario& active = m_ActiveScenarios[i];
		active.Timer.Tick();
		active.Ticks++;
		active.AverageFPS += (PerformanceCounter::FPS() - active.AverageFPS) / active.Ticks;
		
		if(active.pScenario->IsComplete() == false)
		{
			active.pScenario->Run();
		}
		else
		{
			ScenarioType type = active.Type;
			const float averageFPS = active.AverageFPS;
			active.pScenario->Reset();
			const float fTimeTaken = active.Timer.GetTime();

			m_ActiveScenarios.erase(m_ActiveScenarios.begin() + i);

			SMLOG("Scenario Completed - %s \n - Time Taken - %f.1 s \n AverageFPS - %f.1 \n", ScenarioTypeNames[(int)type].c_str(), fTimeTaken, averageFPS);
			
			OnScenarioComplete(type); // Raise Event
		}
	}
	PROFILER_END_SAMPLE();
}

void ScenarioManager::OnRender(IRenderer* a_pRenderer)
{
	PROFILER_BEGIN_SAMPLE(ScenarioManager::OnRender);
	for(auto& active : m_ActiveScenarios)
	{
		active.pScenario->OnRender(a_pRenderer);
	}
	PROFILER_END_SAMPLE();
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
		return Scenarios.pVertexProcessing;
	default:
		break;
	}

	return nullptr;
}

