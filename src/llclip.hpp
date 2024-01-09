/* == LLCLIP.HPP =========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Interfaces Lua to the glfw clipboard functions.                     ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Clip
/* ------------------------------------------------------------------------- */
// ! Contains functions for retrieving and setting text in the Clip.
/* ========================================================================= */
namespace LLClip {                     // Clipboard namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IClipboard::P;
/* ========================================================================= **
** ######################################################################### **
** ## Clip:* member functions                                             ## **
** ######################################################################### **
** ========================================================================= */
// $ Clip:Destroy
// ? Destroys the specified Clip object.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Clip));
/* ========================================================================= */
// $ Clip:Value
// < Value:string = The value retrieved from or sent to the Clip.
// ? Returns the value sent with Clip.Set or the value retrieved with
// ? Clip.Get.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Value, 1, LCGETPTR(1, Clip)->ClipGet());
/* ========================================================================= */
// $ Clip:Id
// < Identifier:string = The identifier given at creation.
// ? Returns the identifier of this request given at creation.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Id, 1, LCGETPTR(1, Clip)->IdentGet());
/* ========================================================================= **
** ######################################################################### **
** ## Clip:* member functions structure                                   ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Clip:* member functions begin
  LLRSFUNC(Destroy), LLRSFUNC(Id), LLRSFUNC(Value),
LLRSEND                                // Clip:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## Clip.* namespace functions                                          ## **
** ######################################################################### **
** ========================================================================= */
// $ Clip.Set
// > Identifier:string=The identifier for the request.
// > Text:string=The text to copy.
// > Completion:function=The function to call when completed.
// ? Stores the specified text into the operating system's clipboard and
// ? executes the specified completion function when done as the window thread
// ? has to request the operation and then forward the request back to the
// ? engine thread.
/* ------------------------------------------------------------------------- */
LLFUNC(Set, LCCLASSCREATE(Clip)->ClipSetAsync(lS));
/* ========================================================================= */
// $ Clip.Get
// > Identifier:string=The identifier for the request.
// > Completion:function=The function to call when completed.
// ? Retrieves text from the operating systems clipboard. Executes the
// ? specified function when completed as the window thread has to request
// ? the operation and then forward the request back to the engine thread.
/* ------------------------------------------------------------------------- */
LLFUNC(Get, LCCLASSCREATE(Clip)->ClipGetAsync(lS));
/* ========================================================================= **
** ######################################################################### **
** ## Clip.* namespace functions structure                                ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Clip.* namespace functions begin
  LLRSFUNC(Get), LLRSFUNC(Set),
LLRSEND                                // Clip.* namespace functions end
/* ========================================================================= */
}                                      // End of Util namespace
/* == EoF =========================================================== EoF == */
