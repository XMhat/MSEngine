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
/* ------------------------------------------------------------------------- */
using namespace IfEvtMain;             // Using event interface
/* == GlFW manager class =================================================== */
static class GlFW final :
  /* -- Base classes ------------------------------------------------------- */
  public IHelper                       // Initialisation helper
{ /* -- Private variables -------------------------------------------------- */
  const IdMap<int> imTargets;          // GLFW target ids to strings
  GLFWwindow      *wClass;             // GLFW window class
  StrVector        vFiles;             // Drag and drop file list
  unsigned int     uiErrorLevel;       // Ignore further glfw errors
  /* -- Error handler prototype -------------------------------------------- */
  static void ErrorHandler(int, const char*const);
  /* -- Return the window handle ------------------------------------------- */
  GLFWwindow *GetWindowHandle(void) const { return wClass; }
  /* -- Assign a new window handle ----------------------------------------- */
  void SetWindowHandle(GLFWwindow*const wNewClass) { wClass = wNewClass; }
  /* -- Clear the window handle -------------------------------------------- */
  void ClearWindowHandle(void) { SetWindowHandle(nullptr); }
  /* -- Check if window handle is set and copy it or return failure -------- */
  bool SetWindowHandleResult(GLFWwindow*const wNewClass)
  { // Return failure if no handle
    if(!wNewClass) return false;
    // Set the new handle
    SetWindowHandle(wNewClass);
    // Succeeded
    return true;
  }
  /* -- Prototypes --------------------------------------------------------- */
  static void OnDropSt(GLFWwindow*const, int, const char**const);
  void OnDrop(GLFWwindow*const wC, unsigned int uiC, const char **const cpaF)
  { // Check if is our window, and check the pointer and return if empty or
    // invalid or a previous event has not been processed yet.
    if(wC != GetWindowHandle() || !cpaF || !*cpaF || !vFiles.empty()) return;
    // Because we're in the main thread, we need to tell the engine thread
    // but the problem is that glfw will free 'cpaFiles' after this function
    // returns so we need to copy all the strings across unfortunately. Not
    // sure if I should mutex lock 'vFiles' as I am not sure if another
    // 'OnDrop' event will fire before this list gets to the Lua callback.
    // I highly doubt it, but if we get crashes, just bear that in mind!
    vFiles.reserve(uiC);
    for(unsigned int uiI = 0; uiI < uiC; ++uiI)
      vFiles.emplace_back(cpaF[uiI]);
    // Now dispatch the event
    cEvtMain->Add(EMC_INP_DRAG_DROP);
  }
  /* -- Register window events --------------------------------------------- */
  void RegisterEvents(void) const
  { // Register glfw event callbacks which will use our event system to process
    // these events.
    glfwSetCharCallback(GetWindowHandle(),
      [](GLFWwindow*const wC, unsigned int uiChar)
        { cEvtMain->Add(EMC_INP_CHAR,
            reinterpret_cast<void*>(wC), uiChar); });
    glfwSetCursorEnterCallback(GetWindowHandle(),
      [](GLFWwindow*const wC, int iOnStage)
        { cEvtMain->Add(EMC_INP_MOUSE_FOCUS,
            reinterpret_cast<void*>(wC), iOnStage); });
    glfwSetCursorPosCallback(GetWindowHandle(),
      [](GLFWwindow*const wC, double dX, double dY)
        { cEvtMain->Add(EMC_INP_MOUSE_MOVE,
            reinterpret_cast<void*>(wC), dX, dY); });
    glfwSetDropCallback(GetWindowHandle(), OnDropSt);
    glfwSetFramebufferSizeCallback(GetWindowHandle(),
      [](GLFWwindow*const wC, int iW, int iH)
        { cEvtMain->Add(EMC_VID_FB_REINIT,
            reinterpret_cast<void*>(wC), iW, iH); });
    glfwSetJoystickCallback(
      [](int iJId, int iEvent)
        { cEvtMain->Add(EMC_INP_JOY_STATE, iJId, iEvent); });
    glfwSetKeyCallback(GetWindowHandle(),
      [](GLFWwindow*const wC, int iKey, int iScanCode, int iAction, int iMods)
        { cEvtMain->Add(EMC_INP_KEYPRESS, reinterpret_cast<void*>(wC), iKey,
            iScanCode, iAction, iMods); });
    glfwSetMouseButtonCallback(GetWindowHandle(),
      [](GLFWwindow*const wC, int iButton, int iAction, int iMods)
        { cEvtMain->Add(EMC_INP_MOUSE_CLICK, reinterpret_cast<void*>(wC),
            iButton, iAction, iMods); });
    glfwSetScrollCallback(GetWindowHandle(),
      [](GLFWwindow*const wC, double dX, double dY)
        { cEvtMain->Add(EMC_INP_MOUSE_SCROLL,
            reinterpret_cast<void*>(wC), dX, dY); });
    glfwSetWindowCloseCallback(GetWindowHandle(),
      [](GLFWwindow*const wC)
    { // GLFW must be denied because we will close the window instead.
      glfwSetWindowShouldClose(wC, GL_FALSE);
      // Dispatch the event to handle this ourselve.
      cEvtMain->Add(EMC_WIN_CLOSE, reinterpret_cast<void*>(wC));
    });
    glfwSetWindowContentScaleCallback(GetWindowHandle(),
      [](GLFWwindow*const wC, const float fX, const float fY)
        { cEvtMain->Add(EMC_WIN_SCALE,
            reinterpret_cast<void*>(wC), fX, fY); });
    glfwSetWindowFocusCallback(GetWindowHandle(),
      [](GLFWwindow*const wC, int iFocused)
        { cEvtMain->Add(EMC_WIN_FOCUS,
            reinterpret_cast<void*>(wC), iFocused); });
    glfwSetWindowIconifyCallback(GetWindowHandle(),
      [](GLFWwindow*const wC, int iIconified)
        { cEvtMain->Add(EMC_WIN_ICONIFY,
            reinterpret_cast<void*>(wC), iIconified); });
    glfwSetWindowPosCallback(GetWindowHandle(),
      [](GLFWwindow*const wC, int iX, int iY)
        { cEvtMain->Add(EMC_WIN_MOVED, reinterpret_cast<void*>(wC), iX, iY);});
    glfwSetWindowRefreshCallback(GetWindowHandle(),
      [](GLFWwindow*const wC)
        { cEvtMain->Add(EMC_WIN_REFRESH, reinterpret_cast<void*>(wC)); });
    glfwSetWindowRefreshCallback(GetWindowHandle(),
      [](GLFWwindow*const wC)
        { cEvtMain->Add(EMC_WIN_REFRESH, reinterpret_cast<void*>(wC)); });
    glfwSetWindowSizeCallback(GetWindowHandle(),
      [](GLFWwindow*const wC, int iW, int iH)
        { cEvtMain->Add(EMC_WIN_RESIZED,
            reinterpret_cast<void*>(wC), iW, iH); });
  }
  /* -- Unregister window events ------------------------------------------- */
  void UnregisterEvents(void) const
  { // Unregister global GLFW events
    glfwSetJoystickCallback(nullptr);
    glfwSetMonitorCallback(nullptr);
    // Done if theres no window class
    if(IsWindowNotAvailable()) return;
    // Remove other callbacks
    glfwSetWindowSizeCallback(GetWindowHandle(), nullptr);
    glfwSetWindowRefreshCallback(GetWindowHandle(), nullptr);
    glfwSetWindowPosCallback(GetWindowHandle(), nullptr);
    glfwSetWindowIconifyCallback(GetWindowHandle(), nullptr);
    glfwSetWindowFocusCallback(GetWindowHandle(), nullptr);
    glfwSetWindowContentScaleCallback(GetWindowHandle(), nullptr);
    glfwSetWindowCloseCallback(GetWindowHandle(), nullptr);
    glfwSetScrollCallback(GetWindowHandle(), nullptr);
    glfwSetMouseButtonCallback(GetWindowHandle(), nullptr);
    glfwSetKeyCallback(GetWindowHandle(), nullptr);
    glfwSetFramebufferSizeCallback(GetWindowHandle(), nullptr);
    glfwSetDropCallback(GetWindowHandle(), nullptr);
    glfwSetCursorPosCallback(GetWindowHandle(), nullptr);
    glfwSetCursorEnterCallback(GetWindowHandle(), nullptr);
    glfwSetCharCallback(GetWindowHandle(), nullptr);
  }
  /* -- Convert window hint id to string ----------------------------------- */
  const string &GetHintName(const int iVar) const
    { return imTargets.Get(iVar); }
  /* --------------------------------------------------------------- */ public:
  void SetIcon(const int iC, const GLFWimage*const iData)
    { glfwSetWindowIcon(GetWindowHandle(), iC, iData); }
  /* -- Update full-screen ------------------------------------------------- */
  void SetWindowMonitor(GLFWmonitor*const mM, const int iX, const int iY,
    const int iW, const int iH, const int iR)
      { glfwSetWindowMonitor(GetWindowHandle(), mM, iX, iY, iW, iH, iR); }
  /* ----------------------------------------------------------------------- */
  static void ForceEventHack(void) { glfwPostEmptyEvent(); }
  /* -- Tell GLFW if it should close the window ---------------------------- */
  void SetCloseWindow(const int iS) const
    { glfwSetWindowShouldClose(GetWindowHandle(), iS); }
  /* -- Get/Set window data ------------------------------------------------ */
  template<typename AnyCast=void*>AnyCast GetWindowData(void) const
    { return reinterpret_cast<AnyCast>
        (glfwGetWindowUserPointer(GetWindowHandle())); }
  template<typename AnyCast=void*const>void SetWindowData(AnyCast acData) const
    { glfwSetWindowUserPointer(GetWindowHandle(),
        reinterpret_cast<void*>(acData)); }
  /* -- Create the window -------------------------------------------------- */
  GLFWwindow *InitWindow(const int iW, const int iH,
    const char*const cpT, GLFWmonitor*const mM)
  { // Bail if handle already set
    if(IsWindowAvailable())
      XC("Window already created!",
         "Class", GetWindowHandle(), "Width",   iW, "Height", iH,
         "Title", cpT,               "Monitor", mM);
    // Set handle and create window, throw exception if failed
    if(!SetWindowHandleResult(glfwCreateWindow(iW, iH, cpT, mM, nullptr)))
      XC("Failed to create window!",
         "Width", iW, "Height", iH, "Title", cpT, "Monitor", mM);
    // Set user pointer to this class
    SetWindowData(this);
    // Return the class we created
    return GetWindowHandle();
  }
  /* -- Get files ---------------------------------------------------------- */
  StrVector &GetFiles(void) { return vFiles; }
  /* -- Destroy the GLFW window -------------------------------------------- */
  void DestroyWindow(void)
  { // Done if theres no window class
    if(IsWindowNotAvailable()) return;
    // Destroy the window
    glfwDestroyWindow(GetWindowHandle());
    // Nullify the handle
    ClearWindowHandle();
    // Unregister remaining events
    UnregisterEvents();
  }
  /* -- WindowShouldClose? ------------------------------------------------- */
  bool ShouldWindowClose(void) const
    { return !IsWindowAvailable() ||
        glfwWindowShouldClose(GetWindowHandle()); }
  /* -- Joystick axes ------------------------------------------------------ */
  static const float *GetJoystickAxes(int iJ, int &iJAC)
    { return glfwGetJoystickAxes(iJ, &iJAC); }
  /* -- Joystick buttons --------------------------------------------------- */
  static const unsigned char *GetJoystickButtons(int iJ, int &iJAB)
    { return glfwGetJoystickButtons(iJ, &iJAB); }
  /* -- Joystick other ----------------------------------------------------- */
  static const char *GetJoystickName(const int iJ)
    { return glfwGetJoystickName(iJ); }
  /* -- Is the window handle set? ------------------------------------------ */
  bool IsWindowAvailable(void) const { return !!GetWindowHandle(); }
  bool IsWindowNotAvailable(void) const { return !IsWindowAvailable(); }
  /* -- Set/Get window size ------------------------------------------------ */
  void SetWindowSize(const int iW, const int iH) const
    { glfwSetWindowSize(GetWindowHandle(), iW, iH); }
  void GetWindowSize(int &iW, int &iH) const
    { glfwGetWindowSize(GetWindowHandle(), &iW, &iH); }
  void GetWindowScale(float &fW, float &fH) const
    { glfwGetWindowContentScale(GetWindowHandle(), &fW, &fH); }
  /* -- Get framebuffer size ----------------------------------------------- */
  void GetFBSize(int &iW, int &iH)
    { glfwGetFramebufferSize(GetWindowHandle(), &iW, &iH); }
  /* -- Set or clear cursor graphic ---------------------------------------- */
  void SetCursorGraphic(GLFWcursor*const gcC)
    { glfwSetCursor(GetWindowHandle(), gcC); }
  /* -- Toggle cursor visibility ------------------------------------------- */
  void SetCursor(const bool bS) const
    { glfwSetInputMode(GetWindowHandle(), GLFW_CURSOR,
        bS ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_HIDDEN); }
  /* -- Get/set cursor position -------------------------------------------- */
  void GetCursorPos(double &dX, double &dY) const
    { glfwGetCursorPos(GetWindowHandle(), &dX, &dY); }
  void SetCursorPos(const double dX, const double dY) const
    { glfwSetCursorPos(GetWindowHandle(), dX, dY); }
  /* -- Set clipboard c-string --------------------------------------------- */
  void SetClipboard(const char*const cpT)
    { glfwSetClipboardString(GetWindowHandle(), cpT); }
  /* -- Set clipboard c++ string ------------------------------------------- */
  void SetClipboardString(const string &strT)
    { SetClipboard(strT.c_str()); }
  /* -- Get clipboard c-string --------------------------------------------- */
  const char *GetClipboard(void) const
  { // Return clipboard string if available
    if(const char*const cpData =
      glfwGetClipboardString(GetWindowHandle())) return cpData;
    // Return empty string
    return cpBlank;
  }
  /* -- Get clipboard C++ string ------------------------------------------- */
  const string GetClipboardString(void) const
    { return GetClipboard(); }
  /* -- Set swap interval -------------------------------------------------- */
  static void SetVSync(const int iI) { glfwSwapInterval(iI); }
  /* -- Get mouse/key button state ----------------------------------------- */
  int GetMouse(const int iB) const
    { return glfwGetMouseButton(GetWindowHandle(), iB); }
  int GetKey(const int iK) const
    { return glfwGetKey(GetWindowHandle(), iK); }
  static const char *GetKeyName(const int iK, const int iSC)
    { return glfwGetKeyName(iK, iSC); }
  /* -- Get window attributes ---------------------------------------------- */
  int GetWindowAttrib(const int iA) const
    { return glfwGetWindowAttrib(GetWindowHandle(), iA); }
  /* -- Get window hint boolean -------------------------------------------- */
  bool GetWindowAttribBoolean(const int iVar) const
    { return GLFWBooleanToBoolean(GetWindowAttrib(iVar)); }
  /* -- Update window attributes ------------------------------------------- */
  void SetWindowAttrib(const int iVar, const int iVal) const
  { // Set the window attribute
    glfwSetWindowAttrib(GetWindowHandle(), iVar, iVal);
    // Log the attribute change
    LW(LH_DEBUG, "GlFW set attrib $<0x$$> to $$<0x$$>.",
      GetHintName(iVar), hex, iVar, dec, iVal, hex, iVal);
  }
  /* -- Window state ------------------------------------------------------- */
  void RestoreWindow(void) const
    { glfwRestoreWindow(GetWindowHandle()); }
  void MinimiseWindow(void) const
    { glfwIconifyWindow(GetWindowHandle()); }
  void MaximiseWindow(void) const
    { glfwMaximizeWindow(GetWindowHandle()); }
  void FocusWindow(void) const
    { glfwFocusWindow(GetWindowHandle()); }
  void ShowWindow(void) const
    { glfwShowWindow(GetWindowHandle()); RegisterEvents(); }
  void HideWindow(void) const
    { UnregisterEvents(); glfwHideWindow(GetWindowHandle()); }
  /* -- Set window aspect ratio -------------------------------------------- */
  void SetAspectRatio(const int iNumeric, const int iDenominator)
    { glfwSetWindowAspectRatio(GetWindowHandle(), iNumeric, iDenominator); }
  void SetFreeAspectRatio(void)
    { SetAspectRatio(GLFW_DONT_CARE, GLFW_DONT_CARE); }
  /* -- Set window aspect ratio with a float or double --------------------- */
  void SetAspectRatio(const string &strVal)
  { // Seperate numeric and denominator
    const size_t stPos = strVal.find('.');
    if(stPos != string::npos)
    { // Get numeric and denominator
      const int iNumeric = ToNumber<int>(strVal.substr(0, stPos)),
        iDenominator = ToNumber<int>(strVal.substr(stPos+1));
      // Free aspect ratio if either are invalid
      if(iNumeric > 0 && iDenominator > 0)
        return SetAspectRatio(iNumeric, iDenominator);
    } // Not valid so free the aspect ratio
    SetFreeAspectRatio();
  }
  /* -- Move window -------------------------------------------------------- */
  void MoveWindow(const int iX, const int iY) const
    { glfwSetWindowPos(GetWindowHandle(), iX, iY); }
  /* -- Get window size ---------------------------------------------------- */
  void GetWindowPos(int &iX, int &iY) const
    { glfwGetWindowPos(GetWindowHandle(), &iX, &iY); }
  /* -- Swap GL buffers ---------------------------------------------------- */
  void SwapGLBuffers(void) const { glfwSwapBuffers(GetWindowHandle()); }
  /* -- Wait for window event ---------------------------------------------- */
  static void WaitEvents(void) { glfwWaitEvents(); }
  static void PollEvents(void) { glfwPollEvents(); }
  /* -- Check current context ---------------------------------------------- */
  bool IsCurrentContext(void) const
    { return glfwGetCurrentContext() == GetWindowHandle(); }
  /* -- Make current context ----------------------------------------------- */
  void SetContext(void) const
    { glfwMakeContextCurrent(GetWindowHandle()); }
  /* -- Release current context -------------------------------------------- */
  static void ReleaseContext(void)
    { glfwMakeContextCurrent(nullptr); }
  /* -- Set window size limits --------------------------------------------- */
  void SetLimits(const int iMinW, const int iMinH,
    const int iMaxW, const int iMaxH)
      { glfwSetWindowSizeLimits(GetWindowHandle(),
          iMinW, iMinH, iMaxW, iMaxH); }
  /* -- Set gamma ---------------------------------------------------------- */
  static void SetGamma(GLFWmonitor*const mDevice, const GLfloat fValue)
    { glfwSetGamma(mDevice, fValue); }
  /* -- Set gamma ---------------------------------------------------------- */
  void SetIcon(const int iCount, const GLFWimage*const giImages) const
    { glfwSetWindowIcon(GetWindowHandle(), iCount, giImages); }
  /* -- Request window attention ------------------------------------------- */
  void RequestWindowAttention(void) const
    { glfwRequestWindowAttention(GetWindowHandle()); }
  /* -- Returns GLFW_TRUE or GLFW_FALSE depending on expression ------------ */
  static int BooleanToGLFWBoolean(const bool bC)
    { return bC ? GLFW_TRUE : GLFW_FALSE; }
  static bool GLFWBooleanToBoolean(const int iR)
    { return iR != GLFW_FALSE; }
  /* -- Get or set input mode ---------------------------------------------- */
  int GetInputMode(const int iM) const
    { return glfwGetInputMode(GetWindowHandle(), iM); }
  bool GetInputModeBoolean(const int iM) const
    { return GLFWBooleanToBoolean(GetInputMode(iM)); }
  void SetInputMode(const int iM, const int iV) const
    { glfwSetInputMode(GetWindowHandle(), iM, iV); }
  void SetInputModeBoolean(const int iM, const bool bS) const
    { SetInputMode(iM, BooleanToGLFWBoolean(bS)); }
  /* -- Set/get raw mouse motion ------------------------------------------- */
  void SetRawMouseMotion(const bool bS)
    { SetInputModeBoolean(GLFW_RAW_MOUSE_MOTION, bS); }
  bool GetRawMouseMotion(void)
    { return GetInputModeBoolean(GLFW_RAW_MOUSE_MOTION); }
  static bool IsRawMouseMotionSupported(void)
    { return GLFWBooleanToBoolean(glfwRawMouseMotionSupported()); }
  /* -- Set/get sticky mouse buttons --------------------------------------- */
  void SetStickyMouseButtons(const bool bS)
    { SetInputModeBoolean(GLFW_STICKY_MOUSE_BUTTONS, bS); }
  bool GetStickyMouseButtons(void)
    { return GetInputModeBoolean(GLFW_STICKY_MOUSE_BUTTONS); }
  /* -- Set/get sticky keys ------------------------------------------------ */
  void SetStickyKeys(const bool bS)
    { SetInputModeBoolean(GLFW_STICKY_KEYS, bS); }
  bool GetStickyKeys(void)
    { return GetInputModeBoolean(GLFW_STICKY_KEYS); }
  /* -- Get mouse buttons tatus--------------------------------------------- */
  int GetMouseButton(const int iB) const
    { return glfwGetMouseButton(GetWindowHandle(), iB); }
  /* == Set window hint ==================================================== */
  static void SetDefaultWindowHints(void) { glfwDefaultWindowHints(); }
  /* -- Set window hint string --------------------------------------------- */
  void SetWindowHintString(const int iVar, const char*const cpVal) const
  { // Set window hint directly
    glfwWindowHintString(iVar, cpVal);
    // Log the change
    LW(LH_DEBUG, "GlFW set hint $<0x$$> to '$'.",
      GetHintName(iVar), hex, iVar, cpVal);
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
      GetHintName(iVar), hex, iVar, dec, iVal, hex, iVal);
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
  /* -- Set window attribute core functions -------------------------------- */
  void SetWindowAttribBoolean(const int iVar, const bool bVal) const
    { SetWindowAttrib(iVar, BooleanToGLFWBoolean(bVal)); }
  void SetWindowAttribEnabled(const int iVar) const
    { SetWindowAttribBoolean(iVar, true); }
  void SetWindowAttribDisabled(const int iVar) const
    { SetWindowAttribBoolean(iVar, false); }
  /* -- Create functions to access all attributes ------------------------- */\
#define SET(nc,nu) \
  [[maybe_unused]] void Set ## nc ## Attrib(const bool bState) const \
    { SetWindowAttribBoolean(GLFW_ ## nu, bState); } \
  [[maybe_unused]] void Set ## nc ## AttribEnabled(void) const \
    { SetWindowAttribEnabled(GLFW_ ## nu); } \
  [[maybe_unused]] void Set ## nc ## AttribDisabled(void) const \
    { SetWindowAttribDisabled(GLFW_ ## nu); } \
  [[maybe_unused]] bool Is ## nc ## AttribEnabled(void) const \
    { return GLFWBooleanToBoolean(GetWindowAttrib(GLFW_ ## nu)); } \
  [[maybe_unused]] bool Is ## nc ## AttribDisabled(void) const \
    { return !Is ## nc ## AttribEnabled(); }
  /* ----------------------------------------------------------------------- */
  SET(Decorated, DECORATED);           // Window has a caption and border?
  SET(Resizable, RESIZABLE);           // Window is resizable?
  SET(Floating, FLOATING);             // Window is always on top?
  SET(AutoIconify, AUTO_ICONIFY);      // Window is auto-minimised?
  SET(FocusOnShow, FOCUS_ON_SHOW);     // Window is shown on focus?
  /* ----------------------------------------------------------------------- */
#undef SET                             // Done with this macro
  /* -- Set window hint core functions ------------------------------------- */
  void SetWindowHintBoolean(const int iVar, const bool bVal) const
    { SetWindowHint(iVar, BooleanToGLFWBoolean(bVal)); }
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
  [[maybe_unused]] bool Is ## nc ## Enabled(void) const \
    { return GetWindowAttribBoolean(GLFW_ ## nu); } \
  [[maybe_unused]] bool Is ## nc ## Disabled(void) const \
    { return !Is ## nc ## Enabled(); }
  /* ----------------------------------------------------------------------- */
  SET(Focus, FOCUSED);                 // Set window focused state
  SET(Iconify, ICONIFIED);             // Set window minimised state
  SET(Resizable, RESIZABLE);           // Set window resizable state
  SET(Visibility, VISIBLE);            // Set window visibility state
  SET(Decorated, DECORATED);           // Set window border state
  SET(AutoIconify, AUTO_ICONIFY);      // Set window auto-minimise state
  SET(Floating, FLOATING);             // Set window floating state
  SET(Maximised, MAXIMIZED);           // Set window maximised state
  SET(CentreCursor, CENTER_CURSOR);    // Set window cursor centre state
  SET(Transparency, TRANSPARENT_FRAMEBUFFER); // Set transparent framebuffer
  SET(Stereo, CENTER_CURSOR);          // Set window cursor centre state
  SET(MouseHovered, HOVERED);          // Set mouse hovered over state
  SET(FocusOnShow, FOCUS_ON_SHOW);     // Set focus on show window
  SET(SRGBCapable, SRGB_CAPABLE);      // Set SRGB colour space capable
  SET(DoubleBuffer, DOUBLEBUFFER);     // Set double buffering
  SET(NoErrors, CONTEXT_NO_ERROR);     // Set context no errors
  SET(Debug, OPENGL_DEBUG_CONTEXT);    // Set opengl debug mode
  SET(ForwardCompat, OPENGL_FORWARD_COMPAT); // Set opengl fwd compatibility
  /* ----------------------------------------------------------------------- */
#if defined(MACOS)                     // Using Apple compiler?
  /* ----------------------------------------------------------------------- */
  SET(MenuBar,      COCOA_MENUBAR);    // Set MacOS menu bar?
  SET(RetinaMode,   COCOA_RETINA_FRAMEBUFFER); // Set retina framebuffer?
  SET(GPUSwitching, COCOA_GRAPHICS_SWITCHING); // Set graphics switching?
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
    DestroyWindow();
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
    imTargets{{                        // Glfw targets as strings
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
    }, "GLFW_UNKNOWN" },               // Unknown GLFW target
    wClass{ nullptr },                 // Uninitialised window context
    uiErrorLevel(0)                    // No errors occured
    /* -- No code ---------------------------------------------------------- */
    { }
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(GlFW);               // Do not need defaults
  /* ----------------------------------------------------------------------- */
} *cGlFW = nullptr;                    // Pointer to static class
/* == Process a drag and drop event ======================================== */
void GlFW::OnDropSt(GLFWwindow*const wC, int iC, const char**const cpaFiles)
  { cGlFW->OnDrop(wC, static_cast<unsigned int>(iC), cpaFiles); }
/* -- Process a glfw error ------------------------------------------------- */
void GlFW::ErrorHandler(int iCode, const char*const cpDesc)
  { cGlFW->HandleError(iCode, cpDesc); }
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
