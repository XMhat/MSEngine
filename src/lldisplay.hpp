/* == LLDISPLAY.HPP ======================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Display' namespace and methods for the guest to use in ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Display
/* ------------------------------------------------------------------------- */
// ! The window class contains functions for manipulating the Window.
/* ========================================================================= */
namespace NsDisplay {                  // Display namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfDisplay;             // Using display namespace
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
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
LLFUNCBEGIN(GetSize)
  LCPUSHINT(cInput->GetWindowWidth());
  LCPUSHINT(cInput->GetWindowHeight());
LLFUNCENDEX(2)
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
LLFUNCBEGIN(GetPos)
  LCPUSHINT(cDisplay->GetWindowPosX());
  LCPUSHINT(cDisplay->GetWindowPosY());
LLFUNCENDEX(2)
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
LLFUNCEX(Iconified, 1, LCPUSHBOOL(cGlFW->WinIsIconifyAttribEnabled()));
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
LLFUNCEX(Focused, 1, LCPUSHBOOL(cGlFW->WinIsFocusAttribEnabled()));
/* ========================================================================= */
// $ Display.Hovered
// < State:boolean=Mouse is over the window?
// ? Indicates whether the cursor is currently directly over the content area
// ? of the window, with no other windows between.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Hovered, 1, LCPUSHBOOL(cGlFW->WinIsMouseHoveredAttribEnabled()));
/* ========================================================================= */
// $ Display.Visible
// < State:boolean=The window is visible?
// ? Indicates whether the window is visible.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Visible, 1, LCPUSHBOOL(cGlFW->WinIsVisibilityAttribEnabled()));
/* ========================================================================= */
// $ Display.Resizable
// < State:boolean=The window is visible?
// ? Indicates whether the window is resizable.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Resizable, 1, LCPUSHBOOL(cGlFW->WinIsResizableAttribEnabled()));
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
LLFUNCEX(Maximised, 1, LCPUSHBOOL(cGlFW->WinIsMaximisedAttribEnabled()));
/* ========================================================================= */
// $ Display.Decordated
// < State:boolean=The window is decorated?
// ? Returns if the window is decorated with a border.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Decorated, 1, LCPUSHBOOL(cGlFW->WinIsDecoratedAttribEnabled()));
/* ========================================================================= */
// $ Display.Transparent
// < State:boolean=The window is using an alpha framebuffer?
// ? Returns if the window is transparent and using a alpha framebuffer.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Transparent, 1, LCPUSHBOOL(cGlFW->WinIsTransparencyAttribEnabled()));
/* ========================================================================= */
// $ Display.Floating
// < State:boolean=The window is floating?
// ? Returns if the window is floating an always showing on top.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Floating, 1, LCPUSHBOOL(cGlFW->WinIsFloatingAttribEnabled()));
/* ========================================================================= */
// $ Display.SetFullScreen
// > State:boolean=true for fullscreen, false for window.
// ? Sets the full-screen window state and resets the video subsystem.
// ? This changes the cvar too.
/* ------------------------------------------------------------------------- */
LLFUNC(SetFullScreen, cDisplay->RequestFSToggle(LCGETBOOL(1, "State")));
/* ========================================================================= */
// $ Display.SetMatrix
// > Width:number=Ortho width.
// > Height:number=Ortho height.
// ? Sets the aspect (ortho) size of the viewport. Recommended to use a 4:3
// ? aspect ratio to allow the automatic aspect ratio feature to work properly
// ? for widescreen window sizes.
/* ------------------------------------------------------------------------- */
LLFUNC(SetMatrix, cFboMain->AutoMatrix(
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
LLFUNCBEGIN(GPU)
  LCPUSHXSTR(cOgl->GetVendor());       LCPUSHXSTR(cOgl->GetVersion());
  LCPUSHXSTR(cOgl->GetRenderer());
LLFUNCENDEX(3)
/* ========================================================================= */
// $ Display.VRAM
// < Available:integer=Available video memory in bytes
// < Total:integer=Total video memory in bytes
// < Used:integer=Used video memory in bytes
// < Dedicated:boolean=Is video memory separate from main memory
// ? Returns info about the GPU's video memory.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(VRAM)
  LCPUSHNUM(cOgl->GetVRAMFreePC());    LCPUSHINT(cOgl->GetVRAMTotal());
  LCPUSHINT(cOgl->GetVRAMFree());      LCPUSHINT(cOgl->GetVRAMUsed());
  LCPUSHBOOL(cOgl->FlagIsSet(GFL_SHARERAM));
LLFUNCENDEX(5)
/* ========================================================================= */
// $ Display.Monitor
// < Id:number=Monitor index.
// ? Returns the currently selected monitor id.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Monitor, 1, LCPUSHINT(cDisplay->GetMonitorId()));
/* ========================================================================= */
// $ Display.Monitors
// < Id:number=Monitor count.
// ? Returns the total number of detected monitors on the system.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Monitors, 1, LCPUSHINT(cDisplay->GetMonitorsCount()));
/* ========================================================================= */
// $ Display.MonitorData
// > Id:integer=Id of monitor to query.
// < Name:string=Name of monitor.
// ? Returns the name of the specified monitor.
/* ------------------------------------------------------------------------- */
LLFUNCEX(MonitorData, 1, LCPUSHXSTR(cDisplay->GetMonitors()[
  LCGETINTLG(size_t, 1, 0, cDisplay->GetMonitorsCount(), "Id")].Name()));
/* ========================================================================= */
// $ Display.VidMode
// < Id:number=Mode id.
// ? Returns the currently selected video mode id.
/* ------------------------------------------------------------------------- */
LLFUNCEX(VidMode, 1, LCPUSHINT(cDisplay->GetVideoModeId()));
/* ========================================================================= */
// $ Display.VidModes
// < Count:number=Total video modes.
// ? Returns the total number of video modes supported by the GPU.
/* ------------------------------------------------------------------------- */
LLFUNCEX(VidModes, 1, LCPUSHINT(cDisplay->GetMonitors()[
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
  LCPUSHINT(rItem.Width());            LCPUSHINT(rItem.Height());
  LCPUSHINT(rItem.Depth());            LCPUSHNUM(rItem.Refresh());
  LCPUSHINT(rItem.Red());              LCPUSHINT(rItem.Green());
  LCPUSHINT(rItem.Blue());
LLFUNCENDEX(7)
/* ========================================================================= */
// $ Display.GPUFPS
// < FPS:number=Frames per second.
// ? Get GPU frames rendered in the second. Should be 60 for most people.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GPUFPS, 1, LCPUSHNUM(cFboMain->dRTFPS));
/* ========================================================================= */
// $ Display.TimerSetInterval
// > Ticks:integer=Number of ticks per second (min:1,max:200).
// ? This functions set the core timing frequency. The default target is to
// ? perform at least 60 game loop iterations per second. This is completely
// ? detatched from GPU rendering which will always render as fast as possible.
/* ------------------------------------------------------------------------- */
LLFUNCEX(SetInterval, 1,
   cTimer->TimerSetInterval(LCGETNUMLG(double, 1, 1, 200, "Interval")));
/* ========================================================================= */
/* ######################################################################### */
/* ## Display.* namespace functions structure                             ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Display.* namespace func
  LLRSFUNC(Attention),   LLRSFUNC(Centre),        LLRSFUNC(Decorated),
  LLRSFUNC(Floating),    LLRSFUNC(Focus),         LLRSFUNC(Focused),
  LLRSFUNC(GetPos),      LLRSFUNC(GetSize),       LLRSFUNC(GPU),
  LLRSFUNC(GPUFPS),      LLRSFUNC(Hovered),       LLRSFUNC(Iconified),
  LLRSFUNC(Iconify),     LLRSFUNC(Maximise),      LLRSFUNC(Maximised),
  LLRSFUNC(Monitor),     LLRSFUNC(MonitorData),   LLRSFUNC(Monitors),
  LLRSFUNC(OnFocused),   LLRSFUNC(Reset),         LLRSFUNC(Resizable),
  LLRSFUNC(Restore),     LLRSFUNC(SetFullScreen), LLRSFUNC(SetInterval),
  LLRSFUNC(SetMatrix),   LLRSFUNC(SetPos),        LLRSFUNC(SetSize),
  LLRSFUNC(Transparent), LLRSFUNC(VidMode),       LLRSFUNC(VidModeData),
  LLRSFUNC(VidModes),    LLRSFUNC(Visible),       LLRSFUNC(VRAM),
  LLRSFUNC(VReset),
LLRSEND                                // Display.* namespace functions end
/* ========================================================================= */
}                                      // End of Display namespace
/* == EoF =========================================================== EoF == */
