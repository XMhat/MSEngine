/* == LLSTREAM.HPP ========================================================= */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Stream' namespace and methods for the guest to use in  ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Stream
/* ------------------------------------------------------------------------- */
// ! The stream class allows loading of large audio files such as ambience
// ! or music fuiles which are streamed into and played back from multiple
// ! buffers.
// !
// ! Only .OGG file format is supported at the moment. The entire file
// ! will be loaded into memory in it's compressed form. It will be
// ! decompressed into multiple buffers during playback.
// !
// ! The 'aud_strbufcount' cvar controls how many buffers are used to stream.
// ! The 'aud_strbufsiz' cvar controls the size of each buffers.
// !
// ! Since playback is done in separate threads, there is not much need to
// ! modify any of the above settings.
/* ========================================================================= */
namespace NsStream {                   // Stream namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfStream;              // Using stream namespace
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Stream:OnEvent
// > Func:function=The callback function to call when an event occurs.
// ? This function is called when the stream stops, starts or loops. The
// ? callback function syntax is 'function(EventType:Integer)'. Please be
// ? aware that that reference counter on the supplied function is increased
// ? therefore the reference will linger until a new function is set or the
// ? Stream class is destroyed. This also means references to variables such as
// ? this class inside the callback will make the variable invulnerable to the
// ? garbage collector if the reference on the function is not properly
// ? unreferenced so be careful how you use this!
/* ------------------------------------------------------------------------- */
LLFUNC(OnEvent, LCGETPTR(1, Stream)->LuaEvtInit(lS));
/* ========================================================================= */
// $ Stream:Play
// > Loops:integer=Number of loops.
// > Volume:number=Stream volume (0 to 1).
// > Position:integer=Sample position.
// ? Plays the specified stream at the specified sample position.
/* ------------------------------------------------------------------------- */
LLFUNC(Play, LCGETPTR(1, Stream)->PlaySafe());
/* ========================================================================= */
// $ Stream:GetDuration
// < Duration:number=Duration in seconds
// ? Returns the duration of the specified stream in seconds.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetDuration, 1, LCPUSHNUM(LCGETPTR(1, Stream)->GetDurationSafe()));
/* ========================================================================= */
// $ Stream:GetElapsed
// < Duration:number=Elapsed time in seconds
// ? Returns the time elapsed of the stream. This is the position of the codec
// ? and not the actual playback position.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetElapsed, 1, LCPUSHNUM(LCGETPTR(1, Stream)->GetElapsedSafe()));
/* ========================================================================= */
// $ Stream:SetElapsed
// > Duration:number=Duration in seconds
// ? Sets the specified time index of the stream. This is not a precise time
// ? index and is optimised for speed. Use Stream.SetElapsedP() for precision.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(SetElapsed)
  Stream &sCref = *LCGETPTR(1, Stream);
  sCref.SetElapsedSafe(LCGETNUMLG(ALdouble,
    2, 0, sCref.GetDurationSafe(), "Time"));
LLFUNCEND
/* ========================================================================= */
// $ Stream:SetElapsedPage
// > Duration:number=Duration in seconds
// ? Sets the precise specified time index of the stream.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(SetElapsedPage)
  Stream &sCref = *LCGETPTR(1, Stream);
  sCref.SetElapsedFastSafe(
    LCGETNUMLG(ALdouble, 2, 0, sCref.GetDurationSafe(), "Time"));
LLFUNCEND
/* ========================================================================= */
// $ Stream:GetPosition
// < Position:integer=Sample position
// ? Returns the sample position elapsed of the stream.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetPosition, 1,
  LCPUSHINT(static_cast<lua_Integer>(LCGETPTR(1, Stream)->GetPositionSafe())));
/* ========================================================================= */
// $ Stream:SetPosition
// > Position:integer=Sample position.
// ? Sets the precise specified sample position index of the stream.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(SetPosition)
  Stream &sCref = *LCGETPTR(1, Stream);
  sCref.SetPositionSafe(
    LCGETINTLG(ogg_int64_t, 2, 0, sCref.GetSamplesSafe(), "Index"));
LLFUNCEND
/* ========================================================================= */
// $ Stream:SetPositionPage
// > Position:integer=Sample position
// ? Sets the current sample position of playback. This is not a precise
// ? position is optimised for speed. Use Stream.SetPosition() for precision.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(SetPositionPage)
  Stream &sCref = *LCGETPTR(1, Stream);
  sCref.SetPositionFastSafe(
    LCGETINTLG(ogg_int64_t, 2, 0, sCref.GetSamplesSafe(), "Index"));
LLFUNCEND
/* ========================================================================= */
// $ Stream:Stop
// ? Stops playback of the specified stream. This will not modify the position
// ? or any other stream property.
/* ------------------------------------------------------------------------- */
LLFUNC(Stop, LCGETPTR(1, Stream)->StopSafe(SR_LUA));
/* ========================================================================= */
// $ Stream:IsPlaying
// < State:boolean=Stream is playing?
// ? Returns true if the stream is playing or false if the stream is stopped.
/* ------------------------------------------------------------------------- */
LLFUNCEX(IsPlaying, 1, LCPUSHBOOL(LCGETPTR(1, Stream)->IsPlaying()));
/* ========================================================================= */
// $ Stream:GetLoop
// < Count:integer=Number of loops left.
// ? Returns the number of loops remaining of playback.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetLoop, 1,
  LCPUSHINT(static_cast<lua_Integer>(LCGETPTR(1, Stream)->GetLoopSafe())));
/* ========================================================================= */
// $ Stream:GetName
// < Name:string=Filename of stream
// ? Returns the stream indentifier or filename
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetName, 1, LCPUSHXSTR(LCGETPTR(1, Stream)->IdentGet()));
/* ========================================================================= */
// $ Stream:GetLoopBegin
// < Count:integer=Smaple position begin point
// ? Gets the current loop begin sample.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetLoopBegin, 1, LCPUSHINT(static_cast<lua_Integer>
  (LCGETPTR(1, Stream)->GetLoopBeginSafe())));
/* ========================================================================= */
// $ Stream:GetLoopEnd
// < Count:integer=Sample position end point
// ? Gets the current loop end sample.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetLoopEnd, 1, LCPUSHINT(static_cast<lua_Integer>
  (LCGETPTR(1, Stream)->GetLoopEndSafe())));
/* ========================================================================= */
// $ Stream:SetLoop
// > Count:integer=New playback loop count.
// ? Sets the new playback loop count.
/* ------------------------------------------------------------------------- */
LLFUNC(SetLoop, LCGETPTR(1, Stream)->
  SetLoopSafe(LCGETINTLGE(ogg_int64_t, 2, -1,
    numeric_limits<ogg_int64_t>::max(), "Count")));
/* ========================================================================= */
// $ Stream:SetLoopRange
// > Begin:integer=New beginning sample playback position of loop.
// > End:integer=New ending sample playback position of loop.
// ? Sets the new playback beginning sample position after stream loops and
// ? the ending sample position.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(SetLoopRange)
  Stream &sCref = *LCGETPTR(1, Stream);
  sCref.SetLoopRangeSafe(
    LCGETINTLGE(ogg_int64_t, 2, 0, sCref.GetLoopEndSafe(), "BeginPos"),
    LCGETINTLGE(ogg_int64_t, 3, sCref.GetLoopBeginSafe(),
      sCref.GetSamplesSafe(), "EndPos"));
LLFUNCEND
/* ========================================================================= */
// $ Stream:SetLoopBegin
// > Begin:integer=New beginning sample playback position of loop.
// ? Sets the new playback beginning sample position after stream loops.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(SetLoopBegin)
  Stream &sCref = *LCGETPTR(1, Stream);
  sCref.SetLoopBeginSafe(LCGETINTLGE(ogg_int64_t,
    2, 0, sCref.GetLoopEndSafe(), "pcmpos"));
LLFUNCEND
/* ========================================================================= */
// $ Stream:SetLoopEnd
// > End:integer=New ending sample playback position of loop.
// ? Sets the new playback ending sample position before stream loops.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(SetLoopEnd)
  Stream &sCref = *LCGETPTR(1, Stream);
  sCref.SetLoopEndSafe(LCGETINTLGE(ogg_int64_t, 2, sCref.GetLoopBeginSafe(),
    sCref.GetSamplesSafe(), "pcmpos"));
LLFUNCEND
/* ========================================================================= */
// $ Stream:SetVolume
// > Volume:number=New stream volume (0 to 1).
// ? Sets the volume of the stream class even if it is playing.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVolume,
  LCGETPTR(1, Stream)->SetVolume(LCGETNUMLG(ALfloat, 2, 0, 1, "Volume")));
/* ========================================================================= */
// $ Stream:GetVolume
// < Volume:number=Current stream volume (0 to 1).
// ? Returns the volume of the stream class.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetVolume, 1, LCPUSHNUM(LCGETPTR(1, Stream)->GetVolume()));
/* ========================================================================= */
// $ Stream:GetBytes
// < Total:integer=Current stream total bytes.
// ? Returns the size of the file that is streaming.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetBytes, 1, LCPUSHINT(LCGETPTR(1, Stream)->GetOggBytes()));
/* ========================================================================= */
// $ Stream:GetSamples
// < Total:integer=Current stream total samples.
// ? Returns the duration of the stream in samples.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetSamples, 1, LCPUSHINT(LCGETPTR(1, Stream)->GetSamplesSafe()));
/* ========================================================================= */
// $ Stream:GetMetaData
// < Data:table=Stream metadata key/value table.
// ? Returns all the metadata that was stored inside the audio file.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetMetaData, 1, LCTOTABLE(LCGETPTR(1, Stream)->GetMetaData()));
/* ========================================================================= */
// $ Stream:GetRate
// < Rate:integer=The sample rate
// ? Returns the sample rate per second in samples.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetRate, 1, LCPUSHINT(LCGETPTR(1, Stream)->GetRate()));
/* ========================================================================= */
// $ Stream:GetBitRate
// < Upper:integer=The upper bitrate
// < Nominal:integer=The nominal bitrate
// < Lower:integer=The lower bitrate
// < Window:integer=The window bitrate
// ? Returns bitrate information for the audio file.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(GetBitRate)
  Stream &sCref = *LCGETPTR(1, Stream);
  LCPUSHINT(sCref.GetBitRateUpper());
  LCPUSHINT(sCref.GetBitRateNominal());
  LCPUSHINT(sCref.GetBitRateLower());
  LCPUSHINT(sCref.GetBitRateWindow());
LLFUNCENDEX(4)
/* ========================================================================= */
// $ Stream:GetVersion
// < Version:integer=The ogg version
// ? Returns the version of the ogg file
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetVersion, 1, LCPUSHINT(LCGETPTR(1, Stream)->GetVersion()));
/* ========================================================================= */
// $ Stream:GetChannels
// < Rate:integer=The number of channels
// ? Returns the number of channels in the audio file.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetChannels, 1, LCPUSHINT(LCGETPTR(1, Stream)->GetChannels()));
/* ========================================================================= */
// $ Stream:Destroy
// ? Stops and destroys the stream object and frees all the memory associated
// ? with it. The object will no longer be useable after this call and an
// ? error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Stream));
/* ========================================================================= */
/* ######################################################################### */
/* ## Stream:* member functions structure                                 ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Stream:* member functions begin
  LLRSFUNC(OnEvent),      LLRSFUNC(GetBitRate),      LLRSFUNC(GetBytes),
  LLRSFUNC(GetChannels),  LLRSFUNC(GetDuration),     LLRSFUNC(GetElapsed),
  LLRSFUNC(GetName),      LLRSFUNC(GetPosition),     LLRSFUNC(GetLoop),
  LLRSFUNC(GetLoopBegin), LLRSFUNC(GetLoopEnd),      LLRSFUNC(GetRate),
  LLRSFUNC(GetSamples),   LLRSFUNC(GetVersion),      LLRSFUNC(GetVolume),
  LLRSFUNC(GetMetaData),  LLRSFUNC(IsPlaying),       LLRSFUNC(OnEvent),
  LLRSFUNC(Play),         LLRSFUNC(SetElapsed),      LLRSFUNC(SetElapsedPage),
  LLRSFUNC(SetPosition),  LLRSFUNC(SetPositionPage), LLRSFUNC(SetLoop),
  LLRSFUNC(SetLoopBegin), LLRSFUNC(SetLoopEnd),      LLRSFUNC(SetLoopRange),
  LLRSFUNC(SetVolume),    LLRSFUNC(Stop),
LLRSEND                                // Stream:* member functions end
/* ========================================================================= */
// $ Stream.FileAsync
// > Filename:string=The filename of the ogg to load
// > ErrorFunc:function=The function to call when there is an error
// > SuccessFunc:function=The function to call when the file is laoded
// ? Loads a streamable ogg file off the main thread. The callback functions
// ? send an argument to the stream object that was created.
/* ------------------------------------------------------------------------- */
LLFUNC(FileAsync, LCCLASSCREATE(Stream)->InitAsyncFile(lS));
/* ======================================================================= */
// $ Stream.File
// > Filename:string=The filename of the ogg file to load
// < Handle:Stream=The stream object
// ? Loads a stream sample on the main thread from the specified file on disk.
// ? Returns the stream object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(File, 1,
  LCCLASSCREATE(Stream)->SyncInitFileSafe(LCGETCPPFILE(1, "File")));
/* ========================================================================= */
// $ Stream.ArrayAsync
// > Id:String=The identifier of the string
// > Data:array=The data of the ogg file to load
// > ErrorFunc:function=The function to call when there is an error
// > SuccessFunc:function=The function to call when the audio file is laoded
// ? Loads an ogg file off the main thread from the specified array object.
// ? The callback functions send an argument to the Stream object that was
// ? created.
/* ------------------------------------------------------------------------- */
LLFUNC(ArrayAsync, LCCLASSCREATE(Stream)->InitAsyncArray(lS));
/* ========================================================================= */
// $ Stream.Asset
// > Id:String=The identifier of the string
// > Data:Asset=The file data of the ogg file to load
// < Handle:Ogg=The ogg object
// ? Loads an ogg file to stream on the main thread from the specified array
// ? object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Asset, 1, LCCLASSCREATE(Stream)->SyncInitArray(
  LCGETCPPSTRINGNE(1, "Identifier"), StdMove(*LCGETPTR(2, Asset))));
/* ======================================================================= */
// $ Stream.WaitAsync
// ? Halts main-thread execution until all async stream mevents have completed
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, cStreams->WaitAsync());
/* ========================================================================= */
// $ Stream.ClearEvents
// ? Clear the OnEvent callback on all Stream objects. This might be useful for
// ? when you use the OnEvent() function and the Stream object is still
// ? referenced and no longer accessable by your code in which the result is
// ? that the Stream object does not get garbage collected.
/* ------------------------------------------------------------------------- */
LLFUNC(ClearEvents, StreamClearEvents());
/* ========================================================================= */
/* ######################################################################### */
/* ## Stream.* namespace functions structure                              ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Stream.* namespace functions begin
  LLRSFUNC(Asset), LLRSFUNC(ArrayAsync), LLRSFUNC(ClearEvents),
  LLRSFUNC(File),  LLRSFUNC(FileAsync),  LLRSFUNC(WaitAsync),
LLRSEND                                // Stream.* namespace functions end
/* ========================================================================= */
/* ######################################################################### */
/* ## Stream.* namespace constants structure                              ## */
/* ######################################################################### */
/* ========================================================================= */
// @ Stream.Flags
// < Codes:table=The table of key/value pairs of available flags
// ? Returns possible values for Stream:OnEvent() event command.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Events)                    // Beginning of Video event flags
LLRSKTITEM(SE_,PLAY),                  LLRSKTITEM(SE_,STOP),
LLRSKTEND                              // End of Video event flags
/* ========================================================================= */
// @ Stream.States
// < Codes:table=The table of key/value pairs of available flags
// ? Returns possible values for Stream:OnEvent() last state command.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(States)                    // Beginning of Stream last state flags
LLRSKTITEM(PS_,STANDBY),               LLRSKTITEM(PS_,PLAYING),
LLRSKTITEM(PS_,PLAYING),               LLRSKTITEM(PS_,FINISHING),
LLRSKTITEM(PS_,WASPLAYING),            LLRSKTITEM(PS_,MAX),
LLRSKTEND                              // End of Stream last state flags
/* ========================================================================= */
// @ Stream.Reasons
// < Codes:table=The table of key/value pairs of available flags
// ? Returns possible values for Stream:OnEvent() stop reason command.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Reasons)                   // Beginning of Stream stop reasons
LLRSKTITEM(SR_,STOPNOUNQ),             LLRSKTITEM(SR_,STOPUNQ),
LLRSKTITEM(SR_,REBUFFAIL),             LLRSKTITEM(SR_,RWREBUFFAIL),
LLRSKTITEM(SR_,GENBUFFAIL),            LLRSKTITEM(SR_,STOPALL),
LLRSKTITEM(SR_,LUA),                   LLRSKTITEM(SR_,MAX),
LLRSKTEND                              // End of Stream stop reasons
/* ========================================================================= */
LLRSCONSTBEGIN                         // Stream.* namespace consts begin
LLRSCONST(Events), LLRSCONST(States), LLRSCONST(Reasons),
LLRSCONSTEND                           // Stream.* namespace consts end
/* ========================================================================= */
}                                      // End of Stream namespace
/* == EoF =========================================================== EoF == */
