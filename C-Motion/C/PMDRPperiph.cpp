//********************************************************
// PMDRPperiph.cpp
// PMD Prodigy/CME Resource Protocol wrapper class implementation
//********************************************************

#include "PMDtypes.h"
#include "PMDsys.h"
#include "PMDperiph.h"
#include "PMDPfunc.h"
#include "PMDRPperiph.h"


PMDMutexDefine(m_MutexRemote);
PMDMutexDefine(m_MutexCOM);
PMDMutexDefine(m_MutexCAN);
PMDMutexDefine(m_MutexSPI);


//********************************************************
// generic RP protocol wrapper class
PMDRPperiph::PMDRPperiph(PMDPeriphHandle* hPeriph)
{
    m_hPeriph = hPeriph;
    m_Address = 0;
}

void PMDRPperiph::SetAddress(PMDparam address)
{
    m_Address = address;
}

void PMDRPperiph::WriteByte(BYTE byte)
{
    PMDPeriphSend(m_hPeriph, &byte, 1, RP_TIMEOUT);
}

PMDresult PMDRPperiph::SendPacket(BYTE* pbuff, PMDparam nCount)
{
    return PMDPeriphSend(m_hPeriph, pbuff, nCount, RP_TIMEOUT);
}

PMDresult PMDRPperiph::ReceivePacket(BYTE* pbuff, PMDparam nMaxCount, PMDparam timeout, PMDparam* nReceived)
{
    return PMDPeriphReceive(m_hPeriph, pbuff, (PMDparam*)nReceived, nMaxCount, timeout);
}

PMDresult PMDRPperiph::SendReceivePacket(PMDparam timeout, BYTE* pdatain, BYTE* pdataout, PMDparam* nbytes)
{
    PMDresult result;

    result = SendPacket(pdatain, *nbytes);
    if (result == PMD_ERR_OK)
        result = ReceivePacket(pdataout, USER_PACKET_LENGTH, timeout, nbytes);

    return result;
}

//********************************************************
// Remote PRP peripheral 
PMDRPperiphRemote::PMDRPperiphRemote(PMDPeriphHandle* hPeriph) : PMDRPperiph(hPeriph)
{
    if (m_MutexRemote == NULL)
      m_MutexRemote = PMDMutexCreate();
}

PMDresult PMDRPperiphRemote::Open()
{
    PMDresult result;
    // Get the device handle attached to the peripheral
    PMDDeviceHandle* phDevice = (PMDDeviceHandle*)m_hPeriph->transport_data;
    // Get the PMDRPperiph object attached to the device handle
    PMDResourceProtocol* rp = (PMDResourceProtocol*)phDevice->transport_data; 
    result = rp->OpenRPDevice((int*)&m_Address, (int)m_hPeriph->address);

    return result;
}

PMDresult PMDRPperiphRemote::SendReceivePacket(PMDparam timeout, BYTE* pdatain, BYTE* pdataout, PMDparam* nbytes)
{
    PMDresult result;
    // Get the device handle attached to the peripheral
    PMDDeviceHandle* phDevice = (PMDDeviceHandle*)m_hPeriph->transport_data;
    // Get the PMDRPperiph object attached to the device handle
    PMDResourceProtocol* rp = (PMDResourceProtocol*)phDevice->transport_data; 

    PMDMutexLock(m_MutexRemote);

    PMDparam nCount = *nbytes;
    result = rp->RPCommand(m_Address, pdatain, pdataout, nCount, RP_TIMEOUT + timeout);
    *nbytes = rp->GetNumberBytesRead();

    PMDMutexUnlock(m_MutexRemote);

    return result;
}


//********************************************************
// COM port protocol is slightly different
PMDRPperiphCOM::PMDRPperiphCOM(PMDPeriphHandle* hPeriph) : PMDRPperiph(hPeriph)
{
    if (m_MutexCOM == NULL)
      m_MutexCOM = PMDMutexCreate();
}

PMDresult PMDRPperiphCOM::SendPacket(BYTE* pbuff, PMDparam nByteCount)
{
    BYTE bMultidrop = (BYTE)(m_hPeriph->param >> 8);
    BYTE RS485address = (BYTE)m_hPeriph->param;
    BYTE checksum = 0;
    BYTE packetlength = (BYTE)nByteCount;
    BYTE COMpacket[TOTAL_PACKET_LENGTH + 8];
    PMDparam i;

    PMDPCOM_FlushRecv(m_hPeriph);

    if (nByteCount == 0 || nByteCount > TOTAL_PACKET_LENGTH)
        return PMD_ERR_RP_PacketLength;

    for (i=0; i<nByteCount; i++)
    {
        checksum += pbuff[i];
    }

    // form the packet to send.
    i = 0;
    if (bMultidrop)
        COMpacket[i++] = RS485address;
    COMpacket[i++] = checksum;
    COMpacket[i++] = packetlength;
    memcpy(&COMpacket[i], pbuff, nByteCount);

    return PMDPeriphSend(m_hPeriph, COMpacket, nByteCount+i, RP_TIMEOUT);
}

PMDresult PMDRPperiphCOM::ReceivePacket(BYTE* pbuff, PMDparam nMaxCount, PMDparam timeout, PMDparam* nReceived)
{
    PMDresult result = PMD_ERR_OK;
    BYTE  bMultidrop = (BYTE)(m_hPeriph->param >> 8);
    BYTE  RS485address = (BYTE)m_hPeriph->param;
    BYTE  checksum = 0;
    PMDparam packetlength = 0;
    PMDparam nBytesReceived = 0;
    PMDparam nBytesExpected = 2;
    BYTE header[4];
    PMDparam i = 0;

    if (bMultidrop)
    {
        nBytesExpected++;
        i = 1;
    }
    result = PMDPeriphReceive(m_hPeriph, &header, &nBytesReceived, nBytesExpected, timeout);
    if (result == PMD_ERR_OK)
    {
        checksum = header[i++];
        packetlength = header[i++];
        nBytesExpected = min(packetlength, nMaxCount);
        result = PMDPeriphReceive(m_hPeriph, &pbuff[PACKET_PADDING_BYTES], &nBytesReceived, nBytesExpected, timeout);
        if (result == PMD_ERR_OK)
        {
            nBytesReceived += PACKET_PADDING_BYTES; // calling function expects return header to start at 4th byte
            *nReceived = nBytesReceived;
        }
        // return invalid address AFTER absorbing the packet to keep things in sync.
        if ((bMultidrop) && (header[0] != RS485address))
            return PMD_ERR_InvalidAddress;
    }

  return result;
}

PMDresult PMDRPperiphCOM::SendReceivePacket(PMDparam timeout, BYTE* pdatain, BYTE* pdataout, PMDparam* nbytes)
{
    PMDresult result;

    PMDMutexLock(m_MutexCOM);

    result = SendPacket(pdatain, *nbytes);
    if (result == PMD_ERR_OK)
        result = ReceivePacket(pdataout, USER_PACKET_LENGTH, timeout, nbytes);

    PMDMutexUnlock(m_MutexCOM);

    return result;
}


//*********************************************************************************
// The CAN port protocol includes a sequence number as the first byte
PMDRPperiphCAN::PMDRPperiphCAN(PMDPeriphHandle* hPeriph) : PMDRPperiph(hPeriph)
{
    if (m_MutexCAN == NULL)
      m_MutexCAN = PMDMutexCreate();
}

PMDresult PMDRPperiphCAN::SendPacket(BYTE* pbuff, PMDparam nByteCount)
{
    int nSequence;
    int nContinued;
    int nPacketSize;
    PMDparam nPacketCount = 0;
    PMDparam nBytesReceived = 0;
    BYTE CANpacket[64];
    const BYTE* pDataTx;
    PMDErrorCode result;
    int maxbytes = 8;
    int PacketDataSize;

    // Flush any data that may have been received erroneously prior to sending a new command.
//    while (PMD_ERR_OK == PMDPeriphReceive(m_hPeriph, CANpacket, &nBytesReceived, 8, 0))
//        nPacketCount++; // count the number of erroneous packets received for debugging

    if (nByteCount == 0 || nByteCount > TOTAL_PACKET_LENGTH)
        return PMD_ERR_RP_PacketLength;

    // if data baud is non-zero we are in FD mode.
    if (m_hPeriph->param & 0xF000)
        maxbytes = 64;

    pDataTx = pbuff;
    PacketDataSize = maxbytes - 1;
    nContinued = (nByteCount-1) / PacketDataSize;
    nSequence = 1;
    CANpacket[0] = (char)nContinued | 0x80;
    while (nContinued-- > 0)
    {
        memcpy(&CANpacket[1], pDataTx, PacketDataSize);
        result = PMDPeriphSend(m_hPeriph, CANpacket, maxbytes, RP_TIMEOUT);
        if (PMD_ERR_OK != result)
            return result;
        pDataTx += PacketDataSize;
        CANpacket[0] = (char)nSequence++;
    }
    {
        nPacketSize = (nByteCount-1) % PacketDataSize + 1;
        if (nPacketSize > 0)
        {
            memcpy(&CANpacket[1], pDataTx, nPacketSize);
            nPacketSize += 1;
            result = PMDPeriphSend(m_hPeriph, CANpacket, nPacketSize, RP_TIMEOUT);
        }
    }
    return result;
}

PMDresult PMDRPperiphCAN::ReceivePacket(BYTE* pbuff, PMDparam nMaxCount, PMDparam timeout, PMDparam* nReceived)
{
    int nSequence;
    int nExpectedSequence;
    int nContinued;
    BYTE CANpacket[64];
    BYTE* pDataRx = &pbuff[PACKET_PADDING_BYTES];
    PMDparam bInitialMessage;
    PMDparam nBytesReceived = 0;
    PMDparam nBytesExpected = 1; // We will receive 1-8 bytes in each PMDPeriphReceive call.
    PMDparam nBytesReceivedTotal = 0;
    PMDErrorCode result;

    result = PMDPeriphReceive(m_hPeriph, CANpacket, &nBytesReceived, nBytesExpected, timeout);
    if (PMD_ERR_OK != result)
        return result;

    if (nBytesReceived == 0)
        return PMD_ERR_InsufficientDataReceived;

    bInitialMessage = CANpacket[0] >> 7;
    nContinued = CANpacket[0] & 0x7F;
    if (bInitialMessage)
    {
        nBytesReceived--; // skip header byte
        // copy CAN packet to command packet
        memcpy(pDataRx, &CANpacket[1], nBytesReceived);
        pDataRx += nBytesReceived;
        nBytesReceivedTotal += nBytesReceived;
        nExpectedSequence = 1;
        while (nContinued-- > 0)
        {
            result = PMDPeriphReceive(m_hPeriph, CANpacket, &nBytesReceived, nBytesExpected, timeout);
            if (PMD_ERR_OK == result)
            {
                if (nBytesReceived == 0)
                    return PMD_ERR_InsufficientDataReceived;

                bInitialMessage = CANpacket[0] >> 7;
                if (bInitialMessage)
                {
                    return PMD_ERR_CommunicationsError;
                }
                nSequence = CANpacket[0] & 0x7F;
                if (nSequence != nExpectedSequence)
                {
                    return PMD_ERR_CommunicationsError;
                }
                nExpectedSequence++;
                nBytesReceived--; // skip header byte
                memcpy(pDataRx, &CANpacket[1], nBytesReceived);
                pDataRx += nBytesReceived;
                nBytesReceivedTotal += nBytesReceived;
            }
        }
    }

    nBytesReceivedTotal += PACKET_PADDING_BYTES; // calling function expects header length to be 4 bytes
    *nReceived = nBytesReceivedTotal;

    return result;
}

PMDresult PMDRPperiphCAN::SendReceivePacket(PMDparam timeout, BYTE* pdatain, BYTE* pdataout, PMDparam* nbytes)
{
    PMDresult result;

    PMDMutexLock(m_MutexCAN);

    result = SendPacket(pdatain, *nbytes);
    if (result == PMD_ERR_OK)
        result = ReceivePacket(pdataout, USER_PACKET_LENGTH, timeout, nbytes);

    PMDMutexUnlock(m_MutexCAN);

    return result;
}

PMDRPperiphSPI::PMDRPperiphSPI(PMDPeriphHandle* hPeriph) : PMDRPperiph(hPeriph)
{
    if (m_MutexSPI == NULL)
      m_MutexSPI = PMDMutexCreate();
}

#define SPIDataType             char
#define SPIDataTypeSize			sizeof(SPIDataType)

static BYTE CalculateChecksum(BYTE* pbuff, int datacount)
{
    int i;
    DWORD checksum = 0;
    
    for (i=0; i<datacount; i++)
    {
        checksum += (BYTE)pbuff[i];
    }
    do {
        checksum = (checksum & 0xFF) + ((checksum >> 8) & 0xFF);
    } while (checksum & 0xFFFFFF00);
    return (BYTE)~checksum;
}

/* SPI send packet format
   -----------------------|--------------------------           --------------------------- 
   |  byte 0  |  byte 1   |  byte 2   |   byte 3   |           |   byte n   |   byte n+1 | 
   |  length  |  length   |  PRP 0    |   PRP 1    |  payload  |         checksum        |
   -----------------------|--------------------------           ---------------------------

The length is the number of (8 bit) bytes in the PRP packet to follow.  This number is important in the PRP protocol,  
however the packet is always sent as a sequence of 16 bit words when using the SPI transport.  
If the number of bytes is odd then the last byte (the high byte of the last word) is a zero pad.

The length may be zero, in which case no PRP packet nor Command Footer follows.  
A zero length is used for reading a PRP command response packet, and may be  useful in order to read the slave status header.  

Bits p0 and p1 are used as an integrity check on the header itself.  If they are not correct then the slave processor 
will not process any subsequent words as a PRP packet, but will report an error in the next status header that it sends.

p0 is chosen so that bit 0 xor bit 2 xor bit 4 … xor bit 14 = 0.
 p1 is chosen so that bit 1 xor bit 3 xor bit 5 … xor bit 15 = 1.

In practice p0 and p1 may be computed together from the value of the word x with the p0 and p1 fields set to zero:
1.	x1 = (x & 0xFF) ^ ((x >> 8) & 0xFF)
2.	x2 = (x1 & 0xF) ^ ((x >> 4) & 0xF)
3.	x3 = (x2 & 3) ^ ((x2>>2) & 3) ^ 1
4.	x3 is [p1 : p0]

    8-bit             bytecount status command subcommand   <- bytecount SPIstatus PRPstatus data 
    8-bit CMENoOperation 02 00 60 00 9F                     <- 01 0c 40 BF
    8-bit CMEGetVersion  04 00 6A 00 01 00 94               <- 05 0c 40 17 00 00 40 68
*/

static const char NullBuffer[TOTAL_PACKET_LENGTH] = {0};

PMDresult PMDRPperiphSPI::SendPacket(BYTE* pbuff, PMDparam nByteCount)
{
    BYTE checksum = 0;
    BYTE datacount;
    DWORD i;

    if (nByteCount == 0 || nByteCount > TOTAL_PACKET_LENGTH)
        return PMD_ERR_RP_PacketLength;

    if (nByteCount % SPIDataTypeSize)
        nByteCount++;
    datacount = (BYTE)(nByteCount / SPIDataTypeSize);
    /*
    The checksum is the logical not of the 16 bit ones complement checksum of the entire packet, but excluding the header.  
    The ones complement checksum computed on the packet words should be 0xFFFF.  
    */
    i = 0;
    SPIpacket[0] = 0; // reserved for parity bits or larger lengths
    SPIpacket[1] = (BYTE)nByteCount;
    PMDparam j = 2;
    for (i=0; i<datacount; i++)
    {
        SPIpacket[j] = pbuff[i];  //copy pbuff to SPIpacket
        checksum += SPIpacket[j];
        j++;
    }
    checksum = CalculateChecksum(&SPIpacket[2], datacount);
    SPIpacket[j] = checksum;
    nByteCount += 2; // 2 bytes for the header
    nByteCount += 1; // 1 bytes for the checksum

    return PMDPeriphSend(m_hPeriph, SPIpacket, nByteCount, RP_TIMEOUT);
}

/* SPI receive packet format
   --------------------------|-------------           --------------
   |   byte 0   |   byte 1   |   byte 2   |           |   byte n   | 
   |   status   |   length   |   PRP 0    |  payload  |  checksum  |
   --------------------------|-------------           --------------
*/
PMDresult PMDRPperiphSPI::ReceivePacket(BYTE* pbuff, PMDparam nMaxCount, PMDparam timeout, PMDparam* nReceived)
{
    PMDresult result = PMD_ERR_OK;
    BYTE  checksum = 0;
    BYTE  calculatedchecksum = 0;
    PMDparam packetlength = 0;
    PMDparam nBytesReceived = 0;
    PMDparam nBytesExpected = 2;
    PMDparam datacount;
    BYTE header[4];
    union {
        BYTE b[4];
        DWORD dw;
    } zero;
    BYTE state = 0;
    BYTE* pData = (BYTE*)&pbuff[PACKET_PADDING_BYTES];
    zero.dw = 0;

    do
    {
        // A nExpected value of 0 is a special case that calls PMDPSPI_WaitUntilReady
        result = PMDPeriphReceive(m_hPeriph, NULL, &nBytesReceived, 0, timeout);

        // The send performs the bi-directional transaction. The receive retrieves the data from a buffer that was filled during the send.
        if (result == PMD_ERR_OK)
            result = PMDPeriphSend(m_hPeriph, zero.b, nBytesExpected, timeout);
        if (result == PMD_ERR_OK)
        // Retrieve the packetlength and SPIStatus bits in the first 2 bytes
            result = PMDPeriphReceive(m_hPeriph, &header, &nBytesReceived, nBytesExpected, timeout);
        if (result == PMD_ERR_OK)
        {
            packetlength = header[1]; // packetlength is in bytes
            state = (header[0] >> 2) & 0x07;
            if (state == HostSPIState_ResultReady) // Retrieve the response if it is ready
            {
                nBytesExpected = min(packetlength, nMaxCount);
                nBytesExpected++; // checksum is not included in packetlength
                // Send null data in order to receive
                if (result == PMD_ERR_OK)
                    result = PMDPeriphSend(m_hPeriph, NullBuffer, nBytesExpected, timeout);
                result = PMDPeriphReceive(m_hPeriph, pData, &nBytesReceived, nBytesExpected, timeout);
                if (result == PMD_ERR_OK)
                {
                    datacount = nBytesReceived;
                    datacount -= 1; // exclude the checksum
                    checksum = pData[datacount];
                    calculatedchecksum = CalculateChecksum(pData, datacount);
                    if (calculatedchecksum != checksum)
                        result = PMD_ERR_Checksum;
                    datacount += PACKET_PADDING_BYTES; // calling function expects return header to start at 4th byte
                    *nReceived = datacount;
                }
            }
            else if (state != HostSPIState_WaitingForResult)
                return PMD_ERR_RP_InvalidPacket;
        }
    } while (state == HostSPIState_WaitingForResult);

   
    return result;
}

PMDresult PMDRPperiphSPI::SendReceivePacket(PMDparam timeout, BYTE* pdatain, BYTE* pdataout, PMDparam* nbytes)
{
    PMDresult result;

    PMDMutexLock(m_MutexSPI);

    result = SendPacket(pdatain, *nbytes);
    if (result == PMD_ERR_OK)
        result = ReceivePacket(pdataout, USER_PACKET_LENGTH, timeout, nbytes);

    PMDMutexUnlock(m_MutexSPI);

    return result;
}


// C++ wrapper functions for calling Packet functions from C code when accessing a remote PRP device
#if defined(__cplusplus)
extern "C" {
#endif

PMDresult SendReceivePacket(PMDDeviceHandle* pDevice, PMDparam timeout, BYTE* pdatain, BYTE* pdataout, PMDparam* nbytes)
{
    return ((PMDRPperiph*)(pDevice->transport_data))->SendReceivePacket(timeout, pdatain, pdataout, nbytes); 
}

#if defined(__cplusplus)
}
#endif
