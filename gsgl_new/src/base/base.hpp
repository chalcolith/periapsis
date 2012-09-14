#ifndef GSGL_BASE_H
#define GSGL_BASE_H

#ifdef WIN32
#ifdef BASE_EXPORTS
#define BASE_API __declspec(dllexport)
#else
#define BASE_API __declspec(dllimport)
#endif
#endif

#ifdef WIN32
#pragma warning(disable : 4996)
#pragma warning(disable : 4251)
#endif

#endif
