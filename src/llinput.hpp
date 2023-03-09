/* == LLINPUT.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Input' namespace and methods for the guest to use in   ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Input
/* ------------------------------------------------------------------------- */
// ! The input class allows the programmer to query the state of the keyboard,
// ! mouse and game controllers.
/* ========================================================================= */
namespace NsInput {                    // Input namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfInput;               // Using input namespace
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Input.SetJoyAxisForwardDeadZone
// > JId:integer=The joystick id.
// > AId:integer=The joystick axis id.
// > Threshold:number=The forward deadzone value
// ? Sets the forward deadzone of the specified axis for the specified joystick
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(SetJoyAxisForwardDeadZone)
  JoyInfo &jiData = cInput->GetJoyData(LCGETINTLGE(size_t, 1, 0,
    cInput->GetJoyCount(), "JoyId"));
  jiData.SetAxisForwardDeadZone(
    LCGETINTLGE(size_t, 2, 0, jiData.GetAxisCount(), "AxisId"),
    LCGETINTLGE(float,  3, 0, 1,                     "DeadZone"));
LLFUNCEND
/* ========================================================================= */
// $ Input.SetJoyAxisReverseDeadZone
// > JId:integer=The joystick id.
// > AId:integer=The joystick axis id.
// > Threshold:number=The forward deadzone value (positive)
// ? Sets the reverse deadzone of the specified axis for the specified joystick
// ? This value automatically gets converted to negative value.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(SetJoyAxisReverseDeadZone)
  JoyInfo &jiData = cInput->GetJoyData(LCGETINTLGE(size_t, 1, 0,
    cInput->GetJoyCount(), "JoyId"));
  jiData.SetAxisReverseDeadZone(
    LCGETINTLGE(size_t, 2, 0, jiData.GetAxisCount(), "AxisId"),
    LCGETINTLGE(float,  3, 0, 1,                     "DeadZone"));
LLFUNCEND
/* ========================================================================= */
// $ Input.SetJoyAxisDeadZones
// > JId:integer=The joystick id.
// > AId:integer=The joystick axis id.
// > FThresh:number=The forward deadzone value (positive)
// > RThresh:number=The reverse deadzone value (positive)
// ? Sets the forward and reverse deadzone of the specified axis for the
// ? specified joystick. The reverse value automatically gets converted to
// ? negative value.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(SetJoyAxisDeadZones)
  JoyInfo &jiData = cInput->GetJoyData(LCGETINTLGE(size_t, 1, 0,
    cInput->GetJoyCount(), "JoyId"));
  jiData.SetAxisDeadZones(
    LCGETINTLGE(size_t, 2, 0, jiData.GetAxisCount(), "AxisId"),
    LCGETINTLGE(float,  3, 0, 1,                     "FDeadZone"),
    LCGETINTLGE(float,  4, 0, 1,                     "RDeadZone"));
LLFUNCEND
/* ========================================================================= */
// $ Input.SetCursorPos
// > X:integer=The X co-ordinate of the mouse cursor.
// > Y:integer=The Y co-ordinate of the mouse cursor.
// ? Sets the position of the mouse cursor relative to the top-left of the
// ? main FBO ortho, meaning X could be negative.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCursorPos,
  cInput->SetCursorPos(LCGETNUM(double, 1, "X"), LCGETNUM(double, 2, "Y")));
/* ========================================================================= */
// $ Input.GetCursorPos
// > X:integer=The X co-ordinate of the mouse cursor.
// > Y:integer=The Y co-ordinate of the mouse cursor.
// ? Sets the position of the mouse cursor relative to the top-left of the
// ? main FBO ortho, meaning X could be negative.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(GetCursorPos)
  double dX, dY; cInput->GetCursorPos(dX, dY);
  LCPUSHNUM(dX);
  LCPUSHNUM(dY);
LLFUNCENDEX(2)
/* ========================================================================= */
// $ Input.MouseState
// > ButtonId:integer=The button id to test.
// < Result:integer=Button state (0=up,1=down).
// ? Returns the state of the specified mouse button.
/* ------------------------------------------------------------------------- */
LLFUNCEX(MouseState, 1, LCPUSHINT(cGlFW->WinGetMouse(
  LCGETINTLG(int, 1, 0, GLFW_MOUSE_BUTTON_LAST, "Button"))));
/* ========================================================================= */
// $ Input.KeyState
// > ButtonId:integer=The GLFW key id to test.
// < Result:integer=Button state (0=up,1=down).
// ? Returns the state of the specified keyboard button direct from cGlFW->
/* ------------------------------------------------------------------------- */
LLFUNCEX(KeyState, 1,
  LCPUSHINT(cGlFW->WinGetKey(LCGETINTLG(int, 1, 0, GLFW_KEY_LAST, "Key"))));
/* ========================================================================= */
// $ Input.ClearStates
// ? Clears the keyboard, mouse and joystick states which wipes the current
// ? state of all the control methods preventing input.
/* ------------------------------------------------------------------------- */
LLFUNC(ClearStates, cInput->ClearJoystickButtons());
/* ========================================================================= */
// $ Input.CursorCentre
// ? Sets the mouse cursor in the centre of the window.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCursorCentre, cInput->SetCursorCentre());
/* ========================================================================= */
// $ Input.SetCursor
// > State:boolean=True to show the cursor, false to hide it.
// ? Shows or hides the OS cursor so you can effectively design your own
// ? software based cursor.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCursor, cInput->SetCursor(LCGETBOOL(1, "State")));
/* ========================================================================= */
// $ Input.JoyExists
// > Id:integer=The joystick id.
// < State:boolean=True if it exists, false if it does not.
// ? Tests if the specified joystick exists.
/* ------------------------------------------------------------------------- */
LLFUNCEX(JoyExists, 1, LCPUSHBOOL(cInput->JoystickExists(
  LCGETINTLGE(size_t, 1, 0, cInput->GetJoyCount(), "JoyId"))));
/* ========================================================================= */
// $ Input.GetJoyName
// > Id:integer=The joystick id.
// < Name:string=The name of the joystick.
// ? Returns the name of the specified joystick as reported by the OS.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetJoyName, 1, LCPUSHXSTR(cInput->GetJoyData(
  LCGETINTLGE(size_t, 1, 0, cInput->GetJoyCount(), "JoyId")).IdentGet()));
/* ========================================================================= */
// $ Input.GetNumJoyButtons
// > Id:integer=The joystick id.
// < Count:integer=Number of buttons.
// ? Returns the number of buttons this joystick supports.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetNumJoyButtons, 1, LCPUSHINT(cInput->GetJoyData(
  LCGETINTLGE(size_t, 1, 0, cInput->GetJoyCount(), "JoyId")).
    GetButtonCount()));
/* ========================================================================= */
// $ Input.GetNumJoyAxises
// > Id:integer=The joystick id.
// < Count:integer=Number of axises.
// ? Returns the number of axises this joystick supports.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetNumJoyAxises, 1, LCPUSHINT(cInput->GetJoyData(
  LCGETINTLGE(size_t, 1, 0, cInput->GetJoyCount(), "JoyId")).
    GetAxisCount()));
/* ========================================================================= */
// $ Input.GetJoyButton
// > Id:integer=The joystick id.
// > ButtonId:integer=The button id to test.
// < Result:integer=Number of ticks the button has been held down.
// ? Returns the state of the specified joystick button.
// ?
// ? Result can be:
// ? =0: The button is not held down.
// ? >0: The button was held down for this many game ticks.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(GetJoyButton)
  const JoyInfo &jiData = cInput->GetJoyData(LCGETINTLGE(size_t,
    1, 0, cInput->GetJoyCount(), "JoyId"));
  LCPUSHINT(jiData.GetButtonState(LCGETINTLGE(size_t, 2, 0,
    jiData.GetButtonCount(), "ButtonId")));
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Input.GetJoyAxis
// > Id:integer=The joystick id.
// > AxisId:integer=The axis id to test.
// < Result:integer=Number of ticks the button has been held down.
// ? Returns the state of the specified joystick axis.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(GetJoyAxis)
  const JoyInfo &jiData = cInput->GetJoyData(LCGETINTLGE(size_t,
    1, 0, cInput->GetJoyCount(), "JoyId"));
  LCPUSHINT(jiData.GetAxisState(LCGETINTLGE(size_t, 2, 0,
    jiData.GetButtonCount(), "AxisId")));
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Input.GetJoyAxisUB
// > Id:integer=The joystick id.
// > AxisId:integer=The axis id to test.
// < Result:number=The raw state of axis.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(GetJoyAxisUB)
  const JoyInfo &jiData = cInput->GetJoyData(LCGETINTLGE(size_t,
    1, 0, cInput->GetJoyCount(), "JoyId"));
  LCPUSHNUM(jiData.GetUnbufferedAxisState(LCGETINTLGE(size_t, 2, 0,
    jiData.GetAxisCount(), "AxisId")));
LLFUNCENDEX(1)
/* ========================================================================= */
// $ Input.OnMouseFocus
// > Func:function=The callback function to use
// ? When the mouse is moved from the window to another application and back,
// ? this function will be called with a integer whether the mouse entered the
// ? window or not.
/* ------------------------------------------------------------------------- */
LLFUNC(OnMouseFocus, LCSETEVENTCB(cInput->lfOnMouseFocus));
/* ========================================================================= */
// $ Input.OnMouseClick
// > Func:function=The callback function to use
// ? When the mouse is clicked, this function will be called with the
// ? mouse button clicked, the state and the key modifiers
/* ------------------------------------------------------------------------- */
LLFUNC(OnMouseClick, LCSETEVENTCB(cInput->lfOnMouseClick));
/* ========================================================================= */
// $ Input.OnMouseMove
// > Func:function=The callback function to use
// ? When the mouse is moved, this function will be called with the
// ? current cursor x and y position based on the main framebuffer.
/* ------------------------------------------------------------------------- */
LLFUNC(OnMouseMove, LCSETEVENTCB(cInput->lfOnMouseMove));
/* ========================================================================= */
// $ Input.OnDragDrop
// > Func:function=The callback function to use
// ? When files are dragged into the window, this function will be called with
// ? an array of files that were dragged into it.
/* ------------------------------------------------------------------------- */
LLFUNC(OnDragDrop, LCSETEVENTCB(cInput->lfOnDragDrop));
/* ========================================================================= */
// $ Input.OnMouseScroll
// > Func:function=The callback function to use
// ? When a filtered key is pressed, this function will be called with the
// ? 2 axis coordinates on which direction the mouse was scrolled
/* ------------------------------------------------------------------------- */
LLFUNC(OnMouseScroll, LCSETEVENTCB(cInput->lfOnMouseScroll));
/* ========================================================================= */
// $ Input.OnChar
// > Func:function=The callback function to use
// ? When a filtered key is pressed, this function will be called with the
// ? key that was pressed.
/* ------------------------------------------------------------------------- */
LLFUNC(OnChar, LCSETEVENTCB(cInput->lfOnChar));
/* ========================================================================= */
// $ Input.OnJoyState
// > Func:function=The callback function to use
// ? When a joystick is disconnected or disconnected, this callback will be
// ? triggered with the joystick id and a boolean if wether the
/* ------------------------------------------------------------------------- */
LLFUNC(OnJoyState, LCSETEVENTCB(cInput->lfOnJoyState));
/* ========================================================================= */
// $ Input.OnKey
// > Func:function=The callback function to use
// ? When an filtered key is pressed, this function will be called with the
// ? key that was pressed
/* ------------------------------------------------------------------------- */
LLFUNC(OnKey, LCSETEVENTCB(cInput->lfOnKey));
/* ========================================================================= */
// $ Input.GetKeyName
// > Value:integer=The id of the key
// < Name:stringr=The name of the key
// ? Returns the name of the specified key
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetKeyName, 1, LCPUSHSTR(GlFWGetKeyName(LCGETINT(int, 1, "Key"),
  LCGETINT(int, 2, "Scan"))));
/* ========================================================================= */
/* ######################################################################### */
/* ## Input.* namespace functions structure                               ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Input.* namespace functions begin
  LLRSFUNC(ClearStates),               LLRSFUNC(GetCursorPos),
  LLRSFUNC(GetJoyAxis),                LLRSFUNC(GetJoyAxisUB),
  LLRSFUNC(GetJoyButton),              LLRSFUNC(GetJoyName),
  LLRSFUNC(GetKeyName),                LLRSFUNC(GetNumJoyAxises),
  LLRSFUNC(GetNumJoyButtons),          LLRSFUNC(JoyExists),
  LLRSFUNC(KeyState),                  LLRSFUNC(MouseState),
  LLRSFUNC(OnChar),                    LLRSFUNC(OnDragDrop),
  LLRSFUNC(OnJoyState),                LLRSFUNC(OnKey),
  LLRSFUNC(OnMouseClick),              LLRSFUNC(OnMouseFocus),
  LLRSFUNC(OnMouseMove),               LLRSFUNC(OnMouseScroll),
  LLRSFUNC(SetCursor),                 LLRSFUNC(SetCursorCentre),
  LLRSFUNC(SetCursorPos),              LLRSFUNC(SetJoyAxisDeadZones),
  LLRSFUNC(SetJoyAxisForwardDeadZone), LLRSFUNC(SetJoyAxisReverseDeadZone),
LLRSEND                                // Input.* namespace functions end
/* ========================================================================= */
/* ######################################################################### */
/* ## Image.* namespace constants structure                               ## */
/* ######################################################################### */
/* ========================================================================= */
// @ Input.GetKeyCodes
// < Data:table=The entire list of key codes GLFW supports
// ? Returns a table of key/value pairs that identify a specific key with its
// ? corresponding GLFW code (for use with GetKeyButton[UB]).
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(KeyCodes)                  // Beginning of key codes
  LLRSKTITEM(GLFW_KEY_,UNKNOWN),       LLRSKTITEM(GLFW_KEY_,SPACE),
  LLRSKTITEM(GLFW_KEY_,APOSTROPHE),    LLRSKTITEM(GLFW_KEY_,COMMA),
  LLRSKTITEM(GLFW_KEY_,MINUS),         LLRSKTITEM(GLFW_KEY_,PERIOD),
  LLRSKTITEM(GLFW_KEY_,SLASH),         LLRSKTITEMEX2("ZERO",GLFW_KEY_0),
  LLRSKTITEMEX2("ONE",GLFW_KEY_1),     LLRSKTITEMEX2("TWO",GLFW_KEY_2),
  LLRSKTITEMEX2("THREE",GLFW_KEY_3),   LLRSKTITEMEX2("FOUR",GLFW_KEY_4),
  LLRSKTITEMEX2("FIVE",GLFW_KEY_5),    LLRSKTITEMEX2("SIX",GLFW_KEY_6),
  LLRSKTITEMEX2("SEVEN",GLFW_KEY_7),   LLRSKTITEMEX2("EIGHT",GLFW_KEY_8),
  LLRSKTITEMEX2("NINE",GLFW_KEY_9),    LLRSKTITEM(GLFW_KEY_,SEMICOLON),
  LLRSKTITEM(GLFW_KEY_,EQUAL),         LLRSKTITEM(GLFW_KEY_,A),
  LLRSKTITEM(GLFW_KEY_,B),             LLRSKTITEM(GLFW_KEY_,C),
  LLRSKTITEM(GLFW_KEY_,D),             LLRSKTITEM(GLFW_KEY_,E),
  LLRSKTITEM(GLFW_KEY_,F),             LLRSKTITEM(GLFW_KEY_,G),
  LLRSKTITEM(GLFW_KEY_,H),             LLRSKTITEM(GLFW_KEY_,I),
  LLRSKTITEM(GLFW_KEY_,J),             LLRSKTITEM(GLFW_KEY_,K),
  LLRSKTITEM(GLFW_KEY_,L),             LLRSKTITEM(GLFW_KEY_,M),
  LLRSKTITEM(GLFW_KEY_,N),             LLRSKTITEM(GLFW_KEY_,O),
  LLRSKTITEM(GLFW_KEY_,P),             LLRSKTITEM(GLFW_KEY_,Q),
  LLRSKTITEM(GLFW_KEY_,R),             LLRSKTITEM(GLFW_KEY_,S),
  LLRSKTITEM(GLFW_KEY_,T),             LLRSKTITEM(GLFW_KEY_,U),
  LLRSKTITEM(GLFW_KEY_,V),             LLRSKTITEM(GLFW_KEY_,W),
  LLRSKTITEM(GLFW_KEY_,X),             LLRSKTITEM(GLFW_KEY_,Y),
  LLRSKTITEM(GLFW_KEY_,Z),             LLRSKTITEM(GLFW_KEY_,LEFT_BRACKET),
  LLRSKTITEM(GLFW_KEY_,BACKSLASH),     LLRSKTITEM(GLFW_KEY_,RIGHT_BRACKET),
  LLRSKTITEM(GLFW_KEY_,GRAVE_ACCENT),  LLRSKTITEM(GLFW_KEY_,WORLD_1),
  LLRSKTITEM(GLFW_KEY_,WORLD_2),       LLRSKTITEM(GLFW_KEY_,ESCAPE),
  LLRSKTITEM(GLFW_KEY_,ENTER),         LLRSKTITEM(GLFW_KEY_,TAB),
  LLRSKTITEM(GLFW_KEY_,BACKSPACE),     LLRSKTITEM(GLFW_KEY_,INSERT),
  LLRSKTITEM(GLFW_KEY_,DELETE),        LLRSKTITEM(GLFW_KEY_,RIGHT),
  LLRSKTITEM(GLFW_KEY_,LEFT),          LLRSKTITEM(GLFW_KEY_,DOWN),
  LLRSKTITEM(GLFW_KEY_,UP),            LLRSKTITEM(GLFW_KEY_,PAGE_UP),
  LLRSKTITEM(GLFW_KEY_,PAGE_DOWN),     LLRSKTITEM(GLFW_KEY_,HOME),
  LLRSKTITEM(GLFW_KEY_,END),           LLRSKTITEM(GLFW_KEY_,CAPS_LOCK),
  LLRSKTITEM(GLFW_KEY_,SCROLL_LOCK),   LLRSKTITEM(GLFW_KEY_,NUM_LOCK),
  LLRSKTITEM(GLFW_KEY_,PRINT_SCREEN),  LLRSKTITEM(GLFW_KEY_,PAUSE),
  LLRSKTITEM(GLFW_KEY_,F1),            LLRSKTITEM(GLFW_KEY_,F2),
  LLRSKTITEM(GLFW_KEY_,F3),            LLRSKTITEM(GLFW_KEY_,F4),
  LLRSKTITEM(GLFW_KEY_,F5),            LLRSKTITEM(GLFW_KEY_,F6),
  LLRSKTITEM(GLFW_KEY_,F7),            LLRSKTITEM(GLFW_KEY_,F8),
  LLRSKTITEM(GLFW_KEY_,F9),            LLRSKTITEM(GLFW_KEY_,F10),
  LLRSKTITEM(GLFW_KEY_,F11),           LLRSKTITEM(GLFW_KEY_,F12),
  LLRSKTITEM(GLFW_KEY_,F13),           LLRSKTITEM(GLFW_KEY_,F14),
  LLRSKTITEM(GLFW_KEY_,F15),           LLRSKTITEM(GLFW_KEY_,F16),
  LLRSKTITEM(GLFW_KEY_,F17),           LLRSKTITEM(GLFW_KEY_,F18),
  LLRSKTITEM(GLFW_KEY_,F19),           LLRSKTITEM(GLFW_KEY_,F20),
  LLRSKTITEM(GLFW_KEY_,F21),           LLRSKTITEM(GLFW_KEY_,F22),
  LLRSKTITEM(GLFW_KEY_,F23),           LLRSKTITEM(GLFW_KEY_,F24),
  LLRSKTITEM(GLFW_KEY_,F25),           LLRSKTITEM(GLFW_KEY_,KP_0),
  LLRSKTITEM(GLFW_KEY_,KP_1),          LLRSKTITEM(GLFW_KEY_,KP_2),
  LLRSKTITEM(GLFW_KEY_,KP_3),          LLRSKTITEM(GLFW_KEY_,KP_4),
  LLRSKTITEM(GLFW_KEY_,KP_5),          LLRSKTITEM(GLFW_KEY_,KP_6),
  LLRSKTITEM(GLFW_KEY_,KP_7),          LLRSKTITEM(GLFW_KEY_,KP_8),
  LLRSKTITEM(GLFW_KEY_,KP_9),          LLRSKTITEM(GLFW_KEY_,KP_DECIMAL),
  LLRSKTITEM(GLFW_KEY_,KP_DIVIDE),     LLRSKTITEM(GLFW_KEY_,KP_MULTIPLY),
  LLRSKTITEM(GLFW_KEY_,KP_SUBTRACT),   LLRSKTITEM(GLFW_KEY_,KP_ADD),
  LLRSKTITEM(GLFW_KEY_,KP_ENTER),      LLRSKTITEM(GLFW_KEY_,KP_EQUAL),
  LLRSKTITEM(GLFW_KEY_,LEFT_SHIFT),    LLRSKTITEM(GLFW_KEY_,LEFT_CONTROL),
  LLRSKTITEM(GLFW_KEY_,LEFT_ALT),      LLRSKTITEM(GLFW_KEY_,LEFT_SUPER),
  LLRSKTITEM(GLFW_KEY_,RIGHT_SHIFT),   LLRSKTITEM(GLFW_KEY_,RIGHT_CONTROL),
  LLRSKTITEM(GLFW_KEY_,RIGHT_ALT),     LLRSKTITEM(GLFW_KEY_,RIGHT_SUPER),
  LLRSKTITEM(GLFW_KEY_,MENU),
LLRSKTEND                              // End of key codes
/* ========================================================================= */
// @ Input.KeyMods
// < Data:table=The entire list of states GLFW supports
// ? Returns a table of key/value pairs that identify a the state in which
// ? the keyboard or mouse key was pressed.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(KeyMods)                   // Beginning of key/mouse mod states
  LLRSKTITEM(GLFW_MOD_,SHIFT),         LLRSKTITEM(GLFW_MOD_,CONTROL),
  LLRSKTITEM(GLFW_MOD_,ALT),           LLRSKTITEM(GLFW_MOD_,SUPER),
  LLRSKTITEM(GLFW_MOD_,CAPS_LOCK),     LLRSKTITEM(GLFW_MOD_,NUM_LOCK),
LLRSKTEND                              // End of mouse codes
/* ========================================================================= */
// @ Input.MouseCodes
// < Data:table=The entire list of key codes GLFW supports
// ? Returns a table of key/value pairs that identify a specific mouse button
// ? with its corresponding GLFW code (for use with GetMouseButton[UB]).
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(MouseCodes)                // Beginning of mouse codes
  LLRSKTITEM(GLFW_MOUSE_BUTTON_,1),    LLRSKTITEM(GLFW_MOUSE_BUTTON_,2),
  LLRSKTITEM(GLFW_MOUSE_BUTTON_,3),    LLRSKTITEM(GLFW_MOUSE_BUTTON_,4),
  LLRSKTITEM(GLFW_MOUSE_BUTTON_,5),    LLRSKTITEM(GLFW_MOUSE_BUTTON_,6),
  LLRSKTITEM(GLFW_MOUSE_BUTTON_,7),    LLRSKTITEM(GLFW_MOUSE_BUTTON_,8),
  LLRSKTITEM(GLFW_MOUSE_BUTTON_,LEFT), LLRSKTITEM(GLFW_MOUSE_BUTTON_,RIGHT),
  LLRSKTITEM(GLFW_MOUSE_BUTTON_,MIDDLE),
LLRSKTEND                              // End of mouse codes
/* ========================================================================= */
// @ Input.States
// < Data:table=The entire list of states GLFW supports
// ? Returns a table of key/value pairs that identify a the state in which
// ? the keyboard or mouse key was pressed.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(States)                    // Beginning of keyboard/mouse states
  LLRSKTITEM(GLFW_,RELEASE),           LLRSKTITEM(GLFW_,PRESS),
  LLRSKTITEM(GLFW_,REPEAT),
LLRSKTEND                              // End of mouse codes
/* ========================================================================= */
LLRSCONSTBEGIN                         // Input.* namespace consts begin
  LLRSCONST(KeyCodes),                 LLRSCONST(KeyMods),
  LLRSCONST(MouseCodes),               LLRSCONST(States),
LLRSCONSTEND                           // Input.* namespace consts end
/* ========================================================================= */
}                                      // End of Input namespace
/* == EoF =========================================================== EoF == */
