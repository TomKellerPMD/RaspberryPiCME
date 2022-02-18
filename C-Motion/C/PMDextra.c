//
// PMDextra.c : Functions for extra features on some products 
//

#include "C-Motion.h"
#include "PMDperiph.h"
#include "PMDdevice.h"
#include "PMDsys.h"
#include "PMDextra.h"

//********************************************************************************
PMDresult PMDCMESSIConfig(PMDAxisHandle* hAxis, PMDDeviceHandle* hDevice,PMDCMESSIResolution resolution, PMDCMESSIFrequency frequency, int MasterClock, PMDCMESSIFormat format)
{
    PMDuint16 modulus, config, PosH;
    int axisoffset;
    PMDresult result;
    PMDPeriphHandle hPeriph;
    PMDparam eventIRQ = 0; 
        
    axisoffset = (hAxis->axis) * 0x20;
    modulus = 0x0001 << (resolution-1);
	if (resolution > 16)
		modulus = 0x7FFF;

    PMDSetEncoderModulus(hAxis, modulus);
    
    config=0x02 + MasterClock + format * 4;     
    //MasterClock=1 if enabled or 0 if disabled.
    //format=0 if Binary or =1 if Gray code


    result = PMDPeriphOpenPIO(&hPeriph, hDevice, (WORD) 0x100, (BYTE) eventIRQ, PMDDataSize_16Bit);
    if(result) 
        return result;
    
    PMDPeriphWrite(&hPeriph, &config, axisoffset, 1);           //Config
    PMDPeriphWrite(&hPeriph, &resolution, axisoffset+0x02, 1);   //resolution
    //resolution in units of bits.

    
    PMDPeriphWrite(&hPeriph, &frequency, axisoffset+0x04, 1);        //frequency
    //Note: true frequency will be 40Mhz/(frequency+1)
    //Maximum true frequency is 4Mhz.

    // if resolution is greater than 16-bits then we need to read the upper word.
    // Otherwise it returns zero in which case this has no effect.

    PMDTaskWait(100);
    //wait 100ms for SSI to start up.

    PMDPeriphRead(&hPeriph, &PosH, axisoffset+0x10, 1);   
    PMDPeriphRead(&hPeriph, &PosH, axisoffset+0x12, 1);  // addresses 0x10 and 0x12 must always be read consequetively.
    if(0x8000 & PosH) 
        return PMD_ERR_ReadError;   //  If bit 31 is set then configuration failed or no SSI data was received.
    
    
    PMDSetEncoderSource(hAxis, PMDEncoderSourceParallel);
    PMDAdjustActualPosition(hAxis, PosH*65536);
    PMDPeriphClose(&hPeriph);

    return PMD_ERR_OK;
}



