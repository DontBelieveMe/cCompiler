#pragma once

#include <cc/logging.h>

#ifdef _DEBUG
#define ASSERT(condition, ...) \
	if(!(condition)) { \
		CFATAL("Assertion failed: {0}", #condition); \
		CFATAL(__VA_ARGS__); \
		__debugbreak(); \
	};
#else
#define ASSERT(condition, ...)
#endif