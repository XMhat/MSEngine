/* == CURSOR.HPP =========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles creation of cursors using the Glfw subsystem.   ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace ICursor {                    // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace ICollector::P;
using namespace IError::P;             using namespace IGlFW::P;
using namespace IIdent::P;             using namespace ILog::P;
using namespace IStd::P;               using namespace ISysUtil::P;
using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* == Cursor collector class for collector data and custom variables ======= */
BEGIN_COLLECTOR(Cursors, Cursor, CLHelperUnsafe);
/* == Cursor class ========================================================= */
BEGIN_MEMBERCLASS(Cursors, Cursor, ICHelperUnsafe),
  /* -- Base classes ------------------------------------------------------- */
  public Lockable                      // Lua garbage collector instruction
{ /* -------------------------------------------------------------- */ private:
  GLFWcursor      *pCursor;            // Pointer to glfw cursor resource
  int              iInternal;          // Is a standard cursor?
  /* -- Activate the cursor graphic -------------------------------- */ public:
  void Activate(void) { cGlFW->WinSetCursorGraphic(pCursor); }
  /* -- Initialise --------------------------------------------------------- */
  void InitStandard(const int iId)
  { // De-init if initialised
    DeInit();
    // Create the cursor
    pCursor = glfwCreateStandardCursor(iId);
    if(!pCursor) XC("Failed to create standard cursor!", "Id", iId);
    // Set assigned cursor
    iInternal = iId;
    // Log created cursor
    cLog->LogDebugExSafe("Cursor created standard cursor using id $$.",
      hex, iId);
  }
  /* -- ReInitialise ------------------------------------------------------- */
  void ReInit(void)
  { // Re-Initialise cursor
    if(iInternal) InitStandard(iInternal);
  }
  /* -- DeInitialise ------------------------------------------------------- */
  bool DeInit(void)
  { // Return if not initialised
    if(!pCursor) return false;
    // Destroy cursor
    glfwDestroyCursor(pCursor);
    // Nullify address
    pCursor = nullptr;
    // Success
    return true;
  }
  /* -- Constructor -------------------------------------------------------- */
  Cursor(void) :
    /* -- Initialisers ----------------------------------------------------- */
    ICHelperCursor{ *cCursors, this }, // Register cursor in list
    IdentCSlave{ cParent.CtrNext() },  // Initialise identification number
    pCursor(nullptr),                  // Default character
    iInternal(0)                       // Default id
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Constructor -------------------------------------------------------- */
  explicit Cursor(const int iId) :
    /* -- Initialisers ----------------------------------------------------- */
    Cursor()                           // Default parameters
    /* -- Init specified cursor -------------------------------------------- */
    { InitStandard(iId); }
  /* -- Destructor --------------------------------------------------------- */
  ~Cursor(void) { if(pCursor) glfwDestroyCursor(pCursor); }
};/* ----------------------------------------------------------------------- */
END_COLLECTOR(Cursors);
/* == Reset cursor graphic ================================================= */
static void CursorReset(void) { cGlFW->WinSetCursorGraphic(nullptr); }
/* == De-initialise all ==================================================== */
static void CursorDeInit(void)
{ // Ignore if no fbos
  if(cCursors->empty()) return;
  // Enumerate each created fbo and deinitialise it (NOT destroy it)
  cLog->LogDebugExSafe("Cursors de-initialising $ objects...",
    cCursors->size());
  for(Cursor*const cCptr : *cCursors) cCptr->DeInit();
  cLog->LogInfoExSafe("Cursors de-initialised $ objects.", cCursors->size());
}
/* == Re-initialise all cursors ============================================ */
static void CursorReInit(void)
{ // Ignore if no fbos
  if(cCursors->empty()) return;
  // Enumerate each created fbo and reinitialise it
  cLog->LogDebugExSafe("Cursors re-initialising $ objects...",
    cCursors->size());
  for(Cursor*const cCptr : *cCursors) cCptr->ReInit();
  cLog->LogInfoExSafe("Cursors re-initialised $ objects.", cCursors->size());
}
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
