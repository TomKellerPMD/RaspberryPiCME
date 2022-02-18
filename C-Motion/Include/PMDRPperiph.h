#ifndef _DEF_INC_PMDRPperiph
#define _DEF_INC_PMDRPperiph

//
//  PMDRPperiph.h -- PMD Prodigy/CME Resource Protocol wrapper class for use with a periph handle
//
//  Performance Motion Devices, Inc.
//

#include "PMDRPtypes.h"
#include "PMDRP.h"
#include "PMDperiph.h"

class PMDRPperiph : public PMDResourceProtocol
{
public:
	PMDPeriphHandle* m_hPeriph;
	PMDparam m_Address;
    
	PMDRPperiph(PMDPeriphHandle* hPeriph);
	virtual ~PMDRPperiph(){};

	void SetAddress(PMDparam address);

    void WriteByte(BYTE byte);
	void ReadByte(BYTE* byte);

	PMDresult SendPacket( BYTE* pbuff, PMDparam nMaxCount);
	PMDresult ReceivePacket(BYTE* pbuff, PMDparam nMaxCount, PMDparam timeout, PMDparam* nReceived);
	virtual PMDresult SendReceivePacket(PMDparam timeout, BYTE* pdatain, BYTE* pdataout, PMDparam* nbytes);
};

class PMDRPperiphRemote : public PMDRPperiph
{
public:
	PMDRPperiphRemote(PMDPeriphHandle* hPeriph);
	virtual ~PMDRPperiphRemote(){};

    PMDparam m_nReceived;
    PMDresult Open(void);

	virtual PMDresult SendReceivePacket(PMDparam timeout, BYTE* pdatain, BYTE* pdataout, PMDparam* nbytes);

};

class PMDRPperiphCOM : public PMDRPperiph
{
public:
	PMDRPperiphCOM(PMDPeriphHandle* hPeriph);
	~PMDRPperiphCOM(){};

	PMDresult SendPacket( BYTE* pbuff, PMDparam nMaxCount);
	PMDresult ReceivePacket(BYTE* pbuff, PMDparam nMaxCount, PMDparam timeout, PMDparam* nReceived);
	virtual PMDresult SendReceivePacket(PMDparam timeout, BYTE* pdatain, BYTE* pdataout, PMDparam* nbytes);

};

class PMDRPperiphCAN : public PMDRPperiph
{
public:
	PMDRPperiphCAN(PMDPeriphHandle* hPeriph);
	~PMDRPperiphCAN(){};

	PMDresult SendPacket( BYTE* pbuff, PMDparam nMaxCount);
	PMDresult ReceivePacket(BYTE* pbuff, PMDparam nMaxCount, PMDparam timeout, PMDparam* nReceived);
	virtual PMDresult SendReceivePacket(PMDparam timeout, BYTE* pdatain, BYTE* pdataout, PMDparam* nbytes);
};

class PMDRPperiphSPI : public PMDRPperiph
{
	PMDPeriphHandle hPeriphPIO;
	BOOL bUseHostSPIStatus;
    BYTE SPIpacket[LINK_PACKET_LENGTH];

public:
	PMDRPperiphSPI(PMDPeriphHandle* hPeriph);
	~PMDRPperiphSPI(){};

	PMDresult WaitForReady(void);
	PMDresult SendPacket( BYTE* pbuff, PMDparam nMaxCount);
	PMDresult ReceivePacket(BYTE* pbuff, PMDparam nMaxCount, PMDparam timeout, PMDparam* nReceived);
	virtual PMDresult SendReceivePacket(PMDparam timeout, BYTE* pdatain, BYTE* pdataout, PMDparam* nbytes);

};

#endif

