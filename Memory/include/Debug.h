#pragma once
#include "Exception.h"


#define BREAK { __asm int 3}

class AssertFailException : Exception
{
public:
	AssertFailException(int a_line, const char* a_fileName, const char* a_expression);

	const char* what() const override;
	virtual const char* GetType() const { return TEXT(AssertFailException); }
	std::string GetFailedExpressionString() const { return m_failedExpression; }
private:
	std::string m_failedExpression;
};

static void AssertFail(int a_line, const char* a_fileName, const char* a_expression)
{
	throw AssertFailException(a_line, a_fileName, a_expression);
}

#define ASSERT(x) (void)((x) || (AssertFail(__LINE__, __FILE__, #x), 0))