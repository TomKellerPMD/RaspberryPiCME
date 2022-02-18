//
//  PMDser.c -- Motion Processor serial protocol functions
//
//  Performance Motion Devices, Inc.
//

#include <stdio.h>
#include <stdlib.h>

#include "PMDtypes.h"
#include "PMDecode.h"
#include "PMDdevice.h"
#include "PMDperiph.h"
#include "PMDsys.h"
#include "PMDPfunc.h"

// only need to include this if diagnostics mode is used
#include "PMDdiag.h"

#define MP_TIMEOUT         100

#define MAXCPPACKETSIZE 20

static long baudList[] = { 1200, 2400, 9600, 19200, 57600, 115200, 230400, 460800 };
PMDMutexDefine(xMutexSerial)

//********************************************************
PMDresult PMDSerial_Sync(PMDPeriphHandle* hPeriph)
{
    const int maxSend = 4;
    int i;
    int bMultidrop = (((hPeriph->param) >> 8) == PMDSerialProtocolMultiDropUsingIdleLineDetection);
    char ch = 0;
    PMDparam bytes;
    PMDresult result;

    PMD_PERIPHCONNECTED(hPeriph);

    if( hPeriph->type != InterfaceSerial )
        return PMD_ERR_InvalidInterface;

    // sync is not required with multi-drop protocols because the command txbuffer resets
    // after idle time or address bit is received.
    if (bMultidrop)
        return PMD_ERR_InvalidOperation;

    // Flush the receive txbuffer in case any unexpected bytes have been received
    result = PMDPeriphFlush(hPeriph);
    if (result != PMD_ERR_OK)
      return result;

    for( i=0; i<maxSend; i++ )
    {
        // write a zero character
        result = PMDPeriphSend(hPeriph, &ch, 1, 0);

        // Attempt to read a response
        result = PMDPeriphReceive(hPeriph, &ch, &bytes, 1, 3);
        if (result == PMD_ERR_OK)
        {
            // a response is always 2 characters so absorb the next one
            result = PMDPeriphReceive(hPeriph, &ch, &bytes, 1, 3);
            break;
        }
    }

    /* If no data was seen, return an error */
    if( i==maxSend )
        return PMD_ERR_Timeout;

    return PMD_ERR_OK;
}

//********************************************************
PMDresult PMDSerial_Send(void* transport_data, PMDuint8 xCt, PMDuint16* xDat, PMDuint8 rCt, PMDuint16* rDat)
{
    PMDDeviceHandle* hDevice = (PMDDeviceHandle*)transport_data;
    PMDPeriphHandle* hPeriph = hDevice->hPeriph;
    PMDparam c=0;
    PMDparam i;
    int nExpected = 2*rCt+2;
    char sum;
    PMDparam  bytes;
    PMDparam  bytesdata;
    PMDuint8 txbuffer[MAXCPPACKETSIZE];
    PMDuint8 rxbuffer[MAXCPPACKETSIZE];
    PMDuint8* prxbuffer = &rxbuffer[0];
    PMDuint16 ProcessorError;
    int bMultidrop = ((hPeriph->param >> 8)  == PMDSerialProtocolMultiDropUsingIdleLineDetection);
	char address =  (char)(hPeriph->param & 0x1f);
    PMDresult result = PMD_ERR_OK;

    PMD_PERIPHCONNECTED(hPeriph);

    if( hPeriph->type != InterfaceSerial )
        return PMD_ERR_InvalidInterface;

    /* Clear address byte & checksum byte */
    txbuffer[ c++ ] = (char)(address);
    txbuffer[ c++ ] = (char)(0);            // checksum

    /* Add axis number and command code */
    txbuffer[ c++ ] = (char)(xDat[0]>>8);   // axis
    txbuffer[ c++ ] = (char)(xDat[0]&0xff); // cmd

    /* add data (handling byte swapping) */
    for( i=1; i<xCt; i++ )
    {
        txbuffer[ c++ ] = (char)(xDat[i] >> 8);
        txbuffer[ c++ ] = (char)(xDat[i] & 0xff);
    }

    /* calculate checksum */
    sum = 0;
    for( i=0; i<c; i++ ) sum += txbuffer[i];
    txbuffer[1] = -sum;

    if (!PMDMutexLock(xMutexSerial))
      return PMD_ERR_MutexTimeout;

    // Flush the receive buffer in case any unexpected bytes have been received
    result = PMDPeriphFlush(hPeriph);
    if (result == PMD_ERR_OK)
      result = PMDPeriphSend(hPeriph, txbuffer, c, MP_TIMEOUT);

    // first read the command status and checksum
    nExpected = 2;
    if (result == PMD_ERR_OK)
    {
        if (bMultidrop)
        {
            // idle line returns an extra byte containing the slave address
            nExpected++;
            result = PMDPeriphReceive(hPeriph, rxbuffer, &bytes, nExpected, MP_TIMEOUT);
            if (result == PMD_ERR_OK)
            {
                // compare the return address
                if(rxbuffer[0] != address)
                    result = PMD_ERR_PortRead;
            }
        }
        else
        {
            result = PMDPeriphReceive(hPeriph, rxbuffer, &bytes, nExpected, MP_TIMEOUT);
        }
    }
    if (result == PMD_ERR_OK)
    {
        if (bytes < 2)
            return PMD_ERR_InsufficientDataReceived;
        if (bMultidrop)
        {
            // skip address byte from head of packet
            prxbuffer++;
            bytes--;
        }

        // first byte in response is a motion processor error code
        ProcessorError = prxbuffer[0];
        // if there was an error and we received the expected #bytes,
        // don't attempt to receive any param data because it won't be coming
        if( ProcessorError && bytes==2 )
        {
            rCt = 0;
            // we will be out of sync if the command contains parameters
            if (!bMultidrop &&
            // and these error codes are returned
            (ProcessorError == PMD_ERR_MP_HardFault ||
             ProcessorError == PMD_ERR_MP_BadSerialChecksum ||
             ProcessorError == PMD_ERR_MP_InvalidInstruction ||
             ProcessorError == PMD_ERR_MP_InvalidAxis ||
             ProcessorError == PMD_ERR_MP_Reset))
                result = PMDSerial_Sync(hPeriph);

            PMDMutexUnlock(xMutexSerial);
            return (PMDresult)ProcessorError;
        }
        else
        {
            // read the data bytes
            nExpected = 2*rCt;
            prxbuffer += bytes;
            result = PMDPeriphReceive(hPeriph, prxbuffer, &bytesdata, nExpected, MP_TIMEOUT);
            bytes += bytesdata;
        }
    }

    PMDMutexUnlock(xMutexSerial);

    if (result != PMD_ERR_OK)
      return result;

    // verify the checksum on the data
    if(bMultidrop)
      bytes++;  // MultiDrop response has extra byte

    sum = 0;
    for( i=0; i<bytes; i++ )
        sum += rxbuffer[i];

    if( sum )
        return PMD_ERR_Checksum;

    // byte swap return data
    for( i=0, c=0; i<rCt; i++ )
    {
        rDat[i]  = (PMDuint16)((*prxbuffer++)<<8);
        rDat[i] |= (PMDuint16)(*prxbuffer++);
    }

/*
    if (ProcessorError && bDiagnostics)
    {
        PMDprintf("Command error: %s\r\n", PMDGetErrorMessage(ProcessorError));
        PMDprintf("C-Motion: %s ",PMDGetOpcodeText(xDat[0]));
        PMDprintf(" TXdata%d",i);
        for(i=1; i<xCt; i++)
            PMDprintf(" %02X ",xDat[i]);
        PMDprintf(" RXdata%d",i);
        for(i=0; i<rCt; i++)
            PMDprintf(" %02X ",rDat[i]);
        PMDprintf("\r\n");
    }
*/

    return (PMDresult)ProcessorError;
}

//********************************************************
PMDresult PMDSerial_SetConfig(void* transport_data, PMDuint16 serialmode)
{
    PMDDeviceHandle* hDevice = (PMDDeviceHandle*)transport_data;
    PMDPeriphHandle* hPeriph = hDevice->hPeriph;
    PMDparam baudindex;
    PMDparam parity;
    PMDparam stopbits;
    PMDparam protocol;
    PMDparam address;

    if (serialmode > 0xF9E7)
      return PMD_ERR_ParameterOutOfRange;

    GET_SERIALPORTMODE(serialmode, baudindex, parity, stopbits, protocol, address)
    PMDPCOM_SetConfig(hPeriph, baudList[baudindex], (PMDSerialParity) parity, (PMDSerialStopBits) stopbits);
    hPeriph->param = address;
    hPeriph->param |= (protocol << 8);

    return PMD_NOERROR;
}

//********************************************************
PMDresult PMDSerial_Init()
{
    xMutexSerial = PMDMutexCreate();

    return PMD_NOERROR;
}

