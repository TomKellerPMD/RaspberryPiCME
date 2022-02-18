//
//  PMDPcan.c -- IXXAT CAN interface peripheral functions
//
//  Performance Motion Devices, Inc.
//

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "PMDtypes.h"
#include "PMDecode.h"
#include "PMDperiph.h"
#include "PMDIXXATCan.h"
#include "PMDPfunc.h"

// only need to include this if diagnostics mode is used
#include "PMDdiag.h"


//********************************************************
static PMDresult PMDPCAN_Close(PMDPeriphHandle* hPeriph)
{
    PMD_PERIPHCONNECTED(hPeriph)
    PMDCAN_Close(hPeriph->transport_data);
    hPeriph->handle = PMD_INVALID_HANDLE;

    return PMD_ERR_OK;
}


//********************************************************
static PMDresult PMDPCAN_Send(PMDPeriphHandle* hPeriph, const void *data, PMDparam nCount, PMDparam timeoutms)
{
    PMD_PERIPHCONNECTED(hPeriph)

    return TransmitData((PMDCANIOTransportData*)hPeriph->transport_data, (char*)data, nCount, timeoutms);
}

//********************************************************
static PMDresult PMDPCAN_Receive(PMDPeriphHandle* hPeriph, void *data, PMDparam nCount, PMDparam *pnreceived, PMDparam timeoutms)
{
    *pnreceived = 0;
    PMD_PERIPHCONNECTED(hPeriph)

    return ReceiveData((PMDCANIOTransportData*)hPeriph->transport_data, (char*)data, (int*)pnreceived, nCount, timeoutms);
}

//********************************************************
static void PMDPCAN_Init(PMDPeriphHandle* hPeriph)
{
    // set the interface type 
    hPeriph->type = InterfaceCAN;

    hPeriph->transport.Close    = PMDPCAN_Close;
    hPeriph->transport.Send     = PMDPCAN_Send;
    hPeriph->transport.Receive  = PMDPCAN_Receive;
    hPeriph->transport.Read     = NULL;
    hPeriph->transport.Write    = NULL;
}

//********************************************************
PMDresult PMDPCAN_AddNodeID(PMDPeriphHandle* hPeriph, PMDPeriphHandle* hPeriphParent, PMDuint8 nodeID)
{
    PMDCANIOTransportData* pCANIOTransportData;
    pCANIOTransportData = (PMDCANIOTransportData*) malloc( sizeof( PMDCANIOTransportData ) );
    hPeriph->transport_data = pCANIOTransportData;
    memcpy(hPeriph->transport_data, hPeriphParent->transport_data, sizeof( PMDCANIOTransportData ) );

    return PMDCAN_SetNodeID((PMDCANIOTransportData*)hPeriph->transport_data, nodeID);
}

//********************************************************
//Set portnum to COMn port number (1 = COM1, 2 = COM2, etc.)
//********************************************************
PMDresult PMDPCAN_Open(PMDPeriphHandle* hPeriph, PMDCANPort port, PMDparam baud, PMDparam addressTX, PMDparam addressRX, PMDparam addressEvent)
{
    PMDresult result;

    PMDCANIOTransportData* pCANIOTransportData;
    pCANIOTransportData = (PMDCANIOTransportData*) malloc( sizeof( PMDCANIOTransportData ) );
    memset(pCANIOTransportData, 0, sizeof(PMDCANIOTransportData));

    pCANIOTransportData->baudrate = baud;

    pCANIOTransportData->txID = addressTX;
    pCANIOTransportData->rxMask = addressRX;
    pCANIOTransportData->rxIntMask = addressEvent;

    hPeriph->transport_data = pCANIOTransportData;
    hPeriph->param = (baud << 8); // baud is used by the driver to determine if we're in FD mode.

    PMDPCAN_Init(hPeriph);

    result = PMDCAN_InitPort((PMDCANIOTransportData*)hPeriph->transport_data);
    if (result == PMD_ERR_OK)
        hPeriph->handle = PMD_CONNECTED_HANDLE;

    return result;
}