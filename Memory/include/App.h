#pragma once

class IApp
{
public:
	IApp() {}
	virtual ~IApp() {}

	virtual int Run() = 0;
	virtual bool Initialise() = 0;
	virtual void OnExit() = 0;
};