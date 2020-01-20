#pragma once
#include "Common.h"
#include "Event.h"
#include "Timer.h"

//------------
// Description
//--------------
// Scenario Manager Class is responsible for Starting/Stopping/Ticking of Memory Scenarios.
//------------

enum class ScenarioType
{
	Invalid = -1,
	ResourceLoadingBootup,
	ResourceLoadingGameplay,
	ParticleSystem,
	VertexDataProcessing,
	COUNT
};

DECLARE_EVENT_ONE_PARAM(ScenarioEvent, void, ScenarioType);

class IScenario;
class ResourceLoadingScenario;
class ParticleSystemScenario;
class VertexDataProcessingScenario;
class IRenderer;

class ScenarioManager
{
public:
	ScenarioManager();
	~ScenarioManager();
	
	void StartScenario(ScenarioType a_type);
	void StopScenario(ScenarioType a_type);

	void Update();
	void OnRender(IRenderer* a_pRenderer);

	ScenarioEvent OnScenarioStarted;
	ScenarioEvent OnScenarioStopped;
	ScenarioEvent OnScenarioComplete;
	
private:

	struct ActiveScenario
	{
		ActiveScenario(IScenario* a_pScenario, ScenarioType a_type)
			:
		pScenario(a_pScenario),
		Type( a_type),
		AverageFPS(0.f),
		Ticks(0)
		{
			Timer.Start();
		}
		IScenario* pScenario;
		ScenarioType Type;
		DT_Timer Timer;
		float AverageFPS;
		int Ticks;
	};

	std::string ScenarioTypeNames[(int)ScenarioType::COUNT] =
	{
		{TEXT(ResourceLoadingBootup)},
		{TEXT(ResourceLoadingGameplay)},
		{TEXT(ParticleSystem)},
		{TEXT(VertexDataProcessing)}
	};

	inline IScenario* GetScenario(ScenarioType a_type);

	// Memory Scenarios
	struct
	{
		ResourceLoadingScenario* pResourceLoading;
		ParticleSystemScenario* pParticleSystem;
		VertexDataProcessingScenario* pVertexProcessing;
	}Scenarios;

	std::vector<ActiveScenario> m_ActiveScenarios;
};
