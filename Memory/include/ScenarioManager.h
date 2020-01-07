#pragma once
#include "ResourceLoadingScenario.h"
#include "ParticleSystemScenario.h"
#include "Common.h"
#include "Event.h"

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
DECLARE_DELEGATE_ONE_PARAM(ScenarioEventDelegate, void, ScenarioType);

class ScenarioManager
{
public:
	ScenarioManager();
	~ScenarioManager();

	void StartScenario(ScenarioType a_type);
	void StopScenario(ScenarioType a_type);

	void Update();

	ScenarioEvent OnScenarioStarted;
	ScenarioEvent OnScenarioStopped;
	ScenarioEvent OnScenarioComplete;
	
private:

	struct ActiveScenario
	{
		ActiveScenario(IScenario* a_pScenario, ScenarioType a_type)
			:
		pScenario(a_pScenario),
		Type( a_type)
		{
			Timer.Start();
		}
		IScenario* pScenario;
		ScenarioType Type;
		TP_Timer Timer;
	};

	std::string ScenarioTypeNames[(int)ScenarioType::COUNT] =
	{
		{TEXT(ResourceLoadingBootup)},
		{TEXT(ResourceLoadingGameplay)},
		{TEXT(ParticleSystem)},
		{TEXT(VertexDataProcessing)}
	};

	inline IScenario* GetScenario(ScenarioType a_type);
	
	struct
	{
		ResourceLoadingScenario ResourceLoading;
		ParticleSystemScenario ParticleSystem;
	}Scenarios;

	std::vector<ActiveScenario> m_ActiveScenarios;
};
