#ifndef PMD_Diagnostics
#define	PMD_Diagnostics

//  PMDdiag.h -- diagnostic functions
//
//  Performance Motion Devices, Inc.
//

#include "PMDtypes.h"

#define PMD_RESULT(_call)  {result = _call; if (result) PMDprintf("Error 0x%04X line: %d, %s\r\n", result, __LINE__, PMDGetErrorMessage(result));}
// Use the following PMD_RESULT macro instead to print out the function that caused an error to aid debugging
//#define PMD_RESULT(_call)  {result = _call; if (result) PMDprintf("Error line: %d, %s   %.60s\r\n", __LINE__, PMDGetErrorMessage(result), #_call);}
// Use the following PMD_RESULT macro to print out every function call 
//#define PMD_RESULT(_call)  {PMDprintf("Calling: %s\r\n", #_call); result = _call; if (result) PMDprintf("Error line: %d, %s\r\n", __LINE__, PMDGetErrorMessage(result));}
#define PMD_ABORTONERROR(_call)  {PMD_RESULT(_call) if (result) PMDTaskAbort(result);}

#if defined(__cplusplus)
extern "C" {
#endif

const char *PMDGetOpcodeText(PMDuint16 opCode);
const char *PMDGetErrorMessage(PMDresult errorCode);
const char *PMDGetResetCauseMessage(PMDuint16 resetcause);

#if defined(__cplusplus)
}
#endif

#endif

