#ifndef PMD_IXXAT_CAN
#define PMD_IXXAT_CAN

//  PMDIXXATCAN.h -- CAN IO based on the IXXAT VCI API
//
//  Performance Motion Devices, Inc.
//

#include <windows.h>

typedef struct tagPMDCANIOTransportData {

    PMDuint16 board_number;     // 0-based index to the IXXAT board number
    PMDuint32 baudrate;         // can baud rate
    PMDuint32 txID;             // ID used for XMIT frames
    PMDuint32 rxMask;           // mask for RX frames
    PMDuint32 rxIntMask;        // mask for RX interrupt frames
    PMDuint32 timeoutms;        // timeout period in ms
    int    FDMode;              // FDCAN mode
    HANDLE hDevice;             // device handle
    HANDLE hCanCtl;             // controller handle 
    HANDLE hCanChn;             // channel handle
} PMDCANIOTransportData;

#define CAN_ADDRESS_BASE_RX         0x580
#define CAN_ADDRESS_BASE_TX         0x600
#define CAN_ADDRESS_BASE_INTR       0x180

PMDresult PMDCAN_Close(void* transport_data);
PMDresult PMDCAN_InitPort(PMDCANIOTransportData* CANtransport_data);
PMDresult PMDCAN_SetNodeID(PMDCANIOTransportData* CANtransport_data, PMDuint8 nodeID);
PMDresult PMDCAN_SetBaud(PMDCANIOTransportData* CANtransport_data, PMDuint8 baud);
PMDresult PMDCAN_FlushBuffers(PMDCANIOTransportData* CANtransport_data);
PMDresult TransmitData(PMDCANIOTransportData* CANtransport_data, char *data, int nbytes, int timeout);
PMDresult ReceiveData( PMDCANIOTransportData* CANtransport_data, char *data, int* nBytesReceieved, int nBytesExpected, int timeout);

#endif


