#pragma once

//------------
// Description
//--------------
// IApp Interface, Base Interface for an Application. Outlines basic App Functions.
//------------

class IApp
{
public:
	IApp() {}
	virtual ~IApp() {}

	// Prevent Copies
	IApp(const IApp&) = delete;
	IApp& operator=(const IApp&) = delete;

	// Called from Main
	virtual int Run() = 0;

protected:
	virtual bool Initialise() = 0;
	virtual void OnExit() = 0;
};