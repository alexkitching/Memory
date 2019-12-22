#include "Exception.h"
#include <sstream>

Exception::Exception(int a_line, const char* a_File)
	:
m_line(a_line),
m_file(a_File)
{
}

const char* Exception::what() const
{
	std::ostringstream oss;
	oss << GetType() << std::endl
	<< GetOriginalString();

	m_whatBuffer = oss.str();
	return m_whatBuffer.c_str();
}

std::string Exception::GetOriginalString() const
{
	std::ostringstream oss;
	oss << "[File] " << m_file << std::endl
	<< "[Line] " << m_line; 
	return oss.str();
}
