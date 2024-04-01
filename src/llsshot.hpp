/* == LLSSHOT.HPP ========================================================== **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Allows taking of asynchronous screenshots                           ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % SShot
/* ------------------------------------------------------------------------- */
// ! Contains functions for making a screenshot
/* ========================================================================= */
namespace LLSShot {                    // Screenshot namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IFbo::P;               using namespace ISShot::P;
using namespace IString::P;
/* ========================================================================= **
** ######################################################################### **
** ## SShot:* member functions                                            ## **
** ######################################################################### **
** ========================================================================= */
// $ SShot:Destroy
// ? Destroys the specified screenshot object.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, SShot));
/* ========================================================================= */
// $ SShot:Id
// < Id:integer=The id number of the SShot object.
// ? Returns the unique id of the SShot object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Id, 1, LCPUSHVAR(LCGETPTR(1, SShot)->CtrGet()));
/* ========================================================================= */
// $ SShot:Name
// < Name:string=The name of the SShot object.
// ? Returns the filename generated on this screenshot object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Name, 1, LCPUSHVAR(LCGETPTR(1, SShot)->IdentGet()));
/* ========================================================================= **
** ######################################################################### **
** ## SShot:* member functions structure                                  ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // SShot:* member functions begin
  LLRSFUNC(Destroy), LLRSFUNC(Id), LLRSFUNC(Name),
LLRSEND                                // SShot:* member functions end
/* ========================================================================= **
** ######################################################################### **
** ## SShot.* namespace functions                                         ## **
** ######################################################################### **
** ========================================================================= */
// $ SShot.Screen
// < Object:SShot=The screenshot object created.
// ? Takes a screenshot of the screen.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Screen, 1, LCCLASSCREATE(SShot)->DumpMain());
/* ========================================================================= */
// $ SShot.Fbo
// > Dest:Fbo=The frame buffer to dump.
// > File:string=(Optional) File to save to.
// < Object:SShot=The screenshot object created.
// ? Takes a screenshot of the specified FBO. You must completely omit the
// ? 'file' parameter to use an engine generated file.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Fbo, 1, LCCLASSCREATE(SShot)->DumpFBO(*LCGETPTR(1, Fbo),
  LuaUtilStackSize(lS) < 3 ? cCommon->Blank() : LCGETCPPFILE(3, "File")));
/* ========================================================================= **
** ######################################################################### **
** ## SShot.* namespace functions structure                               ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // SShot.* namespace functions begin
  LLRSFUNC(Fbo), LLRSFUNC(Screen),
LLRSEND                                // SShot.* namespace functions end
/* ========================================================================= */
}                                      // End of Util namespace
/* == EoF =========================================================== EoF == */
