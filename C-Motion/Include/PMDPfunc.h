#ifndef _DEF_INC_Pfunc
#define _DEF_INC_Pfunc

#if defined(__cplusplus)
extern "C" {
#endif

// forward declerations for local peripherals. Not to be called directly.
PMDresult PMDPCME_Open(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice);
PMDresult PMDPPRP_Open(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam channel, PMDparam bufsize);
PMDresult PMDPCOM_Open(PMDPeriphHandle* hPeriph, PMDparam portnum, PMDparam baud, PMDparam parity, PMDparam stopbits);
PMDresult PMDPTCP_Open(PMDPeriphHandle* hPeriph, PMDparam ipaddress, PMDparam portnum, PMDparam timeout);
PMDresult PMDPUDP_Open(PMDPeriphHandle* hPeriph, PMDparam ipaddress, PMDparam portnum);
PMDresult PMDPISA_Open(PMDPeriphHandle* hPeriph, PMDparam address, PMDparam eventIRQ, PMDDataSize datasize);
PMDresult PMDPPAR_Open(PMDPeriphHandle* hPeriph, PMDparam address, PMDparam eventIRQ, PMDDataSize datasize);
PMDresult PMDPPCI_Open(PMDPeriphHandle* hPeriph, int board_number);
PMDresult PMDPCAN_Open(PMDPeriphHandle* hPeriph, PMDCANPort port, PMDparam baud, PMDparam addressTX, PMDparam addressRX, PMDparam addressEvent);
PMDresult PMDPCAN_OpenRange(PMDPeriphHandle* hPeriph, PMDCANPort port, PMDparam baud, PMDparam addressRXlo, PMDparam addressRXhi);
PMDresult PMDPSPI_Open(PMDPeriphHandle* hPeriph, PMDuint8 port, PMDuint8 chipselect, PMDuint8 mode, PMDuint8 datasize, PMDparam bitrate);

// interface specific config functions
// PMPPxxx are PRP 
// PMPxxx are Magellan 

PMDresult PMDPCAN_AddNodeID(PMDPeriphHandle* hPeriph, PMDPeriphHandle* hPeriphParent, PMDuint8 nodeID);
PMDresult PMDPCOM_SetConfig(PMDPeriphHandle* hPeriph, PMDparam baud, PMDparam parity, PMDparam stopbits);

PMDresult PMDSerial_Sync(PMDPeriphHandle* hPeriph);
PMDresult PMDSerial_Send(void* transport_data, PMDuint8 xCt, PMDuint16* xDat, PMDuint8 rCt, PMDuint16* rDat);
PMDresult PMDSerial_SetConfig(void* transport_data, PMDuint16 serialmode);
PMDresult PMDSerial_Init(void);
PMDresult PMDPCOM_SetTimeout(PMDPeriphHandle* hPeriph, PMDparam msec);
PMDresult PMDPCOM_FlushRecv(PMDPeriphHandle* hPeriph);

PMDresult PMDSPI_Send(void* transport_data, PMDuint8 xCt, PMDuint16* xDat, PMDuint8 rCt, PMDuint16* rDat);
PMDresult PMDSPI_Init(void);

PMDresult PMDCAN_Send(void* transport_data, PMDuint8 xCt, PMDuint16* xDat, PMDuint8 rCt, PMDuint16* rDat);
PMDresult PMDCAN_WaitForEvent(PMDDeviceHandle* hDevice, PMDEvent* event, PMDparam timeout);
PMDresult PMDCAN_Init(void);

PMDresult PMDISA_Send(void* transport_data, PMDuint8 xCt, PMDuint16* xDat, PMDuint8 rCt, PMDuint16* rDat);
PMDresult PMDISA_WaitForEvent(PMDDeviceHandle* hDevice, PMDEvent* event, PMDparam timeout);
PMDresult PMDISA_ReadMemory(PMDMemoryHandle *hMemory, void* data, PMDparam offset_in_dwords, PMDparam dwords_to_read);
PMDresult PMDISA_WriteMemory(PMDMemoryHandle *hMemory, void* data, PMDparam offset_in_dwords, PMDparam dwords_to_write);
PMDresult PMDISA_Init(void);

PMDresult PMDPCI_ReadMemory(PMDMemoryHandle *hMemory, void* data, PMDparam offset_in_dwords, PMDparam dwords_to_read);
PMDresult PMDPCI_WriteMemory(PMDMemoryHandle *hMemory, void* data, PMDparam offset_in_dwords, PMDparam dwords_to_write);
PMDresult PMDPCI_Init(void);

PMDresult PMDPAR_Send(void* transport_data, PMDuint8 xCt, PMDuint16* xDat, PMDuint8 rCt, PMDuint16* rDat);
PMDresult PMDPAR_Read(void* transport_data, PMDuint32* data, PMDparam offset, PMDparam length);
PMDresult PMDPAR_Write(void* transport_data, PMDuint32* data, PMDparam offset, PMDparam length);
PMDresult PMDPAR_ReadMemory(PMDMemoryHandle *hMemory, void* data, PMDparam offset_in_dwords, PMDparam dwords_to_read);
PMDresult PMDPAR_WriteMemory(PMDMemoryHandle *hMemory, void* data, PMDparam offset_in_dwords, PMDparam dwords_to_write);
PMDresult PMDPAR_WaitForEvent(PMDDeviceHandle* hDevice, PMDEvent* event, PMDparam timeout);
PMDresult PMDPAR_HardReset(void* transport_data);
PMDresult PMDPAR_WaitUntilReady(void* transport_data);
PMDresult PMDPAR_Init(void);


#if defined(__cplusplus)
}
#endif


#endif