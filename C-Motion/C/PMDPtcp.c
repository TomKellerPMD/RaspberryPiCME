//
//  PMDPtcp.c - Windows sockets PMD host side peripheral implementation
//
//  Performance Motion Devices, Inc.
//


//#include <winsock2.h>
#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

#include "PMDRPtypes.h"
#include "PMDperiph.h"
#include "PMDecode.h"
#include "PMDsys.h"
#include "PMDPfunc.h"
#include "PMDutil.h"

#define SOCKET int
int             socket_desc;


typedef struct tagPMDTCPIOData {
    SOCKET          m_Socket;
    unsigned short  m_nBasePort;
    PMDparam        m_nIPAddress;
} PMDTCPIOData;

typedef struct tagERR_INFO
{
    DWORD error;
    char *str;
} ERR_INFO;

#define SOCKET_ERROR -1
#define WSAGetLastError() (errno)
#define WSAETIMEDOUT ETIMEDOUT
#define WSADATA char *
#define MAKEWORD( a, b ) (0x0202)
#define WSAStartup(a,b) 1
#define WSACleanup() 1
#define INVALID_SOCKET ESOCKTNOSUPPORT

/*
static ERR_INFO errStringsSockets[] =
{
    { WSANOTINITIALISED     ,"A successful AfxSocketInit must occur before using this API."},
    { WSAENETDOWN           ,"The Windows Sockets implementation detected that the network subsystem failed."},
    { WSAEAFNOSUPPORT       ,"The specified address family is not supported."},
    { WSAEINPROGRESS        ,"A blocking Windows Sockets operation is in progress."},
    { WSAEMFILE             ,"No more file descriptors are available."},
    { WSAEPROTONOSUPPORT    ,"The specified port is not supported."},
    { WSAEPROTOTYPE         ,"The specified port is the wrong type for this socket."},
    { WSAESOCKTNOSUPPORT    ,"The specified socket type is not supported in this address family."},
    { WSAEADDRINUSE         ,"The specified address is already in use."},
    { WSAEADDRNOTAVAIL      ,"The specified address is not available from the local machine."},
    { WSAEAFNOSUPPORT       ,"Addresses in the specified family cannot be used with this socket."},
    { WSAECONNREFUSED       ,"The attempt to connect was rejected."},
    { WSAEDESTADDRREQ       ,"A destination address is required."},
    { WSAEFAULT             ,"The nSockAddrLen argument is incorrect."},
    { WSAEINVAL             ,"Invalid host address."},
    { WSAEISCONN            ,"The socket is already connected."},
    { WSAENETUNREACH        ,"The network cannot be reached from this host at this time."},
    { WSAENOBUFS            ,"No buffer space is available. The socket cannot be connected."},
    { WSAENOTSOCK           ,"The descriptor is not a socket."},
    { WSAETIMEDOUT          ,"Attempt to connect timed out without establishing a connection."},
    { WSAEWOULDBLOCK        ,"The socket is marked as nonblocking and the connection cannot be completed immediately."},
    { WSAEACCES             ,"The requested address is a broadcast address, but the appropriate flag was not set."},                    
    { WSAENETRESET          ,"The connection must be reset because the Windows Sockets implementation dropped it."},
    { WSAENOTCONN           ,"The socket is not connected."},
    { WSAEOPNOTSUPP         ,"MSG_OOB was specified, but the socket is not of type SOCK_STREAM."},
    { WSAESHUTDOWN          ,"The socket has been shut down; it is not possible to call Send on a socket after ShutDown has been invoked with nHow set to 1 or 2."},
    { WSAEMSGSIZE           ,"The socket is of type SOCK_DGRAM, and the datagram is larger than the maximum supported by the Windows Sockets implementation."},
    { WSAEINVAL             ,"The socket has not been bound with Bind."},
    { WSAECONNABORTED       ,"The virtual circuit was aborted due to timeout or other failure."},
    { WSAECONNRESET         ,"The virtual circuit was reset by the remote side. "},

};
*/






//********************************************************
static char *GetErrStringSocket( DWORD err )
{
//    int i;
//    for( i=0; i<sizeof(errStringsSockets)/sizeof(ERR_INFO); i++ )
  //      if( err == errStringsSockets[i].error ) return errStringsSockets[i].str;

    //return "Unknown error code.";
    return strerror(errno);
}



//********************************************************
static void SocketError()
{
    int ErrorCode;
    char szErrorMsg[256];
    szErrorMsg[0] = 0;

    ErrorCode = WSAGetLastError();
    if (ErrorCode)
    {
        PMDprintf("Socket error: %s\n", GetErrStringSocket(ErrorCode));
    }
}

//********************************************************
PMDresult PMDPTCP_Close(PMDPeriphHandle* hPeriph)
{
    PMDTCPIOData* pTCPtransport_data = (PMDTCPIOData*)hPeriph->transport_data;

    close(pTCPtransport_data->m_Socket);
    WSACleanup();
    free(pTCPtransport_data);
    pTCPtransport_data = NULL;
    hPeriph->handle = PMD_INVALID_HANDLE; // set the periph handle as not connected

    return PMD_NOERROR;
}

//********************************************************
PMDresult PMDPTCP_Send(PMDPeriphHandle* hPeriph, void *data, PMDparam nCount, PMDparam timeoutms)
{
    PMDTCPIOData* pTCPtransport_data = (PMDTCPIOData*)hPeriph->transport_data;
    int nCountSent = 0;
    struct timeval sTimeout;
    
    PMD_PERIPHCONNECTED(hPeriph)

    if (timeoutms == PMD_WAITFOREVER)
        timeoutms = 0;
            
    sTimeout=ConvertTime(timeoutms);
     
    if (SOCKET_ERROR == setsockopt(pTCPtransport_data->m_Socket, SOL_SOCKET, SO_SNDTIMEO, (const void *) &sTimeout, sizeof(sTimeout)))
    {
  
        SocketError();
        return PMD_ERR_PortRead;
    }
    nCountSent = send(pTCPtransport_data->m_Socket, (const char *)data, nCount, 0);
    if (SOCKET_ERROR == nCountSent)
    {
        DWORD ErrorCode;
        ErrorCode = WSAGetLastError();
        if ( ErrorCode == WSAETIMEDOUT)
        {
            return PMD_ERR_Timeout;
        }
        else
        {
            SocketError();
            return PMD_ERR_PortWrite;
        }
    }

    return PMD_NOERROR;
}

//********************************************************
PMDresult PMDPTCP_Receive(PMDPeriphHandle* hPeriph, void *data, PMDparam nExpected, PMDparam *pnReceived, PMDparam timeoutms)
{
    PMDTCPIOData* pTCPtransport_data = (PMDTCPIOData*)hPeriph->transport_data;
    int nCountReceived;

    struct timeval sTimeout;
    
    PMD_PERIPHCONNECTED(hPeriph)

    if (timeoutms == PMD_WAITFOREVER)
        timeoutms = 0;
            
    sTimeout=ConvertTime(timeoutms);
    
    *pnReceived = 0;
   
     if (SOCKET_ERROR == setsockopt(pTCPtransport_data->m_Socket, SOL_SOCKET, SO_RCVTIMEO, (const void *)&sTimeout, sizeof(sTimeout)))
    {
          SocketError();
        return PMD_ERR_PortRead;
    }

    do 
    {
        nCountReceived = recv(pTCPtransport_data->m_Socket, (char*)data, nExpected, 0 );
        if (0 == nCountReceived) // socket closed
            return PMD_ERR_NotConnected;

        if (SOCKET_ERROR == nCountReceived)
        {
            int error = WSAGetLastError();
            if ( error == WSAETIMEDOUT)
            {
                return PMD_ERR_Timeout;
            }
            else
            {
                SocketError();
                return PMD_ERR_PortRead;
            }
        }
    }   
    while (nCountReceived < 1);

    *pnReceived = nCountReceived;

/*
    if( nCountReceived != nCountExpected )
    {
        Printf( "Received %d bytes:", nCountReceived);
        for( int i=0; i<nCountReceived; i++ )
            Printf( " %2d: %02x\r", i, pbuff[i] );
        return PMD_ERR_PortRead;
    }
*/  
    return PMD_NOERROR;
}

//********************************************************
static void PMDPTCP_Init(PMDPeriphHandle* hPeriph)
{
    hPeriph->transport.Close    = PMDPTCP_Close;
    hPeriph->transport.Send     = PMDPTCP_Send;
    hPeriph->transport.Receive  = PMDPTCP_Receive;
    hPeriph->transport.Read     = NULL;
    hPeriph->transport.Write    = NULL;
    hPeriph->transport.ReceiveEvent = NULL;
}

//********************************************************
// This implementation does not currently support the timeout parameter when attempting a connection.
// It uses the default socket timeout.
PMDresult PMDPTCP_Open (PMDPeriphHandle* hPeriph, PMDparam ipaddress, PMDparam portnum, PMDparam timeout)
{
    int             socket_type = SOCK_STREAM;
    int             protocol = IPPROTO_TCP;
    int             socketerror = 0;
    WSADATA         wsaData;
    WORD            wVersionRequested = MAKEWORD( 2, 2 );
    PMDTCPIOData*   pTCPtransport_data;
    SOCKET          Socket;

    PMDPTCP_Init(hPeriph);
    // set the interface type 
    hPeriph->type = InterfaceTCP;

    if (WSAStartup(wVersionRequested ,&wsaData) == SOCKET_ERROR) 
    {
        SocketError();
        WSACleanup();
        return PMD_ERR_OpeningPort;
    }

    Socket = socket(AF_INET, socket_type, protocol); // Open a socket
    if (Socket < 0 ) 
    {
        SocketError();
        WSACleanup();
        return PMD_ERR_OpeningPort;
    }

    {
        unsigned int addr;
        int structsize;
        struct sockaddr_in server;

        //
        // Copy the resolved information into the sockaddr_in structure
        //
        addr = htonl(ipaddress);
        memset(&server, 0, sizeof(server));
        memcpy(&(server.sin_addr), &addr, 4);
        server.sin_family = AF_INET;
        server.sin_port = htons((unsigned short)portnum);

        if (addr == 0)
        {
            socketerror = bind(Socket, (struct sockaddr*)&server, sizeof(server));
            if (socketerror != SOCKET_ERROR)
            {
                socketerror = listen(Socket, 0);
                if (socketerror != SOCKET_ERROR)
                {
                    structsize = sizeof(server);
                    Socket = accept(Socket, (struct sockaddr*)&server, &structsize);
                    if (Socket == INVALID_SOCKET)
                        socketerror = SOCKET_ERROR;
                }
            }
        }
        else
        {
            socketerror = connect(Socket, (struct sockaddr*)&server, sizeof(server));
        }

        if (socketerror == SOCKET_ERROR) 
        {
            SocketError();
            WSACleanup();
            return PMD_ERR_OpeningPort;
        }
    }

    pTCPtransport_data = (PMDTCPIOData*) malloc( sizeof(PMDTCPIOData) );
    memset(pTCPtransport_data, 0, sizeof(PMDTCPIOData));
    pTCPtransport_data->m_Socket = Socket;
    hPeriph->transport_data = pTCPtransport_data;
    hPeriph->address = 0;
    hPeriph->handle = PMD_CONNECTED_HANDLE; // set the periph handle as connected

    return PMD_NOERROR;
}



PMDresult PMDPUDP_Open (PMDPeriphHandle* hPeriph, PMDparam ipaddress, PMDparam portnum)
{
    int             socket_type = SOCK_DGRAM;
    int             protocol = IPPROTO_UDP;
    int             socketerror;
    WSADATA         wsaData;
    WORD            wVersionRequested = MAKEWORD( 2, 2 );
    PMDTCPIOData*   pTCPtransport_data;
    SOCKET          Socket;

    PMDPTCP_Init(hPeriph);
    // set the interface type 
    hPeriph->type = InterfaceUDP;

    if (WSAStartup(wVersionRequested ,&wsaData) == SOCKET_ERROR) 
    {
        SocketError();
        WSACleanup();
        return PMD_ERR_OpeningPort;
    }

    Socket = socket(AF_INET, socket_type, protocol);
    if (Socket < 0 ) 
    {
        SocketError();
        WSACleanup();
        return PMD_ERR_OpeningPort;
    }

    {
        unsigned int addr;
        struct sockaddr_in server;

        //
        // Copy the resolved information into the sockaddr_in structure
        //
        addr = htonl(ipaddress);
        memset(&server, 0, sizeof(server));
        memcpy(&(server.sin_addr), &addr, 4);
        server.sin_family = AF_INET;
        server.sin_port = htons((unsigned short)portnum);

        if (addr == 0)
        {
            socketerror = bind(Socket, (struct sockaddr*)&server, sizeof(server));
        }
        else
        {
            socketerror = connect(Socket, (struct sockaddr*)&server, sizeof(server));
        }

        if (socketerror == SOCKET_ERROR) 
        {
            SocketError();
            WSACleanup();
            return PMD_ERR_OpeningPort;
        }
    }

    pTCPtransport_data = (PMDTCPIOData*) malloc( sizeof(PMDTCPIOData) );
    memset(pTCPtransport_data, 0, sizeof(PMDTCPIOData));
    pTCPtransport_data->m_Socket = Socket;
    hPeriph->transport_data = pTCPtransport_data;
    hPeriph->address = 0;
    hPeriph->handle = PMD_CONNECTED_HANDLE; // set the periph handle as connected

    return PMD_NOERROR;
}
