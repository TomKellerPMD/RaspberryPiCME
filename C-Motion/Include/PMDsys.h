#ifndef _PMDSYS_H
#define _PMDSYS_H

/*
  PMDsys.h

  This file defines the operating system-dependant functions
  The CME definition is defined in the user code makefile.
  
  Performance Motion Devices, Inc.
*/

#include "PMDtypes.h"
#include "PMDRPtypes.h"

typedef void (*TaskPointer)( void* pParameters );

#ifdef CME

#include <stddef.h>
#include <stdbool.h>
#include "locations.h"

#define PMDTaskParam PMDparam
typedef void (*TaskFuncPointer)( PMDTaskParam TaskParam);
typedef void (*FuncPointer)( void );
#define TASKFUNC static const TaskFuncPointer
#define EXPORTFUNC static const FuncPointer

void CopyDataSectionToRAM(void);

#define USER_CODE_VERSION( UCmajorversion, UCminorversion )  \
    __attribute__((used, section ("header"))) \
    static const char CodeSignature[12] = UC_DATA_SIGNATURE; \
    __attribute__((used, section ("header"))) \
    static const unsigned long Version = UC_DATA_CMEVERSION; \
    __attribute__((used, section ("date"))) \
    static const char DateTime[] = __DATE__"  " __TIME__; \
    static const char UCFileName[] = __FILE__;  \
    __attribute__((used, section ("filename"))) \
    static const char* pUCFileName = UCFileName;  \
    __attribute__((used, section ("fileversion")))   \
    static const unsigned long UCFileVersion = (UCmajorversion << 16) | UCminorversion; \
    __attribute__((used, section ("funcstart"))) \
    EXPORTFUNC CopyDataSectionToRAM_address = CopyDataSectionToRAM;

#define USER_CODE_LOOP_BEGIN while(1) {
#define USER_CODE_LOOP_END   }

// User code task function definition macro
// note: the ## is used to concatenate strings in macros
#define USER_CODE_TASK( sTaskName ) \
    void sTaskName( PMDTaskParam TaskParam ); \
    __attribute__((used, section ("functable"))) \
    TASKFUNC sTaskName ## _address = sTaskName ;  \
    __attribute__((used, section ("funcnames"))) \
    static const char sTaskName ## _name[] = #sTaskName ;  \
    __attribute__((used, section ("funcstack"))) \
    static const long sTaskName ## _stack = 0;  \
    void sTaskName( PMDTaskParam TaskParam ) 

#ifndef DWORD
#define DWORD PMDuint32
#endif
#ifndef WORD
#define WORD PMDuint16
#endif
#ifndef BYTE
#define BYTE PMDuint8
#endif
#ifndef NULL
#define NULL (void*)0
#endif
#ifndef BOOL
#define BOOL int
#endif
     
#define PMDgetch		' '
#define PMDkbhit()		false
void   PMDputch(int ch);
int    PMDputs(const char *str);
size_t PMDwrite(const char *str, int length);
size_t PMDread(char *str, int length);
int    PMDprintf(const char *fmt, ...);
int    PMDsprintf(char *str, const char *fmt, ...);
void   PMDTaskWait(DWORD milliseconds);
void   PMDTaskWaitUntil(DWORD *pPreviousWakeTime, DWORD TimeIncrementms);
void   PMDTaskAbort(int UserAbortCode);
int    PMDTaskGetAbortCode(void);
int    PMDTaskGetNumber(void);
void   PMDTaskEnterCritical(void);
void   PMDTaskExitCritical(void);
int    PMDTaskCreate(TaskPointer pTask, const char* name, size_t stacksize, void* taskparam, PMDTaskPriority priority);
PMDresult PMDTaskSetPriority(PMDTaskPriority priority);
DWORD  PMDDeviceGetTickCount(void);
DWORD  PMDDeviceGetMicroseconds(void);

typedef void* PMDMutexHandle;
#define PMDMutexDefine(mutex)                   static PMDMutexHandle mutex = NULL;
#define PMDMutexLock(hMutex)                    PMDMutexLockEx(hMutex, MUTEX_TIMEOUT)
PMDMutexHandle PMDMutexCreate(void);
bool  PMDMutexLockEx(PMDMutexHandle hMutex, DWORD timeout);
bool  PMDMutexUnlock(PMDMutexHandle hMutex);
void  PMDMutexDelete(PMDMutexHandle hMutex);


typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME;


#elif defined WIN32 

#include <stdio.h>
#include <windows.h>
#include <assert.h>
#include <tchar.h>
#include <process.h>
#include <conio.h>
#define PMDkbhit									_kbhit
#define PMDgetch									_getch
#define PMDputch                                    putch
#define PMDputs                                     puts
#define PMDprintf                                   printf
#define PMDsprintf                                  sprintf
#define PMDTaskWait(ms)                             Sleep(ms)
#define PMDTaskWaitUntil(a, ms)                     Sleep(ms)
#define PMDTaskAbort(code)                          exit(code)
#define PMDTaskEnterCritical()                      
#define PMDTaskExitCritical()                       
#define PMDDeviceGetTickCount()                     GetTickCount()
#define PMDDeviceGetMicroseconds()                  0
#define PMDTaskCreate(threadfn, name, stacksize, param, priority)  CreateThread( NULL, stacksize, threadfn, param, 0, NULL) 

#define USER_CODE_TASK(a)                           int main(int argc, char* argv[])
#define USER_CODE_VERSION(major, minor)

#define PMDMutexHandle                              HANDLE
#define PMDMutexDefine(mutex)                       static PMDMutexHandle mutex = NULL;
#define PMDMutexDelete(mutex)                       CloseHandle(mutex);
#define PMDMutexCreate()                            CreateMutex( NULL, FALSE, NULL )
#define PMDMutexLock(mutex)                         (WAIT_OBJECT_0 == WaitForSingleObject( mutex, MUTEX_TIMEOUT ))
#define PMDMutexLockEx(mutex, timeout)              (WAIT_OBJECT_0 == WaitForSingleObject( mutex, timeout ))
#define PMDMutexUnlock(mutex)                       ReleaseMutex( mutex );

#else

#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <linux/types.h>
#include <sys/time.h>
#include <sys/file.h>
#include <assert.h>
#include <stdbool.h>

#define ASSERT assert

#ifndef DWORD
#define DWORD PMDuint32
#endif
#ifndef WORD
#define WORD PMDuint16
#endif
#ifndef BYTE
#define BYTE PMDuint8
#endif
#ifndef NULL
#define NULL 0
#endif
#ifndef BOOL
#define BOOL bool
#endif




#define PMDputch                  putch
#define PMDputs                   puts
#define PMDprintf                 printf
#define PMDsprintf                sprintf
#define PMDTaskAbort(code)        abort()
#define PMDDeviceGetTickCount()   GetTickCount()
#define GetCurrentMilliseconds()  GetTickCount()
#define INVALID_HANDLE_VALUE      ((long)-1)
#define HANDLE                    int


#ifdef __cplusplus
extern "C" {
#endif

void PMDputch(int ch);
int  PMDputs(const char *str);
int  PMDprintf(const char *fmt, ...);
int  PMDsprintf(char *str, const char *fmt, ...);
void PMDTaskWait(DWORD milliseconds);
void PMDTaskWaitUntil(DWORD *pPreviousWakeTime, DWORD TimeIncrementms);
void PMDTaskAbort(int UserAbortCode);
int  PMDTaskGetAbortCode(void);
void PMDTaskEnterCritical(void);
void PMDTaskExitCritical(void);
DWORD PMDDeviceGetTickCount();


typedef void* PMDMutexHandle;
//static PMDMutexHandle dummy;

#define PMDMutexDefine(mutex)                   static PMDMutexHandle mutex = NULL;
#define PMDMutexLock(hMutex)                    PMDMutexLockEx(hMutex, MUTEX_TIMEOUT)
//#ifndef _PMDSYS_H
//#define PMDMutexCreate() void *        ///             static PMDMutexHandle mutex=NULL)
//#endif
//#define PMDMutexLockEx(hMutex,timeout) TRUE
//bool  PMDMutexUnlock(PMDMutexHandle hMutex) return TRUE;
#define PMDMutexLockEx(hMutex, timeout) TRUE
//PMDMutexHandle PMDMutexCreate();
//bool  PMDMutexLockEx(PMDMutexHandle hMutex, DWORD timeout);
#define  PMDMutexUnlock(hMutex) TRUE
PMDMutexHandle PMDMutexCreate(void);
void  PMDMutexDelete(PMDMutexHandle hMutex);

#ifdef __cplusplus
}
#endif


typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME;

#endif

// Define NOT_USING_MUTEXES to disable mutexes which are not needed in a single threaded environment.
//#define NOT_USING_MUTEXES
#ifdef NOT_USING_MUTEXES
#undef PMDMutexDelete
#undef PMDMutexCreate
#undef PMDMutexLock
#undef PMDMutexLockEx
#undef PMDMutexUnlock

#define PMDMutexDelete(mutex)
#define PMDMutexCreate()                        NULL     
#define PMDMutexLock(mutex)                     true 
#define PMDMutexLockEx(mutex, timeout)          true
#define PMDMutexUnlock(mutex)                   true
#endif

#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#define PMDZeroMemory(dest, count)              memset(dest, 0, count)
#define MUTEX_TIMEOUT                           (1000)

#endif
