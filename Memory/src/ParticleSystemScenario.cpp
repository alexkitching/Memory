#include "ParticleSystemScenario.h"
#include "Debug.h"
#include "GlobalTime.h"
#include "Renderer.h"
#include "MemSys.h"
#include "MemoryManager.h"
#include "Profiler.h"

int ParticleSystemScenario::m_TotalAllocatedParticles = 0;

ParticleSystemScenario::ParticleSystemScenario(const Config& a_config)
	:
	m_Config(a_config),
	m_bStarted(false)
{
	m_ParticleSystems.reserve(m_Config.ParticleSystemsCount);
}

void ParticleSystemScenario::Run()
{
	PROFILER_BEGIN_SAMPLE(ParticleSystemScenario::Run);
	if (m_bStarted == false) // Begin
	{
		Initialise();
		m_bStarted = true;
	}

	// Update Particle Systems
	for (auto& ps : m_ParticleSystems)
	{
		ps.Update();
	}
	
	PROFILER_END_SAMPLE();
}

void ParticleSystemScenario::OnRender(IRenderer* a_pRenderer)
{
	PROFILER_BEGIN_SAMPLE(ParticleSystemScenario::OnRender);
	for(auto& ps : m_ParticleSystems)
	{
		ps.OnRender(a_pRenderer);
	}
	PROFILER_END_SAMPLE();
}

void ParticleSystemScenario::Reset()
{
	PROFILER_BEGIN_SAMPLE(ParticleSystemScenario::Reset);
	LOG("COMPLETE :: Total Allocated Particles: %i \n", m_TotalAllocatedParticles);
	m_TotalAllocatedParticles = 0;
	m_bStarted = false;
	m_ParticleSystems.clear();
	PROFILER_END_SAMPLE();
}

ParticleSystemScenario::ParticleSystem::ParticleSystem(const Config& a_config)
	:
	m_Config(a_config)
#if USE_MEM_SYS
	,
m_ParticlePool(m_Config.MaxParticles * sizeof(Particle),
				MemoryManager::GetDefaultHeap()->allocate(m_Config.MaxParticles * sizeof(Particle), 64u))
#endif
{ 
	PROFILER_BEGIN_SAMPLE(ParticleSystem::_ctor);
	while ((int)m_Particles.size() != m_Config.StartParticles)
	{
		SpawnParticle();
	}
	PROFILER_END_SAMPLE();
}

ParticleSystemScenario::ParticleSystem::~ParticleSystem()
{
	while(m_Particles.empty() == false)
	{
		DestroyParticle(0);
	}
#if USE_MEM_SYS
	MemoryManager::GetDefaultHeap()->deallocate(m_ParticlePool.GetStartAddress());
#endif
}

void ParticleSystemScenario::ParticleSystem::Update()
{
	PROFILER_BEGIN_SAMPLE(ParticleSystem::Update);
	m_NextParticleTimer.Tick();

	// Spawn Particles
	if ((int)m_Particles.size() < m_Config.MaxParticles) // Not Reached Max Particles
	{
		if (m_NextParticleTimer.IsStarted() == false) // Ensure Timer is Started
		{
			m_NextParticleTimer.Start();
		}

		const float fNextTime = m_NextParticleTimer.GetTime();
		if (fNextTime >= m_Config.FixedParticleSpawnInterval) // Timer Elapsed
		{
			for (int i = 0; i < m_Config.ParticlesPerInterval; ++i)
			{
				SpawnParticle();
			}
			
			m_NextParticleTimer.SetTime(0.f);
		}
	}

	for (auto& p : m_Particles)
	{
		p->LiveTime += Time::DeltaTime();
		p->Position[0] += p->Velocity[0] += Time::DeltaTime();
		p->Position[1] += p->Velocity[1] += Time::DeltaTime();
		p->Position[2] += p->Velocity[2] += Time::DeltaTime();
	}

	for (int i = (int)m_Particles.size() - 1; i >= 0; --i)
	{
		Particle* p = m_Particles[i];
		if (p->LiveTime >= m_Config.LifeTime)
		{
			DestroyParticle(i);
		}
	}

	PROFILER_END_SAMPLE();
}

void ParticleSystemScenario::ParticleSystem::OnRender(IRenderer* a_pRenderer)
{
	PROFILER_BEGIN_SAMPLE(ParticleSystem::OnRender);
	for(auto& p : m_Particles)
	{
		// TODO Optimise Renderer KILLS FPS probs due to Constant/create/destroy of buffers
		//a_pRenderer->DrawCube(p.Position[0], p.Position[1], p.Position[2], 0.1f, Time::DeltaTime());
	}
	PROFILER_END_SAMPLE();
}

ParticleSystemScenario::ParticleSystem::Particle::Particle(float a_fX, float a_fY, float a_fZ)
	:
	Position{ a_fX, a_fY, a_fZ },
	Velocity{ Random::FloatRange(-1.f, 1.f), // Random Dirs
			 Random::FloatRange(-1.f, 1.f) ,
			 Random::FloatRange(-1.f, 1.f) },
	LiveTime(0.f)
{
	// Normalise Dir
	const float len = Velocity[0] + Velocity[1] + Velocity[2];
	Velocity[0] /= len;
	Velocity[1] /= len;
	Velocity[2] /= len;
}

void ParticleSystemScenario::ParticleSystem::SpawnParticle()
{
	PROFILER_BEGIN_SAMPLE(ParticleSystem::SpawnParticle);
	// Push new Particle
#if USE_MEM_SYS
	Particle* pParticle = (Particle*)m_ParticlePool.allocate(sizeof(Particle), 8);
	*pParticle = Particle(m_Config.OriginPosition[0], m_Config.OriginPosition[1], m_Config.OriginPosition[2]);
	m_Particles.push_back(pParticle);
#else
	m_Particles.push_back(new Particle(m_Config.OriginPosition[0], m_Config.OriginPosition[1], m_Config.OriginPosition[2]));
#endif

	m_TotalAllocatedParticles++;

	// Stop Timer
	m_NextParticleTimer.Stop();
	if ((int)m_Particles.size() < m_Config.MaxParticles) // Still Not Reached Max - Start Timer
	{
		m_NextParticleTimer.Start();
	}

	PROFILER_END_SAMPLE();
}

void ParticleSystemScenario::ParticleSystem::DestroyParticle(int a_pIdx)
{
	PROFILER_BEGIN_SAMPLE(ParticleSystem::DestroyParticle);
	Particle* pParticle = m_Particles[a_pIdx];
	m_Particles.erase(m_Particles.begin() + a_pIdx);
#if USE_MEM_SYS
	m_ParticlePool.deallocate(pParticle);
#else
	delete pParticle;
#endif
	PROFILER_END_SAMPLE();
}

void ParticleSystemScenario::Initialise()
{
	PROFILER_BEGIN_SAMPLE(ParticleSystemScenario::Initialise);
	
	while ((int)m_ParticleSystems.size() != m_Config.ParticleSystemsCount)
	{
		ParticleSystem::Config config =
		{
			m_Config.ParticleSystem.MaxParticles,
			m_Config.ParticleSystem.ParticleStartCount,
			m_Config.ParticleSystem.FixedParticleSpawnInterval,
			m_Config.ParticleSystem.ParticlesPerInterval,
			Random::FloatRange(m_Config.ParticleSystem.ParticleLifeTimeMin, m_Config.ParticleSystem.ParticleLifeTimeMax),
		{0.f} // Zero Vector
		};
		m_ParticleSystems.emplace_back(config);
	}
	PROFILER_END_SAMPLE();
}

