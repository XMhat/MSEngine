/* == EVTWIN.HPP =========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## This is the engine events class where the engine can queue messages ## **
** ## to be executed in the engine thread.                                ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IEvtWin {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IEvtCore::P;           using namespace IGlFWUtil::P;
using namespace ILog::P;               using namespace IStd::P;
using namespace ISysUtil::P;           using namespace IThread::P;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* -- Available engine commands -------------------------------------------- */
enum EvtWinCmd : size_t                // Render thread event commands
{ /* -- Main events -------------------------------------------------------- */
  EWC_NONE,                            // 00: No event occured
  /* -- Window events ------------------------------------------------------ */
  EWC_WIN_CENTRE,                      // 01: Put window in centre
  EWC_WIN_CURRESET,                    // 02: Reset cursor
  EWC_WIN_CURSET,                      // 03: Set cursor
  EWC_WIN_HIDE,                        // 04: Hide the window
  EWC_WIN_LIMITS,                      // 05: Window limits change
  EWC_WIN_MOVE,                        // 06: Move window
  EWC_WIN_RESET,                       // 07: Reset window position and size
  EWC_WIN_RESIZE,                      // 08: Resize window
  EWC_WIN_SETICON,                     // 09: Set window icon
  EWC_WIN_TOGGLE_FS,                   // 10: Toggle full-screen
  /* -- Clipboard events --------------------------------------------------- */
  EWC_CB_GET,                          // 11: Get clipboard (via Clip class)
  EWC_CB_SET,                          // 12: Set clipboard (via Clip class)
  EWC_CB_SETNR,                        // 13: " but no callback
  /* ----------------------------------------------------------------------- */
  EWC_NOLOG,                           // 14: Events after this aren't logged
  /* ----------------------------------------------------------------------- */
  EWC_MAX = EWC_NOLOG,                 // 14: Below are just codes
};/* ----------------------------------------------------------------------- */
static class EvtWin final :            // Event list for window thread
  /* -- Dependencies ------------------------------------------------------- */
  public EvtCore                       // Events common class
   <EvtWinCmd,                         // The enum list of events supported
    EWC_MAX,                           // Maximum events allowed
    EWC_NONE,                          // Event id for NONE
    EWC_NOLOG>                         // Event id for NOLOG
{ /* -- Add with copy parameter semantics (starter) ---------------- */ public:
  template<typename ...V>void AddUnblock(const EvtWinCmd ewcCmd,
    const V &...vaVars)
  { // Prepare parameters list and add a new event
    Add(ewcCmd, vaVars...);
    // Unblock the window thread
    GlFWForceEventHack();
  }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~EvtWin)
  /* -- Constructor -------------------------------------------------------- */
  EvtWin(void) :
    /* -- Initialisers ----------------------------------------------------- */
    EvtCore{"EventWin"}                // Set name of this object
    /* -- No core ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(EvtWin)              // Delete copy constructor and operator
  /* -- End ---------------------------------------------------------------- */
} *cEvtWin = nullptr;                  // Pointer to static class
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
