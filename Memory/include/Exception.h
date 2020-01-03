#pragma once
#include <exception>
#include <string>
#include "Common.h"

class Exception : public std::exception
{
public:
	Exception(int a_line, const char* a_File, const char* a_TypeName = nullptr);

	const char* what() const override;

	virtual const char* GetType() const { return  m_type.empty() ? TEXT(Exception) : m_type.c_str(); } // Exception Type Name
	int GetLine() const { return m_line; }
	const std::string& GetFile() const { return m_file; }
	std::string GetOriginalString() const;

protected:
	mutable std::string m_whatBuffer;
private:
	int m_line;
	std::string m_file;
	std::string m_type;
};

#define EXCEPT(typeName) Exception(__LINE__, __FILE__, #typeName); 