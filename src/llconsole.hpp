/* == LLCONSOLE.HPP ======================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Console' namespace and methods for the guest to use in ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Console
/* ------------------------------------------------------------------------- */
// ! The console class allows manipulation of the game console.
/* ========================================================================= */
LLNAMESPACEBEGIN(Console)              // Console namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfConsole;             // Using console interface
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Console.Unregister
// > Command:string=The console command name.
// ? Unregisters the specified console command created by Console.Register.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Unregister)
  LCCHECKMAINSTATE();
  cConsole->UnregisterLuaCommand(LCGETCPPSTRINGNE(1, "Function"));
LLFUNCEND
/* ========================================================================= */
// $ Console.Write
// > Text:string=Text to write to console.
// ? Writes the specified line of text directly to the console with no regard
// ? to colour of text.
/* ------------------------------------------------------------------------- */
LLFUNC(Write,
  cConsole->AddLine(LCGETCPPSTRING(1, "Text"), COLOUR_CYAN));
/* ========================================================================= */
// $ Console.WriteEx
// > Text:string=Text to write to console.
// > Colour:integer=The optional colour to use.
// ? Writes the specified line of text directly to the console with the
// ? specified text colour.
/* ------------------------------------------------------------------------- */
LLFUNC(WriteEx, cConsole->AddLine(LCGETCPPSTRINGNE(1, "Text"),
  LCGETINTLGE(Colour, 2, COLOUR_BLACK, COLOUR_MAX, "Colour")));
/* ========================================================================= */
// $ Console.Register
// > Name:string=The console command name.
// > Minimum:integer=The minimum number of parameters allowed.
// > Maximum:integer=The maximum number of parameters allowed.
// > Callback:function=Pointer to the function to callback.
// ? Registers the specified console command. When the console command is
// ? called, the function specified is called.
/* ------------------------------------------------------------------------- */
LLFUNC(Register, cConsole->RegisterLuaCommand(lS));
/* ========================================================================= */
// $ Console.Exists
// > Command:string=The console command name to lookup
// < Registered:boolean=True if the command is registered
// ? Returns if the specified console command is registered
/* ------------------------------------------------------------------------- */
LLFUNCEX(Exists, 1,
  LCPUSHBOOL(cConsole->CommandIsRegistered(LCGETCPPSTRINGNE(1, "Command"))));
/* ========================================================================= */
// $ Console.StatusLeft
// > Text:string=Console status text
// ? In bot mode, this function will set the text to appear when no text is
// ? input into the input bar. Useful for customised stats. It will update
// ? every second.
/* ------------------------------------------------------------------------- */
LLFUNC(StatusLeft, cConsole->SetStatusLeft(LCGETCPPSTRING(1, "Text")));
/* ========================================================================= */
// $ Console.StatusRight
// > Text:string=Console status text
// ? In bot mode, this function will set the text to appear when no text is
// ? input into the input bar. Useful for customised stats. It will update
// ? every second.
/* ------------------------------------------------------------------------- */
LLFUNC(StatusRight,
  cConsole->SetStatusRight(LCGETCPPSTRING(1, "Text")));
/* ========================================================================= */
// $ Console.ScrollDown
// ? Scrolls the console up one line
/* ------------------------------------------------------------------------- */
LLFUNC(ScrollDown, cConsole->MoveLogDown());
/* ========================================================================= */
// $ Console.ScrollUp
// ? Scrolls the console up one line
/* ------------------------------------------------------------------------- */
LLFUNC(ScrollUp, cConsole->MoveLogUp());
/* ========================================================================= */
/* ######################################################################### */
/* ## Console.* namespace functions structure                             ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Console.* namespace functions begin
  LLRSFUNC(Exists),                    // Console command is registered?
  LLRSFUNC(Register),                  // Create console command
  LLRSFUNC(StatusLeft),                // Set console status (left side)
  LLRSFUNC(StatusRight),               // Set console status (right side)
  LLRSFUNC(ScrollDown),                // Scrolls the console down
  LLRSFUNC(ScrollUp),                  // Scrolls the console up
  LLRSFUNC(Unregister),                // Create console command
  LLRSFUNC(Write),                     // Write message to console
  LLRSFUNC(WriteEx),                   // Write message to console with colour
LLRSEND                                // Console.* namespace functions end
/* ========================================================================= */
/* ######################################################################### */
/* ## Console.* namespace constants structure                             ## */
/* ######################################################################### */
/* ========================================================================= */
// @ Console.Colours
// < Data:table=A table of const string/int key pairs
// ? Returns all the colour palette of colours.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Colours)                   // Beginning of console colours
LLRSKTITEM(COLOUR_,BLACK),             LLRSKTITEM(COLOUR_,BLUE),
LLRSKTITEM(COLOUR_,GREEN),             LLRSKTITEM(COLOUR_,CYAN),
LLRSKTITEM(COLOUR_,RED),               LLRSKTITEM(COLOUR_,GRAY),
LLRSKTITEM(COLOUR_,MAGENTA),           LLRSKTITEM(COLOUR_,BROWN),
LLRSKTITEM(COLOUR_,LGRAY),             LLRSKTITEM(COLOUR_,LBLUE),
LLRSKTITEM(COLOUR_,LGREEN),            LLRSKTITEM(COLOUR_,LCYAN),
LLRSKTITEM(COLOUR_,LRED),              LLRSKTITEM(COLOUR_,LMAGENTA),
LLRSKTITEM(COLOUR_,YELLOW),            LLRSKTITEM(COLOUR_,WHITE),
LLRSKTITEM(COLOUR_,MAX),
LLRSKTEND                              // End of console colours
/* ========================================================================= */
LLRSCONSTBEGIN                         // Console.* namespace consts begin
LLRSCONST(Colours),                    // Console colour palette
LLRSCONSTEND                           // Console.* namespace consts end
/* ========================================================================= */
LLNAMESPACEEND                         // End of Console namespace
/* == EoF =========================================================== EoF == */
