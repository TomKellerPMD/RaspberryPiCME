//
//  PMDser.c -- Motion Processor serial protocol functions
//
//  Performance Motion Devices, Inc.
//

#include <afxtempl.h>
#include <stdio.h>
#include <stdlib.h>

#include "PMDtypes.h"
#include "PMDecode.h"
#include "PMDdevice.h"
#include "PMDperiph.h"
#include "PMDPfunc.h"

#define MAXCPPACKETSIZE 20

static 	CWordArray NVRAMbuff;

//********************************************************
PMDresult PMDNVRAMCommandAtlas(void* transport_data, PMDuint8 xCt, PMDuint16* xDat, PMDuint8 rCt, PMDuint16* rDat)
{
    PMDDeviceHandle* hDevice = (PMDDeviceHandle*)transport_data;
    int c=0;
    int i;
    int sum = 0;
    BYTE axis;

    axis = (BYTE)(xDat[0] >> 8);
	if (axis > 0)
		return PMD_ERR_MP_InvalidAxis; 

	// calculate ones complement checksum.
	// The seed is used for Atlas to make sure that a data bit stuck high or low results in a checksum error every time.
	sum = 0xAA; //seed
	c = xCt;
	for( i=0; i<c; i++ )
	{
		sum += (BYTE)(xDat[i] >> 8) ;
		sum = (0xFF & sum) + (sum >> 8); // add in the carry
		sum += (BYTE)(xDat[i]) ;
		sum = (0xFF & sum) + (sum >> 8); // add in the carry
	}
	// for Atlas the checksum replaces the axis byte 
	xDat[0] |= (0xFF ^ sum) << 8;

    for( i=0; i<xCt; i++ )
    {
        NVRAMbuff.Add(xDat[i]);
    }

    return PMD_NOERROR;
}


//********************************************************
PMDresult PMDNVRAMCommands_Init()
{
	NVRAMbuff.RemoveAll();
    return PMD_NOERROR;
}

void PMDNVRAMCommandsGetBuffer(CWordArray& data)
{
	data.Copy(NVRAMbuff);
}

CWordArray& PMDNVRAMCommandsGetBuffer()
{
	return NVRAMbuff;
}

PMDCFunc PMDNVRAMCommandsDeviceOpen(PMDDeviceHandle *hDevice, PMDPeriphHandle *hPeriph)
{
    PMDZeroMemory(hDevice, sizeof(PMDDeviceHandle));
    hDevice->hPeriph = hPeriph;
    hDevice->type = PMDDeviceTypeMotionProcessor;

    hDevice->transport.SendCommand = PMDNVRAMCommandAtlas;
	NVRAMbuff.RemoveAll();

	return PMD_NOERROR;
}