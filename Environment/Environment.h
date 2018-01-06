#pragma once
#include <QtGlobal>

#ifdef LIBRARY_ENVIRONMENT
#define symbol Q_DECL_EXPORT
#else
#define symbol Q_DECL_IMPORT
#endif

extern "C" {
	symbol void Environment_load(int , char **);
}