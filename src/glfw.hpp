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
using namespace IGlFWUtil::P;          using namespace IGlFWWindow::P;
using namespace ILog::P;               using namespace IStd::P;
using namespace ISysUtil::P;           using namespace IUtil::P;
using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ========================================================================= */
static class GlFW final :              // Root engine class
  /* -- Base classes ------------------------------------------------------- */
  public IHelper,                      // Initialisation helper
  public GlFWWindow                    // GLFW window class
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
  { // We HAVE to ignore clipboard errors as grabbing text with GetClipboard
    // always seems to fail and call this routine. Is this because of a bug?
    if(iCode == GLFW_FORMAT_UNAVAILABLE) return;
    // Send an exception for the first infringement
    if(!uiErrorLevel++) XC(cpDesc, "Code", iCode);
    // Put further errors in the log to prevent message box spam
    cLog->LogWarningExSafe("GlFW got API error $/$: $!",
      uiErrorLevel, iCode, cpDesc);
  }
  /* -- DeInitialiser ---------------------------------------------- */ public:
  void DeInit(void)
  { // Ignore if class not initialised
    if(IHDeInitialise()) return;
    // Report de-initialisation attempt
    cLog->LogDebugSafe("GlFW subsystem de-initialising...");
    // Destroy window if created
    this->WinDeInit();
    // Terminate glfw
    glfwTerminate();
    // Delete error handler
    glfwSetErrorCallback(nullptr);
    // Report de-initialisation successful
    cLog->LogInfoSafe("GlFW subsystem de-initialised.");
  }
  /* -- Reset error level -------------------------------------------------- */
  void ResetErrorLevel(void) { uiErrorLevel = 0; }
  /* -- Initialiser -------------------------------------------------------- */
  void Init(void)
  { // Report initialisation attempt
    cLog->LogDebugSafe("GlFW subsystem initialising...");
    // GLFW 3.4.0 won't compile for Windows XP mode yet even though they said
    // they still wanted to support it for a bit longer. Also GLFW 3.4.0 won't
    // run properly on my 23.04 so we're still using the built-in 3.6.6.
#if defined(MACOS)
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
    // Report initialisation successful
    cLog->LogInfoSafe("GlFW subsystem initialised.");
  }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~GlFW, DeInit())          // Try to de-initialise glfw
  /* -- Constructor -------------------------------------------------------- */
  GlFW(void) :                         // Default constructor (No arguments)
    /* -- Initialisers ----------------------------------------------------- */
    IHelper{ __FUNCTION__ },           // Set class function name
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
