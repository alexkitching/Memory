#pragma once

//------------
// Description
//--------------
// Primary Windows Includes
//------------

// target Windows 7 or later
#define _WIN32_WINNT 0x0601
#include <sdkddkver.h>

#define WIN32_LEAN_AND_MEAN
#define NOGDICAPMASKS
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOSYSCOMMANDS
#define NORASTEROPS
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
#define NOKERNEL
#define NONLS
#define NOMEMMGR
#define NOMETAFILE
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NORPC
#define NOPROXYSTUB
#define NOIMAGE
#define NOTAPE

#define NOMINMAX

#define STRICT

#include <Windows.h>
#undef TEXT
#include "Common.h"
#include <string>

static std::string TranslateHResult(HRESULT a_hresult)
{
	char* pMsgBuf = nullptr;

	// Decode HRESULT Message, Creates Local String on windows side that is freed below.
	DWORD nMsgLen = FormatMessage(
									FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
									nullptr, a_hresult, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
									reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
								);

	if(nMsgLen == 0)
	{
		return "Unidentified Error Code";
	}

	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);

	return errorString;
}
