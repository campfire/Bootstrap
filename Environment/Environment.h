#pragma once

#ifdef _MSC_VER
	#ifdef LIBRARY_ENVIRONMENT
	#define symbol __declspec(dllexport)
	#else
	#define symbol __declspec(dllimport)
	#endif
#else
#define symbol __attribute__((visibility("default")))
#endif

extern "C" {
	symbol void Environment_load(int , char **);
}