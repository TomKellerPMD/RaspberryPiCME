//
//  PMDPSerRapian.c -- Rasperry Pi Linux serial interface command/data transfer functions
//  Replaces PMDPser.c in CMEDSDK libary
// 
//  Performance Motion Devices, Inc.
//

#include <stdio.h>
#include <stdlib.h>
//#include <windows.h>
#include <termios.h>		//Used for UART
#include <sys/ioctl.h>
#include <errno.h>


#include "PMDtypes.h"
#include "PMDecode.h"
#include "PMDperiph.h"
#include "PMDPfunc.h"
#include "PMDsys.h"

// only need to include this if diagnostics mode is used
#include "PMDdiag.h"
#define error_message printf
#define GetLastError() errno

//static BYTE  parList[] = { NOPARITY, ODDPARITY, EVENPARITY, MARKPARITY, SPACEPARITY };
//static BYTE stopList[] = { ONESTOPBIT, TWOSTOPBITS };
//static long baudList[] = { 1200, 2400, 9600, 19200, 57600, 115200, 230400, 460800 };


void FormatGetLastError()
{
  
  //  LPTSTR lpMsgBuf;
  return;
  
   // DWORD dw = GetLastError(); 
   // return dw;
  
   /* FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dw,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );
    LocalFree(lpMsgBuf);
    */ 
    //return err;
}

//********************************************************
static PMDresult PMDPCOM_Close(PMDPeriphHandle* hPeriph)
{
    if ( hPeriph->handle != INVALID_HANDLE_VALUE )
    {
        close(hPeriph->handle);
        flock(hPeriph->handle, LOCK_UN); /* free the port */
        //CloseHandle( hPeriph->handle );
        hPeriph->handle = INVALID_HANDLE_VALUE;
    }
    free(hPeriph);
    return PMD_ERR_OK;
}

//********************************************************
//PMDresult PMDPCOM_SetConfig(PMDPeriphHandle* hPeriph, PMDparam baud, PMDSerialParity parity, PMDSerialStopBits stopbits)
PMDresult PMDPCOM_SetConfig(PMDPeriphHandle* hPeriph, PMDparam baud, PMDparam parity, PMDparam stopbits)
{
//    DCB dcb;

    PMD_PERIPHCONNECTED(hPeriph)

    int test;
    struct termios tty;
    int baudr;
    //void * portptr;
    //portptr=(int *) hPeriph->handle;
    
   
   // PMDSerialIOData* SIOtransport_data = (PMDSerialIOData*)transport_data;

    //SIOtransport_data->baud = baud;
    //SIOtransport_data->parity = parity;
    //SIOtransport_data->stop = ONESTOPBIT;

   // if (SIOtransport_data->hPort == INVALID_HANDLE_VALUE)
    //    return FALSE;

    memset(&tty, 0, sizeof tty);
    if (tcgetattr(hPeriph->handle, &tty) != 0)
    if ((errno = tcgetattr(hPeriph->handle, &tty)) != 0)
    {
            error_message("error %d from tcgetattr", errno);
            return FALSE;
    }

  //  printf("baud=%d\n", baud);
    switch (baud)
    {
    case    2400:
        baudr = B2400;
        break;
    case    4800:
        baudr = B4800;
        break;
    case    9600:
        baudr = B9600;
        break;
    case   19200:
        baudr = B19200;
        break;
    case   57600:
        baudr = B57600;
        break;
    case  115200:
        baudr = B115200;
        break;
    case  230400:
        baudr = B230400;
        break;
    case  460800:
        baudr = B460800;
        break;

    default:
        error_message("invalid baudrate\n");
        return FALSE;
        break;
    }

    int cbits = CS8,
        cpar = 0,
        ipar = IGNPAR,
        bstop = 0;

    switch (parity)
    {
    case PMDSerialParityNone:
        cpar = 0;
        ipar = IGNPAR;
        break;
    case PMDSerialParityEven:
        cpar = PARENB;
        ipar = INPCK;
        break;
    case PMDSerialParityOdd:
        cpar = (PARENB | PARODD);
        ipar = INPCK;
        break;
    default:
        error_message("invalid parity '%c'\n", parity);
        return FALSE;
        break;
    }

    cfsetospeed(&tty, baudr);
    cfsetispeed(&tty, baudr);

    tty.c_cflag = cbits | cpar | bstop;
    tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls, enable reading
    tty.c_cflag |= baudr;
    tty.c_iflag = ipar;
    tty.c_oflag = 0;
    tty.c_lflag = 0;
    /*
    VMIN = 0 and VTIME = 0
        This is a completely non-blocking read - the call is satisfied immediately directly from the driver's input queue. If data are available, it's transferred to the caller's buffer up to nbytes and returned. Otherwise zero is immediately returned to indicate "no data". We'll note that this is "polling" of the serial port, and it's almost always a bad idea. If done repeatedly, it can consume enormous amounts of processor time and is highly inefficient. Don't use this mode unless you really, really know what you're doing.
    VMIN = 0 and VTIME > 0
        This is a pure timed read. If data are available in the input queue, it's transferred to the caller's buffer up to a maximum of nbytes, and returned immediately to the caller. Otherwise the driver blocks until data arrives, or when VTIME tenths expire from the start of the call. If the timer expires without data, zero is returned. A single byte is sufficient to satisfy this read call, but if more is available in the input queue, it's returned to the caller. Note that this is an overall timer, not an intercharacter one.
    VMIN > 0 and VTIME > 0
        A read() is satisfied when either VMIN characters have been transferred to the caller's buffer, or when VTIME tenths expire between characters. Since this timer is not started until the first character arrives, this call can block indefinitely if the serial line is idle. This is the most common mode of operation, and we consider VTIME to be an intercharacter timeout, not an overall one. This call should never return zero bytes read.
    VMIN > 0 and VTIME = 0
        This is a counted read that is satisfied only when at least VMIN characters have been transferred to the caller's buffer - there is no timing component involved. This read can be satisfied from the driver's input queue (where the call could return immediately), or by waiting for new data to arrive: in this respect the call could block indefinitely. We believe that it's undefined behavior if nbytes is less then VMIN.
    */
    tty.c_cc[VMIN] = 0;      /* 1=block until n bytes are received */
    tty.c_cc[VTIME] = 0;     /* block until a timer expires (n * 100 mSec.) */

    tcflush(hPeriph->handle, TCIFLUSH);
    if (errno = tcsetattr(hPeriph->handle, TCSANOW, &tty) != 0)
    {
        error_message("error %d from tcsetattr\n", errno);
        return FALSE;
    }
    
    return PMD_ERR_OK;


/*if (baud <= PMDSerialBaud460800) // support the use of PMDSerialBaud enum instead of actual baud.
        baud = baudList[baud];


    dcb.DCBlength         = sizeof(DCB);
    dcb.BaudRate          = baud;
    dcb.fBinary           = 1;
    dcb.fParity           = TRUE;
    dcb.fOutxCtsFlow      = 0;
    dcb.fOutxDsrFlow      = 0;
    dcb.fDtrControl       = DTR_CONTROL_DISABLE;
    dcb.fDsrSensitivity   = 0;
    dcb.fTXContinueOnXoff = 0;
    dcb.fOutX             = 0;
    dcb.fInX              = 0;
    dcb.fErrorChar        = 0;
    dcb.fNull             = 0;
    dcb.fRtsControl       = RTS_CONTROL_DISABLE;
    dcb.fAbortOnError     = 0;
    dcb.XonLim            = 0;
    dcb.XoffLim           = 0;
    dcb.ByteSize          = 8;
    dcb.Parity            = parList[parity];
    dcb.StopBits          = stopList[stopbits];
    dcb.XonChar           = 0;
    dcb.XoffChar          = 0;
    dcb.ErrorChar         = 0;
    dcb.EofChar           = 0;
    dcb.EvtChar           = 0;

    if( !SetCommState( hPeriph->handle, &dcb ) )
        return PMD_ERR_ConfiguringPort;
    return PMD_ERR_OK;
    */
}

//********************************************************
PMDresult PMDPCOM_SetTimeout(PMDPeriphHandle* hPeriph, DWORD msec)
{
    struct termios tty;
    int error;
    int * portptr;
    portptr=(int*) hPeriph->handle;
    
    //COMMTIMEOUTS timeouts;

    PMD_PERIPHCONNECTED(hPeriph)
        

    //PMDSerialIOData* SIOtransport_data = (PMDSerialIOData*)transport_data;

    //if (SIOtransport_data->hPort == INVALID_HANDLE_VALUE) return FALSE;

    int fd = hPeriph->handle;
    if (error = tcgetattr(fd, &tty) != 0)
    {
        error_message("new error %d from tcgetattr fd=%d\n", error,fd);
        return PMD_ERR_CommandError;
    }

    tty.c_cc[VTIME] = msec / 100;     /* block until a timer expires (n * 100 mSec.) */

   // printf("Timeout %d\n", tty.c_cc[VTIME]);

    if (error = tcsetattr(fd, TCSANOW, &tty) != 0)
    {
        error_message("error %d from tcsetattr\n", error);
        return PMD_ERR_CommandError;
    }

    return PMD_ERR_OK;
    
   /*
        
        
        
    timeouts.ReadIntervalTimeout         = 0;
    timeouts.ReadTotalTimeoutMultiplier  = 10;
    timeouts.ReadTotalTimeoutConstant    = msec;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant   = 0;

    // If SetCommTimeouts fails, the return value is zero. To get extended error information, call GetLastError. 
    if (SetCommTimeouts( hPeriph->handle, &timeouts ))
        return PMD_ERR_OK;
    return PMD_ERR_CommandError;
    */
    }


//********************************************************
static PMDresult PMDPCOM_Send(PMDPeriphHandle* hPeriph, const void* data, PMDparam nCount, PMDparam timeoutms)
{
    PMDparam nCountSent = 0;
    int port_status = 1;
    PMD_PERIPHCONNECTED(hPeriph)

    if (nCount > 0)
    {
        PMDPCOM_SetTimeout(hPeriph, timeoutms);
        nCountSent = write(hPeriph->handle, data, nCount);
               
        //if( !WriteFile( hPeriph->handle, data, nCount, &nCountSent, NULL ) )
       // {
        //    FormatGetLastError();
         //   return PMD_ERR_PortWrite;
       // }
        
        //wait for transmission
        while (port_status) ioctl(hPeriph->handle, TIOCOUTQ, &port_status);
        if (nCountSent < nCount)
            return PMD_ERR_PortWrite;
    }
    return PMD_ERR_OK;
}

//********************************************************
static PMDresult PMDPCOM_Receive(PMDPeriphHandle* hPeriph, void *data, PMDparam nCount, PMDparam *pnreceived, PMDparam timeoutms)
{
    unsigned int sumbytes;
    int i;
    // int *port;
    PMDuint8 tempbuffer[10]; 
    PMDuint8 *bufferptr;
    bufferptr= data;
    
    DWORD bytes; 
    
    *pnreceived = 0;
    PMD_PERIPHCONNECTED(hPeriph)

    if (nCount > 0)
    {
        PMDPCOM_SetTimeout(hPeriph, timeoutms);
        sumbytes = 0;
        bytes = 1;
        while (bytes)
        {
            bytes = read(hPeriph->handle, tempbuffer, nCount);
      //      printf("bytes %d  nCount %d tempbuffer %x \n",bytes,nCount,tempbuffer[0]);
          //  for (i = 0; i < bytes; i++) buffer[sumbytes + i] = tempbuffer[i];
            for (i = 0; i < bytes; i++) bufferptr[sumbytes + i] = tempbuffer[i];
         //   for (i = 0; i < bytes; i++) data[sumbytes + i] = tempbuffer[i];
            sumbytes += bytes;
        //    printf("sumbytes in receive %d\n",sumbytes);
            if (sumbytes == nCount) break;
        }
     //   printf("sumbytes in receive %d\n",sumbytes);
        *pnreceived = sumbytes; 
        
        //if( !ReadFile( hPeriph->handle, data, nCount, pnreceived, NULL ) )
       // {
       //     FormatGetLastError();
       //     return PMD_ERR_PortRead;
       // }
        
        
        if (*pnreceived < nCount)
            return PMD_ERR_Timeout;
    }
   
      return PMD_ERR_OK;
}

//********************************************************
PMDresult PMDPCOM_FlushRecv(PMDPeriphHandle* hPeriph)
{
    PMD_PERIPHCONNECTED(hPeriph)

    tcflush(hPeriph->handle, TCIOFLUSH);
    fflush(stdin);
    fflush(stdout); 
    
    
    return PMD_ERR_OK;
}

//********************************************************
static void PMDPCOM_Init(PMDPeriphHandle* hPeriph)
{
    // set the interface type 
    hPeriph->type = InterfaceSerial;

    hPeriph->transport.Close    = PMDPCOM_Close;
    hPeriph->transport.Send     = PMDPCOM_Send;
    hPeriph->transport.Receive  = PMDPCOM_Receive;
    hPeriph->transport.Read     = NULL;
    hPeriph->transport.Write    = NULL;
}

//*************************************************************************************
void ReportError()
{
char szErrorMsg[256];
DWORD ErrorCode;

    ErrorCode = GetLastError();
    if (ErrorCode != EXIT_SUCCESS)
    {
   //     FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, 
    //              NULL, 
     //             ErrorCode, 
                  //MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
     //             szErrorMsg, 
      //            sizeof(szErrorMsg), 
      //            NULL);
                  puts(szErrorMsg);
    }
}

//********************************************************
//Set portnum as PMDSerialPort  (0 = COM1, 1 = COM2, etc.)
//********************************************************
PMDresult PMDPCOM_Open(PMDPeriphHandle* hPeriph, PMDparam portnum, PMDparam baud, PMDparam parity, PMDparam stopbits)
{
    //_TCHAR szPort[12];
    char szPort[13] = "/dev/serial0";
    int port;
  //  ptrport=&port;
    
       
    PMDPCOM_Init(hPeriph);
    
    //portnum++;
    //_stprintf_s( szPort, 12, _T("\\\\.\\COM%d"), portnum );

    //hPeriph->handle = open(szPort, O_RDWR | O_NOCTTY);
    port = open(szPort, O_RDWR | O_NOCTTY);
    hPeriph->handle= (void*) port;
 
    if (flock(port, LOCK_EX | LOCK_NB) != 0)
    {
        PMDPCOM_Close(hPeriph);
        error_message("Another process has locked the comport.");
        return PMD_ERR_OpeningPort;
    }
    
    if( hPeriph->handle == INVALID_HANDLE_VALUE )
    {
        ReportError();
        return PMD_ERR_OpeningPort;
    }
    if (PMD_ERR_OK != PMDPCOM_SetConfig(hPeriph, baud, parity, stopbits))
    {
        PMDPCOM_Close(hPeriph);
        return PMD_ERR_ConfiguringPort;
    }
    if (PMDPCOM_SetTimeout(hPeriph, 100) != PMD_ERR_OK)
    {
        PMDPCOM_Close(hPeriph);
        return PMD_ERR_ConfiguringPort;
    }
    return PMD_ERR_OK;
}