#ifndef GSGL_MATH_STDAFX_H
#define GSGL_MATH_STDAFX_H

// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#ifdef WIN32
#pragma once
// Including SDKDDKVer.h defines the highest available Windows platform.
// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.
#include <SDKDDKVer.h>
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#endif

// TODO: reference additional headers your program requires here
#define _USE_MATH_DEFINES
#include <cmath>
#include <ctime>
#include <cstring>
#include <ctime>
#include <cwchar>
#include <cwctype>

#endif
