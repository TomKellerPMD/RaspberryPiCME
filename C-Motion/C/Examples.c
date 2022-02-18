//
// Examples.c : Example functions for all products.
//

#include "C-Motion.h"
#include "PMDdiag.h"
#include "PMDRPtypes.h"
#include "PMDperiph.h"
#include "PMDdevice.h"
#include "PMDsys.h"
#include "PMDutil.h"
#include "Examples.h"


// ****************************************************************************
// This example issues all of the Magellan commands that are common amongst the product line
void AllMagellanCommands(PMDAxisHandle* phAxis)
{
    PMDuint16 generation, motorType, numberAxes, chip_count, custom, major, minor;
    PMDuint16 status;
    PMDint32 data;
    PMDresult result;
    PMDuint16 mode;
    PMDint32 position;
    PMDint32 velocity;
    PMDuint32 acceleration;
    PMDuint32 deceleration;
    PMDuint32 jerk;
    PMDint32 ratio;
    PMDAxis masterAxis; 
    PMDuint8 mode8;
    PMDuint16 source;
    PMDint32 kp;
    PMDint32 kd;
    PMDint32 ki;
    PMDint32 kvff;
    PMDint32 kaff;
    PMDint32 kout;
    PMDint32 integrallimit;
    PMDint32 derivativeTime;
    PMDuint32 limit;
    PMDuint16 limit16;

    PMDint32 biquad1Enable;
    PMDint32 biquad1B0;
    PMDint32 biquad1B1;
    PMDint32 biquad1B2;
    PMDint32 biquad1A1;
    PMDint32 biquad1A2;
    PMDint32 biquad1K;
    PMDint32 biquad2Enable;
    PMDint32 biquad2B0;
    PMDint32 biquad2B1;
    PMDint32 biquad2B2;
    PMDint32 biquad2A1;
    PMDint32 biquad2A2;
    PMDint32  biquad2K;

    PMDint16 bias;
    PMDuint16 settleTime;
    PMDuint16 settleWindow;
    PMDuint16 trackingWindow;
    PMDint32 derivative;
    PMDint32 integral;
    PMDint32 positionError;
    PMDint32 biquad;
    PMDuint32 sampleTime;
    PMDint16 value;
    PMDint32 value32;
    PMDuint16 breakpointID = 0; 
    PMDAxis breakAxis; 
    PMDuint8 action; 
    PMDuint8 trigger;
    PMDint32 breakpointValue;
    PMDuint16 interruptMask;
    PMDuint16 eventStatus;
    PMDuint16 mask;
    PMDuint16 captureSource;
    PMDuint16 encoderCounts; 
    PMDuint16 steps;
    PMDint16 motorCommand;
    PMDuint16 frequency;
    PMDuint16 phasePrescale;
    PMDuint16 phaseCounts;
    PMDuint16 phaseInitTime;
    PMDuint16 phaseOffset;
    PMDuint16 phaseAngle;

    PMDuint16 bufferID = 0; 
    PMDuint32 bufferLength;
    PMDuint32 writeIndex;
    PMDuint32 readIndex;
    PMDuint16 tracePeriod;
    PMDuint16 traceID = PMDTraceVariableNumber1; 
    PMDuint8 variable;
    PMDAxis traceAxis; 
    PMDuint8 condition; 
    PMDuint8 bit; 
    PMDuint8 state;
    PMDuint8 sourceRegister;

    PMDuint16 data16;
    PMDuint8 baud; 
    PMDuint8 parity; 
    PMDuint8 stopBits; 
    PMDuint8 protocol; 
    PMDuint8 multiDropID;
    PMDuint32 checksum;
    PMDuint32 numberServoCycles;
    PMDuint32 count;
    PMDint16 command;
    PMDint32 captureValue;
    PMDint32 actualValue;
    PMDuint16 signalStatus;
    PMDuint16 activityStatus;
    PMDuint16 interruptingAxisMask;
    PMDuint16 analogID = 0;
    PMDuint16 param = 0;
    PMDuint16 sourceAxis;
    PMDuint16 selectionMask;
    PMDuint16 senseMask;
    PMDuint16 eventaction;

        
//    PMD_RESULT(PMDReset( phAxis ));
//    PMDTaskWait(500);               // Wait for Magellan to come out of reset.
    
    // Motor
    PMD_RESULT(PMDGetMotorType              (phAxis,  &data16));
    PMD_RESULT(PMDSetMotorType              (phAxis,  data16));		// this will reset all values to their defaults
    PMD_RESULT(PMDGetMotorCommand           (phAxis,  &motorCommand));
    PMD_RESULT(PMDSetMotorCommand           (phAxis,  motorCommand));
    PMD_RESULT(PMDGetActiveMotorCommand     (phAxis,  &motorCommand));

    // Commutation
    PMD_RESULT(PMDGetPWMFrequency           (phAxis,  &frequency));
    PMD_RESULT(PMDSetPWMFrequency           (phAxis,  frequency));
    PMD_RESULT(PMDGetCommutationMode        (phAxis,  &mode));
    PMD_RESULT(PMDSetCommutationMode        (phAxis,  mode));
    PMD_RESULT(PMDGetPhaseInitializeMode    (phAxis,  &mode));
    PMD_RESULT(PMDSetPhaseInitializeMode    (phAxis,  mode));
    PMD_RESULT(PMDGetPhasePrescale          (phAxis,  &phasePrescale));
    PMD_RESULT(PMDSetPhasePrescale          (phAxis,  phasePrescale));
    PMD_RESULT(PMDGetPhaseCounts            (phAxis,  &phaseCounts));
    PMD_RESULT(PMDSetPhaseCounts            (phAxis,  1000));
    PMD_RESULT(PMDGetPhaseInitializeTime    (phAxis,  &phaseInitTime));
    PMD_RESULT(PMDSetPhaseInitializeTime    (phAxis,  phaseInitTime));
    PMD_RESULT(PMDGetPhaseOffset            (phAxis,  &phaseOffset));
    PMD_RESULT(PMDSetPhaseOffset            (phAxis,  0));
    PMD_RESULT(PMDGetPhaseAngle             (phAxis,  &phaseAngle));
    PMD_RESULT(PMDSetPhaseAngle             (phAxis,  phaseAngle));
    PMD_RESULT(PMDGetPhaseCorrectionMode    (phAxis,  &mode));
    PMD_RESULT(PMDSetPhaseCorrectionMode    (phAxis,  mode));
    PMD_RESULT(PMDInitializePhase           (phAxis));
    PMD_RESULT(PMDGetPhaseCommand           (phAxis,  PMDPhaseA,  &command));
    PMD_RESULT(PMDGetPhaseCommand           (phAxis,  PMDPhaseB,  &command));
    PMD_RESULT(PMDGetPhaseCommand           (phAxis,  PMDPhaseC,  &command));
   
    // Encoder
    PMD_RESULT(PMDGetActualPosition         (phAxis,  &position));
    PMD_RESULT(PMDSetActualPosition         (phAxis,  position));
    PMD_RESULT(PMDAdjustActualPosition      (phAxis,  position));
    PMD_RESULT(PMDGetActualPositionUnits    (phAxis,  &mode));
    PMD_RESULT(PMDSetActualPositionUnits    (phAxis,  mode));
    PMD_RESULT(PMDGetCaptureSource          (phAxis,  &captureSource));
    PMD_RESULT(PMDSetCaptureSource          (phAxis,  captureSource));
    PMD_RESULT(PMDGetEncoderSource          (phAxis,  &data16));
    PMD_RESULT(PMDSetEncoderSource          (phAxis,  data16));
    PMD_RESULT(PMDGetEncoderToStepRatio     (phAxis,  &encoderCounts,  &steps));
    PMD_RESULT(PMDSetEncoderToStepRatio     (phAxis,  encoderCounts,  steps));
    PMD_RESULT(PMDGetActualVelocity         (phAxis,  &actualValue));
    PMD_RESULT(PMDGetCaptureValue           (phAxis,  &captureValue));
    PMD_RESULT(PMDGetAuxiliaryEncoderSource (phAxis,  &mode8, &sourceAxis));
    PMD_RESULT(PMDSetAuxiliaryEncoderSource (phAxis,  mode8, sourceAxis));

    // Profile Generation
    PMD_RESULT(PMDGetProfileMode            (phAxis,  &mode));
    PMD_RESULT(PMDSetProfileMode            (phAxis,  mode));
    PMD_RESULT(PMDGetPosition               (phAxis,  &position));
    PMD_RESULT(PMDSetPosition               (phAxis,  position));
    PMD_RESULT(PMDGetVelocity               (phAxis,  &velocity));
    PMD_RESULT(PMDSetVelocity               (phAxis,  velocity));
    PMD_RESULT(PMDGetStartVelocity          (phAxis,  (PMDuint32*)&velocity));
    PMD_RESULT(PMDSetStartVelocity          (phAxis,  velocity));
    PMD_RESULT(PMDGetAcceleration           (phAxis,  &acceleration));
    PMD_RESULT(PMDSetAcceleration           (phAxis,  acceleration));
    PMD_RESULT(PMDGetDeceleration           (phAxis,  &deceleration));
    PMD_RESULT(PMDSetDeceleration           (phAxis,  deceleration));
    PMD_RESULT(PMDGetJerk                   (phAxis,  &jerk));
    PMD_RESULT(PMDSetJerk                   (phAxis,  jerk));
    PMD_RESULT(PMDGetGearRatio              (phAxis,  &ratio));
    PMD_RESULT(PMDSetGearRatio              (phAxis,  ratio));
    PMD_RESULT(PMDGetGearMaster             (phAxis,  &masterAxis,  &source));
    PMD_RESULT(PMDSetGearMaster             (phAxis,  masterAxis,  source));
    PMD_RESULT(PMDGetStopMode               (phAxis,  &mode));
    PMD_RESULT(PMDSetStopMode               (phAxis,  mode));
    PMD_RESULT(PMDGetCommandedPosition      (phAxis,  &position));
    PMD_RESULT(PMDGetCommandedVelocity      (phAxis,  &velocity));
    PMD_RESULT(PMDGetCommandedAcceleration  (phAxis,  (PMDint32*)&acceleration));

    // Servo Filter
    PMD_RESULT(PMDGetMotorLimit             (phAxis,  &limit16));
    PMD_RESULT(PMDSetMotorLimit             (phAxis,  limit16));
    PMD_RESULT(PMDGetMotorBias              (phAxis,  &bias));
    PMD_RESULT(PMDSetMotorBias              (phAxis,  bias));
    PMD_RESULT(PMDGetPositionErrorLimit     (phAxis,  &limit));
    PMD_RESULT(PMDSetPositionErrorLimit     (phAxis,  limit));
    PMD_RESULT(PMDGetSettleTime             (phAxis,  &settleTime));
    PMD_RESULT(PMDSetSettleTime             (phAxis,  settleTime));
    PMD_RESULT(PMDGetSettleWindow           (phAxis,  &settleWindow));
    PMD_RESULT(PMDSetSettleWindow           (phAxis,  settleWindow));
    PMD_RESULT(PMDGetTrackingWindow         (phAxis,  &trackingWindow));
    PMD_RESULT(PMDSetTrackingWindow         (phAxis,  trackingWindow));
    PMD_RESULT(PMDGetMotionCompleteMode     (phAxis,  &mode));
    PMD_RESULT(PMDSetMotionCompleteMode     (phAxis,  mode));
    PMD_RESULT(PMDGetPositionError          (phAxis, &positionError));
    PMD_RESULT(PMDClearPositionError        (phAxis));
    
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopKp                , &kp));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopKi                , &ki));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopIlimit            , &integrallimit));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopKd                , &kd));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopDerivativeTime    , &derivativeTime));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopKout              , &kout));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopKvff              , &kvff));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopKaff              , &kaff));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopBiquad1Enable     , &biquad1Enable));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopBiquad1B0         , &biquad1B0));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopBiquad1B1         , &biquad1B1));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopBiquad1B2         , &biquad1B2));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopBiquad1A1         , &biquad1A1));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopBiquad1A2         , &biquad1A2));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopBiquad1K          , &biquad1K));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopBiquad2Enable     , &biquad2Enable));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopBiquad2B0         , &biquad2B0));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopBiquad2B1         , &biquad2B1));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopBiquad2B2         , &biquad2B2));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopBiquad2A1         , &biquad2A1));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopBiquad2A2         , &biquad2A2));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  PMDPositionLoopBiquad2K          , &biquad2K));

    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopKp                , kp));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopKi                , ki));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopIlimit            , integrallimit));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopKd                , kd));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopDerivativeTime    , derivativeTime));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopKout              , kout));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopKvff              , kvff));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopKaff              , kaff));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopBiquad1Enable     , biquad1Enable));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopBiquad1B0         , biquad1B0));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopBiquad1B1         , biquad1B1));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopBiquad1B2         , biquad1B2));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopBiquad1A1         , biquad1A1));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopBiquad1A2         , biquad1A2));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopBiquad1K          , biquad1K));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopBiquad2Enable     , biquad2Enable));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopBiquad2B0         , biquad2B0));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopBiquad2B1         , biquad2B1));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopBiquad2B2         , biquad2B2));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopBiquad2A1         , biquad2A1));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopBiquad2A2         , biquad2A2));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  PMDPositionLoopBiquad2K          , biquad2K));
        
    PMD_RESULT(PMDGetPositionLoopValue      (phAxis,  PMDPositionLoopValueNodeIntegratorSum, &integral));
    PMD_RESULT(PMDGetPositionLoopValue      (phAxis,  PMDPositionLoopValueNodeIntegralContribution, &integral));
    PMD_RESULT(PMDGetPositionLoopValue      (phAxis,  PMDPositionLoopValueNodeDerivative, &derivative));
    PMD_RESULT(PMDGetPositionLoopValue      (phAxis,  PMDPositionLoopValueNodeBiquad1Input, &biquad));
    PMD_RESULT(PMDGetPositionLoopValue      (phAxis,  PMDPositionLoopValueNodeBiquad2Input, &biquad));
    
    PMD_RESULT(PMDGetSampleTime             (phAxis,  &sampleTime));
    PMD_RESULT(PMDSetSampleTime             (phAxis,  sampleTime));

    // Parameter Update & Breakpoints
    PMD_RESULT(PMDGetBreakpoint             (phAxis,  breakpointID,  &breakAxis,  &action,  &trigger));
    PMD_RESULT(PMDSetBreakpoint             (phAxis,  breakpointID,  breakAxis,  action,  trigger));
    PMD_RESULT(PMDGetBreakpointValue        (phAxis,  breakpointID,  &breakpointValue));
    PMD_RESULT(PMDSetBreakpointValue        (phAxis,  breakpointID,  breakpointValue));
    PMD_RESULT(PMDUpdate                    (phAxis));

    // Interrupt Processing
    PMD_RESULT(PMDGetInterruptMask          (phAxis,  &interruptMask));
    PMD_RESULT(PMDSetInterruptMask          (phAxis,  interruptMask));
    PMD_RESULT(PMDClearInterrupt            (phAxis));
    PMD_RESULT(PMDGetInterruptAxis          (phAxis,  &interruptingAxisMask));

    // Status Register Control
    PMD_RESULT(PMDGetEventStatus            (phAxis,  &eventStatus));
    PMD_RESULT(PMDResetEventStatus          (phAxis,  eventStatus));
    PMD_RESULT(PMDGetActivityStatus         (phAxis,  &activityStatus));
    PMD_RESULT(PMDGetSignalSense            (phAxis,  &mask));
    PMD_RESULT(PMDSetSignalSense            (phAxis,  mask));
    PMD_RESULT(PMDGetSignalStatus           (phAxis,  &signalStatus));

    // External Memory
    PMD_RESULT(PMDGetBufferStart            (phAxis,  bufferID,  (PMDuint32*)&value32));
    PMD_RESULT(PMDSetBufferStart            (phAxis,  bufferID,  value32));
    PMD_RESULT(PMDGetBufferLength           (phAxis,  bufferID,  &bufferLength));
    // the default buffer length is 0 in some cases and we need a buffer length of at least 1 to read/write.
    PMD_RESULT(PMDSetBufferLength           (phAxis,  bufferID,  1));
    PMD_RESULT(PMDReadBuffer                (phAxis,  bufferID,  &data));
    PMD_RESULT(PMDWriteBuffer               (phAxis,  bufferID,  data));
    PMD_RESULT(PMDGetBufferWriteIndex       (phAxis,  bufferID,  &writeIndex));
    PMD_RESULT(PMDSetBufferWriteIndex       (phAxis,  bufferID,  writeIndex));
    PMD_RESULT(PMDGetBufferReadIndex        (phAxis,  bufferID,  &readIndex));
    PMD_RESULT(PMDSetBufferReadIndex        (phAxis,  bufferID,  readIndex));

    // Trace Operations
    PMD_RESULT(PMDGetTraceMode              (phAxis,  &mode));
    PMD_RESULT(PMDSetTraceMode              (phAxis,  mode));
    PMD_RESULT(PMDGetTracePeriod            (phAxis,  &tracePeriod));
    PMD_RESULT(PMDSetTracePeriod            (phAxis,  tracePeriod));
    PMD_RESULT(PMDGetTraceVariable          (phAxis,  traceID,  &traceAxis,  &variable));
    PMD_RESULT(PMDSetTraceVariable          (phAxis,  traceID,  traceAxis,  variable));
    PMD_RESULT(PMDGetTraceStart             (phAxis,  &traceAxis,  &condition,  &bit,  &state));
    PMD_RESULT(PMDSetTraceStart             (phAxis,  traceAxis,  condition,  bit,  state));
    PMD_RESULT(PMDGetTraceStop              (phAxis,  &traceAxis,  &condition,  &bit,  &state));
    PMD_RESULT(PMDSetTraceStop              (phAxis,  traceAxis,  condition,  bit,  state));
    PMD_RESULT(PMDGetTraceStatus            (phAxis,  &status));
    PMD_RESULT(PMDGetTraceCount             (phAxis,  &count));


    PMD_RESULT(PMDGetBreakpointUpdateMask   (phAxis,  breakpointID, &mask));
    PMD_RESULT(PMDSetBreakpointUpdateMask   (phAxis,  breakpointID,  mask));
    PMD_RESULT(PMDGetPositionLoop           (phAxis,  param, &value32));
    PMD_RESULT(PMDSetPositionLoop           (phAxis,  param,  value32));
    PMD_RESULT(PMDGetPositionLoopValue      (phAxis,  param, &value32));
    PMD_RESULT(PMDGetOperatingMode          (phAxis, &mode));
    PMD_RESULT(PMDSetOperatingMode          (phAxis,  mode));
    PMD_RESULT(PMDGetActiveOperatingMode    (phAxis, &mode));
    PMD_RESULT(PMDRestoreOperatingMode      (phAxis));
    PMD_RESULT(PMDGetHoldingCurrent         (phAxis,  0, (PMDuint16*)&value));
    PMD_RESULT(PMDSetHoldingCurrent         (phAxis,  0,  value));
    PMD_RESULT(PMDGetAxisOutMask            (phAxis, &sourceAxis, &sourceRegister, &selectionMask, &senseMask));
    PMD_RESULT(PMDSetAxisOutMask            (phAxis,  sourceAxis, sourceRegister, selectionMask, senseMask));
    PMD_RESULT(PMDGetEventAction            (phAxis,  1, &eventaction));
    PMD_RESULT(PMDSetEventAction            (phAxis,  1,  eventaction));
    PMD_RESULT(PMDGetUpdateMask             (phAxis, &mask));
    PMD_RESULT(PMDSetUpdateMask             (phAxis,  mask));

    PMD_RESULT(PMDNoOperation               (phAxis));
    PMD_RESULT(PMDGetVersion                (phAxis,  &generation,  &motorType,  &numberAxes,  &chip_count, &custom,  &major,  &minor));
    PMD_RESULT(PMDGetInstructionError       (phAxis,  &data16)); 
    PMD_RESULT(PMDGetSerialPortMode         (phAxis,  &baud,  &parity,  &stopBits,  &protocol,  &multiDropID));
    PMD_RESULT(PMDSetSerialPortMode         (phAxis,  baud,  parity,  stopBits,  protocol,  multiDropID));
    PMD_RESULT(PMDGetTime                   (phAxis,  &numberServoCycles));
    PMD_RESULT(PMDGetChecksum               (phAxis,  &checksum));
    PMD_RESULT(PMDReadAnalog                (phAxis,  analogID,  &data16));

    // Miscellaneous
    if (generation == PMDFamilyMagellan)
    {
        PMD_RESULT(PMDGetEncoderModulus         (phAxis,  &data16));
        PMD_RESULT(PMDSetEncoderModulus         (phAxis,  data16));
        PMD_RESULT(PMDReadIO                    (phAxis,  0,  &data16));
        PMD_RESULT(PMDWriteIO                   (phAxis,  0,  data16));
        PMD_RESULT(PMDGetSPIMode                (phAxis,  &mode));
        PMD_RESULT(PMDSetSPIMode                (phAxis,  mode));
        PMD_RESULT(PMDGetStepRange              (phAxis,  &data16));
        PMD_RESULT(PMDSetStepRange              (phAxis,  data16));
        PMD_RESULT(PMDGetSynchronizationMode    (phAxis,  &mode));
        PMD_RESULT(PMDSetSynchronizationMode    (phAxis,  mode));
        PMD_RESULT(PMDGetOutputMode             (phAxis,  &mode));
        PMD_RESULT(PMDSetOutputMode             (phAxis,  mode));
        PMD_RESULT(PMDMultiUpdate               (phAxis,  0));
    }

    PMDprintf("All commands executed.\r\n");
}

// ****************************************************************************
// Execute a trajectory.
// It is assumed at this point that the motor is properly commutating 
// and the position loop is tuned (if applicable).
void ProfileMove(PMDAxisHandle* phAxis)
{
    PMDresult result;
    PMDuint16 status;
    PMDuint16 opmode;
    PMDint32  position;
    PMDint32  captureValue;
    PMDuint32 timeoutms = 10000;
    PMDuint16 eventmask = PMDEventStatusMotionComplete | PMDEventStatusMotionError;
    int bWaitForPositionCapture = 0;

    PMDprintf("Executing the profile...\r\n");

    // enable the trajectory operating mode
    PMD_RESULT(PMDGetOperatingMode(phAxis, &opmode));
    opmode |= PMDOperatingModeTrajectoryEnabled;
    PMD_RESULT(PMDSetOperatingMode(phAxis, opmode));

    // reset commanded position to 0 so this example can be repeated.
    PMDSetActualPosition(phAxis,0);
    PMDClearPositionError(phAxis);
    PMD_RESULT(PMDUpdate( phAxis ));
    // TO DO:   Update with the appropriate parameter values
    //          before executing this code
    PMDSetProfileMode( phAxis, PMDProfileModeTrapezoidal );
    PMDSetPosition( phAxis, 3000 );
    PMDSetVelocity( phAxis, 32768 );
    PMDSetAcceleration( phAxis, 256 );
    PMDSetJerk( phAxis, 65535 );

    PMDResetEventStatus( phAxis, (PMDuint16)~eventmask );
    PMDGetCaptureValue(phAxis,  &captureValue); // reset the position capture mechanism
    PMD_RESULT(PMDUpdate( phAxis ));

    // the following section demonstrates waiting for a position capture event from the Magellan
    if (bWaitForPositionCapture)
    {
        eventmask = PMDEventStatusCaptureReceived | PMDEventStatusMotionError;
        PMD_RESULT(WaitForEvent(phAxis, eventmask, 1000))
        PMD_RESULT(PMDGetCaptureValue(phAxis,  &captureValue));
        PMDprintf("Capture value: %ld\r\n", captureValue);
    }
    // now wait for motion profile to complete
    PMD_RESULT(WaitForEvent(phAxis, eventmask, timeoutms))
   
    PMD_RESULT(PMDGetEventStatus(phAxis, &status));
    PMDprintf("Event Status: %4X\r\n",status);

    PMD_RESULT(PMDGetActualPosition(phAxis, &position));
    PMDprintf("Actual Position: %ld\r\n", position);
}

// ****************************************************************************
// Demonstrate the use of PMDWaitForEvent which uses the Magellan axis interrupt feature.
// This feature only works on the local, CAN or PC104 connected Magellan.
void PMDWaitForEventExample()
{
    PMDresult result;
    PMDDeviceHandle hDevice;
    PMDAxisHandle hAxis1;
    PMDAxisHandle* phAxis = &hAxis1;
    PMDuint32 timeoutms = 10000;
    PMDuint32 time;
    PMDEvent event;
    PMDuint16 eventmask = PMDEventStatusBreakpoint1;
    PMDuint16 breakpointID = PMDBreakpoint1; 
    PMDAxis breakAxis = PMDAxis1; 
    PMDuint8 action = PMDBreakpointActionNone; 
    PMDuint8 trigger = PMDBreakpointTriggerTime;
    PMDint32 breakpointValue;

    PMDprintf("Setting a time breakpoint to trigger an interrupt...\r\n");

    PMD_RESULT(PMDMPDeviceOpen(&hDevice, NULL))	// Open a handle to the local Magellan device 
    PMDAxisOpen(&hAxis1, &hDevice, PMDAxis1 );
    PMDResetEventStatus( phAxis, 0 );
    PMDSetInterruptMask( phAxis, eventmask );
    PMDGetTime(phAxis, &time);
    breakpointValue = time + 1000;
    PMDSetBreakpointValue(phAxis,  breakpointID,  breakpointValue);
    PMDSetBreakpoint(phAxis,  breakpointID,  breakAxis,  action,  trigger);

    // now wait for breakpoint
    PMD_RESULT(PMDWaitForEvent(&hDevice, &event, timeoutms))
    if (result == PMD_ERR_OK)
        PMDprintf("Time breakpoint occured. Event 0x%04X on axis %d\r\n", event.event, event.axis+1);
}

#define BUFSIZE 5

// ****************************************************************************
void MemoryExample(PMDDeviceHandle* phDevice, PMDMemoryType memtype)
{
    PMDresult result;
    PMDuint32 readdata[BUFSIZE];
    PMDuint32 writedata[BUFSIZE];
    PMDuint32 offset_in_dwords = 0;
    PMDuint32 dwords_to_read = BUFSIZE;
    PMDMemoryHandle hMemory;
    PMDuint32 i;
        
    PMD_RESULT(PMDMemoryOpen(&hMemory, phDevice, PMDDataSize_32Bit, memtype));
    if (result == PMD_NOERROR)
    {
        memset(readdata, 0, sizeof(readdata));
        PMDprintf("\r\nRead current contents of memory\r\n");
        PMD_RESULT(PMDMemoryRead(&hMemory, readdata, offset_in_dwords, dwords_to_read));

        for (i=0; i<dwords_to_read; i++)
            PMDprintf("data[%d] = %08lX\r\n", i, readdata[i]);

        for (i=0; i<dwords_to_read; i++)
            writedata[i] = i+1;

        PMD_RESULT(PMDMemoryWrite(&hMemory, writedata, offset_in_dwords, dwords_to_read));

        PMDprintf("Read modified contents of memory\r\n");
        PMD_RESULT(PMDMemoryRead(&hMemory, readdata, offset_in_dwords, dwords_to_read));

        for (i=0; i<dwords_to_read; i++)
            PMDprintf("data[%d] = %08lX\r\n", i, readdata[i]);

        // reset contents to 0
        memset(writedata, 0, sizeof(writedata));
        PMD_RESULT(PMDMemoryWrite(&hMemory, writedata, offset_in_dwords, dwords_to_read));
    }
    else
        PMDprintf("DPRAM MemOpen result 0x%x\r\n",result);
    PMDMemoryClose(&hMemory);
    
}


// ****************************************************************************
void DeviceFunctions(PMDDeviceHandle* phDevice)
{
    PMDresult result;
    PMDuint32 major;
    PMDuint32 minor;
    PMDuint32 version;
    PMDuint32 defaultvalue;
    PMDuint16 resetcause;
    PMDuint16 resetmask = 0;
    PMDTaskState state;
    PMDPeriphHandle hPeriphConsole; 
    char str[100];

//  PMD_RESULT(PMDDeviceReset         (phDevice))
//  PMDTaskWait(3000);
    PMD_INITIALIZEHANDLE(hPeriphConsole) // this is mainly to prevent "unreferenced local variable" compiler warning

    PMD_RESULT(PMDDeviceGetVersion    (phDevice, &major, &minor))
    PMDprintf("CME device version %d.%d\r\n", major, minor);

    PMD_RESULT(PMDDeviceGetDefault      (phDevice, PMD_Default_IPAddress, &defaultvalue, PMDDataSize_32Bit));
    PMDprintf("PMDDeviceGetDefault(PMD_Default_IPAddress) = %08lX\r\n", defaultvalue);
//  PMD_RESULT(PMDDeviceSetDefault      (phDevice, PMD_Default_IPAddress, defaultvalue, PMDDataSize_32Bit));
    PMD_RESULT(PMDDeviceGetResetCause   (phDevice, &resetcause, resetmask))
    PMDprintf("PMDDeviceGetResetCause = 0x%04lX\r\n", resetcause);
    PMD_RESULT(PMDDeviceNoOperation     (phDevice))
    PMD_RESULT(PMDGetUserCodeFileVersion(phDevice, &version))
    PMDprintf("PMDGetUserCodeFileVersion = 0x%08lX\r\n", version);
    PMD_RESULT(PMDGetUserCodeFileName   (phDevice, str))
    PMDprintf("PMDGetUserCodeFileName = %s\r\n", str);
    PMD_RESULT(PMDGetUserCodeFileDate   (phDevice, str))
    PMDprintf("PMDGetUserCodeFileDate = %s\r\n", str);
    PMD_RESULT(PMDTaskGetState          (phDevice, &state))

#if defined CME & 0
    // Open the 2nd serial port to be the console output port
    PMD_RESULT(PMDPeriphOpenCOM       (&hPeriphConsole, phDevice, PMDSerialPort2, 115200, PMDSerialParityNone, PMDSerialStopBits1))
    // or open a UDP port to be the console output port
//  PMD_RESULT(PMDPeriphOpenUDP       (&hPeriphConsole, phDevice, PMD_IP4_ADDR(192,168,2,1), DEFAULT_ETHERNET_PORT ))
    PMD_RESULT(PMDDeviceSetConsole    (phDevice, &hPeriphConsole))
    PMDprintf("This text should be output on the new console output port\r\n");
    // Disable console output
    // If this is not called and the console handle (hPeriphConsole) is not static 
    PMD_RESULT(PMDDeviceSetConsole    (phDevice, NULL))
    PMD_RESULT(PMDPeriphClose(&hPeriphConsole));
    PMDprintf("This text should not be output to the console\r\n");
#endif
}


// ****************************************************************************
// AtlasCommands calls functions that return an Atlas specific value when 
// sent to the Atlas instead of the Magellan.
void AtlasCommands(PMDAxisHandle* phAxis)
{
    PMDAxisHandle hAtlasAxis;
    PMDuint16 generation, motorType, numberAxes, chip_count, custom, major, minor;
    PMDuint16 status;
    PMDuint16 udata16;
    PMDint16  idata16;
    PMDuint32 udata32;
    PMDresult result;
    PMDuint16 mode;
    PMDuint16 bufferID = 0; 
    PMDuint32 bufferLength;
    PMDuint32 writeIndex;
    PMDuint32 readIndex;
    PMDuint16 tracePeriod;
    PMDuint16 traceID = PMDTraceVariableNumber1; 
    PMDuint8 variable;
    PMDAxis traceAxis; 
    PMDuint8 condition; 
    PMDuint8 bit; 
    PMDuint8 state;
    PMDuint32 count;

    PMD_RESULT(PMDGetOutputMode                 (phAxis,  &mode));
    if (mode == PMDOutputModeAtlas)
    {
        // get a handle to the Atlas
        PMDAtlasAxisOpen(phAxis, &hAtlasAxis);

        PMD_RESULT(PMDGetVersion                (&hAtlasAxis,  &generation,  &motorType,  &numberAxes,  &chip_count, &custom,  &major,  &minor));
        PMD_RESULT(PMDGetInstructionError       (&hAtlasAxis,  &udata16)); 
        PMD_RESULT(PMDGetTime                   (&hAtlasAxis,  &udata32));
        PMD_RESULT(PMDGetMotorType              (&hAtlasAxis,  &udata16));
        PMD_RESULT(PMDGetSignalStatus           (&hAtlasAxis,  &status));
        PMD_RESULT(PMDNoOperation               (&hAtlasAxis));

        // External Memory
        PMD_RESULT(PMDGetBufferLength           (&hAtlasAxis,  bufferID,  &bufferLength));
        PMD_RESULT(PMDSetBufferLength           (&hAtlasAxis,  bufferID,  bufferLength));
        PMD_RESULT(PMDReadBuffer16              (&hAtlasAxis,  bufferID,  &idata16));
        PMD_RESULT(PMDGetBufferWriteIndex       (&hAtlasAxis,  bufferID,  &writeIndex));
        PMD_RESULT(PMDSetBufferWriteIndex       (&hAtlasAxis,  bufferID,  writeIndex));
        PMD_RESULT(PMDGetBufferReadIndex        (&hAtlasAxis,  bufferID,  &readIndex));
        PMD_RESULT(PMDSetBufferReadIndex        (&hAtlasAxis,  bufferID,  readIndex));


        // Trace Operations
        PMD_RESULT(PMDGetTraceMode              (&hAtlasAxis,  &mode));
        PMD_RESULT(PMDSetTraceMode              (&hAtlasAxis,  mode));
        PMD_RESULT(PMDGetTracePeriod            (&hAtlasAxis,  &tracePeriod));
        PMD_RESULT(PMDSetTracePeriod            (&hAtlasAxis,  tracePeriod));
        PMD_RESULT(PMDGetTraceVariable          (&hAtlasAxis,  traceID,  &traceAxis,  &variable));
        PMD_RESULT(PMDSetTraceVariable          (&hAtlasAxis,  traceID,  traceAxis,  variable));
        PMD_RESULT(PMDGetTraceStart             (&hAtlasAxis,  &traceAxis,  &condition,  &bit,  &state));
        PMD_RESULT(PMDSetTraceStart             (&hAtlasAxis,  traceAxis,  condition,  bit,  state));
        PMD_RESULT(PMDGetTraceStop              (&hAtlasAxis,  &traceAxis,  &condition,  &bit,  &state));
        PMD_RESULT(PMDSetTraceStop              (&hAtlasAxis,  traceAxis,  condition,  bit,  state));
        PMD_RESULT(PMDGetTraceStatus            (&hAtlasAxis,  &status));
        PMD_RESULT(PMDGetTraceCount             (&hAtlasAxis,  &count));

    
        PMD_RESULT(PMDReset                     (&hAtlasAxis));

        WaitForAtlasToConnect(phAxis);
    }
}

#define NUMDEVICES 2
// This function will connect to multiple Magellens/IONs that are on a RS485 or CAN network.
// The network node ID starts at 0.
// Set hDevice to NULL when calling from CME code.
void ConnectToNetworkedMagellans(PMDDeviceHandle hDevice)
{
    PMDresult result;
    PMDAxisHandle hAxis1ION[NUMDEVICES];
    PMDDeviceHandle hDeviceION[NUMDEVICES];
    // the peripheral handle object
    PMDPeriphHandle hPeriphParent;
    PMDPeriphHandle hPeriphAddress[NUMDEVICES];
    PMDuint32 version;
    PMDuint8 baud; 
    PMDuint8 parity; 
    PMDuint8 stopBits; 
    PMDuint8 protocol; 
    PMDuint8 multiDropID;
    PMDuint8 nodeID;
    PMDuint8 transmission_rate;
    int bSerial = 1;
    int NumDevices = NUMDEVICES;
    int i;


    if (bSerial)
    {
        PMD_RESULT(PMDPeriphOpenCOM( &hPeriphParent, &hDevice, PMDSerialPort1, 57600, PMDSerialParityNone, PMDSerialStopBits1))
        for (i=0; i<NumDevices; i++)
            PMD_RESULT(PMDPeriphOpenMultiDrop( &hPeriphAddress[i], &hPeriphParent, i))
    }
    else
    {
        for (i=0; i<NumDevices; i++)
            PMD_RESULT(PMDPeriphOpenCAN( &hPeriphAddress[i], &hDevice, 0x600+i, 0x580+i, 0))
    }

    for (i=0; i<NumDevices; i++) 
    {
        PMD_RESULT(PMDMPDeviceOpen(&hDeviceION[i], &hPeriphAddress[i]));  // Open a device handle to an ION at RS485 address i
        PMDAxisOpen(&hAxis1ION[i], &hDeviceION[i], PMDAxis1 );

        PMD_RESULT(PMDGetVersion32(&hAxis1ION[i], &version))
        PMDprintf("ION %d Version=%08X\r\n", i, version);

        PMD_RESULT(PMDGetSerialPortMode(&hAxis1ION[i], &baud, &parity, &stopBits, &protocol, &multiDropID))
        PMDprintf("PMDGetSerialPortMode for ION %d : multiDropID is %d\r\n", i, multiDropID);
        PMD_RESULT(PMDGetCANMode(&hAxis1ION[i], &nodeID, &transmission_rate))
        PMDprintf("PMDGetCANMode for ION %d : nodeID is %d\r\n", i, nodeID);
        PMDDeviceClose(&hDeviceION[i]);
    }

    if (bSerial)
        PMDPeriphClose(&hPeriphParent);
}

