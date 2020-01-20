#pragma once

//------------
// Description
//--------------
// Static Performance Counter Class, used for Recording FPS/Memory Usage of the Application
//------------

class PerformanceCounter
{
public:
	static void Tick();

	// Stat Properties
	static float FPS() { return s_fFPS;  }
	static float PhysicalMemoryUsed() { return s_fPhysMemUsed; }
	static float PhysicalMemoryTotal() { return s_fPhysMemTotal;  }
	static float VirtualMemoryUsed() { return s_fVirMemUsed;  }
	static float VirtualMemoryTotal() { return s_fVirMemTotal;  }
	
private:
	static float s_fDeltaTime;
	static float s_fAccumFPS;
	static int s_FrameCount;
	static constexpr float s_fFPSUpdateRate = 2.f;

	// Public Accessed Stats
	static float s_fFPS;
	static float s_fPhysMemUsed;
	static float s_fPhysMemTotal;
	static float s_fVirMemUsed;
	static float s_fVirMemTotal;
};