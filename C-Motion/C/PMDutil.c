/*
  PMDutil.c : C-Motion utility functions
 
  Performance Motion Devices, Inc.
*/

#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <math.h>

#include "PMDperiph.h"
#include "PMDsys.h"
#include "PMDutil.h"

PMDuint32 GetTickCount(void);



// ****************************************************************************
// This function is used by the example code for establishing a connection to a host interface using the default settings
// Function parameters:
// portoffset - is the offset from the default port #
// listen - when set to TRUE places the port in receive mode for user packet examples
//
// interface    default port
// ------------------------------------------------------
// PCI          0 = first board found
// Serial       COM1
// CAN          CAN_ADDRESS_BASE_TX and CAN_ADDRESS_BASE_RX
// TCP          40100
// 
// uncomment these lines if using any of these interface options
// they are disabled here to prevent compiling in the PCI and CAN driver code.
//#define USE_PCI_INTERFACE
//#define USE_CAN_INTERFACE

PMDCFunc PMDPeriphOpen(PMDPeriphHandle* hPeriph, PMDInterfaceType interfacetype, int portoffset, int listen)
{
    PMDresult result;
    PMDZeroMemory(hPeriph, sizeof(PMDPeriphHandle));

    // TODO: adjust the port settings according to your board's defaults

    switch (interfacetype)
    {
#ifdef USE_PCI_INTERFACE
    case InterfacePCI:
        {
            int boardno = portoffset;
            PMDprintf("Connecting to PCI.\r\n" );
            result = PMDPeriphOpenPCI( hPeriph, boardno );
        }
        break;
#endif
#ifdef USE_CAN_INTERFACE
    case InterfaceCAN:
        {
            int CANnodeid = portoffset;
            PMDuint32 addressTX = CAN_ADDRESS_BASE_TX + CANnodeid;
            PMDuint32 addressRX = CAN_ADDRESS_BASE_RX + CANnodeid;
            if (listen) // swap the RX and TX CAN mailboxes in listen mode
            {
                addressTX = CAN_ADDRESS_BASE_RX + CANnodeid;
                addressRX = CAN_ADDRESS_BASE_TX + CANnodeid;
            }
            PMDprintf("Connecting to CAN.\r\n" );
            result = PMDPeriphOpenCAN( hPeriph, NULL, addressTX, addressRX, 0);
        }
        break;
#endif
    case InterfaceSerial:
        {
            int serialportno = PMDSerialPort1 + portoffset;
            PMDprintf("Connecting to COM%d.\r\n", serialportno+1 );
            result = PMDPeriphOpenCOM( hPeriph, NULL, serialportno, 57600, PMDSerialParityNone, PMDSerialStopBits1);
            // uncomment the following lines and set the address accordingly if connecting via RS485
            //hPeriph->address = 1;
            //hPeriph->param = PMDSerialProtocolMultiDropUsingIdleLineDetection;
        }
        break;
    case InterfaceTCP:
        {
            unsigned int addr = PMD_IP4_ADDR(192,168,2,2);
            int portnum = DEFAULT_ETHERNET_PORT + portoffset;
            if (listen)
                addr = 0;
            PMDprintf("Connecting to TCP/IP port: %d\r\n", portnum);
            result = PMDPeriphOpenTCP( hPeriph, NULL, addr, portnum, 0 );
        }
        break;
    default: 
        result = PMD_ERR_InvalidInterface;
    }
    if (result == PMD_ERR_OK)
        PMDprintf("Success!\r\n");
    return result;
}



//*****************************************************************************
void SetupTrace(PMDAxisHandle* phAxis, PMDuint32 bufferlength)
{
    PMDSetTraceStop( phAxis, 0, PMDTraceConditionImmediate, 0, PMDTraceTriggerStateHigh );
    
    // setup the trace buffer (buffer ID = 0)
    PMDSetBufferLength( phAxis, 0, bufferlength );
    PMDSetTracePeriod( phAxis, 10); // cycle(s) per trace
    PMDSetTraceMode( phAxis, PMDTraceModeOneTime );

    // single axis dynamics trace
    PMDSetTraceVariable( phAxis, PMDTraceVariableNumber1, PMDAxis1, PMDTraceVariableCommandedPosition );
    PMDSetTraceVariable( phAxis, PMDTraceVariableNumber2, PMDAxis1, PMDTraceVariableCommandedVelocity );
    PMDSetTraceVariable( phAxis, PMDTraceVariableNumber3, PMDAxis1, PMDTraceVariableCommandedAcceleration ); 
    PMDSetTraceVariable( phAxis, PMDTraceVariableNumber4, PMDAxis1, PMDTraceVariableMotionProcessorTime );

/*
    // single axis commanded vs actual position trace
    PMDSetTraceVariable( phAxis, PMDTraceVariableNumber1, PMDAxis1, PMDTraceVariableCommandedPosition );
    PMDSetTraceVariable( phAxis, PMDTraceVariableNumber2, PMDAxis1, PMDTraceVariableActualPosition );
    PMDSetTraceVariable( phAxis, PMDTraceVariableNumber3, PMDAxis1, PMDTraceVariableActiveMotorCommand );
    PMDSetTraceVariable( phAxis, PMDTraceVariableNumber4, PMDAxis1, PMDTraceVariableNone );

    // 4 axis position trace
    PMDSetTraceVariable( phAxis, PMDTraceVariableNumber1, PMDAxis1, PMDTraceVariableCommandedPosition );
    PMDSetTraceVariable( phAxis, PMDTraceVariableNumber1, PMDAxis2, PMDTraceVariableCommandedPosition );
    PMDSetTraceVariable( phAxis, PMDTraceVariableNumber1, PMDAxis3, PMDTraceVariableCommandedPosition );
    PMDSetTraceVariable( phAxis, PMDTraceVariableNumber1, PMDAxis4, PMDTraceVariableCommandedPosition );

    // 2 axis commanded position vs velocity trace
    PMDSetTraceVariable( phAxis, PMDTraceVariableNumber1, PMDAxis1, PMDTraceVariableCommandedPosition );
    PMDSetTraceVariable( phAxis, PMDTraceVariableNumber2, PMDAxis2, PMDTraceVariableCommandedPosition );
    PMDSetTraceVariable( phAxis, PMDTraceVariableNumber3, PMDAxis1, PMDTraceVariableCommandedVelocity );
    PMDSetTraceVariable( phAxis, PMDTraceVariableNumber4, PMDAxis2, PMDTraceVariableCommandedVelocity );
*/  
    // trace will start on the next update command on Axis 1
    PMDSetTraceStart( phAxis, PMDAxis1, PMDTraceConditionNextUpdate, 0, PMDTraceTriggerStateHigh );
    // or trigger a trace start on InMotion bit instead of an update 
    //PMDSetTraceStart( phAxis, PMDAxis1, PMDTraceConditionActivityStatus, PMDActivityInMotionBit, PMDTraceTriggerStateHigh );
}

/***********************************************************************************

  ReadBuffer

  Implements similar functionality as the chip command PMDReadBuffer except it reads the data
  directly from the DPRAM if the interface supports it.  It is much faster than
  calling PMDReadBuffer for the same number of data reads.
  To further improve performance the PMDGet... commands can be removed and the values stored
  in local variables

***********************************************************************************/
PMDresult ReadBuffer(PMDAxisHandle* phAxis, PMDuint16 bufferID, PMDint32* pbuffer, PMDuint32 dwords_to_read)
{
    PMDuint32 i, index,length,start;
    PMDMemoryHandle hMemory;
    PMDDeviceHandle* phDevice = (PMDDeviceHandle*)phAxis->transport_data;

    if (PMD_ERR_OK == PMDMemoryOpen(&hMemory, phDevice, PMDDataSize_32Bit, PMDMemoryType_DPRAM))
    {
        PMDGetBufferReadIndex(phAxis, bufferID, &index);
        PMDGetBufferLength(phAxis, bufferID, &length);
        PMDGetBufferStart(phAxis, bufferID, &start);

        // can't read more data than the buffer can hold!
        if (dwords_to_read>length)
            return PMD_ERR_ParameterOutOfRange;

        // check if this is a wrap-around condition
        if (dwords_to_read > (length-index))
        {
            // read the first segment
            PMDMemoryRead( &hMemory, pbuffer, index+start, (length-index));
            // read the next segment
            PMDMemoryRead( &hMemory, pbuffer+(length-index), start, dwords_to_read-(length-index));
        }
        else
            PMDMemoryRead( &hMemory, pbuffer, index+start, dwords_to_read);

        PMDMemoryClose( &hMemory );
        PMDSetBufferReadIndex(phAxis, bufferID, dwords_to_read%length);
    }
    else  
    {
        for (i=0; i<dwords_to_read; i++) 
            PMDReadBuffer(phAxis, bufferID, pbuffer++);
    }
    return PMD_ERR_OK;
}

//*****************************************************************************
void DisplayTraceResults(PMDAxisHandle* phAxis)
{
    PMDuint32 i;
    PMDint32 *pbuffer;
    PMDint32 *pbufferindex;
    PMDuint16 status;
    PMDuint16 tracemode;
    PMDuint16 bufferid = 0;
    PMDuint32 bufferlength;
    PMDuint32 tracecount;
    PMDuint32 nTraces;
    PMDuint16 traceperiod;
    PMDuint16 nVariables = 0;
    PMDuint8 tracevar;
    PMDAxis axisno;
    int bDone, bActive, bStarted;


    // get number of trace variables so we can display each one in it's own column
    do
    {
        PMDGetTraceVariable( phAxis, nVariables, &axisno, &tracevar );
        nVariables++;
    }
    while (tracevar != PMDTraceVariableNone && nVariables < 4);

    PMDGetBufferLength(phAxis, bufferid, &bufferlength );
    PMDGetTraceMode(phAxis, &tracemode);
    PMDGetTraceCount(phAxis, &tracecount);
    PMDGetTraceStatus(phAxis, &status);
    PMDGetTracePeriod(phAxis, &traceperiod);
    nTraces = bufferlength / nVariables;

    PMDprintf("Trace status  = %d\r\n", status);
    PMDprintf("Trace count   = %ld\r\n", tracecount);
    PMDprintf("Trace period  = %d cycles\r\n", traceperiod);
    PMDprintf("Buffer length = %ld\r\n", bufferlength );
    PMDprintf("# traces      = %ld\r\n", nTraces );

    bActive = (status & PMDTraceStatusActivity);
    bDone = (tracecount == bufferlength && !bActive);
    bStarted = bActive || bDone;

    if (!bStarted)
    {
        PMDprintf("Error. Trace was not started.\r\n");
        return;
    }

    pbuffer = (PMDint32*)malloc(sizeof(PMDint32) * bufferlength);
    
    if (bActive) // wait until done (not necessary but simpler)
    {
        while (tracecount < bufferlength) 
            PMDGetTraceCount( phAxis, &tracecount);
    }

    // read the data
    ReadBuffer(phAxis, bufferid, pbuffer, tracecount);
    
    // display the data
    tracecount = 0;
    pbufferindex = pbuffer;
    PMDprintf("\r\n");
    PMDprintf("#\tVar1\tVar2\tVar3\tVar4\r\n");
    PMDprintf("----\t----\t----\t----\t----\r\n");
    do 
    {
        PMDprintf("%d", tracecount);
        for (i=0; i<nVariables; i++) 
        {
            PMDprintf("\t%ld", *pbufferindex++);
        }
        PMDprintf("\r\n");
        tracecount++;
    }
    while (tracecount < nTraces);

    free(pbuffer);
}


PMDMutexHandle PMDMutexCreate()
{
    static PMDMutexHandle mutex=NULL;
    return mutex;
}
//#endif

struct timeval ConvertTime(long msec)
{
    struct timeval timeout;
    timeout.tv_usec=msec%1000;
    timeout.tv_sec=msec/1000;
    return timeout;
}    


//  ********************///
// GetTickCount()

PMDuint32 GetTickCount()
{
    struct timespec spec;
    time_t s;
    long ms;
    clock_gettime(CLOCK_REALTIME,&spec);
    
    s=spec.tv_sec;
    ms = round(spec.tv_nsec/1.0e6);
    if(ms>999)
    {
        s++;
        ms=0;
    }
    return s*1000+ms;
}
        

//*****************************************************************************
// WaitForEvent 
//
//   Waits for the specified event to be set in the EventStatus register.
//
//*****************************************************************************
PMDresult WaitForEvent(PMDAxisHandle* phAxis, PMDuint16 eventmask, PMDuint32 timeoutms)
{
    PMDuint16 status = 0;
    PMDresult result = PMD_NOERROR;
    PMDuint32 starttime;
    PMDuint32 endtime;

    PMDprintf("Waiting for event %04X on axis %d.\r\n", eventmask, phAxis->axis + 1);

    starttime = PMDDeviceGetTickCount();
    while (!(status & eventmask))
    {
        PMDGetEventStatus( phAxis, &status );
        if (PMDDeviceGetTickCount() > starttime + timeoutms)
        {
            return PMD_ERR_Timeout;
        }
    }
    endtime = PMDDeviceGetTickCount();
    PMDprintf("Event(s) %04X set on axis #%d in %d ms\r\n", status, phAxis->axis+1, endtime - starttime);
    // clear the event for the next time
    PMDResetEventStatus( phAxis, (PMDuint16)~eventmask );

    return result;
}


// ****************************************************************************
PMDresult WaitForAlgorithmicPhaseInitialization(PMDAxisHandle* phAxis)
{
	PMDuint16 status;
	PMDresult result;

	// poll ActivityStatus to determine when Phase Initilization is complete
	PMDprintf("Waiting for algorithmic phase initialization to complete.\r\n");
	do
	{
		result = PMDGetActivityStatus(phAxis, &status);
	}	
	while(!(status & PMDActivityStatusPhasingInitialized));

	PMDprintf("Algorithmic phase initialization completed.\r\n");
	return result;
}

// ****************************************************************************
// this function will perform the appropriate phase initialize method for the current settings
PMDresult InitializePhase(PMDAxisHandle* phAxis)
{
	PMDuint16 PhaseInitializeMode;
	PMDuint16 MotorType;
	PMDresult result;

	result = PMDGetMotorType( phAxis, &MotorType );

	if (MotorType == PMDMotorTypeBrushlessDC3Phase)
	{
		PMDGetPhaseInitializeMode( phAxis, &PhaseInitializeMode );
		if (PhaseInitializeMode == PMDPhaseInitializeModeAlgorithmic)
		{
			//assumes SetMotorCommand has been set
			result = PMDInitializePhase( phAxis );
			WaitForAlgorithmicPhaseInitialization( phAxis );
		}
		else if (PhaseInitializeMode == PMDPhaseInitializeModeHallBased)
		{
			result = PMDInitializePhase(phAxis);
		}
	}
	return result;
}

// ****************************************************************************
// wait for the "DriveNotConnected" bit in the Magellan's Drive Status register to go low.
PMDresult WaitForAtlasToConnect(PMDAxisHandle* phAxis)
{
    PMDuint16 status = 0;
    PMDuint32 starttime;
    PMDuint32 timeoutms = 1000;

    starttime = PMDDeviceGetTickCount();
    do
    {
        PMDGetDriveStatus( phAxis, &status );
        if (PMDDeviceGetTickCount() > starttime + timeoutms)
        {
            PMDprintf("Timeout waiting for Atlas to connect.\r\n");
            return PMD_ERR_Timeout;
        }
    }
    while (status & PMDDriveStatusAtlasNotConnected);

    return PMD_NOERROR;
}

void PMDTaskWait(DWORD ms)
{
    usleep(1000 * ms);
}
