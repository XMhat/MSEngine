/* == GLFWWIN.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Manage Window objects using GLFW.                                   ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfGlFW {                     // Keep declarations neatly categorised
/* ------------------------------------------------------------------------- */
using namespace IfEvtMain;             // Using event interface
/* -- Convert window hint id to string ------------------------------------- */
static const string &GlFWGetHintAttribStr(const int iTarget)
{ // Glfw targets as strings
  static const IdMap<int> imTargets{{
    IDMAPSTR(GLFW_ACCUM_ALPHA_BITS), IDMAPSTR(GLFW_ACCUM_BLUE_BITS),
    IDMAPSTR(GLFW_ACCUM_GREEN_BITS), IDMAPSTR(GLFW_ACCUM_RED_BITS),
    IDMAPSTR(GLFW_ALPHA_BITS), IDMAPSTR(GLFW_AUTO_ICONIFY),
    IDMAPSTR(GLFW_AUX_BUFFERS), IDMAPSTR(GLFW_BLUE_BITS),
    IDMAPSTR(GLFW_CENTER_CURSOR), IDMAPSTR(GLFW_CLIENT_API),
    IDMAPSTR(GLFW_COCOA_FRAME_NAME), IDMAPSTR(GLFW_COCOA_GRAPHICS_SWITCHING),
    IDMAPSTR(GLFW_COCOA_MENUBAR), IDMAPSTR(GLFW_COCOA_RETINA_FRAMEBUFFER),
    IDMAPSTR(GLFW_CONTEXT_CREATION_API), IDMAPSTR(GLFW_CONTEXT_NO_ERROR),
    IDMAPSTR(GLFW_CONTEXT_RELEASE_BEHAVIOR), IDMAPSTR(GLFW_CONTEXT_REVISION),
    IDMAPSTR(GLFW_CONTEXT_ROBUSTNESS), IDMAPSTR(GLFW_CONTEXT_VERSION_MAJOR),
    IDMAPSTR(GLFW_CONTEXT_VERSION_MINOR), IDMAPSTR(GLFW_DECORATED),
    IDMAPSTR(GLFW_DEPTH_BITS), IDMAPSTR(GLFW_DOUBLEBUFFER),
    IDMAPSTR(GLFW_FLOATING), IDMAPSTR(GLFW_FOCUS_ON_SHOW),
    IDMAPSTR(GLFW_FOCUSED), IDMAPSTR(GLFW_GREEN_BITS),
    IDMAPSTR(GLFW_HOVERED), IDMAPSTR(GLFW_ICONIFIED),
    IDMAPSTR(GLFW_MAXIMIZED), IDMAPSTR(GLFW_OPENGL_DEBUG_CONTEXT),
    IDMAPSTR(GLFW_OPENGL_FORWARD_COMPAT), IDMAPSTR(GLFW_OPENGL_PROFILE),
    IDMAPSTR(GLFW_RED_BITS), IDMAPSTR(GLFW_REFRESH_RATE),
    IDMAPSTR(GLFW_RESIZABLE), IDMAPSTR(GLFW_SAMPLES),
    IDMAPSTR(GLFW_SCALE_TO_MONITOR), IDMAPSTR(GLFW_SRGB_CAPABLE),
    IDMAPSTR(GLFW_STENCIL_BITS), IDMAPSTR(GLFW_STEREO),
    IDMAPSTR(GLFW_TRANSPARENT_FRAMEBUFFER), IDMAPSTR(GLFW_VISIBLE),
    IDMAPSTR(GLFW_X11_CLASS_NAME), IDMAPSTR(GLFW_X11_INSTANCE_NAME),
  }, "GLFW_UNKNOWN" };
  // GLFW target ids to strings
  return imTargets.Get(iTarget);
}
/* -- Returns GLFW_TRUE or GLFW_FALSE depending on expression -------------- */
static int GlFWBooleanToGBoolean(const bool bC)
  { return bC ? GLFW_TRUE : GLFW_FALSE; }
/* -- Returns false if GLFW_FALSE or true if anything else ----------------- */
static bool GlFWGBooleanToBoolean(const int iR)
  { return iR != GLFW_FALSE; }
/* == Win class ========================================================= */
class GlFWWindow
{ /* -- Private variables -------------------------------------------------- */
  GLFWwindow      *wClass;             // GLFW window class
  StrVector        vFiles;             // Drag and drop file list
  /* -- Get window data pointer -------------------------------------------- */
  template<typename AnyCast=void*>
    static AnyCast GetWindowUserPointer(GLFWwindow*const wC)
      { return reinterpret_cast<AnyCast>(glfwGetWindowUserPointer(wC)); }
  template<typename AnyCast=void*>AnyCast WinGetData(void) const
    { return GetWindowUserPointer<AnyCast>(WinGetHandle()); }
  /* -- Set window data pointer -------------------------------------------- */
  template<typename AnyCast=void*const>
    static void SetWindowUserPointer(GLFWwindow*const wC, AnyCast acData)
      { glfwSetWindowUserPointer(wC, reinterpret_cast<void*>(acData)); }
  template<typename AnyCast=void*const>void WinSetData(AnyCast acData) const
    { SetWindowUserPointer<AnyCast>(WinGetHandle(), acData); }
  /* -- Clear the window handle -------------------------------------------- */
  void WinClearHandle(void) { WinSetHandle(nullptr); }
  /* -- Assign a new window handle ----------------------------------------- */
  void WinSetHandle(GLFWwindow*const wNewClass) { wClass = wNewClass; }
  /* -- Check if window handle is set and copy it or return failure -------- */
  bool WinSetHandleResult(GLFWwindow*const wNewClass)
    { if(!wNewClass) return false; WinSetHandle(wNewClass); return true; }
  /* -- Prototypes --------------------------------------------------------- */
  void WinOnDrop(GLFWwindow*const wC,
    unsigned int uiC, const char **const cpaF)
  { // Check if is our window, and check the pointer and return if empty or
    // invalid or a previous event has not been processed yet.
    if(wC != WinGetHandle() || !cpaF || !*cpaF || !vFiles.empty()) return;
    // Because we're in the main thread, we need to tell the engine thread
    // but the problem is that glfw will free 'cpaFiles' after this function
    // returns so we need to copy all the strings across unfortunately. Not
    // sure if I should mutex lock 'vFiles' as I am not sure if another
    // 'OnDrop' event will fire before this list gets to the Lua callback.
    // I highly doubt it, but if we get crashes, just bear that in mind!
    vFiles.reserve(uiC);
    for(unsigned int uiI = 0; uiI < uiC; ++uiI) vFiles.emplace_back(cpaF[uiI]);
    // Now dispatch the event
    cEvtMain->Add(EMC_INP_DRAG_DROP);
  }
  /* -- Return the window handle ----------------------------------- */ public:
  GLFWwindow *WinGetHandle(void) const { return wClass; }
  /* -- Is the window handle set? ------------------------------------------ */
  bool WinIsAvailable(void) const { return !!WinGetHandle(); }
  bool WinIsNotAvailable(void) const { return !WinIsAvailable(); }
  /* -- Get files ---------------------------------------------------------- */
  StrVector &WinGetFiles(void) { return vFiles; }
  /* -- Set window icon ---------------------------------------------------- */
  void WinSetIcon(const int iCount, const GLFWimage*const giImages) const
    { glfwSetWindowIcon(WinGetHandle(), iCount, giImages); }
  /* -- Update monitor ----------------------------------------------------- */
  void WinSetMonitor(GLFWmonitor*const mM, const int iX, const int iY,
    const int iW, const int iH, const int iR) const
      { glfwSetWindowMonitor(WinGetHandle(), mM, iX, iY, iW, iH, iR); }
  /* ----------------------------------------------------------------------- */
  /* -- Register window events --------------------------------------------- */
  void WinRegisterEvents(void) const
  { // Register glfw event callbacks which will use our event system to process
    // these events.
    glfwSetCharCallback(WinGetHandle(),
      [](GLFWwindow*const wC, unsigned int uiChar)
        { cEvtMain->Add(EMC_INP_CHAR,
            reinterpret_cast<void*>(wC), uiChar); });
    glfwSetCursorEnterCallback(WinGetHandle(),
      [](GLFWwindow*const wC, int iOnStage)
        { cEvtMain->Add(EMC_INP_MOUSE_FOCUS,
            reinterpret_cast<void*>(wC), iOnStage); });
    glfwSetCursorPosCallback(WinGetHandle(),
      [](GLFWwindow*const wC, double dX, double dY)
        { cEvtMain->Add(EMC_INP_MOUSE_MOVE,
            reinterpret_cast<void*>(wC), dX, dY); });
    glfwSetDropCallback(WinGetHandle(),
      [](GLFWwindow*const wC, int iC, const char**const cpaFiles)
        { GetWindowUserPointer<GlFWWindow*>(wC)->
            WinOnDrop(wC, static_cast<unsigned int>(iC), cpaFiles); });
    glfwSetFramebufferSizeCallback(WinGetHandle(),
      [](GLFWwindow*const wC, int iW, int iH)
        { cEvtMain->Add(EMC_VID_FB_REINIT,
            reinterpret_cast<void*>(wC), iW, iH); });
    glfwSetJoystickCallback(
      [](int iJId, int iEvent)
        { cEvtMain->Add(EMC_INP_JOY_STATE, iJId, iEvent); });
    glfwSetKeyCallback(WinGetHandle(),
      [](GLFWwindow*const wC, int iKey, int iScanCode, int iAction, int iMods)
        { cEvtMain->Add(EMC_INP_KEYPRESS, reinterpret_cast<void*>(wC), iKey,
            iScanCode, iAction, iMods); });
    glfwSetMouseButtonCallback(WinGetHandle(),
      [](GLFWwindow*const wC, int iButton, int iAction, int iMods)
        { cEvtMain->Add(EMC_INP_MOUSE_CLICK, reinterpret_cast<void*>(wC),
            iButton, iAction, iMods); });
    glfwSetScrollCallback(WinGetHandle(),
      [](GLFWwindow*const wC, double dX, double dY)
        { cEvtMain->Add(EMC_INP_MOUSE_SCROLL,
            reinterpret_cast<void*>(wC), dX, dY); });
    glfwSetWindowCloseCallback(WinGetHandle(),
      [](GLFWwindow*const wC)
    { // GLFW must be denied because we will close the window instead.
      glfwSetWindowShouldClose(wC, GL_FALSE);
      // Dispatch the event to handle this ourselve.
      cEvtMain->Add(EMC_WIN_CLOSE, reinterpret_cast<void*>(wC));
    });
    glfwSetWindowContentScaleCallback(WinGetHandle(),
      [](GLFWwindow*const wC, const float fX, const float fY)
        { cEvtMain->Add(EMC_WIN_SCALE,
            reinterpret_cast<void*>(wC), fX, fY); });
    glfwSetWindowFocusCallback(WinGetHandle(),
      [](GLFWwindow*const wC, int iFocused)
        { cEvtMain->Add(EMC_WIN_FOCUS,
            reinterpret_cast<void*>(wC), iFocused); });
    glfwSetWindowIconifyCallback(WinGetHandle(),
      [](GLFWwindow*const wC, int iIconified)
        { cEvtMain->Add(EMC_WIN_ICONIFY,
            reinterpret_cast<void*>(wC), iIconified); });
    glfwSetWindowPosCallback(WinGetHandle(),
      [](GLFWwindow*const wC, int iX, int iY)
        { cEvtMain->Add(EMC_WIN_MOVED, reinterpret_cast<void*>(wC), iX, iY);});
    glfwSetWindowRefreshCallback(WinGetHandle(),
      [](GLFWwindow*const wC)
        { cEvtMain->Add(EMC_WIN_REFRESH, reinterpret_cast<void*>(wC)); });
    glfwSetWindowRefreshCallback(WinGetHandle(),
      [](GLFWwindow*const wC)
        { cEvtMain->Add(EMC_WIN_REFRESH, reinterpret_cast<void*>(wC)); });
    glfwSetWindowSizeCallback(WinGetHandle(),
      [](GLFWwindow*const wC, int iW, int iH)
        { cEvtMain->Add(EMC_WIN_RESIZED,
            reinterpret_cast<void*>(wC), iW, iH); });
  }
  /* -- Unregister window events ------------------------------------------- */
  void WinUnregisterEvents(void) const
  { // Unregister global GLFW events
    glfwSetJoystickCallback(nullptr);
    glfwSetMonitorCallback(nullptr);
    // Done if theres no window class
    if(WinIsNotAvailable()) return;
    // Remove other callbacks
    glfwSetWindowSizeCallback(WinGetHandle(), nullptr);
    glfwSetWindowRefreshCallback(WinGetHandle(), nullptr);
    glfwSetWindowPosCallback(WinGetHandle(), nullptr);
    glfwSetWindowIconifyCallback(WinGetHandle(), nullptr);
    glfwSetWindowFocusCallback(WinGetHandle(), nullptr);
    glfwSetWindowContentScaleCallback(WinGetHandle(), nullptr);
    glfwSetWindowCloseCallback(WinGetHandle(), nullptr);
    glfwSetScrollCallback(WinGetHandle(), nullptr);
    glfwSetMouseButtonCallback(WinGetHandle(), nullptr);
    glfwSetKeyCallback(WinGetHandle(), nullptr);
    glfwSetFramebufferSizeCallback(WinGetHandle(), nullptr);
    glfwSetDropCallback(WinGetHandle(), nullptr);
    glfwSetCursorPosCallback(WinGetHandle(), nullptr);
    glfwSetCursorEnterCallback(WinGetHandle(), nullptr);
    glfwSetCharCallback(WinGetHandle(), nullptr);
  }
  /* -- Tell GLFW if it should close the window ---------------------------- */
  void WinSetClose(const int iS) const
    { glfwSetWindowShouldClose(WinGetHandle(), iS); }
  /* -- Returns if the window should close --------------------------------- */
  bool WinShouldClose(void) const
    { return !WinIsAvailable() || glfwWindowShouldClose(WinGetHandle()); }
  /* -- Restore the window from being minimised ---------------------------- */
  void WinRestore(void) const { glfwRestoreWindow(WinGetHandle()); }
  /* -- Minimise/Iconify the window ---------------------------------------- */
  void WinMinimise(void) const { glfwIconifyWindow(WinGetHandle()); }
  /* -- Maximise the window ------------------------------------------------ */
  void WinMaximise(void) const { glfwMaximizeWindow(WinGetHandle()); }
  /* -- Set focus on the window -------------------------------------------- */
  void WinFocus(void) const { glfwFocusWindow(WinGetHandle()); }
  /* -- Show a window and register events ---------------------------------- */
  void WinShow(void) const
    { glfwShowWindow(WinGetHandle()); WinRegisterEvents(); }
  /* -- Hide a window and register events ---------------------------------- */
  void WinHide(void) const
    { WinUnregisterEvents(); glfwHideWindow(WinGetHandle()); }
  /* -- Set window size ---------------------------------------------------- */
  void WinSetSize(const int iW, const int iH) const
    { glfwSetWindowSize(WinGetHandle(), iW, iH); }
  /* -- Get window size ---------------------------------------------------- */
  void WinGetSize(int &iW, int &iH) const
    { glfwGetWindowSize(WinGetHandle(), &iW, &iH); }
  /* -- Get window DPI scale ----------------------------------------------- */
  void WinGetScale(float &fW, float &fH) const
    { glfwGetWindowContentScale(WinGetHandle(), &fW, &fH); }
  /* -- Get framebuffer size ----------------------------------------------- */
  void WinGetFBSize(int &iW, int &iH)
    { glfwGetFramebufferSize(WinGetHandle(), &iW, &iH); }
  /* -- Set or clear cursor graphic ---------------------------------------- */
  void WinSetCursorGraphic(GLFWcursor*const gcC)
    { glfwSetCursor(WinGetHandle(), gcC); }
  /* -- Toggle cursor visibility ------------------------------------------- */
  void WinSetCursor(const bool bS) const
    { glfwSetInputMode(WinGetHandle(), GLFW_CURSOR,
        bS ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN); }
  /* -- Get/set cursor position -------------------------------------------- */
  void WinGetCursorPos(double &dX, double &dY) const
    { glfwGetCursorPos(WinGetHandle(), &dX, &dY); }
  void WinSetCursorPos(const double dX, const double dY) const
    { glfwSetCursorPos(WinGetHandle(), dX, dY); }
  /* -- Set clipboard c-string --------------------------------------------- */
  void WinSetClipboard(const char*const cpT)
    { glfwSetClipboardString(WinGetHandle(), cpT); }
  /* -- Set clipboard c++ string ------------------------------------------- */
  void WinSetClipboardString(const string &strT)
    { WinSetClipboard(strT.c_str()); }
  /* -- Get clipboard c-string --------------------------------------------- */
  const char *WinGetClipboard(void) const
  { // Return clipboard string if available
    if(const char*const cpData =
      glfwGetClipboardString(WinGetHandle())) return cpData;
    // Return empty string
    return cpBlank;
  }
  /* -- Get clipboard C++ string ------------------------------------------- */
  const string GetClipboardString(void) const
    { return WinGetClipboard(); }
  /* -- Get mouse/key button state ----------------------------------------- */
  int WinGetMouse(const int iB) const
    { return glfwGetMouseButton(WinGetHandle(), iB); }
  int WinGetKey(const int iK) const
    { return glfwGetKey(WinGetHandle(), iK); }
  /* -- Get window attributes ---------------------------------------------- */
  int WinGetAttrib(const int iA) const
    { return glfwGetWindowAttrib(WinGetHandle(), iA); }
  /* -- Get window hint boolean -------------------------------------------- */
  bool WinGetAttribBoolean(const int iVar) const
    { return GlFWGBooleanToBoolean(WinGetAttrib(iVar)); }
  /* -- Update window attributes ------------------------------------------- */
  void WinSetAttrib(const int iAttrib, const int iValue) const
  { // Set the window attribute
    glfwSetWindowAttrib(WinGetHandle(), iAttrib, iValue);
    // Log the attribute change
    LW(LH_DEBUG, "GlFW set attrib $<0x$$> to $$<0x$$>.",
      GlFWGetHintAttribStr(iAttrib), hex, iAttrib, dec, iValue, hex, iValue);
  }
  /* -- Set window aspect ratio -------------------------------------------- */
  void WinSetAspectRatio(const int iNumeric, const int iDenominator)
    { glfwSetWindowAspectRatio(WinGetHandle(), iNumeric, iDenominator); }
  void WinSetFreeAspectRatio(void)
    { WinSetAspectRatio(GLFW_DONT_CARE, GLFW_DONT_CARE); }
  /* -- Set window aspect ratio with a float or double --------------------- */
  void WinSetAspectRatio(const string &strVal)
  { // Seperate numeric and denominator
    const size_t stPos = strVal.find('.');
    if(stPos != string::npos)
    { // Get numeric and denominator
      const int iNumeric = ToNumber<int>(strVal.substr(0, stPos)),
        iDenominator = ToNumber<int>(strVal.substr(stPos+1));
      // Free aspect ratio if either are invalid
      if(iNumeric > 0 && iDenominator > 0)
        return WinSetAspectRatio(iNumeric, iDenominator);
    } // Not valid so free the aspect ratio
    WinSetFreeAspectRatio();
  }
  /* -- Move window -------------------------------------------------------- */
  void WinMove(const int iX, const int iY) const
    { glfwSetWindowPos(WinGetHandle(), iX, iY); }
  /* -- Get window size ---------------------------------------------------- */
  void WinGetPos(int &iX, int &iY) const
    { glfwGetWindowPos(WinGetHandle(), &iX, &iY); }
  /* -- Swap GL buffers ---------------------------------------------------- */
  void WinSwapGLBuffers(void) const { glfwSwapBuffers(WinGetHandle()); }
  /* -- Set window attribute core functions -------------------------------- */
  void WinSetAttribBoolean(const int iVar, const bool bVal) const
    { WinSetAttrib(iVar, GlFWBooleanToGBoolean(bVal)); }
  void WinSetAttribEnabled(const int iVar) const
    { WinSetAttribBoolean(iVar, true); }
  void WinSetAttribDisabled(const int iVar) const
    { WinSetAttribBoolean(iVar, false); }
  /* -- Create functions to access all attributes ------------------------- */\
#define SET(nc,nu) \
  [[maybe_unused]] void WinSet ## nc ## Attrib(const bool bState) const \
    { WinSetAttribBoolean(GLFW_ ## nu, bState); } \
  [[maybe_unused]] void WinSet ## nc ## AttribEnabled(void) const \
    { WinSetAttribEnabled(GLFW_ ## nu); } \
  [[maybe_unused]] void WinSet ## nc ## AttribDisabled(void) const \
    { WinSetAttribDisabled(GLFW_ ## nu); } \
  [[maybe_unused]] bool WinIs ## nc ## AttribEnabled(void) const \
    { return GlFWGBooleanToBoolean(WinGetAttrib(GLFW_ ## nu)); } \
  [[maybe_unused]] bool WinIs ## nc ## AttribDisabled(void) const \
    { return !WinIs ## nc ## AttribEnabled(); }
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
  SET(Resizable, RESIZABLE);           // Win is resizable?
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
  /* -- Check current context ---------------------------------------------- */
  bool WinIsCurrentContext(void) const
    { return glfwGetCurrentContext() == WinGetHandle(); }
  /* -- Make current context ----------------------------------------------- */
  void WinSetContext(void) const
    { glfwMakeContextCurrent(WinGetHandle()); }
  /* -- Set window size limits --------------------------------------------- */
  void WinSetLimits(const int iMinW, const int iMinH,
    const int iMaxW, const int iMaxH)
      { glfwSetWindowSizeLimits(WinGetHandle(),
          iMinW, iMinH, iMaxW, iMaxH); }
  /* -- Request window attention ------------------------------------------- */
  void WinRequestAttention(void) const
    { glfwRequestWindowAttention(WinGetHandle()); }
  /* -- Get or set input mode ---------------------------------------------- */
  int WinGetInputMode(const int iM) const
    { return glfwGetInputMode(WinGetHandle(), iM); }
  bool WinGetInputModeBoolean(const int iM) const
    { return GlFWGBooleanToBoolean(WinGetInputMode(iM)); }
  void WinSetInputMode(const int iM, const int iV) const
    { glfwSetInputMode(WinGetHandle(), iM, iV); }
  void WinSetInputModeBoolean(const int iM, const bool bS) const
    { WinSetInputMode(iM, GlFWBooleanToGBoolean(bS)); }
  /* -- Set/get raw mouse motion ------------------------------------------- */
  void WinSetRawMouseMotion(const bool bS)
    { WinSetInputModeBoolean(GLFW_RAW_MOUSE_MOTION, bS); }
  bool WinGetRawMouseMotion(void)
    { return WinGetInputModeBoolean(GLFW_RAW_MOUSE_MOTION); }
  /* -- Set/get sticky mouse buttons --------------------------------------- */
  void WinSetStickyMouseButtons(const bool bS)
    { WinSetInputModeBoolean(GLFW_STICKY_MOUSE_BUTTONS, bS); }
  bool WinGetStickyMouseButtons(void)
    { return WinGetInputModeBoolean(GLFW_STICKY_MOUSE_BUTTONS); }
  /* -- Set/get sticky keys ------------------------------------------------ */
  void WinSetStickyKeys(const bool bS)
    { WinSetInputModeBoolean(GLFW_STICKY_KEYS, bS); }
  bool WinGetStickyKeys(void)
    { return WinGetInputModeBoolean(GLFW_STICKY_KEYS); }
  /* -- Get mouse buttons tatus--------------------------------------------- */
  int WinGetMouseButton(const int iB) const
    { return glfwGetMouseButton(this->WinGetHandle(), iB); }
  /* -- Destroy the GLFW window -------------------------------------------- */
  void WinDeInit(void)
  { // Done if theres no window class
    if(WinIsNotAvailable()) return;
    // Destroy the window
    glfwDestroyWindow(WinGetHandle());
    // Nullify the handle
    WinClearHandle();
    // Unregister remaining events
    WinUnregisterEvents();
  }
  /* -- Create the window -------------------------------------------------- */
  GLFWwindow *WinInit(const int iW, const int iH,
    const char*const cpT, GLFWmonitor*const mM)
  { // Bail if handle already set
    if(WinIsAvailable())
      XC("Window already created!",
         "Class", WinGetHandle(), "Width",   iW, "Height", iH,
         "Title", cpT,               "Monitor", mM);
    // Set handle and create window, throw exception if failed
    if(!WinSetHandleResult(glfwCreateWindow(iW, iH, cpT, mM, nullptr)))
      XC("Failed to create window!",
         "Width", iW, "Height", iH, "Title", cpT, "Monitor", mM);
    // Set user pointer to this class
    WinSetData(this);
    // Return the class we created
    return WinGetHandle();
  }
  /* -- Constructor --------------------------------------------- */ protected:
  GlFWWindow(void) :
    /* -- Initialisation of members ---------------------------------------- */
    wClass{ nullptr }                  // Uninitialised window context
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(GlFWWindow);         // Do not need defaults
};/* ----------------------------------------------------------------------- */
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
