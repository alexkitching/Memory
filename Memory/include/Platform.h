#pragma once

//------------
// Description
//--------------
// Platform Specific Defines
//------------

#if _WIN64
#define x64
#define DEFAULT_ALIGNMENT 8u
#elif _WIN32
#define x32
#define DEFAULT_ALIGNMENT 4u
#endif