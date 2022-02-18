#ifndef _DEF_INC_PMDecode
#define	_DEF_INC_PMDecode

/*
  PMDecode.h -- error codes

  Performance Motion Devices, Inc.
*/

#if defined(__cplusplus)
extern "C" {
#endif

typedef enum PMDErrorCodesEnum {

	// Motion processor errors
	PMD_NOERROR                                 = 0,
	PMD_ERR_OK                                  = 0,
	PMD_ERR_MP_Reset                            = 0x01,
	PMD_ERR_MP_InvalidInstruction               = 0x02,
	PMD_ERR_MP_InvalidAxis                      = 0x03,
	PMD_ERR_MP_InvalidParameter                 = 0x04,
	PMD_ERR_MP_TraceRunning                     = 0x05,
	PMD_ERR_MP_BlockOutOfBounds                 = 0x07,
	PMD_ERR_MP_TraceBufferZero                  = 0x08,
	PMD_ERR_MP_BadSerialChecksum                = 0x09,
	PMD_ERR_MP_InvalidNegativeValue             = 0x0B,
	PMD_ERR_MP_InvalidParameterChange           = 0x0C,
	PMD_ERR_MP_LimitEventPending                = 0x0D,
	PMD_ERR_MP_InvalidMoveIntoLimit             = 0x0E,
	PMD_ERR_MP_NVRAMMode                        = 0x0F,
	PMD_ERR_MP_InvalidOperatingModeRestore      = 0x10,
	PMD_ERR_MP_InvalidOperatingModeForCommand   = 0x11,
	PMD_ERR_MP_BadState                         = 0x12,
	PMD_ERR_MP_HardFault                        = 0x13,
	PMD_ERR_MP_AtlasNotDetected                 = 0x14,
	PMD_ERR_MP_BadSPIChecksum                   = 0x15,
	PMD_ERR_MP_InvalidSPIprotocol               = 0x16,
	PMD_ERR_MP_SPICommandTimingViolation        = 0x17,
	PMD_ERR_MP_InvalidTorqueCommand             = 0x18,
	PMD_ERR_MP_BadFlashChecksum                 = 0x19,
	PMD_ERR_MP_InvalidFlashModeCommand          = 0x1A,
	PMD_ERR_MP_ReadOnly                         = 0x1B,
	PMD_ERR_MP_InitializationOnlyCommand        = 0x1C,
	PMD_ERR_MP_IncorrectDataCount               = 0x1D,
	PMD_ERR_MP_MoveWhileInError                 = 0x1E,
	PMD_ERR_MP_WaitTimedOut                     = 0x1F,
	PMD_ERR_MP_InitializationRunning            = 0x20,
	PMD_ERR_MP_InvalidClock                     = 0x21,
	PMD_ERR_MP_InitializationSkipped            = 0x22,
	PMD_ERR_MP_InvalidInterface                 = 0x23,

	// General errors
	PMD_ERR_Version                             = 0x1002,
	PMD_ERR_WriteError                          = 0x1005,
	PMD_ERR_ReadError                           = 0x1006,
	PMD_ERR_Cancelled                           = 0x1007,
	PMD_ERR_CommunicationsError                 = 0x1008,
	PMD_ERR_InsufficientDataReceived            = 0x100A,
	PMD_ERR_UnexpectedDataReceived              = 0x100B,
	PMD_ERR_Memory                              = 0x100C,
	PMD_ERR_Timeout                             = 0x100D,
	PMD_ERR_Checksum                            = 0x100E,
	PMD_ERR_CommandError                        = 0x100F,
	PMD_ERR_MutexTimeout                        = 0x1010,

	// Function call errors
	PMD_ERR_NotSupported                        = 0x1101,
	PMD_ERR_InvalidOperation                    = 0x1102,
	PMD_ERR_InvalidInterface                    = 0x1103,
	PMD_ERR_InvalidPort                         = 0x1104,
	PMD_ERR_InvalidBaud                         = 0x1105,
	PMD_ERR_InvalidHandle                       = 0x1106,
	PMD_ERR_InvalidDataSize                     = 0x1107,
	PMD_ERR_InvalidParameter                    = 0x1108,
	PMD_ERR_InvalidAddress                      = 0x1109,
	PMD_ERR_ParameterOutOfRange                 = 0x110A,
	PMD_ERR_ParameterAlignment                  = 0x110B,

	// Interface connection errors
	PMD_ERR_NotConnected                        = 0x1201,
	PMD_ERR_NotResponding                       = 0x1202,
	PMD_ERR_PortRead                            = 0x1203,
	PMD_ERR_PortWrite                           = 0x1204,
	PMD_ERR_OpeningPort                         = 0x1205,
	PMD_ERR_ConfiguringPort                     = 0x1206,
	PMD_ERR_InterfaceNotInitialized             = 0x1207,
	PMD_ERR_Driver                              = 0x1208,
	PMD_ERR_AddressInUse                        = 0x1209,
	PMD_ERR_IPRouting                           = 0x120A,
	PMD_ERR_OutOfResources                      = 0x120B,
	PMD_ERR_QueueFull                           = 0x120C,
	PMD_ERR_QueueEmpty                          = 0x120D,
	PMD_ERR_ResourceInUse                       = 0x120E,
	PMD_ERR_SerialOverrun                       = 0x1211,
	PMD_ERR_SerialBreak                         = 0x1212,
	PMD_ERR_SerialParity                        = 0x1213,
	PMD_ERR_SerialFrame                         = 0x1214,
	PMD_ERR_SerialNoise                         = 0x1215,
	PMD_ERR_ReceiveOverrun                      = 0x1220,
	PMD_ERR_CAN_BitStuff                        = 0x1221,
	PMD_ERR_CAN_Form                            = 0x1222,
	PMD_ERR_CAN_Acknowledge                     = 0x1223,
	PMD_ERR_CAN_BitRecessive                    = 0x1224,
	PMD_ERR_CAN_BitDominant                     = 0x1225,
	PMD_ERR_CAN_CRC                             = 0x1226,
	PMD_ERR_CAN_BusOff                          = 0x1228,
	PMD_ERR_CAN_Passive                         = 0x1229,
	PMD_ERR_CAN_Active                          = 0x122A,

	// Resource Protocol errors
	PMD_ERR_RP_RemoteErrorMask                  = 0x2000,
	PMD_ERR_RP_Reset                            = 0x2001,
	PMD_ERR_RP_InvalidVersion                   = 0x2002,
	PMD_ERR_RP_InvalidResource                  = 0x2003,
	PMD_ERR_RP_InvalidAddress                   = 0x2004,
	PMD_ERR_RP_InvalidAction                    = 0x2005,
	PMD_ERR_RP_InvalidSubAction                 = 0x2006,
	PMD_ERR_RP_InvalidCommand                   = 0x2007,
	PMD_ERR_RP_InvalidParameter                 = 0x2008,
	PMD_ERR_RP_InvalidPacket                    = 0x2009,
	PMD_ERR_RP_PacketLength                     = 0x200A,
	PMD_ERR_RP_PacketAlignment                  = 0x200B,
	PMD_ERR_RP_Checksum                         = 0x200E,

	// Prodigy/CME user code format errors
	PMD_ERR_UC_Signature                        = 0x2101,
	PMD_ERR_UC_Version                          = 0x2102,
	PMD_ERR_UC_FileSize                         = 0x2103,
	PMD_ERR_UC_Checksum                         = 0x2104,
	PMD_ERR_UC_WriteError                       = 0x2105,
	PMD_ERR_UC_NotProgrammed                    = 0x2106,
	PMD_ERR_UC_TaskNotCreated                   = 0x2107,
	PMD_ERR_UC_TaskAlreadyRunning               = 0x2108,
	PMD_ERR_UC_TaskNotFound                     = 0x2109,
	PMD_ERR_UC_Format                           = 0x210A,
	PMD_ERR_UC_Reserved                         = 0x210B,
	PMD_ERR_UC_TaskAborted                      = 0x210C,

  	PMD_ERR_NVRAM                               = 0x2200, // low byte is an extended error code

	PMD_ERR_Unknown                             = 0xFFFF,
    
} PMDErrorCode;

#if defined(__cplusplus)
}
#endif

#endif
