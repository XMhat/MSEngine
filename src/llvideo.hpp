/* == LLVIDEO.HPP ========================================================== */
/* ######################################################################### */
/* ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## */
/* ######################################################################### */
/* ## Defines the 'Video' namespace and methods for the guest to use in   ## */
/* ## Lua. This file is invoked by 'lualib.hpp'.                          ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// % Video
/* ------------------------------------------------------------------------- */
// ! The video class allows loading of video streams and playback into an fbo.
/* ========================================================================= */
namespace NsVideo {                    // Video namespace
/* -- Includes ------------------------------------------------------------- */
using namespace IfVideo;               // Using video namespace
/* ========================================================================= */
/* ######################################################################### */
/* ========================================================================= */
// $ Video:OnEvent
// > Func:function=The callback function to call when an event occurds.
// ? This function is called when the video stops, starts or loops. The
// ? callback function syntax is 'function(EventType:Integer)'.
/* ------------------------------------------------------------------------- */
LLFUNC(OnEvent, LCGETPTR(1, Video)->LuaEvtInit(lS));
/* ========================================================================= */
// $ Video:Blit
// ? Blits the specified video texture frame to the currently ACTIVE fbo. This
// ? just adds to the active FBO drawing arrays and doesn't actually render
// ? until the active fbo is finished and rendered. The currently stored
// ? vertex, texcoord, colour and ortho values are used.
/* ------------------------------------------------------------------------- */
LLFUNC(Blit, LCGETPTR(1, Video)->Blit());
/* ========================================================================= */
// $ Video:BlitT
// ? TriIndex:integer=The specified triangle id.
// ? Blits the specified video texture frame to the currently ACTIVE fbo. This
// ? just adds to the active FBO drawing arrays and doesn't actually render
// ? until the active fbo is finished and rendered. The currently stored
// ? vertex, texcoord, colour and ortho values are used.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitT, LCGETPTR(1, Video)->
  BlitTri(LCGETINTLGE(size_t, 2, 0, stTrisPerQuad, "TriIndex")))
/* ========================================================================= */
// $ Video:Play
// ? Plays the specified video.
/* ------------------------------------------------------------------------- */
LLFUNC(Play, LCGETPTR(1, Video)->Play());
/* ========================================================================= */
// $ Video:Pause
// ? Pauses the specified video.
/* ------------------------------------------------------------------------- */
LLFUNC(Pause, LCGETPTR(1, Video)->Pause());
/* ========================================================================= */
// $ Video:Rewind
// ? Rewinds the specified video back to the beginning.
/* ------------------------------------------------------------------------- */
LLFUNC(Rewind, LCGETPTR(1, Video)->Rewind());
/* ========================================================================= */
// $ Video:Stop
// ? Stops and unloads the specified video, this frees resources.
/* ------------------------------------------------------------------------- */
LLFUNC(Stop, LCGETPTR(1, Video)->Stop());
/* ========================================================================= */
// $ Video:Awaken
// ? Re-initialises the video after being stopped.
/* ------------------------------------------------------------------------- */
LLFUNC(Awaken, LCGETPTR(1, Video)->Awaken());
/* ========================================================================= */
// $ Video:SetVertex
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Right:number=The right co-ordinate.
// > Bottom:number=The bottom co-ordinate.
// ? Allows you to set basic vertex co-ordinates when blitting the fbo. For
// ? a more advanced version of this function, see Fbo:SetVertexEx().
/* ------------------------------------------------------------------------- */
LLFUNC(SetVertex, LCGETPTR(1, Video)->SetFBOVertex(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom")));
/* ========================================================================= */
// $ Video:SetVertexA
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Right:number=The right co-ordinate.
// > Bottom:number=The bottom co-ordinate.
// > Angle:number=The angle of the vertex
// ? Allows you to set basic vertex co-ordinates when blitting the fbo.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVertexA, LCGETPTR(1, Video)->SetFBOVertex(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom"),
  LCGETNUM(GLfloat, 6, "Angle")));
/* ========================================================================= */
// $ Video:SetTexCoord
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Right:number=The right co-ordinate.
// > Bottom:number=The bottom co-ordinate.
// ? Allows you to set basic texture co-ordinates when blitting the video frame
// ? texture. For an advanced version of this function,
// ? see Video:SetTexCoordEx().
/* ------------------------------------------------------------------------- */
LLFUNC(SetTexCoord, LCGETPTR(1, Video)->SetFBOTexCoord(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom")));
/* ========================================================================= */
// $ Video:SetColour
// > Red:number=The entire fbo texture red colour intensity (0 to 1).
// > Green:number=The entire fbo texture green colour intensity (0 to 1).
// > Blue:number=The entire fbo texture blue colour intensity (0 to 1).
// > Alpha:number=The entire fbo texture alpha colour intensity (0 to 1).
// ? Sets the colour intensity of all the vertexes for the entire video frame
// ? texture.
/* ------------------------------------------------------------------------- */
LLFUNC(SetColour, LCGETPTR(1, Video)->SetFBOColour(
  LCGETNUM(GLfloat, 2, "Red"),  LCGETNUM(GLfloat, 3, "Green"),
  LCGETNUM(GLfloat, 4, "Blue"), LCGETNUM(GLfloat, 5, "Alpha")));
/* ========================================================================= */
// $ Video:SetKeyColour
// > Red:number=The R component to phase out
// > Green:number=The G component to phase out
// > Blue:number=The B component to phase out
// > Intensity:number=The intensity of the phasing out
// ? Allows a certain colour to be phased out on the video. You must enable
// ? keying colour with the SetKeyed() function before using this
/* ------------------------------------------------------------------------- */
LLFUNC(SetKeyColour, LCGETPTR(1, Video)->fiYUV.SetQuadRGBA(
  LCGETNUM(GLfloat, 2, "Red"),  LCGETNUM(GLfloat, 3, "Green"),
  LCGETNUM(GLfloat, 4, "Blue"), LCGETNUM(GLfloat, 5, "Intensity")));
/* ========================================================================= */
// $ Video:GetFPS
// < FPS:Number=The FPS rate of the video
// ? Returns the FPS rate of the video
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetFPS, 1, LCPUSHNUM(LCGETPTR(1, Video)->GetFPS()));
/* ========================================================================= */
// $ Video:GetTime
// < Seconds:Number=Returns the elapsed video time in seconds
// ? Returns the elapsed audio time in seconds
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetTime, 1, LCPUSHNUM(LCGETPTR(1, Video)->GetVideoTime()));
/* ========================================================================= */
// $ Video:GetATime
// < Seconds:Number=Returns the elapsed audio time in seconds
// ? Returns the elapsed video time in seconds
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetATime, 1, LCPUSHNUM(LCGETPTR(1, Video)->GetAudioTime()));
/* ========================================================================= */
// $ Video:GetPlaying
// < Status:boolean=Playback status
// ? Returns the playback status
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetPlaying, 1, LCPUSHBOOL(LCGETPTR(1, Video)->IsPlaying()));
/* ========================================================================= */
// $ Video:GetDrift
// < Seconds:Number=Drift time in seconds.
// ? Returns the drift between audio and video sync.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetDrift, 1, LCPUSHNUM(LCGETPTR(1, Video)->GetDrift()));
/* ========================================================================= */
// $ Video:GetFrame
// < Frame:Integer=The current frame
// ? Returns the current frame number
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetFrame, 1, LCPUSHINT(LCGETPTR(1, Video)->GetFrame()));
/* ========================================================================= */
// $ Video:GetFrames
// < Frames:Integer=Returns the frames decoded
// ? Returns the number of frames decoded
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetFrames, 1, LCPUSHINT(LCGETPTR(1, Video)->GetFrames()));
/* ========================================================================= */
// $ Video:GetFramesLost
// < Frames:Integer=Returns the frames that were skipped
// ? Returns the number of skipped frames decoded
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetFramesLost, 1,
  LCPUSHINT(LCGETPTR(1, Video)->GetFramesSkipped()));
/* ========================================================================= */
// $ Video:SetFilter
// < Filter:boolean=Video should be filtered by OpenGL
// ? Sets GL_LINEAR filtering on the video if true, GL_NEAREST if not.
/* ------------------------------------------------------------------------- */
LLFUNC(SetFilter, LCGETPTR(1, Video)->SetFilter(
  LCGETBOOL(2, "Filter")));
/* ========================================================================= */
// $ Video:SetKeyed
// < Keyed:boolean=Video should be keyed with the set colour
// ? Enabling this allowed the video to be chroma keyed. This also disables
// ? changing the colour intensity because the specified colour using SetColour
// ? will set the colour to be keyed. If the pixel is greater or equal then the
// ? specified colour then the pixel is set to the specified alpha.
/* ------------------------------------------------------------------------- */
LLFUNC(SetKeyed, LCGETPTR(1, Video)->SetKeyed(LCGETBOOL(2, "Keyed")));
/* ========================================================================= */
// $ Video:GetName
// < Id:string=The video identifier
// ? Returns the identifier of the video, usually the filename.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetName, 1, LCPUSHXSTR(LCGETPTR(1, Video)->IdentGet()));
/* ========================================================================= */
// $ Video:GetWidth
// < Width:integer=The width of the video
// ? Returns the width of the video
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetWidth, 1, LCPUSHINT(LCGETPTR(1, Video)->GetWidth()));
/* ========================================================================= */
// $ Video:GetHeight
// < Width:integer=The height of the video
// ? Returns the height of the video
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetHeight, 1, LCPUSHINT(LCGETPTR(1, Video)->GetHeight()));
/* ========================================================================= */
// $ Video:GetLoop
// < Count:integer=Number of loops left.
// ? Returns the number of loops remaining of playback.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetLoop, 1,
  LCPUSHINT(static_cast<lua_Integer>(LCGETPTR(1, Video)->GetLoop())));
/* ========================================================================= */
// $ Video:SetLoop
// > Count:integer=New playback loop count (-1 = infinity).
// ? Sets the new playback loop count.
/* ------------------------------------------------------------------------- */
LLFUNC(SetLoop, LCGETPTR(1, Video)->SetLoop(LCGETINT(size_t, 2, "Count")));
/* ========================================================================= */
// $ Video:SetVolume
// > Volume:number=New video volume (0 to 1).
// ? Sets the volume of the video class even if it is playing.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVolume,
  LCGETPTR(1, Video)->SetVolume(LCGETNUMLG(ALfloat, 2, 0, 1, "Volume")));
/* ========================================================================= */
// $ Video:Destroy
// ? Stops and destroys the stream object and frees all the memory associated
// ? with it. The OpenGL handles and VRAM associated with the object will be
// ? freed after the main FBO has been rendered. The object will no longer be
// ? useable after this call and an error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Video));
/* ========================================================================= */
/* ######################################################################### */
/* ## Video:* member functions structure                                  ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Video:* member functions begin
  LLRSFUNC(Awaken),        LLRSFUNC(Blit),         LLRSFUNC(BlitT),
  LLRSFUNC(Destroy),       LLRSFUNC(GetATime),     LLRSFUNC(GetDrift),
  LLRSFUNC(GetFPS),        LLRSFUNC(GetFrame),     LLRSFUNC(GetFrames),
  LLRSFUNC(GetFramesLost), LLRSFUNC(GetHeight),    LLRSFUNC(GetLoop),
  LLRSFUNC(GetName),       LLRSFUNC(GetPlaying),   LLRSFUNC(GetTime),
  LLRSFUNC(GetWidth),      LLRSFUNC(OnEvent),      LLRSFUNC(Pause),
  LLRSFUNC(Play),          LLRSFUNC(Rewind),       LLRSFUNC(SetColour),
  LLRSFUNC(SetFilter),     LLRSFUNC(SetKeyColour), LLRSFUNC(SetKeyed),
  LLRSFUNC(SetLoop),       LLRSFUNC(SetTexCoord),  LLRSFUNC(SetVertex),
  LLRSFUNC(SetVertexA),    LLRSFUNC(SetVolume),    LLRSFUNC(Stop),
LLRSEND                                // Video:* member functions end
/* ========================================================================= */
// $ Video.FileAsync
// > Filename:string=The filename of the ogv to load
// > ErrorFunc:function=The function to call when there is an error
// > SuccessFunc:function=The function to call when the file is laoded
// ? Loads a streamable ogv file off the main thread. The callback functions
// ? send an argument to the video stream object that was created.
/* ------------------------------------------------------------------------- */
LLFUNC(FileAsync, LCCLASSCREATE(Video)->InitAsyncFile(lS));
/* ======================================================================= */
// $ Video.File
// > Filename:string=The filename of the ogg file to load
// < Handle:Video=The video stream object
// ? Loads a video stream sample on the main thread from the specified file
// ? on disk. Returns the video stream object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(File, 1,
  LCCLASSCREATE(Video)->SyncInitFileSafe(LCGETCPPFILE(1, "File")));
/* ========================================================================= */
// $ Video.ArrayAsync
// > Id:String=The identifier of the string
// > Data:array=The data of the ogg file to load
// > ErrorFunc:function=The function to call when there is an error
// > SuccessFunc:function=The function to call when the audio file is laoded
// ? Loads an ogv file off the main thread from the specified array object.
// ? The callback functions send an argument to the video stream object that
// ? was created.
/* ------------------------------------------------------------------------- */
LLFUNC(ArrayAsync, LCCLASSCREATE(Video)->InitAsyncArray(lS));
/* ======================================================================= */
// $ Video.Asset
// > Id:String=The identifier of the string
// > Data:Asset=The file data of the ogg file to load
// < Handle:Ogg=The ogg object
// ? Loads an ogv video file to stream on the main thread from the specified
// ? array object.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Asset, 1, LCCLASSCREATE(Video)->SyncInitArray(
  LCGETCPPSTRING(1, "Identifier"), StdMove(*LCGETPTR(2, Asset))));
/* ======================================================================= */
// $ Video.WaitAsync
// ? Halts main-thread execution until all async video stream events have
// ? completed.
/* ------------------------------------------------------------------------- */
LLFUNC(WaitAsync, cVideos->WaitAsync());
/* ========================================================================= */
// $ Video.ClearEvents
// ? Clear the OnEvent callback on all Video objects. This might be useful for
// ? when you use the OnEvent() function and the Video object is still
// ? referenced and no longer accessable by your code in which the result is
// ? that the Video object does not get garbage collected.
/* ------------------------------------------------------------------------- */
LLFUNC(ClearEvents, VideoClearEvents());
/* ========================================================================= */
/* ######################################################################### */
/* ## Video.* namespace functions structure                               ## */
/* ######################################################################### */
/* ------------------------------------------------------------------------- */
LLRSBEGIN                              // Video.* namespace functions begin
  LLRSFUNC(Asset), LLRSFUNC(ArrayAsync), LLRSFUNC(ClearEvents),
  LLRSFUNC(File),  LLRSFUNC(FileAsync),  LLRSFUNC(WaitAsync),
LLRSEND                                // Video.* namespace functions end
/* ========================================================================= */
/* ######################################################################### */
/* ## Video.* namespace constants structure                               ## */
/* ######################################################################### */
/* ========================================================================= */
// @ Video.Flags
// < Codes:table=The table of key/value pairs of available flags
// ? Returns possible values for Video:OnEvent() event command.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Events)                    // Beginning of Stream event flags
  LLRSKTITEM(Video::VE_,FINISH),       LLRSKTITEM(Video::VE_,LOOP),
  LLRSKTITEM(Video::VE_,PAUSE),        LLRSKTITEM(Video::VE_,PLAY),
  LLRSKTITEM(Video::VE_,STOP),
LLRSKTEND                              // End of Stream event flags
/* ========================================================================= */
LLRSCONSTBEGIN                         // Video.* namespace consts begin
  LLRSCONST(Events),
LLRSCONSTEND                           // Video.* namespace consts end
/* ========================================================================= */
}                                      // End of Video namespace
/* == EoF =========================================================== EoF == */
