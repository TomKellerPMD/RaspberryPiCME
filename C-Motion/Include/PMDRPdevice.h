
#if defined(__cplusplus)
extern "C" {
#endif

PMDresult PMDRPCOM_Open(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam portnum, PMDparam baud, PMDparam parity, PMDparam stopbits);
PMDresult PMDRPCAN_Open(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam portnum, PMDparam baud, PMDparam transmitid, PMDparam receiveid, PMDparam eventid);
PMDresult PMDRPSPI_Open(PMDPeriphHandle* hPeriph, PMDDeviceHandle* hDevice, PMDuint8 portnum, PMDuint8 chipselect, PMDuint8 mode, PMDuint8 datasize, PMDparam bitrate);
PMDresult PMDRPPAR_Open(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDuint16 address, PMDuint8 eventIRQ, PMDDataSize datasize);
PMDresult PMDRPISA_Open(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDuint16 address, PMDuint8 eventIRQ, PMDDataSize datasize);
PMDresult PMDRPUDP_Open(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam ipaddress, PMDparam portnum);
PMDresult PMDRPTCP_Open(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDparam ipaddress, PMDparam portnum);
PMDresult PMDRPMultiDrop_Open(PMDPeriphHandle* hPeriph, PMDDeviceHandle *hDevice, PMDPeriphHandle* hPeriphParent, PMDparam nodeID);

PMDresult PMDRPMemoryOpen(PMDMemoryHandle *hMemory, PMDDeviceHandle *hDevice, PMDDataSize datasize, PMDMemoryType memorytype);
PMDresult PMDRPMemoryWrite(PMDMemoryHandle *hMemory, void *data, PMDparam offset, PMDparam length);
PMDresult PMDRPMemoryRead(PMDMemoryHandle *hMemory, void *data, PMDparam offset, PMDparam length);

PMDresult PMDRPMailboxOpen(PMDMailboxHandle* hMailbox, PMDDeviceHandle *hDevice, PMDparam mailboxID, PMDparam depth, PMDparam itemsize);

PMDCFunc PMDMPDeviceOpenRemote      (PMDDeviceHandle* hDevice, PMDPeriphHandle* hPeriph);


#if defined(__cplusplus)
}
#endif
