//  PMDspi.c -- Motion Processor SPI protocol functions 
//
//  Performance Motion Devices, Inc.
//

#include "PMDperiph.h"
#include "PMDecode.h"
#include "PMDsys.h"
#include "PMDPfunc.h"

#define Printf PMDprintf
PMDMutexDefine(xMutexSPI)


/*  
Sending a command
word    Host                        MC58113
------------------------------------------------------
        Assert ~HostSPIEnable   
1       8b axis 8b opcode           0
2       8b rsv  8b checksum         0
3       16b argument 1 (optional)   0
4       16b argument 2 (optional)   0
5       16b argument 3 (optional)   0
        De-assert ~HostSPIEnable    

        wait for HostSPIReady signal

Receiving a response
word    Host                        MC58113
------------------------------------------------------
        Assert ~HostSPIEnable   
1       0                           8b checksum  8b command status
2       0                           16b response 1 (optional)
3       0                           16b response 2 (optional)
4       0                           16b response 3 (optional)
        De-assert ~HostSPIEnable    
*/

#define MP_TIMEOUT         100

PMDresult PMDSPI_Send(void* transport_data, PMDuint8 xCt, PMDuint16* xDat, PMDuint8 rCt, PMDuint16* rDat)
{
    PMDDeviceHandle* hDevice = (PMDDeviceHandle*)transport_data;
    PMDPeriphHandle* hPeriph = hDevice->hPeriph;

    union buff 
    {
        WORD  w[8];
        BYTE  b[16];
    } txbuff, rxbuff;

    PMDresult result = PMD_ERR_OK;
    int  ProcessorError = 0;
    char  nbytes;
    char  nwords;
    int  i;
    int bVerbose = 0;
    int bByteSwap = (hPeriph->param == 8); // param is datasize in bits. We need to byteswap the data if sending in 8-bit SPI mode.
    WORD sum;
    WORD carry;
    PMDuint32 WriteSize;
    PMDuint8* WriteData = txbuff.b;
    const PMDuint8  Nop[8] = {0,0,0,0,0,0,0,0};
    PMDuint32 ReadSize;
    PMDuint32 nRead;
    PMDuint8* ReadData = rxbuff.b;

    // The 16-bit words sent to the chip is in big endian format.
    txbuff.w[0] = xDat[0];  // axis and opcode bytes
    txbuff.w[1] = 0;        // reserved and checksum bytes
    txbuff.w[2] = xDat[1];
    txbuff.w[3] = xDat[2];
    txbuff.w[4] = xDat[3];

    nwords = xCt + 1;
    nbytes = nwords * 2;

    // calculate checksum
    sum = 0xAA; // seed
    for( i=0; i<nbytes; i++ ) 
        sum += txbuff.b[i];

    carry = sum >> 8;
    sum = sum & 0xFF;
    sum += carry;
    carry = sum >> 8;
    sum = sum & 0xFF;
    sum += carry;
    sum = ~sum;
    txbuff.b[2] = (PMDuint8)sum; // checksum byte

    // byte swap data if SPI datasize is 8 bit
	if (bByteSwap)
    {
        for( i=0; i<nbytes; i+=2 )
        {
            char temp  = txbuff.b[i];
            txbuff.b[i] = txbuff.b[i+1];
            txbuff.b[i+1] = temp;
        }
    }

    if (!PMDMutexLock(xMutexSPI))
      return PMD_ERR_MutexTimeout;

    // if the HostSPIStatus signal is already low, we're out of sync 
    // so do a read of any number of words to resync. 
    // nExpected of 0 is a special case that calls PMDPSPI_WaitUntilReady
    result = PMDPeriphReceive(hPeriph, NULL, &nRead, 0, 0);
    
    if (result == PMD_ERR_OK)
    {
        WriteSize = 2;
        Printf("SPI resync\n");
        result = PMDPeriphSend(hPeriph, Nop, WriteSize, MP_TIMEOUT);
    }

    if (bVerbose) {
         Printf("     send:");
         for (i = 0; i < nwords; i++)
             Printf(" %04X", txbuff.w[i]);
         Printf("\n");
    }

    // Send the command
    WriteSize = nbytes;
    result = PMDPeriphSend(hPeriph, WriteData, WriteSize, MP_TIMEOUT);

    // nExpected of 0 is a special case that calls PMDPSPI_WaitUntilReady
    result = PMDPeriphReceive(hPeriph, NULL, NULL, 0, 1);

    ReadSize = (rCt + 1) * 2; // +1 to include checksum word
    if (result == PMD_ERR_OK)
    {
        result = PMDPeriphSend(hPeriph, Nop, ReadSize, MP_TIMEOUT);
        result = PMDPeriphReceive(hPeriph, ReadData, &nRead, ReadSize, MP_TIMEOUT);
    }
    
    if (result == PMD_ERR_OK)
    {
        // byte swap return data if SPI datasize is 8 bit
        if (bByteSwap)
        {
            for( i=0; i<(int)ReadSize; i+=2 )
            {
                BYTE temp = rxbuff.b[i];
                rxbuff.b[i] = rxbuff.b[i+1];
                rxbuff.b[i+1] = temp;
            }
        }

        if (bVerbose)
        {
            Printf("  receive:");
            for (i = 0; 2*i < (int)nwords; i++)
                Printf(" %04X", rxbuff.w[i]);
            Printf("\n");
        }

        // first byte in response is the motion processor command status 
        // a positive command status is # result words that are available 
        ProcessorError = 0;
        if ((PMDint8)(rxbuff.b[0]) < 0)
        {
            ProcessorError = -(char)(rxbuff.b[0]);
        }
        // invalid packet if # bytes is > 3
        if (rxbuff.b[0] > 3)
            return PMD_ERR_UnexpectedDataReceived;

        sum = 0xAA;
        for( i=0; i<(int)ReadSize; i++ )
            sum += rxbuff.b[i];
        carry = sum >> 8;
        sum = sum & 0xFF;
        sum += carry;
        carry = sum >> 8;
        sum = sum & 0xFF;
        sum += carry;

        if( sum != 0xFF)
        {
            Printf("\r\nChecksum error: (opcode 0x%02X)", xDat[0]>>8 );
            for(i=0; i<(int)ReadSize; i++ )
                Printf("  %04X", rxbuff.w[i] );
            Printf(" S: %02X\r\n", sum );

            result = PMD_ERR_Checksum;
        }

        // copy return data skipping checksum word
        for( i=0; i<rCt; i++ )
        {
            rDat[i] = rxbuff.w[i+1];
        }
    }
    PMDMutexUnlock(xMutexSPI);
    
    // report error if one occured
    if( ProcessorError )
        return (PMDErrorCode)ProcessorError;

    return result;
}

//********************************************************
PMDresult PMDSPI_Init()
{
    xMutexSPI = PMDMutexCreate();

    return PMD_NOERROR;
}

