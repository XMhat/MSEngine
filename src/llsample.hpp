/* == LLSAMPLE.HPP ========================================================= **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Sample' namespace and methods for the guest to use in  ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Sample
/* ------------------------------------------------------------------------- */
// ! The sample class allows loading of audio files into a single buffer.
// ! Samples can also be played with 3D positional audio.
/* ========================================================================= */
namespace LLSample {                   // Sample namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IPcm::P;               using namespace ISample::P;
using namespace Lib::OpenAL;
/* ========================================================================= **
** ######################################################################### **
** ## Sample:* member functions                                           ## **
** ######################################################################### **
** ========================================================================= */
// $ Sample:Play
// > Gain:number=Volume of sample to play at.
// > Pan:integer=Current 2D position.
// > Pitch:integer=Pitch of played sample.
// > Loop:boolean=Loop the sample?
// ? Plays the specified sample at the specified gain and pitch with the
// ? specified 2D panning position. The source that this sample spawns is
// ? managed internally. See PlayEx() if you need to manage the source.
/* ------------------------------------------------------------------------- */
LLFUNC(Play, LCGETPTR(1, Sample)->Play(
  LCGETNUMLG(ALfloat, 2,  0.0f, 1.0f, "Gain"),
  LCGETNUMLG(ALfloat, 3, -1.0f, 1.0f, "Pan"),
  LCGETNUM(ALfloat, 4, "Pitch"), LCGETBOOL(5,"Loop") ? AL_TRUE : AL_FALSE));
/* ========================================================================= */
// $ Sample:Spawn
// < Class:Source=The source created from the sample or left channel source.
// < Class:Source=(Optional) If in stereo, the right channel source.
// ? Spawns a new source from the sample. It is initially stopped and
// ? uninitialised in a 3-D state.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Spawn, 2, LCGETPTR(1, Sample)->Spawn(lS));
/* ========================================================================= */
// $ Sample:PlayEx
// < Class:Source=The source created from the sample or left channel source.
// < Class:Source=(Optional) If in stereo, the right channel source.
// > Gain:number=Volume of sample to play at.
// > Pan:integer=Current 2D position.
// > Pitch:integer=Pitch of played sample.
// > Loop:boolean=Loop the sample?
// ? Plays the specified sample at the specified gain and pitch with the
// ? specified 2D panning position. The source that this sample spawns is
// ? managed internally. See PlayEx() if you need to manage the source.
/* ------------------------------------------------------------------------- */
LLFUNCEX(PlayEx, 2, LCGETPTR(1, Sample)->Play(lS));
/* ========================================================================= */
// $ Sample:Stop
// < Count:integer=Number of source buffers stopped;
// ? Stops playing the specified sample.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Stop, 1, LCPUSHVAR(LCGETPTR(1, Sample)->Stop()));
/* ========================================================================= */
// $ Sample:Duration
// < Duration:number=Duration of buffer in seconds
// ? Returns the duration of the buffer in seconds
/* ------------------------------------------------------------------------- */
LLFUNCEX(Duration, 1, LCPUSHVAR(LCGETPTR(1, Sample)->GetDuration()));
/* ========================================================================= */
// $ Sample:Id
// < Id:integer=The id number of the Sample object.
// ? Returns the unique id of the Sample object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Id, 1, LCPUSHVAR(LCGETPTR(1, Sample)->CtrGet()));
/* ========================================================================= */
// $ Sample:Name
// < Name:string=Name of the sample.
// ? If this sample was loaded by a filename or it was set with a custom id.
// ? This function returns that name which was assigned to it.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Name, 1, LCPUSHVAR(LCGETPTR(1, Sample)->IdentGet()));
/* ========================================================================= */
// $ Sample:Destroy
// ? Destroys the sample object and frees all the memory associated with it.
// ? The object will no longer be useable after this call and an error will be
// ? generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Sample));
/* ========================================================================= **
** ######################################################################### **
** ## Sample:* member functions structure                                 ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Sample:* member functions begin
  LLRSFUNC(Destroy), LLRSFUNC(Duration), LLRSFUNC(Id),    LLRSFUNC(Name),
  LLRSFUNC(Play),    LLRSFUNC(PlayEx),   LLRSFUNC(Spawn), LLRSFUNC(Stop),
LLRSEND                                // Sample:* member functions end
/* ========================================================================= */
// $ Sample.Create
// > Handle:Pcm=The pcm sample class to load
// < Handle:Sample=The handle to the specified sample
// ? Loads the specified number of samples and returns a handle.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Create, 1, LCCLASSCREATE(Sample)->InitSample(*LCGETPTR(1, Pcm)));
/* ========================================================================= **
** ######################################################################### **
** ## Sample.* namespace functions structure                              ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Sample.* namespace functions begin
  LLRSFUNC(Create),
LLRSEND                                // Sample.* namespace functions end
/* ========================================================================= */
}                                      // End of Sample namespace
/* == EoF =========================================================== EoF == */
