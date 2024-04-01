/* == LLDISPLAY.HPP ======================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Display' namespace and methods for the guest to use in ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Display
/* ------------------------------------------------------------------------- */
// ! The window class contains functions for manipulating the Window.
/* ========================================================================= */
namespace LLDisplay {                  // Display namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IDisplay::P;           using namespace IEvtMain::P;
using namespace IFboCore::P;           using namespace IGlFW::P;
using namespace IGlFWCursor::P;        using namespace IGlFWMonitor::P;
using namespace IInput::P;             using namespace ILua::P;
using namespace IOgl::P;               using namespace ITimer::P;
using namespace Lib::OS::GlFW;
/* ========================================================================= **
** ######################################################################### **
** ## Display.* namespace functions                                       ## **
** ######################################################################### **
** ========================================================================= */
// $ Display.Reset
// ? Resets the size and position of the window.
/* ------------------------------------------------------------------------- */
LLFUNC(Reset, cDisplay->RequestReposition());
/* ========================================================================= */
// $ Display.Attention
// ? This function requests user attention to the main window. On platforms
// ? where this is not supported, attention is requested to the application
// ? as a whole. Once the user has given attention, usually by focusing the
// ? window or application, the system will end the request automatically.
/* ------------------------------------------------------------------------- */
LLFUNC(Attention, cGlFW->WinRequestAttention());
/* ========================================================================= */
// $ Display.Centre
// ? Moves the window to the centre of the screen.
/* ------------------------------------------------------------------------- */
LLFUNC(Centre, cDisplay->RequestCentre());
/* ========================================================================= */
// $ Display.GetSize
// < Width:integer=Width of the window.
// < Height:integer=Height of the window.
// ? Resets the current size of the window.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetSize, 2,
  LCPUSHVAR(cInput->GetWindowWidth(), cInput->GetWindowHeight()));
/* ========================================================================= */
// $ Display.SetSize
// > Width:integer=New width of the window.
// > Height:integer=New height of the window.
// ? Sets the new size of the window. This does not change the cvars which
// ? control the default window size.
/* ------------------------------------------------------------------------- */
LLFUNC(SetSize, cDisplay->RequestResize(
  LCGETINTLGE(int, 1, 0, numeric_limits<int>::max(), "Width"),
  LCGETINTLGE(int, 2, 0, numeric_limits<int>::max(), "Height")));
/* ========================================================================= */
// $ Display.GetPos
// < X:integer=Current X position of the window.
// < Y:integer=Current Y position of the window.
// ? Gets the current position of the window on the desktop.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetPos, 2,
  LCPUSHVAR(cDisplay->GetWindowPosX(), cDisplay->GetWindowPosY()));
/* ========================================================================= */
// $ Display.SetPos
// > X:integer=New X position of the window.
// > Y:integer=New Y position of the window.
// ? Sets the new position of the window. This does not change the cvars which
// ? control the default window position.
/* ------------------------------------------------------------------------- */
LLFUNC(SetPos, cDisplay->RequestMove(LCGETINT(int, 1, "X"),
                                     LCGETINT(int, 2, "Y")));
/* ========================================================================= */
// $ Display.Iconify
// ? Minimises the window.
/* ------------------------------------------------------------------------- */
LLFUNC(Iconify, cGlFW->WinMinimise());
/* ========================================================================= */
// $ Display.Iconified
// < State:boolean=The window is inconified?
// ? Returns if the window is iconified.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Iconified, 1, LCPUSHVAR(cGlFW->WinIsIconifyAttribEnabled()));
/* ========================================================================= */
// $ Display.Restore
// ? Restores the window from a minimized state. This may not work in
// ? full-screen mode. Check Display.Focus().
/* ------------------------------------------------------------------------- */
LLFUNC(Restore, cGlFW->WinRestore());
/* ========================================================================= */
// $ Display.Focus
// ? Sets input focus to the window. This will actually restore the window if
// ? in full-screen mode.
/* ------------------------------------------------------------------------- */
LLFUNC(Focus, cGlFW->WinFocus());
/* ========================================================================= */
// $ Display.Focused
// < State:boolean=The window is focused?
// ? Returns if the window is focused.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Focused, 1, LCPUSHVAR(cGlFW->WinIsFocusAttribEnabled()));
/* ========================================================================= */
// $ Display.Hovered
// < State:boolean=Mouse is over the window?
// ? Indicates whether the cursor is currently directly over the content area
// ? of the window, with no other windows between.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Hovered, 1, LCPUSHVAR(cGlFW->WinIsMouseHoveredAttribEnabled()));
/* ========================================================================= */
// $ Display.Visible
// < State:boolean=The window is visible?
// ? Indicates whether the window is visible.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Visible, 1, LCPUSHVAR(cGlFW->WinIsVisibilityAttribEnabled()));
/* ========================================================================= */
// $ Display.Resizable
// < State:boolean=The window is visible?
// ? Indicates whether the window is resizable.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Resizable, 1, LCPUSHVAR(cGlFW->WinIsResizableAttribEnabled()));
/* ========================================================================= */
// $ Display.Maximise
// ? Maximises the window.
/* ------------------------------------------------------------------------- */
LLFUNC(Maximise, cGlFW->WinMaximise());
/* ========================================================================= */
// $ Display.Maximised
// < State:boolean=The window is maximised?
// ? Returns if the window is maximised.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Maximised, 1, LCPUSHVAR(cGlFW->WinIsMaximisedAttribEnabled()));
/* ========================================================================= */
// $ Display.Decordated
// < State:boolean=The window is decorated?
// ? Returns if the window is decorated with a border.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Decorated, 1, LCPUSHVAR(cGlFW->WinIsDecoratedAttribEnabled()));
/* ========================================================================= */
// $ Display.Transparent
// < State:boolean=The window is using an alpha framebuffer?
// ? Returns if the window is transparent and using a alpha framebuffer.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Transparent, 1, LCPUSHVAR(cGlFW->WinIsTransparencyAttribEnabled()));
/* ========================================================================= */
// $ Display.Floating
// < State:boolean=The window is floating?
// ? Returns if the window is floating an always showing on top.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Floating, 1, LCPUSHVAR(cGlFW->WinIsFloatingAttribEnabled()));
/* ========================================================================= */
// $ Display.SetFullScreen
// > State:boolean=true for fullscreen, false for window.
// ? Sets the full-screen window state and resets the video subsystem.
// ? This changes the cvar too.
/* ------------------------------------------------------------------------- */
LLFUNC(SetFullScreen, cDisplay->RequestFSToggle(LCGETBOOL(1, "State")));
/* ========================================================================= */
// $ Display.SetMatrix
// > Width:number=Matrix width.
// > Height:number=Matrix height.
// ? Sets the 2D aspect size of the matrix. Recommended to use a 4:3
// ? aspect ratio to allow the automatic aspect ratio feature to work properly
// ? for widescreen window sizes.
/* ------------------------------------------------------------------------- */
LLFUNC(SetMatrix, cFboCore->AutoMatrix(
  LCGETNUMLG(GLfloat, 1, 1, 16384, "Width"),
  LCGETNUMLG(GLfloat, 2, 1, 16384, "Height"), false));
/* ========================================================================= */
// $ Display.VReset
// ? Resets the video subsystem.
/* ------------------------------------------------------------------------- */
LLFUNC(VReset, cEvtMain->Add(EMC_QUIT_THREAD));
/* ========================================================================= */
// $ Display.OnFocused
// > Func:function=The event callback function
// ? When the window loses or gains focus, this event is called.
/* ------------------------------------------------------------------------- */
LLFUNC(OnFocused, LCSETEVENTCB(cDisplay->lrFocused));
/* ========================================================================= */
// $ Display.GPU
// < Vendor:string=GL vendor string.
// < Version:string=GL version string.
// < Renderer:string=GL gpu string.
// ? Returns info about OpenGL.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GPU, 3,
  LCPUSHVAR(cOgl->GetVendor(), cOgl->GetVersion(), cOgl->GetRenderer()));
/* ========================================================================= */
// $ Display.VRAM
// < Available:integer=Available video memory in bytes
// < Total:integer=Total video memory in bytes
// < Used:integer=Used video memory in bytes
// < Dedicated:boolean=Is video memory separate from main memory
// ? Returns info about the GPU's video memory.
/* ------------------------------------------------------------------------- */
LLFUNCEX(VRAM, 5,
  LCPUSHVAR(cOgl->GetVRAMFreePC(), cOgl->GetVRAMTotal(),
            cOgl->GetVRAMFree(),   cOgl->GetVRAMUsed(),
            cOgl->FlagIsSet(GFL_SHARERAM)));
/* ========================================================================= */
// $ Display.Monitor
// < Id:number=Monitor index.
// ? Returns the currently selected monitor id.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Monitor, 1, LCPUSHVAR(cDisplay->GetMonitorId()));
/* ========================================================================= */
// $ Display.Monitors
// < Id:number=Monitor count.
// ? Returns the total number of detected monitors on the system.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Monitors, 1, LCPUSHVAR(cDisplay->GetMonitorsCount()));
/* ========================================================================= */
// $ Display.MonitorData
// > Id:integer=Id of monitor to query.
// < Name:string=Name of monitor.
// ? Returns the name of the specified monitor.
/* ------------------------------------------------------------------------- */
LLFUNCEX(MonitorData, 1, LCPUSHVAR(cDisplay->GetMonitors()[
  LCGETINTLG(size_t, 1, 0, cDisplay->GetMonitorsCount(), "Id")].Name()));
/* ========================================================================= */
// $ Display.VidMode
// < Id:number=Mode id.
// ? Returns the currently selected video mode id.
/* ------------------------------------------------------------------------- */
LLFUNCEX(VidMode, 1, LCPUSHVAR(cDisplay->GetVideoModeId()));
/* ========================================================================= */
// $ Display.VidModes
// < Count:number=Total video modes.
// ? Returns the total number of video modes supported by the GPU.
/* ------------------------------------------------------------------------- */
LLFUNCEX(VidModes, 1, LCPUSHVAR(cDisplay->GetMonitors()[
  LCGETINTLGE(size_t, 1, 0,
    cDisplay->GetMonitorsCount(), "MonitorId")].size()));
/* ========================================================================= */
// $ Display.VidModeData
// > MonitorId:integer=Id of monitor to query.
// > ModeId:integer=Id of video mode to query.
// < Width:integer=Width of the video mode in pixels.
// < Height:integer=Height of the video mode in pixels.
// < Depth:integer=Depth of the video mode. (16 or 24).
// < Refresh:integer=Refresh rate of the video mode (normally 60).
// < Red:integer=Number of bits in the red component (normally 8).
// < Green:integer=Number of bits in the green component (normally 8).
// < Blue:integer=Number of bits in the blue component (normally 8).
// ? Returns information about the specified video mode.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(VidModeData)
  const GlFWMonitor &mItem = cDisplay->GetMonitors()[
    LCGETINTLGE(size_t, 1, 0, cDisplay->GetMonitorsCount(), "MonitorId")];
  const GlFWRes &rItem = mItem[
    LCGETINTLGE(size_t, 2, 0, mItem.size(), "VideoModeId")];
  LCPUSHVAR(rItem.Width(),   rItem.Height(), rItem.Depth(),
            rItem.Refresh(), rItem.Red(),    rItem.Green(),
            rItem.Blue());
LLFUNCENDEX(7)
/* ========================================================================= */
// $ Display.GPUFPS
// < FPS:number=Frames per second.
// ? Get GPU frames rendered in the second. Should be 60 for most people.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GPUFPS, 1, LCPUSHVAR(cFboCore->dRTFPS));
/* ========================================================================= */
// $ Display.SetInterval
// > Ticks:integer=Number of ticks per second (min:1,max:200).
// ? This functions set the core timing frequency. The default target is to
// ? perform at least 60 game loop iterations per second. This is completely
// ? detatched from GPU rendering which will always render as fast as possible.
/* ------------------------------------------------------------------------- */
LLFUNCEX(SetInterval, 1,
  cTimer->TimerSetInterval(LCGETNUMLG(uint64_t, 1,
     cTimer->TimerGetMinInterval(), cTimer->TimerGetMaxInterval(),
    "Interval")));
/* ========================================================================= */
// $ Display.FSType
// < Type:integer=Type of full-screen
// ? This function returns the type of full-screen currently engaged. Check
// ? Display.FSTypes table for a list of possible values.
/* ------------------------------------------------------------------------- */
LLFUNCEX(FSType, 1, LCPUSHVAR(cDisplay->GetFSType()));
/* ========================================================================= */
// $ Display.ResetCursor
// ? Resets to the standard cursor graphic
/* ------------------------------------------------------------------------- */
LLFUNC(ResetCursor, cDisplay->RequestResetCursor());
/* ========================================================================= */
// $ Display.SetCursor
// < Id:integer=The id of the new cursor
// ? This function sets a new standard cursor id. This operation is
// ? asynchronous and won't take effect until the next frame.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCursor, cDisplay->RequestSetCursor(LCGETINTLGE(GlFWCursorType, 1,
  CUR_ARROW, CUR_MAX, "CursorId")));
/* ========================================================================= **
** ######################################################################### **
** ## Display.* namespace functions structure                             ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Display.* namespace func
  LLRSFUNC(Attention),   LLRSFUNC(Centre),        LLRSFUNC(Decorated),
  LLRSFUNC(Floating),    LLRSFUNC(Focus),         LLRSFUNC(Focused),
  LLRSFUNC(FSType),      LLRSFUNC(GetPos),        LLRSFUNC(GetSize),
  LLRSFUNC(GPU),         LLRSFUNC(GPUFPS),        LLRSFUNC(Hovered),
  LLRSFUNC(Iconified),   LLRSFUNC(Iconify),       LLRSFUNC(Maximise),
  LLRSFUNC(Maximised),   LLRSFUNC(Monitor),       LLRSFUNC(MonitorData),
  LLRSFUNC(Monitors),    LLRSFUNC(OnFocused),     LLRSFUNC(Reset),
  LLRSFUNC(ResetCursor), LLRSFUNC(Resizable),     LLRSFUNC(Restore),
  LLRSFUNC(SetCursor),   LLRSFUNC(SetFullScreen), LLRSFUNC(SetInterval),
  LLRSFUNC(SetMatrix),   LLRSFUNC(SetPos),        LLRSFUNC(SetSize),
  LLRSFUNC(Transparent), LLRSFUNC(VidMode),       LLRSFUNC(VidModeData),
  LLRSFUNC(VidModes),    LLRSFUNC(Visible),       LLRSFUNC(VRAM),
  LLRSFUNC(VReset),
LLRSEND                                // Display.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Display.* namespace constants                                       ## **
** ######################################################################### **
** ========================================================================= */
// @ Display.Cursors
// < Data:table=The available standard cursor types
// ? Returns a table of key/value pairs that identify standard cursors
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Cursors)                   // Beginning of cursor codes
  LLRSKTITEM(CUR_,ARROW),              LLRSKTITEM(CUR_,CROSSHAIR),
  LLRSKTITEM(CUR_,HAND),               LLRSKTITEM(CUR_,HRESIZE),
  LLRSKTITEM(CUR_,IBEAM),              LLRSKTITEM(CUR_,NOT_ALLOWED),
  LLRSKTITEM(CUR_,RESIZE_ALL),         LLRSKTITEM(CUR_,RESIZE_EW),
  LLRSKTITEM(CUR_,RESIZE_NESW),        LLRSKTITEM(CUR_,RESIZE_NS),
  LLRSKTITEM(CUR_,RESIZE_NWSE),        LLRSKTITEM(CUR_,VRESIZE),
LLRSKTEND                              // End of cursor codes
/* ========================================================================= */
// @ Display.FSTypes
// < Data:table=The available full-screen types
// ? Returns a table of key/value pairs that identify the states of the window.
// ? ID   Keyname     Meaning
// ? ===  =======     =========================================================
// ? [0]  WINDOW      Decorated and can be moved and sized around the desktop.
// ? [1]  EXCLUSIVE   Undecorated, covers the whole screen and set resolution.
// ? [2]  BORDERLESS  Undecorated, covers the whole screen only.
// ? [3]  NATIVE      (MacOS only) Same as BORDERLESS but disables toggler.
// ? ===  =======     =========================================================
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(FSTypes)                   // Beginning of full-screen types
  LLRSKTITEM(Display::FST_,WINDOW),    LLRSKTITEM(Display::FST_,EXCLUSIVE),
  LLRSKTITEM(Display::FST_,BORDERLESS),LLRSKTITEM(Display::FST_,NATIVE),
LLRSKTEND                              // End of mouse codes
/* ========================================================================= **
** ######################################################################### **
** ## Display.* namespace structure                                       ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Display.* namespace consts begin
  LLRSCONST(Cursors), LLRSCONST(FSTypes),
LLRSCONSTEND                           // Display.* namespace consts end
/* ========================================================================= */
}                                      // End of Display namespace
/* == EoF =========================================================== EoF == */
