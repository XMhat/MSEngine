/* == GLFW.HPP ============================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Allows the engine to talk to GLFW easily.                           ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfGlFW {                     // Keep declarations neatly categorised
/* == GlFW manager class =================================================== */
static class GlFW final :
  /* -- Base classes ------------------------------------------------------- */
  public IHelper,                      // Initialisation helper
  public GlFWWindow                    // GLFW window class
{ /* -- Private variables -------------------------------------------------- */
  unsigned int     uiErrorLevel;       // Ignore further glfw errors
  /* -- Error handler prototype -------------------------------------------- */
  static void ErrorHandler(int, const char*const);
  /* --------------------------------------------------------------- */ public:
  static void ForceEventHack(void) { glfwPostEmptyEvent(); }
  /* -- Joystick axes ------------------------------------------------------ */
  static const float *GetJoystickAxes(int iJ, int &iJAC)
    { return glfwGetJoystickAxes(iJ, &iJAC); }
  /* -- Joystick buttons --------------------------------------------------- */
  static const unsigned char *GetJoystickButtons(int iJ, int &iJAB)
    { return glfwGetJoystickButtons(iJ, &iJAB); }
  /* -- Joystick other ----------------------------------------------------- */
  static const char *GetJoystickName(const int iJ)
    { return glfwGetJoystickName(iJ); }
  /* -- Set swap interval -------------------------------------------------- */
  static void SetVSync(const int iI) { glfwSwapInterval(iI); }
  /* -- Wait for window event ---------------------------------------------- */
  static void WaitEvents(void) { glfwWaitEvents(); }
  static void PollEvents(void) { glfwPollEvents(); }
  /* -- Release current context -------------------------------------------- */
  static void ReleaseContext(void)
    { glfwMakeContextCurrent(nullptr); }
  /* -- Set gamma ---------------------------------------------------------- */
  static void SetGamma(GLFWmonitor*const mDevice, const GLfloat fValue)
    { glfwSetGamma(mDevice, fValue); }
  /* == Set window hint ==================================================== */
  static void SetDefaultWindowHints(void) { glfwDefaultWindowHints(); }
  /* -- Set window hint string --------------------------------------------- */
  void SetWindowHintString(const int iHint, const char*const cpValue) const
  { // Set window hint directly
    glfwWindowHintString(iHint, cpValue);
    // Log the change
    LW(LH_DEBUG, "GlFW set hint $<0x$$> to '$'.",
      GlFWGetHintAttribStr(iHint), hex, iHint, cpValue);
  }
  /* -- Set frame name in MacOS -------------------------------------------- */
  void SetWindowCocoaFrameName([[maybe_unused]] const char*const cpName) const
  { // Only applies to Apple targets
#if defined(MACOS)
    SetWindowHintString(GLFW_COCOA_FRAME_NAME, cpName);
#endif
  }
  /* -- Set class name in X11 ---------------------------------------------- */
  void SetWindowX11ClassName([[maybe_unused]] const char*const cpName) const
  { // Only applies to Linux targets
#if defined(LINUX)
    SetWindowHintString(GLFW_X11_CLASS_NAME, cpName);
#endif
  }
  /* -- Set instance name in X11 ------------------------------------------- */
  void SetWindowX11InstanceName([[maybe_unused]] const char*const cpName) const
  { // Only applies to Linux targets
#if defined(LINUX)
    SetWindowHintString(GLFW_X11_INSTANCE_NAME, cpName);
#endif
  }
  /* -- Set window frame names --------------------------------------------- */
  void SetWindowFrameName([[maybe_unused]] const char*const cpName)
  { // Set custom frame names
#if defined(MACOS)
    SetWindowCocoaFrameName(cpName);
#elif defined(LINUX)
    SetWindowX11ClassName(cpName);
    SetWindowX11InstanceName(cpName);
#endif
  }
  /* -- Set window hint ---------------------------------------------------- */
  void SetWindowHint(const int iVar, const int iVal) const
  { // Set window hint directly
    glfwWindowHint(iVar, iVal);
    // Log the change
    LW(LH_DEBUG, "GlFW set hint $<0x$$> to $$<0x$$>.",
      GlFWGetHintAttribStr(iVar), hex, iVar, dec, iVal, hex, iVal);
  }
  /* -- Create functions to access all attributes ------------------------- */\
#define SET(nc,nu) [[maybe_unused]] void Set ## nc(const int iNV) const \
    { SetWindowHint(GLFW_ ## nu, iNV); }
  /* ---------------------------------------------------------------------- */\
  SET(RedBits, RED_BITS);              // Set depth of red component
  SET(GreenBits, GREEN_BITS);          // Set depth of green component
  SET(BlueBits, BLUE_BITS);            // Set depth of blue component
  SET(AlphaBits, ALPHA_BITS);          // Set depth of alpha component
  SET(DepthBits, DEPTH_BITS);          // Set depth of Z component
  SET(StencilBits, STENCIL_BITS);      // Set depth of stencil component
  SET(Multisamples, SAMPLES);          // Set anti-aliasing factor
  SET(AuxBuffers, AUX_BUFFERS);        // Set auxiliary buffer count
  SET(RefreshRate, REFRESH_RATE);      // Set desktop refresh rate
  SET(ClientAPI, CLIENT_API);          // Set client api to use
  SET(CtxMajor, CONTEXT_VERSION_MAJOR); // Set gl context major version
  SET(CtxMinor, CONTEXT_VERSION_MINOR); // Set gl context minor version
  SET(CoreProfile, OPENGL_PROFILE);    // Set gl profile to use
  SET(Robustness, CONTEXT_ROBUSTNESS); // Set context robustness
  /* ----------------------------------------------------------------------- */
#undef SET                             // Done with this macro
  /* -- Set window bits all in one ----------------------------------------- */
  void SetColourDepth(const int iR, const int iG, const int iB, const int iA)
    const { SetRedBits(iR); SetGreenBits(iG); SetBlueBits(iB);
            SetAlphaBits(iA); }
  /* -- Set opengl context version in one function ------------------------- */
  void SetContextVersion(const int iMajor, const int iMinor) const
    { SetCtxMajor(iMajor); SetCtxMinor(iMinor); }
  /* -- Set window hint core functions ------------------------------------- */
  void SetWindowHintBoolean(const int iVar, const bool bVal) const
    { SetWindowHint(iVar, GlFWBooleanToGBoolean(bVal)); }
  void SetWindowHintEnabled(const int iVar) const
    { SetWindowHintBoolean(iVar, true); }
  void SetWindowHintDisabled(const int iVar) const
    { SetWindowHintBoolean(iVar, false); }
  /* -- Create functions to access all attributes ------------------------- */\
#define SET(nc,nu) \
  [[maybe_unused]] void Set ## nc(const bool bState) const \
    { SetWindowHintBoolean(GLFW_ ## nu, bState); } \
  [[maybe_unused]] void Set ## nc ## Enabled(void) const \
    { SetWindowHintEnabled(GLFW_ ## nu); } \
  [[maybe_unused]] void Set ## nc ## Disabled(void) const \
    { SetWindowHintDisabled(GLFW_ ## nu); } \
  /* ----------------------------------------------------------------------- */
  SET(AutoIconify, AUTO_ICONIFY);      // Set window auto-minimise state
  SET(CentreCursor, CENTER_CURSOR);    // Set window cursor centre state
  SET(Debug, OPENGL_DEBUG_CONTEXT);    // Set opengl debug mode
  SET(Decorated, DECORATED);           // Set window border state
  SET(DoubleBuffer, DOUBLEBUFFER);     // Set double buffering
  SET(Floating, FLOATING);             // Set window floating state
  SET(Focus, FOCUSED);                 // Set window focused state
  SET(FocusOnShow, FOCUS_ON_SHOW);     // Set focus on show window
  SET(ForwardCompat, OPENGL_FORWARD_COMPAT); // Set opengl fwd compatibility
  SET(Iconify, ICONIFIED);             // Set window minimised state
  SET(Maximised, MAXIMIZED);           // Set window maximised state
  SET(MouseHovered, HOVERED);          // Set mouse hovered over state
  SET(NoErrors, CONTEXT_NO_ERROR);     // Set context no errors
  SET(Resizable, RESIZABLE);           // Set window resizable state
  SET(SRGBCapable, SRGB_CAPABLE);      // Set SRGB colour space capable
  SET(Stereo, CENTER_CURSOR);          // Set window cursor centre state
  SET(Transparency, TRANSPARENT_FRAMEBUFFER); // Set transparent framebuffer
  SET(Visibility, VISIBLE);            // Set window visibility state
  /* ----------------------------------------------------------------------- */
#if defined(MACOS)                     // Using Apple compiler?
  /* ----------------------------------------------------------------------- */
  SET(GPUSwitching, COCOA_GRAPHICS_SWITCHING); // Set graphics switching?
  SET(MenuBar,      COCOA_MENUBAR);    // Set MacOS menu bar?
  SET(RetinaMode,   COCOA_RETINA_FRAMEBUFFER); // Set retina framebuffer?
  /* ----------------------------------------------------------------------- */
#endif                                 // End of Apple check
  /* ----------------------------------------------------------------------- */
#undef SET                             // Done with this macro
  /* -- Get function address------------------------------------------------ */
  static bool ProcExists(const char*const cpFunction)
    { return !!glfwGetProcAddress(cpFunction); }
  /* -- Return monitor count ----------------------------------------------- */
  static GLFWmonitor** GetMonitors(int &iCount)
    { return glfwGetMonitors(&iCount); }
  /* -- Return monitor count ----------------------------------------------- */
  static int GetMonitorCount(void)
    { int iMonitors; GetMonitors(iMonitors); return iMonitors; }
  /* -- Get internal name of key ------------------------------------------- */
  static const char *GetKeyName(const int iK, const int iSC)
    { return glfwGetKeyName(iK, iSC); }
  /* - Return if raw mouse is supported? ----------------------------------- */
  static bool IsRawMouseMotionSupported(void)
    { return GlFWGBooleanToBoolean(glfwRawMouseMotionSupported()); }
  /* ----------------------------------------------------------------------- */
  GLFWglproc GetProcAddress(const char*const cpFunction) const
  { // Get function and return it if we have it
    if(GLFWglproc fCB = glfwGetProcAddress(cpFunction)) return fCB;
    // No function so return error
    XC("The specified OpenGL ICD function could not be found from your video "
        "card manufacturers display driver. Try upgrading your graphics "
        "drivers to the latest version from your manufacturers or OEM "
        "website and make sure your operating system is up to date. If all "
        "else fails, you will need to upgrade your graphics hardware!",
        "Function", cpFunction);
  }
  /* -- DeInitialiser ------------------------------------------------------ */
  void DeInit(void)
  { // Ignore if class not initialised
    if(IHNotDeInitialise()) return;
    // Report de-initialisation attempt
    LW(LH_DEBUG, "GlFW subsystem de-initialising...");
    // Destroy window if created
    this->WinDeInit();
    // Terminate glfw
    glfwTerminate();
    // Report de-initialisation successful
    LW(LH_INFO, "GlFW subsystem de-initialised.");
  }
  /* -- Reset error level -------------------------------------------------- */
  void ResetErrorLevel(void) { uiErrorLevel = 0; }
  /* -- Error handler converted to thiscall -------------------------------- */
  void HandleError(const int iCode, const char*const cpDesc)
  { // We HAVE to ignore clipboard errors as grabbing text with GetClipboard
    // always seems to fail and call this routine. Is this because of a bug?
    if(iCode == GLFW_FORMAT_UNAVAILABLE) return;
    // Send an exception for the first infringement
    if(!uiErrorLevel++) XC(cpDesc, "Code", iCode);
    // Put further errors in the log to prevent message box spam
    LW(LH_WARNING, "GlFW got API error $/$: $!", uiErrorLevel, iCode, cpDesc);
  }
  /* -- Initialiser -------------------------------------------------------- */
  void Init(void)
  { // Report initialisation attempt
    LW(LH_DEBUG, "GlFW subsystem initialising...");
    // Set error callback which just throws an exception
    glfwSetErrorCallback(ErrorHandler);
    // Reset error level
    ResetErrorLevel();
    // Initialise GlFW
    if(!glfwInit()) XC("GLFW initialisation failed!");
    // Class initialised
    IHInitialise();
    // Report initialisation successful
    LW(LH_INFO, "GlFW subsystem initialised.");
  }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~GlFW, DeInit())
  /* -- Constructor -------------------------------------------------------- */
  GlFW(void) :
    /* -- Initialisation of members ---------------------------------------- */
    IHelper{ __FUNCTION__ },           // Set class function name
    uiErrorLevel(0)                    // No errors occured
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(GlFW);               // Do not need defaults
  /* ----------------------------------------------------------------------- */
} *cGlFW = nullptr;                    // Pointer to static class
/* == Process a glfw error ================================================= */
void GlFW::ErrorHandler(int iCode, const char*const cpDesc)
  { cGlFW->HandleError(iCode, cpDesc); }
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
