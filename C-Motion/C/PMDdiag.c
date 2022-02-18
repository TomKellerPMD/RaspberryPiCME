/*
  PMDdiag.c : Converts opcodes and error codes to strings
 
  Performance Motion Devices, Inc.
*/ 

#include "PMDtypes.h"
#include "PMDRPtypes.h"
#include "PMDdiag.h"
#include "PMDecode.h"


const char* PMDOpcodeText[] = {
    "NoOperation               ",
    "Invalid opcode!           ",
    "SetMotorType              ",
    "GetMotorType              ",
    "SetBiQuadCoefficient      ",
    "GetBiQuadCoefficient      ",
    "SetMotorLimit             ",
    "GetMotorLimit             ",
    "SetAuxiliaryEncoderSource ",
    "GetAuxiliaryEncoderSource ",
    "SetSPIMode                ",
    "GetSPIMode                ",
    "SetPWMFrequency           ",
    "GetPWMFrequency           ",
    "GetDriveStatus            ",
    "SetMotorBias              ",

    "SetPosition               ",
    "SetVelocity               ",
    "SetCANMode                ",
    "SetJerk                   ",
    "SetGearRatio              ",
    "GetCANMode                ",
    "Invalid opcode!           ",
    "Invalid opcode!           ",
    "Invalid opcode!           ",
    "Invalid opcode!           ",
    "Update                    ",
    "SetOvertemperatureLimit   ",
    "GetOvertemperatureLimit   ",
    "GetCommandedPosition      ",
    "GetCommandedVelocity      ",
    "Invalid opcode!           ",

    "Invalid opcode!           ",
    "SetFeedbackParameter      ",
    "GetFeedbackParameter      ",
    "SetDrivePWM               ",
    "GetDrivePWM               ",
    "SetKp                     ",
    "SetKi                     ",
    "SetKd                     ",
    "GetTraceValue             ",
    "SetAnalogCalibration      ",
    "GetAnalogCalibration      ",
    "SetKvff                   ",
    "GetPhaseAngle             ",
    "GetMotorBias              ",
    "RestoreOperatingMode      ",
    "SetInterruptMask          ",

    "NVRAM                     ",
    "GetEventStatus            ",
    "SetBreakpointUpdateMask   ",
    "GetBreakpointUpdateMask   ",
    "ResetEventStatus          ",
    "ExecutionControl          ", //InitializationDelay in Atlas documentation
    "GetCaptureValue           ",
    "GetActualPosition         ",
    "Invalid opcode!           ",
    "Reset                     ",
    "GetActiveMotorCommand     ",
    "SetSampleTime             ",
    "GetSampleTime             ",
    "Invalid opcode!           ",
    "GetTime                   ",
    "Invalid opcode!           ",

    "GetBusVoltage             ",
    "SetCurrentFoldback        ",
    "GetCurrentFoldback        ",
    "SetCurrentControlMode     ",
    "GetCurrentControlMode     ",
    "SetAxisOutMask            ",
    "GetAxisOutMask            ",
    "ClearPositionError        ",
    "SetEventAction            ",
    "GetEventAction            ",
    "GetPosition               ",
    "GetVelocity               ",
    "GetAcceleration           ",
    "SetActualPosition         ",
    "Invalid opcode!           ",
    "Invalid opcode!           ",

    "GetKp                     ",
    "GetKi                     ",
    "GetKd                     ",
    "GetTemperature            ",
    "GetKvff                   ",
    "GetPositionLoopValue      ",
    "GetInterruptMask          ",
    "GetActiveOperatingMode    ",
    "GetJerk                   ",
    "GetGearRatio              ",
    "GetFOCValue               ",
    "MultiUpdate               ",
    "Invalid opcode!           ",
    "Invalid opcode!           ",
    "SetCurrent                ",
    "GetCurrent                ",

    "GetDriveFaultParameter    ",
    "Invalid opcode!           ",
    "SetDriveFaultParameter    ",
    "Invalid opcode!           ",
    "Invalid opcode!           ",
    "SetOperatingMode          ",
    "GetOperatingMode          ",
    "SetPositionLoop           ",
    "GetPositionLoop           ",
    "GetMotorCommand           ",
    "SetStartVelocity          ",
    "GetStartVelocity          ",
    "ClearDriveFaultStatus     ",
    "GetDriveFaultStatus       ",
    "GetOutputMode             ",
    "CalibrateAnalog           ",

    "GetDriveValue             ",
    "GetCurrentLoopValue       ",
    "SetPhaseInitializeTime    ",
    "SetCurrentLoop            ",
    "GetCurrentLoop            ",
    "SetPhaseCounts            ",
    "SetPhaseOffset            ",
    "SetMotorCommand           ",
    "SetLoop                   ",
    "GetLoop                   ",
    "InitializePhase           ",
    "GetPhaseOffset            ",
    "GetPhaseInitializeTime    ",
    "GetPhaseCounts            ",
    "SetDriveCommandMode       ",
    "GetDriveCommandMode       ",

    "SetLimitSwitchMode        ",
    "GetLimitSwitchMode        ",
    "WriteIO                   ",
    "ReadIO                    ",
    "SetPhaseAngle             ",
    "Invalid opcode!           ",
    "Invalid opcode!           ",
    "SetAxisMode               ",
    "GetAxisMode               ",
    "SetDefault                ",
    "GetDefault                ",
    "SetSerialPortMode         ",
    "GetSerialPortMode         ",
    "SetEncoderModulus         ",
    "GetEncoderModulus         ",
    "GetVersion                ",

    "SetAcceleration           ",
    "SetDeceleration           ",
    "GetDeceleration           ",
    "SetKaff                   ",
    "GetKaff                   ",
    "SetIntegrationLimit       ",
    "GetIntegrationLimit       ",
    "SetPositionErrorLimit     ",
    "GetPositionErrorLimit     ",
    "GetPositionError          ",
    "GetIntegral               ",
    "GetDerivative             ",
    "SetDerivativeTime         ",
    "GetDerivativeTime         ",
    "SetKout                   ",
    "GetKout                   ",

    "SetProfileMode            ",
    "GetProfileMode            ",
    "SetSignalSense            ",
    "GetSignalSense            ",
    "GetSignalStatus           ",
    "GetInstructionError       ",
    "GetActivityStatus         ",
    "GetCommandedAcceleration  ",
    "SetTrackingWindow         ",
    "GetTrackingWindow         ",
    "SetSettleTime             ",
    "GetSettleTime             ",
    "ClearInterrupt            ",
    "GetActualVelocity         ",
    "SetGearMaster             ",
    "GetGearMaster             ",

    "SetTraceMode              ",
    "GetTraceMode              ",
    "SetTraceStart             ",
    "GetTraceStart             ",
    "SetTraceStop              ",
    "GetTraceStop              ",
    "SetTraceVariable          ",
    "GetTraceVariable          ",
    "SetTracePeriod            ",
    "GetTracePeriod            ",
    "GetTraceStatus            ",
    "GetTraceCount             ",
    "SetSettleWindow           ",
    "GetSettleWindow           ",
    "SetActualPositionUnits    ",
    "GetActualPositionUnits    ",

    "SetBufferStart            ",
    "GetBufferStart            ",
    "SetBufferLength           ",
    "GetBufferLength           ",
    "SetBufferWriteIndex       ",
    "GetBufferWriteIndex       ",
    "SetBufferReadIndex        ",
    "GetBufferReadIndex        ",
    "WriteBuffer               ",
    "ReadBuffer                ",
    "SetBufferFunction         ",
    "GetBufferFunction         ",
    "WriteBuffer16             ",
    "ReadBuffer16              ",
    "GetStepRange              ",
    "SetStepRange              ",

    "SetStopMode               ",
    "GetStopMode               ",
    "SetAutoStopMode           ",
    "GetAutoStopMode           ",
    "SetBreakpoint             ",
    "GetBreakpoint             ",
    "SetBreakpointValue        ",
    "GetBreakpointValue        ",
    "SetCaptureSource          ",
    "GetCaptureSource          ",
    "SetEncoderSource          ",
    "GetEncoderSource          ",
    "SetMotorMode              ",
    "GetMotorMode              ",
    "SetEncoderToStepRatio     ",
    "GetEncoderToStepRatio     ",

    "SetOutputMode             ",
    "GetInterruptAxis          ",
    "SetCommutationMode        ",
    "GetCommutationMode        ",
    "SetPhaseInitializeMode    ",
    "GetPhaseInitializeMode    ",
    "SetPhasePrescale          ",
    "GetPhasePrescale          ",
    "SetPhaseCorrectionMode    ",
    "GetPhaseCorrectionMode    ",
    "GetPhaseCommand           ",
    "SetMotionCompleteMode     ",
    "GetMotionCompleteMode     ",
    "SetAxisOutSource          ",
    "GetAxisOutSource          ",
    "ReadAnalog                ",

    "Invalid opcode!           ",
    "Invalid opcode!           ",
    "SetSynchronizationMode    ",
    "GetSynchronizationMode    ",
    "Invalid opcode!           ",
    "AdjustActualPosition      ",
    "SetFOC                    ",
    "GetFOC                    ",
    "GetChecksum               ",
    "SetUpdateMask             ",
    "GetUpdateMask             ",
    "SetFaultOutMask           ",
    "GetFaultOutMask           ",
    "Invalid opcode!           ",
    "Invalid opcode!           ",
    "Invalid opcode!           "

};


const char *PMDGetOpcodeText(PMDuint16 opCode)
{
    return PMDOpcodeText[opCode & 0xff];
}

const char *PMDGetErrorMessage(PMDresult errorCode)
{
    return errorCode == PMD_ERR_OK                         ? "No error" :

    // Motion processor errors
    errorCode == PMD_ERR_MP_Reset                          ? "Processor reset" :
    errorCode == PMD_ERR_MP_InvalidInstruction             ? "Invalid instruction" :
    errorCode == PMD_ERR_MP_InvalidAxis                    ? "Invalid axis" :
    errorCode == PMD_ERR_MP_InvalidParameter               ? "Invalid data parameter" :
    errorCode == PMD_ERR_MP_TraceRunning                   ? "Trace currently running" :
    errorCode == PMD_ERR_MP_BlockOutOfBounds               ? "Block out of bounds" :
    errorCode == PMD_ERR_MP_TraceBufferZero                ? "Zero length trace buffer" :
    errorCode == PMD_ERR_MP_BadSerialChecksum              ? "Invalid checksum" :
    errorCode == PMD_ERR_MP_InvalidNegativeValue           ? "Invalid negative value for profile mode" :
    errorCode == PMD_ERR_MP_InvalidParameterChange         ? "Invalid parameter change for profile mode" :
    errorCode == PMD_ERR_MP_LimitEventPending              ? "Invalid move with limit event pending" :
    errorCode == PMD_ERR_MP_InvalidMoveIntoLimit           ? "Invalid move into limit" :
    errorCode == PMD_ERR_MP_NVRAMMode                      ? "Invalid command in NVRAM mode" :
    errorCode == PMD_ERR_MP_InvalidOperatingModeRestore    ? "Invalid operating mode restore" :
    errorCode == PMD_ERR_MP_InvalidOperatingModeForCommand ? "Command not valid in this operating mode" :
    errorCode == PMD_ERR_MP_BadState                       ? "Command not accepted in current state" :
    errorCode == PMD_ERR_MP_AtlasNotDetected               ? "Atlas command specified but no Atlas detected." :
    errorCode == PMD_ERR_MP_HardFault                      ? "A hard fault has occured. The processor must be reset " :
    errorCode == PMD_ERR_MP_BadSPIChecksum                 ? "Bad SPI command checksum" :
    errorCode == PMD_ERR_MP_InvalidSPIprotocol             ? "Incorrect SPI command protocol" :
    errorCode == PMD_ERR_MP_SPICommandTimingViolation      ? "An SPI command result was requested before it was ready" :
    errorCode == PMD_ERR_MP_InvalidTorqueCommand           ? "Invalid torque command" :
    errorCode == PMD_ERR_MP_BadFlashChecksum               ? "Bad flash checksum" :
    errorCode == PMD_ERR_MP_InvalidFlashModeCommand        ? "Command not valid in flash mode" :
    errorCode == PMD_ERR_MP_ReadOnly                       ? "Write to read only buffer" :
    errorCode == PMD_ERR_MP_InitializationOnlyCommand      ? "Command only valid in NVRAM intialization" :
    errorCode == PMD_ERR_MP_IncorrectDataCount             ? "Incorrect amount of data received by processor" :
    errorCode == PMD_ERR_MP_MoveWhileInError               ? "Attempt to move while an event bit is set that caused a stop" :
    errorCode == PMD_ERR_MP_WaitTimedOut                   ? "ExecutionControl command timed out" :
    errorCode == PMD_ERR_MP_InitializationRunning          ? "Attempt to change buffer 1 state while NVRAM init is running" :
    errorCode == PMD_ERR_MP_InvalidClock                   ? "Invalid external clock detected" :
    errorCode == PMD_ERR_MP_InitializationSkipped          ? "Reset was performed with NVRAM initialization inhibited" :
    errorCode == PMD_ERR_MP_InvalidInterface               ? "Operation not permitted on command interface" :

    // General errors
    errorCode == PMD_ERR_Version                           ? "Incorrect version" :
    errorCode == PMD_ERR_WriteError                        ? "Write error" :
    errorCode == PMD_ERR_ReadError                         ? "Read error" :
    errorCode == PMD_ERR_Cancelled                         ? "Wait cancelled" :
    errorCode == PMD_ERR_CommunicationsError               ? "Communication error" :
    errorCode == PMD_ERR_InsufficientDataReceived          ? "Insufficient data received" :
    errorCode == PMD_ERR_UnexpectedDataReceived            ? "Unexpected data received" :
    errorCode == PMD_ERR_Memory                            ? "Memory allocation failure" :
    errorCode == PMD_ERR_Timeout                           ? "Timeout" :
    errorCode == PMD_ERR_Checksum                          ? "Checksum error" :
    errorCode == PMD_ERR_CommandError                      ? "Command error" :
    errorCode == PMD_ERR_MutexTimeout                      ? "Mutex Timeout" :

    // Function call errors
    errorCode == PMD_ERR_NotSupported                      ? "Not supported" :
    errorCode == PMD_ERR_InvalidOperation                  ? "Invalid operation" :
    errorCode == PMD_ERR_InvalidInterface                  ? "Invalid interface" :
    errorCode == PMD_ERR_InvalidPort                       ? "Invalid port" :
    errorCode == PMD_ERR_InvalidBaud                       ? "Invalid baud" :
    errorCode == PMD_ERR_InvalidHandle                     ? "Invalid handle" :
    errorCode == PMD_ERR_InvalidDataSize                   ? "Invalid data size" :
    errorCode == PMD_ERR_InvalidParameter                  ? "Invalid parameter" :
    errorCode == PMD_ERR_InvalidAddress                    ? "Invalid address" :
    errorCode == PMD_ERR_ParameterOutOfRange               ? "Parameter out of range" :
    errorCode == PMD_ERR_ParameterAlignment                ? "Data pointer parameter misaligned" :

    // Interface connection errors
    errorCode == PMD_ERR_NotConnected                      ? "Not connected" :
    errorCode == PMD_ERR_NotResponding                     ? "Device not responding" :
    errorCode == PMD_ERR_PortRead                          ? "Error reading from port" :
    errorCode == PMD_ERR_PortWrite                         ? "Error writing to port" :
    errorCode == PMD_ERR_OpeningPort                       ? "Error opening port" :
    errorCode == PMD_ERR_ConfiguringPort                   ? "Error configuring port" :
    errorCode == PMD_ERR_InterfaceNotInitialized           ? "Interface not initialized" :
    errorCode == PMD_ERR_Driver                            ? "Driver error" :
    errorCode == PMD_ERR_AddressInUse                      ? "Address in use" :
    errorCode == PMD_ERR_IPRouting                         ? "Cannot route IP address" :
    errorCode == PMD_ERR_OutOfResources                    ? "Out of resources" :
    errorCode == PMD_ERR_QueueFull                         ? "Queue full" :
    errorCode == PMD_ERR_QueueEmpty                        ? "Queue empty" :
    errorCode == PMD_ERR_ResourceInUse                     ? "Resource in use" :
    errorCode == PMD_ERR_SerialOverrun                     ? "Serial receive data overrun" :
    errorCode == PMD_ERR_SerialBreak                       ? "Serial break condition" :
    errorCode == PMD_ERR_SerialParity                      ? "Serial parity error" :
    errorCode == PMD_ERR_SerialFrame                       ? "Serial framing error" :
    errorCode == PMD_ERR_SerialNoise                       ? "Serial noise error" :
    errorCode == PMD_ERR_ReceiveOverrun                    ? "Receive overrun" :
    errorCode == PMD_ERR_CAN_BitStuff                      ? "CAN Bit stuff error" :
    errorCode == PMD_ERR_CAN_Form                          ? "CAN Form error" :
    errorCode == PMD_ERR_CAN_Acknowledge                   ? "CAN Acknowledge error" :
    errorCode == PMD_ERR_CAN_BitRecessive                  ? "CAN Bit recessive error" :
    errorCode == PMD_ERR_CAN_BitDominant                   ? "CAN Bit dominant error" :
    errorCode == PMD_ERR_CAN_CRC                           ? "CAN CRC error" :
    errorCode == PMD_ERR_CAN_BusOff                        ? "CAN Bus off" :
    errorCode == PMD_ERR_CAN_Passive                       ? "CAN Passive error state" :
    errorCode == PMD_ERR_CAN_Active                        ? "CAN Active error state" :
        
    // Resource Protocol errors
    errorCode == PMD_ERR_RP_Reset                          ? "Device was reset since last command" :
    errorCode == PMD_ERR_RP_InvalidVersion                 ? "Invalid RP version" :
    errorCode == PMD_ERR_RP_InvalidResource                ? "Invalid RP resource" :
    errorCode == PMD_ERR_RP_InvalidAddress                 ? "Invalid RP address" :
    errorCode == PMD_ERR_RP_InvalidAction                  ? "Invalid RP action" :
    errorCode == PMD_ERR_RP_InvalidSubAction               ? "Invalid RP subaction" :
    errorCode == PMD_ERR_RP_InvalidCommand                 ? "Invalid RP command" :
    errorCode == PMD_ERR_RP_InvalidParameter               ? "Invalid RP parameter" :
    errorCode == PMD_ERR_RP_InvalidPacket                  ? "Invalid RP packet" :
    errorCode == PMD_ERR_RP_Checksum                       ? "Invalid RP checksum" :

    // Prodigy/CME user code errors
    errorCode == PMD_ERR_UC_Signature                      ? "Invalid user code signature" :
    errorCode == PMD_ERR_UC_Version                        ? "Invalid user code version" :
    errorCode == PMD_ERR_UC_FileSize                       ? "Invalid user code file size" :
    errorCode == PMD_ERR_UC_Checksum                       ? "Invalid user code checksum" :
    errorCode == PMD_ERR_UC_WriteError                     ? "User code write error" :
    errorCode == PMD_ERR_UC_NotProgrammed                  ? "User code not programmed" :
    errorCode == PMD_ERR_UC_Format                         ? "Invalid user code format" :
    errorCode == PMD_ERR_UC_TaskNotCreated                 ? "Task not started" :
    errorCode == PMD_ERR_UC_TaskAlreadyRunning             ? "Task already running" :
    errorCode == PMD_ERR_UC_TaskNotFound                   ? "Task not found" :
    errorCode == PMD_ERR_UC_TaskAborted                    ? "Task aborted" :
    errorCode == PMD_ERR_UC_Reserved                       ? "Reserved function call" :

    "Undefined error occurred";
}

const char *PMDGetResetCauseMessage(PMDuint16 resetcause)
{
    return resetcause ==  0 ? "None" :
          (resetcause & PMDResetCause_UnderVoltage  ) ? "Undervoltage" :
          (resetcause & PMDResetCause_HardReset     ) ? "Hard reset" :
          (resetcause & PMDResetCause_SysWatchdog   ) ? "System watchdog" :
          (resetcause & PMDResetCause_Exception     ) ? "Exception" :
          (resetcause & PMDResetCause_SoftReset     ) ? "Soft reset" :
          "Undefined reset cause";
};
