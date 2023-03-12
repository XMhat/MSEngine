/* == DISPLAY.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module handles window creation and manipulation via the GLFW   ## */
/* ## library.                                                            ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IfDisplay {                  // Start of module namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfInput;               // Using input namespace
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
  // Window is actually in fullscreen? Graphics switching enabled?
  DF_INFULLSCREEN        {0x00002000}, DF_GASWITCH            {0x00004000},
  // SRGB namespace is enabled?        Windoe transparency enabled?
  DF_SRGB                {0x00008000}, DF_TRANSPARENT         {0x00010000},
  // OpenGL debug context?             Stereo mode enabled?
  DF_DEBUG               {0x00020000}, DF_STEREO              {0x00040000},
  // No opengl errors?                 Window maximised at start?
  DF_NOERRORS            {0x00080000}, DF_MAXIMISED           {0x00100000}
);/* == Display class ====================================================== */
static class Display final :
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
                   iVSelected,         // Video mode selected
                   iBPPSelected;       // Selected bit depth mode
  GLfloat          fGamma,             // Monitor gamma setting
                   fOrthoWidth,        // Saved ortho width
                   fOrthoHeight;       // Saved ortho height
  int              iWinPosX, iWinPosY, // Window position
                   iWinDefX, iWinDefY, // Default window position
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
      cLog->LogDebugExSafe("Display received window position of $x$.",
        iNewX, iNewY);
      // Done
      return;
    } // Report change
    cLog->LogInfoExSafe("Display changed window position from $x$ to $x$.",
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
    if(IsFloatEqual(fNewWidth, fWinScaleWidth) &&
       IsFloatEqual(fNewHeight, fWinScaleHeight))
    { // Report event
      cLog->LogDebugExSafe("Display received window scale of $x$.",
        fNewWidth, fNewHeight);
      // Done
      return;
    } // Report change
    cLog->LogInfoExSafe("Display changed window scale from $x$ to $x$.",
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
    cGlFW->WinSetLimits(iMinW, iMinH, iMaxW, iMaxH);
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
        cLog->LogDebugSafe("Display window focus restored.");
        // Done
        break;
      // Focus lost?
      case GLFW_FALSE:
        // Return if we already recorded an unfocus event.
        if(FlagIsClear(DF_FOCUSED)) return;
        // Window is focused
        FlagClear(DF_FOCUSED);
        // Send message
        cLog->LogDebugSafe("Display window focus lost.");
        // Done
        break;
      // Unknown state?
      default:
        // Log the unknown state
        cLog->LogWarningExSafe("Display received unknown focus state $<0x$$>!",
          iState, hex, iState);
        // Done
        return;
    } // Dispatch event to lua
    lrFocused.LuaFuncDispatch(iState);
  }
  /* -- Window contents damaged and needs refreshing ----------------------- */
  void OnRefresh(const EvtMain::Cell&)
  { // Report that the window was resized
    cLog->LogDebugSafe("Display redrawing window contents.");
    // Set to force redraw the next frame
    cFboMain->SetDraw();
  }
  /* -- On window resized callback ----------------------------------------- */
  void OnResized(const EvtMain::Cell &ewcArgs)
  { // Get width and height
    const int iWidth = ewcArgs.vParams[1].i,
              iHeight = ewcArgs.vParams[2].i;
    // If position not changed? Report event and return
    if(cInput->GetWindowWidth() == iWidth &&
       cInput->GetWindowHeight() == iHeight)
      return cLog->LogDebugExSafe("Display received window size of $x$.",
        iWidth, iHeight);
    // Report change
    cLog->LogInfoExSafe("Display changed window size from $x$ to $x$.",
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
      case GLFW_TRUE: cLog->LogDebugSafe("Display window state minimised.");
        return;
      // Restored? Redraw console at least and log event
      case GLFW_FALSE:
        cFboMain->SetDraw();
        cLog->LogDebugSafe("Display window state restored.");
        break;
      // Unknown state so log it
      default:
        cLog->LogWarningExSafe("Display received unknown iconify state $.",
          iState);
        break;
    }
  }
  /* -- Enumerate monitors ------------------------------------------------- */
  void EnumerateMonitorsAndVideoModes(void)
  { // Log progress
    cLog->LogDebugSafe("Display now enumerating available displays...");
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
    GLFWmonitor*const*const mData = GlFWGetMonitors(iMonitors);
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
        cLog->LogDebugExSafe(
          "- Monitor $: $.\n"
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
          cLog->LogDebugExSafe("-- Mode $: $x$x$bpp @$hz (RGB$$$).",
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
    cLog->LogInfoExSafe(
      "Display finished enumerating $ displays...\n"
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
  void OnMonitor(GLFWmonitor*const mAffected, const int iAction)
  { // Get monitor name and if we got it? Compare state
    if(const char*const cpName = glfwGetMonitorName(mAffected)) switch(iAction)
    { // Device was connected?
      case GLFW_CONNECTED:
        // Log the event and return
        cLog->LogInfoExSafe("Display detected monitor '$'.", cpName);
        // Nothing else needs to be done
        break;
      // Device was disconnected?
      case GLFW_DISCONNECTED:
        // If this is our active montior?
        if(mAffected == mSelected)
        { // Log disconnection
          cLog->LogInfoExSafe(
            "Display disconnected monitor '$', re-initialising...", cpName);
          // Enumerate monitors again as current one is invalid
          EnumerateMonitorsAndVideoModes();
          // We need to re-initialise the opengl context
          cEvtMain->Add(EMC_QUIT_THREAD);
        } // Not our active monitor but still report it
        else cLog->LogInfoExSafe("Display disconnected monitor '$'.", cpName);
        // Continue normally
        break;
      // Unknown state?
      default:
        // Log the bad state
        cLog->LogWarningExSafe(
          "Display sent bad state of 0x$$ for monitor '$'!",
          hex, iAction, cpName);
        // Continue as normal
        break;
    } // Unknown monitor name
    else cLog->LogWarningExSafe(
      "Display sent state $$ with invalid monitor at 0x$!",
      hex, iAction, reinterpret_cast<void*>(mAffected));
  }
  /* == Call to reset the fbo ============================================== */
  void DoResizeFbo(const GLsizei stWidth, const GLsizei stHeight)
  { // Log new viewport
    cLog->LogDebugExSafe("Display received new framebuffer size of $x$.",
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
    cLog->LogDebugExSafe("Display received new framebuffer size of $x$.",
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
    cGlFW->WinSetSize(ewcArgs.vParams[0].i, ewcArgs.vParams[1].i);
    // Done
    return;
  }
  /* == Window move requested ============================================== */
  void OnMove(const EvtWin::Cell &ewcArgs)
  { // Set requested window size
    cGlFW->WinMove(ewcArgs.vParams[0].i, ewcArgs.vParams[1].i);
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
    cGlFW->WinMove(iX, iY);
  }
  /* == Window reset requested ============================================= */
  void OnReset(const EvtWin::Cell&)
  { // If in full screen mode, don't resize or move anything
    if(FlagIsSet(DF_INFULLSCREEN)) return;
    // Restore window state
    cGlFW->WinRestore();
    // Translate user specified window size and set the size of hte window
    int iX, iY; TranslateUserSize(iX, iY);
    cGlFW->WinSetSize(iX, iY);
    // Get new window size (Input won't have received it yet).
    int iW, iH; cGlFW->WinGetSize(iW, iH);
    TranslateUserCoords(iX, iY, iW, iH);
    // Update window position
    cGlFW->WinMove(iX, iY);
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
    GlFWSetGamma(mSelected, fGamma);
    // Report
    cLog->LogDebugExSafe("Display set gamma to $$.", fixed, fGamma);
  }
  /* -- ReInit desktop mode window ----------------------------------------- */
  void ReInitDesktopModeWindow(void)
  { // Not in full-screen mode
    FlagClear(DF_INFULLSCREEN);
    // Trnslate user specified window size
    int iW, iH; TranslateUserSize(iW, iH);
    int iX, iY; TranslateUserCoords(iX, iY, iW, iH);
    // Set menu bar on MacOS
    // cGlFW->SetCocoaMenuBarEnabled();
    // Initialise the desktop window and send the handle to system class
    // because they need the handle for exceptions, icons and other things.
    cGlFW->WinSetMonitor(nullptr, iX, iY, iW, iH, 0);
    // Window mode so update users window border setting
    cGlFW->WinSetDecoratedAttrib(FlagIsSet(DF_BORDER));
    // Log that we switched to window mode
    cLog->LogInfoExSafe("Display switched to desktop window $x$ at $x$.",
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
    { // Hide menu bar on MacOS if in borderless full-screen mode
//    cGlFW->SetCocoaMenuBarDisabled();
      // Not using exclusive full-screen mode
      mUsing = nullptr;
      cpType = "borderless";
      // Force disable window border
      GlFWSetDecoratedDisabled();
    } // Set the full-screen window
    cGlFW->WinSetMonitor(mUsing, 0, 0,
      vmData.width, vmData.height, vmData.refreshRate);
    // Log that we switched to full-screen mode
    cLog->LogInfoExSafe(
      "Display switch to $ full-screen $x$ (M:$>$;V:$>$;R:$).",
      cpType, vmData.width, vmData.height, iMRequested, iMSelected,
      iVRequested, iVSelected, vmData.refreshRate);
  }
  /* -- Translate user specified window dimensions ------------------------- */
  void TranslateUserSize(int &iW, int &iH) const
  { // Get window size specified by user
    iW = iWinWidthReq;
    iH = iWinHeightReq;
    // If size is not valid? Report result and return
    if(iW > 0 && iH > 0)
      return cLog->LogDebugExSafe(
        "Display using user specified dimensions of $x$.", iW, iH);
    // We need a selected video resolution
    if(!vSelected)
    { // Set defaults
      iW = 640;
      iH = 480;
      // Put message in log
      return cLog->LogWarningExSafe("Display class cannot automatically "
        "detect the best dimensions for the window because the current "
        "desktop resolution was not detected properly! Using fallback default "
        "of $x$.", iW, iH);
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
    cLog->LogDebugExSafe("Display translated user size to $x$.", iW, iH);
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
      cLog->LogWarningSafe(
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
    iX = iWinDefX;
    iY = iWinDefY;
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
    cGlFW->WinSetResizableAttrib(FlagIsSet(DF_SIZABLE));
    cGlFW->WinSetFloatingAttrib(FlagIsSet(DF_FLOATING));
    cGlFW->WinSetAutoIconifyAttrib(FlagIsSet(DF_AUTOICONIFY));
    cGlFW->WinSetFocusOnShowAttrib(FlagIsSet(DF_AUTOFOCUS));
    // Store current window position
    cGlFW->WinGetPos(iWinPosX, iWinPosY);
    cInput->UpdateWindowSize();
    cGlFW->WinGetScale(fWinScaleWidth, fWinScaleHeight);
    // Need to fix a GLFW scaling bug with this :(
#if defined(MACOS)
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
    cGlFW->WinShow();
    cGlFW->WinFocus();
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
  /* -- Set maximised at startup ------------------------------------------- */
  CVarReturn SetMaximisedMode(const bool bState)
    { FlagSetOrClear(DF_MAXIMISED, bState); return ACCEPT; }
  /* -- Set opengl no errors mode ------------------------------------------ */
  CVarReturn SetNoErrorsMode(const bool bState)
    { FlagSetOrClear(DF_NOERRORS, bState); return ACCEPT; }
  /* -- Set stereo mode ---------------------------------------------------- */
  CVarReturn SetStereoMode(const bool bState)
    { FlagSetOrClear(DF_STEREO, bState); return ACCEPT; }
  /* -- Set OpenGL debug mode ---------------------------------------------- */
  CVarReturn SetGLDebugMode(const bool bState)
    { FlagSetOrClear(DF_DEBUG, bState); return ACCEPT; }
  /* -- Set window transparency mode  -------------------------------------- */
  CVarReturn SetWindowTransparency(const bool bState)
    { FlagSetOrClear(DF_TRANSPARENT, bState); return ACCEPT; }
  /* -- Set default orthagonal width  -------------------------------------- */
  CVarReturn SetForcedBitDepth(const int iBPP)
    { return CVarSimpleSetIntNLG(iBPPSelected, iBPP, 0, 16); }
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
  /* -- Set SRGB colour space ---------------------------------------------- */
  CVarReturn SRGBColourSpaceChanged(const bool bState)
    { FlagSetOrClear(DF_SRGB, bState); return ACCEPT; }
  /* -- Set graphics switching --------------------------------------------- */
  CVarReturn GraphicsSwitchingChanged(const bool bState)
    { FlagSetOrClear(DF_GASWITCH, bState); return ACCEPT; }
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
  /* -- Set window X position ---------------------------------------------- */
  CVarReturn SetXPosition(const int iNewX)
  { // Deny change request if an invalid value was sent
    if(!CVarToBoolReturn(CVarSimpleSetInt(iWinDefX, iNewX))) return DENY;
    // Apply window position if window is available
    if(cGlFW && cGlFW->WinIsAvailable()) RequestReposition();
    // Success
    return ACCEPT;
  }
  /* -- Set window Y position ---------------------------------------------- */
  CVarReturn SetYPosition(const int iNewY)
  { // Deny change request if an invalid value was sent
    if(!CVarToBoolReturn(CVarSimpleSetInt(iWinDefY, iNewY))) return DENY;
    // Apply window position if window is available
    if(cGlFW && cGlFW->WinIsAvailable()) RequestReposition();
    // Success
    return ACCEPT;
  }
  /* -- Set gamma ---------------------------------------------------------- */
  CVarReturn GammaChanged(const GLfloat fNewGamma)
  { // Deny change request if an invalid gamma value was sent
    if(!CVarToBoolReturn(CVarSimpleSetIntNLG(fGamma, fNewGamma, 0.25f, 4.00f)))
      return DENY;
    // Apply new gamma setting if window is available
    if(cGlFW && cGlFW->WinIsAvailable() && mSelected) ApplyGamma();
    // Success
    return ACCEPT;
  }
  /* -- Icon filenames changed (allow blank strings) ------ Core::SetIcon -- */
  CVarReturn SetIcon(const string &strF, string&)
    { return BoolToCVarReturn(strF.empty() || SetIcon(strF)); }
  /* -- Get selected monitor id -------------------------------------------- */
  int GetMonitorId(void) const { return iMSelected; }
  /* -- Get selected video mode id ----------------------------------------- */
  int GetVideoModeId(void) const { return iVSelected; }
  /* -- Get video mode data ------------------------------------------------ */
  const GLFWvidmode &GetVideoMode(const int iMId, const int iVId) const
  { // Num monitors and num modes
    int iMonitors, iModes;
     // Get monitors list
    GLFWmonitor*const*const mData = GlFWGetMonitors(iMonitors);
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
    GLFWmonitor*const*const mData = GlFWGetMonitors(iMonitors);
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
#if !defined(MACOS)
    // If using interactive mode?
    if(cSystem->IsGuiMode(GM_GRAPHICS))
    { // Ignore if no icons
      if(giImages.empty()) return;
      // Capture exceptions and ask GLFW to set the icon
      try { cGlFW->WinSetIcon(IntOrMax<int>(giImages.size()),
              giImages.data()); }
      // Exception occured? GLFW can throw GLFW_PLATFORM_ERROR on Wayland which
      // is absolutely retarded as is not consistent with other platforms such
      // as MacOS which will silently succeed
      catch(const exception &e)
      { // Just log the error that occured
        cLog->LogWarningExSafe(
          "Display could not load $ icon files due to GlFW exception: $.",
            giImages.size(), e.what());
        // Done
        return;
      } // Report that we updated the icons
      cLog->LogInfoExSafe("Display updated $ windows icons." ,
        giImages.size());
      // Show details?
      if(cLog->HasLevel(LH_DEBUG))
        for(Image &imC : imgIcons)
        { // Get first icon
          ImageSlot &imsD = imC.front();
          // Write data
          cLog->LogNLCDebugExSafe("- $x$x$: $.",
            imsD.DimGetWidth(), imsD.DimGetHeight(),
            imC.GetBitsPerPixel(),
            imC.IdentGet());
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
          Image{ std::move(strName), IL_REVERSE|IL_TORGB|IL_TO32BPP });
        ImageSlot &imsD = imC.front();
        giImages.emplace_back(GLFWimage{
          imsD.DimGetWidth<int>(), imsD.DimGetHeight<int>(),
          imsD.Ptr<unsigned char>() });
      }
    } // Not in interactive mode?
    else
    { // Only Win32 terminal windows can change the icon afaik
#if defined(WINDOWS)
      // Have two icons at least?
      if(tIcons.size() >= 2)
      { // Set small icon from the last icon specified
        Image imC{ std::move(*prev(tIcons.cend())), IL_REVERSE|IL_TOBGR };
        ImageSlot &imsD = imC.front();
        cSystem->SetSmallIcon(imC.IdentGet(), imsD.DimGetWidth(),
          imsD.DimGetHeight(), imC.GetBitsPerPixel(), imsD);
      } // Set large icon from the first icon specified
      Image imC{ std::move(*next(tIcons.cbegin())), IL_REVERSE|IL_TOBGR };
      ImageSlot &imsD = imC.front();
      cSystem->SetLargeIcon(imC.IdentGet(), imsD.DimGetWidth(),
        imsD.DimGetHeight(), imC.GetBitsPerPixel(), imsD);
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
    cLog->LogDebugExSafe("Display class starting up...");
    // Inform main fbo class of our transparency setting
    cFboMain->fboMain.SetTransparency(FlagIsSet(DF_TRANSPARENT));
    // Enumerate monitors and video modes
    EnumerateMonitorsAndVideoModes();
    // We are using the OpenGL 3.2 forward compatible API
    GlFWSetClientAPI(GLFW_OPENGL_API);
    GlFWSetContextVersion(3, 2);
    GlFWSetCoreProfile(GLFW_OPENGL_CORE_PROFILE);
    GlFWSetForwardCompatEnabled();
    GlFWSetRobustness(GLFW_LOSE_CONTEXT_ON_RESET);
    // Set other settings
    GlFWSetDebug(FlagIsSet(DF_DEBUG));
    GlFWSetNoErrors(FlagIsSet(DF_NOERRORS));
    GlFWSetDoubleBufferEnabled();
    GlFWSetSRGBCapable(FlagIsSet(DF_SRGB));
    GlFWSetRefreshRate(vSelected->refreshRate);
    GlFWSetAuxBuffers(iAuxBuffers);
    GlFWSetMultisamples(iSamples);
    GlFWSetMaximised(FlagIsSet(DF_MAXIMISED));
    GlFWSetStereo(FlagIsSet(DF_STEREO));
    GlFWSetTransparency(cFboMain->fboMain.IsTransparencyEnabled());
    GlFWSetDepthBits(0);
    GlFWSetStencilBits(0);
    // Force custom bit-depth?
    if(iBPPSelected)
      GlFWSetColourDepth(iBPPSelected, iBPPSelected, iBPPSelected,
        cFboMain->fboMain.IsTransparencyEnabled() ? iBPPSelected : 0);
    // Use default? Set regular depth buffer bits
    else GlFWSetColourDepth(vSelected->redBits, vSelected->greenBits,
      vSelected->blueBits, cFboMain->fboMain.IsTransparencyEnabled() ? 8 : 0);
    // Set Apple operating system only settings
#if defined(MACOS)
    GlFWSetRetinaMode(FlagIsSet(DF_HIDPI));
    GlFWSetGPUSwitching(FlagIsSet(DF_GASWITCH));
#endif
    // Get window name and use it for frame and instance name. It's assumed
    // that 'cpTitle' won't be freed while using it these two times.
    const char*const cpTitle = cCVars->GetInternalCStrSafe(APP_TITLE);
    GlFWSetFrameName(cpTitle);
    // Initialise basic window. We will modify it after due to limitations in
    // this particular function. For example, this can't set the refresh rate.
    cSystem->WindowInitialised(cGlFW->WinInit(1, 1, cpTitle, nullptr));
    // Re-adjust the window
    ReInitWindow(FlagIsSet(DF_FULLSCREEN));
    // Set forced aspect ratio
    cGlFW->WinSetAspectRatio(cCVars->GetInternalStrSafe(WIN_ASPECT));
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
    cLog->LogInfoExSafe("Display class started successfully.");
  }
  /* -- DeInit ------------------------------------------------------------- */
  void DeInit(void)
  { // Ignore if class not initialised
    if(IHNotDeInitialise()) return;
    // Log progress
    cLog->LogDebugExSafe("Display class deinitialising...");
    // Remove events we personally handle
    glfwSetMonitorCallback(nullptr);
    // Unfocused
    FlagClear(DF_FOCUSED);
    // Deinit window and engine events
    cEvtWin->UnregisterEx(*this);
    cEvtMain->UnregisterEx(*this);
    // Have window?
    if(cGlFW->WinIsAvailable())
    { // If we have monitor?
      if(mSelected)
      { // Restore gamma (this fails if theres no window).
        GlFWSetGamma(mSelected, 1.0);
        // Monitor no longer valid
        mSelected = nullptr;
      } // Tell system we destroyed the window
      cSystem->SetWindowDestroyed();
      // Actually destroy window
      cGlFW->WinDeInit();
      // Log progress
      cLog->LogDebugExSafe("Display window handle and context destroyed.");
    } // Don't have window
    else
    { // Skipped removal of window
      cLog->LogDebugExSafe(
        "Display window handle and context destruction skipped.");
      // Can't restore gamma without window
      mSelected = nullptr;
    } // Clear selected video mode
    vSelected = nullptr;
    // Log progress
    cLog->LogInfoExSafe("Display class deinitialised successfully.");
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
    iBPPSelected(-1),                  // Bit depth not selected yet
    fGamma(0),                         // Gamma initialised by CVars
    fOrthoWidth(0.0f),                 // Ortho width initialised by CVars
    fOrthoHeight(0.0f),                // Ortho height initialised by CVars
    iWinPosX(-1), iWinPosY(-1),        // No initial window position
    iWinDefX(-1), iWinDefY(-1),        // No default window position
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
/* ------------------------------------------------------------------------- */
};                                     // End of module namespace
/* == EoF =========================================================== EoF == */
