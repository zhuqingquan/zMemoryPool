
#ifndef _MEMORYPOOLHEADER_
#define _MEMORYPOOLHEADER_

#ifdef _WINDOWS
#ifdef MEMORYPOOL_EXPORT
#define MEMORYPOOL_EXPORT_IMPORT __declspec(dllexport)
#else
#define MEMORYPOOL_EXPORT_IMPORT __declspec(dllimport)
#endif 
#else  //else _WINDOWS
#define MEMORYPOOL_EXPORT_IMPORT 
#endif //_WINDOWS

#ifndef HAS_CPP11
#	ifdef _MSVC_LANG
#		if _MSVC_LANG > 199711L
#			define HAS_CPP11 1
#		else
#			define HAS_CPP11 0
#		endif
#	esle
#		if __cplusplus > 199711
#			define HAS_CPP11	1
#		else
#			define HAS_CPP11 0
#		endif
#	endif
#endif
#endif//#define _MEMORYPOOLHEADER_
