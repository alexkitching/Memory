#pragma once
#include "Scenario.h"
#include "Timer.h"
#include "RandomUtility.h"
#include "PoolAllocator.h"
#include <vector>

class Heap;

class ParticleSystemScenario : public IScenario
{
public:
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

		float RunLength;
	};

	ParticleSystemScenario() = default;
	ParticleSystemScenario(const Config& a_config);
	
	virtual ~ParticleSystemScenario() {}

	void Run() override;
	void OnRender(IRenderer* a_pRenderer) override;
	void Reset() override;
	bool IsComplete() override { return m_bComplete; }

	static int TotalAllocatedParticles;
private:
	
	
	class ParticleSystem
	{
		friend class ParticleSystemScenario;
	public:
		struct Config
		{
			int MaxParticles;
			int StartParticles;
			float FixedParticleSpawnInterval;
			int ParticlesPerInterval;
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
		Pool<Particle, 8> m_ParticlePool;
#endif
		std::vector<Particle*> m_Particles;
	};

	void Initialise();
	
	Config m_Config;
	DT_Timer m_RunTimeTimer;
	bool m_bComplete;

	std::vector<ParticleSystem> m_ParticleSystems;
};
