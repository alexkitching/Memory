#pragma once
#include "Exception.h"
#include <cstdarg>

//------------
// Description
//--------------
// This header contains Common Debug Related Defines and Classes providing a variety of debug functionality.
//------------


// Crashes 'Breaks' the program
#define BREAK { __asm int 3}

// Assert Fail Exception thrown on Assertion.
class AssertFailException : public Exception
{
public:
	AssertFailException(int a_line, const char* a_fileName, const char* a_expression);

	const char* what() const override;
	virtual const char* GetType() const { return TEXT(AssertFailException); }
	std::string GetFailedExpressionString() const { return m_failedExpression; }

private:
	std::string m_failedExpression;
};

// Called within Assert Macro
static void AssertFail(int a_line, const char* a_fileName, const char* a_expression)
{
	throw AssertFailException(a_line, a_fileName, a_expression);
}

#if DEBUG
#define ASSERT(x) (void)((x) || (AssertFail(__LINE__, __FILE__, #x), 0))
#define UNREACHABLE(x) ASSERT(false && ("UNREACHABLE::" x))
#else
#define ASSERT(x) (x)
#define UNREACHABLE(x)
#endif

// Logging

// Log Handler Interface to recieve log callbacks from define.
class ILogHandler
{
public:
	virtual ~ILogHandler() {}

	virtual void Log(const char* fmt, ...) = 0;
	virtual void OnLog(const char* fmt, va_list a_va) = 0;
};

#if DEBUG
#define LOG(x, ...) Debug::Log(x, __VA_ARGS__);
#else
#define LOG(x, ...)
#endif

// Static Debug Class Used to Set Log Handler and Containing the Log Function.
class Debug
{
public:
	static void SetLogHandler(ILogHandler* a_pHandler) { s_pLogHandler = a_pHandler; }
	static void Log(const char* fmt, ...)
	{
		if (s_pLogHandler != nullptr)
		{
			va_list va;
			va_start(va, fmt);

			s_pLogHandler->OnLog(fmt, va);
			va_end(va);
		}
	};
private:
	static ILogHandler* s_pLogHandler;
};