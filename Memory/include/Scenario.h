#pragma once

class IScenario
{
public:
	virtual ~IScenario() {}

	virtual void Run() = 0;
	virtual void Reset() = 0;
	
	virtual bool IsComplete() = 0;
};