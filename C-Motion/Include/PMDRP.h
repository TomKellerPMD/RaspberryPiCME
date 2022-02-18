#ifndef _DEF_INC_PMDRP
#define	_DEF_INC_PMDRP

//
//  PMDRP.h - PMD Prodigy/CME Resource Protocol class
//
//  Performance Motion Devices, Inc.
//

#include "PMDRPtypes.h"
#include "PMDtypes.h"

class PMDMutex
{
public:
    PMDMutex(PMDMutexHandle mutex);
    ~PMDMutex();
    PMDMutexHandle m_Mutex;
    BOOL Lock();
};


class PMDResourceProtocol
{
public:
#pragma pack(4) // 32-bit align in order to efficiently copy 32 bit words into the packet
    BYTE m_txBuffer[TOTAL_PACKET_LENGTH];
    BYTE m_rxBuffer[TOTAL_PACKET_LENGTH];
    BYTE m_txHeader[PACKET_HEADER_LENGTH];
#pragma pack()
    BYTE* m_prxBuffer;
    BYTE* m_ptxBuffer;
    int m_txbufindex;
    int m_rxbufindex;
    int m_rxbufcount;
    int m_bIgnoreResetError;
    PMDresult m_Result;


    PMDResourceProtocol();
    ~PMDResourceProtocol();

    PMDresult CheckErrorCode();
    void  DisplayReceivedPacket();
    BYTE* FormPacketHeader(BYTE resource, BYTE action, BYTE address = 0);
    void  AddSubAction(int command);
    void  AddSubCommand(int command);
    DWORD ExtractDWord();
    WORD  ExtractWord();
    BYTE  ExtractByte();
    void  ExtractString(char* pData);
    void  ExtractBytes(BYTE* pData, int nBytes);
    void  ExtractDWords(DWORD* pData, int nDWords);
    void  ExtractWords(WORD* pData, int nWords);
    void  AddDWord(DWORD data);
    void  AddWord(WORD data);
    void  AddByte(BYTE data);
    BYTE* GetBuffer();
    int   GetNumberBytesRead();
    long  m_DefaultTimeout;

    PMDresult ProcessPacket(long timeout = 0);

	virtual PMDresult SendReceivePacket(PMDparam timeout, BYTE* pdatain, BYTE* pdataout, PMDparam* nbytes){return PMD_ERR_InterfaceNotInitialized;}

    PMDresult NoOperation();
    PMDresult Reset();
    PMDresult ResetMagellan();
    PMDresult Command( int address, char axis, char cmd, int xCt, WORD *xDat, int rCt, WORD *rDat );
    PMDresult Command( int address, int xCt, WORD *xDat, int rCt, WORD *rDat );
    PMDresult RPCommand(int address, BYTE* pdatain, BYTE* pdataout, int nCount, PMDparam timeout);

    PMDresult ReadMemory (int address, DWORD* data, PMDparam offset_in_dwords, PMDparam dwords_to_read);
    PMDresult WriteMemory(int address, DWORD* data, PMDparam offset_in_dwords, PMDparam dwords_to_write);
    PMDresult ReadMemory (int address, WORD* data, PMDparam offset_in_words, PMDparam words_to_read);
    PMDresult WriteMemory(int address, WORD* data, PMDparam offset_in_words, PMDparam words_to_write);
    PMDresult EraseMemory(int address);

    PMDresult SendMail(int mailboxID, const void* pItem, int itemsize, PMDparam timeout);
    PMDresult ReceiveMail(int mailboxID, void* pItem, int itemsize, PMDparam timeout);
    PMDresult PeekMail(int mailboxID, void* pItem, int itemsize, PMDparam timeout);

    PMDresult SendCME(BYTE* pbuff, PMDparam nCount, PMDparam timeout);
    PMDresult ReceiveCME(BYTE* pbuff, PMDparam nMaxExpected, PMDparam timeout, PMDparam* nReceived);
    PMDresult SendPeriph(int address, char* pbuff, PMDparam nCount, PMDparam timeout);
    PMDresult ReceivePeriph(int address, char* pbuff, PMDparam nMaxExpected, PMDparam timeout, PMDparam* nReceived);
    PMDresult WritePeriph(int address, WORD* pdata, PMDparam offset_in_bytes, PMDparam words_to_write);
    PMDresult ReadPeriph(int address, WORD* pdata, PMDparam offset_in_bytes, PMDparam words_to_read);

    PMDresult SetConsole(int periphaddress);
    PMDresult OpenPeriphTCP(int* periphaddress, DWORD ipaddress, DWORD port);
    PMDresult OpenPeriphUDP(int* periphaddress, DWORD ipaddress, DWORD port);
    PMDresult OpenPeriphSerial(int* periphaddress, int portno, int baud, int parity, int stopbits);
    PMDresult OpenPeriphCAN(int* periphaddress, int portno, PMDparam baud, PMDparam transmitid, PMDparam receiveid, PMDparam eventid);
    PMDresult OpenPeriphPAR(int* periphaddress, WORD address, BYTE irq, BYTE datasize);
    PMDresult OpenPeriphISA(int* periphaddress, WORD address, BYTE irq, BYTE datasize);
    PMDresult OpenPeriphSPI(int* periphaddress, BYTE port, BYTE chipselect, BYTE mode, BYTE datasize, DWORD bitrate);
    PMDresult OpenPeriphMultiDrop(int parentaddress, int nodeID);
    PMDresult OpenMemory(int* resourceaddress, int* memsize, PMDDataSize datasize, PMDMemoryType memorytype);
    PMDresult OpenMailbox(int* resourceaddress, int mailboxID, int depth, int itemsize);
	PMDresult OpenRPDevice(int* resourceaddress, int periphaddress);
	PMDresult OpenMotionProcessor(int* resourceaddress, int periphaddress);

    PMDresult CloseMotionProcessor(int address);
	PMDresult CloseDevice(int address);
    PMDresult ClosePeriph(int address);
    PMDresult CloseMemory(int address);
                
    PMDresult GetDefault(DWORD dwParamNo, DWORD* dwParam);
    PMDresult SetDefault(DWORD dwParamNo, DWORD dwParam);
                
    PMDresult StartUserTasks(DWORD taskparam);
    PMDresult StopUserTasks();
    PMDresult GetUserCodeTaskName(int taskno, char*);
    PMDresult GetUserCodeFileName(char*);
    PMDresult GetUserCodeFileDate(char*);
    PMDresult GetUserCodeFileChecksum(DWORD*);
    PMDresult GetUserCodeFileVersion(DWORD*);
    PMDresult GetUserCodeStatus(int taskno, PMDint32 *state);
    PMDresult GetTaskInfo(int taskno, PMDTaskInfo ID, PMDint32* info);
    PMDresult GetDeviceInfo(PMDDeviceInfo ID, WORD option, DWORD* info);
    PMDresult GetFirmwareVersion(DWORD* version);
    PMDresult GetSystemTime(SYSTEMTIME* time);
    PMDresult SetSystemTime(const SYSTEMTIME* time);
    PMDresult EraseUserCode();
    PMDresult StoreUserCodeBegin(int filesize);
    PMDresult StoreUserCodeData(BYTE* pdata, int length);
    PMDresult StoreUserCodeEnd();
    PMDresult GetResetCause(WORD resetmask, WORD* resetcause);
    PMDresult GetFaultCode(WORD index, DWORD* code);
    PMDresult SetNodeID(BYTE nodeId, BYTE DOsignal, BYTE DIsignal, BYTE DIsense);

};

#endif

