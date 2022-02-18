///////////////////////////////////////////////////////////////////////////
//
//  c-motionSpecial.c -- Special C-Motion API implementation
//
//  Performance Motion Devices, Inc.
//  
///////////////////////////////////////////////////////////////////////////


#include "c-motion.h"
#include "PMDocode.h"


PMDresult PMDPoke(PMDAxisInterface axis_intf, PMDuint16 address, PMDuint16 data) 
{
	return SendCommandWordWordWord(axis_intf, 0xF1, 1, address, data); 
}

PMDresult PMDPeek(PMDAxisInterface axis_intf, PMDuint16 address, PMDuint16* data) 
{
	return SendCommandWordWordGetWord(axis_intf, 0xF0, 1, address, data); 
}

PMDresult PMDAtlasPeek(PMDAxisInterface axis_intf, PMDuint32 address, PMDuint16* data) 
{
	PMDuint16 address1 = (PMDuint16)((address >> 16) | 0x100); //(0=data space, 1=pgm space)
	PMDuint16 address2 = (PMDuint16)address;

	return SendCommandWordWordGetWord(axis_intf, 0xF0, address1, address2, data); 
}




// Enables reading and writing to ION EEPROM defaults above 1.
// I.e. PMDSetDefault( 2, 200 )
// note: this command will return an invalid opcode command error

PMDresult PMDUnlockDefaults(PMDAxisInterface axis_intf)
{
	return SendCommandWord(axis_intf, 0xFE, 0x4A47);
}



// if the bus current exceeds this value the ION will hard fault 
PMDresult PMDSetMaxBusCurrent(PMDAxisInterface axis_intf, PMDuint16 value) 
{
	return SendCommandWordWordWord(axis_intf, 0xF1, 1, 0xF5D, value); 
}



PMDresult PMDSetBufferFunction(PMDAxisInterface axis_intf, PMDuint16 function, PMDint16 bufferID)
{
	return SendCommandWordWord(axis_intf, PMDOPSetBufferFunction, function, (PMDuint16)bufferID);
}

PMDresult PMDGetBufferFunction(PMDAxisInterface axis_intf, PMDuint16 function, PMDint16* bufferID)
{
	return SendCommandWordGetWord(axis_intf, PMDOPGetBufferFunction, function, (PMDuint16*)bufferID);
}

PMDresult PMDDriveFlash(PMDAxisInterface axis_intf, PMDuint16 op, PMDuint16 arg)
{
  return SendCommandWordWord(axis_intf, 0x30, op, arg);
}
