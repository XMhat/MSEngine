/* == LLFTF.HPP ============================================================ **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Ftf' namespace and methods for the guest to use in     ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Ftf
/* ------------------------------------------------------------------------- */
// ! This allows the programmer to maniuplate supported freetype fonts forms
// ! asynchronously and send the objects to a font rendering object.
/* ========================================================================= */
namespace LLFtf {                      // Ftf namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IFtf::P;
using namespace IStd::P;               using namespace Lib::OS::GlFW;
/* ========================================================================= **
** ######################################################################### **
** ## Ftf:* member functions                                              ## **
** ######################################################################### **
** ========================================================================= */
// $ Ftf:Name
// < Name:string=The name of the freetype font
// ? Returns the name of the specified object when it was created.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Name, 1, LCPUSHVAR(LCGETPTR(1, Ftf)->IdentGet()));
/* ========================================================================= */
// $ Ftf:Family
// < Name:string=The internal name of the freetype font
// ? Returns the internal name of the actual freetype font. (e.g. Arial)
/* ------------------------------------------------------------------------- */
LLFUNCEX(Family, 1, LCPUSHVAR(LCGETPTR(1, Ftf)->GetFamily()));
/* ========================================================================= */
// $ Ftf:Style
// < Name:string=The internal style name of the freetype font
// ? Returns the internal name of the actual freetype font. (e.g. Bold)
/* ------------------------------------------------------------------------- */
LLFUNCEX(Style, 1, LCPUSHVAR(LCGETPTR(1, Ftf)->GetStyle()));
/* ========================================================================= */
// $ Ftf:Glyphs
// < Count:integer=Number of glyphs available.
// ? Returns the internal number of glyphs supported by this freetype font.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Glyphs, 1, LCPUSHVAR(LCGETPTR(1, Ftf)->GetGlyphCount()));
/* ========================================================================= */
// $ Ftf:Destroy
// ? Destroys the freetype font and frees all the memory associated with it.
// ? The object will no longer be useable after this call and an error will be
// ? generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Ftf));
/* ========================================================================= **
** ######################################################################### **
** ## Ftf:* member functions structure                                    ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Ftf:* member functions begin
  LLRSFUNC(Destroy), LLRSFUNC(Name),   LLRSFUNC(Family),
  LLRSFUNC(Style),   LLRSFUNC(Glyphs),
LLRSEND                                // Ftf:* member functions end
/* ========================================================================= */
// $ Ftf.FileAsync
// > Filename:string=The filename of the encoded waveform to load
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call when there is progress
// > SuccessFunc:function=The function to call when the file is laoded
// ? Loads a audio file off the main thread. The callback functions send an
// ? argument to the Ftf object that was created.
/* ------------------------------------------------------------------------- */
LLFUNC(FileAsync, LCCLASSCREATE(Ftf)->InitAsyncFile(lS));
/* ========================================================================= */
// $ Ftf.File
// > Filename:string=The filename of the audio file to load
// < Handle:Ftf=The Ftf object
// ? Loads a audio sample on the main thread from the specified file on disk.
// ? Returns the Ftf object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(File, 1, LCCLASSCREATE(Ftf)->InitFile(LCGETCPPFILE(1, "File"),
  LCGETNUMLG(GLfloat,      2, 1, 4096, "Width"),
  LCGETNUMLG(GLfloat,      3, 1, 4096, "Height"),
  LCGETINTLG(unsigned int, 4, 1, 1024, "DPIWidth"),
  LCGETINTLG(unsigned int, 5, 1, 1024, "DPIHeight"),
  LCGETNUMLG(GLfloat,      6, 0, 1024, "Outline")));
/* ========================================================================= */
// $ Ftf.ArrayAsync
// > Id:String=The identifier of the string
// > Data:array=The data of the audio file to load
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call when there is progress
// > SuccessFunc:function=The function to call when the audio file is laoded
// ? Loads an audio file off the main thread from the specified array object.
// ? The callback functions send an argument to the Ftf object that was
// ? created.
/* ------------------------------------------------------------------------- */
LLFUNC(ArrayAsync, LCCLASSCREATE(Ftf)->InitAsyncArray(lS));
/* ========================================================================= */
// $ Ftf.Asset
// > Id:String=The identifier of the string
// > Data:Asset=The file data of the audio file to load
// < Handle:Ftf=The Ftf object
// ? Loads an audio file on the main thread from the specified array object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Asset, 1, LCCLASSCREATE(Ftf)->InitArray(
  LCGETCPPSTRINGNE(1, "Identifier"), StdMove(*LCGETPTR(2, Asset)),
  LCGETNUMLG(GLfloat, 3, 1, 4096, "Width"),
  LCGETNUMLG(GLfloat, 4, 1, 4096, "Height"),
  LCGETINTLG(unsigned int, 5, 1, 1024, "DPIWidth"),
  LCGETINTLG(unsigned int, 6, 1, 1024, "DPIHeight"),
  LCGETNUMLG(GLfloat, 7, 0, 1024, "Outline")));
/* ========================================================================= */
// $ Ftf.WaitAsync
// ? Halts main-thread execution until all async Ftf events have completed
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, cFtfs->WaitAsync());
/* ========================================================================= **
** ######################################################################### **
** ## Ftf.* namespace functions structure                                 ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Ftf.* namespace functions begin
  LLRSFUNC(Asset),     LLRSFUNC(ArrayAsync), LLRSFUNC(File),
  LLRSFUNC(FileAsync), LLRSFUNC(WaitAsync),
LLRSEND                                // Ftf.* namespace functions end
/* ========================================================================= */
}                                      // End of Ftf namespace
/* == EoF =========================================================== EoF == */
