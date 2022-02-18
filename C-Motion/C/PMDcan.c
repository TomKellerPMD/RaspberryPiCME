//
//  PMDcan.c -- Motion Processor CAN Protocol functions
//
//  Performance Motion Devices, Inc.
//

#include "PMDtypes.h"
#include "PMDecode.h"
#include "PMDdevice.h"
#include "PMDperiph.h"
#include "PMDsys.h"


// only need to include this if diagnostics mode is used
#include "PMDdiag.h"

#define MP_TIMEOUT         100

#define MAXCPPACKETSIZE 20

PMDMutexDefine(xMutexCAN)


//********************************************************
PMDresult PMDCAN_Send(void* transport_data, PMDuint8 xCt, PMDuint16* xDat, PMDuint8 rCt, PMDuint16* rDat)
{
    PMDDeviceHandle* hDevice = (PMDDeviceHandle*)transport_data;
    PMDPeriphHandle* hPeriph = hDevice->hPeriph;
    int c=0;
    int i;
    PMDparam nExpected = 2*rCt+2;
    PMDparam bytes;
    PMDuint8 txbuffer[MAXCPPACKETSIZE];
    PMDuint8 rxbuffer[MAXCPPACKETSIZE];
    PMDuint8* prxbuffer = &rxbuffer[0];
    PMDuint16 ProcessorError;
    PMDresult result;

    PMD_PERIPHCONNECTED(hPeriph);

    if( hPeriph->type != InterfaceCAN )
        return PMD_ERR_InvalidInterface;


    // add data (handling byte swapping)
    for( i=0; i<xCt; i++ )
    {
        txbuffer[ c++ ] = (char)(xDat[i] >> 8);
        txbuffer[ c++ ] = (char)(xDat[i] & 0xFF);
    }

    if (!PMDMutexLock(xMutexCAN))
      return PMD_ERR_MutexTimeout;

    // Flush the receive buffer in case any unexpected bytes have been received
//    PMDPeriphFlush(hPeriph);

    result = PMDPeriphSend(hPeriph, txbuffer, c, MP_TIMEOUT);
    if (result == PMD_ERR_OK)
    {
        // read return data
        nExpected = 2*rCt+2;
        // CAN returns data in blocks of 8 bytes
        result = PMDPeriphReceive(hPeriph, rxbuffer, &bytes, nExpected, MP_TIMEOUT);
    }
    PMDMutexUnlock(xMutexCAN);
    if (result != PMD_ERR_OK)
      return result;

    // second byte in response is a motion processor error code
    ProcessorError = prxbuffer[1];

    // if there was an error, don't attempt to receive any data
    if( ProcessorError )
    {
        rCt = 0;
        nExpected = 2;
    }

    // if we don't get enough bytes
    if( bytes != nExpected )
        return PMD_ERR_PortRead;

    /* byte swap return data */
    for( i=0, c=2; i<rCt; i++ )
    {
        rDat[i]  = (PMDuint16)((prxbuffer[c++])<<8);
        rDat[i] |= (PMDuint16) (prxbuffer[c++]);
    }

/*
    if (ProcessorError && CANtransport_data->bDiagnostics)
    {
        PMDprintf("Processor status: %s\r\n", PMDGetErrorMessage(ProcessorError));
        PMDprintf("C-Motion: %s ",PMDGetOpcodeText(xDat[0]));
        for(i=1; i<xCt; i++)
            PMDprintf(" TXdata%d %X ",i, xDat[i]);
        for(i=0; i<rCt; i++)
            PMDprintf(" RXdata%d %X ",i, rDat[i]);
        PMDprintf("\r\n");
    }
*/

    return (PMDresult)( ProcessorError );
}

//********************************************************
PMDresult PMDCAN_WaitForEvent(PMDDeviceHandle* hDevice, PMDEvent* event_data, PMDuint32 timeout)
{
PMDPeriphHandle* hPeriph = hDevice->hPeriph;
PMDuint16 event;
PMDAxis axis;
PMDparam nExpected = 4;
PMDparam nReceived;
PMDresult result;
char Data[8];

    if (hPeriph->transport.ReceiveEvent == NULL)
        return PMD_ERR_NotSupported;
    result = hPeriph->transport.ReceiveEvent( hPeriph, &Data, nExpected, &nReceived, timeout );
    if (result == PMD_ERR_OK && event_data != NULL)
    {
        axis = Data[1];
        event = Data[2] << 8;
        event |= Data[3];
        event_data->axis = axis;
        event_data->event = event;
    }
    return result;
}

//********************************************************
PMDresult PMDCAN_Init()
{
    xMutexCAN = PMDMutexCreate();

    return PMD_NOERROR;
}

