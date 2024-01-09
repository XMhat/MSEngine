/* == DISPLAY.HPP ========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## This module handles window creation and manipulation via the GLFW   ## **
** ## library.                                                            ## **
** ######################################################################### **
** ========================================================================= */
#pragma once                           // Only one incursion allowed
/* ------------------------------------------------------------------------- */
namespace IDisplay {                   // Start of private module namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace ICollector::P;         using namespace IConsole::P;
using namespace ICVar::P;              using namespace ICVarDef::P;
using namespace ICVarLib::P;           using namespace IDim;
using namespace IDir::P;               using namespace IEvtMain::P;
using namespace IEvtWin::P;            using namespace IFboMain::P;
using namespace IFlags;                using namespace IFont::P;
using namespace IGlFW::P;              using namespace IGlFWMonitor::P;
using namespace IGlFWUtil::P;          using namespace IIdent::P;
using namespace IImage::P;             using namespace IImageDef::P;
using namespace IInput::P;             using namespace ILog::P;
using namespace ILuaFunc::P;           using namespace IStd::P;
using namespace IString::P;            using namespace ISystem::P;
using namespace ISysUtil::P;           using namespace ITexture::P;
using namespace IToken::P;             using namespace IUtil::P;
using namespace Lib::OS::GlFW;
/* ------------------------------------------------------------------------- */
namespace P {                          // Start of public module namespace
/* ------------------------------------------------------------------------- */
BUILD_FLAGS(Display,
  /* -- Active flags ------------------------------------------------------- */
  // No display flags                  // Window is focused?
  DF_NONE                {0x00000000}, DF_FOCUSED             {0x00000001},
  // Exclusive mode full-screen?       Display system restarting?
  DF_EXCLUSIVE           {0x00000002}, DF_RESTARTING          {0x00000004},
  // Window is in it's own thread?     Window is actually in fullscreen?
  DF_WINTHREADED         {0x00000008}, DF_INFULLSCREEN        {0x00000010},
  // Full-screen locked?
  DF_NATIVEFS            {0x00000020},
  /* -- End-user configuration flags --------------------------------------- */
  // Automatic minimise?               Focus on show?
  DF_AUTOICONIFY         {0x00010000}, DF_AUTOFOCUS           {0x00020000},
  // Window is resizable?              Always on top?
  DF_SIZABLE             {0x00040000}, DF_FLOATING            {0x00080000},
  // Window has a border?              Minimize on lose focus?
  DF_BORDER              {0x00100000}, DF_MINFOCUS            {0x00200000},
  // HiDPI is enabled?                 SRGB namespace is enabled?
  DF_HIDPI               {0x00400000}, DF_SRGB                {0x00800000},
  // Graphics switching enabled?       Full-screen mode set?
  DF_GASWITCH            {0x01000000}, DF_FULLSCREEN          {0x02000000},
  // Window is closable?               Stereo mode enabled?
  DF_CLOSEABLE           {0x04000000}, DF_STEREO              {0x08000000},
  // OpenGL debug context?             Window transparency enabled?
  DF_DEBUG               {0x10000000}, DF_TRANSPARENT         {0x20000000},
  // No opengl errors?                 Window maximised at start?
  DF_NOERRORS            {0x40000000}, DF_MAXIMISED           {0x80000000}
);
/* == Display class ======================================================== */
static class Display final :
  /* -- Base classes ------------------------------------------------------- */
  private IHelper,                     // Initialisation helper
  public  DisplayFlags,                // Display settings
  private EvtMain::RegVec,             // Main events list to register
  private EvtWin::RegVec,              // Window events list to register
  private DimCoInt                     // Requested window position and size
{ /* -- Variables ---------------------------------------------------------- */
  GlFWMonitors     mlData;             // Monitor list data
  const GlFWMonitor *moSelected;       // Monitor selected
  const GlFWRes   *rSelected;          // Monitor resolution selected
  size_t           stMRequested,       // Monitor id request
                   stVRequested;       // Video mode requested
  GLfloat          fGamma,             // Monitor gamma setting
                   fOrthoWidth,        // Saved ortho width
                   fOrthoHeight;       // Saved ortho height
  int              iBPPSelected,       // Selected bit depth mode
                   iWinPosX, iWinPosY, // Window position
                   iAuxBuffers,        // Auxilliary buffers to use
                   iSamples;           // FSAA setting to use
  float            fWinScaleWidth,     // Window scale width
                   fWinScaleHeight;    // Window scale height
  string           strClipboard;       // Clipboard string to copy or grab
  /* -- Icons -------------------------------------------------------------- */
  typedef vector<GLFWimage> GlFWIconList; // Glfw image list
  GlFWIconList     gilIcons;           // Contiguous memory for glfw image data
  typedef vector<Image> IconList;      // Actual icon list
  IconList         ilIcons;            // Image icon data
  /* -- Macros ----------------------------------------------------- */ public:
  LuaFunc          lrFocused,          // Window focused lua event
                   lrClipboard;        // Clipboard function callback
  /* -- Public typedefs ---------------------------------------------------- */
  enum FSType {                        // Available window types
    /* --------------------------------------------------------------------- */
    FST_STANDBY,                       // [0] Full-screen type is not set
    FST_WINDOW,                        // [1] Window/desktop mode
    FST_EXCLUSIVE,                     // [2] Exclusive full-screen mode
    FST_BORDERLESS,                    // [3] Borderless full-screen mode
    FST_NATIVE,                        // [4] Native full-screen mode (MacOS)
    FST_MAX,                           // [5] Number of id's
    /* --------------------------------------------------------------------- */
  } fsType;                            // Current value
  /* -- Display class ------------------------------------------------------ */
  typedef IdList<FST_MAX> FSTStrings;  // List of FST_ id strings typedef.
  const FSTStrings    fstStrings;      // " container
  /* -- Check if window moved ------------------------------------- */ private:
  void CheckWindowMoved(const int iNewX, const int iNewY)
  { // If position not changed?
    if(iWinPosX == iNewX && iWinPosY == iNewY)
    { // Report event and return
      cLog->LogDebugExSafe("Display received window position of $x$.",
        iNewX, iNewY);
      return;
    } // Report change
    cLog->LogInfoExSafe("Display changed window position from $x$ to $x$.",
      iWinPosX, iWinPosY, iNewX, iNewY);
    // Update position
    iWinPosX = iNewX;
    iWinPosY = iNewY;
  }
  /* -- Window moved request ----------------------------------------------- */
  void OnMoved(const EvtMain::Cell &ewcArgs)
    { CheckWindowMoved(ewcArgs.vParams[1].i, ewcArgs.vParams[2].i); }
  /* -- Window set icon request -------------------------------------------- */
  void OnSetIcon(const EvtWin::Cell&) { UpdateIcons(); }
  /* -- Window was asked to be hidden -------------------------------------- */
  void OnHide(const EvtWin::Cell&) { cGlFW->WinHide(); }
  /* -- Window scale change request ---------------------------------------- */
  void OnScale(const EvtMain::Cell &ewcArgs)
  { // Get new values
    const float fNewWidth = ewcArgs.vParams[1].f,
                fNewHeight = ewcArgs.vParams[2].f;
    // If scale not changed?
    if(UtilIsFloatEqual(fNewWidth, fWinScaleWidth) &&
       UtilIsFloatEqual(fNewHeight, fWinScaleHeight))
    { // Report event and return
      cLog->LogDebugExSafe("Display received window scale of $x$.",
        fNewWidth, fNewHeight);
      return;
    } // Report change
    cLog->LogInfoExSafe("Display changed window scale from $x$ to $x$.",
      fWinScaleWidth, fWinScaleHeight, fNewWidth, fNewHeight);
    // Set new value
    fWinScaleWidth = fNewWidth;
    fWinScaleHeight = fNewHeight;
  }
  /* -- Window limits change request --------------------------------------- */
  void OnLimits(const EvtWin::Cell &ewcArgs)
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
  /* == Check if window resized ============================================ */
  void CheckWindowResized(const int iWidth, const int iHeight) const
  { // If position not changed? Report event and return
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
  /* -- On window resized callback ----------------------------------------- */
  void OnResized(const EvtMain::Cell &ewcArgs)
    { CheckWindowResized(ewcArgs.vParams[1].i, ewcArgs.vParams[2].i); }
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
    // Refresh monitors data
    mlData.Refresh();
    // Get primary monitor information, throw exception if invalid
    for(const GlFWMonitor &mData : mlData)
    { // Write information about the monitor
      cLog->LogDebugExSafe(
        "- Monitor $: $.\n"            "- Mode count: $.\n"
        "- Position: $x$.\n"           "- Dimensions: $$$x$\" ($x$mm).\n"
        "- Size: $\" ($mm).",
        mData.Index(), mData.Name(), mData.size(), mData.CoordGetX(),
        mData.CoordGetY(), fixed, setprecision(1), mData.WidthInch(),
        mData.HeightInch(), mData.DimGetWidth(), mData.DimGetHeight(),
        mData.DiagonalInch(), mData.Diagonal());
      // Log resolution data
      for(const GlFWRes &rData : mData)
        cLog->LogDebugExSafe("-- Mode $: $x$x$<RGB$$$> @ $Hz$.",
          rData.Index(), rData.Width(), rData.Height(), rData.Depth(),
          rData.Red(), rData.Green(), rData.Blue(), rData.Refresh(),
          &rData == mData.Primary() ? " (Active)" : cCommon->Blank());
    } // Custom monitor selected (-2) and valid monitor? Set it
    static constexpr size_t stM2 = StdMaxSizeT - 1;
    moSelected = (stMRequested < stM2 && stMRequested < mlData.size()) ?
      &mlData[stMRequested] : mlData.Primary();
    // Get selected screen resolution
    rSelected = (stVRequested < stM2 && stVRequested < (*moSelected).size()) ?
      &(*moSelected)[stVRequested] : moSelected->Primary();
    // Set exclusive full-screen mode flag if value is not -2
    FlagSetOrClear(DF_EXCLUSIVE, stVRequested != stM2);
    // Get primary monitor and mode
    const GlFWMonitor &moPrimary = *mlData.Primary();
    const GlFWRes &rPrimary = *moPrimary.Primary();
    // Write to log the monitor we are using
    cLog->LogInfoExSafe(
      "Display finished enumerating $ displays...\n"
      "- Primary monitor $: $ @$x$ ($$$\"x$\"=$\").\n"
      "- Primary mode $: $x$x$bpp(R$G$B$) @$hz.\n"
      "- Selected monitor $: $ @$x$ ($$$\"x$\"=$\").\n"
      "- Selected mode $: $x$x$bpp(R$G$B$) @$hz.",
      mlData.size(),
      moPrimary.Index(), moPrimary.Name(), moPrimary.CoordGetX(),
        moPrimary.CoordGetY(), fixed, setprecision(1),
        moPrimary.WidthInch(), moPrimary.HeightInch(),
        moPrimary.DiagonalInch(),
      rPrimary.Index(), rPrimary.Width(), rPrimary.Height(),
        rPrimary.Depth(), rPrimary.Red(), rPrimary.Green(),
        rPrimary.Blue(), rPrimary.Refresh(),
      moSelected->Index(), moSelected->Name(), moSelected->CoordGetX(),
        moSelected->CoordGetY(), fixed, setprecision(1),
        moSelected->WidthInch(), moSelected->HeightInch(),
        moSelected->DiagonalInch(),
      rSelected->Index(), rSelected->Width(), rSelected->Height(),
        rSelected->Depth(), rSelected->Red(), rSelected->Green(),
        rSelected->Blue(), rSelected->Refresh());
  }
  /* -- Monitor changed ---------------------------------------------------- */
  static void OnMonitorStatic(GLFWmonitor*const, const int);
  void OnMonitor(GLFWmonitor*const mAffected, const int iAction)
  { // Get monitor state
    switch(iAction)
    { // Device was connected?
      case GLFW_CONNECTED:
        // Ignore if null context
        if(!mAffected)
          cLog->LogWarningSafe(
            "Display connected monitor event with null context!");
        // We shouldn't already have this monitor in our structured list
        else if(const GlFWMonitor*const moAffected = mlData.Find(mAffected))
          cLog->LogWarningExSafe(
            "Display already connected monitor '$'...", moAffected->Name());
        // We don't have this context in our list, get name and if valid?
        else if(const char*const cpName = GlFWGetMonitorName(mAffected))
        { // If monitor name is not blank?
          if(*cpName)
            cLog->LogInfoExSafe(
              "Display connected monitor '$', refreshing device list...",
              cpName);
          // Monitor name not specified so OS could be messing around
          else cLog->LogInfoExSafe("Display hardware shuffle in progress, "
            "refreshing device list...");
          // Re-enumerate monitors and video modes
          EnumerateMonitorsAndVideoModes();
        } // Break to return
        break;
      // Device was disconnected?
      case GLFW_DISCONNECTED:
        // Ignore if null context
        if(!mAffected)
          cLog->LogWarningSafe(
            "Display disconnected monitor event with null context!");
        // We should have this monitor in our structured list
        else if(const GlFWMonitor*const moAffected = mlData.Find(mAffected))
        { // If this was our monitor?
          if(moAffected == moSelected)
          { // We recognise it so we can savely disconnect it
            cLog->LogInfoExSafe(
              "Display disconnected monitor '$', re-initialising...",
                moAffected->Name());
            // The selected device is no longer valid so make sure it is
            // cleared so DeInit() doesn't try to restore gamma and crash the
            // whole engine with an exception.
            moSelected = nullptr;
            // We need to re-initialise the opengl context
            cEvtMain->Add(EMC_QUIT_THREAD);
          } // This is not our monitor?
          else
          { // We don't need to do anything but refresh the list
            cLog->LogInfoExSafe(
             "Display disconnected monitor '$', refreshing device list...",
               moAffected->Name());
            // Re-enumerate monitors and video modes
            EnumerateMonitorsAndVideoModes();
          }
        } // We don't have it so ignore it
        else cLog->LogWarningExSafe(
          "Display already disconnected monitor '$'...",
            GlFWGetMonitorName(mAffected));
        // Break to return
        break;
      // Unknown state?
      default:
        // Ignore if null context
        if(!mAffected)
          cLog->LogWarningExSafe(
            "Display sent bad state of 0x$$ with null context!",
            hex, iAction);
        // Not a null context?
        else
        { // Find this monitor context, log the failure and ignore event
          const GlFWMonitor*const moAffected = mlData.Find(mAffected);
          cLog->LogWarningExSafe(
            "Display sent bad state of 0x$$ for monitor '$'!",
            hex, iAction,
            moAffected ? moAffected->Name() : GlFWGetMonitorName(mAffected));
        } // Break to return
        break;
    }
  }
  /* == Matrix reset requested ============================================= */
  void OnMatrixReset(const EvtMain::Cell&) { ReInitMatrix(); }
  /* == Framebuffer reset requested ======================================== */
  void OnFBReset(const EvtMain::Cell &emcArgs)
  { // Get new frame buffer size
    const int iWidth = emcArgs.vParams[1].i, iHeight = emcArgs.vParams[2].i;
    // On Mac?
#ifdef MACOS
    // Get addition position and window size data
    const int iWinX = emcArgs.vParams[3].i, iWinY = emcArgs.vParams[4].i,
      iWinWidth = emcArgs.vParams[5].i, iWinHeight = emcArgs.vParams[6].i;
    // Log new viewport
    cLog->LogDebugExSafe(
      "Display received new frame buffer size of $x$ (P:$x$;W:$x$).",
        iWidth, iHeight, iWinX, iWinY, iWinWidth, iWinHeight);
    // What is the window type?
    // Frame buffer is covering the entire screen?
    if(!iWinX && !iWinY && rSelected->IsDim(iWinWidth, iWinHeight))
    { // We don't recognise the full-screen?
      if(FlagIsClear(DF_INFULLSCREEN))
      { // Now a native borderless full-screen window
        fsType = FST_NATIVE;
        // In native full-screen mode also re-init the console fbo as well.
        FlagSet(DF_INFULLSCREEN|DF_NATIVEFS);
        // Log non-standard full-screen switch
        cLog->LogDebugExSafe(
          "Display received external full-screen switch!");
        // Update viewport and check if window moved/resized as glfw wont
        goto UpdateViewport;
      }
    } // Full-screen mode flag is set?
    else if(FlagIsSet(DF_INFULLSCREEN))
    { // Now a normal window again
      fsType = FST_WINDOW;
      // Not in native full-screen mode and re-init console fbo
      FlagClear(DF_INFULLSCREEN|DF_NATIVEFS);
      // Log non-standard full-screen switch
      cLog->LogDebugExSafe("Display received external desktop switch!");
      // Update viewport jump from above if/condition scope
      UpdateViewport: cEvtMain->Add(EMC_VID_MATRIX_REINIT);
      // Check if window moved/resized as glfw wont send these
      CheckWindowMoved(iWinX, iWinY);
      CheckWindowResized(iWinWidth, iWinHeight);
    } // Clear native flag otherwise
    else if(FlagIsSet(DF_NATIVEFS)) FlagClear(DF_NATIVEFS);
    // Anything but Mac?
#else
    // Just log new viewport
    cLog->LogDebugExSafe("Display received new framebuffer size of $x$.",
      iWidth, iHeight);
#endif
    // Resize main viewport and if it changed, re-initialise the console fbo
    // and redraw the console
    if(cFboMain->AutoViewport(iWidth, iHeight)) cConsole->InitFBO();
    // Redraw the console if enabled
    else cConsole->SetRedrawIfEnabled();
  }
  /* == Window size requested ============================================== */
  void OnResize(const EvtWin::Cell &ewcArgs)
    { cGlFW->WinSetSize(ewcArgs.vParams[0].i, ewcArgs.vParams[1].i); }
  /* == Window move requested ============================================== */
  void OnMove(const EvtWin::Cell &ewcArgs)
    { cGlFW->WinMove(ewcArgs.vParams[0].i, ewcArgs.vParams[1].i); }
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
  { // Ignore further requests if already restarting or using native fullscreen
    if(FlagIsAnyOfSet(DF_RESTARTING, DF_NATIVEFS)) return;
    // Changing window settings, accept no more events of this type
    FlagSet(DF_RESTARTING);
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
    GlFWSetGamma(moSelected->Context(), fGamma);
    // Report
    cLog->LogDebugExSafe("Display set gamma to $$.", fixed, fGamma);
  }
  /* -- ReInit desktop mode window ----------------------------------------- */
  void ReInitDesktopModeWindow(void)
  { // Not in full-screen mode or native mode
    FlagClear(DF_INFULLSCREEN|DF_NATIVEFS);
    // Trnslate user specified window size
    int iW, iH; TranslateUserSize(iW, iH);
    TranslateUserCoords(iWinPosX, iWinPosY, iW, iH);
    // Is a desktop mode window (Could change via OnFBReset())
    fsType = FST_WINDOW;
    // We need to adjust to the position of the currently selected monitor so
    // it actually appears on that monitor.
    iWinPosX += moSelected->CoordGetX();
    iWinPosY += moSelected->CoordGetY();
    // Instruct glfw to change to window mode
    cGlFW->WinSetMonitor(nullptr, iWinPosX, iWinPosY, iW, iH, 0);
    // Window mode so update users window border
    cGlFW->WinSetDecoratedAttrib(FlagIsSet(DF_BORDER));
    cGlFW->WinSetResizableAttrib(FlagIsSet(DF_SIZABLE));
    // Log that we switched to window mode
    cLog->LogInfoExSafe("Display switched to desktop window $x$ at $x$.",
      iW, iH, iWinPosX, iWinPosY);
  }
  /* -- ReInit full-screen mode window ------------------------------------- */
  void ReInitFullScreenModeWindow(void)
  { // Actually in full screen mode window
    FlagSet(DF_INFULLSCREEN);
    // Chosen settings
    GLFWmonitor *mUsing;
    const char *cpType;
    // Exclusive full-screen window requested?
    if(FlagIsSet(DF_EXCLUSIVE))
    { // Set video mode and label
      mUsing = moSelected->Context();
      cpType = "exclusive";
      fsType = FST_EXCLUSIVE;
    } // Not exclusive full-screen mode?
    else
    { // Not using exclusive full-screen mode
      mUsing = nullptr;
      cpType = "borderless";
      fsType = FST_BORDERLESS;
      // Need to disable decoration and resizing
      cGlFW->WinSetDecoratedAttribDisabled();
      cGlFW->WinSetResizableAttribDisabled();
    } // Position is top-left in full-screen
    iWinPosX = iWinPosY = 0;
    // Instruct glfw to set full-screen window
    cGlFW->WinSetMonitor(mUsing, iWinPosX, iWinPosY,
      rSelected->Width(), rSelected->Height(), rSelected->Refresh());
    // Log that we switched to full-screen mode
    cLog->LogInfoExSafe(
      "Display switch to $ full-screen $x$ (M:$>$;V:$>$;R:$).",
      cpType, rSelected->Width(), rSelected->Height(), stMRequested,
      moSelected->Index(), stVRequested, rSelected->Index(),
      rSelected->Refresh());
  }
  /* -- Translate user specified window dimensions ------------------------- */
  void TranslateUserSize(int &iW, int &iH) const
  { // Get window size specified by user
    iW = DimGetWidth();
    iH = DimGetHeight();
    // If size is not valid? Report result and return
    if(iW > 0 && iH > 0)
      return cLog->LogDebugExSafe(
        "Display using user specified dimensions of $x$.", iW, iH);
    // Convert selected height to double as we need to use it twice
    const double fdHeight = static_cast<double>(rSelected->Height());
    // Set the height to 80% of desktop height
    iH = static_cast<int>(ceil(fdHeight * 0.8));
    // Now set the width based on desktop aspect ratio
    iW = static_cast<int>(ceil(static_cast<double>(iH) *
      (static_cast<double>(rSelected->Width()) / fdHeight)));
    // Report result
    cLog->LogDebugExSafe("Display translated user size to $x$.", iW, iH);
  }
  /* -- Get centre co-ordinates -------------------------------------------- */
  void GetCentreCoords(int &iX, int &iY, const int iW, const int iH) const
  { // Bail if not in full screen
    if(FlagIsSet(DF_INFULLSCREEN)) { iX = iY = 0; return; }
    // Must have current desktop information
    if(!moSelected->Context())
    { // Clear co-ordinates
      iX = iY = 0;
      // Put message in  log
      cLog->LogWarningSafe(
        "Display class cannot centre the window without monitor data.");
      return;
    } // Get new co-ordinates
    iX = (rSelected->Width() / 2) - (iW / 2);
    iY = (rSelected->Height() / 2) - (iH / 2);
  }
  /* -- Translate co-ordinates --------------------------------------------- */
  void TranslateUserCoords(int &iX, int &iY, const int iW, const int iH) const
  { // Put user values into window co-ordinates
    iX = CoordGetX();
    iY = CoordGetY();
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
      cFboMain->DimSet(static_cast<GLsizei>(cInput->GetWindowWidth()),
                       static_cast<GLsizei>(cInput->GetWindowHeight()));
      // Done
      return;
    } // Get window scale
    // Update the main fbo viewport size with scale
    cFboMain->DimSet(static_cast<GLsizei>(cInput->GetWindowWidth()) *
                       static_cast<GLsizei>(fWinScaleWidth),
                     static_cast<GLsizei>(cInput->GetWindowHeight()) *
                       static_cast<GLsizei>(fWinScaleHeight));
    // Remove native flag since GLFW canno set or detect this directly.
    FlagClear(DF_NATIVEFS);
    // Windows and linux doesn't need the scale
#else
    // Update the main fbo viewport size without scale
    cFboMain->DimSet(static_cast<GLsizei>(cInput->GetWindowWidth()),
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
  /* -- Get monitors list ------------------------------------------ */ public:
  const GlFWMonitors &GetMonitors(void) const { return mlData; }
  /* -- Get monitors cound ------------------------------------------------- */
  size_t GetMonitorsCount(void) const { return mlData.size(); }
  /* -- Return selected monitor -------------------------------------------- */
  const GlFWMonitor *GetSelectedMonitor(void) const { return moSelected; }
  /* -- Return selected resolution ----------------------------------------- */
  const GlFWRes *GetSelectedRes(void) const { return rSelected; }
  /* -- Request from alternative thread to resize window ------------------- */
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
  /* -- Request to close window -------------------------------------------- */
  void RequestClose(void) { cEvtWin->Add(EWC_WIN_HIDE); }
  /* -- Set full screen in Window thread ----------------------------------- */
  void SetFullScreen(const bool bState)
  {// Return if setting not different than actual
    if(FlagIsEqualToBool(DF_INFULLSCREEN, bState)) return;
    // Update new fullscreen setting and re-initialise if successful
    ReInitWindow(bState);
    // Update viewport
    cEvtMain->Add(EMC_VID_MATRIX_REINIT);
  }
  /* -- Return current video mode refresh rate ----------------------------- */
  int GetRefreshRate(void) { return rSelected->Refresh(); }
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
    { DimSetWidth(iWidth); return ACCEPT; }
  /* -- Set window height -------------------------------------------------- */
  CVarReturn HeightChanged(const int iHeight)
    { DimSetHeight(iHeight); return ACCEPT; }
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
  { // Return if invalid full-screen mode
    if(iVId < -2) return DENY;
    // Set video mode requested
    stVRequested = static_cast<size_t>(iVId);
    // Accepted
    return ACCEPT;
  }
  /* -- Set monitor number ------------------------------------------------- */
  CVarReturn MonitorChanged(const int iMId)
  { // Return if invalid full-screen mode
    if(iMId < -1) return DENY;
    // Set video mode requested
    stMRequested = static_cast<size_t>(iMId);
    // Accepted
    return ACCEPT;
  }
  /* -- Set window X position ---------------------------------------------- */
  CVarReturn SetXPosition(const int iNewX)
  { // Deny change request if an invalid value was sent
    CoordSetX(iNewX);
    // Apply window position if window is available
    if(cGlFW && cGlFW->WinIsAvailable()) RequestReposition();
    // Success
    return ACCEPT;
  }
  /* -- Set window Y position ---------------------------------------------- */
  CVarReturn SetYPosition(const int iNewY)
  { // Deny change request if an invalid value was sent
    CoordSetY(iNewY);
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
    if(cGlFW && cGlFW->WinIsAvailable() && moSelected->Context()) ApplyGamma();
    // Success
    return ACCEPT;
  }
  /* -- Icon filenames changed (allow blank strings) ------ Core::SetIcon -- */
  CVarReturn SetIcon(const string &strF, string&)
    { return BoolToCVarReturn(strF.empty() || SetIcon(strF)); }
  /* -- Get selected monitor id -------------------------------------------- */
  int GetMonitorId(void) const { return moSelected->Index(); }
  /* -- Get selected video mode id ----------------------------------------- */
  int GetVideoModeId(void) const { return rSelected->Index(); }
  /* -- Init info ---------------------------------------------------------- */
  const string &GetMonitorName(void) const { return moSelected->Name(); }
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
    if(cSystem->IsGraphicalMode())
    { // Ignore if no icons
      if(gilIcons.empty()) return;
      // Capture exceptions and ask GLFW to set the icon
      try { cGlFW->WinSetIcon(UtilIntOrMax<int>(gilIcons.size()),
              gilIcons.data()); }
      // Exception occured? GLFW can throw GLFW_PLATFORM_ERROR on Wayland which
      // is absolutely retarded as is not consistent with other platforms such
      // as MacOS which will silently succeed
      catch(const exception &e)
      { // Just log the error that occured
        cLog->LogWarningExSafe(
          "Display could not load $ icon files due to GlFW exception: $.",
            gilIcons.size(), e.what());
        // Done
        return;
      } // Report that we updated the icons
      cLog->LogInfoExSafe("Display updated $ windows icons." ,
        gilIcons.size());
      // Show details?
      if(cLog->HasLevel(LH_DEBUG))
        for(Image &imC : ilIcons)
        { // Get first icon
          const ImageSlot &imsD = imC.GetSlotsConst().front();
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
    if(cSystem->IsGraphicalMode())
    { // Clear images and icons
      gilIcons.clear();
      ilIcons.clear();
      // Create contiguous memory for glfw icon descriptors and icon data
      gilIcons.reserve(tIcons.size());
      ilIcons.reserve(tIcons.size());
      // Build icons
      for(string &strName : tIcons)
      { // Check filename
        DirVerifyFileNameIsValid(strName);
        // Load icon as RGB 32BPP.
        const Image &imC = ilIcons.emplace_back(
          Image{ StdMove(strName), IL_REVERSE|IL_TORGB|IL_TO32BPP });
        const ImageSlot &imsD = imC.GetSlotsConst().front();
        gilIcons.emplace_back(GLFWimage{
          imsD.DimGetWidth<int>(), imsD.DimGetHeight<int>(),
          imsD.Ptr<unsigned char>() });
      }
    } // Not in interactive mode?
    else
    { // Only Win32 terminal windows can change the icon
#if defined(WINDOWS)
      // Have two icons at least?
      if(tIcons.size() >= 2)
      { // Get string
        // Set small icon from the last icon specified
        string &strFile = tIcons.back();
        const Image imC{ StdMove(strFile), IL_REVERSE|IL_TOBGR };
        const ImageSlot &imsD = imC.GetSlotsConst().front();
        cSystem->SetSmallIcon(imC.IdentGet(), imsD.DimGetWidth(),
          imsD.DimGetHeight(), imC.GetBitsPerPixel(), imsD);
      } // Set large icon from the first icon specified
      string &strFile = tIcons.front();
      const Image imC{ StdMove(strFile), IL_REVERSE|IL_TOBGR };
      const ImageSlot &imsD = imC.GetSlotsConst().front();
      cSystem->SetLargeIcon(imC.IdentGet(), imsD.DimGetWidth(),
        imsD.DimGetHeight(), imC.GetBitsPerPixel(), imsD);
     // Not using windows?
#else
     // Log that we cannot set icons
     cLog->LogDebugSafe("Display cannot set icons on this mode and system.");
     // Windows check?
#endif
    } // Success
    return true;
  }
  /* -- Update icons and refresh icon if succeeded ------------------------- */
  void SetIconFromLua(const string &strNames)
    { if(SetIcon(strNames)) return cEvtWin->Add(EWC_WIN_SETICON); }
  /* -- Get window full-screen type ---------------------------------------- */
  FSType GetFSType(void) const { return fsType; }
  const string &GetFSTypeString(const FSType fsT) const
    { return fstStrings.Get(fsT); }
  const string &GetFSTypeString(void) const { return GetFSTypeString(fsType); }
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
    // Enumerate monitors and video modes
    EnumerateMonitorsAndVideoModes();
    // Inform main fbo class of our transparency setting
    cFboMain->fboMain.SetTransparency(FlagIsSet(DF_TRANSPARENT));
    // We are using the OpenGL 3.2 forward compatible API
    cGlFW->GlFWSetClientAPI(GLFW_OPENGL_API);
    cGlFW->GlFWSetContextVersion(3, 2);
    cGlFW->GlFWSetCoreProfile(GLFW_OPENGL_CORE_PROFILE);
    cGlFW->GlFWSetForwardCompatEnabled();
    cGlFW->GlFWSetRobustness(GLFW_LOSE_CONTEXT_ON_RESET);
    // Set other settings
    cGlFW->GlFWSetDebug(FlagIsSet(DF_DEBUG));
    cGlFW->GlFWSetNoErrors(FlagIsSet(DF_NOERRORS));
    cGlFW->GlFWSetDoubleBufferEnabled();
    cGlFW->GlFWSetSRGBCapable(FlagIsSet(DF_SRGB));
    cGlFW->GlFWSetRefreshRate(GetRefreshRate());
    cGlFW->GlFWSetAuxBuffers(iAuxBuffers);
    cGlFW->GlFWSetMultisamples(iSamples);
    cGlFW->GlFWSetMaximised(FlagIsSet(DF_MAXIMISED));
    cGlFW->GlFWSetStereo(FlagIsSet(DF_STEREO));
    cGlFW->GlFWSetTransparency(cFboMain->fboMain.IsTransparencyEnabled());
    cGlFW->GlFWSetDepthBits(0);
    cGlFW->GlFWSetStencilBits(0);
    // Force custom bit-depth?
    if(iBPPSelected)
      cGlFW->GlFWSetColourDepth(iBPPSelected, iBPPSelected, iBPPSelected,
        cFboMain->fboMain.IsTransparencyEnabled() ? iBPPSelected : 0);
    // Use default? Set regular depth buffer bits
    else cGlFW->GlFWSetColourDepth(rSelected->Red(), rSelected->Green(),
      rSelected->Blue(), cFboMain->fboMain.IsTransparencyEnabled() ? 8 : 0);
    // Set Apple operating system only settings
#if defined(MACOS)
    cGlFW->GlFWSetRetinaMode(FlagIsSet(DF_HIDPI));
    cGlFW->GlFWSetGPUSwitching(FlagIsSet(DF_GASWITCH));
#endif
    // Get window name and use it for frame and instance name. It's assumed
    // that 'cpTitle' won't be freed while using it these two times.
    const char*const cpTitle = cCVars->GetInternalCStrSafe(APP_TITLE);
    cGlFW->GlFWSetFrameName(cpTitle);
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
    // Remove invalidated active flags
    FlagClear(DF_FOCUSED|DF_EXCLUSIVE|DF_INFULLSCREEN|DF_NATIVEFS);
    // Window type deinitialised
    fsType = FST_STANDBY;
    // Deinit window and engine events
    cEvtWin->UnregisterEx(*this);
    cEvtMain->UnregisterEx(*this);
    // Have window?
    if(cGlFW->WinIsAvailable())
    { // If we have monitor?
      if(moSelected)
      { // Restore gamma (this fails if theres no window).
        GlFWSetGamma(moSelected->Context(), 1.0);
        // Monitor no longer valid
        moSelected = nullptr;
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
      moSelected = nullptr;
    } // Clear selected video mode
    rSelected = nullptr;
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
      { EWC_WIN_LIMITS,    bind(&Display::OnLimits,   this, _1) },
      { EWC_WIN_MOVE,      bind(&Display::OnMove,     this, _1) },
      { EWC_WIN_SETICON,   bind(&Display::OnSetIcon,  this, _1) },
      { EWC_WIN_HIDE,      bind(&Display::OnHide,     this, _1) },
    },
    DimCoInt{ -1, -1, 0, 0 },          // Requested position and size
    moSelected(nullptr),               // No monitor selected
    rSelected(nullptr),                // No video mode selected
    stMRequested(StdMaxSizeT),         // No monitor requested
    stVRequested(StdMaxSizeT),         // No video mode id requested
    fGamma(0),                         // Gamma initialised by CVars
    fOrthoWidth(0.0f),                 // Ortho width initialised by CVars
    fOrthoHeight(0.0f),                // Ortho height initialised by CVars
    iBPPSelected(-1),                  // Bit depth not selected yet
    iWinPosX(-1), iWinPosY(-1),        // No initial window position
    iAuxBuffers(0),                    // No auxiliary buffers specified
    iSamples(0),                       // No anti-aliasing samples specified
    fWinScaleWidth(1.0f),              // No initial scale width
    fWinScaleHeight(1.0f),             // No initial scale height
    lrFocused{ "OnFocused" },          // Set name for OnFocused lua event
    fsType(FST_STANDBY),               // Full-screen type
    fstStrings{{                       // Init full-screen type strings
      STR(FST_STANDBY),    STR(FST_WINDOW), STR(FST_EXCLUSIVE),
      STR(FST_BORDERLESS), STR(FST_NATIVE)
    }, "FST_UNKNOWN"}                  // End of full-screen type strings list
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Display, DeInit())
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Display)             // Do not need copy defaults
  /* ----------------------------------------------------------------------- */
} *cDisplay = nullptr;                 // Pointer to static class
/* -- Monitor changed static event ----------------------------------------- */
void Display::OnMonitorStatic(GLFWmonitor*const mA, const int iA)
  { cDisplay->OnMonitor(mA, iA); }
/* ------------------------------------------------------------------------- */
}                                      // End of public module namespace
/* ------------------------------------------------------------------------- */
}                                      // End of private module namespace
/* == EoF =========================================================== EoF == */
