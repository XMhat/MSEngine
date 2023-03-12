/* == TIMER.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This class is the manager for the frame time.                       ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfTimer {                    // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfClock;               // Using util namespace
using namespace IfCVarDef;             // Using cvardef namespace
/* -- Variables ------------------------------------------------------------ */
static ClkTimePoint ctStart{ milliseconds{0} },    // Start of frame time
                    ctTimeOut{ ctStart },          // Time script times out
                    ctEnd{ ctStart };              // End of frame time
static ClkDuration  cdDuration{ milliseconds{0} }, // Frame duration
                    cdAcc{ cdDuration },           // Accumulator duration
                    cdLimit{ cdDuration },         // Frame limit
                    cdDelay{ cdDuration },         // Delay duration
                    cdDelayPst{ cdDuration },      // Persistent delay duration
                    cdTimeOut{ cdDuration };       // Frame timeout duration
static uint64_t     qTriggers = 0,      // Number of frame timeout checks
                    qTicks    = 0;      // Number of ticks processed this sec
static bool         bWait     = false;  // Force wait?
/* -- Set engine tick rate ------------------------------------------------- */
static void TimerSetInterval(const double fdNInt)
  { cdLimit = duration_cast<ClkDuration>(duration<double>{ 1 / fdNInt }); }
/* -- Set global target fps ------------------------------------------------ */
static CVarReturn TimerTickRateModified(const unsigned int uiNewInterval)
{ // Ignore invalid values
  if(uiNewInterval < 2 || uiNewInterval > 200) return DENY;
  // Set new interval
  TimerSetInterval(static_cast<double>(uiNewInterval));
  // Success
  return ACCEPT;
}
/* -- Update delay as double ----------------------------------------------- */
static void TimerUpdateDelay(const unsigned int uiNewDelay)
  { cdDelay = milliseconds{ uiNewDelay }; }
/* -- Forces a delay internally if delay is disabled --------------------- */
static void TimerSetDelayIfZero(void)
  { if(cdDelay != seconds(0)) TimerUpdateDelay(1); }
/* -- Restore saved persistent delay timer --------------------------------- */
static void TimerRestoreDelay(void) { cdDelay = cdDelayPst; }
/* -- TimerSetDelay -------------------------------------------------------- */
static CVarReturn TimerSetDelay(const unsigned int uiNewDelay)
{ // Ignore if set over one second, any sort of app would not be usable with
  // over one second delay, so might as well cap it
  if(uiNewDelay > 1000) return DENY;
  // Set new delay
  TimerUpdateDelay(uiNewDelay);
  // Update persistent delay
  cdDelayPst = cdDelay;
  // Success
  return ACCEPT;
}
/* -- Do time calculations ----------------------------------------------- */
static void TimerCalculateTime(void)
{ // Set end time
  ctEnd = cmHiRes.GetTime();
  // Get duration we had to wait since end of last game loop
  cdDuration = ctEnd - ctStart;
  // Set new start time
  ctStart = ctEnd;
  // Set new timeout time
  ctTimeOut = ctEnd + cdTimeOut;
}
/* -- Thread suspense by duration ------------------------------------------ */
static void TimerSuspend(const ClkDuration cdAmount)
  { std::this_thread::sleep_for(cdAmount); }
/* -- Thread suspense by integer ------------------------------------------- */
static void TimerSuspend(const unsigned int uiMilliseconds)
  { TimerSuspend(milliseconds(uiMilliseconds)); }
/* -- Thread suspense by one millisecond ----------------------------------- */
static void TimerSuspend(void) { TimerSuspend(1); }
/* -- Thread suspense by requested duration -------------------------------- */
static void TimerSuspendRequested(void) { TimerSuspend(cdDelay); }
/* -- Force wait if delay is disabled (cFboMain->Render()) ----------------- */
static void TimerForceWait(void)
  { if(cdDelay == seconds(0)) bWait = true; }
/* -- Calculate time elapsed since c++  ------------------------------------ */
static void TimerUpdateBot(void)
{ // Sleep if theres a delay
  TimerSuspendRequested();
  // Calculate current time using stl
  TimerCalculateTime();
  // Increment ticks
  ++qTicks;
}
/* -- Calculate time elapsed since end of last loop ------------------------ */
static void TimerUpdateInteractive(void)
{ // Calculate frame time using glfw
  TimerCalculateTime();
  // Increase accumulator by frame time
  cdAcc += cdDuration;
}
/* -- Should execute a game tick? ------------------------------------------ */
static bool TimerShouldTick(void)
{ // Frame limit not reached?
  if(cdAcc < cdLimit)
  { // Force if we're forced to wait
    if(bWait) TimerSuspend();
    // Or wait a little bit if we can
    else TimerSuspendRequested();
    // Suspend engine thread for the requested delay
    return false;
  } // Reduce accumulator
  cdAcc -= cdLimit;
  // Increase number of ticks rendered
  ++qTicks;
  // We are no longer forced to wait
  bWait = false;
  // Tick loop should wait more
  return true;
}
/* -- Reset counters and re-initialise start and end time ------------------ */
static void TimerCatchup(void)
{ // Reset accumulator and duration
  cdAcc = cdDuration = seconds{ 0 };
  // Update new start and end time
  ctStart = ctEnd = cmHiRes.GetTime();
}
/* -- TimerSetDelay -------------------------------------------------------- */
static CVarReturn TimerSetTimeOut(const unsigned int uiTimeOut)
  { return CVarSimpleSetIntNL(cdTimeOut, seconds{ uiTimeOut }, seconds{ 1 }); }
/* -- Return members ------------------------------------------------------- */
static bool TimerIsTimedOut(void)
  { ++qTriggers; return cmHiRes.GetTime() >= ctTimeOut; }
static uint64_t TimerGetTriggers(void) { return qTriggers; }
static double TimerGetTimeOut(void)
  { return ClockDurationToDouble(cdTimeOut); }
static double TimerGetLimit(void) { return ClockDurationToDouble(cdLimit); }
static double TimerGetAccumulator(void)
  { return ClockDurationToDouble(cdAcc); }
static double TimerGetDuration(void)
  { return ClockDurationToDouble(cdDuration); }
static double TimerGetSecond(void) { return 1.0 / TimerGetDuration(); }
static uint64_t TimerGetTicks(void) { return qTicks; }
static double TimerGetDelay(void) { return ClockDurationToDouble(cdDelay); }
static double TimerGetStart(void)
  { return ClockDurationToDouble(ctStart.time_since_epoch()); }
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
