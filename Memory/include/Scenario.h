#pragma once

#include "MemSys.h"

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