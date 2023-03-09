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
LLNAMESPACEBEGIN(Stream)               // Stream namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfStream;              // Using stream interface
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
// > Position:integer=PCM position.
// ? Plays the specified stream at the specified PCM position.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(Play)
  Stream &sCref = *LCGETPTR(1, Stream);
  sCref.PlaySafe(
    LCGETINT   (size_t,      2,                           "Loops"),
    LCGETNUMLG (ALfloat,     3,  0,                    1, "Volume"),
    LCGETINTLGE(ogg_int64_t, 4, -1, sCref.GetTotalSafe(), "Position"));
LLFUNCEND
/* ========================================================================= */
// $ Stream:GetDuration
// < Duration:number=Duration in seconds
// ? Returns the duration of the specified stream in seconds.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetDuration, 1,
  LCPUSHNUM(LCGETPTR(1, Stream)->GetDurationSafe()));
/* ========================================================================= */
// $ Stream:GetElapsed
// < Duration:number=Elapsed time in seconds
// ? Returns the time elapsed of the stream. This is the position of the codec
// ? and not the actual playback position.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetElapsed, 1,
  LCPUSHNUM(LCGETPTR(1, Stream)->GetElapsedSafe()));
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
// < Position:integer=PCM Position
// ? Returns the PCM index position elapsed of the stream. This is the position
// ? of the codec and not the actual playback position.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetPosition, 1,
  LCPUSHINT(static_cast<lua_Integer>(LCGETPTR(1, Stream)->GetPositionSafe())));
/* ========================================================================= */
// $ Stream:SetPosition
// > Position:integer=PCM position.
// ? Sets the current PCM position of playback. This is not a precise PCM
// ? index and is optimised for speed. Use Stream.SetPositionP() for precision.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(SetPosition)
  Stream &sCref = *LCGETPTR(1, Stream);
  sCref.SetPositionSafe(
    LCGETINTLG(ogg_int64_t, 2, 0, sCref.GetTotalSafe(), "Index"));
LLFUNCEND
/* ========================================================================= */
// $ Stream:SetPositionPage
// > Position:integer=PCM position
// ? Sets the precise specified PCM position index of the stream.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(SetPositionPage)
  Stream &sCref = *LCGETPTR(1, Stream);
  sCref.SetPositionFastSafe(
    LCGETINTLG(ogg_int64_t, 2, 0, sCref.GetTotalSafe(), "Index"));
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
  LCPUSHINT(static_cast<lua_Integer>(LCGETPTR(1, Stream)->GetLoop())));
/* ========================================================================= */
// $ Stream:GetName
// < Name:string=Filename of stream
// ? Returns the stream indentifier or filename
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetName, 1, LCPUSHXSTR(LCGETPTR(1, Stream)->IdentGet()));
/* ========================================================================= */
// $ Stream:GetLoopBegin
// < Count:integer=The PCM position where loops begin
// ? Gets the current loop begin PCM point
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetLoopBegin, 1,
  LCPUSHINT(static_cast<lua_Integer>(LCGETPTR(1, Stream)->GetLoopBegin())));
/* ========================================================================= */
// $ Stream:GetLoopEnd
// < Count:integer=The PCM position of where loops end and back to begin point
// ? Gets the current loop end PCM point
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetLoopEnd, 1,
  LCPUSHINT(static_cast<lua_Integer>(LCGETPTR(1, Stream)->GetLoopEnd())));
/* ========================================================================= */
// $ Stream:SetLoop
// > Count:integer=New playback loop count.
// ? Sets the new playback loop count.
/* ------------------------------------------------------------------------- */
LLFUNC(SetLoop,
  LCGETPTR(1, Stream)->SetLoop(LCGETINT(size_t, 2, "Count")));
/* ========================================================================= */
// $ Stream:SetLoopRange
// > Begin:integer=New beginning PCM playback position of loop.
// > End:integer=New ending PCM playback position of loop.
// ? Sets the new playback beginning PCM position after stream loops and
// ? the ending PCM position.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(SetLoopRange)
  Stream &sCref = *LCGETPTR(1, Stream);
  sCref.SetLoopBegin(LCGETINTLGE(ogg_int64_t,
    2, 0, sCref.GetLoopEnd(), "pcmpos"));
  sCref.SetLoopEnd(LCGETINTLGE(ogg_int64_t,
    3, sCref.GetLoopBegin(), sCref.GetTotalSafe(), "pcmpos"));
LLFUNCEND
/* ========================================================================= */
// $ Stream:SetLoopBegin
// > Begin:integer=New beginning PCM playback position of loop.
// ? Sets the new playback beginning PCM position after stream loops.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(SetLoopBegin)
  Stream &sCref = *LCGETPTR(1, Stream);
  sCref.SetLoopBegin(LCGETINTLGE(ogg_int64_t,
    2, 0, sCref.GetLoopEnd(), "pcmpos"));
LLFUNCEND
/* ========================================================================= */
// $ Stream:SetLoopEnd
// > End:integer=New ending PCM playback position of loop.
// ? Sets the new playback ending PCM position before stream loops.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(SetLoopEnd)
  Stream &sCref = *LCGETPTR(1, Stream);
  sCref.SetLoopEnd(LCGETINTLGE(ogg_int64_t, 2, sCref.GetLoopBegin(),
    sCref.GetTotalSafe(), "pcmpos"));
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
// $ Stream:GetMetaData
// < Data:table=Stream metadata key/value table.
// ? Returns all the metadata that was stored inside the audio file.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetMetaData, 1,
  LCTOTABLE(LCGETPTR(1, Stream)->GetMetaData()));
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
  LLRSFUNC(OnEvent),                   // Set event handler
  LLRSFUNC(GetDuration),               // Get codec position
  LLRSFUNC(GetElapsed),                // Get elapsed time
  LLRSFUNC(GetPosition),               // Get PCM position
  LLRSFUNC(GetName),                   // Get stream filename
  LLRSFUNC(GetLoop),                   // Get loop count left
  LLRSFUNC(GetLoopBegin),              // Get loop begin point
  LLRSFUNC(GetLoopEnd),                // Get loop end point
  LLRSFUNC(GetVolume),                 // Get volume
  LLRSFUNC(GetMetaData),               // Get stream metadata
  LLRSFUNC(IsPlaying),                 // Stream is playing?
  LLRSFUNC(OnEvent),                   // Set event handler
  LLRSFUNC(Play),                      // Play Stream
  LLRSFUNC(SetElapsed),                // Seek by time
  LLRSFUNC(SetElapsedPage),            // Seek by time (page)
  LLRSFUNC(SetPosition),               // Seek PCM position
  LLRSFUNC(SetPositionPage),           // Seek PCM position (page)
  LLRSFUNC(SetLoop),                   // Set loop count
  LLRSFUNC(SetLoopBegin),              // Set loop begin point
  LLRSFUNC(SetLoopEnd),                // Set loop end point
  LLRSFUNC(SetLoopRange),              // Set loop begin & end
  LLRSFUNC(SetVolume),                 // Set volume
  LLRSFUNC(Stop),                      // Stop Stream
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
  LCGETCPPSTRINGNE(1, "Identifier"), move(*LCGETPTR(2, Asset))));
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
  LLRSFUNC(Asset),                     // Load from array asyncronously
  LLRSFUNC(ArrayAsync),                //   "  asyncronously
  LLRSFUNC(ClearEvents),               // Clear all lua reference events
  LLRSFUNC(File),                      // Load from specified file
  LLRSFUNC(FileAsync),                 //   "  asyncronously
  LLRSFUNC(WaitAsync),                 // Wait for async events to complete
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
LLRSKTBEGIN(Events)                    // Beginning of Stream event flags
LLRSKTITEM(SE_,PLAY),                  LLRSKTITEM(SE_,STOP),
LLRSKTEND                              // End of Stream event flags
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
LLRSCONST(Events),                     // Stream event command
LLRSCONST(States),                     // Stream state command
LLRSCONST(Reasons),                    // Stream stop command
LLRSCONSTEND                           // Stream.* namespace consts end
/* ========================================================================= */
LLNAMESPACEEND                         // End of Stream namespace
/* == EoF =========================================================== EoF == */
