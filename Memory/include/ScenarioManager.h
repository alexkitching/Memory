#pragma once
#include "Common.h"
#include "Event.h"
#include "Timer.h"

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

class IScenario;
class ResourceLoadingScenario;
class ParticleSystemScenario;
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
		ResourceLoadingScenario* pResourceLoading;
		ParticleSystemScenario* pParticleSystem;
	}Scenarios;

	std::vector<ActiveScenario> m_ActiveScenarios;
};
