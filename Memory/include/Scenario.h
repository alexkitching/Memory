#pragma once


class IRenderer;
class IScenario
{
public:
	virtual ~IScenario() {}

	virtual void Run() = 0;
	virtual void OnRender(IRenderer* a_pRenderer) {};
	virtual void Reset() = 0;
	
	virtual bool IsComplete() = 0;
};