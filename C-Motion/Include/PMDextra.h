#ifndef PMD_Extra
#define PMD_Extra

//  PMDextra.h -- API for extra features on some products.
//
//  Performance Motion Devices, Inc.
//

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum
{

    PMDCMESSIFreq4M   = 9,    // 4Mhz max
    PMDCMESSIFreq2M   = 19,
    PMDCMESSIFreq1M   = 39,
    PMDCMESSIFreq500K = 79,
    PMDCMESSIFreq100K = 399,
    PMDCMESSIFreq78K  = 511   // 78.125Khz min
} PMDCMESSIFrequency;

typedef enum
{
    PMDCMESSIRes10 = 10,      // 10-bits
    PMDCMESSIRes12 = 12,
    PMDCMESSIRes13 = 13,
    PMDCMESSIRes25 = 25,
    PMDCMESSIRes31 = 31       //max
} PMDCMESSIResolution;

typedef enum
{
    PMDCMESSIFormatBinary = 0,
    PMDCMESSIFormatGray,
} PMDCMESSIFormat;


PMDresult PMDCMESSIConfig(PMDAxisHandle* axis_handle, PMDDeviceHandle* hDevice,PMDCMESSIResolution resolution, PMDCMESSIFrequency frequency, int MasterClock, PMDCMESSIFormat format);

#if defined(__cplusplus)
}
#endif

#endif
