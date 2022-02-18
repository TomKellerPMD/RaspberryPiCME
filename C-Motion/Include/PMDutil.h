#ifndef PMD_Util
#define PMD_Util
// ****************************************************************
// PMD_Util.h : sample test and utility function declerations
//
// Performance Motion Devices, Inc.
//

#if defined(__cplusplus)
extern "C" {
#endif

#include "C-Motion.h"

PMDCFunc PMDPeriphOpen(PMDPeriphHandle* hPeriph, PMDInterfaceType interfacetype, int portoffset, int listen);
void SetupTrace(PMDAxisHandle* phAxis, PMDuint32 bufferlength);
void DisplayTraceResults(PMDAxisHandle* phAxis);
PMDresult ReadBuffer(PMDAxisHandle* phAxis, PMDuint16 bufferID, PMDint32* pbuffer, PMDuint32 dwords_to_read);
PMDresult WaitForEvent(PMDAxisHandle* phAxis, PMDuint16 eventmask, PMDuint32 timeoutms);
PMDresult WaitForAlgorithmicPhaseInitialization(PMDAxisHandle* phAxis);
PMDresult InitializePhase(PMDAxisHandle* phAxis);
PMDresult WaitForAtlasToConnect(PMDAxisHandle* phAxis);
struct timeval ConvertTime(long msec);

// in Pro-MotionExport.c
PMDresult SetupAxis1(PMDAxisHandle* phAxis); 
PMDresult SetupAxis2(PMDAxisHandle* phAxis); 
PMDresult SetupAxis3(PMDAxisHandle* phAxis); 
PMDresult SetupAxis4(PMDAxisHandle* phAxis); 


#if defined(__cplusplus)
}
#endif

#endif
