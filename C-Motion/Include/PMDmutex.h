#ifndef _DEF_INC_PMDmutex
#define _DEF_INC_PMDmutex

#include "PMDsys.h"

#ifdef USE_MUTEXES
#define PMDMutexType			HANDLE
#define PMDMutexDefine(mutex)   static PMDMutexType mutex = NULL;
#define PMDMutexDelete(mutex)   CloseHandle(mutex);
#define PMDMutexCreate(mutex)   if (mutex == NULL) mutex = CreateMutex( NULL, FALSE, _T(#mutex) );
#define PMDMutexLock(mutex)     (WAIT_OBJECT_0 == WaitForSingleObject( mutex, 2000 ))
#define PMDMutexUnLock(mutex)   ReleaseMutex( mutex );

#else

#define PMDMutexType			HANDLE
#define PMDMutexDefine(mutex)   
#define PMDMutexDelete(mutex)   
#define PMDMutexCreate(mutex)   
#define PMDMutexLock(mutex)     (TRUE)
#define PMDMutexUnLock(mutex)   

#endif
#endif
