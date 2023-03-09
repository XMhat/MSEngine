/* == LLSOURCE.HPP ========================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Source' namespace and methods for the guest to use in  ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Source
/* ------------------------------------------------------------------------- */
// ! The source is a sound channel in OpenAL that can be manipulated such as */
// ! changing volume, pitch, position and other things. It can only be       */
// ! created by the Sample:Play() function.
/* ========================================================================= */
LLNAMESPACEBEGIN(Source)               // Stream namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfSource;              // Using source interface
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Source:GetElapsed
// < Seconds:number=Number of seconds elapsed.
// ? Returns the number of seconds elapsed in the source.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetElapsed, 1, LCPUSHNUM(LCGETPTR(1, Source)->GetElapsed()));
/* ========================================================================= */
// $ Source:SetElapsed
// > Seconds:number=Number of seconds elapsed.
// ? Sets the number of seconds elapsed in the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetElapsed,
  LCGETPTR(1, Source)->SetElapsed(LCGETNUM(ALfloat, 2, "Seconds")));
/* ========================================================================= */
// $ Source:GetGain
// < Gain:number=The current gain value.
// ? Returns the current gain of the source.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetGain, 1, LCPUSHNUM(LCGETPTR(1, Source)->GetGain()));
/* ========================================================================= */
// $ Source:SetGain
// > Gain:number=The new gain value.
// ? Sets the new gain of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetGain, LCGETPTR(1, Source)->SetGain(LCGETNUM(ALfloat, 2, "Gain")));
/* ========================================================================= */
// $ Source:GetMinGain
// < Gain:number=The current gain value.
// ? Returns the current minimum gain of the source.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetMinGain, 1, LCPUSHNUM(LCGETPTR(1, Source)->GetMinGain()));
/* ========================================================================= */
// $ Source:SetMinGain
// > Gain:number=The new minimum gain value.
// ? Sets the new minimum gain gain of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetMinGain,
  LCGETPTR(1, Source)->SetMinGain(LCGETNUM(ALfloat, 2, "MinGain")));
/* ========================================================================= */
// $ Source:GetMaxGain
// < Gain:number=The current gain value.
// ? Returns the current maximum gain of the source.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetMaxGain, 1, LCPUSHNUM(LCGETPTR(1, Source)->GetMaxGain()));
/* ========================================================================= */
// $ Source:SetMaxGain
// > Gain:number=The new maximum  gain value.
// ? Sets a new maximum gain of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetMaxGain,
  LCGETPTR(1, Source)->SetMaxGain(LCGETNUM(ALfloat, 2, "MaxGain")));
/* ========================================================================= */
// $ Source:GetPitch
// < Pitch:number=The current pitch value.
// ? Returns the current pitch of the source.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetPitch, 1, LCPUSHNUM(LCGETPTR(1, Source)->GetPitch()));
/* ========================================================================= */
// $ Source:SetPitch
// > Pitch:number=The new pitch value.
// ? Sets a new pitch value of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetPitch, LCGETPTR(1, Source)->
  SetPitch(LCGETNUM(ALfloat, 2, "Pitch")));
/* ========================================================================= */
// $ Source:GetRefDist
// < Distance:number=The current reference distance.
// ? Returns the current reference distance of the source.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetRefDist, 1, LCPUSHNUM(LCGETPTR(1, Source)->GetRefDist()));
/* ========================================================================= */
// $ Source:SetRefDist
// > Distance:number=The new reference distance.
// ? Sets the new reference distance of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetRefDist,
  LCGETPTR(1, Source)->SetRefDist(LCGETNUM(ALfloat, 2, "RefDist")));
/* ========================================================================= */
// $ Source:GetRollOff
// < RollOff:number=The current rolloff.
// ? Returns the current rolloff of the source.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetRollOff, 1, LCPUSHNUM(LCGETPTR(1, Source)->GetRollOff()));
/* ========================================================================= */
// $ Source:SetRollOff
// > RollOff:number=The new rolloff.
// ? Sets the new rolloff of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetRollOff,
  LCGETPTR(1, Source)->SetRollOff(LCGETNUM(ALfloat, 2, "RefDist")));
/* ========================================================================= */
// $ Source:GetMaxDist
// < Distance:number=The current maximum distance.
// ? Returns the current maximum distance of the source.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetMaxDist, 1, LCPUSHNUM(LCGETPTR(1, Source)->GetMaxDist()));
/* ========================================================================= */
// $ Source:SetMaxDist
// > Distance:number=The new maximum distance.
// ? Sets the maximum distance of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetMaxDist,
  LCGETPTR(1, Source)->SetMaxDist(LCGETNUM(ALfloat, 2, "MaxDist")));
/* ========================================================================= */
// $ Source:GetLooping
// < State:boolean=Is the source looping?
// ? Returns if the source is looping.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetLooping, 1, LCPUSHBOOL(LCGETPTR(1, Source)->GetLooping()));
/* ========================================================================= */
// $ Source:SetLooping
// > State:boolean=The new looping state
// ? Sets if the source should be looping.
/* ------------------------------------------------------------------------- */
LLFUNC(SetLooping, LCGETPTR(1, Source)->SetLooping(LCGETBOOL(2, "Looping")));
/* ========================================================================= */
// $ Source:GetRelative
// < State:boolean=Is the source relative?
// ? Returns if the source is relative.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetRelative, 1, LCPUSHBOOL(LCGETPTR(1, Source)->GetRelative()));
/* ========================================================================= */
// $ Source:SetRelative
// > State:boolean=Sets if the source is relative.
// ? Sets the new relative state of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetRelative,
  LCGETPTR(1, Source)->SetRelative(LCGETBOOL(2, "Relative")));
/* ========================================================================= */
// $ Source:GetDirection
// < X:number=Current X direction.
// < Y:number=Current Y direction.
// < Z:number=Current Z direction.
// ? Returns the current direction of the source.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(GetDirection)
  ALfloat fX, fY, fZ;
  LCGETPTR(1, Source)->GetDirection(fX, fY, fZ);
  LCPUSHNUM(fX); LCPUSHNUM(fY); LCPUSHNUM(fZ);
LLFUNCENDEX(3);
/* ========================================================================= */
// $ Source:SetDirection
// > X:number=The new X direction.
// > Y:number=The new Y direction.
// > Z:number=The new Z direction.
// ? Sets the new direction of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetDirection,
  LCGETPTR(1, Source)->SetDirection(LCGETNUM(ALfloat, 2, "X"),
                                    LCGETNUM(ALfloat, 3, "Y"),
                                    LCGETNUM(ALfloat, 4, "Z")));
/* ========================================================================= */
// $ Source:GetPosition
// < X:number=Current X position.
// < Y:number=Current Y position.
// < Z:number=Current Z position.
// ? Returns the current position of the source.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(GetPosition)
  ALfloat fX, fY, fZ;
  LCGETPTR(1, Source)->GetPosition(fX, fY, fZ);
  LCPUSHNUM(fX); LCPUSHNUM(fY); LCPUSHNUM(fZ);
LLFUNCENDEX(3);
/* ========================================================================= */
// $ Source:SetPosition
// < X:number=The new X position.
// < Y:number=The new Y position.
// < Z:number=The new Z position.
// ? Sets the new position of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetPosition,
  LCGETPTR(1, Source)->SetPosition(LCGETNUM(ALfloat, 2, "X"),
                                   LCGETNUM(ALfloat, 3, "Y"),
                                   LCGETNUM(ALfloat, 4, "Z")));
/* ========================================================================= */
// $ Source:GetVelocity
// < X:number=Current X velocity.
// < Y:number=Current Y velocity.
// < Z:number=Current Z velocity.
// ? Returns the current velocity of the source.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(GetVelocity)
  ALfloat fX, fY, fZ;
  LCGETPTR(1, Source)->GetVelocity(fX, fY, fZ);
  LCPUSHNUM(fX); LCPUSHNUM(fY); LCPUSHNUM(fZ);
LLFUNCENDEX(3);
/* ========================================================================= */
// $ Source:SetVelocity
// < X:number=The new X velocity.
// < Y:number=The new Y velocity.
// < Z:number=The new Z velocity.
// ? Sets the new velocity of the source.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVelocity,
  LCGETPTR(1, Source)->SetVelocity(LCGETNUM(ALfloat, 2, "X"),
                                   LCGETNUM(ALfloat, 3, "Y"),
                                   LCGETNUM(ALfloat, 4, "Z")));
/* ========================================================================= */
// $ Source:Play
// ? Plays the source.
/* ------------------------------------------------------------------------- */
LLFUNC(Play, LCGETPTR(1, Source)->Play());
/* ========================================================================= */
// $ Source:Stop
// ? Stops the source.
/* ------------------------------------------------------------------------- */
LLFUNC(Stop, LCGETPTR(1, Source)->Stop());
/* ========================================================================= */
// $ Source:GetState
// < State:integer=The current playback state.
// ? Returns if current playback state.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetState, 1, LCPUSHINT(LCGETPTR(1, Source)->GetState()));
/* ========================================================================= */
// $ Source:Destroy
// ? Stops and destroys the source object and frees all the memory associated
// ? with it. The object will no longer be useable after this call and an
// ? error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Source));
/* ========================================================================= */
/* ######################################################################### */
/* ## Source:* member functions structure                                 ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Source:* member functions begin
  LLRSFUNC(Destroy),                   // Destroy internal source object
  LLRSFUNC(GetDirection),              // Current source direction
  LLRSFUNC(GetElapsed),                // Current source elapsed time
  LLRSFUNC(GetGain),                   // Current source gain
  LLRSFUNC(GetLooping),                // Current source is looping?
  LLRSFUNC(GetMaxDist),                // Current source maximum distance
  LLRSFUNC(GetMaxGain),                // Current source maximum gain
  LLRSFUNC(GetMinGain),                // Current source minimum gain
  LLRSFUNC(GetPitch),                  // Current source pitch
  LLRSFUNC(GetPosition),               // Current source position
  LLRSFUNC(GetRefDist),                // Current source ref distance
  LLRSFUNC(GetRelative),               // Current source is relative?
  LLRSFUNC(GetRollOff),                // Current source roll off
  LLRSFUNC(GetState),                  // Current source playback state
  LLRSFUNC(GetVelocity),               // Current source velocity
  LLRSFUNC(Play),                      // Play the source
  LLRSFUNC(SetDirection),              // Set source direction
  LLRSFUNC(SetElapsed),                // Set source elapsed time
  LLRSFUNC(SetGain),                   // Set source gain
  LLRSFUNC(SetLooping),                // Set source is looping?
  LLRSFUNC(SetMaxDist),                // Set source maximum distance
  LLRSFUNC(SetMaxGain),                // Set source maximum gain
  LLRSFUNC(SetMinGain),                // Set source minimum gain
  LLRSFUNC(SetPitch),                  // Set source pitch
  LLRSFUNC(SetPosition),               // Set source position
  LLRSFUNC(SetRefDist),                // Set source ref distance
  LLRSFUNC(SetRelative),               // Set source is relative?
  LLRSFUNC(SetRollOff),                // Set source roll off
  LLRSFUNC(SetVelocity),               // Set source velocity
  LLRSFUNC(Stop),                      // Stop the source
LLRSEND                                // Source:* member functions end
/* ========================================================================= */
/* ######################################################################### */
/* ## Source.* namespace functions structure                              ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Source.* namespace functions begin
LLRSEND                                // Source.* namespace functions end
/* ========================================================================= */
LLNAMESPACEEND                         // End of Stream namespace
/* == EoF =========================================================== EoF == */
