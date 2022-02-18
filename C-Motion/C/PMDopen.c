//
//  PMDopen.c -- functions for opening PMD handles
//
//  Performance Motion Devices, Inc.
//

/*
  The PMDP?_Open() functions open the local peripheral
  The PMDRP?_Open() functions open the peripheral on a PMD C-Motion Engine (CME) device.
*/

#include "PMDperiph.h"
#include "PMDRPdevice.h"
#include "PMDPfunc.h"
#include "PMDsys.h"

#define PMD_VERIFYDEVICEHANDLE(handle)  \
    if( handle == NULL ) return PMD_ERR_InvalidHandle; \
    if (handle->type == PMDDeviceTypeNone) return PMD_ERR_InterfaceNotInitialized;



PMDCFunc PMDPeriphOpenCOM(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam portnum, PMDparam baud, PMDSerialParity parity, PMDSerialStopBits stopbits)
{
    PMDZeroMemory(hPeriph, sizeof(PMDPeriphHandle));
    if (hDevice == NULL)
        return PMDPCOM_Open(hPeriph, portnum, baud, parity, stopbits);
    else
        return PMDRPCOM_Open(hPeriph, hDevice, portnum, baud, parity, stopbits);
}

PMDCFunc PMDPeriphOpenSPI(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDuint8 port, PMDuint8 chipselect, PMDuint8 mode, PMDuint8 datasize, PMDparam bitrate)
{
    PMDZeroMemory(hPeriph, sizeof(PMDPeriphHandle));
    if (hDevice == NULL)
#ifdef USE_SPI_INTERFACE
        return PMDPSPI_Open(hPeriph, port, chipselect, mode, datasize, bitrate);
#else
		return PMD_ERR_NotSupported;
#endif
    else
        return PMDRPSPI_Open(hPeriph, hDevice, port, chipselect, mode, datasize, bitrate);
}

PMDCFunc PMDPeriphOpenTCP(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam ipaddress, PMDparam portnum, PMDparam timeout)
{
    PMDZeroMemory(hPeriph, sizeof(PMDPeriphHandle));
    if (hDevice == NULL)
        return PMDPTCP_Open(hPeriph, ipaddress, portnum, timeout);
    else
        return PMDRPTCP_Open(hPeriph, hDevice, ipaddress, portnum);
}

PMDCFunc PMDPeriphOpenUDP(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam ipaddress, PMDparam portnum)
{
    PMDZeroMemory(hPeriph, sizeof(PMDPeriphHandle));
    if (hDevice == NULL)
        return PMDPUDP_Open(hPeriph, ipaddress, portnum);
    else
        return PMDRPUDP_Open(hPeriph, hDevice, ipaddress, portnum);
}

PMDCFunc PMDPeriphOpenCAN(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam transmitid, PMDparam receiveid, PMDparam eventid)
{
    PMDZeroMemory(hPeriph, sizeof(PMDPeriphHandle));
    if (hDevice == NULL)
#ifdef USE_CAN_INTERFACE
		return PMDPCAN_Open(hPeriph, PMDCANPort1, PMDCANBaud1000000, transmitid, receiveid, eventid);
#else
		return PMD_ERR_NotSupported;
#endif
    else
        return PMDRPCAN_Open(hPeriph, hDevice, PMDCANPort1, PMDCANBaud1000000, transmitid, receiveid, eventid);
}

PMDCFunc PMDPeriphOpenCANFD(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDCANPort port, PMDCANBaud baud, PMDparam transmitid, PMDparam receiveid, PMDparam addressmode)
{
    PMDZeroMemory(hPeriph, sizeof(PMDPeriphHandle));
    if (hDevice == NULL)
#ifdef USE_CAN_INTERFACE
		return PMDPCAN_Open(hPeriph, port, baud, transmitid, receiveid, addressmode);
#else
		return PMD_ERR_NotSupported;
#endif
    else
        return PMDRPCAN_Open(hPeriph, hDevice, port, baud, transmitid, receiveid, addressmode);
}

PMDCFunc PMDPeriphOpenPIO(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDuint16 address, PMDuint8 eventIRQ, PMDDataSize datasize)
{
    PMDZeroMemory(hPeriph, sizeof(PMDPeriphHandle));
    if (hDevice == NULL)
        return PMD_ERR_NotSupported;
    else
        return PMDRPPAR_Open(hPeriph, hDevice, address, eventIRQ, datasize);
}

PMDCFunc PMDPeriphOpenISA(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDuint16 address, PMDuint8 eventIRQ, PMDDataSize datasize)
{
    PMDZeroMemory(hPeriph, sizeof(PMDPeriphHandle));
    if (hDevice == NULL)
        return PMD_ERR_NotSupported;
    else
        return PMDRPISA_Open(hPeriph, hDevice, address, eventIRQ, datasize);
}

PMDCFunc PMDPeriphOpenPCI(PMDPeriphHandle* hPeriph, PMDparam boardnum)
{
#ifdef USE_PCI_INTERFACE
    PMDZeroMemory(hPeriph, sizeof(PMDPeriphHandle));
    return PMDPPCI_Open(hPeriph, boardnum);
#else
    return PMD_ERR_NotSupported;
#endif
}

//********************************************************
// Use this function to share an existing handle to an open port
// and set the multi-drop address and axis number
// only close the hPeriphParent when done. Do not close the hPeriph handle.
PMDCFunc PMDPeriphOpenMultiDrop(PMDPeriphHandle* hPeriph, PMDPeriphHandle* hPeriphParent, PMDparam nodeID)
{
    PMD_PERIPHCONNECTED(hPeriphParent)

    if( hPeriphParent->type != InterfaceSerial 
    &&  hPeriphParent->type != InterfaceCAN )
        return PMD_ERR_InvalidInterface;

    // copy the handle
    memcpy(hPeriph, hPeriphParent, sizeof( PMDPeriphHandle ) );

#ifdef USE_CAN_INTERFACE
    if (hPeriphParent->type == InterfaceCAN)
        return PMDPCAN_AddNodeID(hPeriph, hPeriphParent, (PMDuint8)nodeID);
	else
#endif
	{
		if (nodeID > 0x1F) // max serial port nodeID is 31 
		  return PMD_ERR_InvalidParameter;

		// store the multidrop information in the general purpose param field.
		hPeriph->param = nodeID;
		hPeriph->param |= PMDSerialProtocolMultiDropUsingIdleLineDetection << 8;
	}

    return PMD_ERR_OK;
}

//********************************************************
PMDCFunc PMDAxisOpen(PMDAxisHandle *hAxis, PMDDeviceHandle *hDevice, PMDAxis axis_number)
{
    PMD_VERIFYDEVICEHANDLE(hDevice)
    // set the axis we are talking to with this handle
    PMDZeroMemory(hAxis, sizeof(PMDAxisHandle));
    hAxis->axis = axis_number;
    hAxis->transport_data = hDevice;
    hAxis->transport.SendCommand = hDevice->transport.SendCommand;

    return PMD_ERR_OK;
}

//********************************************************
PMDCFunc PMDMemoryOpen(PMDMemoryHandle *hMemory, PMDDeviceHandle *hDevice, PMDDataSize datasize, PMDMemoryType memorytype)
{
    PMD_VERIFYDEVICEHANDLE(hDevice)
    if (datasize == PMDDataSize_8Bit)
        return PMD_ERR_NotSupported;

    PMDZeroMemory(hMemory, sizeof(PMDMemoryHandle));
    // if the device handle contains valid Memory functions then this must be an MP device
    // otherwise it is memory on a remote PRP device (Prodigy/CME)
    if (hDevice->transport.ReadMemory && hDevice->transport.WriteMemory && datasize == PMDDataSize_32Bit)
    {
        hMemory->transport.Read  = hDevice->transport.ReadMemory;
        hMemory->transport.Write = hDevice->transport.WriteMemory;
        hMemory->hDevice = hDevice;
        hMemory->datasize = datasize;	// todo: datasize passed in may not be compatible with default ReadMemory datasize
    }
    else
    {
        return PMDRPMemoryOpen(hMemory, hDevice, datasize, memorytype);
    }

    return PMD_ERR_OK;
}

//********************************************************
PMDCFunc PMDMailboxOpen(PMDMailboxHandle* hMailbox, PMDDeviceHandle *hDevice, PMDparam mailboxID, PMDparam depth, PMDparam itemsize)
{
    PMDZeroMemory(hMailbox, sizeof(PMDMailboxHandle));
    if (hDevice == NULL)
        return PMD_ERR_NotSupported;
    else
        return PMDRPMailboxOpen(hMailbox, hDevice, mailboxID, depth, itemsize);
}

//*******************************************************
// Return an error, rather than faulting when calling an
// MP device opened with an unsupported peripheral type.
PMDresult PMDERR_Send(void* transport_data, PMDuint8 xCt, PMDuint16* xDat, PMDuint8 rCt, PMDuint16* rDat) 
{
    return PMD_ERR_NotSupported;
}

//********************************************************
// Open a motion processor protocol device
PMDCFunc PMDMPDeviceOpen(PMDDeviceHandle *hDevice, PMDPeriphHandle *hPeriph)
{
    PMDresult result = PMD_ERR_OK;
    PMDZeroMemory(hDevice, sizeof(PMDDeviceHandle));
    PMD_PERIPHCONNECTED(hPeriph)
    hDevice->hPeriph = hPeriph;
    hDevice->type = PMDDeviceTypeMotionProcessor;

    // address is > 0 when hPeriph is a CME (RP) connected peripheral
    if (hPeriph->address > 0)
        return PMDMPDeviceOpenRemote(hDevice, hPeriph);

    // set the appropriate Send function for the interface type
    switch (hPeriph->type)
    {
    case InterfaceSerial:
        hDevice->transport.SendCommand = PMDSerial_Send;
        result = PMDSerial_Init();
        break;
#ifdef USE_SPI_INTERFACE
    case InterfaceSPI:
        hDevice->transport.SendCommand = PMDSPI_Send;
        result = PMDSPI_Init();
        break;
#endif
#ifdef USE_CAN_INTERFACE
    case InterfaceCAN:
        hDevice->transport.SendCommand = PMDCAN_Send;
        result = PMDCAN_Init();
        break;
#endif
#ifdef USE_PCI_INTERFACE
    case InterfacePCI:
        hDevice->transport.ReadMemory  = PMDPCI_ReadMemory;
        hDevice->transport.WriteMemory = PMDPCI_WriteMemory;
        hDevice->transport.SendCommand = PMDPAR_Send;
        result = PMDPAR_Init();
        break;
#endif
#if 0  // enable this if connecting to a Prodigy card from a host via a PC104 Prodigy/CME card.
    case InterfaceISA:
        //hDevice->transport.ReadMemory  = PMDISA_ReadMemory;
        //hDevice->transport.WriteMemory = PMDISA_WriteMemory;
        hDevice->transport.SendCommand = PMDPAR_Send;
        result = PMDPAR_Init();
        break;
    case InterfaceParallel:
        hDevice->transport.SendCommand = PMDPAR_Send;
        result = PMDPAR_Init();
        break;
#endif
    default:
        hDevice->transport.SendCommand = PMDERR_Send;
        return PMD_ERR_InvalidPort;
    }

    return result;
}

//PMDresult PMDEventOpen(PMDEventHandle* hEvent, PMDEventType type, PMDparam var1, PMDparam var2, PMDparam var3)
//{
 //   return PMD_ERR_NotSupported;
//}/

