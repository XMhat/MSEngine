/* == DISPLAY.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module handles window creation and manipulation via the GLFW   ## */
/* ## library.                                                            ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfDisplay {                  // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfInput;               // Using input interface
/* -- Typedefs ------------------------------------------------------------- */
BUILD_FLAGS(Display,
  /* ----------------------------------------------------------------------- */
  // No display flags                  Full-screen mode set?
  DF_NONE                {0x00000000}, DF_FULLSCREEN          {0x00000001},
  // Window is closable?               Window is focused?
  DF_CLOSEABLE           {0x00000002}, DF_FOCUSED             {0x00000004},
  // Window has a border?              Minimize on lose focus?
  DF_BORDER              {0x00000008}, DF_MINFOCUS            {0x00000010},
  // Exclusive mode full-screen?       Display system restarting?
  DF_EXCLUSIVE           {0x00000020}, DF_RESTARTING          {0x00000040},
  // Window is resizable?              Always on top?
  DF_SIZABLE             {0x00000080}, DF_FLOATING            {0x00000100},
  // Automatic minimise?               Focus on show?
  DF_AUTOICONIFY         {0x00000200}, DF_AUTOFOCUS           {0x00000400},
  // Engine is in it's own thread?     HiDPI is enabled?
  DF_THREADED            {0x00000800}, DF_HIDPI               {0x00001000},
  // Window is actually in fullscreen?
  DF_INFULLSCREEN        {0x00002000}
);/* == Display class ====================================================== */
static class Display :
  /* -- Base classes ------------------------------------------------------- */
  private IHelper,                     // Initialisation helper
  public DisplayFlags,                 // Display settings
  private EvtMain::RegVec,             // Main events list to register
  private EvtWin::RegVec               // Window events list to register
{ /* -- Variables ------------------------------------------------- */ private:
  GLFWmonitor     *mSelected;          // Monitor selected
  int              iMRequested,        // Monitor id request
                   iMSelected;         // Monitor id selected
  const GLFWvidmode *vSelected;        // Video mode selected
  int              iVRequested,        // Video mode requested
                   iVSelected;         // Video mode selected
  GLfloat          fGamma,             // Monitor gamma setting
                   fOrthoWidth,        // Saved ortho width
                   fOrthoHeight;       // Saved ortho height
  int              iWinPosX, iWinPosY, // Window position
                   iWinWidthReq,       // Window width
                   iWinHeightReq;      // Window height
  float            fWinScaleWidth,     // Window scale width
                   fWinScaleHeight;    // Window scale height
  int              iAuxBuffers,        // Auxilliary buffers to use
                   iSamples;           // FSAA setting to use
  /* -- Icons -------------------------------------------------------------- */
  vector<GLFWimage> giImages;          // Contiguous memory for glfw image data
  vector<Image>     imgIcons;          // Image icon data
  /* -- Macros ----------------------------------------------------- */ public:
  LuaFunc          lrFocused;          // Window focused lua event
  /* -- Window moved request -------------------------------------- */ private:
  void OnMoved(const EvtMain::Cell &ewcArgs)
  { // Get new values
    const int iNewX = ewcArgs.vParams[1].i,
              iNewY = ewcArgs.vParams[2].i;
    // If position not changed?
    if(iWinPosX == iNewX && iWinPosY == iNewY)
    { // Report event
      LW(LH_DEBUG, "Display received window position of $x$.",
        iNewX, iNewY);
      // Done
      return;
    } // Report change
    LW(LH_INFO, "Display changed window position from $x$ to $x$.",
      iWinPosX, iWinPosY, iNewX, iNewY);
    // Update position
    iWinPosX = iNewX;
    iWinPosY = iNewY;
  }
  /* -- Window set icon request -------------------------------------------- */
  void OnSetIcon(const EvtWin::Cell&) { UpdateIcons(); }
  /* -- Window scale change request ---------------------------------------- */
  void OnScale(const EvtMain::Cell &ewcArgs)
  { // Get new values
    const float fNewWidth = ewcArgs.vParams[1].f,
                fNewHeight = ewcArgs.vParams[2].f;
    // If scale not changed?
    if(fNewWidth == fWinScaleWidth && fNewHeight == fWinScaleHeight)
    { // Report event
      LW(LH_DEBUG, "Display received window scale of $x$.",
        fNewWidth, fNewHeight);
      // Done
      return;
    } // Report change
    LW(LH_INFO, "Display changed window scale from $x$ to $x$.",
      fWinScaleWidth, fWinScaleHeight, fNewWidth, fNewHeight);
    // Set new value
    fWinScaleWidth = fNewWidth;
    fWinScaleHeight = fNewHeight;
  }
  /* -- Window limits change request --------------------------------------- */
  void OnLimits(const EvtMain::Cell &ewcArgs)
  { // Get the new limits
    const int iMinW = ewcArgs.vParams[0].i, iMinH = ewcArgs.vParams[1].i,
              iMaxW = ewcArgs.vParams[2].i, iMaxH = ewcArgs.vParams[3].i;
    // Set the new limits
    cGlFW->SetLimits(iMinW, iMinH, iMaxW, iMaxH);
  }
  /* -- Window focused ----------------------------------------------------- */
  void OnFocus(const EvtMain::Cell &ewcArgs)
  { // Get state and check it
    const int iState = ewcArgs.vParams[1].i;
    switch(iState)
    { // Focus restored?
      case GLFW_TRUE:
        // Return if we already recorded an focus event.
        if(FlagIsSet(DF_FOCUSED)) return;
        // Window is focused
        FlagSet(DF_FOCUSED);
        // Send message
        LW(LH_DEBUG, "Display window focus restored.");
        // Done
        break;
      // Focus lost?
      case GLFW_FALSE:
        // Return if we already recorded an unfocus event.
        if(FlagIsClear(DF_FOCUSED)) return;
        // Window is focused
        FlagClear(DF_FOCUSED);
        // Send message
        LW(LH_DEBUG, "Display window focus lost.");
        // Done
        break;
      // Unknown state?
      default:
        // Log the unknown state
        LW(LH_WARNING, "Display received unknown focus state $<0x$$>!",
          iState, hex, iState);
        // Done
        return;
    } // Dispatch event to lua
    lrFocused.LuaFuncDispatch(iState);
  }
  /* -- Window contents damaged and needs refreshing ----------------------- */
  void OnRefresh(const EvtMain::Cell&)
  { // Report that the window was resized
    LW(LH_DEBUG, "Display redrawing window contents.");
    // Set to force redraw the next frame
    cFboMain->SetDraw();
  }
  /* -- On window resized callback ----------------------------------------- */
  void OnResized(const EvtMain::Cell &ewcArgs)
  { // Get width and height
    const int iWidth = ewcArgs.vParams[1].i,
              iHeight = ewcArgs.vParams[2].i;
    // If position not changed?
    if(cInput->GetWindowWidth() == iWidth &&
       cInput->GetWindowHeight() == iHeight)
    { // Report event
      LW(LH_DEBUG, "Display received window size of $x$.", iWidth, iHeight);
      // Done
      return;
    } // Report change
    LW(LH_INFO, "Display changed window size from $x$ to $x$.",
      cInput->GetWindowWidth(), cInput->GetWindowHeight(), iWidth, iHeight);
    // Update position
    cInput->SetWindowSize(iWidth, iHeight);
  }
  /* -- On window closed callback ------------------------------------------ */
  void OnClose(const EvtMain::Cell&)
  { // If window is not closable then ignore the event
    if(FlagIsClear(DF_CLOSEABLE)) return;
    // Send quit event
    cEvtMain->Add(EMC_QUIT);
  }
  /* -- Window iconified --------------------------------------------------- */
  void OnIconify(const EvtMain::Cell &ewcArgs)
  { // Get state and check it
    switch(const int iState = ewcArgs.vParams[1].i)
    { // Minimized? Log that we minimised and return
      case GLFW_TRUE:
        LW(LH_DEBUG, "Display window state minimised.");
        return;
      // Restored? Redraw console at least and log event
      case GLFW_FALSE:
        cFboMain->SetDraw();
        LW(LH_DEBUG, "Display window state restored.");
        break;
      // Unknown state so log it
      default:
        LW(LH_WARNING, "Display received unknown iconify state $.", iState);
        break;
    }
  }
  /* -- Enumerate monitors ------------------------------------------------- */
  void EnumerateMonitorsAndVideoModes(void)
  { // Log progress
    LW(LH_DEBUG, "Display now enumerating available displays...");
    // Get primary monitor information, throw exception if invalid
    GLFWmonitor*const mPrimary = glfwGetPrimaryMonitor();
    if(!mPrimary)
      XC("Could not detect primary monitor!");
    // Get primary video mode information
    const GLFWvidmode*const vPrimary = glfwGetVideoMode(mPrimary);
    if(!vPrimary)
      XC("Could not detect primary video mode!",
         "Monitor", glfwGetMonitorName(mPrimary));
    // Primary video mode, monitor and monitor count
    int iVPrimary = -1, iMPrimary = -1, iMonitors;
    // Get monitors count and if valid?
    GLFWmonitor*const*const mData = cGlFW->GetMonitors(iMonitors);
    if(mData && iMonitors > 0)
    { // Display information about each new monitor
      for(int iMonitor = 0; iMonitor < iMonitors; ++iMonitor)
      { // Get monitor, position, physical size and video modes
        GLFWmonitor*const mItem = mData[iMonitor];
        if(mItem == mPrimary) iMPrimary = iMonitor;
        int iPosX, iPosY;
        glfwGetMonitorPos(mItem, &iPosX, &iPosY);
        int iWidth, iHeight;
        glfwGetMonitorPhysicalSize(mItem, &iWidth, &iHeight);
        int iModes;
        const GLFWvidmode*const vModes = glfwGetVideoModes(mItem, &iModes);
        // Write information about the monitor
        LW(LH_DEBUG, "- Monitor $: $.\n"
                     "- Mode count: $.\n"
                     "- Position: $x$.\n"
                     "- Dimensions: $$$x$\" ($x$mm).\n"
                     "- Size: $\" ($mm).",
          iMonitor, glfwGetMonitorName(mItem), iModes, iPosX, iPosY, fixed,
          setprecision(1), MillimetresToInches(iWidth),
          MillimetresToInches(iHeight), iWidth, iHeight,
          MillimetresToInches(GetDiagLength(iWidth, iHeight)),
          GetDiagLength(iWidth, iHeight));
        // Get video modes for this monitor and ignore if invalid
        if(!vModes || iModes <= 0) continue;
        // Enumerate video modes
        for(int iMode = 0; iMode < iModes; ++iMode)
        { // Get mode and set primary mode id if found
          const GLFWvidmode &vMode = vModes[iMode];
          if(mItem == mPrimary && !memcmp(&vMode, vPrimary, sizeof(vMode)))
            iVPrimary = iMode;
          // Report mode
          LW(LH_DEBUG, "-- Mode $: $x$x$bpp @$hz (R:$;G:$;B:$).",
            iMode, vMode.width, vMode.height,
            vMode.redBits+vMode.greenBits+vMode.blueBits, vMode.refreshRate,
            vMode.redBits, vMode.greenBits, vMode.blueBits);
        }
      } // Check that we detected the primary resolution and monitor
      if(iMPrimary == -1) XC("Could not detect primary monitor id!");
      if(iVPrimary == -1) XC("Could not detect primary resolution id!");
      // Custom monitor selected and valid monitor? Set it
      if(iMRequested >= 0 && iMRequested < iMonitors)
        { mSelected = mData[iMRequested]; iMSelected = iMRequested; }
      // Select primary resolution
      else { mSelected = mPrimary; iMSelected = iMPrimary; }
    } // Select primary monitor
    else { mSelected = mPrimary; iMSelected = -1; }
    // Get number of video modes on selected monitor
    int iModes;
    const GLFWvidmode *vModes = glfwGetVideoModes(mSelected, &iModes);
    if(vModes && iVRequested >= 0 && iVRequested < iModes)
      { vSelected = &vModes[iVRequested]; iVSelected = iVRequested; }
    else { vSelected = vPrimary; iVSelected = iVPrimary; }
    // Get information about the primary monitor
    int iPrimaryPosX, iPrimaryPosY;
    glfwGetMonitorPos(mPrimary, &iPrimaryPosX, &iPrimaryPosY);
    int iPrimaryWidth, iPrimaryHeight;
    glfwGetMonitorPhysicalSize(mPrimary, &iPrimaryWidth, &iPrimaryHeight);
    // Get information about the selected monitor
    int iSelectedPosX, iSelectedPosY;
    glfwGetMonitorPos(mSelected, &iSelectedPosX, &iSelectedPosY);
    int iSelectedWidth, iSelectedHeight;
    glfwGetMonitorPhysicalSize(mSelected, &iSelectedWidth, &iSelectedHeight);
    // Write to log the monitor we are using
    LW(LH_INFO, "Display finished enumerating $ displays...\n"
                "- Primary monitor $: $ @$x$ ($$$\"x$\"=$\").\n"
                "- Primary mode $: $x$x$bpp(R$G$B$) @$hz.\n"
                "- Selected monitor $: $ @$x$ ($$$\"x$\"=$\").\n"
                "- Selected mode $: $x$x$bpp(R$G$B$) @$hz.",
      iMonitors,
      iMPrimary, glfwGetMonitorName(mPrimary), iPrimaryPosX, iPrimaryPosY,
        fixed, setprecision(1), MillimetresToInches(iPrimaryWidth),
        MillimetresToInches(iPrimaryHeight),
        MillimetresToInches(GetDiagLength(iPrimaryWidth, iPrimaryHeight)),
      iVPrimary, vPrimary->width, vPrimary->height,
        vPrimary->redBits+vPrimary->greenBits+vPrimary->blueBits,
        vPrimary->redBits, vPrimary->greenBits, vPrimary->blueBits,
        vPrimary->refreshRate,
      iMSelected, glfwGetMonitorName(mSelected), iSelectedPosX, iSelectedPosY,
        fixed, setprecision(1), MillimetresToInches(iSelectedWidth),
        MillimetresToInches(iSelectedHeight),
        MillimetresToInches(GetDiagLength(iSelectedWidth, iSelectedHeight)),
      iVSelected, vSelected->width, vSelected->height,
        vSelected->redBits+vSelected->greenBits+vSelected->blueBits,
        vSelected->redBits, vSelected->greenBits, vSelected->blueBits,
        vSelected->refreshRate);
    // Set exclusive full-screen mode flag meaning that the cvars say that
    // the guest or end user wants a full-screen mode set.
    FlagSetOrClear(DF_EXCLUSIVE, iVRequested >= -1);
  }
  /* -- Monitor changed ---------------------------------------------------- */
  static void OnMonitorStatic(GLFWmonitor*const, const int);
  void OnMonitor(GLFWmonitor*const mA, const int iA)
  {  // Get monitor name
    const char*const cpName =
      mA ? glfwGetMonitorName(mA) : "<unknown monitor>";
    // Compare state
    switch(iA)
    { // Device was connected?
      case GLFW_CONNECTED:
        // Log the event and return
        LW(LH_INFO, "Display detected monitor '$'.", cpName); return;
      // Device was disconnected?
      case GLFW_DISCONNECTED:
        // If this is not our active montiro
        if(mA != mSelected)
        { // Log that the monitor was disconnected and return
          LW(LH_INFO, "Display disconnected monitor '$'.", cpName);
          return;
        } // Log disconnection
        LW(LH_INFO, "Display disconnected monitor '$', re-initialising...",
          cpName);
        // Enumerate monitors again as current one is invalid
        EnumerateMonitorsAndVideoModes();
        // We need to re-initialise the opengl context
        cEvtMain->Add(EMC_QUIT_THREAD);
        // Done
        return;
      // Unknown state so log the bad state and return
      default: LW(LH_WARNING,
                 "Display received bad state of $$ for monitor '$'!",
                   hex, iA, cpName); return;
    } // Should not get here
  }
  /* == Call to reset the fbo ============================================== */
  void DoResizeFbo(const GLsizei stWidth, const GLsizei stHeight)
  { // Log new viewport
    LW(LH_DEBUG, "Display received new framebuffer size of $x$.",
      stWidth, stHeight);
    // Resize main viewport and if it changed, re-initialise the console fbo
    // and redraw the console
    if(cFboMain->AutoViewport(stWidth, stHeight)) cConsole->InitFBO();
    // Else redraw the console if enabled
    else cConsole->SetRedrawIfEnabled();
  }
  /* == Matrix reset requested ============================================= */
  void OnMatrixReset(const EvtMain::Cell&) { ReInitMatrix(); }
  /* == Framebuffer reset requested ======================================== */
  void OnFBReset(const EvtMain::Cell &ewcArgs)
  { // Get new frame buffer size
    const GLsizei stWidth = ewcArgs.vParams[1].i,
                  stHeight = ewcArgs.vParams[2].i;
    // Log new viewport
    LW(LH_DEBUG, "Display received new framebuffer size of $x$.",
      stWidth, stHeight);
    // Resize main viewport and if it changed, re-initialise the console fbo
    // and redraw the console
    if(cFboMain->AutoViewport(stWidth, stHeight)) cConsole->InitFBO();
    // Else redraw the console if enabled
    else cConsole->SetRedrawIfEnabled();
  }
  /* == Window size requested ============================================== */
  void OnResize(const EvtWin::Cell &ewcArgs)
  { // Set requested window size
    cGlFW->SetWindowSize(ewcArgs.vParams[0].i, ewcArgs.vParams[1].i);
    // Done
    return;
  }
  /* == Window move requested ============================================== */
  void OnMove(const EvtWin::Cell &ewcArgs)
  { // Set requested window size
    cGlFW->MoveWindow(ewcArgs.vParams[0].i, ewcArgs.vParams[1].i);
    // Done
    return;
  }
  /* -- Window centre request ---------------------------------------------- */
  void OnCentre(const EvtWin::Cell&)
  { // If in full screen mode, don't resize or move anything
    if(FlagIsSet(DF_INFULLSCREEN)) return;
    // Get centre co-ordinates
    int iX, iY; GetCentreCoords(iX, iY,
      cInput->GetWindowWidth(), cInput->GetWindowHeight());
    // Move the window
    cGlFW->MoveWindow(iX, iY);
  }
  /* == Window reset requested ============================================= */
  void OnReset(const EvtWin::Cell&)
  { // If in full screen mode, don't resize or move anything
    if(FlagIsSet(DF_INFULLSCREEN)) return;
    // Restore window state
    cGlFW->RestoreWindow();
    // Translate user specified window size and set the size of hte window
    int iX, iY; TranslateUserSize(iX, iY);
    cGlFW->SetWindowSize(iX, iY);
    // Get new window size (Input won't have received it yet).
    int iW, iH; cGlFW->GetWindowSize(iW, iH);
    TranslateUserCoords(iX, iY, iW, iH);
    // Update window position
    cGlFW->MoveWindow(iX, iY);
  }
  /* -- Set new full screen setting ---------------------------------------- */
  void SetFullScreenSetting(const bool bState) const
    { cCVars->SetInternalSafe<bool>(VID_FS, bState); }
  /* -- Toggle full-screen event (Engine thread) --------------------------- */
  void OnToggleFS(const EvtWin::Cell &ewcArgs)
  { // Ignore further requests if already restarting
    if(FlagIsSet(DF_RESTARTING)) return;
    // If an argument was not specified?
    if(ewcArgs.vParams.empty())
    { // Set full screen depending on current state
      SetFullScreenSetting(FlagIsClear(DF_FULLSCREEN));
      // We can do quick re-init
      SetFullScreen(FlagIsSet(DF_FULLSCREEN));
    } // Use requested setting instead
    else SetFullScreen(ewcArgs.vParams[0].b);
  }
  /* -- Apply gamma setting ------------------------------------------------ */
  void ApplyGamma(void)
  { // Set gamma
    GlFW::SetGamma(mSelected, fGamma);
    // Report
    LW(LH_DEBUG, "Display set gamma to $$.", fixed, fGamma);
  }
  /* -- ReInit desktop mode window ----------------------------------------- */
  void ReInitDesktopModeWindow(void)
  { // Not in full-screen mode
    FlagClear(DF_INFULLSCREEN);
    // Trnslate user specified window size
    int iW, iH; TranslateUserSize(iW, iH);
    int iX, iY; TranslateUserCoords(iX, iY, iW, iH);
    // Initialise the desktop window and send the handle to system class
    // because they need the handle for exceptions, icons and other things.
    cGlFW->SetWindowMonitor(nullptr, iX, iY, iW, iH, 0);
    // Window mode so update users window border setting
    cGlFW->SetWindowAttribBoolean(GLFW_DECORATED, FlagIsSet(DF_BORDER));
    // Log that we switched to window mode
    LW(LH_INFO, "Display switched to desktop window $x$ at $x$.",
      iW, iH, iX, iY);
  }
  /* -- ReInit full-screen mode window ------------------------------------- */
  void ReInitFullScreenModeWindow(void)
  { // Actually in full screen mode window
    FlagSet(DF_INFULLSCREEN);
    // Get selected desktop resolution as reference
    const GLFWvidmode &vmData = *vSelected;
    // Chosen settings
    GLFWmonitor *mUsing;
    const char *cpType;
    // Exclusive full-screen window requested? Set video mode and label
    if(FlagIsSet(DF_EXCLUSIVE)) { mUsing = mSelected; cpType = "exclusive"; }
    // Not exclusive full-screen mode?
    else
    { // Not using exclusive full-screen mode
      mUsing = nullptr;
      cpType = "borderless";
      // Force disable window border
      cGlFW->SetWindowAttribDisable(GLFW_DECORATED);
    } // Set the full-screen window
    cGlFW->SetWindowMonitor(mUsing, 0, 0,
      vmData.width, vmData.height, vmData.refreshRate);
    // Log that we switched to full-screen mode
    LW(LH_INFO, "Display switch to $ full-screen $x$ (M:$>$;V:$>$;R:$).",
      cpType, vmData.width, vmData.height, iMRequested, iMSelected,
      iVRequested, iVSelected, vmData.refreshRate);
  }
  /* -- Translate user specified window dimensions ------------------------- */
  void TranslateUserSize(int &iW, int &iH) const
  { // Get window size specified by user
    iW = iWinWidthReq;
    iH = iWinHeightReq;
    // If size is not valid?
    if(iW > 0 && iH > 0)
    { // Report result and return
      LW(LH_DEBUG, "Display using user specified dimensions of $x$.", iW, iH);
      return;
    } // We need a selected video resolution
    if(!vSelected)
    { // Set defaults
      iW = 640;
      iH = 480;
      // Put message in log
      LW(LH_WARNING, "Display class cannot automatically detect the best "
        "dimensions for the window because the current desktop resolution "
        "was not detected properly! Using fallback default of $x$.", iW, iH);
      return;
    } // Get selected resolution as reference
    const GLFWvidmode &vmData = *vSelected;
    // Convert selected height to double as we need to use it twice
    const double fdHeight = static_cast<double>(vmData.height);
    // Set the height to 80% of desktop height
    iH = static_cast<int>(ceil(fdHeight * 0.8));
    // Now set the width based on desktop aspect ratio
    iW = static_cast<int>(ceil(static_cast<double>(iH) *
      (static_cast<double>(vmData.width) / fdHeight)));
    // Report result
    LW(LH_DEBUG, "Display translated user size to $x$.", iW, iH);
  }
  /* -- Get centre co-ordinates -------------------------------------------- */
  void GetCentreCoords(int &iX, int &iY, const int iW, const int iH) const
  { // Bail if not in full screen
    if(FlagIsSet(DF_INFULLSCREEN)) { iX = iY = 0; return; }
    // Must have current desktop information
    if(!mSelected)
    { // Clear co-ordinates
      iX = iY = 0;
      // Put message in  log
      LW(LH_WARNING,
        "Display class cannot centre the window without monitor data.");
      return;
    } // Get desktop mode
    const GLFWvidmode*const vmData = glfwGetVideoMode(mSelected);
    // Get new co-ordinates
    iX = (vmData->width / 2) - (iW / 2);
    iY = (vmData->height / 2) - (iH / 2);
  }
  /* -- Translate co-ordinates --------------------------------------------- */
  void TranslateUserCoords(int &iX, int &iY, const int iW, const int iH) const
  { // Put user values into window co-ordinates
    iX = cCVars->GetInternalSafe<int>(WIN_POSX);
    iY = cCVars->GetInternalSafe<int>(WIN_POSY);
    // Centre window?
    if(iX==-2 || iY==-2) GetCentreCoords(iX, iY, iW, iH);
  }
  /* -- Re-initialise window ----------------------------------------------- */
  void ReInitWindow(const bool bState)
  { // Full-screen selected?
    if(bState) ReInitFullScreenModeWindow();
    // Window mode selected
    else ReInitDesktopModeWindow();
    // Update window attributes
    cGlFW->SetWindowAttribBoolean(GLFW_RESIZABLE, FlagIsSet(DF_SIZABLE));
    cGlFW->SetWindowAttribBoolean(GLFW_FLOATING, FlagIsSet(DF_FLOATING));
    cGlFW->SetWindowAttribBoolean(GLFW_AUTO_ICONIFY,
      FlagIsSet(DF_AUTOICONIFY));
    cGlFW->SetWindowAttribBoolean(GLFW_FOCUS_ON_SHOW, FlagIsSet(DF_AUTOFOCUS));
    // Store current window position
    cGlFW->GetWindowPos(iWinPosX, iWinPosY);
    cInput->UpdateWindowSize();
    cGlFW->GetWindowScale(fWinScaleWidth, fWinScaleHeight);
    // Need to fix a GLFW scaling bug with this :(
#ifdef __APPLE__
    // Return if hidpi not enabled
    if(FlagIsClear(DF_HIDPI))
    { // Update the main fbo viewport size without scale
      cFboMain->SetViewport(0, 0,
        static_cast<GLsizei>(cInput->GetWindowWidth()),
        static_cast<GLsizei>(cInput->GetWindowHeight()));
      // Done
      return;
    } // Get window scale
    // Update the main fbo viewport size with scale
    cFboMain->SetViewport(0, 0,
      static_cast<GLsizei>(cInput->GetWindowWidth()) *
        static_cast<GLsizei>(fWinScaleWidth),
      static_cast<GLsizei>(cInput->GetWindowHeight()) *
        static_cast<GLsizei>(fWinScaleHeight));
    // Windows and linux doesn't need the scale
#else
    // Update the main fbo viewport size without scale
    cFboMain->SetViewport(0, 0,
      static_cast<GLsizei>(cInput->GetWindowWidth()),
      static_cast<GLsizei>(cInput->GetWindowHeight()));
#endif
    // Show and focus the window
    cGlFW->ShowWindow();
    cGlFW->FocusWindow();
    // Update cursor visibility as OS or glfw can mess it up
    cInput->CommitCursor();
    // Focused and no longer restarting
    FlagSetAndClear(DF_FOCUSED, DF_RESTARTING);
  }
  /* -- Request from alternative thread to resize window ----------- */ public:
  void RequestResize(const int iW, const int iH)
    { cEvtWin->AddUnblock(EWC_WIN_RESIZE, iW, iH); }
  /* -- Request from alternative thread to move window --------------------- */
  void RequestMove(const int iX, const int iY)
    { cEvtWin->AddUnblock(EWC_WIN_MOVE, iX, iY); }
  /* -- Request from alternative thread to centre the window --------------- */
  void RequestCentre(void) { cEvtWin->AddUnblock(EWC_WIN_CENTRE); }
  /* -- Request from alternative thread to fullscreen toggle without save -- */
  void RequestFSToggle(const bool bState)
    { cEvtWin->AddUnblock(EWC_WIN_TOGGLE_FS, bState); }
  /* -- Request from alternative thread to reposition the window ----------- */
  void RequestReposition(void) { cEvtWin->AddUnblock(EWC_WIN_RESET); }
  /* -- Set full screen ---------------------------------------------------- */
  void SetFullScreen(const bool bState)
  { // Return if setting not different than actual
    if(FlagIsEqualToBool(DF_INFULLSCREEN, bState)) return;
    // Update new fullscreen setting and re-initialise if successful
    ReInitWindow(bState);
    // Update viewport
    cEvtMain->Add(EMC_VID_MATRIX_REINIT);
  }
  /* -- Return current video mode refresh rate ----------------------------- */
  int GetRefreshRate(void) { return vSelected->refreshRate; }
  /* -- Set default orthagonal width  -------------------------------------- */
  CVarReturn SetOrthoWidth(const GLfloat fWidth)
    { return CVarSimpleSetIntNLG(fOrthoWidth, fWidth, 320.0f, 16384.0f); }
  /* -- Set default orthagonal height -------------------------------------- */
  CVarReturn SetOrthoHeight(const GLfloat fHeight)
    { return CVarSimpleSetIntNLG(fOrthoHeight, fHeight, 200.0f, 16384.0f); }
  /* -- Set fsaa value ----------------------------------------------------- */
  CVarReturn FsaaChanged(const int iCount)
    { return CVarSimpleSetIntNLG(iSamples, iCount, GLFW_DONT_CARE, 8); }
  /* -- Set aux buffers count ---------------------------------------------- */
  CVarReturn AuxBuffersChanged(const int iCount)
    { return CVarSimpleSetIntNLG(iAuxBuffers, iCount, GLFW_DONT_CARE, 16); }
  /* -- Set window width --------------------------------------------------- */
  CVarReturn WidthChanged(const int iWidth)
    { return CVarSimpleSetInt(iWinWidthReq, iWidth); }
  /* -- Set window height -------------------------------------------------- */
  CVarReturn HeightChanged(const int iHeight)
    { return CVarSimpleSetInt(iWinHeightReq, iHeight); }
  /* -- Set full-screen cvar ----------------------------------------------- */
  CVarReturn BorderChanged(const bool bState)
    { FlagSetOrClear(DF_BORDER, bState); return ACCEPT; }
  /* -- Set hidpi cvar ----------------------------------------------------- */
  CVarReturn HiDPIChanged(const bool bState)
    { FlagSetOrClear(DF_HIDPI, bState); return ACCEPT; }
  /* -- Set window resizable ----------------------------------------------- */
  CVarReturn SizableChanged(const bool bState)
    { FlagSetOrClear(DF_SIZABLE, bState); return ACCEPT; }
  /* -- Set full-screen cvar ----------------------------------------------- */
  CVarReturn FullScreenStateChanged(const bool bState)
    { FlagSetOrClear(DF_FULLSCREEN, bState); return ACCEPT; }
  /* -- Closeable state modified ------------------------------------------- */
  CVarReturn CloseableChanged(const bool bState)
    { FlagSetOrClear(DF_CLOSEABLE, bState); return ACCEPT; }
  /* -- Autominimise state modified ---------------------------------------- */
  CVarReturn MinFocusChanged(const bool bState)
    { FlagSetOrClear(DF_MINFOCUS, bState); return ACCEPT; }
  /* -- Autominimise state modified ---------------------------------------- */
  CVarReturn FloatingChanged(const bool bState)
    { FlagSetOrClear(DF_FLOATING, bState); return ACCEPT; }
  /* -- Auto iconify modified------ ---------------------------------------- */
  CVarReturn AutoIconifyChanged(const bool bState)
    { FlagSetOrClear(DF_AUTOICONIFY, bState); return ACCEPT; }
  /* -- Auto iconify modified------ ---------------------------------------- */
  CVarReturn AutoFocusChanged(const bool bState)
    { FlagSetOrClear(DF_AUTOFOCUS, bState); return ACCEPT; }
  /* -- Set full-screen video mode cvar ------------------------------------ */
  CVarReturn FullScreenModeChanged(const int iVId)
    { return CVarSimpleSetIntNL(iVRequested, iVId, -2); }
  /* -- Set monitor number ------------------------------------------------- */
  CVarReturn MonitorChanged(const int iMId)
    { return CVarSimpleSetIntNL(iMRequested, iMId, -1); }
  /* -- Set gamma ---------------------------------------------------------- */
  CVarReturn GammaChanged(const GLfloat fNewGamma)
  { // Bail if invalid gamma value
    if(fNewGamma < 0.25f || fNewGamma > 4.00f) return DENY;
    // Record new gamma
    fGamma = fNewGamma;
    // Apply new gamma setting if window is available
    if(cGlFW && cGlFW->IsWindowAvailable() && mSelected) ApplyGamma();
    // Success
    return ACCEPT;
  }
  /* -- Icon filenames changed (allow blank strings) ------ Core::SetIcon -- */
  CVarReturn SetIcon(const string &strF, string&)
    { return BoolToCVarReturn(strF.empty() || SetIcon(strF)); }
  /* -- Get name of monitor by id ------------------------------------------ */
  const char *GetMonitorNameById(const int iMId) const
  { // Num monitors and num modes
    int iMonitors;
    // Get monitors list
    GLFWmonitor*const*const mData = cGlFW->GetMonitors(iMonitors);
    // Bail if monitor id invalid
    if(iMId < 0 || iMId >= iMonitors)
      XC("Invalid monitor id number", "iMId", iMId, "iMonitors", iMonitors);
    // Return monitor name
    return glfwGetMonitorName(mData[iMId]);
  }
  /* -- Get selected monitor id -------------------------------------------- */
  int GetMonitorId(void) const { return iMSelected; }
  /* -- Get selected video mode id ----------------------------------------- */
  int GetVideoModeId(void) const { return iVSelected; }
  /* -- Get video mode data ------------------------------------------------ */
  const GLFWvidmode &GetVideoMode(const int iMId, const int iVId) const
  { // Num monitors and num modes
    int iMonitors, iModes;
     // Get monitors list
    GLFWmonitor*const*const mData = cGlFW->GetMonitors(iMonitors);
    // Bail if monitor id invalid
    if(iMId < 0 || iMId >= iMonitors)
      XC("Invalid monitor id number!",
         "iMId", iMId, "iMonitors", iMonitors);
    // Get number of video modes for
    const GLFWvidmode*const vData = glfwGetVideoModes(mData[iMId], &iModes);
    // Bail if monitor id invalid
    if(iVId < 0 || iVId >= iModes)
      XC("Invalid video mode id number!",
         "iVId", iVId, "iModes", iModes, "iMId", iMId, "iMonitors", iMonitors);
    // Return video mode info
    return vData[iVId];
  }
  /* -- Get video mode count ----------------------------------------------- */
  int GetVideoModes(const int iId) const
  { // Num monitors and num modes
    int iMonitors, iModes;
     // Get monitors list
    GLFWmonitor*const*const mData = cGlFW->GetMonitors(iMonitors);
    // Bail if monitor id invalid
    if(iId < 0 || iId >= iMonitors)
      XC("Invalid monitor id number",
         "iId", iId, "iMonitors", iMonitors);
    // Get number of video modes for
    glfwGetVideoModes(mData[iId], &iModes);
    // Return number of video modes
    return iModes;
  }
  /* -- Init info ---------------------------------------------------------- */
  const char *GetMonitorName(void) const
    { return glfwGetMonitorName(mSelected); }
  /* -- Set default matrix ------------------------------------------------- */
  void SetDefaultMatrix(const bool bForce) const
  { // Set the default matrix from the configuration and if it was changed
    // also update the consoles FBO too.
    if(cFboMain->AutoMatrix(fOrthoWidth, fOrthoHeight, bForce))
      cConsole->InitFBO();
    // Else redraw the console if enabled
    else cConsole->SetRedrawIfEnabled();
  }
  /* -- ReInit matrix ------------------------------------------------------ */
  void ForceReInitMatrix(void) { SetDefaultMatrix(true); }
  /* -- ReInit matrix ------------------------------------------------------ */
  void ReInitMatrix(void)
  { // Force-reinitialise matrix
    ForceReInitMatrix();
    // Inform lua scripts that they should redraw the framebuffer
    cEvtMain->Add(EMC_LUA_REDRAW);
  }
  /* -- Update window icon ------------------------------------------------- */
  void UpdateIcons(void)
  { // This functionality throws a GLFW api error on MacOS so just NullOp it
#ifndef __APPLE__
    // If using interactive mode?
    if(cSystem->IsGuiMode(GM_GRAPHICS))
    { // Ignore if no icons
      if(giImages.empty()) return;
      // Capture exceptions and ask GLFW to set the icon
      try { cGlFW->SetIcon(IntOrMax<int>(giImages.size()), giImages.data()); }
      // Exception occured? GLFW can throw GLFW_PLATFORM_ERROR on Wayland which
      // is absolutely retarded as is not consistent with other platforms such
      // as MacOS which will silently succeed
      catch(const exception &e)
      { // Just log the error that occured
        LW(LH_WARNING,
          "Display could not load $ icon files due to GlFW exception: $.",
            giImages.size(), e.what());
        // Done
        return;
      } // Report that we updated the icons
      LW(LH_INFO, "Display updated $ windows icons." , giImages.size());
      // Show details?
      if(cLog->HasLevel(LH_DEBUG))
        for(Image &imC : imgIcons)
        { // Get first icon
          ImageSlot &imsD = imC.GetFirstSlot();
          // Write data
          LWN(LH_DEBUG, "- $x$x$: $.",
            imsD.uiWidth, imsD.uiHeight, imC.GetBitsPP(), imC.IdentGet());
        }
    } // Using console mode
    else cSystem->UpdateIcons();
#endif
  }
  /* -- Set window icons --------------------------------------------------- */
  bool SetIcon(const string &strNames)
  { // Seperate icon names and return failure if we have more than three
    Token tIcons{ strNames, ":" };
    if(tIcons.empty() || tIcons.size() > 3) return false;
    // If using interactive mode?
    if(cSystem->IsGuiMode(GM_GRAPHICS))
    { // Clear images and icons
      giImages.clear();
      imgIcons.clear();
      // Create contiguous memory for glfw icon descriptors and icon data
      giImages.reserve(tIcons.size());
      imgIcons.reserve(tIcons.size());
      // Build icons
      for(string &strName : tIcons)
      { // Check filename
        DirVerifyFileNameIsValid(strName);
        // Load icon as RGB 32BPP.
        Image &imC = imgIcons.emplace_back(
          Image{ move(strName), IL_REVERSE|IL_TORGB|IL_TO32BPP });
        ImageSlot &imsD = imC.GetFirstSlot();
        giImages.emplace_back(GLFWimage{
          static_cast<int>(imsD.uiWidth),
          static_cast<int>(imsD.uiHeight),
          imsD.memData.Ptr<unsigned char>() });
      }
    } // Not in interactive mode?
    else
    { // Only Win32 terminal windows can change the icon afaik
#ifdef _WIN32
      // Have two icons at least?
      if(tIcons.size() >= 2)
      { // Set small icon from the last icon specified
        Image imC{ move(*prev(tIcons.cend())), IL_REVERSE|IL_TOBGR };
        ImageSlot &imsD = imC.GetFirstSlot();
        cSystem->SetSmallIcon(imC.IdentGet(), imsD.uiWidth, imsD.uiHeight,
          imC.GetBitsPP(), imsD.memData);
      } // Set large icon from the first icon specified
      Image imC{ move(*next(tIcons.cbegin())), IL_REVERSE|IL_TOBGR };
      ImageSlot &imsD = imC.GetFirstSlot();
      cSystem->SetLargeIcon(imC.IdentGet(), imsD.uiWidth, imsD.uiHeight,
        imC.GetBitsPP(), imsD.memData);
#endif
    } // Success
    return true;
  }
  /* -- Update icons and refresh icon if succeeded ------------------------- */
  void SetIconFromLua(const string &strNames)
    { if(SetIcon(strNames)) return cEvtWin->Add(EWC_WIN_SETICON); }
  /* -- Get window position ------------------------------------------------ */
  int GetWindowPosX(void) const { return iWinPosX; }
  int GetWindowPosY(void) const { return iWinPosY; }
  float GetWindowScaleWidth(void) const { return fWinScaleWidth; }
  float GetWindowScaleHeight(void) const { return fWinScaleHeight; }
  /* -- Init --------------------------------------------------------------- */
  void Init(void)
  { // Class initialised
    IHInitialise();
    // Log progress
    LW(LH_DEBUG, "Display class starting up...");
    // Enumerate monitors and video modes
    EnumerateMonitorsAndVideoModes();
    // GLFW_CLIENT_API indicates the client API provided by the window's c
    // ontext; either GLFW_OPENGL_API, GLFW_OPENGL_ES_API or GLFW_NO_API.
    cGlFW->SetWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    // ************** WE WANT A STANDARD OPENGL 3.2 CORE CONTEXT **************
    // GLFW_CONTEXT_VERSION_MAJOR and GLFW_CONTEXT_VERSION_MINOR specify the
    // client API version that the created context must be compatible with.
    // The exact behavior of these hints depend on the requested client API.
    cGlFW->SetWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    cGlFW->SetWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    // GLFW_OPENGL_PROFILE indicates the OpenGL profile used by the context.
    // This is GLFW_OPENGL_CORE_PROFILE or GLFW_OPENGL_COMPAT_PROFILE if the
    // context uses a known profile, or GLFW_OPENGL_ANY_PROFILE if the OpenGL
    // profile is unknown or the context is an OpenGL ES context. Note that
    // the returned profile may not match the profile bits of the context
    // flags, as GLFW will try other means of detecting the profile when no
    // bits are set.
    cGlFW->SetWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // Specifies whether the OpenGL context should be forward-compatible, i.e.
    // one where all functionality deprecated in the requested version of
    // OpenGL is removed. This must only be used if the requested OpenGL
    // version is 3.0 or above. If OpenGL ES is requested, this hint is
    // ignored. https://www.opengl.org/registry/
    cGlFW->SetWindowHintEnable(GLFW_OPENGL_FORWARD_COMPAT);
    // Specifies whether to create a debug OpenGL context, which may have
    // additional error and performance issue reporting functionality. If
    // OpenGL ES is requested, this hint is ignored.
    cGlFW->SetWindowHintBoolean(GLFW_OPENGL_DEBUG_CONTEXT,
      cCVars->GetInternalSafe<bool>(VID_DEBUG));
    // Specifies whether errors should be generated by the context. Possible
    // values are GLFW_TRUE and GLFW_FALSE. If enabled, situations that would
    // have generated errors instead cause undefined behavior.
    cGlFW->SetWindowHintBoolean(GLFW_CONTEXT_NO_ERROR,
      cCVars->GetInternalSafe<bool>(VID_NOERRORS));
    // Specifies whether the framebuffer should be double buffered. You nearly
    // always want to use double buffering. This is a hard constraint.
    cGlFW->SetWindowHintEnable(GLFW_DOUBLEBUFFER);
    // No depth or stencil buffer
    cGlFW->SetWindowHint(GLFW_DEPTH_BITS, 0);
    cGlFW->SetWindowHint(GLFW_STENCIL_BITS, 0);
    // Force custom bit-depth?
    if(const int iDepth = cCVars->GetInternalSafe<int>(VID_BPP))
    { // Force depth buffer bits
      cGlFW->SetWindowHint(GLFW_RED_BITS, iDepth);
      cGlFW->SetWindowHint(GLFW_GREEN_BITS, iDepth);
      cGlFW->SetWindowHint(GLFW_BLUE_BITS, iDepth);
      cGlFW->SetWindowHint(GLFW_ALPHA_BITS,
        cFboMain->fboMain.IsTransparencyEnabled() ? iDepth : 0);
    } // Use default
    else
    { // Set regular depth buffer bits
      cGlFW->SetWindowHint(GLFW_RED_BITS, vSelected->redBits);
      cGlFW->SetWindowHint(GLFW_GREEN_BITS, vSelected->greenBits);
      cGlFW->SetWindowHint(GLFW_BLUE_BITS, vSelected->blueBits);
      cGlFW->SetWindowHint(GLFW_ALPHA_BITS,
        cFboMain->fboMain.IsTransparencyEnabled() ? 8 : 0);
    } // Set transparency of main FBO
    cFboMain->fboMain.SetTransparency
      (cCVars->GetInternalSafe<bool>(VID_ALPHA));
    // Specifies whether the framebuffer should be sRGB capable. If supported,
    // a created OpenGL context will support the GL_FRAMEBUFFER_SRGB enable,
    // also called GL_FRAMEBUFFER_SRGB_EXT) for controlling sRGB rendering and
    // a created OpenGL ES context will always have sRGB rendering enabled.
    cGlFW->SetWindowHintBoolean(GLFW_SRGB_CAPABLE,
      cCVars->GetInternalSafe<bool>(VID_SRGB));
    // Specifies the desired refresh rate for full screen windows. If set to
    // GLFW_DONT_CARE, the highest available refresh rate will be used. This
    // hint is ignored for windowed mode windows. Although we could add an
    // override, I'm not sure what the implications are if the user picks a
    // bad mode so we'll keep this strict for now.
    cGlFW->SetWindowHint(GLFW_REFRESH_RATE, vSelected->refreshRate);
    // Specifies the desired number of auxiliary buffers. GLFW_DONT_CARE means
    // the application has no preference.
    cGlFW->SetWindowHint(GLFW_AUX_BUFFERS, iAuxBuffers);
    // Specifies the desired number of samples to use for multisampling. Zero
    // disables multisampling. GLFW_DONT_CARE means the application has no
    // preference.
    cGlFW->SetWindowHint(GLFW_SAMPLES, iSamples);
    // Specifies whether to use stereoscopic rendering. This is a hard
    // constraint.
    cGlFW->SetWindowHintBoolean(GLFW_STEREO,
      cCVars->GetInternalSafe<bool>(VID_STEREO));
    // Specifies whether the window framebuffer will be transparent. If
    // enabled and supported by the system, the window framebuffer alpha
    // channel will be used to combine the framebuffer with the background.
    // This does not affect window decorations. Possible values are GLFW_TRUE
    // and GLFW_FALSE.
    cGlFW->SetWindowHintBoolean(GLFW_TRANSPARENT_FRAMEBUFFER,
      cFboMain->fboMain.IsTransparencyEnabled());
    // Set maximized on startup attribute
    cGlFW->SetWindowHintBoolean(GLFW_MAXIMIZED,
      cCVars->GetInternalSafe<bool>(WIN_MAXIMISED));
    // Set input focused on startup
    cGlFW->SetWindowHintBoolean(GLFW_FOCUSED,
      cCVars->GetInternalSafe<bool>(WIN_FOCUSED));
    // Compiling on Mac?
#ifdef __APPLE__
    // Set retina framebuffer if MacOS version
    cGlFW->SetWindowHintBoolean(GLFW_COCOA_RETINA_FRAMEBUFFER,
      FlagIsSet(DF_HIDPI));
    // Set graphics autoswitching
    cGlFW->SetWindowHintBoolean(GLFW_COCOA_GRAPHICS_SWITCHING,
      cCVars->GetInternalSafe<bool>(VID_GASWITCH));
#endif
    // These window hints changed dynamically after creation
    cGlFW->SetWindowHintDisable(GLFW_RESIZABLE);
    cGlFW->SetWindowHintDisable(GLFW_AUTO_ICONIFY);
    cGlFW->SetWindowHintDisable(GLFW_FOCUS_ON_SHOW);
    cGlFW->SetWindowHintDisable(GLFW_FLOATING);
    cGlFW->SetWindowHintDisable(GLFW_VISIBLE);
    // Initialise basic window. We will modify it after due to limitations in
    // this particular function. For example, this can't set the refresh rate.
    cSystem->WindowInitialised(cGlFW->InitWindow(1, 1,
      cCVars->GetInternalCStrSafe(APP_TITLE), nullptr));
    // Re-adjust the window
    ReInitWindow(FlagIsSet(DF_FULLSCREEN));
    // Set forced aspect ratio
    cGlFW->SetAspectRatio(cCVars->GetInternalStrSafe(WIN_ASPECT));
    // Register monitor removal event. We can't use our events system for this
    // because once the event callback is over, the data for the monitor is
    // freed.
    glfwSetMonitorCallback(OnMonitorStatic);
    // Update icons if there are some loaded by the cvars callbacks
    UpdateIcons();
    // Set default gamma for selected monitor
    ApplyGamma();
    // Register main and window thread events
    cEvtMain->RegisterEx(*this);
    cEvtWin->RegisterEx(*this);
    // Log progress
    LW(LH_INFO, "Display class started successfully.");
  }
  /* -- DeInit ------------------------------------------------------------- */
  void DeInit(void)
  { // Ignore if class not initialised
    if(IHNotDeInitialise()) return;
    // Log progress
    LW(LH_DEBUG, "Display class deinitialising...");
    // Remove events we personally handle
    glfwSetMonitorCallback(nullptr);
    // Unfocused
    FlagClear(DF_FOCUSED);
    // Deinit window and engine events
    cEvtWin->UnregisterEx(*this);
    cEvtMain->UnregisterEx(*this);
    // Have window?
    if(cGlFW->IsWindowAvailable())
    { // If we have monitor?
      if(mSelected)
      { // Restore gamma (this fails if theres no window).
        GlFW::SetGamma(mSelected, 1.0);
        // Monitor no longer valid
        mSelected = nullptr;
      } // Tell system we destroyed the window
      cSystem->SetWindowDestroyed();
      // Actually destroy window
      cGlFW->DestroyWindow();
      // Log progress
      LW(LH_DEBUG, "Display window handle and context destroyed.");
    } // Don't have window
    else
    { // Skipped removal of window
      LW(LH_DEBUG, "Display window handle and context destruction skipped.");
      // Can't restore gamma without window
      mSelected = nullptr;
    } // Clear selected video mode
    vSelected = nullptr;
    // Log progress
    LW(LH_INFO, "Display class deinitialised successfully.");
  }
  /* -- Constructor -------------------------------------------------------- */
  Display(void) :
    /* --------------------------------------------------------------------- */
    IHelper{ __FUNCTION__ },           // Send name to init helper
    DisplayFlags{ DF_NONE },           // No display flags set
    EvtMain::RegVec{                   // Register main events
      { EMC_VID_FB_REINIT,     bind(&Display::OnFBReset,     this, _1) },
      { EMC_VID_MATRIX_REINIT, bind(&Display::OnMatrixReset, this, _1) },
      { EMC_WIN_CLOSE,         bind(&Display::OnClose,       this, _1) },
      { EMC_WIN_FOCUS,         bind(&Display::OnFocus,       this, _1) },
      { EMC_WIN_ICONIFY,       bind(&Display::OnIconify,     this, _1) },
      { EMC_WIN_LIMITS,        bind(&Display::OnLimits,      this, _1) },
      { EMC_WIN_MOVED,         bind(&Display::OnMoved,       this, _1) },
      { EMC_WIN_REFRESH,       bind(&Display::OnRefresh,     this, _1) },
      { EMC_WIN_RESIZED,       bind(&Display::OnResized,     this, _1) },
      { EMC_WIN_SCALE,         bind(&Display::OnScale,       this, _1) },
    },
    EvtWin::RegVec{                    // Register window events
      { EWC_WIN_RESET,     bind(&Display::OnReset,    this, _1) },
      { EWC_WIN_TOGGLE_FS, bind(&Display::OnToggleFS, this, _1) },
      { EWC_WIN_CENTRE,    bind(&Display::OnCentre,   this, _1) },
      { EWC_WIN_RESIZE,    bind(&Display::OnResize,   this, _1) },
      { EWC_WIN_MOVE,      bind(&Display::OnMove,     this, _1) },
      { EWC_WIN_SETICON,   bind(&Display::OnSetIcon,  this, _1) },
    },
    mSelected(nullptr),                // No monitor selected
    iMRequested(-1),                   // No monitor requested
    iMSelected(-1),                    // No monitor id selected
    vSelected(nullptr),                // No video mode selected
    iVRequested(-1),                   // No video mode id requested
    iVSelected(-1),                    // No video mode id selected
    fGamma(0),                         // Gamma initialised by CVars
    fOrthoWidth(0.0f),                 // Ortho width initialised by CVars
    fOrthoHeight(0.0f),                // Ortho height initialised by CVars
    iWinPosX(-1),                      // No initial window left position
    iWinPosY(-1),                      // No initial window right position
    iWinWidthReq(0),                   // No initial min width requested
    iWinHeightReq(0),                  // No initial min height requested,
    fWinScaleWidth(1.0),               // No initial scale width
    fWinScaleHeight(1.0),              // No initial scale height
    iAuxBuffers(0),                    // No auxiliary buffers specified
    iSamples(0),                       // No anti-aliasing samples specified
    lrFocused{ "OnFocused" }           // Set name for OnFocused lua event
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Display, DeInit());
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Display);            // Do not need copy defaults
  /* ----------------------------------------------------------------------- */
} *cDisplay = nullptr;                 // Pointer to static class
/* -- Monitor changed static event ----------------------------------------- */
void Display::OnMonitorStatic(GLFWmonitor*const mA, const int iA)
  { cDisplay->OnMonitor(mA, iA); }
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
