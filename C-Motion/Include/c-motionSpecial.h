///////////////////////////////////////////////////////////////////////////
//
//  c-motionSpecial.h -- Special C-Motion API
//
//  Performance Motion Devices, Inc.
//  
///////////////////////////////////////////////////////////////////////////

#if defined(__cplusplus)
extern "C" {
#endif


PMDresult PMDPoke(PMDAxisInterface axis_intf, PMDuint16 address, PMDuint16 data);
PMDresult PMDPeek(PMDAxisInterface axis_intf, PMDuint16 address, PMDuint16* data);
PMDresult PMDAtlasPeek(PMDAxisInterface axis_intf, PMDuint32 address, PMDuint16* data);
PMDresult PMDUnlockDefaults(PMDAxisInterface axis_intf);
PMDresult PMDSetMaxBusCurrent(PMDAxisInterface axis_intf, PMDuint16 value);
PMDresult PMDSetBufferFunction(PMDAxisInterface axis_intf, PMDuint16 function, PMDint16 bufferID);
PMDresult PMDGetBufferFunction(PMDAxisInterface axis_intf, PMDuint16 function, PMDint16* bufferID);
PMDresult PMDSetAnalogCalibration(PMDAxisInterface axis_intf, PMDuint16 parameter, PMDint16 value);
PMDresult PMDGetAnalogCalibration(PMDAxisInterface axis_intf, PMDuint16 parameter, PMDint16* value);
PMDresult PMDDriveFlash(PMDAxisInterface axis_intf, PMDuint16 op, PMDuint16 arg);


// undocumented command parameters 
enum {
		PMDTraceSPIChecksumErrors = 255
};


#if defined(__cplusplus)
}
#endif

