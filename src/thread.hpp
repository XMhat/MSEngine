/* == THREAD.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module handles the ability to manage and spawn multiple        ## */
/* ## threads for parallel and asynchronous execution.                    ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfThread {                   // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfLog;                 // Using log interface
using namespace IfCollector;           // Using collector interface
/* == Thread collector class with global thread id counter ================= */
BEGIN_COLLECTOREX(Threads, Thread, CLHelperSafe,
  uint64_t         qwNextId;           /* Unique thread id                   */
  SafeSizeT        stRunning           /* Number of threads running          */
);/* == Function that returns the next unique thread id ==================== */
static uint64_t ThreadGetNextId(void) { return ++cThreads->qwNextId; }
/* == Thread variables class =============================================== */
class ThreadVariables                  // Thread variables class
{ /* -- Private typedefs ---------------------------------------- */ protected:
  typedef int (CBFuncT)(Thread&);      // Thread callback function
  /* -- Public typedefs -------------------------------------------- */ public:
  typedef function<CBFuncT> CBFunc;    // Wrapped inside a function class
  /* -- Private variables --------------------------------------- */ protected:
  SafeUInt64       qwThreadId;         // Unique thread identifier
  SafeInt          iExitCode;          // Callback exit code
  void            *vpParam;            // User parameter
  CBFunc           threadCallback;     // Thread callback function
  SafeBool         bShouldExit;        // Thread should exit
  SafeBool         bRunning;           // Thread is running?
  SafeClkDuration  duStartTime;        // Thread start time
  SafeClkDuration  duEndTime;          // Thread end time
  /* -- Constructor -------------------------------------------------------- */
  ThreadVariables(const uint64_t qwI,  // Thread unique identifier
                  void*const vpP,      // Thread user parameter
                  const CBFunc &cbF) : // Thread callback function
    /* -- Initialisation of members ---------------------------------------- */
    qwThreadId(qwI),                   // Set unique identifier
    iExitCode(0),                      // Set exit code to standby
    vpParam(vpP),                      // Set user thread parameter
    threadCallback{ cbF },             // Set thread callback function
    bShouldExit(false),                // Should never exit at first
    bRunning(false),                   // Should never be running at first
    duStartTime{ seconds{ 0 } },       // Never started time
    duEndTime{ seconds{ 0 } }          // Never finished time
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
};/* ----------------------------------------------------------------------- */
/* == Thread object class ================================================== */
BEGIN_MEMBERCLASS(Threads, Thread, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Ident,                        // Object name
  public ThreadVariables               // Thread variables class
{ /* -- Thread class ---------------------------------------------- */ private:
  thread           tThread;            // The C++11 thread
  /* -- Thread handler function -------------------------------------------- */
  void ThreadHandler(void) try
  { // Incrememt thread running count
    ++cParent.stRunning;
    // Thread longer running
    bRunning = true;
    // Thread starting up in log
    LW(LH_DEBUG, "Thread $<$> started.", ThreadGetId(), IdentGet());
    // Set the start time and initialise the end time
    duStartTime = cmHiRes.GetEpochTime();
    duEndTime = seconds(0);
    // Loop forever until thread should exit
    while(ThreadShouldNotExit())
    { // Set exit code to -1 as a reference that execution is proceeding
      ThreadSetExitCode(-1);
      // Run thread and capture result
      ThreadSetExitCode(ThreadGetCallback()(*this));
      // If non-zero then break else start thread again
      if(ThreadGetExitCode()) break;
    } // Thread no longer running
    bRunning = false;
    // Set shutdown time
    duEndTime = cmHiRes.GetEpochTime();
    // Log if thread didn't signal to exit
    LW(LH_DEBUG, "Thread $<$> exited in $ with code $<$$>.",
      ThreadGetId(), IdentGet(),
      ToShortDuration(ClockTimePointRangeToClampedDouble(ThreadGetEndTime(),
        ThreadGetStartTime())),
      ThreadGetExitCode(), hex, ThreadGetExitCode());
    // Reduce thread count
    --cParent.stRunning;
  } // exception occured in thread so handle it
  catch(const exception &e)
  { // Thread no longer running
    bRunning = false;
    // Return error and set thread to exit
    ThreadSetExitCode(-2);
    // Reduce thread count
    --cParent.stRunning;
    // Set shutdown time
    duEndTime = cmHiRes.GetEpochTime();
    // Log if thread didn't signal to exit
    LW(LH_ERROR, "Thread $<$> exited in $ due to exception: $!",
      ThreadGetId(), IdentGet(),
      ToShortDuration(ClockTimePointRangeToClampedDouble(ThreadGetEndTime(),
        ThreadGetStartTime())),
      e.what());
  }
  /* ----------------------------------------------------------------------- */
  static void ThreadMain(void*const vpPtr)
  { // Get pointer to thread class and if valid?
    if(Thread*const tPtr = reinterpret_cast<Thread*>(vpPtr))
    { // Set thread name in system
      SysSetThreadName(tPtr->IdentGetCStr());
      // Run the thread callback
      tPtr->ThreadHandler();
    } // Report the problem
    else LW(LH_ERROR, "Thread switch to thiscall failed with null class ptr!");
  }
  /* --------------------------------------------------------------- */ public:
  template<typename ReturnType>ReturnType ThreadGetParam(void) const
    { return reinterpret_cast<ReturnType>(vpParam); }
  template<typename ReturnType>void ThreadSetParam(const ReturnType rtParam)
    { vpParam = reinterpret_cast<void*>(rtParam); }
  /* ----------------------------------------------------------------------- */
  const ClkTimePoint ThreadGetStartTime(void) const
    { return ClkTimePoint{ duStartTime }; }
  const ClkTimePoint ThreadGetEndTime(void) const
    { return ClkTimePoint{ duEndTime }; }
  /* ----------------------------------------------------------------------- */
  int ThreadGetExitCode(void) const { return iExitCode; }
  void ThreadSetExitCode(const int iNewCode) { iExitCode = iNewCode; }
  /* ----------------------------------------------------------------------- */
  uint64_t ThreadGetId(void) const { return qwThreadId.load(); }
  /* ----------------------------------------------------------------------- */
  const CBFunc &ThreadGetCallback(void) const { return threadCallback; }
  bool ThreadHaveCallback(void) const { return !!ThreadGetCallback(); }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsParamSet(void) const { return !!vpParam; }
  /* ----------------------------------------------------------------------- */
  void ThreadSetExit(void)
  { // Ignore if already exited or already signalled to exit
    if(ThreadShouldExit()) return;
    // Set exit
    bShouldExit = true;
    // Log signalled to exit
    LW(LH_DEBUG, "Thread $<$> $ to exit.", ThreadGetId(), IdentGet(),
      ThreadIsCurrent() ? "signalling" : "signalled");
  }
  /* ----------------------------------------------------------------------- */
  void ThreadCancelExit(void) { bShouldExit = false; }
  /* ----------------------------------------------------------------------- */
  void ThreadWait(void)
  { // Wait for the thread to terminate. It is important to put this here
    // even though the thread explicitly said it's no longer executing
    // because join() HAS to be called in order for thread to be reused, and
    // join() will abort() if joinable() isn't checked as well.
    if(ThreadIsJoinable()) ThreadJoin();
    // No longer executing
    ThreadCancelExit();
  }
  /* ----------------------------------------------------------------------- */
  void ThreadStop(void)
  { // If is this thread then this is a bad idea
    if(ThreadIsCurrent())
      XC("Tried to join from the same thread!", "Identifier", IdentGet());
    // Thread is running? Inform thread loops that it should exit now
    if(ThreadIsRunning()) ThreadSetExit();
    // Wait for thread to complete
    ThreadWait();
    // Set to standby with a new thread
    tThread = thread();
  }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsCurrent(void) const
    { return ::std::this_thread::get_id() == tThread.get_id(); }
  bool ThreadIsNotCurrent(void) const { return !ThreadIsCurrent(); }
  /* ----------------------------------------------------------------------- */
  bool ThreadShouldExit(void) const { return bShouldExit; }
  bool ThreadShouldNotExit(void) const { return !ThreadShouldExit(); }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsRunning(void) const { return bRunning; }
  bool ThreadIsNotRunning(void) const { return !ThreadIsRunning(); }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsJoinable(void) const { return tThread.joinable(); }
  bool ThreadIsNotJoinable(void) const { return !ThreadIsJoinable(); }
  /* ----------------------------------------------------------------------- */
  void ThreadJoin(void) { return tThread.join(); }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsExited(void) const { return ThreadGetExitCode() != -1; }
  /* ----------------------------------------------------------------------- */
  bool ThreadIsException(void) const { return ThreadGetExitCode() == -2; }
  /* ----------------------------------------------------------------------- */
  void ThreadStart(void*const vpPtr = nullptr)
  { // Bail if thread already started
    if(ThreadIsJoinable()) return;
    // Set new thread parameters
    ThreadCancelExit();
    ThreadSetParam(vpPtr);
    ThreadSetExitCode(0);
    // Set a thread id, but not zero. Uniqueness not essential.
    qwThreadId = ThreadGetNextId();
    // Start the thread and move that thread class to this thread
    tThread = thread(ThreadMain, this);
  }
  /* -- Stop and de-initialise the thread ---------------------------------- */
  void ThreadDeInit(void)
    { ThreadStop(); this->CollectorUnregister(); }
  /* -- Initialise with callback only and register ------------------------- */
  void ThreadInit(const CBFunc &tC)
    { threadCallback = tC; this->CollectorRegister(); }
  /* -- Initialise with name and callback ---------------------------------- */
  void ThreadInit(const string &strN, const CBFunc &tC)
    { IdentSet(strN); ThreadInit(tC); }
  /* -- Initialise with name, callback, parameter and start execute -------- */
  void ThreadInit(const string &strN, const CBFunc &tC, void*const vpPtr)
    { ThreadInit(strN, tC); ThreadStart(vpPtr); }
  /* -- Initialise with callback, parameter and start execute -------------- */
  void ThreadInit(const CBFunc &tC, void*const vpPtr)
    { ThreadInit(tC); ThreadStart(vpPtr); }
  /* -- Destructor --------------------------------------------------------- */
  ~Thread(void)
  { // Done if not running
    if(ThreadIsNotJoinable()) return;
    // Not signalled to exit?
    if(ThreadIsRunning() && ThreadShouldNotExit())
    { // Set exit signal
      bShouldExit = true;
      // Log signalled to exit in destructor
      LW(LH_WARNING, "Thread $<$> signalled to exit in destructor.",
        ThreadGetId(), IdentGet());
    } // Wait to synchronise
    ThreadJoin();
  }
  /* -- Full initialise and execute constructor ---------------------------- */
  Thread(const string &strN,           // Requested Thread name
         const CBFunc &tC,             // Requested callback function
         void*const vpPtr) :           // User parameter to store
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperThread{ *cThreads, this }, // Automatic (de)registration
    Ident{ strN },                     // Initialise requested thread name
    ThreadVariables{ThreadGetNextId(), // Initialise new thread id
                    vpPtr,             // Set requested thread user parameter
                    tC},               // Set requested thread callback
    tThread{ ThreadMain, this }        // Start the thread straight away
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Standby constructor (set everything except user parameter) --------- */
  Thread(const string &strN,           // Requested Thread name
         const CBFunc &tC) :           // Requested callback function
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperThread{ *cThreads, this }, // Automatic (de)registration
    Ident{ strN },                     // Set requested identifier
    ThreadVariables{ 0, nullptr, tC }  // Just set callback function
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Standby constructor (set only name) -------------------------------- */
  explicit Thread(const string &strN) :// Requested Thread name
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperThread{ *cThreads },       // No automatic registration
    Ident{ strN },                     // Set requested identifer
    ThreadVariables{ 0, nullptr, nullptr } // Initialise nothing else
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* -- Standby constructor ------------------------------------------------ */
  Thread(void) :                       // No parameters
    /* -- Initialisation of members ---------------------------------------- */
    ICHelperThread{ *cThreads },       // No automatic registration
    ThreadVariables{ 0, nullptr, nullptr } // Initialise nothing else
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Thread);             // Disable copy constructor and operator
};/* ======================================================================= */
END_COLLECTOREX(Threads,,,,qwNextId(0),stRunning{0});
/* -- Thread sync helper --------------------------------------------------- */
template<class Callbacks>class ThreadSyncHelper : private Callbacks
{ /* -------------------------------------------------------------- */ private:
  Thread            &tOwner;           // Reference to thread owner
  SafeBool           bUnlock;          // Synchronisation should occur?
  bool               bAToB,            // Release lock for thread A?
                     bBToA;            // Release lock for thread B?
  mutex              mAToB,            // Thread A to Thread B mutex
                     mBToA;            // Thread B to Thread A mutex
  condition_variable cvAToB,           // Thread A to Thread B notification
                     cvBToA;           // Thread B to Thread A notification
  /* ----------------------------------------------------------------------- */
  void SendNotification(mutex &mM, condition_variable &cvCV, bool &bUL)
  { // Acquire unique lock
    const UniqueLock ulGuard{ mM };
    // Modify the unlock boolean
    bUL = true;
    // Send notification
    cvCV.notify_one();
  }
  /* ----------------------------------------------------------------------- */
  void WaitForThreadNotification(mutex &mM, condition_variable &cvCV,
    bool &bUL)
  { // Setup lock for condition variable
    UniqueLock uLock{ mM };
    // Wait for Thread A to notify us to continue.
    cvCV.wait(uLock, [this, &bUL]{ return bUL || tOwner.ThreadShouldExit(); });
    // Reset locked boolean
    bUL = false;
  }
  /* ----------------------------------------------------------------------- */
  void SendNotificationToThreadA(void)
    { SendNotification(mBToA, cvBToA, bBToA); }
  void WaitForThreadANotification(void)
    { WaitForThreadNotification(mAToB, cvAToB, bAToB); }
  void SendNotificationToThreadB(void)
    { SendNotification(mAToB, cvAToB, bAToB); }
  void WaitForThreadBNotification(void)
    { WaitForThreadNotification(mBToA, cvBToA, bBToA); }
  /* --------------------------------------------------------------- */ public:
  void CheckStateInThreadA(void)
  { // Ignore if other thread doesn't need action
    if(!bUnlock) return;
    // Unlock progressing
    bUnlock = false;
    // Perform starting callback from 'Callbacks' class
    this->StartThreadACallback();
    // Send notification to Thread B to say we're done initial processing
    SendNotificationToThreadB();
    // Wait for thread B to finish processing
    WaitForThreadBNotification();
    // Perform finishing callback from 'Callbacks' class
    this->FinishThreadACallback();
    // Tell thread B we've finished with the procedure
    SendNotificationToThreadB();
  }
  /* -- End notification to Thread A from Thread B ------------------------- */
  void FinishNotifyThreadA(void)
  { // Ignore if render thread isn't running
    if(tOwner.ThreadIsNotRunning()) return;
    // Call final Thread B callback
    this->FinishThreadBCallback();
    // Send notification to Thread A
    SendNotificationToThreadA();
    // Wait for Thread A to send notification back to us in Thread B
    WaitForThreadANotification();
  }
  /* -- Begin notification to Thread A from Thread B ----------------------- */
  void StartNotifyThreadA(void)
  { // Ignore if render thread isn't running
    if(tOwner.ThreadIsNotRunning()) return;
    // Set state so Thread A thread knows to proceed
    bUnlock = true;
    // Wait for Thread A to send notification back to us in Thread B
    WaitForThreadANotification();
    // Now processing Thread B start procedure
    this->StartThreadBCallback();
  }
  /* -- Constructor -------------------------------------------------------- */
  explicit ThreadSyncHelper(Thread &tO) : // Thread being used
    /* -- Initialisation of members ---------------------------------------- */
    tOwner(tO),                        // Set thread owner
    bUnlock(false),                    // Initially unlocked
    bAToB(false),                      // Not sending msg from Thread A to B
    bBToA(false)                       // Not sending msg from Thread B to A
    /* --------------------------------------------------------------------- */
    { }                                // Do nothing else
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(ThreadSyncHelper);   // Omit copy constructor for safety
};/* ----------------------------------------------------------------------- */
static size_t ThreadGetRunning(void) { return cThreads->stRunning; }
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
