/* == LLAUDIO.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Audio' namespace and methods for the guest to use in   ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Audio
/* ------------------------------------------------------------------------- */
// ! The audio class allows control of audio subsystem.
/* ========================================================================= */
namespace NsAudio {                    // Audio namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfAudio;               // Using audio namespace
/* ========================================================================= */
// $ Audio.GetGlobalVolume
// < Volume:number=Current master volume (0 to 1).
// ? Returns global/master volume.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetGlobalVolume, 1, LCPUSHNUM(cSources->fGVolume.load()));
/* ========================================================================= */
// $ Audio.GetStreamVolume
// < Volume:number=Current streams volume (0 to 1).
// ? Returns master volume of all stream classes.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetStreamVolume, 1, LCPUSHNUM(cSources->fMVolume.load()));
/* ========================================================================= */
// $ Audio.GetSampleVolume
// < Volume:number=Current samples volume (0 to 1).
// ? Returns master volume of all sample classes.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetSampleVolume, 1, LCPUSHNUM(cSources->fSVolume.load()));
/* ========================================================================= */
// $ Audio.GetVideoVolume
// < Volume:number=Current samples volume (0 to 1).
// ? Returns master volume of all video classes.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetVideoVolume, 1, LCPUSHNUM(cSources->fVVolume.load()));
/* ========================================================================= */
// $ Audio.SetGlobalVolume
// > Volume:number=New master volume (0 to 1).
// ? Sets new master/global volume of all current and new streams/samples. This
// ? does not modify the cvar which controls the default sample volume.
/* ------------------------------------------------------------------------- */
LLFUNC(SetGlobalVolume,
  cAudio->SetGlobalVolume(LCGETNUMLG(ALfloat, 1, 0, 1, "Volume")));
/* ========================================================================= */
// $ Audio.StreamSetVolume
// > Volume:number=New streams volume (0 to 1).
// ? Sets new volume of all current and new streams. This does not modify the
// ? cvar which controls the default sample volume.
/* ------------------------------------------------------------------------- */
LLFUNC(StreamSetVolume,
  StreamSetVolume(LCGETNUMLG(ALfloat, 1, 0, 1, "Volume")));
/* ========================================================================= */
// $ Audio.SetSampleVolume
// > Volume:number=New samples volume (0 to 1).
// ? Sets new volume of all current and new samples. This does not modify the
// ? cvar which controls the default sample volume.
/* ------------------------------------------------------------------------- */
LLFUNC(SetSampleVolume,
  SetSampleVolume(LCGETNUMLG(ALfloat, 1, 0, 1,"Volume")));
/* ========================================================================= */
// $ Audio.VideoSetVolume
// > Volume:number=New samples volume (0 to 1).
// ? Sets new volume of all current and new videos. This does not modify the
// ? cvar which controls the default video volume.
/* ------------------------------------------------------------------------- */
LLFUNC(VideoSetVolume, VideoSetVolume(LCGETNUMLG(ALfloat, 1, 0, 1, "Volume")));
/* ========================================================================= */
// $ Audio.SetPosition
// > X:number=New X listener position.
// > Y:number=New Y listener position.
// > Z:number=New Z listener position.
// ? Sets the new audio listener position. This would normally be equal to the
// ? position of the game's protagonist character (if applicable).
/* ------------------------------------------------------------------------- */
LLFUNC(SetPosition, cAudio->SetPosition(LCGETNUM(ALfloat, 1, "X"),
  LCGETNUM(ALfloat, 2, "Y"), LCGETNUM(ALfloat, 3, "Z")));
/* ========================================================================= */
// $ Audio.SetOrientation
// > XLook:number=New X listener look orientation.
// > YLook:number=New Y listener look orientation.
// > ZLook:number=New Z listener look orientation.
// > XUp:number=New X listener up orientation.
// > YUp:number=New Y listener up orientation.
// > ZUp:number=New Z listener up orientation.
// ? Sets the new audio listener orientation. This would normally be equal to
// ? orientation of the game's protagonist character (if applicable).
/* ------------------------------------------------------------------------- */
LLFUNC(SetOrientation, cAudio->SetOrientation(
  LCGETNUM(ALfloat, 1, "XLook"), LCGETNUM(ALfloat, 2, "YLook"),
  LCGETNUM(ALfloat, 3, "ZLook"), LCGETNUM(ALfloat, 4, "XUp"),
  LCGETNUM(ALfloat, 5, "YUp"),   LCGETNUM(ALfloat, 6, "ZUp")
));
/* ========================================================================= */
// $ Audio.SetVelocity
// > X:number=New X listener velocity.
// > Y:number=New Y listener velocity.
// > Z:number=New Z listener velocity.
// ? Sets the new audio listener velocity. This would normally be equal to
// ? velocity of the game's protagonist character (if applicable).
/* ------------------------------------------------------------------------- */
LLFUNC(SetVelocity, cAudio->SetVelocity(LCGETNUM(ALfloat, 1, "X"),
  LCGETNUM(ALfloat, 2, "Y"), LCGETNUM(ALfloat, 3, "Z")));
/* ========================================================================= */
// $ Audio.GetPBDeviceName
// > Id:integer=The playback audio device id.
// < Name:string=The name of the audio playback device.
// ? Returns the name of the playback audio device at id.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetPBDeviceName, 1, LCPUSHXSTR(cAudio->GetPlaybackDeviceById(
  LCGETINTLG(size_t, 1, 0, cAudio->GetNumPlaybackDevices(), "Index"))));
/* ========================================================================= */
// $ Audio.GetNumPBDevices
// < Count:integer=The number of playback devices detected.
// ? Returns the number of audio playback devices detected by OpenAL.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetNumPBDevices, 1, LCPUSHINT(cAudio->GetNumPlaybackDevices()));
/* ========================================================================= */
// $ Audio.Reset
// < Result:boolean = Was the event sent successfully?
// ? Resets the audio subsystem completely. Currently playing streams and
// ? samples should resume playing after the reset. The function will return
// ? 'false' if the request was already sent.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Reset, 1, LCPUSHBOOL(cAudio->ReInit()));
/* ========================================================================= */
/* ######################################################################### */
/* ## Audio.* namespace functions structure                               ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Audio.* namespace functions begin
  LLRSFUNC(GetGlobalVolume),           LLRSFUNC(GetNumPBDevices),
  LLRSFUNC(GetPBDeviceName),           LLRSFUNC(GetSampleVolume),
  LLRSFUNC(GetStreamVolume),           LLRSFUNC(GetVideoVolume),
  LLRSFUNC(Reset),                     LLRSFUNC(SetGlobalVolume),
  LLRSFUNC(SetOrientation),            LLRSFUNC(SetPosition),
  LLRSFUNC(SetSampleVolume),           LLRSFUNC(SetVelocity),
  LLRSFUNC(StreamSetVolume),           LLRSFUNC(VideoSetVolume),
LLRSEND                                // Audio.* namespace functions end
/* ========================================================================= */
}                                      // End of Audio namespace
/* == EoF =========================================================== EoF == */
