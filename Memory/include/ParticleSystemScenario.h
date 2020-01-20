#pragma once
#include "Scenario.h"
#include "Timer.h"
#include "MemSys.h"
#include "PoolAllocator.h"
#include <vector>

//------------
// Description
//--------------
// Particle System Memory Scenario
// Particle Systems Spawn Particles with limited varied lifespans at a High Frequency demanding fast allocation.
// Using the Custom Memory System the scenario uses a Pool Allocator
//------------

class Heap;

class ParticleSystemScenario : public IScenario
{
public:
	// Scenario Configuration
	struct Config
	{
		int ParticleSystemsCount;
		struct
		{
			int MaxParticles;
			float FixedParticleSpawnInterval;
			int ParticlesPerInterval;
			int ParticleStartCount;
			float ParticleLifeTimeMax;
			float ParticleLifeTimeMin;
		}ParticleSystem;
	};
	static Config Configuration;

	ParticleSystemScenario();
	virtual ~ParticleSystemScenario() {}

	// Scenario Interface
	void Initialise() override;
	void Run() override;
	void OnRender(IRenderer* a_pRenderer) override;
	void Reset() override;
	bool IsComplete() override { return false; }

	static int m_TotalAllocatedParticles;
private:
	
	class ParticleSystem
	{
		friend class ParticleSystemScenario;
	public:
		struct Config
		{
			float LifeTime;
			float OriginPosition[3];
		};
		
		
		ParticleSystem(const Config& a_config);
		~ParticleSystem();

		void Update();
		void OnRender(IRenderer* a_pRenderer);

		int GetParticleCount() const { return (int)m_Particles.size(); }
		
	private:
		struct Particle
		{
			Particle(float a_fX, float a_fY, float a_fZ);
				
			float Position[3];
			float Velocity[3];
			float LiveTime; // 56 Bytes
		};

		void SpawnParticle();
		void DestroyParticle(int a_pIdx);

		Config m_Config;
		DT_Timer m_NextParticleTimer;
#if USE_MEM_SYS
		PoolAllocator<Particle, 8> m_ParticlePool;
#endif
		std::vector<Particle*> m_Particles;
	};

	std::vector<ParticleSystem> m_ParticleSystems;
};
