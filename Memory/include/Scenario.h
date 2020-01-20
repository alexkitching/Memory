#pragma once

//------------
// Description
//--------------
// Base Memory Scenario Interface
// Contains primary functions for a memory scenario to be called from Scenario Manager
//------------

class IRenderer;
class IScenario
{
public:
	virtual ~IScenario() {}

	virtual void Initialise() {}
	virtual void Run() = 0;
	virtual void OnRender(IRenderer* a_pRenderer) {};
	virtual void Reset() = 0;
	
	virtual bool IsComplete() = 0;
};