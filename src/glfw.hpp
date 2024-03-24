/* == GLFW.HPP ============================================================= **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Allows the engine to talk to GLFW easily.                           ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IGlFW {                      // Start of module namespace
/* ------------------------------------------------------------------------- */
using namespace ICollector::P;         using namespace IError::P;
using namespace IGlFWCursor::P;        using namespace IGlFWUtil::P;
using namespace IGlFWWindow::P;        using namespace ILog::P;
using namespace IStd::P;               using namespace ISysUtil::P;
using namespace IUtil::P;              using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
typedef array<GlFWCursor, CUR_MAX> CursorStandard;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ========================================================================= */
static class GlFW final :              // Root engine class
  /* -- Base classes ------------------------------------------------------- */
  public IHelper,                      // Initialisation helper
  public GlFWWindow,                   // GLFW window class
  private CursorStandard               // Standard cursors list
{ /* -- Private variables and functions ------------------------------------ */
  unsigned int     uiErrorLevel;       // Ignore further glfw errors
  /* -- Custom allocator --------------------------------------------------- */
  static void *GlFWAlloc(size_t stSize, void*const)
    { return UtilMemAlloc<void>(stSize); }
  /* -- Custom reallocator ------------------------------------------------- */
  static void *GlFWReAlloc(void*const vpPtr, size_t stSize, void*const)
    { return UtilMemReAlloc(vpPtr, stSize); }
  /* -- Custom free -------------------------------------------------------- */
  static void GlFWFree(void*const vpPtr, void*const)
    { UtilMemFree(vpPtr); }
  /* -- Error handler prototype (full body at bottom) ---------------------- */
  static void ErrorHandler(int, const char*const);
  /* -- Error handler converted to thiscall -------------------------------- */
  void HandleError(const int iCode, const char*const cpDesc)
  { // What's the error code?
    switch(iCode)
    { // Errors that are safe to ignore
      case GLFW_FORMAT_UNAVAILABLE: [[fallthrough]];
      case GLFW_FEATURE_UNIMPLEMENTED: [[fallthrough]];
      case GLFW_FEATURE_UNAVAILABLE:
        return cLog->LogDebugExSafe("GlFW ignored API error $: $!",
          iCode, cpDesc);
      // Anything else?
      default:
        // Send an exception for the first infringement
        if(!uiErrorLevel++) XC(cpDesc, "Code", iCode);
        // Put further errors in the log to prevent message box spam
        cLog->LogWarningExSafe("GlFW got API error $/$: $!",
          uiErrorLevel, iCode, cpDesc);
        // Done
        return;
    }
  }
  /* -- DeInitialiser ---------------------------------------------- */ public:
  void DeInit(void)
  { // Ignore if class not initialised
    if(IHDeInitialise()) return;
    // Report de-initialisation attempt
    cLog->LogDebugSafe("GlFW subsystem de-initialising...");
    // Destroy window if created
    this->WinDeInit();
    // De-Initialise standard cursors
    DeInitCursors();
    // Terminate glfw
    glfwTerminate();
    // Delete error handler
    glfwSetErrorCallback(nullptr);
    // Report de-initialisation successful
    cLog->LogInfoSafe("GlFW subsystem de-initialised.");
  }
  /* -- Set the specified cursor ------------------------------------------- */
  void SetCursor(const GlFWCursorType gctCursorId)
    { WinSetCursorGraphic(at(gctCursorId).CursorGetContext()); }
  /* -- DeInitialise all standard cursors ---------------------------------- */
  void DeInitCursors(void)
  { // Enumerate each created fbo and deinitialise it (NOT destroy it)
    cLog->LogDebugExSafe("GlFW de-initialising $ standard cursors...", size());
    for(GlFWCursor &gcObj : *this) gcObj.CursorDeInit();
    cLog->LogInfoExSafe("GlFW de-initialised $ standard cursors.", size());
  }
  /* -- Initialise all standard cursors ------------------------------------ */
  void InitCursors(void)
  { // Enumerate each created fbo and reinitialise it
    cLog->LogDebugExSafe("GlFW initialising $ standard cursors...", size());
    for(GlFWCursor &gcObj : *this) gcObj.CursorInit();
    cLog->LogInfoExSafe("GlFW initialised $ standard cursors.", size());
  }
  /* -- Reset error level -------------------------------------------------- */
  void ResetErrorLevel(void) { uiErrorLevel = 0; }
  /* -- Initialiser -------------------------------------------------------- */
  void Init(void)
  { // Report initialisation attempt
    cLog->LogDebugSafe("GlFW subsystem initialising...");
    // Only available on 3.4 and above
#if GLFW_VERSION_MAJOR >= 3 && GLFW_VERSION_MINOR >= 4
    // Setup custom allocator
    GLFWallocator gaInfo{ GlFWAlloc, GlFWReAlloc, GlFWFree, this };
    glfwInitAllocator(&gaInfo);
#endif
    // Set error callback which just throws an exception and reset error level
    glfwSetErrorCallback(ErrorHandler);
    ResetErrorLevel();
    // Initialise GlFW and throw exception if failed
    if(!glfwInit()) XC("GLFW initialisation failed!");
    // Class initialised
    IHInitialise();
    // Initialise standard cursors
    InitCursors();
    // Report initialisation successful
    cLog->LogInfoSafe("GlFW subsystem initialised.");
  }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~GlFW, DeInit())          // Try to de-initialise glfw
  /* -- Constructor -------------------------------------------------------- */
  GlFW(void) :                         // Default constructor (No arguments)
    /* -- Initialisers ----------------------------------------------------- */
    IHelper{ __FUNCTION__ },           // Set class function name
    /* --------------------------------------------------------------------- */
#define CURSOR(x) { GLFW_ ## x ## _CURSOR }
    /* --------------------------------------------------------------------- */
    CursorStandard{{                   // Define standard cursors
      CURSOR(ARROW),                   CURSOR(CROSSHAIR),
      CURSOR(HAND),                    CURSOR(HRESIZE),
      CURSOR(IBEAM),                   CURSOR(NOT_ALLOWED),
      CURSOR(RESIZE_ALL),              CURSOR(RESIZE_EW),
      CURSOR(RESIZE_NESW),             CURSOR(RESIZE_NS),
      CURSOR(RESIZE_NWSE),             CURSOR(VRESIZE)
    }},
    /* --------------------------------------------------------------------- */
#undef CURSOR                          // Done with this macro
    /* --------------------------------------------------------------------- */
    uiErrorLevel(0)                    // No errors occured
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(GlFW)                // Prevent copying for safety reasons
  /* ----------------------------------------------------------------------- */
} *cGlFW = nullptr;                    // Pointer to static class
/* == Process a glfw error ================================================= */
void GlFW::ErrorHandler(int iCode, const char*const cpDesc)
  { cGlFW->HandleError(iCode, cpDesc); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
