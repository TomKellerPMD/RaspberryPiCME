#ifndef _DEF_INC_PMDperiph
#define _DEF_INC_PMDperiph

//
//  "Base Class" definitions for PeriphTransport
//
//  Performance Motion Devices, Inc.
//

#include "PMDtypes.h"
#include "PMDdevice.h"

#define PMD_INVALID_HANDLE      (void*)(0)
#define PMD_CONNECTED_HANDLE    (void*)(1)
#define PMD_WAITFOREVER         (PMDparam)0xFFFFFFFFUL

#define PMD_CANID_MODE_MASK     (PMDparam)0x80000001UL


#define PMD_VERIFYHANDLE(handle)        if( handle == NULL ) return PMD_ERR_InvalidHandle;
#define PMD_PERIPHCONNECTED(hPeriph)    if( hPeriph == NULL ) return PMD_ERR_InterfaceNotInitialized; \
                                        if( hPeriph->handle == PMD_INVALID_HANDLE ) return PMD_ERR_NotConnected;
// zero the handle structure so that a call to a function with an uninitialized handle won't cause problems.
#define PMD_INITIALIZEHANDLE(handle)    memset(&handle, 0, sizeof(handle));

#define PMD_IP4_ADDR(a,b,c,d)  (((PMDuint32)((a) & 0xff) << 24) | \
                                ((PMDuint32)((b) & 0xff) << 16) | \
                                ((PMDuint32)((c) & 0xff) << 8)  | \
                                 (PMDuint32)((d) & 0xff) << 0)

#if defined(__cplusplus)
extern "C" {
#endif

// open C-Motion Engine User Packet legacy 'peripheral'
PMDCFunc PMDPeriphOpenCME         (PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice);

/*
  PMDPeriphOpenPRP

  Open a handle to a PRP peripheral channel that can send and receive user data via a PRP connection.

  channel      The channel to send and receive data to and from user code. 
               Channel #0 is reserved for the legacy CME peripheral opened via PMDPeriphOpenCME.
               Channel #1 is reserved for the console IO.
*/ 
PMDCFunc PMDPeriphOpenPRP         (PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam channel, PMDparam bufsize);
PMDCFunc PMDPeriphOpenCOM         (PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam port, PMDparam baud, PMDSerialParity parity, PMDSerialStopBits stopbits);
PMDCFunc PMDPeriphOpenSPI         (PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDuint8 port, PMDuint8 chipselect, PMDuint8 mode, PMDuint8 datasize, PMDparam bitrate);

/*
  PMDPeriphOpenCAN

  Open a handle to the Expansion CAN port at the default or current baud rate. This is a legacy function normally used to communicate to Magellan motion ICs.

  addressTX     The address to send data to. 0-7FF                          (0x600 + nodeID)
  addressRX     The address to accept data from. 0-7FF                      (0x580 + nodeID)
  addressEvent  The address to accept asynchronous event data from. 0-7FF   (0x180 + nodeID)
*/
PMDCFunc PMDPeriphOpenCAN         (PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam addressTX, PMDparam addressRX, PMDparam eventid);

/*
  PMDPeriphOpenCANFD

  Open a handle to the Expansion CAN or HostCAN port at the specified baud rate. 

  baud          The desired bit rate. It is one of PMDCANBaud for standard CAN. When using CANFD the data bit rate is specified in the 2nd nibble. 
                baud = (PMDCANDataBuad << 4) | PMDCANBaud
                Warning: If the CAN port is currently open with a different baud it will be changed to the baud specified here.
                A baud value of 0xFF can be specified to use the current/default baud.

  addressmode   One of PMDCANMode

  When addressmode is PMDCANMode_StdID
  addressTX     The address to send data to 
  
  
  PMDCANMode_StdMask      PMDCANMode_StdRange
  addressmode   PMDCANMode_StdID      PMDCANMode_StdMask      PMDCANMode_StdRange
  -------------------------------------------------------------------------------

  addressTX     The address to send data to when filter = PMDCANMode_StdID 
                The address mask send data to when filter = PMDCANMode_StdMask 
                The address to send data to when filter = PMDCANMode_StdRange 

*/
PMDCFunc PMDPeriphOpenCANFD       (PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDCANPort port, PMDCANBaud baud, PMDparam addressTX, PMDparam addressRX, PMDparam addressmode);

/*
  PMDPeriphOpenCANNodeID

  Wrapper function to open a handle to a PMD CAN device at nodeID.
  Same as PMDPeriphOpenCANFD except:

  baud          The desired bit rate. It is one of PMDCANBaud for standard CAN. When using CANFD the data bit rate is specified in the 2nd nibble. 
                baud = (PMDCANDataBuad << 4) | PMDCANBaud
                Warning: If the CAN port is currently open with a different baud it will be changed to the baud specified here.
  nodeID        The nodeID of the device. 0-127
                CAN_ADDRESS_BASE_TX         0x600   a CAN PMD device listens for commands at CAN address of 0x600 + nodeID
                CAN_ADDRESS_BASE_RX         0x580   a CAN PMD device sends command responses to CAN address of 0x580 + nodeID
                CAN_ADDRESS_BASE_INTR       0x180   a CAN PMD device sends asynchronous events (see SetInterruptMask) to CAN address of 0x180 + nodeID
*/
PMDCFunc PMDPeriphOpenCANNodeID   (PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam baud, PMDparam nodeID);

/*
  PMDPeriphOpenCANMask (wrapper function)

  same as PMDPeriphOpenCANFD except:
  receiveMask  A 0 bit in the filter mask will mask out the corresponding bit position of the ID filter
  receiveId    The message ID bits to be masked for acceptance filtering. See examples below.
  
  The bits of the received message ID where the corresponding receiveMask bits are 1 are relevant for acceptance filtering
  Some examples: 
	if receiveId = 0x200 and receiveMask = 0x700 then it will accept all messages with addresses 0x200-0x2FF
	if receiveId = 0x100 and receiveMask = 0x7F0 then it will accept all messages with addresses 0x100-0x10F
*/
PMDCFunc PMDPeriphOpenCANMask     (PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam baud, PMDparam receiveMask, PMDparam receiveId);

/*
  PMDPeriphOpenCANRange (wrapper function)

  same as PMDPeriphOpenCANFD except:
  receiveLow    The low (minimum) address to accept data from. 0-7FF
  receiveHigh   The high (maximum) address to accept data from. 0-7FF
*/
PMDCFunc PMDPeriphOpenCANRange    (PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam baud, PMDparam receiveLow, PMDparam receiveHigh);

/*
  PMDPeriphOpenTCP

  Open a peripheral handle to a TCP port
*/
PMDCFunc PMDPeriphOpenTCP         (PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam ipaddress, PMDparam portnum, PMDparam timeout);

/*
  PMDPeriphOpenUDP

  Open a peripheral handle to a UDP port
*/
PMDCFunc PMDPeriphOpenUDP         (PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam ipaddress, PMDparam portnum);

/*
  PMDPeriphOpenPIO

  Open a peripheral handle to the internal peripheral IO port
*/
PMDCFunc PMDPeriphOpenPIO         (PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDuint16 address, PMDuint8 eventIRQ, PMDDataSize datasize);

/*
  PMDPeriphOpenMultiDrop 

  Creates a handle to a multidrop (RS485) serial port peripheral for communicating with PMD products via RS485.
  Use the hPeriph handle for all communications and only close the hPeriphParent when done.
  hPeriphParent is a handle to an open serial port peripheral from a call to PMDPeriphOpenCOM.
*/
PMDCFunc PMDPeriphOpenMultiDrop   (PMDPeriphHandle* hPeriph, PMDPeriphHandle* hPeriphParent, PMDparam address);

// for peripherals that can receive asynchronous events, ReceiveEvent will wait until the event is received or timeout has elapsed
PMDCFunc PMDPeriphReceiveEvent    (PMDPeriphHandle* hPeriph, void *data, PMDparam *pnReceived, PMDparam nExpected, PMDparam timeoutms);

PMDCFunc PMDPeriphReceive         (PMDPeriphHandle* hPeriph, void *data, PMDparam *pnReceived, PMDparam nExpected, PMDparam timeoutms);

PMDCFunc PMDPeriphReceiveAddress  (PMDPeriphHandle* hPeriph, void *data, PMDparam *pnReceived, PMDparam nExpected, PMDparam timeoutms, PMDparam* address);

PMDCFunc PMDPeriphSend            (PMDPeriphHandle* hPeriph, const void *data, PMDparam nCount, PMDparam timeoutms);
/*
  PMDPeriphSendToAddress

  Send data to an open peripheral handle that supports addressing such as RS485, SPI, CAN or UDP.

  address   The peripheral-specific address to send data to. (CAN ID, RS485 address, SPI ChipSelect, or UDP ipaddress)
*/
PMDCFunc PMDPeriphSendToAddress   (PMDPeriphHandle* hPeriph, void *data, PMDparam nCount, PMDparam timeoutms, PMDparam address);

/*
  PMDPeriphRead and PMDPeriphWrite           

  For reading and writing parallel data via PCI or ISA only
*/
PMDCFunc PMDPeriphRead            (PMDPeriphHandle* hPeriph, void *data, PMDparam offset, PMDparam length);
PMDCFunc PMDPeriphWrite           (PMDPeriphHandle* hPeriph, void *data, PMDparam offset, PMDparam length);

void      PMDPeriphOut            (PMDPeriphHandle* hPeriph, PMDparam offset, PMDparam data);
PMDparam  PMDPeriphIn             (PMDPeriphHandle* hPeriph, PMDparam offset);

PMDCFunc PMDPeriphClose           (PMDPeriphHandle* hPeriph);
PMDCFunc PMDPeriphFlush           (PMDPeriphHandle* hPeriph);


#if defined(__cplusplus)
}
#endif

#endif
