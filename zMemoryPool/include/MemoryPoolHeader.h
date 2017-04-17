
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

#endif
