/* == LLPCM.HPP ============================================================ **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Pcm' namespace and methods for the guest to use in     ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Pcm
/* ------------------------------------------------------------------------- */
// ! This allows the programmer to load pcm wave forms or decompress encoded
// ! pcm waveforms and send the objects to the OpenAL if needed. You can use
// ! the command 'pcmfmts' to see what formats are usable which are WAV
// ! (WaveForm Audio StrFormat) PCM/INT/FLOAT/LE, CAF (CoreAudio StrFormat),
// ! PCM/INT/FLOAT/LE/BE, OGG (OGG Vorbis) and MP3 (MPEG-Layer III).
/* ========================================================================= */
namespace LLPcm {                      // Pcm namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IAsset::P;             using namespace IPcm::P;
using namespace IStd::P;               using namespace IPcmLib::P;
/* ========================================================================= **
** ######################################################################### **
** ## Pcm:* member functions                                              ## **
** ######################################################################### **
** ========================================================================= */
// $ Pcm:Name
// < Name:string=The name of the object
// ? Returns the name of the specified object when it was created, or if used
// ? by another function, a small trace of who took ownership of it prefixed
// ? with an exclamation mark (!).
/* ------------------------------------------------------------------------- */
LLFUNCEX(Name, 1, LCPUSHVAR(LCGETPTR(1, Pcm)->IdentGet()));
/* ========================================================================= */
// $ Pcm:Destroy
// ? Destroys the pcm object and frees all the memory associated with it. The
// ? object will no longer be useable after this call and an error will be
// ? generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Pcm));
/* ========================================================================= **
** ######################################################################### **
** ## Pcm:* member functions structure                                    ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Pcm:* member functions begin
  LLRSFUNC(Destroy),                   // Destroy the internal pcm object
  LLRSFUNC(Name),                      // Name or filename of the pcm object
LLRSEND                                // Pcm:* member functions end
/* ========================================================================= */
// $ Pcm.FileAsync
// > Filename:string=The filename of the encoded waveform to load
// > Flags:Integer=Load flags
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call when there is progress
// > SuccessFunc:function=The function to call when the file is laoded
// ? Loads a audio file off the main thread. The callback functions send an
// ? argument to the pcm object that was created.
/* ------------------------------------------------------------------------- */
LLFUNC(FileAsync, LCCLASSCREATE(Pcm)->InitAsyncFile(lS));
/* ========================================================================= */
// $ Pcm.File
// > Filename:string=The filename of the audio file to load
// > Flags:Integer=Load flags
// < Handle:Pcm=The pcm object
// ? Loads a audio sample on the main thread from the specified file on disk.
// ? Returns the pcm object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(File, 1, LCCLASSCREATE(Pcm)->InitFile(LCGETCPPFILE(1, "File"),
  LCGETFLAGS(PcmFlagsConst, 2, PL_MASK, "Flags")));
/* ========================================================================= */
// $ Pcm.ArrayAsync
// > Id:String=The identifier of the string
// > Data:array=The data of the audio file to load
// > Flags:Integer=Load flags
// > ErrorFunc:function=The function to call when there is an error
// > ProgressFunc:function=The function to call when there is progress
// > SuccessFunc:function=The function to call when the audio file is laoded
// ? Loads an audio file off the main thread from the specified array object.
// ? The callback functions send an argument to the Pcm object that was
// ? created.
/* ------------------------------------------------------------------------- */
LLFUNC(ArrayAsync, LCCLASSCREATE(Pcm)->InitAsyncArray(lS));
/* ========================================================================= */
// $ Pcm.Asset
// > Id:String=The identifier of the string
// > Data:Asset=The file data of the audio file to load
// < Handle:Pcm=The pcm object
// ? Loads an audio file on the main thread from the specified array object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Asset, 1, LCCLASSCREATE(Pcm)->InitArray(
  LCGETCPPSTRINGNE(1, "Identifier"), StdMove(*LCGETPTR(2, Asset)),
  LCGETFLAGS(PcmFlagsConst, 3, PL_MASK, "Flags")));
/* ========================================================================= */
// $ Pcm.Raw
// > Identifier:string=Identifier of the sample.
// > Data:Asset=Sample PCM data.
// > Rate:integer=Sample rate.
// > Channels:integer=Sample channels.
// > Bits:integer=Sample ibts per channel.
// < Handle:Pcm=The pcm object
// ? Loads an audio file on the main thread from the specified array object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Raw, 1, LCCLASSCREATE(Pcm)->InitRaw(
  LCGETCPPSTRINGNE(1, "Identifier"), StdMove(*LCGETPTR(2, Asset)),
  LCGETINT(unsigned int, 3, "Rate"), LCGETINT(unsigned int, 4, "Channel"),
  LCGETINT(unsigned int, 5, "Bits")));
/* ========================================================================= */
// $ Pcm.WaitAsync
// ? Halts main-thread execution until all async pcm events have completed
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, cPcms->WaitAsync());
/* ========================================================================= **
** ######################################################################### **
** ## Pcm.* namespace functions structure                                 ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Pcm.* namespace functions begin
  LLRSFUNC(ArrayAsync), LLRSFUNC(Asset), LLRSFUNC(File),
  LLRSFUNC(FileAsync),  LLRSFUNC(Raw),   LLRSFUNC(WaitAsync),
LLRSEND                                // Pcm.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Pcm.* namespace constants                                           ## **
** ######################################################################### **
** ========================================================================= */
// @ Pcm.Flags
// < Codes:table=The table of key/value pairs of available flags
// ? Returns the pcm flags available. Returned as key/value pairs. The
// ? value is a unique identifier to the flag.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Flags)                     // Beginning of pcm loading flags
  LLRSKTITEM(PL_,NONE),                LLRSKTITEM(PL_,FCE_CAF),
  LLRSKTITEM(PL_,FCE_MP3),             LLRSKTITEM(PL_,FCE_OGG),
  LLRSKTITEM(PL_,FCE_WAV),
LLRSKTEND                              // End of pcm loading flags
/* ========================================================================= **
** ######################################################################### **
** ## Pcm.* namespace constants structure                                 ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Pcm.* namespace consts begin
  LLRSCONST(Flags),
LLRSCONSTEND                           // Pcm.* namespace consts end
/* ========================================================================= */
}                                      // End of Pcm namespace
/* == EoF =========================================================== EoF == */
