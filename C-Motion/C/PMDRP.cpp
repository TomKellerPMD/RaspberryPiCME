//********************************************************
// PMDResourceProtocol.cpp
// PMD Prodigy/CME Resource Protocol class
//********************************************************

#include "PMDRPtypes.h"
#include "PMDperiph.h"
#include "PMDecode.h"
#include "PMDdiag.h"
#include "PMDsys.h"
#include "PMDRP.h"

#if USING_EXCEPTIONS
#define ERROR_CHECK(a) if (!(a)) {throw PMD_ERR_RP_PacketAlignment;}
#else
#define ERROR_CHECK(a) if (!(a)) {m_Result = PMD_ERR_RP_PacketAlignment;}
#endif


PMDMutex::PMDMutex(PMDMutexHandle mutex)
{
    m_Mutex = mutex;
    Lock();
}

BOOL PMDMutex::Lock()
{
    return PMDMutexLock(m_Mutex);
}

PMDMutex::~PMDMutex()
{
    PMDMutexUnlock(m_Mutex);
}

//********************************************************
PMDResourceProtocol::PMDResourceProtocol()
{
    m_rxbufindex = 0;
    m_txbufindex = 0;
    m_prxBuffer = &m_rxBuffer[PACKET_PADDING_BYTES]; // to word align response packets
    m_bIgnoreResetError = 0;
    m_Result = PMD_ERR_RP_InvalidPacket;
    m_DefaultTimeout = RP_TIMEOUT;
}

//********************************************************
PMDResourceProtocol::~PMDResourceProtocol()
{
}

//********************************************************
PMDresult PMDResourceProtocol::ProcessPacket(long timeout)
{
    PMDresult result = m_Result;
    PMDparam nReceived = 0;

    if (timeout == 0)
        timeout = m_DefaultTimeout;

    if (result == PMD_ERR_OK)
    {
        nReceived = m_txbufindex;
        result = SendReceivePacket(timeout, m_txBuffer, m_rxBuffer, &nReceived);
        if (result == PMD_ERR_OK)
        {
            m_rxbufcount = nReceived;
            if (result == PMD_ERR_OK)
            {
                if (m_rxbufcount > PACKET_PADDING_BYTES)
                {
                    m_rxbufcount -= PACKET_PADDING_BYTES; // ignore padding bytes
                    m_rxbufindex = PACKET_HEADER_LENGTH_RX; // point to data to extract
                    result = CheckErrorCode();
                }
                else
                    result = PMD_ERR_InsufficientDataReceived;
            }
        }
    }

    return result; // if exception handling is preferred, replace "return" with "throw" here to catch all communication errors.
}

//********************************************************
int PMDResourceProtocol::GetNumberBytesRead()
{
    return m_rxbufcount - 1; // exclude the header byte from the count
}

//********************************************************
PMDresult PMDResourceProtocol::CheckErrorCode()
{
    int bError = (m_prxBuffer[0] >> 4 & 3) == PMD_PRPStatus_Error;
    int status = (m_prxBuffer[0] >> 4 & 3);

    if (bError && m_rxbufcount < PACKET_ERROR_LENGTH)
    {
        DisplayReceivedPacket();
        return PMD_ERR_InsufficientDataReceived;
    }
    // check version bits
    if ((m_prxBuffer[0] >> 6 & 3) != PROTOCOL_VERSION)
        return PMD_ERR_Version;
    if (bError)
    {
        PMDErrorCode CMEerror = (PMDErrorCode)*(unsigned short*)&m_prxBuffer[ERROR_CODE_POS];
//      Printf("%s. Prodigy/CME error code: 0x%04X\r\n", PMDGetErrorMessage(CMEerror), CMEerror);
        if (CMEerror == PMD_ERR_RP_Reset && m_bIgnoreResetError)
        { // resend the command because it was not processed
            return ProcessPacket();
        }
        else
            return CMEerror;
    } 
    else if (status != 0)
    {
        return PMD_ERR_UnexpectedDataReceived;
    }
    
    return PMD_ERR_OK;
}

//********************************************************
void PMDResourceProtocol::AddSubCommand(int command)
{
    ERROR_CHECK(m_txbufindex == SUB_COMMAND_POS);

    m_txBuffer[SUB_COMMAND_POS] = command;
    m_txbufindex++;
}

//********************************************************
void PMDResourceProtocol::AddSubAction(int subaction)
{
    ERROR_CHECK(m_txbufindex == SUB_ACTION_POS);

    m_txBuffer[SUB_ACTION_POS] = subaction;
    m_txbufindex++;
}

//-----------------------------------------------------------------
//						  PRP outgoing header
//|			  byte 0			  |			  byte 1			  |
//|	7	6	5	4	3	2	1	0 |	7	6	5	4	3	2	1	0 |
//|	  |	  |	  |	  |	  |	  |	  |	  |	  |	  |	  |	  |	  |	  |	  |	  |
//|	 ver  |	status|	   action	  |	 resource |		 address	  |
//-----------------------------------------------------------------
//********************************************************
BYTE* PMDResourceProtocol::FormPacketHeader(BYTE resource, BYTE action, BYTE address /* = 0 */)
{
    m_txbufindex = PACKET_HEADER_LENGTH;
    action &= 0x0F;
    m_txBuffer[0] = PROTOCOL_VERSION << 6 | PMD_PRPStatus_Outgoing << 4 | action;
    m_txBuffer[1] = resource << 5 | address;
    m_txBuffer[2] = 0;
    m_txBuffer[3] = 0;
    m_Result = PMD_ERR_OK;

    return &m_txBuffer[2]; // return a pointer to the data portion
}

//********************************************************
DWORD PMDResourceProtocol::ExtractDWord()
{
    ERROR_CHECK((DWORD)(&m_prxBuffer[m_rxbufindex]) % 4 == 0); // make sure we're on a dword aligned address
    ERROR_CHECK(m_rxbufindex + 4 <= m_rxbufcount);

    DWORD data = *(DWORD*)&m_prxBuffer[m_rxbufindex];
    m_rxbufindex += 4;

    return data;
}

//********************************************************
WORD PMDResourceProtocol::ExtractWord()
{
    ERROR_CHECK((DWORD)(&m_prxBuffer[m_rxbufindex]) % 2 == 0); // make sure we're on a word aligned address
    ERROR_CHECK(m_rxbufindex + 2 <= m_rxbufcount);

    WORD data = *(WORD*)&m_prxBuffer[m_rxbufindex];
    m_rxbufindex += 2;

    return data;
}

//********************************************************
BYTE PMDResourceProtocol::ExtractByte()
{
    ERROR_CHECK(m_rxbufindex + 1 <= m_rxbufcount);
    return m_prxBuffer[m_rxbufindex++];
}

//********************************************************
void PMDResourceProtocol::ExtractString(char* str)
{
    int index = m_rxbufindex;
    char* pStart = (char*)&m_prxBuffer[index];
    int length = (int)strlen(pStart);
    m_rxbufindex += length;
    ERROR_CHECK(m_rxbufindex <= m_rxbufcount);
    strcpy(str, pStart);
}

//********************************************************
void PMDResourceProtocol::ExtractDWords(DWORD* pData, int nDWords)
{
    for (int i=0; i<nDWords; i++)
        pData[i] = ExtractDWord();
}

//********************************************************
void PMDResourceProtocol::ExtractWords(WORD* pData, int nWords)
{
    for (int i=0; i<nWords; i++)
        pData[i] = ExtractWord();
}

//********************************************************
void PMDResourceProtocol::ExtractBytes(BYTE* pData, int nBytes)
{
    int i = nBytes;
    while(i-- > 0)
        *pData++ = ExtractByte();
}

//********************************************************
BYTE* PMDResourceProtocol::GetBuffer()
{
    return m_prxBuffer;
}

//********************************************************
void PMDResourceProtocol::AddDWord(DWORD data)
{
    ERROR_CHECK((DWORD)(&m_txBuffer[m_txbufindex]) % 4 == 0);

    *(DWORD*)&m_txBuffer[m_txbufindex] = data;
    m_txbufindex += 4;
}

//********************************************************
void PMDResourceProtocol::AddWord(WORD data)
{
    ERROR_CHECK((DWORD)(&m_txBuffer[m_txbufindex]) % 2 == 0);

    *(WORD*)&m_txBuffer[m_txbufindex] = data;
    m_txbufindex += 2;
}

//********************************************************
void PMDResourceProtocol::AddByte(BYTE data)
{
    m_txBuffer[m_txbufindex++] = data;
}

//********************************************************
void PMDResourceProtocol::DisplayReceivedPacket()
{
/*
    Printf( "Packet received:" );
    for( int i=0; i<m_rxbufindex; i++ )
        Printf( " %02X", (BYTE)m_prxBuffer[i]);
    Printf("\n");
*/
}

//*********************************************************************************
// PMD RP Specific commands
// words and dwords are little endian
//*********************************************************************************
// tx byte 1 | PMD_Action_NOP
// tx byte 2 | PMD_Resource_Device
PMDresult PMDResourceProtocol::NoOperation()
{
    FormPacketHeader(PMD_Resource_Device, PMD_Action_NOP);
    
    return ProcessPacket();
}

//********************************************************
// Magellan command processing
PMDresult PMDResourceProtocol::Command( int address, int xCt, WORD *xDat, int rCt, WORD *rDat )
{
    char axis = (char)(xDat[0] >> 8);
    char cmd  = (char)xDat[0];

    xCt--;
    xDat++;
    return Command( address, axis, cmd, xCt, xDat, rCt, rDat );
}

//********************************************************
PMDresult PMDResourceProtocol::Command( int address, char axis, char cmd, int xCt, WORD *xDat, int rCt, WORD *rDat )
{
    int  i;
    PMDresult result;

    FormPacketHeader(PMD_Resource_MotionProcessor, PMD_Action_Command, address);

    // place expected #return words in axis word for efficiency
    // it will be extracted by the receiving end
    axis |= (rCt << 6);
    // add axis number and command code
    AddByte(cmd);
    AddByte(axis);

    // add data
    for( i=0; i<xCt; i++ )
    {
        AddWord(xDat[i]);
    }

    result = ProcessPacket();

    if (result == PMD_ERR_OK)
    {
        // retrieve data
        for( i=0; i<rCt; i++ )
        {
            rDat[i] = ExtractWord();
        }
    }
    return result;
}

//********************************************************
// Passthrough/remote PRP command processing
PMDresult PMDResourceProtocol::RPCommand(int address, BYTE* pdatain, BYTE* pdataout, int nCount, PMDparam timeout)
{
    PMDresult result;
    int i;
    BYTE* pdata = pdatain;

    if (nCount > USER_PACKET_LENGTH)
        return PMD_ERR_ParameterOutOfRange;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Command, address);
    AddSubAction(0); // PMD_Command
    AddSubCommand(0);
    if (timeout > 0xFFFF)
        timeout = 0xFFFF - 1;
    AddWord((WORD)timeout);
    while(nCount-- > 0)
        AddByte(*pdata++);

    result = ProcessPacket(timeout + m_DefaultTimeout);

    if (result == PMD_ERR_OK)
    {
        i = m_rxbufcount - m_rxbufindex;
        pdata = pdataout;
        while(i-- > 0)
            *pdata++ = ExtractByte();
    }

    return result;
}

//*********************************************************************************
// tx byte 1 | PMD_Action_Reset
// tx byte 2 | PMD_Resource_Device
PMDresult PMDResourceProtocol::GetResetCause(WORD resetmask, WORD* resetcause)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Get);
    AddSubAction(PMD_Value_ResetCause);
    AddSubCommand(0);
    AddWord(resetmask);
    
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        *resetcause = ExtractWord();

    return result;
}

//*********************************************************************************
// tx byte 1 | PMD_Action_Reset
// tx byte 2 | PMD_Resource_Device
PMDresult PMDResourceProtocol::GetFaultCode(WORD index, DWORD* code)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Get);
    AddSubAction(PMD_Value_FaultCode);
    AddSubCommand(0);
    AddDWord(index);
    
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        *code = ExtractDWord();

    return result;
}
//*********************************************************************************
// tx byte 1 | PMD_Action_Reset
// tx byte 2 | PMD_Resource_Device
PMDresult PMDResourceProtocol::Reset()
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Reset);
    AddSubAction(0);
    AddSubCommand(0);

    result = ProcessPacket();

    // allow enough time for board to come out of reset
    PMDTaskWait(1000);

    return result;
}

//*********************************************************************************
// tx byte 1 | PMD_Action_Reset
// tx byte 2 | PMD_Resource_MotionProcessor
PMDresult PMDResourceProtocol::ResetMagellan()
{
    FormPacketHeader(PMD_Resource_MotionProcessor, PMD_Action_Reset);
    
    return ProcessPacket();
}

//*********************************************************************************
// tx byte | PMD_Action_Close
// tx byte | PMD_Resource_Peripheral
PMDresult PMDResourceProtocol::ClosePeriph(int address)
{
    FormPacketHeader(PMD_Resource_Peripheral, PMD_Action_Close, address);
    
    return ProcessPacket();
}

//*********************************************************************************
// tx byte | PMD_Action_Close
// tx byte | PMD_Resource_Memory
PMDresult PMDResourceProtocol::CloseMemory(int address)
{
    FormPacketHeader(PMD_Resource_Memory, PMD_Action_Close, address);
    
    return ProcessPacket();
}

//*********************************************************************************
// tx byte | PMD_Action_Send
// tx byte | PMD_Resource_Peripheral
// tx word | timeout
PMDresult PMDResourceProtocol::SendPeriph(int address, char* pbuff, PMDparam nCount, PMDparam timeout)
{
    if (nCount > USER_PACKET_LENGTH)
        return PMD_ERR_ParameterOutOfRange;
    FormPacketHeader(PMD_Resource_Peripheral, PMD_Action_Send, address);
    AddWord((WORD)timeout);
    while(nCount-- > 0)
        AddByte(*pbuff++);
    
    return ProcessPacket(timeout + m_DefaultTimeout);
}

//*********************************************************************************
// tx byte | PMD_Action_Receive
// tx byte | PMD_Resource_Peripheral
// tx word | timeout
PMDresult PMDResourceProtocol::ReceivePeriph(int address, char* pbuff, PMDparam nMaxExpected, PMDparam timeout, PMDparam* nReceived)
{
    PMDresult result;
    PMDparam nBytesRead;
    if (nMaxExpected > USER_PACKET_LENGTH)
        return PMD_ERR_ParameterOutOfRange;

    FormPacketHeader(PMD_Resource_Peripheral, PMD_Action_Receive, address);
    AddWord((WORD)timeout);
    AddWord((WORD)nMaxExpected);
    
    result = ProcessPacket(timeout + m_DefaultTimeout);

    if (result == PMD_ERR_OK)
    {
        nBytesRead = GetNumberBytesRead();
        if (nBytesRead > nMaxExpected)
            nBytesRead = nMaxExpected;
        if (nMaxExpected > 0)
        {
            ExtractBytes((BYTE*)pbuff, nBytesRead);
            *nReceived = nBytesRead;
        }
    }

    return result;
}

//********************************************************
PMDresult PMDResourceProtocol::WritePeriph(int address, WORD* pdata, PMDparam offset_in_bytes, PMDparam words_to_write)
{
    words_to_write = min(words_to_write, MAX_DATA_DWORDS*2);

    FormPacketHeader(PMD_Resource_Peripheral, PMD_Action_Write, address);
    AddByte(PMDDataSize_16Bit);
    AddByte(0);
    AddDWord(offset_in_bytes);
    AddDWord(words_to_write);

    for (PMDparam i = 0; i<words_to_write; i++)
    {
        AddWord(pdata[i]);
    }
    
    return ProcessPacket();
}

//********************************************************
PMDresult PMDResourceProtocol::ReadPeriph(int address, WORD* pdata, PMDparam offset_in_bytes, PMDparam words_to_read)
{
    PMDresult result;
    PMDparam nBytesRead;

    words_to_read = min(words_to_read, MAX_DATA_DWORDS*2);

    FormPacketHeader(PMD_Resource_Peripheral, PMD_Action_Read, address);
    AddByte(PMDDataSize_16Bit);
    AddByte(0);
    AddDWord(offset_in_bytes);
    AddDWord(words_to_read);

    result = ProcessPacket();

    if (result == PMD_ERR_OK)
    {
        nBytesRead = GetNumberBytesRead();
        if (nBytesRead * 2 < words_to_read)
            return PMD_ERR_InsufficientDataReceived;

        for (DWORD i=0; i<words_to_read; i++)
            pdata[i] = ExtractWord();
    }

    return result;
}

//*********************************************************************************
// tx byte | PMD_Action_Send
// tx byte | PMD_Resource_CMotionEngine
// tx word | timeout
PMDresult PMDResourceProtocol::SendCME(BYTE* pbuff, PMDparam nCount, PMDparam timeout)
{
    if (nCount > USER_PACKET_LENGTH)
        return PMD_ERR_ParameterOutOfRange;

    FormPacketHeader(PMD_Resource_CMotionEngine, PMD_Action_Send);
    AddWord((WORD)timeout);
    while(nCount-- > 0)
        AddByte(*pbuff++);
    
    return ProcessPacket(timeout + m_DefaultTimeout);
}

//*********************************************************************************
// tx byte | PMD_Action_Receive
// tx byte | PMD_Resource_CMotionEngine
// tx word | timeout
PMDresult PMDResourceProtocol::ReceiveCME(BYTE* pbuff, PMDparam nMaxExpected, PMDparam timeout, PMDparam* nReceived)
{
    PMDresult result;
    PMDparam nBytesRead;

    if (nMaxExpected > USER_PACKET_LENGTH)
        return PMD_ERR_ParameterOutOfRange;

    FormPacketHeader(PMD_Resource_CMotionEngine, PMD_Action_Receive);
    AddWord((WORD)timeout);
    
    result = ProcessPacket(timeout + m_DefaultTimeout);
    if (result == PMD_ERR_OK)
    {
        nBytesRead = GetNumberBytesRead();
        if (nBytesRead > nMaxExpected)
            nBytesRead = nMaxExpected;
        ExtractBytes((BYTE*)pbuff, nBytesRead);
        *nReceived = nBytesRead;
    }
    return result;
}

// # dwords sent must be 32 byte aligned for the NVRAM minimum 256-bit requirement.
#define MAX_DATA_32BYTE_ALIGNED_DWORDS ((MAX_DATA_DWORDS/8)*8)
//********************************************************
PMDresult PMDResourceProtocol::WriteMemory(int address, DWORD* data, PMDparam offset_in_dwords, PMDparam dwords_to_write)
{
    PMDresult result = PMD_ERR_ParameterOutOfRange;
    DWORD dwords_to_write_packet;
    long dwords_to_write_remaining = dwords_to_write;

    while (dwords_to_write_remaining > 0)
    {
        dwords_to_write_packet = min(dwords_to_write_remaining, MAX_DATA_32BYTE_ALIGNED_DWORDS);
        FormPacketHeader(PMD_Resource_Memory, PMD_Action_Write, address);
        AddWord(PMDDataSize_32Bit);
        AddDWord(offset_in_dwords);
        AddDWord(dwords_to_write_packet);

        for (PMDparam i = 0; i<dwords_to_write_packet; i++)
        {
            AddDWord(data[i]);
        }

        result = ProcessPacket();

        if (result == PMD_ERR_OK)
        {
            data += dwords_to_write_packet;
            offset_in_dwords += dwords_to_write_packet;
            dwords_to_write_remaining -= dwords_to_write_packet;
        }
        else
            break;
    }
    return result;
}

//********************************************************
PMDresult PMDResourceProtocol::ReadMemory(int address, DWORD* data, PMDparam offset_in_dwords, PMDparam dwords_to_read)
{
    PMDresult result = PMD_ERR_ParameterOutOfRange;
    DWORD dwords_to_read_packet;
    long dwords_to_read_remaining = dwords_to_read;

    while (dwords_to_read_remaining > 0)
    {
        dwords_to_read_packet = min(dwords_to_read_remaining, MAX_DATA_DWORDS);
        FormPacketHeader(PMD_Resource_Memory, PMD_Action_Read, address);
        AddWord(PMDDataSize_32Bit);
        AddDWord(offset_in_dwords);
        AddDWord(dwords_to_read_packet);

        result = ProcessPacket();

        if (result == PMD_ERR_OK)
        {
            // retrieve data
            ExtractDWords(data, dwords_to_read_packet);
            data += dwords_to_read_packet;
            offset_in_dwords += dwords_to_read_packet;
            dwords_to_read_remaining -= dwords_to_read_packet;
        }
        else
            break;
    }
    
    return result;
}

//********************************************************
PMDresult PMDResourceProtocol::WriteMemory(int address, WORD* data, PMDparam offset_in_words, PMDparam words_to_write)
{
    PMDresult result = PMD_ERR_ParameterOutOfRange;
    PMDparam words_to_write_packet;
    long words_to_write_remaining = words_to_write;
    words_to_write = min(words_to_write, MAX_DATA_DWORDS*2);

    while (words_to_write_remaining > 0)
    {
        words_to_write_packet = min(words_to_write_remaining, MAX_DATA_DWORDS*2);
        FormPacketHeader(PMD_Resource_Memory, PMD_Action_Write, address);
        AddWord(PMDDataSize_16Bit);
        AddDWord(offset_in_words);
        AddDWord(words_to_write_packet);

        for (PMDparam i = 0; i<words_to_write_packet; i++)
        {
            AddWord(data[i]);
        }

        result = ProcessPacket();

        if (result == PMD_ERR_OK)
        {
            data += words_to_write_packet;
            offset_in_words += words_to_write_packet;
            words_to_write_remaining -= words_to_write_packet;
        }
        else
            break;
    }

    return result;
}

//********************************************************
PMDresult PMDResourceProtocol::ReadMemory(int address, WORD* data, PMDparam offset_in_words, PMDparam words_to_read)
{
    PMDresult result = PMD_ERR_ParameterOutOfRange;
    PMDparam words_to_read_packet;
    long words_to_read_remaining = words_to_read;

    while (words_to_read_remaining > 0)
    {
        words_to_read_packet = min(words_to_read_remaining, MAX_DATA_DWORDS*2);
        FormPacketHeader(PMD_Resource_Memory, PMD_Action_Read, address);
        AddWord(PMDDataSize_32Bit);
        AddDWord(offset_in_words);
        AddDWord(words_to_read_packet);

        result = ProcessPacket();

        if (result == PMD_ERR_OK)
        {
            ExtractWords(data, words_to_read_packet);
            data += words_to_read_packet;
            offset_in_words += words_to_read_packet;
            words_to_read_remaining -= words_to_read_packet;
        }
        else
            break;
    }
    
    return result;
}

//********************************************************
PMDresult PMDResourceProtocol::EraseMemory(int address)
{
    FormPacketHeader(PMD_Resource_Memory, PMD_Action_Clear, address);
    
    return ProcessPacket(6000);
}

//*********************************************************************************
// tx byte | PMD_Action_Open
// tx byte | PMD_Resource_Device
// tx word | portid
// rx word | periphaddress - this is the address returned from and Open Peripheral action
PMDresult PMDResourceProtocol::OpenPeriphSerial(int* periphaddress, int portno, int baud, int parity, int stopbits)
{
    PMDresult result;
    WORD serialmode;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Open);
    AddSubAction(PMD_Open_PeriphCOM);
    AddSubCommand(portno);
     // form the Magellan SetSerialPort word.
    if (baud <= PMDSerialBaud460800) // support the use of PMDSerialBaud enum instead of actual baud.
        SET_SERIALPORTMODE(serialmode, baud, parity, stopbits, 0, 0)
    else
        SET_SERIALPORTMODE(serialmode, 0, parity, stopbits, 0, 0)
    AddWord(serialmode);
    if (baud > PMDSerialBaud460800)
        AddWord(baud / 1200); //baud is sent as a separate 16-bit word to allow for more than the 8 Magelan baud rates
    
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        *periphaddress = ExtractByte();

    return result;
}

//*********************************************************************************
// tx byte | PMD_Action_Open
// tx byte | PMD_Resource_Device
// tx word | portid
// rx word | periphaddress - this is the address returned from and Open Peripheral action
PMDresult PMDResourceProtocol::OpenPeriphCAN(int* periphaddress, int portno, PMDparam baud, PMDparam transmitid, PMDparam receiveid, PMDparam eventid)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Open);
    AddSubAction(PMD_Open_PeriphCAN);
    AddSubCommand(portno);
    AddDWord(transmitid);
    AddDWord(receiveid);
    AddDWord(eventid);
    AddDWord(baud);
    
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        *periphaddress = ExtractByte();

    return result;
}

//*********************************************************************************
// tx byte | PMD_Action_Open
// tx byte | PMD_Resource_Device
// tx word | portid
// rx word | periphaddress - this is the address returned from and Open Peripheral action
PMDresult PMDResourceProtocol::OpenPeriphSPI(int* periphaddress, BYTE port, BYTE chipselect, BYTE mode, BYTE datasize, DWORD bitrate)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Open);
    AddSubAction(PMD_Open_PeriphSPI);
    AddSubCommand(port);
    AddByte(chipselect);
    AddByte(mode);
    AddByte(datasize);
    AddByte(0);
    AddDWord(bitrate);
    
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        *periphaddress = ExtractByte();

    return result;
}

//*********************************************************************************
// tx byte | PMD_Action_Open
// tx byte | PMD_Resource_Device
// tx word | portid
// rx word | periphaddress - this is the address returned from and Open Peripheral action
PMDresult PMDResourceProtocol::OpenPeriphUDP(int* periphaddress, DWORD ipaddress, DWORD port)
{
    PMDresult result;
    BYTE portno = 0;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Open);
    AddSubAction(PMD_Open_PeriphUDP);
    AddSubCommand(portno);
    AddDWord(ipaddress);
    AddDWord(port);
    
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        *periphaddress = ExtractByte();

    return result;
}

//*********************************************************************************
// tx byte | PMD_Action_Open
// tx byte | PMD_Resource_Device
// tx word | portid
// rx word | periphaddress - this is the address returned from and Open Peripheral action
PMDresult PMDResourceProtocol::OpenPeriphTCP(int* periphaddress, DWORD ipaddress, DWORD port)
{
    PMDresult result;
    BYTE portno = 0;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Open);
    AddSubAction(PMD_Open_PeriphTCP);
    AddSubCommand(portno);
    AddDWord(ipaddress);
    AddDWord(port);

    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        *periphaddress = ExtractByte();

    return result;
}

//*********************************************************************************
// tx byte | PMD_Action_Open
// tx byte | PMD_Resource_Device
// tx word | 16-bit start address
// rx word | periphaddress - this is the address returned from and Open Peripheral action
PMDresult PMDResourceProtocol::OpenPeriphISA(int* periphaddress, WORD address, BYTE irq, BYTE datasize)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Open);
    AddSubAction(PMD_Open_PeriphISA);
    AddSubCommand(0);
    AddWord(address);
    AddByte(irq);
    AddByte(datasize); // 1,2,4
    
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        *periphaddress = ExtractByte();

    return result;
}

//*********************************************************************************
// tx byte | PMD_Action_Open
// tx byte | PMD_Resource_Device
// tx word | 16-bit start address
// rx word | periphaddress - this is the address returned from and Open Peripheral action
PMDresult PMDResourceProtocol::OpenPeriphPAR(int* periphaddress, WORD address, BYTE irq, BYTE datasize)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Open);
    AddSubAction(PMD_Open_PeriphPAR);
    AddSubCommand(0);
    AddWord(address);
    AddByte(irq);
    AddByte(datasize); // 1,2,4
    
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        *periphaddress = ExtractByte();

    return result;
}

//*********************************************************************************
// tx byte | PMD_Action_Open
// tx byte | PMD_Resource_Device
// tx byte | PMD_Open_Memory
// tx byte | memorytype
// tx byte | datasize
// rx word | resourceaddress - this is the address returned from and Open Memory action
PMDresult PMDResourceProtocol::OpenMemory(int* resourceaddress, int* memsize, PMDDataSize datasize, PMDMemoryType memorytype)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Open);
    AddSubAction(PMD_Open_Memory);
    AddSubCommand(memorytype);
    AddByte(datasize);
    
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
    {
        *resourceaddress = ExtractDWord();
        *memsize = ExtractDWord();
    }

    return result;
}

//*********************************************************************************
// tx byte | PMD_Action_Open
// tx byte | PMD_Resource_Device
// tx byte | PMD_Open_Memory
// tx byte | memorytype
// tx byte | datasize
// rx word | resourceaddress - this is the address returned from and Open Memory action
PMDresult PMDResourceProtocol::OpenMotionProcessor(int* resourceaddress, int periphaddress)
{
    PMDresult result;

	FormPacketHeader(PMD_Resource_Peripheral, PMD_Action_Open, periphaddress);
	AddSubAction(PMD_Open_MotionProcessor);

    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        *resourceaddress = ExtractByte();  // return MotionProcessor address

    return result;
}

PMDresult PMDResourceProtocol::CloseMotionProcessor(int address)
{
	FormPacketHeader(PMD_Resource_MotionProcessor, PMD_Action_Close, address);

    return ProcessPacket();
}

//*********************************************************************************
// tx byte | PMD_Action_Open
// tx byte | PMD_Resource_Device
// tx byte | PMD_Open_Memory
// tx byte | memorytype
// tx byte | datasize
// rx word | resourceaddress - this is the address returned from and Open Memory action
PMDresult PMDResourceProtocol::OpenRPDevice(int* resourceaddress, int periphaddress)
{
    PMDresult result;

	FormPacketHeader(PMD_Resource_Peripheral, PMD_Action_Open, periphaddress);
	AddSubAction(PMD_Open_Device);
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        *resourceaddress = ExtractByte();  // return Resource address

    return result;
}

PMDresult PMDResourceProtocol::CloseDevice(int address)
{   
	FormPacketHeader(PMD_Resource_Device, PMD_Action_Close, address);

    return ProcessPacket();
}


//*********************************************************************************
// tx byte | PMD_Action_Open
// tx byte | PMD_Resource_Device
// tx byte | PMD_Open_Memory
// tx byte | memorytype
// tx byte | datasize
// rx word | resourceaddress - this is the address returned from and Open Memory action
PMDresult PMDResourceProtocol::OpenMailbox(int* resourceaddress, int mailboxID, int depth, int itemsize)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Open);
    AddSubAction(PMD_Open_Mailbox);
    AddSubCommand(mailboxID);
    AddWord(depth);
    AddWord(itemsize);
    
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        *resourceaddress = ExtractByte();

    return result;
}

//*********************************************************************************
// tx byte | PMD_Action_Send
// tx byte | PMD_Resource_Device
// tx word | timeout
PMDresult PMDResourceProtocol::SendMail(int mailboxID, const void* pItem, int itemsize, PMDparam timeout)
{
    BYTE* pbuff = (BYTE*)pItem;

    if (itemsize > USER_PACKET_LENGTH)
        return PMD_ERR_ParameterOutOfRange;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Send);
    AddWord((WORD)timeout);
    AddWord(mailboxID);
    while(itemsize-- > 0)
        AddByte(*pbuff++);
    
    return ProcessPacket(timeout + m_DefaultTimeout);
}

//*********************************************************************************
// tx byte | PMD_Action_Receive
// tx byte | PMD_Resource_Peripheral
// tx word | timeout
PMDresult PMDResourceProtocol::ReceiveMail(int mailboxID, void* pItem, int itemsize, PMDparam timeout)
{
    BYTE* pbuff = (BYTE*)pItem;
    int nBytesRead;
    PMDresult result;
    if (itemsize > USER_PACKET_LENGTH)
        return PMD_ERR_ParameterOutOfRange;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Receive);
    AddWord((WORD)timeout);
    AddWord(mailboxID);
    
    result = ProcessPacket(timeout + m_DefaultTimeout);

    if (result == PMD_ERR_OK)
    {
        nBytesRead = GetNumberBytesRead();
        if (nBytesRead < itemsize)
            result = PMD_ERR_InsufficientDataReceived;
        if (nBytesRead > itemsize)
            return PMD_ERR_UnexpectedDataReceived;
        ExtractBytes(pbuff, nBytesRead);
    }
    return result;
}

//*********************************************************************************
// tx byte | PMD_Action_Receive
// tx byte | PMD_Resource_Peripheral
// tx word | timeout
PMDresult PMDResourceProtocol::PeekMail(int mailboxID, void* pItem, int itemsize, PMDparam timeout)
{
    BYTE* pbuff = (BYTE*)pItem;
    int nBytesRead;
    PMDresult result;
    if (itemsize > USER_PACKET_LENGTH)
        return PMD_ERR_ParameterOutOfRange;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Read);
    AddWord(mailboxID);
    AddWord((WORD)timeout);
    
    result = ProcessPacket(timeout + m_DefaultTimeout);

    if (result == PMD_ERR_OK)
    {
        nBytesRead = GetNumberBytesRead();
        if (nBytesRead < itemsize)
            result = PMD_ERR_InsufficientDataReceived;
        if (nBytesRead > itemsize)
            return PMD_ERR_UnexpectedDataReceived;
        ExtractBytes(pbuff, nBytesRead);
    }

    return result;
}

//*********************************************************************************
// tx byte | PMD_Action_Set
// tx byte | PMD_Resource_CMotionEngine
// tx word | PMD_Value_Console
// tx word | periphaddress - this is the address returned from and Open Peripheral action
PMDresult PMDResourceProtocol::SetConsole(int periphaddress)
{
    FormPacketHeader(PMD_Resource_CMotionEngine, PMD_Action_Set);
    AddSubAction(PMD_Value_Console);
    AddSubCommand(periphaddress);
    
    return ProcessPacket();
}

//*********************************************************************************
// tx byte 1 | PMD_Action_Get
// tx byte 2 | PMD_Resource_Device
// tx byte 3 | PMD_Value_Default
// tx byte 4 | 0
// tx word  | default id
// tx words  | default value
PMDresult PMDResourceProtocol::GetDefault(DWORD dwParamNo, DWORD* dwParam)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Get);
    AddSubAction(PMD_Value_Default);
    AddSubCommand(0);
    AddDWord(dwParamNo);
    
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        *dwParam = ExtractDWord();

    return result;
}

//*********************************************************************************
// tx byte 1 | PMD_Action_Set
// tx byte 2 | PMD_Resource_Device
// tx byte 3 | PMD_Value_Default
// tx byte 4 | 0
// tx word  | default id
// tx words  | default value
//
PMDresult PMDResourceProtocol::SetDefault(DWORD dwParamNo, DWORD dwParam)
{
    FormPacketHeader(PMD_Resource_Device, PMD_Action_Set);
    AddSubAction(PMD_Value_Default);
    AddSubCommand(0);
    AddDWord(dwParamNo);
    AddDWord(dwParam);
    
    return ProcessPacket(2000); // need a longer than normal timeout value to accomodate
                   // the off chance that the NVRAM needs to be formatted.
}

//*********************************************************************************
// tx byte 1 | PMD_Action_Command
// tx byte 2 | PMD_Resource_CMotionEngine
// tx byte 3 | PMD_Command_TaskControl
// tx byte 4 | PMD_TaskControl_Start
PMDresult PMDResourceProtocol::StartUserTasks(DWORD taskparam)
{
    FormPacketHeader(PMD_Resource_CMotionEngine, PMD_Action_Command);
    AddSubAction(PMD_Command_TaskControl);
    AddSubCommand(PMD_TaskControl_StartAll);
    AddDWord(taskparam);
    
    return ProcessPacket();
}
//*********************************************************************************
// tx byte 1 | PMD_Action_Command
// tx byte 2 | PMD_Resource_CMotionEngine
// tx byte 3 | PMD_Command_TaskControl
// tx byte 4 | PMD_TaskControl_Start
PMDresult PMDResourceProtocol::StopUserTasks()
{
    FormPacketHeader(PMD_Resource_CMotionEngine, PMD_Action_Command);
    AddSubAction(PMD_Command_TaskControl);
    AddSubCommand(PMD_TaskControl_StopAll);
    
    return ProcessPacket();
}

//*********************************************************************************
// tx byte | PMD_Action_Get
// tx byte | PMD_Resource_CMotionEngine
// tx byte | PMD_Value_FileName
// rx bytes | filename
PMDresult PMDResourceProtocol::GetUserCodeFileName(char* filename)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_CMotionEngine, PMD_Action_Get);
    AddSubAction(PMD_Value_FileName);
    
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        ExtractString(filename);

    return result;
}

//*********************************************************************************
PMDresult PMDResourceProtocol::GetUserCodeFileDate(char* filedate)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_CMotionEngine, PMD_Action_Get);
    AddSubAction(PMD_Value_FileDate);
    
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        ExtractString(filedate);

    return result;
}

//*********************************************************************************
PMDresult PMDResourceProtocol::GetUserCodeFileChecksum(DWORD* checksum)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_CMotionEngine, PMD_Action_Get);
    AddSubAction(PMD_Value_FileChecksum);
    
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        *checksum = ExtractDWord();

    return result;
}

//*********************************************************************************
PMDresult PMDResourceProtocol::GetUserCodeFileVersion(DWORD* version)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_CMotionEngine, PMD_Action_Get);
    AddSubAction(PMD_Value_FileVersion);
    
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
        *version = ExtractDWord();

    return result;
}

//*********************************************************************************
// tx byte | PMD_Action_Get
// tx byte | PMD_Resource_CMotionEngine
// tx byte | PMD_Value_TaskState
// rx dword | 
PMDresult PMDResourceProtocol::GetUserCodeStatus(int taskno, PMDint32* taskstate)
{
    return GetTaskInfo(taskno, PMDTaskInfo_State, taskstate);
}

//*********************************************************************************
// tx byte | PMD_Action_Get
// tx byte | PMD_Resource_CMotionEngine
// tx byte | PMD_Value_TaskInfo
// rx dword | 
PMDresult PMDResourceProtocol::GetTaskInfo(int taskno, PMDTaskInfo ID, PMDint32* info)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_CMotionEngine, PMD_Action_Get);
    AddSubAction(PMD_Value_TaskState);
    AddSubCommand(taskno);
    AddDWord(ID);
    
    result = ProcessPacket();

    if (result == PMD_ERR_OK)
        *info = (PMDint32)ExtractDWord();

    return result;
}

//*********************************************************************************
// tx byte 1 | PMD_Action_Get
// tx byte 2 | PMD_Resource_Device
// tx byte 3 | PMD_Value_Version
PMDresult PMDResourceProtocol::GetFirmwareVersion(DWORD* version)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Get);
    AddSubAction(PMD_Value_Version);
    AddSubCommand(0);
    
    result = ProcessPacket();

    if (result == PMD_ERR_OK)
        *version = ExtractDWord();

    return result;
}

//*********************************************************************************
// tx byte 1 | PMD_Action_Get
// tx byte 2 | PMD_Resource_Device
// tx byte 3 | PMD_Value_Version
PMDresult PMDResourceProtocol::GetDeviceInfo(PMDDeviceInfo ID, WORD option, DWORD* info)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Get);
    AddSubAction(PMD_Value_Version);
    AddSubCommand(ID);
    AddWord(option);
    
    result = ProcessPacket();

    if (result == PMD_ERR_OK)
        *info = ExtractDWord();

    return result;
}

//*********************************************************************************
// tx byte | PMD_Action_Get
// tx byte | PMD_Resource_Device
// tx byte | PMD_Value_Time
// rx dword | 
PMDresult PMDResourceProtocol::GetSystemTime(SYSTEMTIME* time)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Get);
    AddSubAction(PMD_Value_Time);
    
    result = ProcessPacket();
    if (result == PMD_ERR_OK)
    {
        time->wYear   = ExtractWord();
        time->wMonth  = ExtractWord();
        time->wDayOfWeek = ExtractWord();
        time->wDay   = ExtractWord();
        time->wHour   = ExtractWord();
        time->wMinute  = ExtractWord();
        time->wSecond  = ExtractWord();
        time->wMilliseconds = ExtractWord();
    }

    return result;
}

//*********************************************************************************
// tx byte | PMD_Action_Set
// tx byte | PMD_Resource_Device
// tx byte | PMD_Value_Time
// rx dword | 
PMDresult PMDResourceProtocol::SetSystemTime(const SYSTEMTIME* time)
{
    PMDresult result;

    FormPacketHeader(PMD_Resource_Device, PMD_Action_Set);
    AddSubAction(PMD_Value_Time);
    AddSubCommand(0);
    
    AddWord(time->wYear   );
    AddWord(time->wMonth  );
    AddWord(time->wDayOfWeek );
    AddWord(time->wDay   );
    AddWord(time->wHour   );
    AddWord(time->wMinute  );
    AddWord(time->wSecond  );
    AddWord(time->wMilliseconds );

    result = ProcessPacket();

    return result;
}

//*********************************************************************************
// tx byte 1 | PMD_Action_Set
// tx byte 2 | PMD_Resource_Device
// tx byte 3 | PMD_Value_NodeID
// tx byte 4 | 0
// tx byte 5 | nodeId
// tx byte 6 | DOsignal
// tx byte 7 | DIsignal
// tx byte 8 | DIsense
PMDresult PMDResourceProtocol::SetNodeID(BYTE nodeId, BYTE DOsignal, BYTE DIsignal, BYTE DIsense)
{
    FormPacketHeader(PMD_Resource_Device, PMD_Action_Set);
    AddSubAction(PMD_Value_NodeID);
    AddSubCommand(0);
    AddByte(nodeId);
    AddByte(DOsignal);
    AddByte(DIsignal);
    AddByte(DIsense);
    
    return ProcessPacket();
}



//*********************************************************************************
// tx byte 1 | PMD_Action_Command
// tx byte 2 | PMD_Resource_CMotionEngine
// tx byte 3 | PMD_Command_Flash
// tx byte 4 | PMD_Flash_Erase
PMDresult PMDResourceProtocol::EraseUserCode()
{
    FormPacketHeader(PMD_Resource_CMotionEngine, PMD_Action_Command);
    AddSubAction(PMD_Command_Flash);
    AddSubCommand(PMD_Flash_Erase);
    
    return ProcessPacket(6000);
}

//*********************************************************************************
// StoreUserCodeBegin is called to prepare the CME to receive the user code binary file.
// This command may take several seconds so the communications timeout must be increased accordingly
PMDresult PMDResourceProtocol::StoreUserCodeBegin(int filesize)
{
    FormPacketHeader(PMD_Resource_CMotionEngine, PMD_Action_Command);
    AddSubAction(PMD_Command_Flash);
    AddSubCommand(PMD_Flash_Start);
    AddDWord(filesize);
    
    return ProcessPacket(6000);
}

//*********************************************************************************
// StoreUserCodeData is called for each contiguous block of the user code binary file.
// length must be even except for last buffer
// length must not exceed MAX_PACKET_DATA_LENGTH
PMDresult PMDResourceProtocol::StoreUserCodeData(BYTE* pdata, int length)
{
    FormPacketHeader(PMD_Resource_CMotionEngine, PMD_Action_Command);
    AddSubAction(PMD_Command_Flash);
    AddSubCommand(PMD_Flash_Data);
    for (int i=0; i<length; i++)
        AddByte(pdata[i]);
    
    return ProcessPacket();
}

//*********************************************************************************
// StoreUserCodeEnd is called to complete the download process.
PMDresult PMDResourceProtocol::StoreUserCodeEnd()
{
    FormPacketHeader(PMD_Resource_CMotionEngine, PMD_Action_Command);
    AddSubAction(PMD_Command_Flash);
    AddSubCommand(PMD_Flash_End);
    
    return ProcessPacket();
}

#ifdef WINDOWS
#pragma warning(pop)
#endif
