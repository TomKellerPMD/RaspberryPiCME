//  PMDIXXATCAN3.c -- CAN interface command/data transfer functions. The implementations
//  are for the IXXAT VCI (Virtual Can Interface) v3.x API.
//
//  Performance Motion Devices, Inc.
//

#include <stdio.h>

//C-Motion includes
#include "PMDtypes.h"
#include "PMDecode.h"
// only include this if we are running in diagnostics mode
#include "PMDdiag.h"
#include "vcinpl.h" 
#include "PMDIXXATCan.h"

#define Printf printf 


//*****************************************************************************
void DisplayError(HRESULT hResult)
{
    char szError[VCI_MAX_ERRSTRLEN];

    if (hResult != VCI_SUCCESS)
    {
        if (hResult == -1)
            hResult = GetLastError();

        szError[0] = 0;
        vciFormatError(hResult, szError, sizeof(szError));
        Printf(szError);
        Printf("\n");
    }
}

//*****************************************************************************
PMDresult TransmitData(PMDCANIOTransportData* CANtransport_data, char *data, int nbytes, int timeout)
{
    PMDresult result;
    HRESULT hResult;
    CANMSG  sCanMsg;
    UINT8   i;

    ZeroMemory(&sCanMsg, sizeof(CANMSG));

    sCanMsg.dwTime   = 0;
    sCanMsg.dwMsgId  = CANtransport_data->txID;    // CAN message identifier

    sCanMsg.uMsgInfo.Bytes.bType    = CAN_MSGTYPE_DATA;
    sCanMsg.uMsgInfo.Bytes.bFlags = CAN_MAKE_MSGFLAGS(8,0,0,0,0);
    sCanMsg.uMsgInfo.Bits.dlc = nbytes;

    for (i = 0; i < nbytes; i++ )
    {
        sCanMsg.abData[i] = data[i];
    }

    // write the CAN message into the transmit FIFO
    hResult = canChannelSendMessage(CANtransport_data->hCanChn, timeout, &sCanMsg);

    DisplayError(hResult);
    if (hResult == VCI_SUCCESS)
        result = PMD_ERR_OK;
    else if (hResult == VCI_E_TIMEOUT)
        result = PMD_ERR_Timeout;
    else
        result = PMD_ERR_CommunicationsError; 

    return result;
}

//*****************************************************************************
PMDresult ReceiveData( PMDCANIOTransportData* CANtransport_data, char *data, int* nBytesReceieved, int nBytesExpected, int timeout)
{
    PMDresult result;
    HRESULT hResult;
    CANMSG  sCanMsg;
    UINT8  bType = CAN_MSGTYPE_DATA;

    *nBytesReceieved = 0;
    
    do
    {
        // read a CAN message from the receive FIFO
        hResult = canChannelReadMessage(CANtransport_data->hCanChn, timeout, &sCanMsg);

        if (hResult == VCI_SUCCESS)
        {
            bType = sCanMsg.uMsgInfo.Bytes.bType;
            if (sCanMsg.uMsgInfo.Bytes.bType == CAN_MSGTYPE_DATA)
            {
                //
                // show data frames
                //
                if (sCanMsg.uMsgInfo.Bits.rtr == 0)
                {
                    UINT8 j;
/*
                    Printf("\nTime: %10u  ID: %3X  DLC: %1u  Data:",
                                    sCanMsg.dwTime,
                                    sCanMsg.dwMsgId,
                                    sCanMsg.uMsgInfo.Bits.dlc);
*/
                    // copy data bytes
                    *nBytesReceieved = sCanMsg.uMsgInfo.Bits.dlc;
                    for (j = 0; j < sCanMsg.uMsgInfo.Bits.dlc; j++)
                    {
//                      Printf(" %.2X", sCanMsg.abData[j]);
                        data[j] = sCanMsg.abData[j];
                    }
                }
/*
                else
                {
                    Printf("\nTime: %10u ID: %3X  DLC: %1u  Remote Frame",
                                 sCanMsg.dwTime,
                                 sCanMsg.dwMsgId,
                                 sCanMsg.uMsgInfo.Bits.dlc);
                }
*/
            }
            else if (sCanMsg.uMsgInfo.Bytes.bType == CAN_MSGTYPE_INFO)
            {
                //
                // show informational frames
                //
                switch (sCanMsg.abData[0])
                {
                    case CAN_INFO_START: Printf("CAN started..."); break;
                    case CAN_INFO_STOP : Printf("CAN stopped...");  break;
                    case CAN_INFO_RESET: Printf("CAN reset..."); break;
                }
                Printf("\r\n");
            }
            else if (sCanMsg.uMsgInfo.Bytes.bType == CAN_MSGTYPE_ERROR)
            {
                //
                // show error frames
                //
                switch (sCanMsg.abData[0])
                {
                    case CAN_ERROR_STUFF: Printf("stuff error...");                  break; 
                    case CAN_ERROR_FORM : Printf("form error...");                   break; 
                    case CAN_ERROR_ACK  : Printf("acknowledgement error...");        break;
                    case CAN_ERROR_BIT  : Printf("bit error...");                    break; 
                    case CAN_ERROR_CRC  : Printf("CRC error...");                    break; 
                    case CAN_ERROR_OTHER:
                    default             : Printf("other error...");                  break;
                }
                Printf("\r\n");
            }
        }
    }
    while (hResult == VCI_SUCCESS && bType != CAN_MSGTYPE_DATA);

	if (hResult != VCI_E_RXQUEUE_EMPTY)
		DisplayError(hResult);
    if (hResult == VCI_SUCCESS)
        result = PMD_ERR_OK;
    else if (hResult == VCI_E_TIMEOUT || hResult == VCI_E_RXQUEUE_EMPTY)
        result = PMD_ERR_Timeout;
    else
        result = PMD_ERR_CommunicationsError; 

    return result;
}


/*****************************************************************************
 Function:
    SelectDevice

 Description:
    Selects the first CAN adapter.

 Arguments:
    fUserSelect -> If this parameter is set to TRUE the functions display
                   a dialog box which allows the user to select the device.

 Results:
    none
*****************************************************************************/
HRESULT SelectDevice( PMDCANIOTransportData* CANtransport_data, BOOL fUserSelect )
{
    HRESULT hResult; // error code
    int count = 0;
    HANDLE hDevice = NULL;

    if (fUserSelect == FALSE)
    {
        HANDLE hEnum;   // enumerator handle
        VCIDEVICEINFO sInfo;     // device info

        //
        // open the device list
        //
        hResult = vciEnumDeviceOpen(&hEnum);

        //
        // retrieve information about the first
        // device within the device list
        //
        while (hResult == VCI_SUCCESS)
        {
            hResult = vciEnumDeviceNext(hEnum, &sInfo);
            if (hResult == VCI_SUCCESS)
                count++;
        }

        //
        // close the device list (no longer needed)
        //
        vciEnumDeviceClose(hEnum);

        if (hResult == VCI_E_NO_MORE_ITEMS)
        {
            // if more than one device is found than display the selection dialog
            if (count > 1)
                hResult = vciDeviceOpenDlg(NULL, &hDevice);
            // otherwise just open the one device
            else 
                hResult = vciDeviceOpen(&sInfo.VciObjectId, &hDevice);
        }
    }
    else
    {
        //
        // open a device selected by the user
        //
        hResult = vciDeviceOpenDlg(NULL, &hDevice);
    }

	CANtransport_data->hDevice = hDevice;

    DisplayError(hResult);

    return hResult;
}

/*************************************************************************
 Function: 
    InitSocket

 Description : 
    Opens the specified socket, creates a message channel, initializes
    and starts the CAN controller.

 Arguments: 
    dwCanNo -> Number of the CAN controller to open.

 Results:
    none

 Remarks:
    If <dwCanNo> is set to 0xFFFFFFFF, the function shows a dialog box
    which allows the user to select the VCI device and CAN controller.
*************************************************************************/
HRESULT InitSocket(PMDCANIOTransportData* CANtransport_data, UINT32 dwCanNo)
{
    HRESULT hResult;

    //
    // create a message channel with exclusive use of CAN connection
    //
    hResult = canChannelOpen(CANtransport_data->hDevice, dwCanNo, TRUE, &CANtransport_data->hCanChn);

    //
    // initialize the message channel
    //
    if (hResult == VCI_SUCCESS)
    {
        UINT16 wRxFifoSize  = 100;
        UINT16 wRxThreshold = 1;
        UINT16 wTxFifoSize  = 10;
        UINT16 wTxThreshold = 1;

        hResult = canChannelInitialize( CANtransport_data->hCanChn, wRxFifoSize, wRxThreshold, wTxFifoSize, wTxThreshold);
    }

    //
    // activate the CAN channel
    //
    if (hResult == VCI_SUCCESS)
    {
        hResult = canChannelActivate(CANtransport_data->hCanChn, TRUE);
    }

    //
    // open the CAN controller
    //
    if (hResult == VCI_SUCCESS)
    {
        hResult = canControlOpen(CANtransport_data->hDevice, dwCanNo, &CANtransport_data->hCanCtl);
        // this function fails if the controller is in use
        // by another application.
    }

    DisplayError(hResult);

    return hResult;
}

//*****************************************************************************
HRESULT InitController(PMDCANIOTransportData* CANtransport_data)
{
    HRESULT hResult;
    int nBaud;
    HANDLE hCanCtl = CANtransport_data->hCanCtl;

    //
    // initialize the CAN controller
    //
    UINT8 bt0;
    UINT8 bt1;
    //default
    bt0=0x00;
    bt1=0x14; 
    nBaud = CANtransport_data->baudrate;

    if(nBaud==7) {bt0=0x31; bt1=0x1C;}  //    10,000
    if(nBaud==6) {bt0=0x18; bt1=0x1C;}  //    20,000
    if(nBaud==5) {bt0=0x09; bt1=0x1C;}  //    50,000
    if(nBaud==4) {bt0=0x03; bt1=0x1C;}  //   125,000
    if(nBaud==3) {bt0=0x01; bt1=0x1C;}  //   250,000
    if(nBaud==2) {bt0=0x00; bt1=0x1C;}  //   500,000
    if(nBaud==1) {bt0=0x00; bt1=0x16;}  //   800,000
    if(nBaud==0) {bt0=0x00; bt1=0x14;}  // 1,000,000

    hResult = canControlReset(hCanCtl);

    if (hResult == VCI_SUCCESS)
        hResult = canControlInitialize(hCanCtl, CAN_OPMODE_STANDARD, bt0, bt1);

    if (hResult == VCI_SUCCESS)
    { 
        BOOL b29bit = FALSE;
        hResult = canControlSetAccFilter( hCanCtl, b29bit, CAN_ACC_CODE_NONE, CAN_ACC_MASK_NONE);
    }

    //
    // start the CAN controller
    //
    if (hResult == VCI_SUCCESS)
    {
        hResult = canControlStart(hCanCtl, TRUE);
    }

    DisplayError(hResult);

    return hResult;
}

//*****************************************************************************
HRESULT AddFilter(PMDCANIOTransportData* CANtransport_data)
{
    HRESULT hResult;
    HANDLE hCanCtl = CANtransport_data->hCanCtl;

    //
    // stop the CAN controller
    //
    hResult = canControlStart(hCanCtl, FALSE);

    //
    // add an acceptance filter
    //
    if (hResult == VCI_SUCCESS)
    { 
        UINT32 bRTR = 0;
        BOOL b29bit = FALSE;
        // RTR bit is LSB so shift everything left by 1
        UINT32 nAcceptanceCode = ((CANtransport_data->rxMask) << 1) + bRTR;
        UINT32 nAcceptanceMask = CAN_ACC_MASK_NONE;
        hResult = canControlAddFilterIds( hCanCtl, b29bit, nAcceptanceCode, nAcceptanceMask);
    }

    //
    // start the CAN controller
    //
    if (hResult == VCI_SUCCESS)
    {
        hResult = canControlStart(hCanCtl, TRUE);
    }

    DisplayError(hResult);

    return hResult;
}

//*****************************************************************************
HRESULT RemoveFilter(PMDCANIOTransportData* CANtransport_data)
{
    HRESULT hResult;
    HANDLE hCanCtl = CANtransport_data->hCanCtl;

    //
    // stop the CAN controller
    //
    hResult = canControlStart(hCanCtl, FALSE);

    //
    // add an acceptance filter
    //
    if (hResult == VCI_SUCCESS)
    { 
        UINT32 bRTR = 0;
        BOOL b29bit = FALSE;
        // RTR bit is LSB so shift everything left by 1
        UINT32 nAcceptanceCode = ((CANtransport_data->rxMask) << 1) + bRTR;
        UINT32 nAcceptanceMask = CAN_ACC_MASK_NONE;
        hResult = canControlRemFilterIds( hCanCtl, b29bit, nAcceptanceCode, nAcceptanceMask);
    }

    //
    // start the CAN controller
    //
    if (hResult == VCI_SUCCESS)
    {
        hResult = canControlStart(hCanCtl, TRUE);
    }

    DisplayError(hResult);

    return hResult;
}

//*****************************************************************************
PMDresult PMDCAN_FlushBuffers(PMDCANIOTransportData* CANtransport_data)
{
    HRESULT hResult;
    HANDLE hCanCtl = CANtransport_data->hCanCtl;
    PMDuint8 rbuff[10];
    int     rbytes;

	do 
	{
	    hResult = ReceiveData(CANtransport_data, rbuff, &rbytes, 8, 0);
	} 
	while (hResult == VCI_SUCCESS); //VCI_E_RXQUEUE_EMPTY | VCI_E_TIMEOUT);


	return PMD_ERR_OK;
}

//*****************************************************************************
PMDresult Open( PMDCANIOTransportData* CANtransport_data )
{
    DWORD dwCanNo = 0; // CAN card number
    UINT32 dwMajorVersion;
    UINT32 dwMinorVersion;
    HRESULT hResult;

    hResult = vciGetVersion(&dwMajorVersion, &dwMinorVersion);
    if (VCI_SUCCESS == hResult)
    {
        Printf("IXXAT driver version: %ld.%ld\r\n", dwMajorVersion, dwMinorVersion );

        hResult = SelectDevice(CANtransport_data, FALSE);
        if (VCI_SUCCESS == hResult)
        {
            hResult = InitSocket(CANtransport_data, dwCanNo);
            if (VCI_SUCCESS == hResult)
            {
                hResult = InitController(CANtransport_data);
                if (VCI_SUCCESS == hResult)
                {
                    hResult = AddFilter(CANtransport_data);
                    if (VCI_SUCCESS == hResult)
                    {
                        VCIDEVICEINFO info;
                        vciDeviceGetInfo( CANtransport_data->hDevice, &info);
                        //FlushReceive();
                        return PMD_ERR_OK;
                    }
                }
            }
        }
    }
    if (VCI_SUCCESS != hResult)
    {
        DisplayError(hResult);
    }

    return PMD_ERR_OpeningPort;
}


// ------------------------------------------------------------------------
PMDresult PMDCAN_Close(void* transport_data)
{
    PMDCANIOTransportData* CANtransport_data = (PMDCANIOTransportData*)transport_data;

    if (CANtransport_data != NULL)
    {
        HRESULT hResult;
        RemoveFilter(CANtransport_data); 

        // close all open handles
        // if hDevice is not NULL it means this is the parent handle so we can close it 
        if (CANtransport_data->hDevice)
        {
            if (CANtransport_data->hCanChn)
                hResult = canChannelClose(CANtransport_data->hCanChn);
            if (CANtransport_data->hCanCtl)
                hResult = canControlClose(CANtransport_data->hCanCtl);
            if (CANtransport_data->hDevice)
                hResult = vciDeviceClose(CANtransport_data->hDevice);
        }
        free(CANtransport_data);
    }
    return PMD_ERR_OK;
}


// ------------------------------------------------------------------------
// PMDCAN_InitPort
// Initializes CAN hardware and interface.
PMDresult PMDCAN_InitPort(PMDCANIOTransportData* CANtransport_data)
{
    return Open( CANtransport_data );
}

// ------------------------------------------------------------------------
// set the nodeID for this handle
PMDresult PMDCAN_SetNodeID(PMDCANIOTransportData* CANtransport_data, PMDuint8 nodeID)
{
    CANtransport_data->txID = CAN_ADDRESS_BASE_TX + nodeID;
    CANtransport_data->rxMask = CAN_ADDRESS_BASE_RX + nodeID;
    CANtransport_data->rxIntMask = CAN_ADDRESS_BASE_INTR + nodeID;
    // clear the device handles to prevent them from being closed when closing this shared handle.
    // they will be closed when the parent is closed.
    CANtransport_data->hDevice = NULL;

    AddFilter(CANtransport_data); // add the new nodeID to the receive filter

    return PMD_ERR_OK;
}

// ------------------------------------------------------------------------
// change the baud
PMDresult PMDCAN_SetBaud(PMDCANIOTransportData* CANtransport_data, PMDuint8 baud)
{
    CANtransport_data->baudrate = baud;

    InitController(CANtransport_data);
    AddFilter(CANtransport_data); // add the new nodeID to the receive filter
	PMDCAN_FlushBuffers(CANtransport_data);

    return PMD_ERR_OK;
}
