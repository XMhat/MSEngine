/* == INPUT.HPP ============================================================ */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## This module handles the keyboard and controller input.              ## */
/* ######################################################################### */
/* ========================================================================= */
#pragma once                           // Only one incursion allowed
/* -- Module namespace ----------------------------------------------------- */
namespace IfInput {                    // Keep declarations neatly categorised
/* -- Includes ------------------------------------------------------------- */
using namespace IfConsole;             // Using console interface
using namespace IfEvtWin;              // Using window event manager interface
/* == Input flags ========================================================== */
BUILD_FLAGS(Input,
  /* ----------------------------------------------------------------------- */
  // No flags                          Mouse cursor is enabled?
  IF_NONE                {0x00000000}, IF_CURSOR              {0x00000001},
  // Full-screen toggler enabled?      Mouse cursor has focus?
  IF_FSTOGGLER           {0x00000002}, IF_MOUSEFOCUS          {0x00000004}
);/* -- Defines ------------------------------------------------------------ */
#define JC_JOYSTICKS (GLFW_JOYSTICK_LAST+1)       // Maximum joysticks
#define JC_AXISES    (GLFW_GAMEPAD_AXIS_LAST+1)   // Maximum axises per joy
#define JC_BUTTONS   (GLFW_GAMEPAD_BUTTON_LAST+1) // Maximum buttons per joy
/* -- Axis class ----------------------------------------------------------- */
class JoyAxisInfo
{ /* -------------------------------------------------------------- */ private:
  const int        iId;                // Axis unique identifier
  float            fDeadZoneR;         // Reverse deadzone threshold
  float            fDeadZoneF;         // Forward deadzone threshold
  float            fUnbuffered;        // Current unbuffered axis press state
  int              iBuffered;          // Current buffered axis press state
  /* -- Get axis identifier ---------------------------------------- */ public:
  int AxisGetId(void) const { return iId; }
  /* -- Get reverse deadzone value ----------------------------------------- */
  float AxisGetReverseDeadZone(void) const { return fDeadZoneR; }
  /* -- Get forward deadzone value ----------------------------------------- */
  float AxisGetForwardDeadZone(void) const { return fDeadZoneF; }
  /* -- Set reverse deadzone value ----------------------------------------- */
  void AxisSetReverseDeadZone(const float fDZ) { fDeadZoneR = -fDZ; }
  /* -- Set forward deadzone value ----------------------------------------- */
  void AxisSetForwardDeadZone(const float fDZ) { fDeadZoneF = fDZ; }
  /* -- Get unbuffered state ----------------------------------------------- */
  float AxisGetUnbufferedState(void) const { return fUnbuffered; }
  /* -- Get buffered state ------------------------------------------------- */
  int AxisGetBufferedState(void) const { return iBuffered; }
  /* -- Clear button state ------------------------------------------------- */
  void AxisClearState(void) { fUnbuffered = 0.0; iBuffered = GLFW_RELEASE; }
  /* -- Set axis state ----------------------------------------------------- */
  void AxisSetState(const float*const fpData)
  { // Get the axis reading and if it's moving negatively?
    fUnbuffered = fpData[AxisGetId()];
    if(fUnbuffered < fDeadZoneR)
    { // Released or already in reverse position?
      if(iBuffered <= GLFW_RELEASE)
      { // Set to -GLFW_REPEAT if currently -GLFW_PRESSED or
        //        -GLFW_PRESSED if currently GLFW_RELEASE
        if(iBuffered > -GLFW_REPEAT) --iBuffered;
        // State changed
        return;
      }
    } // Moving positively?
    else if(fUnbuffered > fDeadZoneF)
    { // Released or already in forward position?
      if(iBuffered >= GLFW_RELEASE)
      { // Set to GLFW_REPEAT if currentl GLFW_PRESSED or
        //        GLFW_PRESSED if currently GLFW_RELEASE
        if(iBuffered < GLFW_REPEAT) ++iBuffered;
        // State changed
        return;
      }
    } // Nothing pressed so reset buffered counter
    iBuffered = GLFW_RELEASE;
  }
  /* -- Constructor with unique id ----------------------------------------- */
  explicit JoyAxisInfo(const int iNId) :
    /* -- Initialisation of members ---------------------------------------- */
    iId(iNId),                         // Set unique id
    fDeadZoneR(-0.25),                 // Set default reverse deadzone
    fDeadZoneF(0.25),                  // Set default forward deadzone
    fUnbuffered(0.0),                  // Set default unbuffered state
    iBuffered(GLFW_RELEASE)            // Set default state
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
typedef array<JoyAxisInfo, JC_AXISES> JoyAxisList; // Axis data list type
/* -- Button class --------------------------------------------------------- */
class JoyButtonInfo
{ /* -- Private variables ----------------------------------------- */ private:
  const int        iId;                // Button unique identifier
  unsigned int     uiUnbuffered;       // Current unbuffered butn press state
  unsigned int     uiBuffered;         // Current buffered butn press state
  /* -- Get button id ---------------------------------------------- */ public:
  int ButtonGetId(void) const { return iId; }
  /* -- Get unbuffered state (GLFW_RELEASE or GLFW_PRESS) ------------------ */
  unsigned int ButtonGetUnbufferedState(void) const { return uiUnbuffered; }
  /* -- Get buffered state (GLFW_RELEASE, GLFW_PRESS or GLFW_REPEAT) ------- */
  unsigned int ButtonGetBufferedState(void) const { return uiBuffered; }
  /* -- Clear button state ------------------------------------------------- */
  void ButtonClearState(void) { uiUnbuffered = uiBuffered = GLFW_RELEASE; }
  /* -- Set button state --------------------------------------------------- */
  void ButtonSetState(const unsigned char*const ucpState)
  { // Save unbuffered state and compare it
    uiUnbuffered = static_cast<unsigned int>(ucpState[ButtonGetId()]);
    switch(uiUnbuffered)
    { // Button is released?
      case GLFW_RELEASE:
        // Reset button state if not released (0 = released)
        if(uiBuffered > GLFW_RELEASE) uiBuffered = GLFW_RELEASE;
        // Done
        return;
      // Button is pressed? Increase button state (1=pressed and 2=held)
      case GLFW_PRESS:
        // Set GLFW_PRESSED if GLFW_RELEASED or
        //     GLFW_REPEAT if GLFW_PRESSED.
        if(uiBuffered < GLFW_REPEAT) ++uiBuffered;
        // Done
        return;
      // Shouldn't get here
      default: break;
    } // Reset to released
    uiBuffered = GLFW_RELEASE;
  }
  /* -- Constructor with unique id ----------------------------------------- */
  explicit JoyButtonInfo(const int iNId) :
    /* -- Initialisation of members ---------------------------------------- */
    iId(iNId),                         // Set unique id
    uiUnbuffered(GLFW_RELEASE),        // Set default unbuffered state
    uiBuffered(GLFW_RELEASE)           // Set default buffered state
    /* -- No code ---------------------------------------------------------- */
    { }
};/* ----------------------------------------------------------------------- */
typedef array<JoyButtonInfo, JC_BUTTONS> JoyButtonList; // Button data list typ
/* -- Joystick type typedef ------------------------------------------------ */
BUILD_FLAGS(Joy,
  /* ----------------------------------------------------------------------- */
  // No flags                          Joystick is connnected?
  JF_NONE                (0x00000000), JF_CONNECTED           (0x00000001),
  // Joystick is actually a gamepad
  JF_GAMEPAD             (0x00000002)
);/* -- Joystick class ----------------------------------------------------- */
class JoyInfo :
  /* -- Derived classes ---------------------------------------------------- */
  public JoyFlags,                     // Joystick flags
  private JoyAxisList,                 // Joystick axises data
  private JoyButtonList,               // Joystick buttons data
  public Ident                         // Joystick identifier
{ /* -------------------------------------------------------------- */ private:
  const int      iId;                  // Unique identifier of this
  /* -- Unbuffered and buffered axis data struct --------------------------- */
  size_t         stAxises;             // Axis count
  size_t         stButtons;            // Button count
  /* -- Return joystick id ----------------------------------------- */ public:
  int GetId(void) const { return iId; }
  /* -- Get button list data ----------------------------------------------- */
  JoyButtonList &GetButtonList(void)
    { return static_cast<JoyButtonList&>(*this); }
  const JoyButtonList &GetConstButtonList(void) const
    { return static_cast<const JoyButtonList&>(*this); }
  /* -- Clear button state ------------------------------------------------- */
  void ClearButtonState(void)
    { MYFOREACH(par_unseq, GetButtonList().begin(), GetButtonList().end(),
        [](JoyButtonInfo& jbiItem) { jbiItem.ButtonClearState(); }); }
  /* -- Get button count ------------------------------------------------- */
  size_t GetButtonCount(void) const { return stButtons; }
  /* -- Refresh button data ---------------------------------------------- */
  void RefreshButtons(void)
  { // Get joystick buttons and if found?
    int iButtons;
    if(const unsigned char*const cpData =
      GlFW::GetJoystickButtons(GetId(), iButtons))
    { // Clamp count to number we support on the stack
      stButtons = Minimum(static_cast<size_t>(iButtons),
        GetConstButtonList().size());
      // Return if no buttons
      if(!stButtons) return;
      // Enumate state for each joy button
      for(size_t stBId = 0; stBId < stButtons; ++stBId)
        GetButtonList()[stBId].ButtonSetState(cpData);
    }
  }
  /* -- Get axis list data ------------------------------------------------- */
  JoyAxisList &GetAxisList(void)
    { return static_cast<JoyAxisList&>(*this); }
  const JoyAxisList &GetConstAxisList(void) const
    { return static_cast<const JoyAxisList&>(*this); }
  /* -- Clear axis state ------------------------------------------------- */
  void ClearAxisState(void)
    { MYFOREACH(par_unseq, GetAxisList().begin(), GetAxisList().end(),
        [](JoyAxisInfo &jaiItem) { jaiItem.AxisClearState(); }); }
  /* -- Get axis count --------------------------------------------------- */
  size_t GetAxisCount(void) const { return stAxises; }
  /* -- Refresh axis data for this --------------------------------------- */
  void RefreshAxes(void)
  { // Get axis data and if succeeded?
    int iAxises;
    if(const float*const fpData = GlFW::GetJoystickAxes(GetId(), iAxises))
    { // Clamp count to number we support on the stack
      stAxises = Minimum(static_cast<size_t>(iAxises),
        GetConstAxisList().size());
      // Return if no axis
      if(!stAxises) return;
      // Enumate state for each joystick axis
      for(size_t stAId = 0; stAId < stAxises; ++stAId)
        GetAxisList()[stAId].AxisSetState(fpData);
    }
  }
  /* -- Refresh data ----------------------------------------------------- */
  void RefreshData(void) { RefreshAxes(); RefreshButtons(); }
  /* -- Return type of joystick ------------------------------------------ */
  const char *GetGamepadOrJoystickString(void) const
    { return FlagIsSetTwo(JF_GAMEPAD, "gamepad", "joystick"); }
  /* -- Joystick is connected? ----------------------------------- */ public:
  bool IsConnected(void) const { return FlagIsSet(JF_CONNECTED); }
  /* -- Joystick is disconnected? ---------------------------------------- */
  bool IsDisconnected(void) const { return !IsConnected(); }
  /* -- Clear button state if connected ---------------------------------- */
  void ClearButtonStateIfConnected(void)
    { if(IsConnected()) ClearButtonState(); }
  /* -- Clear buttons and axis state ------------------------------------- */
  void ClearState(void) { ClearAxisState(); ClearButtonState(); }
  /* -- Refresh data if connected ---------------------------------------- */
  void RefreshDataIfConnected() { if(IsConnected()) RefreshData(); }
  /* -- Get button state ------------------------------------------------- */
  unsigned int GetButtonState(const size_t stButtonId) const
    { return GetConstButtonList()[stButtonId].ButtonGetBufferedState(); }
  /* -- Get axis state --------------------------------------------------- */
  int GetAxisState(const size_t stAxisId) const
    { return GetConstAxisList()[stAxisId].AxisGetBufferedState(); }
  /* -- Get unbuffered axis state ---------------------------------------- */
  float GetUnbufferedAxisState(const size_t stAxisId) const
    { return GetConstAxisList()[stAxisId].AxisGetUnbufferedState(); }
  /* -- Get axis state --------------------------------------------------- */
  void SetAxisForwardDeadZone(const size_t stAxisId, const float fDZ)
    { GetAxisList()[stAxisId].AxisSetForwardDeadZone(fDZ); }
  void SetAxisReverseDeadZone(const size_t stAxisId, const float fDZ)
    { GetAxisList()[stAxisId].AxisSetReverseDeadZone(fDZ); }
  void SetAxisDeadZones(const size_t stAxisId,
    const float fFDZ, const float fRDZ)
  { // Get axis data and set the new values
    JoyAxisInfo &jaiItem = GetAxisList()[stAxisId];
    jaiItem.AxisSetForwardDeadZone(fFDZ);
    jaiItem.AxisSetReverseDeadZone(fRDZ);
  }
  /* -- Set default positive deadzone -------------------------------------- */
  void SetReverseDeadZone(const float fDZ)
    { MYFOREACH(par_unseq, GetAxisList().begin(), GetAxisList().end(),
        [fDZ](JoyAxisInfo &jaiItem)
          { jaiItem.AxisSetReverseDeadZone(fDZ); }); }
  /* -- Set default positive deadzone -------------------------------------- */
  void SetForwardDeadZone(const float fDZ)
    { MYFOREACH(par_unseq, GetAxisList().begin(), GetAxisList().end(),
        [fDZ](JoyAxisInfo &jaiItem)
          { jaiItem.AxisSetForwardDeadZone(fDZ); }); }
  /* -- Get/Set gamepad status ------------------------------------------- */
  void Connect(void)
  { // Now connected
    FlagSet(JF_CONNECTED);
    // Set gamepad status
    FlagSetOrClear(JF_GAMEPAD, glfwJoystickIsGamepad(GetId()));
    // Get name of joystick and set a generic one if invalid
    IdentSet(IfNull(glfwGetJoystickName(GetId())));
    // Refresh joystick data
    RefreshData();
    // We gained this joystick
    LW(LH_INFO, "Input detected $ '$' (I:$;B:$;A:$).",
      GetGamepadOrJoystickString(), IdentGet(), GetId(), GetButtonCount(),
      GetAxisCount());
    // Return if it's not a gamepad
    if(FlagIsClear(JF_GAMEPAD)) return;
    // Report name and identifier
    if(const char*const cpN = glfwGetGamepadName(GetId()))
      LW(LH_DEBUG, "- Gamepad Name: $.", cpN);
    if(const char*const cpG = glfwGetJoystickGUID(GetId()))
      LW(LH_DEBUG, "- Gamepad Identifier: $.", cpG);
  }
  /* -- Reset data ------------------------------------------------------- */
  void Disconnect(void)
  { // Ignore if already disconnected
    if(IsDisconnected()) return;
    // No longer connected
    FlagClear(JF_CONNECTED);
    // We lost the specified joystick
    LW(LH_INFO, "Input disconnected $ '$' (I:$).",
      GetGamepadOrJoystickString(), IdentGet(), GetId());
  }
  /* -- Detect joystick ---------------------------------------------------- */
  bool IsPresent(void) { return glfwJoystickPresent(GetId()); }
  /* -- Constructor -------------------------------------------------------- */
  explicit JoyInfo(const int iNId) :
    /* -- Initialisation of members ---------------------------------------- */
    JoyFlags(JF_NONE),                 // Set no flags
    /* -- Initialise joystick axises --------------------------------------- */
    JoyAxisList{ {
      JoyAxisInfo(GLFW_GAMEPAD_AXIS_LEFT_X),       // Axis 0 of 6
      JoyAxisInfo(GLFW_GAMEPAD_AXIS_LEFT_Y),       // Axis 1 of 6
      JoyAxisInfo(GLFW_GAMEPAD_AXIS_RIGHT_X),      // Axis 2 of 6
      JoyAxisInfo(GLFW_GAMEPAD_AXIS_RIGHT_Y),      // Axis 3 of 6
      JoyAxisInfo(GLFW_GAMEPAD_AXIS_LEFT_TRIGGER), // Axis 4 of 6
      JoyAxisInfo(GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER) // Axis 5 of 6
    } },
    /* -- Initialise joystick buttons -------------------------------------- */
    JoyButtonList{ {
      JoyButtonInfo(GLFW_GAMEPAD_BUTTON_A),
      JoyButtonInfo(GLFW_GAMEPAD_BUTTON_B),
      JoyButtonInfo(GLFW_GAMEPAD_BUTTON_X),
      JoyButtonInfo(GLFW_GAMEPAD_BUTTON_Y),
      JoyButtonInfo(GLFW_GAMEPAD_BUTTON_LEFT_BUMPER),
      JoyButtonInfo(GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER),
      JoyButtonInfo(GLFW_GAMEPAD_BUTTON_BACK),
      JoyButtonInfo(GLFW_GAMEPAD_BUTTON_START),
      JoyButtonInfo(GLFW_GAMEPAD_BUTTON_GUIDE),
      JoyButtonInfo(GLFW_GAMEPAD_BUTTON_LEFT_THUMB),
      JoyButtonInfo(GLFW_GAMEPAD_BUTTON_RIGHT_THUMB),
      JoyButtonInfo(GLFW_GAMEPAD_BUTTON_DPAD_UP),
      JoyButtonInfo(GLFW_GAMEPAD_BUTTON_DPAD_RIGHT),
      JoyButtonInfo(GLFW_GAMEPAD_BUTTON_DPAD_DOWN),
      JoyButtonInfo(GLFW_GAMEPAD_BUTTON_DPAD_LEFT)
    } },
    /* -- Other initialisers ----------------------------------------------- */
    iId(iNId),                         // Set unique joystick id
    stAxises(0),                       // Set no axies
    stButtons(0)                       // Set no buttons
    /* -- No code ---------------------------------------------------------- */
    { }
};/* -- State typedefs --------------------------------------------------- */
typedef array<JoyInfo, JC_JOYSTICKS> JoyList; // Actual joystick data
typedef JoyList::const_iterator    JoyListIt; // Iterator for vector of joys
/* == Input class ========================================================== */
static class Input :                   // Handles keyboard, mouse & controllers
  /* -- Base classes ------------------------------------------------------- */
  private IHelper,                     // Initialsation helper
  public InputFlags,                   // Input configuration settings
  private JoyList,                     // Joystick data
  private EvtMain::RegVec              // Events list to register
{ /* -------------------------------------------------------------- */ private:
  enum JoyStatus { JOY_DETECT=-1, JOY_DISABLE, JOY_ENABLE };
  /* -- Console ------------------------------------------------------------ */
  int              iConsoleKey1;       // First console key
  int              iConsoleKey2;       // Second console key
  /* -- Events ----------------------------------------------------- */ public:
  LuaFunc          lfOnMouseClick;     // Mouse button clicked
  LuaFunc          lfOnMouseMove;      // Mouse cursor moved
  LuaFunc          lfOnMouseScroll;    // Mouse scroll wheel moved
  LuaFunc          lfOnMouseFocus;     // Mouse focus changed from window
  LuaFunc          lfOnKey;            // Unfiltered key pressed
  LuaFunc          lfOnChar;           // Filtered key pressed
  LuaFunc          lfOnDragDrop;       // Drag and dropped files
  LuaFunc          lfOnJoyState;       // Joystick connection event
  /* -------------------------------------------------------------- */ private:
  JoyStatus        jsStatus;           // Joystick detection/polling state
  /* ----------------------------------------------------------------------- */
  int              iWinWidth;          // Actual window width
  int              iWinHeight;         // Actual window height
  int              iWinWidthD2;        // Actual half window width
  int              iWinHeightD2;       // Actual half window height
  /* -- Set cursor position ------------------------------------------------ */
  void OnSetCurPos(const EvtMain::Cell &epData)
  { // This function is called on an event sent by cEvtMain->Add. It it assume
    // to send only two parameters which is the newly requested X and Y
    // position. Glfw says that SetCursorPos should only be used in the Window
    // thread, so this is why it can't be an on-demand call.
    // More information:- https://www.glfw.org/docs/3.1/group__input.html
    // Calculate new position based on main fbo ortho matrix.
    const float
      fAdjX = (epData.vParams[0].f - cFboMain->fboMain.fcStage.GetCoLeft()) /
        cFboMain->fboMain.GetCoRight() * GetWindowWidth(),
      fAdjY = (epData.vParams[1].f - cFboMain->fboMain.fcStage.GetCoTop()) /
        cFboMain->fboMain.GetCoBottom() * GetWindowHeight(),
      // Clamp the new position to the window bounds.
      fNewX = Clamp(fAdjX, 0.0f, cFboMain->fboMain.GetCoRight() - 1.0f),
      fNewY = Clamp(fAdjY, 0.0f, cFboMain->fboMain.GetCoBottom() - 1.0f);
    // Now translate that position back into the actual window cursor pos.
    cGlFW->SetCursorPos(static_cast<double>(fNewX),
                        static_cast<double>(fNewY));
  }
  /* -- Filtered key pressed ----------------------------------------------- */
  void OnFilteredKey(const EvtMain::Cell &epData)
  { // Get key pressed
    const unsigned int uiKey = epData.vParams[1].ui;
    // If console is enabled, send it to console instead
    if(cConsole->IsVisible()) return cConsole->OnCharPress(uiKey);
    // Else send the key to lua callbacks
    lfOnChar.LuaFuncDispatch(uiKey);
  }
  /* -- Mouse went inside the window --------------------------------------- */
  void OnMouseFocus(const EvtMain::Cell &epData)
  { // Get and check state
    const int iState = epData.vParams[1].i;
    switch(iState)
    { // Mouse is in the window? Set mouse in window flag
      case GLFW_TRUE: FlagSet(IF_MOUSEFOCUS); break;
      // Mouse is not in the window? Clear mouse in window flag
      case GLFW_FALSE: FlagClear(IF_MOUSEFOCUS); break;
      // Unknown state? Log the bad mouse focus state and return
      default: LW(LH_WARNING, "Input ignored bad mouse focus state $<$$>!",
                 iState, hex, iState); return;
    } // Dispatch event to lua scripts
    return lfOnMouseFocus.LuaFuncDispatch(iState);
  }
  /* -- Mouse wheel scroll ------------------------------------------------- */
  void OnMouseWheel(const EvtMain::Cell &epData)
  { // Set event to lua callbacks
    lfOnMouseScroll.LuaFuncDispatch(epData.vParams[1].d, epData.vParams[2].d);
  }
  /* -- Mouse button clicked ----------------------------------------------- */
  void OnMouseClick(const EvtMain::Cell &epData)
  { // Set event to lua callbacks
    lfOnMouseClick.LuaFuncDispatch(epData.vParams[1].i,
                                   epData.vParams[2].i,
                                   epData.vParams[3].i);
  }
  /* -- Mouse button clicked ----------------------------------------------- */
  void OnMouseMove(const EvtMain::Cell &epData)
  { // Recalculate cursor position based on framebuffer size and send the
    // new co-ordinates to the lua callback handler
    lfOnMouseMove.LuaFuncDispatch(
      static_cast<double>(cFboMain->fboMain.fcStage.GetCoLeft()) +
        ((epData.vParams[1].d/GetWindowWidth()) *
        static_cast<double>(cFboMain->fboMain.GetCoRight())),
      static_cast<double>(cFboMain->fboMain.fcStage.GetCoTop()) +
        ((epData.vParams[2].d/GetWindowHeight()) *
        static_cast<double>(cFboMain->fboMain.GetCoBottom())));
  }
  /* -- Unfiltered key pressed --------------------------------------------- */
  void OnKeyPress(const EvtMain::Cell &epData)
  { // Get key code and ignore if unknown key
    const int iKey = epData.vParams[1].i;
    if(iKey == GLFW_KEY_UNKNOWN) return;
    // Get modifier and actions and if Alt+Enter was pressed or released and
    // full-screen toggler enabled?
    const int iState = epData.vParams[3].i, iMod = epData.vParams[4].i;
    // Check for full-screen/window toggle press
    if(iKey == GLFW_KEY_ENTER &&       // If the ENTER key was pressed? and
       iMod == GLFW_MOD_ALT &&         // ...ALT key is held? and
       FlagIsSet(IF_FSTOGGLER))        // ...FullScreen key enabled?
    { // Toggle full-screen if key released
      if(iState == GLFW_RELEASE) cEvtWin->AddUnblock(EWC_WIN_TOGGLE_FS);
      // Just being held so ignore further presses so the guest cannot
      // interpret unintended return keypresses.
      return;
    } // Console is enabled?
    if(cConsole->IsVisible())
    { // Add normal key pressed. Since GLFW inconveniently gives us 3 int
      // parameters, we need to pack 2 ints together. Luckily, GLFW_RELEASE etc
      // is only 8-bit, we'll pack the modifiers with this value.
      cConsole->OnKeyPress(iKey, iState, iMod);
      // We handled this key so do not dispatch it to scripts
      return;
    } // Ignore the ESCAPE generated from hiding the console
    if(cConsole->FlagIsSet(CF_IGNOREESC) &&
       iState == GLFW_RELEASE && !iMod && iKey == GLFW_KEY_ESCAPE)
    { // Remove flag
      cConsole->FlagClear(CF_IGNOREESC);
      // We handled this key so do not dispatch it to scripts
      return;
    } // If key was pressed and is console key and no modifiers are pressed?
    if(iState == GLFW_PRESS && !iMod &&
      (iKey == iConsoleKey1 || iKey == iConsoleKey2))
    { // Set console enabled and if enabled? Ignore first key as registering
      // OnCharPress will trigger this keystroke and print it out in the
      // console.
      if(cConsole->SetVisible(true)) cConsole->FlagSet(CF_IGNOREKEY);
      // We handled this key so do not dispatch it to scripts
      return;
    } // Send lua event for key
    lfOnKey.LuaFuncDispatch(iKey, iState, iMod, epData.vParams[2].i);
  }
  /* -- Files dragged and dropped on window--------------------------------- */
  void OnDragDrop(const EvtMain::Cell &)
  { // Get files and return if empty
    StrVector &vFiles = cGlFW->GetFiles();
    if(vFiles.empty()) return;
    // Send off the event to lua callbacks
    lfOnDragDrop.LuaFuncDispatch(vFiles);
    // Clear the list
    vFiles.clear();
    vFiles.shrink_to_fit();
  }
  /* -- Joystick state changed --------------------------------------------- */
  void OnJoyState(const EvtMain::Cell &epData)
  { // Get joystick id as int
    const int iId = epData.vParams[0].i;
    // What happened to the joystick?
    switch(const int iState = epData.vParams[1].i)
    { // Connected? Setup joystick and return
      case GLFW_CONNECTED:
        return SetupJoystickAndDispatch(static_cast<size_t>(iId));
      // Disconnected?
      case GLFW_DISCONNECTED:
        return ClearJoystickAndDispatch(static_cast<size_t>(iId));
      // Invalid code? Log the bad joystick state and return
      default: LW(LH_WARNING, "Input ignored bad joystick state ($ = $$)!",
                 iId, hex, iState); return;
    }
  }
  /* -- Window past event--------------------------------------------------- */
  void OnWindowPaste(const EvtMain::Cell&)
  { // Get text in clipboard
    Decoder utfString(cGlFW->GetClipboard());
    // For each character, ddd the character to queue if valid
    while(const unsigned int uiChar = utfString.Next())
      if(uiChar >= 32) cConsole->OnCharPress(uiChar);
  }
  /* -- Handle a deadzone change ------------------------------------------- */
  CVarReturn SetDefaultJoyDZ(const float fDZ,
    const function<void(JoyInfo&)> &fcbCallBack)
  { // Bail if invalid deadzone
    if(fDZ > 1) return DENY;
    // Set it
    MYFOREACH(par_unseq, GetJoyList().begin(),
      GetJoyList().end(), fcbCallBack);
    // Success
    return ACCEPT;
  }
  /* -- Update half window ------------------------------------------------- */
  void UpdateWindowSizeD2(void) { iWinWidthD2 = GetWindowWidth()/2;
                                  iWinHeightD2 = GetWindowHeight()/2; }
  /* -- Set default negative deadzone ------------------------------ */ public:
  CVarReturn SetDefaultJoyRevDZ(const float fNewDeadZone)
    { return SetDefaultJoyDZ(fNewDeadZone, [fNewDeadZone](JoyInfo &jiItem)
        { jiItem.SetReverseDeadZone(fNewDeadZone); }); }
  /* -- Set default positive deadzone -------------------------------------- */
  CVarReturn SetDefaultJoyFwdDZ(const float fNewDeadZone)
    { return SetDefaultJoyDZ(fNewDeadZone, [fNewDeadZone](JoyInfo &jiItem)
        { jiItem.SetForwardDeadZone(fNewDeadZone); }); }
  /* -- Set first console key ---------------------------------------------- */
  CVarReturn SetConsoleKey1(const int iK)
    { return CVarSimpleSetIntNG(iConsoleKey1, iK, GLFW_KEY_LAST); }
  /* -- Set second console key --------------------------------------------- */
  CVarReturn SetConsoleKey2(const int iK)
    { return CVarSimpleSetIntNG(iConsoleKey2, iK, GLFW_KEY_LAST); }
  /* -- Set full screen toggler -------------------------------------------- */
  CVarReturn SetFSTogglerEnabled(const bool bState)
    { FlagSetOrClear(IF_FSTOGGLER, bState); return ACCEPT; }
  /* -- Commit visibility of mouse cursor ---------------------------------- */
  void CommitCursor(void) { cGlFW->SetCursor(FlagIsSet(IF_CURSOR)); }
  /* -- Set visibility of mouse cursor ------------------------------------- */
  void SetCursor(const bool bEnabled)
  { // Set member var incase window needs to re-init so we can restore the
    // cursor state
    FlagSetOrClear(IF_CURSOR, bEnabled);
    // Commit cursor
    CommitCursor();
  }
  /* -- Get button list data ----------------------------------------------- */
  JoyList &GetJoyList(void)
    { return static_cast<JoyList&>(*this); }
  const JoyList &GetConstJoyList(void) const
    { return static_cast<const JoyList&>(*this); }
  /* -- Get window size ---------------------------------------------------- */
  int GetWindowWidth(void) const { return iWinWidth; }
  int GetWindowHeight(void) const { return iWinHeight; }
  /* -- Update window size from actual glfw window ------------------------- */
  void UpdateWindowSize(void)
  { // Get new window size
    cGlFW->GetWindowSize(iWinWidth, iWinHeight);
    // Update half window size
    UpdateWindowSizeD2();
  }
  /* -- Update window size (from display) ---------------------------------- */
  void SetWindowSize(const int iX, const int iY)
  { // Update new size
    iWinWidth = iX;
    iWinHeight = iY;
    // Update half size
    UpdateWindowSizeD2();
  }
  /* -- Get cursor position ------------------------------------------------ */
  void GetCursorPos(double &dX, double &dY) const
  { // Get the cursor position
    cGlFW->GetCursorPos(dX, dY);
    // Translate cursor position to framebuffer aspect
    dX = static_cast<double>(cFboMain->fboMain.fcStage.GetCoLeft()) +
      ((dX/GetWindowWidth()) *
        static_cast<double>(cFboMain->fboMain.GetCoRight()));
    dY = static_cast<double>(cFboMain->fboMain.fcStage.GetCoTop()) +
      ((dY/GetWindowHeight()) *
        static_cast<double>(cFboMain->fboMain.GetCoBottom()));
  }
  /* -- Forcefully move the cursor ----------------------------------------- */
  void SetCursorPos(const double dX, const double dY)
    { cEvtMain->Add(EMC_INP_SET_CUR_POS, dX, dY); }
  void SetCursorCentre(void)
    { SetCursorPos(static_cast<double>(iWinWidthD2),
                   static_cast<double>(iWinHeightD2)); }
  /* -- Joystick main tick ------------------------------------------------- */
  void PollJoysticks(void)
  { // If joystick enabled? Enumerate joysticks and refresh data
    if(jsStatus != JOY_DISABLE)
      for(JoyInfo &jsData : GetJoyList())
        jsData.RefreshDataIfConnected();
  }
  bool JoystickExists(const size_t stId)
    { return GetJoyData(stId).IsConnected(); }
  /* -- DeInitialise a joystick -------------------------------------------- */
  void ClearJoystickAndDispatch(const size_t stJoystickId)
  { // Get joystick data and ign ore if joystick wasn't originally connected
    JoyInfo &jsData = GetJoyData(stJoystickId);
    if(jsData.IsDisconnected()) return;
    // Send lua event to let guest know joystick was disconnected
    lfOnJoyState.LuaFuncDispatch(static_cast<lua_Integer>(stJoystickId),
      false);
    // Clear joystick, axis and button data
    jsData.Disconnect();
  }
  /* -- Initialise a new joystick ------------------------------------------ */
  void SetupJoystickAndDispatch(const size_t stJoystickId)
  { // Detect if is gamepad?
    GetJoyData(stJoystickId).Connect();
    // Send lua event to let guest know joystick was connected and return
    lfOnJoyState.LuaFuncDispatch(static_cast<lua_Integer>(stJoystickId), true);
  }
  /* -- Return a joystick is present? -------------------------------------- */
  void AutoDetectJoystick(void)
  { // Joysticks detected count
    size_t stCount = 0;
    // Enumerate joysticks...
    for(JoyInfo &jsData : GetJoyList())
      // If joystick is present?
      if(jsData.IsPresent())
      { // Send event that the joystick was connected
        cEvtMain->Add(EMC_INP_JOY_STATE,
          jsData.GetId(), GLFW_CONNECTED);
        ++stCount;
      } // Send event that the joystick was disconnected
      else cEvtMain->Add(EMC_INP_JOY_STATE,
        jsData.GetId(), GLFW_DISCONNECTED);
    // If we did not find joysticks? Disable polling
    if(!stCount)
    { // If smart detection was enabled?
      if(jsStatus == JOY_DETECT)
      { // Display joystick polling to save on CPU resources
        jsStatus = JOY_DISABLE;
        // Log result
        LW(LH_INFO,
          "Input disabling joystick polling as no devices are detected.");
      } // Done
      return;
    } // Poll joysticks
    PollJoysticks();
    // Done if smart detection was not enabled
    if(jsStatus != JOY_DETECT) return;
    // Joystick polling set to enabled
    jsStatus = JOY_ENABLE;
    // Log result
    LW(LH_INFO,
      "Input enabling joystick polling as $ devices are detected.", stCount);
  }
  // -- CVar callback to toggle raw mouse ---------------------------------- */
  CVarReturn SetRawMouseEnabled(const bool bState)
  { // Ignore changing flags right now if not initialised
    if(IHIsNotInitialised()) return ACCEPT;
    // If raw mouse support is supported?
    if(!GlFW::IsRawMouseMotionSupported())
    { // Log that mouse support is not supported
      LW(LH_WARNING, "Input raw mouse support is not available!");
      return ACCEPT;
    } // Set the new input if we can and log status
    cGlFW->SetRawMouseMotion(bState);
    LW(LH_DEBUG, "Input updated raw mouse status to $.",
      TrueOrFalse(cGlFW->GetRawMouseMotion()));
    // CVar allowed to be set
    return ACCEPT;
  }
  // -- CVar callback to toggle sticky keys -------------------------------- */
  CVarReturn SetStickyKeyEnabled(const bool bState)
  { // Ignore changing flags right now if not initialised
    if(IHIsNotInitialised()) return ACCEPT;
    // Set the new input if we can and log status
    cGlFW->SetStickyKeys(bState);
    LW(LH_DEBUG, "Input updated sticky keys status to $.",
      TrueOrFalse(cGlFW->GetStickyKeys()));
    // CVar allowed to be set
    return ACCEPT;
  }
  // -- CVar callback to toggle sticky mouse ------------------------------- */
  CVarReturn SetStickyMouseEnabled(const bool bState)
  { // Ignore changing flags right now if not initialised
    if(IHIsNotInitialised()) return ACCEPT;
    // Set the new input if we can and log status
    cGlFW->SetStickyMouseButtons(bState);
    LW(LH_DEBUG, "Input updated sticky mouse status to $.",
      TrueOrFalse(cGlFW->GetStickyMouseButtons()));
    // CVar allowed to be set
    return ACCEPT;
  }
  // -- CVar callback to toggling joystick polling ------------------------- */
  CVarReturn SetJoystickEnabled(const int iState)
  { // If input already initialised? Get new joystick setting
    if(IHIsInitialised() && cGlFW) switch(iState)
    { // Set to detection mode or enable? Redetect joysticks
      case JOY_DETECT: case JOY_ENABLE: AutoDetectJoystick(); break;
      // Disable it? Set to disabled and clear joystick states
      case JOY_DISABLE: ClearJoystickButtons(); break;
      // Invalid value
      default: return DENY;
    } // Not initialised so just check joystick setting
    else switch(iState)
    { // Valid enum? Set state and return succees
      case JOY_DETECT: case JOY_DISABLE: case JOY_ENABLE: break;
      // Invalid value
      default: return DENY;
    } // Update status
    jsStatus = static_cast<JoyStatus>(iState);
    // CVar allowed to set
    return ACCEPT;
  }
  /* -- Clear joystick state ----------------------------------------------- */
  void ClearJoystickButtons(void)
    { MYFOREACH(par_unseq, GetJoyList().begin(), GetJoyList().end(),
        [](JoyInfo &jsData) { jsData.ClearButtonStateIfConnected(); }); }
  /* -- Clear keyboard, mouse and joystick states -------------------------- */
  void ClearJoyStates(void) { ClearJoystickButtons(); }
  /* -- Return joystick data ----------------------------------------------- */
  JoyInfo &GetJoyData(const size_t stId) { return GetJoyList()[stId]; }
  /* -- Return joysticks count --------------------------------------------- */
  size_t GetJoyCount(void) const { return GetConstJoyList().size(); }
  /* -- Init --------------------------------------------------------------- */
  void Init(void)
  { // if window not available? This should never happen but we will put
    // this here just incase. The subsequent operations are pointless without
    // a valid GLFW window.
    if(!cGlFW->IsWindowAvailable())
    { // Log that initialisation is being skipped and return
      LW(LH_WARNING, "Input initialisation skipped with no window available!");
      return;
    } // Class initialised
    IHInitialise();
    // Log progress
    LW(LH_DEBUG, "Input interface is initialising...");
    // Init input engine events
    cEvtMain->RegisterEx(*this);
    // Init input settings
    SetRawMouseEnabled(cCVars->GetInternalSafe<bool>(INP_RAWMOUSE));
    SetStickyKeyEnabled(cCVars->GetInternalSafe<bool>(INP_STICKYKEY));
    SetStickyMouseEnabled(cCVars->GetInternalSafe<bool>(INP_STICKYMOUSE));
    SetJoystickEnabled(cCVars->GetInternalSafe<int>(INP_JOYSTICK));
    // Set/Restore cursor state
    SetCursor(FlagIsSet(IF_CURSOR));
    // Log progress
    LW(LH_INFO, "Input interface initialised (R:$;J:$).",
      TrueOrFalse(GlFW::IsRawMouseMotionSupported()), GetJoyCount());
  }
  /* -- DeInit ------------------------------------------------------------- */
  void DeInit(void)
  { // Ignore if class not initialised
    if(IHNotDeInitialise()) return;
    // Log progress
    LW(LH_DEBUG, "Input interface deinitialising...");
    // Deinit engine events in the order they were registered
    cEvtMain->UnregisterEx(*this);
    // Log progress
    LW(LH_INFO, "Input interface deinitialised.");
  }
  /* -- Constructor -------------------------------------------------------- */
  Input(void) :
    /* -- Initialisation of members ---------------------------------------- */
    IHelper(__FUNCTION__),             // Init initialisation helper class
    InputFlags(IF_NONE),               // No flags set initially
    /* -- Init joysticks --------------------------------------------------- */
    JoyList{ { JoyInfo(GLFW_JOYSTICK_1),  JoyInfo(GLFW_JOYSTICK_2),
               JoyInfo(GLFW_JOYSTICK_3),  JoyInfo(GLFW_JOYSTICK_4),
               JoyInfo(GLFW_JOYSTICK_5),  JoyInfo(GLFW_JOYSTICK_6),
               JoyInfo(GLFW_JOYSTICK_7),  JoyInfo(GLFW_JOYSTICK_8),
               JoyInfo(GLFW_JOYSTICK_9),  JoyInfo(GLFW_JOYSTICK_10),
               JoyInfo(GLFW_JOYSTICK_11), JoyInfo(GLFW_JOYSTICK_12),
               JoyInfo(GLFW_JOYSTICK_13), JoyInfo(GLFW_JOYSTICK_14),
               JoyInfo(GLFW_JOYSTICK_15), JoyInfo(GLFW_JOYSTICK_16) } },
    /* -- Init events for event manager ------------------------------------ */
    EvtMain::RegVec{                   // Events list to register
      { EMC_INP_SET_CUR_POS,  bind(&Input::OnSetCurPos,   this, _1) },
      { EMC_INP_CHAR,         bind(&Input::OnFilteredKey, this, _1) },
      { EMC_INP_PASTE,        bind(&Input::OnWindowPaste, this, _1) },
      { EMC_INP_MOUSE_MOVE,   bind(&Input::OnMouseMove,   this, _1) },
      { EMC_INP_MOUSE_CLICK,  bind(&Input::OnMouseClick,  this, _1) },
      { EMC_INP_MOUSE_FOCUS,  bind(&Input::OnMouseFocus,  this, _1) },
      { EMC_INP_MOUSE_SCROLL, bind(&Input::OnMouseWheel,  this, _1) },
      { EMC_INP_KEYPRESS,     bind(&Input::OnKeyPress,    this, _1) },
      { EMC_INP_DRAG_DROP,    bind(&Input::OnDragDrop,    this, _1) },
      { EMC_INP_JOY_STATE,    bind(&Input::OnJoyState,    this, _1) },
    },
    /* -- More initialisers ------------------------------------------------ */
    iConsoleKey1(GLFW_KEY_UNKNOWN),    // Init primary console key
    iConsoleKey2(GLFW_KEY_UNKNOWN),    // Init secondary console key
    lfOnMouseClick{ "OnMouseClick" },  // Init mouse click lua event
    lfOnMouseMove{ "OnMouseMove" },    // Init mouse movement lua event
    lfOnMouseScroll{ "OnMouseScroll" },// Init mouse wheel lua event
    lfOnMouseFocus{ "OnMouseFocus" },  // Init mouse focus lua event
    lfOnKey{ "OnUnfilteredKey" },      // Init unfiltered keypress lua event
    lfOnChar{ "OnFilteredKey" },       // Init filtered keypress lua event
    lfOnDragDrop{ "OnDragDrop" },      // Init drag & drop lua event
    lfOnJoyState{ "OnJoyState" },      // Init joy state lua event
    jsStatus(JOY_DISABLE),             // Initial joystick status is disabled
    iWinWidth(0),                      // Window width init by display
    iWinHeight(0),                     // Window height init by display
    iWinWidthD2(0),                    // Window width/2 init by display
    iWinHeightD2(0)                    // Window height/2 init by display
    /* -- No code ---------------------------------------------------------- */
    { }
  /* -- Destructor --------------------------------------------------------- */
  DTORHELPER(~Input, DeInit());
  /* ----------------------------------------------------------------------- */
  DELETECOPYCTORS(Input);              // Do not need defaults
  /* ----------------------------------------------------------------------- */
} *cInput = nullptr;                   // Global input class
/* -- End of module namespace ---------------------------------------------- */
};                                     // End of interface
/* == EoF =========================================================== EoF == */
