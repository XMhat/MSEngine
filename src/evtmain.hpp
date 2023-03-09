/* == EVTMAIN.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This is the engine events class where the engine can queue messages ## */
/* ## to be executed in the engine thread.                                ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfEvtMain {                  // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfLog;                 // Using log interface
using namespace IfThread;              // Using thread interface
/* -- Available engine commands -------------------------------------------- */
enum EvtMainCmd                        // Render thread event commands
{ /* -- Main events -------------------------------------------------------- */
  EMC_NONE,                            // 00: No event occured
  /* -- Quit events -------------------------------------------------------- */
  EMC_QUIT,                            // 01: Main loop should quit
  EMC_QUIT_THREAD,                     // 02: Thread loop should quit
  EMC_QUIT_RESTART,                    // 03: Cleanly quit and restart app
  EMC_QUIT_RESTART_NP,                 // 04: Same as above but without args
  /* -- Lua events --------------------------------------------------------- */
  EMC_LUA_PAUSE,                       // 05: Pause LUA execution
  EMC_LUA_RESUME,                      // 06: Resume LUA execution
  EMC_LUA_REDRAW,                      // 07: Ask LUA guest to redraw fbo's
  EMC_LUA_REINIT,                      // 08: Reinit LUA execution
  EMC_LUA_END,                         // 09: End LUA execution
  EMC_LUA_ASK_EXIT,                    // 10: To LUA asking to clean up & exit
  EMC_LUA_CONFIRM_EXIT,                // 11: From LUA to confirm the exit
  /* -- Window events ------------------------------------------------------ */
  EMC_WIN_MOVED,                       // 12: Window was moved
  EMC_WIN_FOCUS,                       // 13: Window focus was changed
  EMC_WIN_SCALE,                       // 14: Window content scale changed
  EMC_WIN_RESIZED,                     // 15: Window was resized
  EMC_WIN_REFRESH,                     // 16: Window content needs refresh
  EMC_WIN_ICONIFY,                     // 17: Window was minimised or restored
  EMC_WIN_CLOSE,                       // 18: Window wants to be closed
  EMC_WIN_LIMITS,                      // 19: Window limits change
  /* -- OpenGL events ------------------------------------------------------ */
  EMC_VID_SCREENSHOT,                  // 20: Take video screenshot
  EMC_VID_FB_REINIT,                   // 21: Reset frame buffer
  EMC_VID_MATRIX_REINIT,               // 22: Reset matrix
  /* -- Audio events ------------------------------------------------------- */
  EMC_AUD_REINIT,                      // 23: Re-initialise audio
  /* -- Input events ------------------------------------------------------- */
  EMC_INP_PASTE,                       // 24: Paste into input from clipboard
  /* ----------------------------------------------------------------------- */
  EMC_NOLOG,                           // 25: Events after this aren't logged
  /* -- Console events ----------------------------------------------------- */
  EMC_CON_UPDATE = EMC_NOLOG,          // 26: Force a syscon display update
  EMC_CON_RESIZE,                      // 27: Syscon resized event
  EMC_CON_KEYPRESS,                    // 28: Console key pressed
  /* -- Input events ------------------------------------------------------- */
  EMC_INP_KEYPRESS,                    // 29: Unfiltered key pressed
  EMC_INP_CHAR,                        // 30: Filtered key pressed
  EMC_INP_DRAG_DROP,                   // 31: Files dragged and dropped
  EMC_INP_MOUSE_FOCUS,                 // 32: Mouse moved outside/inside window
  EMC_INP_MOUSE_MOVE,                  // 33: Mouse cursor moved
  EMC_INP_MOUSE_CLICK,                 // 34: Mouse button clicked
  EMC_INP_MOUSE_SCROLL,                // 35: Mouse wheel scrolled
  EMC_INP_JOY_STATE,                   // 36: Joystick status changed
  EMC_INP_SET_CUR_POS,                 // 37: Set cursor position
  /* -- Async events ------------------------------------------------------- */
  EMC_MP_ARCHIVE,                      // 38: Archive async event occured
  EMC_MP_ASSET,                        // 39: Asset async event occured
  EMC_MP_FONT,                         // 40: Char async event occured
  EMC_MP_IMAGE,                        // 41: Image async event occured
  EMC_MP_JSON,                         // 42: Json async event occured
  EMC_MP_PCM,                          // 43: Pcm async event occured
  EMC_MP_PROCESS,                      // 44: Process async event occured
  EMC_MP_SOCKET,                       // 45: Socket async event occured
  EMC_MP_STREAM,                       // 46: Stream async event occured
  EMC_MP_VIDEO,                        // 47: Video async event occured
  /* -- Stream events ------------------------------------------------------ */
  EMC_STR_EVENT,                       // 48: Stream event occured
  EMC_VID_EVENT,                       // 49: Video event occured
  /* ----------------------------------------------------------------------- */
  EMC_MAX,                             // 50: Below are just codes
  /* ----------------------------------------------------------------------- */
  EMC_LUA_ERROR,                       // 51: Error in LUA exec (not an event)
  /* ----------------------------------------------------------------------- */
};
/* ------------------------------------------------------------------------- */
static class EvtMain :                 // Event list for render thread
  /* -- Dependencies ------------------------------------------------------- */
  private IfCollector::IHelper,        // Initialisation helper
  public IfEvtCore::EvtCore            // Events common class
   <EvtMainCmd,                        // The enum list of events supported
    EMC_MAX,                           // Maximum events allowed
    EMC_NONE,                          // Event id for NONE
    EMC_NOLOG>,                        // Event id for NOLOG
  public Thread                        // Engine thread
{ /* -- Private --------------------------------------------------- */ private:
  EvtMainCmd       emcPending;         // Event fired before exit requested
  EvtMainCmd       emcExit;            // Thread exit code
  unsigned int     uiConfirm;          // Exit confirmation progress
  /* -- Events list -------------------------------------------------------- */
  const RegVec     rvEvents;           // Events list to register
  /* -- Check events (called from Main) ------------------------------------ */
  bool ProcessResult(const EvtMainCmd emcResult)
  { // Which event?
    switch(emcResult)
    { // Thread should quit. Tell main loop to loop again
      case EMC_QUIT_THREAD: emcExit = EMC_QUIT_THREAD;
        return false;
      // Thread should quit and main thread should restart completely
      case EMC_QUIT_RESTART: ConfirmExit(EMC_QUIT_RESTART); break;
      // Same as above but without command line parameters
      case EMC_QUIT_RESTART_NP: ConfirmExit(EMC_QUIT_RESTART_NP); break;
      // Lua executing is ending
      case EMC_LUA_END: ConfirmExit(EMC_LUA_END); break;
      // Lua executing is re-initialising
      case EMC_LUA_REINIT: ConfirmExit(EMC_LUA_REINIT); break;
      // Lua confirmed exit is allowed now so return the code we recorded
      case EMC_LUA_CONFIRM_EXIT: if(!uiConfirm) break;
        // Restore original exit code, reset exit and confirmation codes
        emcExit = emcPending;
        emcPending = EMC_NONE;
        uiConfirm = 0;
        // Break main loop
        return false;
      // Thread and main thread should quit so tell thread to break.
      case EMC_QUIT: ConfirmExit(EMC_QUIT); break;
      // Other event, thread shouldn't break
      default: break;
    } // Thread shouldn't break
    return true;
  }
  /* -- Get exit reason code --------------------------------------- */ public:
  EvtMainCmd GetExitReason(void) const { return emcExit; }
  /* -- Set exit reason code ----------------------------------------------- */
  void SetExitReason(const EvtMainCmd emcReason) { emcExit = emcReason; }
  /* -- Check exit reason -------------------------------------------------- */
  bool IsExitReason(const EvtMainCmd emcReason) const
    { return emcExit == emcReason; }
  bool IsNotExitReason(const EvtMainCmd emcReason) const
    { return !IsExitReason(emcReason); }
  /* -- Incase of error we need to update the exit code -------------------- */
  bool ExitRequested(void) const { return !!uiConfirm; }
  /* -- Incase of error we need to update the exit code -------------------- */
  void UpdateConfirmExit(void)
  { // Ignore if not in a confirmation request
    if(!uiConfirm) return;
    // Reset confirmation
    uiConfirm = 0;
    // Set exit code and clear stored exit code
    emcExit = emcPending;
    emcPending = EMC_NONE;
  }
  /* -- Informs LUA that the user wants to quit ---------------------------- */
  void ConfirmExit(const EvtMainCmd emcWhat)
  { // A confirmation request is currently in progress?
    if(uiConfirm)
    { // Set the new exit code overriding the new one.
      emcPending = emcWhat;
      // Still waiting for confirmation
      return;
    } // Exit confirming
    uiConfirm = 1;
    // Ask lua to quit when it is ready. By default it will send confirm.
    Execute(EMC_LUA_ASK_EXIT, emcWhat);
    // Set the new exit code
    emcPending = emcWhat;
  }
  /* -- Handle events from parallel loop ----------------------------------- */
  bool HandleSafe(void)
  { // Main thread requested break? Why bother managing events?
    if(ThreadShouldExit()) return false;
    // Handle event and return true if nothing special happened
    if(ProcessResult(ManageSafe())) return true;
    // Thread should terminate
    ThreadSetExit();
    // Thread should break
    return false;
  }
  /* -- Handle events from serialised loop --------------------------------- */
  bool HandleUnsafe(void)
  { // Main thread requested break? Why bother managing events?
    if(ThreadShouldExit()) return false;
    // Handle event and return true if nothing special happened
    if(ProcessResult(ManageUnsafe())) return true;
    // Thread should terminate
    ThreadSetExit();
    // Thread should break
    return false;
  }
  /* -- Initialise base events (called from Main) -------------------------- */
  void Init(void)
  { // Class initialised
    IHInitialise();
    // Registrer base events
    RegisterEx(rvEvents);
  }
  /* -- DeInitialise base events (called from Main) ------------------------ */
  void DeInit(void)
  { // Ignore if class not initialised
    if(IHNotDeInitialise()) return;
    // Registrer base events
    UnregisterEx(rvEvents);
  }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~EvtMain);
  /* -- Constructor -------------------------------------------------------- */
  EvtMain(void) :
    /* -- Initialisation of members ---------------------------------------- */
    IHelper{ __FUNCTION__ },           // Initialise helper
    EvtCore{"EventMain"},              // Set name of this object
    Thread{ "engine" },                // Sandbox thread
    emcPending(EMC_NONE),              // Not exiting yet
    emcExit(EMC_NONE),                 // Not exited yet
    uiConfirm(0),                      // Exit not confirmed yet
    /* --------------------------------------------------------------------- */
    rvEvents{                          // Initialise custom handled events
      { EMC_QUIT,             nullptr },  { EMC_QUIT_THREAD,      nullptr },
      { EMC_QUIT_RESTART,     nullptr },  { EMC_QUIT_RESTART_NP,  nullptr },
      { EMC_LUA_END,          nullptr },  { EMC_LUA_REINIT,       nullptr },
      { EMC_LUA_CONFIRM_EXIT, nullptr }
    }
    /* --------------------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(EvtMain);            // Delete copy constructor and operator
  /* -- End ---------------------------------------------------------------- */
} *cEvtMain = nullptr;                 // Pointer to static class
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
