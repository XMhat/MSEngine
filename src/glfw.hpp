/* == GLFW.HPP ============================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Allows the engine to talk to GLFW easily.                           ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfGlFW {                     // Start of module namespace
/* ------------------------------------------------------------------------- */
using namespace IfGlFWWindow;          // Using glfw window namespace
/* ========================================================================= */
static class GlFW final :              // Root engine class
  /* -- Base classes ------------------------------------------------------- */
  public IHelper,                      // Initialisation helper
  public GlFWWindow                    // GLFW window class
{ /* -- Private variables and functions ------------------------------------ */
  unsigned int     uiErrorLevel;       // Ignore further glfw errors
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
    if(IHNotDeInitialise()) return;
    // Report de-initialisation attempt
    cLog->LogDebugSafe("GlFW subsystem de-initialising...");
    // Destroy window if created
    this->WinDeInit();
    // Terminate glfw
    glfwTerminate();
    // Report de-initialisation successful
    cLog->LogInfoSafe("GlFW subsystem de-initialised.");
  }
  /* -- Reset error level -------------------------------------------------- */
  void ResetErrorLevel(void) { uiErrorLevel = 0; }
  /* -- Initialiser -------------------------------------------------------- */
  void Init(void)
  { // Report initialisation attempt
    cLog->LogDebugSafe("GlFW subsystem initialising...");
    // Set error callback which just throws an exception
    glfwSetErrorCallback(ErrorHandler);
    // Reset error level
    ResetErrorLevel();
    // Initialise GlFW
    if(!glfwInit()) XC("GLFW initialisation failed!");
    // Class initialised
    IHInitialise();
    // Report initialisation successful
    cLog->LogInfoSafe("GlFW subsystem initialised.");
  }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~GlFW, DeInit())
  /* -- Constructor -------------------------------------------------------- */
  GlFW(void) :                         // Default constructor (No arguments)
    /* -- Initialisation of members ---------------------------------------- */
    IHelper{ __FUNCTION__ },           // Set class function name
    uiErrorLevel(0)                    // No errors occured
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(GlFW);               // Prevent copying for safety reasons
  /* ----------------------------------------------------------------------- */
} *cGlFW = nullptr;                    // Pointer to static class
/* == Process a glfw error ================================================= */
void GlFW::ErrorHandler(int iCode, const char*const cpDesc)
  { cGlFW->HandleError(iCode, cpDesc); }
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
