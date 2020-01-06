#include "Debug.h"
#include<sstream>

ILogHandler* Debug::s_pLogHandler = nullptr;

AssertFailException::AssertFailException(int a_line, const char* a_fileName, const char* a_expression)
	:
Exception(a_line, a_fileName),
m_failedExpression(a_expression)
{
}

const char* AssertFailException::what() const
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Failed Expression] " << std::endl 
	<< GetFailedExpressionString() << std::endl;

	oss << GetOriginalString();

	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}
