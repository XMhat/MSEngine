/* == EVTWIN.HPP =========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This is the engine events class where the engine can queue messages ## */
/* ## to be executed in the engine thread.                                ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfEvtWin {                   // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfGlFW;                // Using glfw namespace
using namespace IfLog;                 // Using log namespace
using namespace IfThread;              // Using thread namespace
/* -- Available engine commands -------------------------------------------- */
enum EvtWinCmd                         // Render thread event commands
{ /* -- Main events -------------------------------------------------------- */
  EWC_NONE,                            // 00: No event occured
  /* -- Window events ------------------------------------------------------ */
  EWC_WIN_RESET,                       // 01: Reset window position and size
  EWC_WIN_TOGGLE_FS,                   // 02: Toggle full-screen
  EWC_WIN_CENTRE,                      // 03: Put window in centre
  EWC_WIN_MOVE,                        // 04: Move window
  EWC_WIN_RESIZE,                      // 05: Resize window
  EWC_WIN_SETICON,                     // 06: Set window icon
  /* -- Clipboard events --------------------------------------------------- */
  EWC_CB_GET,                          // 07: Get clipboard (via Clip class)
  EWC_CB_SET,                          // 08: Set clipboard (via Clip class)
  EWC_CB_SETNR,                        // 09: " but no callback
  /* ----------------------------------------------------------------------- */
  EWC_NOLOG,                           // 10: Events after this aren't logged
  /* ----------------------------------------------------------------------- */
  EWC_MAX = EWC_NOLOG,                 // 11: Below are just codes
};/* ----------------------------------------------------------------------- */
static class EvtWin final :            // Event list for window thread
  /* -- Dependencies ------------------------------------------------------- */
  public IfEvtCore::EvtCore            // Events common class
   <EvtWinCmd,                         // The enum list of events supported
    EWC_MAX,                           // Maximum events allowed
    EWC_NONE,                          // Event id for NONE
    EWC_NOLOG>                         // Event id for NOLOG
{ /* -- Add with copy parameter semantics (starter) ---------------- */ public:
  template<typename... V>void AddUnblock(const EvtWinCmd ewcCmd, V... vVars)
  { // Prepare parameters list and add a new event
    Add(ewcCmd, vVars...);
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
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
