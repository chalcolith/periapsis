#ifndef GSGL_DATA_STDAFX_H
#define GSGL_DATA_STDAFX_H

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

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

// TODO: reference additional headers your program requires here

#include <cstdlib>
#include <cstdarg>
#include <cstdio>
#include <cwchar>
#include <cstring>
#include <typeinfo>
#include <cwctype>
#include <cctype>
#include <cerrno>

#ifdef WIN32
#include <direct.h>
#include <io.h>
#endif

#endif
