/*****************************************************************//**
 * \file   DebugPrint.h
 * \brief  Header file to make sure there is no overhead for message
 *		   printing in the RELEASE build.
 * 
 * \author Mikalai Varapai
 * \date   May 2025
 *********************************************************************/
#pragma once

#if defined(_DEBUG) || defined(DEBUG)	// Debug build: show messages

#include <debugapi.h>
#include <cstdio>
#include <cstdarg>

// Only use via macro
inline void DebugPrint(const char* fmt, ...)
{
	char buf[1024];
	va_list args;
	va_start(args, fmt);
	vsnprintf_s(buf, sizeof(buf), _TRUNCATE, fmt, args);
	va_end(args);
	OutputDebugStringA(buf);
}

#define DPRINT(...)								\
do {											\
	DebugPrint(__VA_ARGS__);					\
	OutputDebugStringA("\n");					\
} while(0)


#define DPRINT_LOC(...)							\
do {											\
	DebugPrint("[%s:%u] ", __FILE__, __LINE__);	\
	DebugPrint(__VA_ARGS__);					\
	OutputDebugStringA("\n");					\
} while(0)

#else									// Release build: remove debug messages

#define DPRINT(...) do {} while(0)
#define DPRINT_LOC(...) do {} while(0)

#endif
