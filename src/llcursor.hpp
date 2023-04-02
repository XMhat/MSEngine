/* == LLCURSOR.HPP ========================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Cursor' namespace and methods for the guest to use in  ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Cursor
/* ------------------------------------------------------------------------- */
// ! The cursor class contains functions to create custom hardware cursors.
/* ========================================================================= */
namespace NsCursor {                   // Cursor namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfCursor;              // Using cursor namespace
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Cursor:Activate
// ? Activates the currently loaded cursor.
/* ------------------------------------------------------------------------- */
LLFUNC(Activate, LCGETPTR(1, Cursor)->Activate());
/* ========================================================================= */
// $ Cursor:Destroy
// ? Destroys the cursor and frees all the memory associated with it. The
// ? object will no longer be useable after this call and an error will be
// ? generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Cursor));
/* ========================================================================= */
/* ######################################################################### */
/* ## Cursor:* member functions structure                                 ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Cursor:* member functions begin
  LLRSFUNC(Activate), LLRSFUNC(Destroy),
LLRSEND                                // Cursor:* member functions end
/* ========================================================================= */
// $ Cursor.Reset
// ? Resets to the default cursor graphic.
/* ------------------------------------------------------------------------- */
LLFUNC(Reset, CursorReset());
/* ========================================================================= */
// $ Cursor.Standard
// > Id:integer=The id number of the cursor to load.
// < Data:Cursor=The cursor class returned.
// ? Loads a new standard cursor from the operating system.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Standard, 1, LCCLASSCREATE(Cursor)->
  InitStandard(LCGETINTLGE(int, 1, 0, numeric_limits<int>::max(), "Id")));
/* ========================================================================= */
/* ######################################################################### */
/* ## Cursor.* namespace functions structure                              ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Cursor.* namespace functions begin
  LLRSFUNC(Reset), LLRSFUNC(Standard),
LLRSEND                                // Cursor.* namespace functions end
/* ========================================================================= */
/* ######################################################################### */
/* ## Cursor.* namespace constants structure                              ## */
/* ######################################################################### */
/* ========================================================================= */
// @ Cursor.Codes
// < Data:table=The entire list of cursor codes GLFW supports
// ? Returns a table of key/value pairs that identify a specific cursor with
// ? its corresponding GLFW code (for use with SetCursorShape).
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Codes)                     // Beginning of cursor codes
  LLRSKTITEMEX(GLFW_,ARROW,_CURSOR),   LLRSKTITEMEX(GLFW_,IBEAM,_CURSOR),
  LLRSKTITEMEX(GLFW_,CROSSHAIR,_CURSOR), LLRSKTITEMEX(GLFW_,HAND,_CURSOR),
  LLRSKTITEMEX(GLFW_,HRESIZE,_CURSOR), LLRSKTITEMEX(GLFW_,VRESIZE,_CURSOR),
// LLRSKTITEMEX(GLFW_,RESIZE_NWSE,_CURSOR),
// LLRSKTITEMEX(GLFW_,RESIZE_NESW,_CURSOR),
// LLRSKTITEMEX(GLFW_,RESIZE_ALL,_CURSOR),
// LLRSKTITEMEX(GLFW_,NOT_ALLOWED,_CURSOR),
LLRSKTEND                              // End of cursor codes
/* ========================================================================= */
LLRSCONSTBEGIN                         // Cursor.* namespace consts begin
  LLRSCONST(Codes),
LLRSCONSTEND                           // Cursor.* namespace consts end
/* ========================================================================= */
}                                      // End of Cursor namespace
/* == EoF =========================================================== EoF == */
