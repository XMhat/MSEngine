/* == LLFBO.HPP ============================================================ **
** ######################################################################### **
** ## MS-ENGINE              Copyright (c) MS-Design, All Rights Reserved ## **
** ######################################################################### **
** ## Defines the 'Fbo' namespace and methods for the guest to use in     ## **
** ## Lua. This file is invoked by 'lualib.hpp'.                          ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
#pragma once                           // Only one incursion allowed
/* ========================================================================= **
** ######################################################################### **
** ========================================================================= */
// % Fbo
/* ------------------------------------------------------------------------- */
// ! The fbo class allows the programmer to create an OpenGL frame-buffer
// ! object which they can use to create seperate drawing canvases which they
// ! can apply special effects to without modifying other canvases.
/* ========================================================================= */
namespace LLFbo {                      // Fbo namespace
/* -- Dependencies --------------------------------------------------------- */
using namespace IConsole::P;           using namespace IFboDef::P;
using namespace IFbo::P;               using namespace IFboCore::P;
using namespace ILua::P;               using namespace IOgl::P;
using namespace ISShot::P;             using namespace IString::P;
using namespace Lib::OS::GlFW;
/* ========================================================================= **
** ######################################################################### **
** ## Fbo:* member functions                                              ## **
** ######################################################################### **
** ========================================================================= */
// $ Fbo.SetBlend
// > srcRGB:integer=How the source RGB blending factors are computed.
// > dstRGB:integer=How the dest RGB blending factors are computed.
// > srcAlpha:integer=How the alpha source blending factor is computed.
// > dstAlpha:integer=How the alpha destination blending factor is computed.
// ? Sets the alpha blending function formula of the back buffer using
// ? glBlendFuncSeparate. The parameters are a zero index of the formula you
// ? want to use. These values are available in Fbo.Blends().
/* ------------------------------------------------------------------------- */
LLFUNC(SetBlend, LCGETPTR(1, Fbo)->FboSetBlend(
  LCGETINTLGE(OglBlendEnum, 2, OB_Z, OB_MAX, "srcRGB"),
  LCGETINTLGE(OglBlendEnum, 3, OB_Z, OB_MAX, "dstRGB"),
  LCGETINTLGE(OglBlendEnum, 4, OB_Z, OB_MAX, "srcAlpha"),
  LCGETINTLGE(OglBlendEnum, 5, OB_Z, OB_MAX, "dstAlpha")));
/* ========================================================================= */
// $ Fbo:SetVX
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > V1Left:number=Coord #1 of vertex #1 of the specified triangle.
// > V1Top:number=Coord #2 of vertex #2 of the specified triangle.
// > V2Left:number=Coord #1 of vertex #3 of the specified triangle.
// > V2Top:number=Coord #2 of vertex #1 of the specified triangle.
// > V3Left:number=Coord #1 of vertex #2 of the specified triangle.
// > V3Top:number=Coord #2 of vertex #3 of the specified triangle.
// ? Stores the specified vertex co-ordinates when using the 'Fbo:Blit'
// ? function.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVX, LCGETULPTR(1, Fbo)->
  FboItemSetVertexEx(LCGETINTLGE(size_t, 2, 0, stTrisPerQuad, "TriIndex"), {
    LCGETNUM(GLfloat, 3, "V1Left"), LCGETNUM(GLfloat, 4, "V1Top"),
    LCGETNUM(GLfloat, 5, "V2Left"), LCGETNUM(GLfloat, 6, "V2Top"),
    LCGETNUM(GLfloat, 7, "V3Left"), LCGETNUM(GLfloat, 8, "V3Top")
  }));
/* ========================================================================= */
// $ Fbo:SetTCX
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > TC1Left:number=Coord #1 of vertex #1 of the specified triangle.
// > TC1Top:number=Coord #2 of vertex #2 of the specified triangle.
// > TC2Left:number=Coord #1 of vertex #3 of the specified triangle.
// > TC2Top:number=Coord #2 of vertex #1 of the specified triangle.
// > TC3Left:number=Coord #1 of vertex #2 of the specified triangle.
// > TC3Top:number=Coord #2 of vertex #3 of the specified triangle.
// ? Stores the specified texture co-ordinates for when the frame buffer object
// ? is drawn.
/* ------------------------------------------------------------------------- */
LLFUNC(SetTCX, LCGETULPTR(1, Fbo)->
  FboItemSetTexCoordEx(LCGETINTLGE(size_t, 2, 0, stTrisPerQuad, "TriIndex"), {
    LCGETNUM(GLfloat, 3, "TC1Left"), LCGETNUM(GLfloat, 4, "TC1Top"),
    LCGETNUM(GLfloat, 5, "TC2Left"), LCGETNUM(GLfloat, 6, "TC2Top"),
    LCGETNUM(GLfloat, 7, "TC3Left"), LCGETNUM(GLfloat, 8, "TC3Top")
  }));
/* ========================================================================= */
// $ Fbo:SetCX
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// > V1Red:number=Red component of vertex #1 of the specified triangle.
// > V1Green:number=Green component of vertex #1 of the specified triangle.
// > V1Blue:number=Blue component of vertex #1 of the specified triangle.
// > V1Alpha:number=Alpha component of vertex #1 of the specified triangle.
// > V2Red:number=Red component of vertex #2 of the specified triangle.
// > V2Green:number=Green component of vertex #2 of the specified triangle.
// > V2Blue:number=Blue component of vertex #2 of the specified triangle.
// > V2Alpha:number=Alpha component of vertex #2 of the specified triangle.
// > V3Red:number=Red component of vertex #3 of the specified triangle.
// > V3Green:number=Green component of vertex #3 of the specified triangle.
// > V3Blue:number=Blue component of vertex #3 of the specified triangle.
// > V3Alpha:number=Alpha component of vertex #3 of the specified triangle.
// ? Stores the specified colour intensities on each vertex for the
// ? Fbo:Blit() function.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCX, LCGETPTR(1, Fbo)->
  FboItemSetColourEx(LCGETINTLGE(size_t, 2, 0, stTrisPerQuad, "TriIndex"), {
    LCGETNUM(GLfloat, 3, "V1Red"), LCGETNUM(GLfloat, 4, "V1Green"),
    LCGETNUM(GLfloat, 5, "V1Blue"), LCGETNUM(GLfloat, 6, "V1Alpha"),
    LCGETNUM(GLfloat, 7, "V2Red"), LCGETNUM(GLfloat, 8, "V2Green"),
    LCGETNUM(GLfloat, 9, "V2Blue"), LCGETNUM(GLfloat, 10, "V2Alpha"),
    LCGETNUM(GLfloat, 11, "V3Red"), LCGETNUM(GLfloat, 12, "V3Green"),
    LCGETNUM(GLfloat, 13, "V3Blue"), LCGETNUM(GLfloat, 14, "V3Alpha")
  }));
/* ========================================================================= */
// $ Fbo:SetVLTRB
// > Left:number=The destination left co-ordinate.
// > Top:number=The destination top co-ordinate.
// > Right:number=The destination right co-ordinate.
// > Bottom:number=The destination bottom co-ordinate.
// ? Allows you to set basic vertex bounds when blitting the frame buffer
// ? object.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVLTRB, LCGETULPTR(1, Fbo)->FboItemSetVertex(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom")));
/* ========================================================================= */
// $ Fbo:SetVLTWH
// > Left:number=The destination left co-ordinate.
// > Top:number=The destination top co-ordinate.
// > Width:number=The destination width.
// > Height:number=The destination height.
// ? Allows you to set basic vertex co-ordinates and dimensions when blitting
// ? the frame buffer object.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVLTWH, LCGETULPTR(1, Fbo)->FboItemSetVertexWH(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Width"), LCGETNUM(GLfloat, 5, "Height")));
/* ========================================================================= */
// $ Fbo:SetVLTRBA
// > Left:number=The destination left co-ordinate.
// > Top:number=The destination top co-ordinate.
// > Right:number=The destination right co-ordinate.
// > Bottom:number=The destination bottom co-ordinate.
// > Angle:number=The angle of the vertex.
// ? Allows you to set basic vertex co-ordinates when blitting the fbo with
// ? angle calculations.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVLTRBA, LCGETULPTR(1, Fbo)->FboItemSetVertex(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom"),
  LCGETNORM(GLfloat, 6, "Angle")));
/* ========================================================================= */
// $ Fbo:SetVLTWHA
// > Left:number=The destination left co-ordinate.
// > Top:number=The destination top co-ordinate.
// > Width:number=The destination width.
// > Height:number=The destination height.
// > Angle:number=The angle of the vertex.
// ? Allows you to set basic vertex co-ordinates when blitting the fbo with
// ? angle calculations.
/* ------------------------------------------------------------------------- */
LLFUNC(SetVLTWHA, LCGETULPTR(1, Fbo)->FboItemSetVertexWH(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Width"), LCGETNUM(GLfloat, 5, "Height"),
  LCGETNORM(GLfloat, 6, "Angle")));
/* ========================================================================= */
// $ Fbo:SetOrtho
// > Left:number=The left co-ordinate.
// > Top:number=The top co-ordinate.
// > Right:number=The right co-ordinate.
// > Bottom:number=The bottom co-ordinate.
// ? Sets the ortho of the specified fbo, meaning, these are the bounds of
// ? where you can draw. Same as glOrtho().
// ? See: https://www.opengl.org/sdk/docs/man2/xhtml/glOrtho.xml
/* ------------------------------------------------------------------------- */
LLFUNC(SetOrtho, LCGETULPTR(1, Fbo)->FboSetOrtho(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom")));
/* ========================================================================= */
// $ Fbo:SetWireframe
// > Wireframe:Boolean=Use polygon mode GL_LINE (true) or GL_FILL (false).
// ? Sets drawing the contents in the fbo in wireframe more or texture filled
// ? mode (default).
/* ------------------------------------------------------------------------- */
LLFUNC(SetWireframe,
  LCGETULPTR(1, Fbo)->FboSetWireframe(LCGETBOOL(2, "Wireframe")));
/* ========================================================================= */
// $ Fbo:SetTCLTRB
// > Left:number=The destination left co-ordinate.
// > Top:number=The destination top co-ordinate.
// > Right:number=The destination right co-ordinate.
// > Bottom:number=The destination bottom co-ordinate.
// ? Stores the specified texture bounds used when blitting the frame buffer
// ? object.
/* ------------------------------------------------------------------------- */
LLFUNC(SetTCLTRB, LCGETULPTR(1, Fbo)->FboItemSetTexCoord(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Right"), LCGETNUM(GLfloat, 5, "Bottom")));
/* ========================================================================= */
// $ Fbo:SetTCLTWH
// > Left:number=The left destination co-ordinate.
// > Top:number=The top destination co-ordinate.
// > Width:number=The destination width.
// > Height:number=The destination height.
// ? Stores the specified texture co-ords and dimensions when blitting the
// ? frame buffer object.
/* ------------------------------------------------------------------------- */
LLFUNC(SetTCLTWH, LCGETULPTR(1, Fbo)->FboItemSetTexCoordWH(
  LCGETNUM(GLfloat, 2, "Left"),  LCGETNUM(GLfloat, 3, "Top"),
  LCGETNUM(GLfloat, 4, "Width"), LCGETNUM(GLfloat, 5, "Height")));
/* ========================================================================= */
// $ Fbo:SetClear
// > State:bool=New clear state
// ? Sets if the fbo should be cleared before rendering to it
/* ------------------------------------------------------------------------- */
LLFUNC(SetClear, LCGETPTR(1, Fbo)->FboSetClear(LCGETBOOL(2, "Clear")));
/* ========================================================================= */
// $ Fbo:SetClearColour
// > Red:number=The red component intensity (0 to 1).
// > Green:number=The green component intensity (0 to 1).
// > Blue:number=The blue component intensity (0 to 1).
// > Alpha:number=The alpha component intensity (0 to 1).
// ? Same as glClearColour(). Allows you to set the colour for glClear(),
// ? meaning the fbo is cleared with this colour on each new frame loop.
// ? See: https://www.opengl.org/sdk/docs/man2/xhtml/glClear.xml.
// ? See: https://www.opengl.org/sdk/docs/man2/xhtml/glClearColor.xml.
/* ------------------------------------------------------------------------- */
LLFUNC(SetClearColour, LCGETPTR(1, Fbo)->FboSetClearColour(
  LCGETNUM(GLfloat, 2, "Red"),  LCGETNUM(GLfloat, 3, "Green"),
  LCGETNUM(GLfloat, 4, "Blue"), LCGETNUM(GLfloat, 5, "Alpha")));
/* ========================================================================= */
// $ Fbo:SetCRGBA
// > Red:number=The entire fbo texture red colour intensity (0 to 1).
// > Green:number=The entire fbo texture green colour intensity (0 to 1).
// > Blue:number=The entire fbo texture blue colour intensity (0 to 1).
// > Alpha:number=The entire fbo texture alpha colour intensity (0 to 1).
// ? Sets the colour intensity of all the vertexes for the entire fbo texture.
/* ------------------------------------------------------------------------- */
LLFUNC(SetCRGBA, LCGETPTR(1, Fbo)->FboItemSetQuadRGBA(
  LCGETNUM(GLfloat, 2, "Red"),  LCGETNUM(GLfloat, 3, "Green"),
  LCGETNUM(GLfloat, 4, "Blue"), LCGETNUM(GLfloat, 5, "Alpha")));
/* ========================================================================= */
// $ Fbo:SetFilter
// > Id:integer=The texture filter id.
// ? Sets the texture filtering id for the fbo texture. Mipmaps are
// ? automatically generated if a mipmapping id is selected.
// ? (Magnification / Minification). See 'Texture.Filters()' for more info.
/* ------------------------------------------------------------------------- */
LLFUNC(SetFilter, LCGETPTR(1, Fbo)->FboSetFilterCommit(
  LCGETINTLGE(OglFilterEnum, 2, OF_N_N, OF_NM_MAX, "Filter")));
/* ========================================================================= */
// $ Fbo:Activate
// ? Makes the specified fbo the active fbo and all subsequent Texture:* calls
// ? will apply to this fbo. This can be called on the main fbo.
/* ------------------------------------------------------------------------- */
LLFUNC(Activate, LCGETPTR(1, Fbo)->FboSetActive());
/* ========================================================================= */
// $ Fbo:Finish
// ? Queues the fbo for redrawing after LUA has finished it's tick function.
// ? You only need to run this once, subsequent calls in the same frame will
// ? be ignored as the fbo has already been queued for redrawing.
/* ------------------------------------------------------------------------- */
LLFUNC(Finish, LCGETPTR(1, Fbo)->FboFinishAndRender());
/* ========================================================================= */
// $ Fbo:Blit
// ? Blits the SPECIFIED fbo to the currently ACTIVE fbo. This just adds
// ? to the active FBO drawing arrays and doesn't actually render until the
// ? active fbo is finished and rendered. The currently stored vertex,
// ? texcoord, colour and ortho values are used.
/* ------------------------------------------------------------------------- */
LLFUNC(Blit, FboActive()->FboBlit(*LCGETPTR(1, Fbo)));
/* ========================================================================= */
// $ Fbo:BlitT
// > TriIndex:integer=Triangle index. Triangle #1 (zero) or triangle #2 (one).
// ? Blits the SPECIFIED triangle of the SPECIFIED fbo to the currently
// ? ACTIVE fbo. This just adds to the active FBO drawing arrays and doesn't
// ? actually render until the active fbo is finished and rendered. The
// ? currently stored vertex, texcoord, colour and ortho values are used.
/* ------------------------------------------------------------------------- */
LLFUNC(BlitT, FboActive()->FboBlitTri(*LCGETPTR(1, Fbo),
  LCGETINTLGE(size_t, 2, 0, stTrisPerQuad, "TriIndex")));
/* ========================================================================= */
// $ Fbo:GetLFloatCount
// < Count:integer=Number of floats in display lists.
// ? Returns the number of floating point numbers entered into the display list
// ? on the last rendered frame.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetLFloatCount, 1, LCPUSHVAR(LCGETPTR(1, Fbo)->FboGetTris()));
/* ========================================================================= */
// $ Fbo:GetFloatCount
// < Count:integer=Number of floats in display lists.
// ? Returns the number of floating point numbers currently entered into the
// ? display list.
/* ------------------------------------------------------------------------- */
LLFUNCEX(GetFloatCount, 1, LCPUSHVAR(LCGETPTR(1, Fbo)->FboGetTrisNow()));
/* ========================================================================= */
// $ Fbo:IsFinished
// < State:boolean=Is the fbo finished
// ? Returns if the fbo has been finished.
/* ------------------------------------------------------------------------- */
LLFUNCEX(IsFinished, 1, LCPUSHVAR(!!LCGETPTR(1, Fbo)->FboGetFinishCount()));
/* ========================================================================= */
// $ Fbo:GetMatrix
// < Width:number=Total count of horizontal viewable pixels.
// < Height:number=Total count of vertical viewable pixels.
// < Left:number=Minimum viewable matrix X position.
// < Top:number=Minimum viewable matrix Y position.
// < Right:number=Maximum viewable matrix X position.
// < Bottom:number=Maximum viewable matrix Y position.
// < OrthoWidth:number=Current requested ortho width.
// < OrthoHeight:number=Current requested ortho height.
// ? Returns the current matrix information for the specified fbo.
/* ------------------------------------------------------------------------- */
LLFUNCBEGIN(GetMatrix)
  const Fbo &fboCref = *LCGETPTR(1, Fbo);
  LCPUSHVAR(fboCref.GetCoRight(),         fboCref.GetCoBottom(),
            fboCref.fcStage.GetCoLeft(),  fboCref.fcStage.GetCoTop(),
            fboCref.fcStage.GetCoRight(), fboCref.fcStage.GetCoBottom());
LLFUNCENDEX(6)
/* ========================================================================= */
// $ Fbo:Dump
// < Dest:Fbo=The frame buffer to dump.
// < File:string=(Optional) File to save to.
// ? Takes a screenshot of the specified FBO. You must completely omit the
// ? 'file' parameter to use an engine generated file.
/* ------------------------------------------------------------------------- */
LLFUNC(Dump, cSShot->DumpFBO(*LCGETPTR(1, Fbo),
  LuaUtilStackSize(lS) < 2 ? cCommon->Blank() : LCGETCPPFILE(2, "File")));
/* ========================================================================= */
// $ Fbo:Reserve
// > Vertexes:integer=How many 64-bit floats to reserve in GPU float list
// > Commands:integer=How many structs to reserve in GPU commands list
// ? Reserves memory the respective number of items. Use this if the first
// ? complex frame you draw is slow. It's probably because of this as the
// ? default reservation level for the lists is 1024 x 64-bit float/command.
/* ------------------------------------------------------------------------- */
LLFUNC(Reserve, LCGETPTR(1, Fbo)->FboReserve(
  LCGETINTLG(size_t, 2, 1, 1000000, "Vertexes"),
  LCGETINTLG(size_t, 3, 1, 1000000, "Commands")));
/* ========================================================================= */
// $ Fbo:Destroy
// ? Destroys the FBO and frees all the memory associated with it. The OpenGL
// ? handles and VRAM allocated by it are freed after the main FBO has has
// ? been rendered. The object will no longer be useable after this call and an
// ? error will be generated if accessed.
/* ------------------------------------------------------------------------- */
LLFUNC(Destroy, LCCLASSDESTROY(1, Fbo));
/* ========================================================================= **
** ######################################################################### **
** ## Fbo:* member functions structure                                    ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSMFBEGIN                            // Fbo:* member functions begin
  LLRSFUNC(Activate),      LLRSFUNC(Blit),           LLRSFUNC(BlitT),
  LLRSFUNC(Destroy),       LLRSFUNC(Dump),           LLRSFUNC(Finish),
  LLRSFUNC(GetFloatCount), LLRSFUNC(GetLFloatCount), LLRSFUNC(GetMatrix),
  LLRSFUNC(IsFinished),    LLRSFUNC(Reserve),        LLRSFUNC(SetBlend),
  LLRSFUNC(SetClear),      LLRSFUNC(SetClearColour), LLRSFUNC(SetCRGBA),
  LLRSFUNC(SetCX),         LLRSFUNC(SetFilter),      LLRSFUNC(SetOrtho),
  LLRSFUNC(SetTCLTRB),     LLRSFUNC(SetTCLTWH),      LLRSFUNC(SetTCX),
  LLRSFUNC(SetVLTRB),      LLRSFUNC(SetVLTWH),       LLRSFUNC(SetVLTRBA),
  LLRSFUNC(SetVLTWHA),     LLRSFUNC(SetVX),          LLRSFUNC(SetWireframe),
LLRSEND                                // Fbo:* member functions end
/* ========================================================================= */
// $ Fbo.Main
// < Handle:Fbo=A handle to the main fbo object.
// ? Returns a handle to the main FBO so you can draw to it. Changing the
// ? properties of the main FBO may result in undefined behaviour. Note that
// ? the engine console shares the same FBO so you may want to create a
// ? separatte FBO to draw to.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Main, 1, LCCLASSCREATEPTR(Fbo, &cFboCore->fboMain));
/* ========================================================================= */
// $ Fbo.Create
// > Identifier:string=Reference only user-defined identifier.
// > Width:integer=Width of the FBO.
// > Height:integer=Height of the FBO.
// < Handle:Fbo=A handle to the newly created main fbo object.
// ? Creates a new FBO of the specified size.
/* ------------------------------------------------------------------------- */
LLFUNCEX(Create, 1, LCCLASSCREATE(Fbo)->FboInit(
  LCGETCPPSTRINGNE(         1,                                 "Identifier"),
  LCGETINTLG      (GLsizei, 2, 1, cOgl->MaxTexSize<GLsizei>(), "Width"),
  LCGETINTLG      (GLsizei, 3, 1, cOgl->MaxTexSize<GLsizei>(), "Height")));
/* ========================================================================= */
// $ Fbo.Draw
// ? When the UI mode is set to 3 which is fully manual mode, you need to call
// ? this function for every frame that changes. The main screen FBO will be
// ? flushed and not finished and OpenGL will NOT swap buffers if you do not
// ? call this function.
/* ------------------------------------------------------------------------- */
LLFUNC(Draw, cFboCore->SetDraw());
/* ========================================================================= */
// $ Fbo.IsDrawing
// ? < State:boolean=Is the main fbo set to redraw?
// ? Returns if the main fbo is set to redraw.
/* ------------------------------------------------------------------------- */
LLFUNCEX(IsDrawing, 1, LCPUSHVAR(cFboCore->CanDraw()));
/* ========================================================================= */
// $ Fbo.OnRedraw
// > Func:function=The main redraw function to change to
// ? When OpenGL needs to be reinitialised, the specified callback will be
// ? called so the guest can redraw FBO's and maybe do other stuff. All
// ? texture and fbo data is already preserved so there is no need to
// ? reinitialise any of that, you just need to redraw them.
/* ------------------------------------------------------------------------- */
LLFUNC(OnRedraw, LCSETEVENTCB(cLua->lrMainRedraw));
/* ========================================================================= */
// $ Fbo.ConSet
// > State:boolean=The console state.
// < Success:boolean=Returns if the requested function was successful.
// ? Shows or hides the console. This does not modify the cvar state. This
// ? function is in Fbo because it only applies to opengl mode. It also
// ? overrides the 'con_disabled' setting.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ConSet, 1,
  LCPUSHVAR(cConGraphics->DoSetVisible(LCGETBOOL(1, "State"))));
/* ========================================================================= */
// $ Fbo.ConHeight
// > State:number=The console height.
// ? Sets the console height normal without modifying the cvar. This function
// ? is in Fbo because it only applies to opengl mode.
/* ------------------------------------------------------------------------- */
LLFUNC(ConHeight,
  cConGraphics->SetHeight(LCGETNUMLG(GLfloat, 1, 0.0f, 1.0f, "Height")));
/* ========================================================================= */
// $ Fbo.ConEnabled
// < State:boolean=The console state.
// ? Returns true if the console is showing, false if it is not. This function
// ? is in Fbo because it only applies to opengl mode.
/* ------------------------------------------------------------------------- */
LLFUNCEX(ConEnabled, 1, LCPUSHVAR(cConsole->IsVisible()));
/* ========================================================================= */
// $ Fbo.ConLock
// < State:boolean=The console visibility lock state.
// ? Lock the visibility of the console on or off. This function is in Fbo
// ? because it only applies to opengl mode.
/* ------------------------------------------------------------------------- */
LLFUNC(ConLock, cConGraphics->SetCantDisable(LCGETBOOL(1, "State")));
/* ========================================================================= **
** ######################################################################### **
** ## Fbo.* namespace functions structure                                 ## **
** ######################################################################### **
** ------------------------------------------------------------------------- */
LLRSBEGIN                              // Fbo.* namespace functions begin
  LLRSFUNC(ConEnabled), LLRSFUNC(ConHeight), LLRSFUNC(ConLock),
  LLRSFUNC(ConSet),     LLRSFUNC(Create),    LLRSFUNC(Draw),
  LLRSFUNC(IsDrawing),  LLRSFUNC(Main),      LLRSFUNC(OnRedraw),
LLRSEND                                // Fbo.* namespace functions end
/* ========================================================================= **
** ######################################################################### **
** ## Fbo.* namespace constants                                           ## **
** ######################################################################### **
** ========================================================================= */
// @ Fbo.Blends
// < Codes:table=The table of key/value pairs of available flags
// ? Returns the texture filters available. Returned as key/value pairs. The
// ? value is a unique identifier to the flag as below.
// ? Indx  Keyname   Actual GL value              Default Parameter?
// ? [00]  Z         GL_ZERO                      -
// ? [01]  O         GL_ONE                       srcAlpha
// ? [02]  S_C       GL_SRC_COLOR                 -
// ? [03]  O_M_S_C   GL_ONE_MINUS_SRC_COLOR       -
// ? [04]  D_C       GL_DST_COLOR                 -
// ? [05]  O_M_D_C   GL_ONE_MINUS_DST_COLOR       -
// ? [06]  S_A       GL_SRC_ALPHA                 srcRGB
// ? [07]  O_M_S_A   GL_ONE_MINUS_SRC_ALPHA       dstRGB, dstAlpha
// ? [08]  D_A       GL_DST_ALPHA                 -
// ? [09]  O_M_D_A   GL_ONE_MINUS_DST_ALPHA       -
// ? [10]  C_C       GL_CONSTANT_COLOR            -
// ? [11]  O_M_C_C   GL_ONE_MINUS_CONSTANT_COLOR  -
// ? [12]  C_A       GL_CONSTANT_ALPHA            -
// ? [13]  O_M_C_A   GL_ONE_MINUS_CONSTANT_ALPHA  -
// ? [14]  S_A_S     GL_SRC_ALPHA_SATURATE        -
// ? [15]  S1_C      GL_SRC1_COLOR                -
// ? [16]  O_M_S1_C  GL_ONE_MINUS_SRC1_COLOR      -
// ? [17]  S1_A      GL_SRC1_ALPHA                -
// ? [18]  O_M_S1_A  GL_ONE_MINUS_SRC1_ALPHA      -
// ? See https://www.khronos.org
// ?   /registry/OpenGL-Refpages/gl4/html/glBlendFuncSeparate.xhtml
// ? for more information about the formulas.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Blends)                    // Beginning of blending types
  LLRSKTITEM(OB_,Z),                   LLRSKTITEM(OB_,O),
  LLRSKTITEM(OB_,S_C),                 LLRSKTITEM(OB_,O_M_S_C),
  LLRSKTITEM(OB_,D_C),                 LLRSKTITEM(OB_,O_M_D_C),
  LLRSKTITEM(OB_,S_A),                 LLRSKTITEM(OB_,O_M_S_A),
  LLRSKTITEM(OB_,D_A),                 LLRSKTITEM(OB_,O_M_D_A),
  LLRSKTITEM(OB_,C_C),                 LLRSKTITEM(OB_,C_A),
  LLRSKTITEM(OB_,S_A_S),
LLRSKTEND                              // End of blending types
/* ========================================================================= */
// @ Fbo.Filters
// < Codes:table=The table of key/value pairs of available flags
// ? Returns the texture filters available. Returned as key/value pairs. The
// ? value is a unique identifier to the flag as below.
// ? # ID  KEYNAME   MAGNIFICATION  MINIFICATION
// ? # ==  ========  =============  =========================
// ? # 00  N_N       GL_NEAREST     GL_NEAREST
// ? # 01  N_L       GL_NEAREST     GL_LINEAR
// ? # 02  L_N       GL_LINEAR      GL_NEAREST
// ? # 03  L_L       GL_LINEAR      GL_LINEAR
// ? # 04  N_N_MM_N  GL_NEAREST     GL_NEAREST_MIPMAP_NEAREST
// ? # 05  L_N_MM_N  GL_LINEAR      GL_NEAREST_MIPMAP_NEAREST
// ? # 06  N_N_MM_L  GL_NEAREST     GL_NEAREST_MIPMAP_LINEAR
// ? # 07  L_N_MM_L  GL_LINEAR      GL_NEAREST_MIPMAP_LINEAR
// ? # 08  N_L_MM_N  GL_NEAREST     GL_LINEAR_MIPMAP_NEAREST
// ? # 09  L_L_MM_N  GL_LINEAR      GL_LINEAR_MIPMAP_NEAREST
// ? # 10  N_L_MM_L  GL_NEAREST     GL_LINEAR_MIPMAP_LINEAR
// ? # 11  L_L_MM_L  GL_LINEAR      GL_LINEAR_MIPMAP_LINEAR
// ? # ==  ========  =============  =========================
// ? Explanations for magnification and minification...
// ? # GL_NEAREST
// ? Returns the value of the texture element that is nearest (in Manhattan
// ? distance) to the center of the pixel being textured.
// ? # GL_LINEAR
// ? Returns the weighted average of the four texture elements that are closest
// ? to the center of the pixel being textured.
// ? # GL_NEAREST_MIPMAP_NEAREST
// ? Chooses the mipmap that most closely matches the size of the pixel being
// ? textured and uses the GL_NEAREST criterion (the texture element nearest to
// ? the center of the pixel) to produce a texture value.
// ? # GL_LINEAR_MIPMAP_NEAREST
// ? Chooses the mipmap that most closely matches the size of the pixel being
// ? textured and uses the GL_LINEAR criterion (a weighted average of the four
// ? texture elements that are closest to the center of the pixel) to produce a
// ? texture value.
// ? # GL_NEAREST_MIPMAP_LINEAR
// ? Chooses the two mipmaps that most closely match the size of the pixel
// ? being textured and uses the GL_NEAREST criterion (the texture element
// ? nearest to the center of the pixel) to produce a texture value from each
// ? mipmap. The final texture value is a weighted average of those two values.
// ? # GL_LINEAR_MIPMAP_LINEAR
// ? Chooses the two mipmaps that most closely match the size of the pixel
// ? being textured and uses the GL_LINEAR criterion (a weighted average of the
// ? four texture elements that are closest to the center of the pixel) to
// ? produce a texture value from each mipmap. The final texture value is a
// ? weighted average of those two values.
/* ------------------------------------------------------------------------- */
LLRSKTBEGIN(Filters)                   // Beginning of filter types
  LLRSKTITEM(OF_,N_N),                 LLRSKTITEM(OF_,N_L),
  LLRSKTITEM(OF_,L_N),                 LLRSKTITEM(OF_,L_L),
  LLRSKTITEM(OF_,N_N_MM_N),            LLRSKTITEM(OF_,L_N_MM_N),
  LLRSKTITEM(OF_,N_N_MM_L),            LLRSKTITEM(OF_,L_N_MM_L),
  LLRSKTITEM(OF_,N_L_MM_N),            LLRSKTITEM(OF_,L_L_MM_N),
  LLRSKTITEM(OF_,N_L_MM_L),            LLRSKTITEM(OF_,L_L_MM_L),
LLRSKTEND                              // End of filter types
/* ========================================================================= **
** ######################################################################### **
** ## Fbo.* namespace constants structure                                 ## **
** ######################################################################### **
** ========================================================================= */
LLRSCONSTBEGIN                         // Fbo.* namespace consts begin
  LLRSCONST(Blends), LLRSCONST(Filters),
LLRSCONSTEND                           // Fbo.* namespace consts end
/* ========================================================================= */
}                                      // End of Fbo namespace
/* == EoF =========================================================== EoF == */
